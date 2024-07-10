#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "intelHex_lib.h"

int hexToInt(char hex)
{
    char flagHex;
    if (hex >= '0' && hex <= '9')
    {
        flagHex = hex - '0';
    }
    else if (hex >= 'A' && hex <= 'F')
    {
        flagHex = hex - 'A' + 10;
    }
    else if (hex >= 'a' && hex <= 'f')
    {
        flagHex = hex - 'a' + 10;
    }
    else
    {
        flagHex = -1;
    }
    return flagHex;
}

int isEndOfFileRecord(const char *line)
{
    int flagEOF = 0;
    if (line[0] == ':' && strlen(line) == 11)
    {
        int byteCount = (hexToInt(line[1]) << 4) + hexToInt(line[2]);
        int recordType = (hexToInt(line[7]) << 4) + hexToInt(line[8]);
        int checksum = (hexToInt(line[9]) << 4) + hexToInt(line[10]);
        if (byteCount == 0 && recordType == 1 && checksum == 0xFF)
        {
            flagEOF = 1;
        }
    }
    return flagEOF;
}

int calculateChecksum(const char *line)
{
    int i = 0;
    int checksum = 0;
    for (i = 1; i < strlen(line) - 2; i += 2)
    {
        checksum += (hexToInt(line[i]) << 4) + hexToInt(line[i + 1]);
    }
    checksum &= 0xFF;
    checksum = 0xFF - checksum + 1;
    return checksum;
}

int checkStartCode(const char *line)
{
    int errorCode = NO_ERROR;
    if (line[0] != ':')
    {
        errorCode = LINE_FORMAT_ERROR;
    }
    return errorCode;
}

int checkChecksum(const char *line)
{
    int errorCode = NO_ERROR;
    int byteCount = (hexToInt(line[1]) << 4) + hexToInt(line[2]);
    if (calculateChecksum(line) != ((hexToInt(line[byteCount * 2 + 9]) << 4) + hexToInt(line[byteCount * 2 + 10])))
    {
        errorCode = CHECKSUM_ERROR;
    }
    return errorCode;
}

void parseIntelHexRecord(const char *line, int *currentSegmentAddress, int *currentAbsoluteAddress)
{
    int byteCount = (hexToInt(line[1]) << 4) + hexToInt(line[2]);
    int address = (hexToInt(line[3]) << 12) + (hexToInt(line[4]) << 8) + (hexToInt(line[5]) << 4) + hexToInt(line[6]);
    int recordType = (hexToInt(line[7]) << 4) + hexToInt(line[8]);

    if (recordType == 2)
    {
        *currentSegmentAddress = (hexToInt(line[9]) << 12) + (hexToInt(line[10]) << 8) + (hexToInt(line[11]) << 4) + hexToInt(line[12]);
        printf("\nExtended Segment Address: %04X\n", *currentSegmentAddress);
    }
    if (recordType == 4)
    {
        *currentSegmentAddress = (hexToInt(line[9]) << 12) + (hexToInt(line[10]) << 8) + (hexToInt(line[11]) << 4) + hexToInt(line[12]);
        printf("\nExtended Segment Address: %04X\n", *currentSegmentAddress);
    }
    else if (recordType == 0)
    {
        int absoluteAddress = (*currentSegmentAddress << 4) | address;
        *currentAbsoluteAddress = absoluteAddress;

        /** Display data */
        printf("\n| %-10s | %-8s | %-10s | %-39s |\n", "Byte Count", "Address", "Record Type", "Data");
        printf("|------------|----------|-------------|-----------------------------------------|\n");

        if (*currentSegmentAddress == 0)
        {
            printf("| %-10d | %04X     | %02X          | ", byteCount, address, recordType);
        }
        else
        {
            printf("| %-10d | %08X | %02X          | ", byteCount, *currentAbsoluteAddress, recordType);
        }

        int i;
        for (i = 0; i < byteCount; ++i)
        {
            printf("%c%c", line[9 + i * 2], line[10 + i * 2]);
        }
        printf("        |\n");
    }
}
