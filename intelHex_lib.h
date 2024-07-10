#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    NO_ERROR = 0,
    FILE_OPEN_ERROR,
    LINE_FORMAT_ERROR,
    EOF_RECORD_ERROR,
    CHECKSUM_ERROR,
    SEGMENT_ADDRESS_ERROR
} ErrorCode;

typedef struct
{
    ErrorCode code;
    char message[256];
} ErrorInfo;

int hexToInt(char hex);
int isEndOfFileRecord(const char *line);
int calculateChecksum(const char *line);
int checkStartCode(const char *line);
int checkChecksum(const char *line);
void parseIntelHexRecord(const char *line, int *currentSegmentAddress, int *currentAbsoluteAddress);
