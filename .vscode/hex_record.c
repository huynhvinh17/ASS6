#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hex_record.h"

// Hàm để chuyển đổi từ ký tự hex sang giá trị số nguyên
int hexToInt(const char *hex)
{
    int value;
    sscanf(hex, "%x", &value);
    return value;
}

// Hàm để tính checksum
bool verifyChecksum(const char *line)
{
    int sum = 0;
    size_t i;
    for (i = 1; i < strlen(line) - 2; i += 2)
    {
        char hex[3] = {line[i], line[i + 1], '\0'};
        sum += hexToInt(hex);
    }
    char checksumHex[3] = {line[strlen(line) - 2], line[strlen(line) - 1], '\0'};
    int checksum = hexToInt(checksumHex);
    sum += checksum;
    return (sum & 0xFF) == 0;
}

// Hàm để phân tích một dòng HEX thành HexRecord
bool parseHexRecord(const char *line, HexRecord *record)
{
    if (line[0] != ':')
    {
        return false;
    }

    record->byteCount = hexToInt(line + 1);
    record->address = hexToInt(line + 3);
    record->recordType = hexToInt(line + 7);
    int i;
    for (i = 0; i < record->byteCount; i++)
    {
        char byteHex[3] = {line[9 + i * 2], line[9 + i * 2 + 1], '\0'};
        record->data[i] = hexToInt(byteHex);
    }

    return true;
}
