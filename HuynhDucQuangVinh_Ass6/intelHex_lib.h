/*******************************************************************************
 * Definitions
 ******************************************************************************/

#ifndef _INTELHEX_LIB_
#define _INTELHEX_LIB_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/**
 * @brief Enumeration of error codes
 */
typedef enum
{
    NO_ERROR = 0,      /** No error occurred */
    FILE_OPEN_ERROR,   /** Error opening file */
    LINE_FORMAT_ERROR, /** Error: Line does not start with ':' */
    EOF_RECORD_ERROR,  /** EOF record not found */
    CHECKSUM_ERROR,    /** Checksum error */
} ErrorCode;

/**
 * @brief Structure to hold error information.
 */
typedef struct
{
    ErrorCode code;    /** Error code */
    char message[256]; /** Error message */
} ErrorInfo;

/**
 * @brief Converts a hexadecimal character to its integer value.
 *
 * @param hex The hexadecimal character ('0'-'9', 'A'-'F', 'a'-'f').
 * @return int The integer value of the hexadecimal character, or -1 if invalid.
 */
int hexToInt(char hex);

/**
 * @brief Checks if a line from an Intel Hex file is an End of File (EOF) record.
 *
 * @param line The line to check.
 * @return int Returns 1 if it is an EOF record, otherwise 0.
 */
int isEndOfFileRecord(const char *line);

/**
 * @brief Calculates the checksum of a line from an Intel Hex file.
 *
 * @param line The line to calculate checksum for.
 * @return int The calculated checksum.
 */
int calculateChecksum(const char *line);

/**
 * @brief Checks if a line from an Intel Hex file starts with ':'.
 *
 * @param line The line to check.
 * @return int Returns NO_ERROR if the line starts with ':', otherwise LINE_FORMAT_ERROR.
 */
int checkStartCode(const char *line);

/**
 * @brief Checks the checksum of a line from an Intel Hex file.
 *
 * @param line The line to check.
 * @return int Returns NO_ERROR if the checksum is correct, otherwise CHECKSUM_ERROR.
 */
int checkChecksum(const char *line);

/**
 * @brief Processes an Intel Hex file, performing error checks and parsing records.
 *
 * @param filename The name of the Intel Hex file to process.
 * @param errorCallback Pointer to a function that handles errors.
 * @return int Returns 0 if no errors were encountered, 1 otherwise.
 */
int processFile(const char *filename, void (*errorCallback)(ErrorCode, ErrorInfo *));

#endif /* _INTELHEX_LIB_ */
