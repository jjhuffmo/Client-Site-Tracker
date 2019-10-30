#pragma once
#include <string>
#include <sql.h>
#include <sqlext.h>
#include <mbstring.h>
#include "Client Site Tracker.h"

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
WCHAR   wszInput[SQL_QUERY_SIZE];
SHORT   gHeight = 80;       // Users screen height

//
//	TABLE: USER_INFO
//
//	PURPOSE: Holds logged in users information (Name, Access Level, etc)
//
//	NOTES: Modify this structure as the User_Info table entries change
//
#define USER_TABLE		"User_Info"					// Name of the table to connect to
#define USER_SIZE		50							// Maximum length of user name to return
#define DBUSERID		1							// Column # of ID field
#define DBUSERNAME		2							// Column # of Name field
#define DBUSERACCESS	3							// Column # of Access Level field
// Define database structures for records here
// User_Info Table
class DBUSER {
public:
	int User_ID = 0;
	CString User_Name;
	int User_Access = 0;
} Current_User;


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


