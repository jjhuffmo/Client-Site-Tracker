#pragma once
#ifndef DATABASE_H
#define DATABASE_H


#include "Client Site Tracker.h"

extern INT SQLConnStatus;

INT ConnectSQL(HWND hWnd);
void	CloseDBLinks(void);
void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);


#define DISPLAY_MAX 50          // Arbitrary limit on column width to display
#define DISPLAY_FORMAT_EXTRA 3  // Per column extra display bytes (| <data> )
#define DISPLAY_FORMAT      L"%c %*.*s "
#define DISPLAY_FORMAT_C    L"%c %-*.*s "
#define NULL_SIZE           6   // <NULL>
#define SQL_QUERY_SIZE      1000 // Max. Num characters for SQL Query passed in.
#define PIPE                L'|'

// Define database structures for records here

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

//
//	TABLE: SITE_INFO
//
//	PURPOSE: Customer Site Information
//
//	NOTES: Modify this structure as the Site_Info table entries change
//
#define SITE_TABLE		"Site_Info"					// Name of the table to connect to
#define SITE_MAX_SIZE	255							// Maximum length of string to return from this table
#define ST_SITE_ID		1							// Site ID Column # is 1
#define ST_SHORT_NAME	2							// Site Shot Name Column # is 2
#define ST_FULL_NAME	3							// Site Full Name Column # is 3
#define ST_CUSTOMER		4							// Site Customer Name Column # is 4
#define ST_ADDRESS		5							// Site Address Column # is 5

//
//	TABLE: SITE_USERS
//
//	PURPOSE: Users Access To Customer Sites Information
//
//	NOTES: 
//
#define SITE_USERS_TABLE		"Site_Users"				// Name of the table to connect to
#define SU_SITE_USERS_ID		1							// Site Users ID Column # is 1
#define SU_SITE_ID				2							// Site ID Column # is 2
#define SU_USER_ID				3							// Users ID Column # is 3
#define SU_ACCESS				4							// Users Access To Site Column # is 4

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


std::vector<SITE> Get_Sites(HWND hWnd, INT User_ID, INT Site_ID);
std::vector<SITE_USERS> Get_User_Sites(HWND hWnd, INT User_ID, INT Site_No);

#endif // !DATABASE_H
