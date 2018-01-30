#include "MidiFile.h"
#include "Options.h"
#include <ctype.h>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <algorithm> // remove and remove_if
#include "intelhexclass.h"

using namespace std;

typedef unsigned char uchar;

// user interface variables
Options options;
bool globalDebugFlag=false;

// function declarations:
void convertMidiFileToNoteMap(MidiFile &midifile, map<int, vector<int>> &noteMap);
void convertNoteMaptoRom(map<int, vector<int>> &noteMap, vector<char> &mem);
void convertMemToHexFile(vector<char> &mem, string originalHexFilePath, string targetHexFilePath);
void reMapNoteMap(map<int, vector<int>> &noteMap);
void analyzeNoteMap(map<int, vector<int>> &noteMap);
int reMapMidiNote(int midiNote);
void checkOptions(Options &opts, int argc, char **argv);
void example(void);
void usage(const char *command);
extern "C" {
extern int micronucleus_main(int argc, char **argv);
}

//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{

      map<int, vector<int>> noteMap;
      checkOptions(options, argc, argv);
      MidiFile midifile(options.getArg(1));
      convertMidiFileToNoteMap(midifile, noteMap);
      vector<char> mem;
      analyzeNoteMap(noteMap);
      reMapNoteMap(noteMap);
      convertNoteMaptoRom(noteMap, mem);
      convertMemToHexFile(mem, "./hex-file/mg.hex", "target.hex");

      char argString[][32] = {"micronucleus_mainaaa", "-r", "--fast-mode", "target.hex"};
      char *micronucleus_argv[4];
      for (int i = 0; i < 4; i++)
            micronucleus_argv[i] = argString[i];
      return micronucleus_main(4, micronucleus_argv);
}
void analyzeNoteMap(map<int, vector<int>> &noteMap)
{
      map<int,int> noteNumMap;
      for (auto &noteMapItem : noteMap)
      {
            for(auto &note : noteMapItem.second)
            {
                  ++noteNumMap[note];
            }
      }

      for(auto &noteNumMapItem:noteNumMap)
      {
            cout<<"Note: "<<noteNumMapItem.first<<" Times: "<<noteNumMapItem.second<<endl;
      }

      int highestPitch=noteNumMap.begin()->first;
      int lowestPitch=(--noteNumMap.end())->first;
      cout<<"Highest pitch: "<<highestPitch<<" Lowest pitch: "<<lowestPitch<<endl;
}
void reMapNoteMap(map<int, vector<int>> &noteMap)
{
	auto noteMapItem = noteMap.begin();
	for (; noteMapItem != std::prev(noteMap.end()) /* not hoisted */; /* no increment */)
	{
		cout << "Tick: " << noteMapItem->first << " Key: ";
        auto &noteItems = noteMapItem->second;
        sort(noteItems.begin(),noteItems.end());
        for (auto &note : noteItems)
        {
            int reMapNote = reMapMidiNote(note);
            note = reMapNote;
            cout << reMapNote << " ";
        }
        cout << endl;
        // removes all elements with the value -1
        noteItems.erase(std::remove(noteItems.begin(), noteItems.end(), -1), noteItems.end());
		
		if (noteItems.empty())
		{
			noteMap.erase(noteMapItem++);    // or "it = m.erase(it)" since C++11
		}
		else
		{
			++noteMapItem;
		}
	}
	  
      cout << "Tick: " << noteMapItem->first << " EOS" << endl;
}

int reMapMidiNote(int midiNote)
{
      int reMapNote = midiNote - 45;
      if (reMapNote >= 0 && reMapNote <= 55)
            return reMapNote;
      else
            return -1;
}

void convertMemToHexFile(vector<char> &mem, string originalHexFilePath, string targetHexFilePath)
{
      // Create an input stream
      ifstream intelHexInput;
      // Create an output stream
      ofstream intelHexOutput;

      // Create a variable for the intel hex data
      intelhex ihMusicBoxFirmRom;
      intelHexInput.open(originalHexFilePath, ifstream::in);

      if (!intelHexInput.good())
      {
            std::cerr << "Error: couldn't open " << originalHexFilePath << std::endl;
      }
      /* Decode file                                                            */
      intelHexInput >> ihMusicBoxFirmRom;
      unsigned long addr = 1024 * 2;
      for (auto &byte : mem)
      {
            ihMusicBoxFirmRom.overwriteData(byte, addr++);
      }

      intelHexOutput.open(targetHexFilePath, ofstream::out);

      if (!intelHexOutput.good())
      {
            std::cerr << "Error: couldn't open " << targetHexFilePath << std::endl;
      }

      intelHexOutput << ihMusicBoxFirmRom;
}

void convertNoteMaptoRom(map<int, vector<int>> &noteMap, vector<char> &mem)
{
      auto noteMapItem = noteMap.begin();
      for (; noteMapItem != std::prev(noteMap.end()); ++noteMapItem)
      {
            mem.push_back(static_cast<char>(noteMapItem->first));
            mem.push_back(static_cast<char>(noteMapItem->first >> 8));

            auto noteItem = noteMapItem->second.begin();
            for (; noteItem != noteMapItem->second.end(); ++noteItem)
                  mem.push_back(*noteItem);
            mem.back() |= 128;
      }

      mem.push_back(static_cast<char>(noteMapItem->first));
      mem.push_back(static_cast<char>(noteMapItem->first >> 8));
      mem.push_back(0xFF);

      cout << "Mem size: " << mem.size() << "b" << endl;
}

void convertMidiFileToNoteMap(MidiFile &midifile, map<int, vector<int>> &noteMap)
{
      midifile.absoluteTicks();
      midifile.joinTracks();

      int key = 0;

      for (int i = 0; i < midifile.getNumEvents(0); i++)
      {
            int command = midifile[0][i][0] & 0xf0;
            if (command == 0x90 && midifile[0][i][2] != 0)
            {
                  key = midifile[0][i][1];
                  int t = static_cast<int>(midifile.getTimeInSeconds(midifile[0][i].tick) * 120.0);
                  noteMap[t].push_back(key);
            }
      }

      for (int i = midifile.getNumEvents(0) - 1; i >= 0; --i)
      {
            int command = midifile[0][i][0] & 0xf0;
            if ((command == 0x90 && midifile[0][i][2] == 0) || command == 0x80)
            {
                  key = midifile[0][i][1];
                  int t = static_cast<int>(midifile.getTimeInSeconds(midifile[0][i].tick) * 120.0);
                  noteMap[t].push_back(key);
                  break;
            }
      }
}

void checkOptions(Options &opts, int argc, char *argv[])
{
      opts.define("author=b", "author of program");
      opts.define("version=b", "compilation info");
      opts.define("example=b", "example usages");
      opts.define("h|help=b", "short description");

      opts.define("d|debug=b", "debug mode to find errors in input file");

      opts.process(argc, argv);

      // handle basic options:
      if (opts.getBoolean("author"))
      {
            cout << "Written by Yuan." << endl;
            exit(0);
      }
      else if (opts.getBoolean("version"))
      {
            cout << argv[0] << ", version: 1.0" << endl;
            cout << "compiled: " << __DATE__ << endl;
            exit(0);
      }
      else if (opts.getBoolean("help"))
      {
            usage(opts.getCommand().data());
            exit(0);
      }
      else if (opts.getBoolean("example"))
      {
            example();
            exit(0);
      }
      else if (opts.getBoolean("debug"))
      {
            globalDebugFlag=true;
      }

      if (opts.getArgCount() != 1)
      {
            usage(opts.getCommand().data());
            exit(1);
      }
}

//////////////////////////////
//
// example --
//

void example(void)
{
}

//////////////////////////////
//
// usage --
//

void usage(const char *command)
{
      cout << "Usage: " << command << " midifile" << endl;
}
