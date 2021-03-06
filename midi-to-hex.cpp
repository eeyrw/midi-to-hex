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
#include "ByteStream.h"
#include "fmt/core.h"

using namespace std;
using namespace smf;
using bprinter::TablePrinter;
using noteListProcessor::NoteListProcessor;

typedef unsigned char uchar;

// user interface variables
Options options;

// function declarations:
void convertNoteMaptoRom(map<int, vector<int>> &noteMap, vector<char> &mem);
void convertMemToHexFile(vector<char> &mem, string originalHexFilePath, string targetHexFilePath);
void convertMemToSourceFile(vector<char> &mem, string targetSourceFilePath);
void generateScoreListMemAndScore(string midiFileListPath);
void checkOptions(Options &opts, int argc, char **argv);
void example(void);
void usage(const char *command);
extern "C"
{
      extern int micronucleus_main(int argc, char **argv);
}

//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{

      map<int, vector<int>> noteMap;

      checkOptions(options, argc, argv);
      vector<char> mem;

      if (options.getBoolean("midi"))
      {
            NoteListProcessor np = NoteListProcessor(options.getString("midi"));

            if (options.getBoolean("lower"))
                  np.recommLowestPitch = options.getInteger("lower");
            if (options.getBoolean("upper"))
                  np.recommHighestPitch = options.getInteger("upper");

            if (options.getBoolean("transpose"))
            {
                  int t = options.getInteger("transpose");
                  np.setExternTranspose(t);
            }
            np.analyzeNoteMapByCentroid();
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
                  }
                  else if (dev == "m328p")
                  {
                        defaultHexFile = "./hex-file/mg_m328p.hex";
                        cout << "Generate hex file for Atmega328p." << endl;
                  }
            }
            convertMemToHexFile(mem, defaultHexFile, "target.hex");
            convertMemToSourceFile(mem, "score.c");
      }

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

      if (options.getBoolean("scorelist"))
      {
            string path = options.getString("scorelist");
            generateScoreListMemAndScore(path);
      }

      return 0;
}

void generateScoreListMemAndScore(string midiFileListPath)
{
      std::ifstream stream(midiFileListPath);
      std::string path;
      vector<char> scoreMem;
      vector<string> pathList;
      fmt::print("Start Generate Score Data list");

      while (std::getline(stream, path))
      {
            if (path[0] != '#')
            {
                  ifstream f(path.c_str());
                  if (f.good())
                        pathList.push_back(path);
                  else
                        cout << "Cannot find midi file:" << path;
            }
      }

      ByteStream scoreListMem = ByteStream(128 * 1024);

      string identifer = "SCRE";
      scoreListMem.writeBytes(identifer.c_str(), 4);
      scoreListMem.writeUInt32(pathList.size());
      int scoreMemPointer = scoreListMem.size() + sizeof(uint32_t) * pathList.size();
      int headerOffest;
      double totalDuration = 0;

      std::ofstream logFileStream;
      logFileStream.open("ScoreListGen.log");

      for (auto midiFilePath : pathList)
      {
            vector<char> mem;
            NoteListProcessor np = NoteListProcessor(midiFilePath, &logFileStream);
            if (options.getBoolean("lower"))
                  np.recommLowestPitch = options.getInteger("lower");
            if (options.getBoolean("upper"))
                  np.recommHighestPitch = options.getInteger("upper");

            logFileStream << "File: " << midiFilePath << '\n';
            np.analyzeNoteMapByCentroid();
            np.transposeTickNoteMap();
            np.generateDeltaBin(mem);
            scoreListMem.writeUInt32(scoreMemPointer);
            scoreMemPointer += mem.size();
            std::move(mem.begin(), mem.end(), std::back_inserter(scoreMem));
            totalDuration += np.midiDuration;
            logFileStream << "\n\n";
      }
      headerOffest = scoreListMem.size();

      scoreListMem.writeBytes(scoreMem.data(), scoreMem.size());

      vector<char> scoreListMemVector(scoreListMem.size());
      scoreListMem.readBytes(scoreListMemVector.data(), scoreListMem.size(), 0);
      convertMemToSourceFile(scoreListMemVector, "scoreList.c");
      std::ofstream ofile("scoreList.raw", std::ios::binary);
      ofile.write(scoreListMemVector.data(), scoreListMem.size());

      logFileStream << "\n\n\n=============================================\n";
      logFileStream << "Score Count: " << pathList.size() << '\n';
      logFileStream << "Total Mem Size (byte): " << scoreListMem.size() + headerOffest << '\n';
      logFileStream << "Score Data Mem Size (byte): " << scoreListMem.size() << '\n';
      int dur = static_cast<int>(totalDuration);
      logFileStream << "Total Duration: " << dur / 3600 << "h "
                    << dur / 60 % 60 << "m "
                    << dur % 60 << "s " << '\n';
      logFileStream.close();
}

void convertMemToSourceFile(vector<char> &mem, string targetSourceFilePath)
{
      FILE *targetSourceFile;
      targetSourceFile = fopen(targetSourceFilePath.c_str(), "w");
      fprintf(targetSourceFile, "#ifdef __AVR_ARCH__\n");
      fprintf(targetSourceFile, "#include <avr/pgmspace.h>\n");      
      fprintf(targetSourceFile, "const unsigned char Score[] PROGMEM ={\n");
      fprintf(targetSourceFile, "#else\n");
      fprintf(targetSourceFile, "const unsigned char Score[]={\n");
      fprintf(targetSourceFile, "#endif\n");            
      int lineCounter = 0;
      for (auto b : mem)
      {
            fprintf(targetSourceFile, "0x%02x,", (unsigned char)b);
            if (lineCounter > 16)
            {
                  fprintf(targetSourceFile, "\n");
                  lineCounter = 0;
            }
            lineCounter++;
      }
      fprintf(targetSourceFile, "};\n");
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
      unsigned long addr = ihMusicBoxFirmRom.currentAddress() + 1;
      cout << "The score data located at: 0x" << setfill('0') << uppercase << hex << addr << endl;
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
      opts.define("l|scorelist=s", "Midi file list path.");
      opts.define("lower=i", "Proper Lower Pitch");
      opts.define("upper=i", "Proper Upper Pitch");

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
