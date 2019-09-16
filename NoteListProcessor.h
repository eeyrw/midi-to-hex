#ifndef __NOTE_LIST_PROCESSOR__
#define __NOTE_LIST_PROCESSOR__

#include <ctype.h>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <algorithm> // remove and remove_if
#include <map>
#include "MidiFile.h"
#include "bprinter/table_printer.h"

using namespace std;
using namespace smf;

namespace noteListProcessor
{

class MidiHelper
{
public:
  MidiHelper(string midifilePath);
  ~MidiHelper();
  void getTickNoteMap(map<int, vector<int>> &tickNoteMap);

private:
  int tickPerSecond = 120;
  string midiFilePath;
};

class NoteListProcessor
{
public:
  int highestPitch;
  int lowestPitch;
  int validHighestPitch = 100;
  int validLowestPitch = 45;
  int suggestTranpose;
  double midiDuration;
  map<int, int> noteOccurTimesMap;

  NoteListProcessor(string midifilePath);
  ~NoteListProcessor();
  void setExternTranspose(int t);
  void analyzeNoteMap();
  void transposeTickNoteMap();
  void generateBin(vector<char> &mem);
  void generateDeltaBin(vector<char> &mem);

private:
  map<int, vector<int>> tickNoteMap;
  map<int, vector<int>> tickNoteMapTransposed;
  string pitchName[129];
  bool useExternTransposeParam = false;
  int externTransposeParam = 0;
  void InitPitchName();
  void printAnalyzeResult();
};
}
#endif
