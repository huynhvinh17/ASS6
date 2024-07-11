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

    /** Check if the line starts with ':' and has a length of 11 characters */
    if ((line[0] == ':') && (strlen(line) == 11))
    {
        /**Extract byte count, record type, and checksum from the line */
        int byteCount = (hexToInt(line[1]) << 4) + hexToInt(line[2]);
        int recordType = (hexToInt(line[7]) << 4) + hexToInt(line[8]);
        int checksum = (hexToInt(line[9]) << 4) + hexToInt(line[10]);

        /** Verify if it matches the conditions for an EOF record */
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

    /**Convert pairs of hexadecimal characters to their integer value and add to checksum */
    for (i = 1; i < strlen(line) - 2; i += 2)
    {
        checksum += (hexToInt(line[i]) << 4) + hexToInt(line[i + 1]);
    }

    /** Apply 8-bit mask to ensure checksum is within 0-255 range */
    checksum &= 0xFF;

    /**Caculate checksum complement and return */
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

    /** Calculate expected checksum and compare with the actual checksum from the line */
    if (calculateChecksum(line) != ((hexToInt(line[byteCount * 2 + 9]) << 4) + hexToInt(line[byteCount * 2 + 10])))
    {
        errorCode = CHECKSUM_ERROR; /** Checksum error */
    }

    return errorCode;
}

int processFile(const char *filename, void (*errorCallback)(ErrorCode, ErrorInfo *))
{
    FILE *infile = NULL;
    char line[100]; /** Maximum length of each line in the hex file */

    ErrorInfo errorInfo = {NO_ERROR, ""};
    int hasError = 0; /** Flag to track if there is any error */
    int eofFound = 0; /** Flag to check if EOF record is found */

    /** Open the file for reading */
    infile = fopen(filename, "r");
    if (infile == NULL)
    {
        errorCallback(FILE_OPEN_ERROR, &errorInfo);
        hasError = 1;
    }
    else
    {
        while (fgets(line, sizeof(line), infile))           /** Read each line in the file and check */
        {

            line[strcspn(line, "\n")] = 0;                  /** Remove newline character if present */

            if (checkStartCode(line) != NO_ERROR)           /** Check if line starts with ':' */
            {
                errorCallback(LINE_FORMAT_ERROR, &errorInfo);
                hasError = 1;
            }
            else if (checkChecksum(line) != NO_ERROR)       /** Check for valid checksum */
            {
                errorCallback(CHECKSUM_ERROR, &errorInfo);
                hasError = 1;
            }
            if (isEndOfFileRecord(line))                    /** Check if it is the End of File record */
            {
                printf("\nEnd of File record found.\n");
                eofFound = 1;
            }
        }
        if (!hasError && !eofFound)                         /** Check if the End of File record not found*/
        {
            errorCallback(EOF_RECORD_ERROR, &errorInfo);
            hasError = 1;
        }

        fclose(infile);
    }

    return hasError;
}
