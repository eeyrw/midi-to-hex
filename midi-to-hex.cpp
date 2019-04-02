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
#include "bprinter/table_printer.h"
#include "NoteListProcessor.h"

using namespace std;
using bprinter::TablePrinter;
using noteListProcessor::NoteListProcessor;

typedef unsigned char uchar;

// user interface variables
Options options;

// function declarations:
void convertNoteMaptoRom(map<int, vector<int>> &noteMap, vector<char> &mem);
void convertMemToHexFile(vector<char> &mem, string originalHexFilePath, string targetHexFilePath);
void convertMemToSourceFile(vector<char> &mem, string targetSourceFilePath);
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
      vector<char> mem;
      NoteListProcessor np = NoteListProcessor(options.getString("midi"));
      if (options.getBoolean("transpose"))
      {
            int t = options.getInteger("transpose");
            np.setExternTranspose(t);
      }
      np.analyzeNoteMap();
      np.transposeTickNoteMap();
      np.generateDeltaBin(mem);
      string defaultHexFile = "./hex-file/mg.hex";
      if (options.getBoolean("device"))
      {
            string dev = options.getString("device");
            if (dev == "t167")
            {
                  defaultHexFile = "./hex-file/mg_167.hex";
                  cout << "Generate hex file for ATTINY 167." << endl;
            }else if (dev == "m328p")
			{
                  defaultHexFile = "./hex-file/mg_m328p.hex";
                  cout << "Generate hex file for Atmega328p." << endl;				
			}
      }
      convertMemToHexFile(mem, defaultHexFile, "target.hex");
      convertMemToSourceFile(mem,"score.c");

      if (options.getBoolean("download"))
      {
            cout << "Download target.hex to mcu through micronucleus bootloader." << endl;
            cout << "Please ensure the mcu device is under bootloader mode (usally by re-pluging usb or reseting mcu)." << endl;
            char argString[][32] = {"micronucleus_main", "-r", "--fast-mode", "target.hex"};
            char *micronucleus_argv[4];
            for (int i = 0; i < 4; i++)
                  micronucleus_argv[i] = argString[i];
            return micronucleus_main(4, micronucleus_argv);
      }

      return 0;
}

void convertMemToSourceFile(vector<char> &mem, string targetSourceFilePath)
{
      FILE* targetSourceFile;
      targetSourceFile=fopen(targetSourceFilePath.c_str(),"w");
      fprintf(targetSourceFile,"const unsigned char Score[]={\n");
      fprintf(targetSourceFile,"const unsigned char Score[]={\n");
      int lineCounter=0;
      for(auto b:mem)
      {
            fprintf(targetSourceFile,"0x%02x,",(unsigned char)b);
            if(lineCounter>16)
            {
                  fprintf(targetSourceFile,"\n");
                  lineCounter=0;
            }
            lineCounter++;
            
      }
      fprintf(targetSourceFile,"};\n");
      fclose(targetSourceFile);
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
      ihMusicBoxFirmRom.end();
      unsigned long addr = ihMusicBoxFirmRom.currentAddress()+1;
      cout<<"The score data located at: 0x"<<setfill('0') << uppercase << hex << addr <<endl;
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

void checkOptions(Options &opts, int argc, char *argv[])
{
      opts.define("author=b", "author of program");
      opts.define("version=b", "compilation info");
      opts.define("example=b", "example usages");
      opts.define("h|help=b", "short description");
      opts.define("t|transpose=i", "Specify the transpose (half note). The suggestion transpose will be applied if without specified transpose.");
      opts.define("d|download=b", "Download the hex file to mcu through micronucleus directly.");
      opts.define("m|midi=s", "Midi file path.");
      opts.define("device=s", "Target mcu.");

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
      // if (opts.getArgCount() != 1)
      // {
      //       usage(opts.getCommand().data());
      //       exit(1);
      // }
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
