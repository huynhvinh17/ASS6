#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "intelHex_lib.h"

void handleErrorCode(ErrorCode code, ErrorInfo *errorInfo)
{
    switch (code)
    {
    case FILE_OPEN_ERROR:
        snprintf(errorInfo->message, sizeof(errorInfo->message), "Cannot open file");
        break;
    case LINE_FORMAT_ERROR:
        snprintf(errorInfo->message, sizeof(errorInfo->message), "Error: Line does not start with ':'");
        break;
    case CHECKSUM_ERROR:
        snprintf(errorInfo->message, sizeof(errorInfo->message), "Checksum error");
        break;
    default:
        snprintf(errorInfo->message, sizeof(errorInfo->message), "Unknown error");
        break;
    }
}

int processFile(const char *filename, void (*errorCallback)(ErrorCode, ErrorInfo *))
{
    FILE *infile = NULL;
    char line[100]; /** Maximum length of each line in the hex file */
    int currentSegmentAddress = 0;
    int currentAbsoluteAddress = 0;
    ErrorInfo errorInfo = {NO_ERROR, ""};
    int hasError = 0; // Flag to track if there is any error
    int eofFound = 0; // Flag to check if EOF record is found

    /** Open the file for reading */
    infile = fopen(filename, "r");
    if (infile == NULL)
    {
        errorCallback(FILE_OPEN_ERROR, &errorInfo);
        hasError = 1;
    }
    else
    {
        /** Read each line in the file and check */
        while (fgets(line, sizeof(line), infile))
        {
            /** Remove newline character if present */
            line[strcspn(line, "\n")] = 0;

            /** Check if line starts with ':' */
            if (checkStartCode(line) != NO_ERROR)
            {
                errorCallback(LINE_FORMAT_ERROR, &errorInfo);
                hasError = 1;
                break;
            }

            /** Check for valid checksum */
            if (checkChecksum(line) != NO_ERROR)
            {
                errorCallback(CHECKSUM_ERROR, &errorInfo);
                hasError = 1;
                break;
            }

            /** Check if it is the End of File record */
            if (isEndOfFileRecord(line))
            {
                printf("\nEnd of File record found.\n");
                eofFound = 1;
                break;
            }
        }

        fclose(infile);
    }

    /** Check if EOF record was not found */
    if (!hasError && !eofFound)
    {
        errorCallback(EOF_RECORD_ERROR, &errorInfo);
        hasError = 1;
    }

    /** If there was no error, print the result */
    if (!hasError)
    {
        printf("\nNo errors found. Displaying data:\n");
        // Re-open the file to start reading from the beginning
        infile = fopen(filename, "r");
        if (infile == NULL)
        {
            errorCallback(FILE_OPEN_ERROR, &errorInfo);
            hasError = 1;
        }
        else
        {
            while (fgets(line, sizeof(line), infile))
            {
                line[strcspn(line, "\n")] = 0;

                // Skip processing lines with errors
                if (checkStartCode(line) == NO_ERROR && checkChecksum(line) == NO_ERROR)
                {
                    // Process the line and display information
                    parseIntelHexRecord(line, &currentSegmentAddress, &currentAbsoluteAddress);
                }
            }

            fclose(infile);
        }
    }

    return hasError ? 1 : 0;
}

int main(void)
{
    const char *filename = "example.hex";
    ErrorInfo errorInfo = {NO_ERROR, ""};

    int result = processFile(filename, handleErrorCode);

    if (result != 0)
    {
        fprintf(stderr, "%s\n", errorInfo.message);
    }

    return 0;
}
