#pragma once
#include <string>
#include <sql.h>
#include <sqlext.h>
#include <mbstring.h>

// Define database connection information
SQLWCHAR *ConnStrIn = (SQLWCHAR*)(L"DRIVER={SQL Server};SERVER=(local);DATABASE=Site_Management;Trusted_Connection=yes;");

#define DISPLAY_MAX 50          // Arbitrary limit on column width to display
#define DISPLAY_FORMAT_EXTRA 3  // Per column extra display bytes (| <data> )
#define DISPLAY_FORMAT      L"%c %*.*s "
#define DISPLAY_FORMAT_C    L"%c %-*.*s "
#define NULL_SIZE           6   // <NULL>
#define SQL_QUERY_SIZE      1000 // Max. Num characters for SQL Query passed in.
#define PIPE                L'|'

SQLHENV henv1 = NULL;
SQLHDBC hdbc1 = NULL;
SQLHSTMT hstmt1 = NULL;
WCHAR       wszInput[SQL_QUERY_SIZE];
SHORT   gHeight = 80;       // Users screen height
BOOL	Logged_In = 0;

/******************************************/
/* Structure to store information about   */
/* a column.
/******************************************/

typedef struct STR_BINDING {
	SQLSMALLINT         cDisplaySize;           /* size to display  */
	WCHAR*				wszBuffer;             /* display buffer   */
	SQLLEN              indPtr;                 /* size or null     */
	BOOL                fChar;                  /* character col?   */
	struct STR_BINDING* sNext;                 /* linked list      */
} BINDING;

// Define database structures for constant tables here
class DBUSER {
	int USERID;
	std::string USERNAME;
	int USERACCESS;
};


/*******************************************/
/* Macro to call ODBC functions and        */
/* report an error on failure.             */
/* Takes handle, handle type, and stmt     */
/*******************************************/

#define TRYODBC(h, ht, x)   {   RETCODE rc = x;\
                                if (rc != SQL_SUCCESS) \
                                { \
                                    HandleDiagnosticRecord (h, ht, rc); \
                                } \
                                if (rc == SQL_ERROR) \
                                { \
                                    fwprintf(stderr, L"Error in " L#x L"\n"); \
                                    goto Exit;  \
                                }  \
                            }


