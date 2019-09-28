#ifndef __SCORE_DATA_DOWNLOAD__
#define __SCORE_DATA_DOWNLOAD__

#include <ctype.h>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <algorithm> // remove and remove_if
#include "bprinter/table_printer.h"
#include "ByteStream.h"
#include "fmt/core.h"
#include "Serial.h"

using namespace std;

namespace scoreDataDownload
{
class ScoreDataDownload
{
public:
    std::ostream *defaultOutput;
    Serial serialPort;
    ScoreDataDownload(string dataFilePath, std::ostream *outputStream = &(std::cout));
    ~ScoreDataDownload();
    bool sendCmdPacket(char cmd, const char cmdData[], uint32_t cmdLen);
    bool download();
private:
string dataFilePath;
};
} // namespace scoreDataDownload
#endif
