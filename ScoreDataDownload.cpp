#include "ScoreDataDownload.h"

namespace scoreDataDownload
{

ScoreDataDownload::ScoreDataDownload(string dataFilePath, std::ostream *outputStream)
{
    serialPort = Serial(0x1A86, 0x7523);
    //string portName("\\\\.\\COM6");
    //serialPort = Serial(portName, 115200);
    cout<<serialPort.commHandle<<endl;
    this->dataFilePath=dataFilePath;
    defaultOutput = outputStream;
    char temp[512];
    memset(temp,0,512);
    sendCmdPacket(0x02,temp,10);
}

ScoreDataDownload::~ScoreDataDownload()
{
}

std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

bool ScoreDataDownload::download()
{
    int32_t dataSize = filesize(dataFilePath.c_str());
    (*defaultOutput)<<"Data File Szie: "<<dataSize<<endl;
    ifstream scoreDataFile(dataFilePath,ios_base::binary);
    char blockTemp[512];
    int blockIndex=0;
    while(dataSize>0)
    {
        scoreDataFile.read(blockTemp,512);
        int readNum=scoreDataFile.gcount();
        ByteStream cmdData = ByteStream(1024);
        cmdData.writeUInt16(blockIndex);
        cmdData.writeUInt16(readNum);
        cmdData.writeBytes(blockTemp,readNum);
        sendCmdPacket(0x02,cmdData.getBuffer(),cmdData.size());
        _sleep(50);
        (*defaultOutput)<<"Writen: "<<readNum<<endl;
        dataSize-=readNum;
    }
}

bool ScoreDataDownload::sendCmdPacket(char cmd, const char cmdData[], uint32_t cmdLen)
{
    uint16_t totalFrameLength = 2 + 2 + 1 + cmdLen;
    ByteStream frame = ByteStream(totalFrameLength);
    frame.writeUInt16(0x776e);
    frame.writeUInt16(totalFrameLength);
    frame.writeUInt8(cmd);
    frame.writeBytes(cmdData, cmdLen);
    int written = serialPort.write(frame.getBuffer(), frame.size());
    (*defaultOutput)<<"SR WRT: "<<written<<endl;
    if (written == frame.size())
        return true;
    else
        return false;
}

} // namespace scoreDataDownload