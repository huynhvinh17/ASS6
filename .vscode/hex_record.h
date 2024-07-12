#ifndef HEX_RECORD_H
#define HEX_RECORD_H

#include <stdbool.h>

typedef struct
{
    int byteCount;
    int address;
    int recordType;
    unsigned char data[255];
} HexRecord;

bool parseHexRecord(const char *line, HexRecord *record);
bool verifyChecksum(const char *line);

#endif // HEX_RECORD_H
