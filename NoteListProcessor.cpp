#include "NoteListProcessor.h"

namespace noteListProcessor
{
using bprinter::TablePrinter;
MidiHelper::MidiHelper(string midiFilePath)
{
    midifile = MidiFile(midiFilePath);
}

MidiHelper::~MidiHelper()
{
}

void MidiHelper::getTickNoteMap(map<int, vector<int>> &tickNoteMap)
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
                  int t = static_cast<int>(midifile.getTimeInSeconds(midifile[0][i].tick) * tickPerSecond);
                  tickNoteMap[t].push_back(key);
            }
      }


      // Find the last note off
      for (int i = midifile.getNumEvents(0) - 1; i >= 0; --i)
      {
            int command = midifile[0][i][0] & 0xf0;
            if ((command == 0x90 && midifile[0][i][2] == 0) || command == 0x80)
            {
                  key = 128;
                  int t = static_cast<int>(midifile.getTimeInSeconds(midifile[0][i].tick) * tickPerSecond);
                  tickNoteMap[t].push_back(key);
                  break;
            }
      }
}

NoteListProcessor::NoteListProcessor(string midifilePath)
{
      MidiHelper helper=MidiHelper(midifilePath);
      helper.getTickNoteMap(tickNoteMap);
      InitPitchName();
}
NoteListProcessor::~NoteListProcessor()
{

}

void NoteListProcessor::InitPitchName()
{
      string pitchInOneOctave[12]={"C","C#","D","Eb","E","F","F#","G","Ab","A","Bb","B"};


      // Midi pitch 12=C0,108=C8
      for(int i=0;i!=127;++i)
      {
            int octaveGroup=(i-12)/12;
            int pitch=abs((i-12)%12);
            pitchName[i]=to_string(octaveGroup)+":"+pitchInOneOctave[pitch];
      }
      pitchName[128]="Fine";
}

void NoteListProcessor::printAnalyzeResult()
{
      TablePrinter tp(&std::cout);
      tp.AddColumn("Note pitch", 12);
      tp.AddColumn("Occur times", 12);
      tp.PrintHeader();
      for (auto &noteNumMapItem : noteOccurTimesMap)
      {
            tp << pitchName[noteNumMapItem.first] << noteNumMapItem.second;
      }
      tp.PrintFooter();
      cout << "Highest pitch: " << pitchName[highestPitch] << endl << "Lowest pitch: " << pitchName[lowestPitch] << endl;
      cout << "Transpose suggestion: "<< suggestTranpose << " half note";
}

void NoteListProcessor::analyzeNoteMap()
{
      for (auto &noteMapItem : tickNoteMap)
      {
            for (auto &note : noteMapItem.second)
            {
                  ++noteOccurTimesMap[note];
            }
      }

      lowestPitch = noteOccurTimesMap.begin()->first; 
      highestPitch = (--(--noteOccurTimesMap.end()))->first;//remove fine

      int offestToVailidHighestPitch=validHighestPitch-highestPitch;
      int offestToVailidLowestPitch=validLowestPitch-lowestPitch;

      if(offestToVailidHighestPitch>=0 && offestToVailidLowestPitch<=0)
            suggestTranpose=0;
      else if(offestToVailidHighestPitch<0)
            suggestTranpose=offestToVailidHighestPitch; //keep the highest pitch by no means
      else if(offestToVailidLowestPitch>=0)
      {
            if(abs(offestToVailidHighestPitch)>=abs(offestToVailidLowestPitch))
                  suggestTranpose=offestToVailidLowestPitch;
            else
                  suggestTranpose=offestToVailidHighestPitch;
      }
}
void NoteListProcessor::transposeTickNoteMap()
{
      for(auto &tickNoteItem:tickNoteMap)
      {
            vector<int> candidateNotes={};
            for(auto &note:tickNoteItem.second)
            {
                  int transNote=note+suggestTranpose;
                  if(transNote>=validLowestPitch && transNote<=validHighestPitch)
                  {
                        candidateNotes.push_back(transNote-validLowestPitch);
                  }
                  if(note==128)
                  {
                        candidateNotes.push_back(128);
                  }
            }
            if(!candidateNotes.empty())
            {
                  tickNoteMapTransposed[tickNoteItem.first]=candidateNotes;
            }
      }
}

void NoteListProcessor::generateBin(vector<char> &mem)
{
      auto noteMapItem = tickNoteMapTransposed.begin();
      for (; noteMapItem != std::prev(tickNoteMapTransposed.end()); ++noteMapItem)
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

}