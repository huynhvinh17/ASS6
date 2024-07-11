/*******************************************************************************
 * Definitions
 ******************************************************************************/

#include "intelHex_lib.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

int currentRecordtype = 0; /** Variable to keep track of the current record type */

/*******************************************************************************
 * Code
 ******************************************************************************/

/**
 * @brief Handles error codes by setting error messages based on the code
 *
 * @param code The error code to handle.
 * @param errorInfo Pointer to the structure holding error information.
 */
void handleErrorCode(ErrorCode code, ErrorInfo *errorInfo)
{
    /** Set the error message based on the error code */
    switch (code)
    {
    case FILE_OPEN_ERROR:
        snprintf(errorInfo->message, sizeof(errorInfo->message), "Cannot open file");                       /** Handle file open error*/
        break;
    case LINE_FORMAT_ERROR:
        snprintf(errorInfo->message, sizeof(errorInfo->message), "Error: Line does not start with ':'");    /** Handle line format error */
        break;
    case EOF_RECORD_ERROR:
        snprintf(errorInfo->message, sizeof(errorInfo->message), "EOF record not found");                   /** Handle EOF record error */
        break;
    case CHECKSUM_ERROR:
        snprintf(errorInfo->message, sizeof(errorInfo->message), "Checksum error");                         /** Handle checksum error */
        break;
    default:
        snprintf(errorInfo->message, sizeof(errorInfo->message), "Unknown error");                          /** Handle unknown error */
        break;
    }

    /**Print the error code and message to stderr */
    fprintf(stderr, "Error Code: %d\n",code );
    fprintf(stderr, "Error Message: %s\n", errorInfo->message);
}

/**
 * @brief Parses an Intel Hex record line and prints the parsed data.
 *
 * @param line The line to parse.
 * @param currentExtendedAddress Pointer to the current segment address.
 * @param currentAbsoluteAddress Pointer to the current absolute address.
 */
void intelHexRecord(const char *line, int *currentExtendedAddress, int *currentAbsoluteAddress)
{
    int byteCount = (hexToInt(line[1]) << 4) + hexToInt(line[2]);   /** Parse byte count */
    int address = (hexToInt(line[3]) << 12) + (hexToInt(line[4]) << 8) + (hexToInt(line[5]) << 4) + hexToInt(line[6]);  /**Parse address */
    int recordType = (hexToInt(line[7]) << 4) + hexToInt(line[8]);  /** Parse record type */
    int i = 0;          /** Loop variable */

    if (2 == recordType)
    {
        /** Calculate and display absolute address for extended segment address record */
        *currentExtendedAddress = (hexToInt(line[9]) << 12) + (hexToInt(line[10]) << 8) + (hexToInt(line[11]) << 4) + hexToInt(line[12]);
        printf("\nExtended Segment Address: %04X\n", *currentExtendedAddress);
        currentRecordtype = 2;
    }
    if (4 == recordType)
    {
        /** Calculate and display absolute address for extended linear address record */
        *currentExtendedAddress = (hexToInt(line[9]) << 12) + (hexToInt(line[10]) << 8) + (hexToInt(line[11]) << 4) + hexToInt(line[12]);
        printf("\nExtended linear Address: %04X\n", *currentExtendedAddress);
        currentRecordtype = 4;
    }
    if (0 == recordType)
    {

        /** Display the data bytes in the record */
        printf("\n| %-10s | %-8s | %-10s | %-39s |\n", "Byte Count", "Address", "Record Type", "Data");
        printf("|------------|----------|-------------|-----------------------------------------|\n");

        if (0 == currentRecordtype)
        {
            printf("| %-10d | %04X     | %02X          | ", byteCount, address, recordType);
        }
        else if (2 == currentRecordtype)
        {
            int absoluteAddress = (*currentExtendedAddress << 4) | address;
            *currentAbsoluteAddress = absoluteAddress;

            printf("| %-10d | %08X | %02X          | ", byteCount, *currentAbsoluteAddress, recordType);
        }
        else if (4 == currentRecordtype)
        {
            int absoluteAddress = (*currentExtendedAddress << 16) | address;
            *currentAbsoluteAddress = absoluteAddress;

            printf("| %-10d | %08X | %02X          | ", byteCount, *currentAbsoluteAddress, recordType);
        }

        for (i = 0; i < byteCount; ++i)
        {
            printf("%c%c", line[9 + i * 2], line[10 + i * 2]);
        }
        printf("        |\n");
    }
}

/**
 * @brief Main function to demonstrate processing of a Intel Hex file.
 *
 * @return int Program exit status.
 */
int main(void)
{
    const char *filename = "example.hex";   /** Filename of the Intel Hex file to process */
    FILE *infile = NULL;                    /** File pointer to read data from the file*/
    char line[100];                         /** Maximum length of each line in the hex file */
    ErrorInfo errorInfo = {NO_ERROR, ""};   /** Structure to hold error information */
    int currentExtendedAddress = 0;         /** Current extended address*/
    int currentAbsoluteAddress = 0;         /** Current absolute address*/

    /** Process the Intel Hex file with error handling */
    if (!processFile(filename, handleErrorCode))
    {
        printf("\nNo errors found. Displaying data:\n");    /**If no errors are found, display a message and start reading the file */
        infile = fopen(filename, "r");                      /** Open the file for reading */
        while (fgets(line, sizeof(line), infile))
        {
            line[strcspn(line, "\n")] = 0;                  /**Remove newline character if present */

            if (checkStartCode(line) == NO_ERROR && checkChecksum(line) == NO_ERROR)        /** Check the start code and checksum of the line */
            {
                intelHexRecord(line, &currentExtendedAddress, &currentAbsoluteAddress);     /** Parse and display data from the hex line */
            }
        }
        fclose(infile);     /**Close the file after reading */
    }

    return 0;
}
