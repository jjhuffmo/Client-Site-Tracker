// Contains all the database calls and returns
#pragma once

#include "Database Defs.h"

// Define database connection information
SQLWCHAR* ConnStrIn = (SQLWCHAR*)(L"DRIVER={SQL Server};SERVER=(local);DATABASE=Site_Management;Trusted_Connection=yes;");
SQLHENV henv1 = NULL;
SQLHDBC hdbc1 = NULL;
SQLHSTMT hstmt1 = NULL;
WCHAR   wszInput[SQL_QUERY_SIZE];
SHORT   gHeight = 80;       // Users screen height
DBUSER Current_User;

//
//  FUNCTION: INT ConnectSQL(HWND hWnd)
//
//  PURPOSE: Attempts to log into the database using Windows credentials.  Used when the program is first run.
//
//  NOTES:  If this function fails to connect, the user will have to manually sign using the pop-up
//
INT ConnectSQL(HWND hWnd)
{
	// Try to connect using Windows authentication
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv1) == SQL_ERROR)
	{
		//Unable to allocate an environment handle
		exit(-1);
	}

	// Register this as an application that expects 3.x behavior,
	// you must register something if you use AllocHandle

	TRYODBC(henv1,
		SQL_HANDLE_ENV,
		SQLSetEnvAttr(henv1,
			SQL_ATTR_ODBC_VERSION,
			(SQLPOINTER)SQL_OV_ODBC3,
			0));

	// Allocate a connection
	TRYODBC(henv1,
		SQL_HANDLE_ENV,
		SQLAllocHandle(SQL_HANDLE_DBC, henv1, &hdbc1));

	// Connect to the driver.  Use the connection string if supplied
	// on the input, otherwise let the driver manager prompt for input.
	TRYODBC(hdbc1,
		SQL_HANDLE_DBC,
		SQLDriverConnect(hdbc1,
			GetDesktopWindow(),
			//NULL,
			ConnStrIn,
			SQL_NTS,
			NULL,
			0,
			NULL,
			SQL_DRIVER_COMPLETE));

	TRYODBC(hdbc1,
		SQL_HANDLE_DBC,
		SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1));

	return 1;

Exit:
	// Return failure if all calls weren't successfully completed
	return 0;
}

//
//  FUNCTION: CloseDBLinks(void)
//
//  PURPOSE: Closes (frees) all database handles
//
//  NOTES:  Called before exiting to release resources and do a clean exit
//
void CloseDBLinks(void)
{
	// Free ODBC handles and exit

	if (hstmt1)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);
	}

	if (hdbc1)
	{
		SQLDisconnect(hdbc1);
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc1);
	}

	if (henv1)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, henv1);
	}

	SQLConnStatus = 0;
}


/************************************************************************
/* HandleDiagnosticRecord : display error/warning information
/*
/* Parameters:
/*      hHandle     ODBC handle
/*      hType       Type of handle (HANDLE_STMT, HANDLE_ENV, HANDLE_DBC)
/*      RetCode     Return code of failing command
/************************************************************************/

void HandleDiagnosticRecord(SQLHANDLE      hHandle,
	SQLSMALLINT    hType,
	RETCODE        RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER  iError;
	WCHAR       wszMessage[1000];
	WCHAR       wszState[SQL_SQLSTATE_SIZE + 1];


	if (RetCode == SQL_INVALID_HANDLE)
	{
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}

	while (SQLGetDiagRec(hType,
		hHandle,
		++iRec,
		wszState,
		&iError,
		wszMessage,
		(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)),
		(SQLSMALLINT*)NULL) == SQL_SUCCESS)
	{
		// Hide data truncated.. 
		if (wcsncmp(wszState, L"01004", 5))
		{
			fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
		}
	}
}

//
//  FUNCTION: int CheckUser(void)
//
//  PURPOSE: Checks the user's rights whenever the user name changes.  If the related Access_Level == 0 then hide all options in menu other than Sign In
//
//  NOTES:  Will need to make sure to shut down all open windows on sign out in this section when those sections get defined.
//
int CheckUser(CString New_User)
{
	SQLINTEGER sqlUserID = 0, sqlUserIDPtr;
	SQLWCHAR* sqlUserName = (SQLWCHAR*)malloc(USER_SIZE);
	SQLINTEGER sqlUserNamePtr;
	SQLINTEGER sqlAccessLevel = 0, sqlAccessLevelPtr;
	//SQLRETURN results = 0;
	// If the user is a domain user, truncate the domain off to get the raw name
	if (New_User.Find(L"\\", 0) > 0)
	{
		New_User = New_User.Right(New_User.GetLength() - (New_User.Find(L"\\", 0) + 1));
	}
	CString Query = "SELECT * FROM " + (CString)USER_TABLE + " WHERE User_Name = '" + New_User + "'";

	if (New_User == "Logout")
	{
		goto Exit;
	}
	TRYODBC(hdbc1,
		SQL_HANDLE_DBC,
		SQLExecDirect(hstmt1, (SQLWCHAR*)(LPCWSTR)(Query), SQL_NTS));
	TRYODBC(hdbc1,
		SQL_HANDLE_DBC,
		SQLFetch(hstmt1));
	TRYODBC(hdbc1,
		SQL_HANDLE_DBC,
		SQLGetData(hstmt1, DBUSERID, SQL_C_SSHORT, &sqlUserID, 0, &sqlUserIDPtr));
	TRYODBC(hdbc1,
		SQL_HANDLE_DBC,
		SQLGetData(hstmt1, DBUSERNAME, SQL_WCHAR, sqlUserName, USER_SIZE, &sqlUserNamePtr));
	TRYODBC(hdbc1,
		SQL_HANDLE_DBC,
		SQLGetData(hstmt1, DBUSERACCESS, SQL_C_SSHORT, &sqlAccessLevel, 0, &sqlAccessLevelPtr));

	Current_User.User_ID = (int)sqlUserID;
	Current_User.User_Name = sqlUserName;
	Current_User.User_Access = (int)sqlAccessLevel;

	return (int)sqlAccessLevel;
Exit:
	Current_User.User_Access = 0;
	Current_User.User_Name = "Not Logged In";
	return 0;
}

//
//  FUNCTION: BOOL Read_Sites(INT User_ID, std::vector<SITE> SiteList)
//
//  PURPOSE: Reads the sites database table and stores the information in the structure
//
//  ARGUMENTS:	User_ID -> If only listing User Sites, this is the user_id to read
//				SiteList -> Holding Vector of sites
//				Site_ID -> If trying to get Site Information, this is the Site_ID to read
//
std::vector<SITE> Get_Sites(HWND hWnd, INT User_ID, INT Site_ID)
{
	SQLINTEGER sqlSiteID, sqlSiteIDPtr;
	SQLWCHAR* sqlShortName = (SQLWCHAR*)malloc(SITE_MAX_SIZE);
	SQLWCHAR* sqlFullName = (SQLWCHAR*)malloc(SITE_MAX_SIZE);
	SQLWCHAR* sqlCustName = (SQLWCHAR*)malloc(SITE_MAX_SIZE);
	SQLWCHAR* sqlAddress = (SQLWCHAR*)malloc(SITE_MAX_SIZE);
	SQLINTEGER sqlShortNamePtr, sqlFullNamePtr, sqlCustNamePtr, sqlAddressPtr;
	SQLRETURN results = 0;
	INT found = 0, i = 0;
	INT Has_Access = 0;
	CString Query = "SELECT * FROM " + (CString)SITE_TABLE;
	CString SID = L"";
	CString UID = L"";
	
	std::vector<SITE> SiteList;
	std::vector<SITE_USERS> Users_Sites;
	SITE HoldingSite;

	if (User_ID > 0)
	{
		Users_Sites = Get_User_Sites(hWnd, User_ID, 0);
		// Add logic to read the Site_Users database to retrieve just this users available sites
		Query = "SELECT * FROM " + (CString)SITE_TABLE;
	}
	if (Site_ID > 0)
	{
		SID.Format(L"%d", Site_ID);

		SID.Format(L"%d", Site_ID);
		Query = "SELECT * FROM " + (CString)SITE_TABLE + " WHERE Site_ID = " + SID;
	}

	SQLConnStatus = (ConnectSQL(hWnd));

	if (SQLConnStatus)
	{
		TRYODBC(hdbc1,
			SQL_HANDLE_DBC,
			SQLExecDirect(hstmt1, (SQLWCHAR*)(LPCWSTR)(Query), SQL_NTS));

		// Read the database until no records match
		while (1)
		{
			TRYODBC(hdbc1,
				SQL_HANDLE_DBC,
				SQLFetch(hstmt1));
			TRYODBC(hdbc1,
					SQL_HANDLE_DBC,
					SQLGetData(hstmt1, ST_SITE_ID, SQL_C_SLONG, &sqlSiteID, 0, &sqlSiteIDPtr));
			TRYODBC(hdbc1,
				SQL_HANDLE_DBC,
				SQLGetData(hstmt1, ST_SHORT_NAME, SQL_WCHAR, sqlShortName, SITE_MAX_SIZE, &sqlShortNamePtr));
			TRYODBC(hdbc1,
				SQL_HANDLE_DBC,
				SQLGetData(hstmt1, ST_FULL_NAME, SQL_WCHAR, sqlFullName, SITE_MAX_SIZE, &sqlFullNamePtr));
			TRYODBC(hdbc1,
				SQL_HANDLE_DBC,
				SQLGetData(hstmt1, ST_CUSTOMER, SQL_WCHAR, sqlCustName, SITE_MAX_SIZE, &sqlCustNamePtr));
			TRYODBC(hdbc1,
				SQL_HANDLE_DBC,
				SQLGetData(hstmt1, ST_ADDRESS, SQL_WCHAR, sqlAddress, SITE_MAX_SIZE, &sqlAddressPtr));
			HoldingSite.Site_ID = (INT)sqlSiteID;
			HoldingSite.Short_Name = (CString)sqlShortName;
			HoldingSite.Full_Name = (CString)sqlFullName;
			HoldingSite.Customer_Name = (CString)sqlCustName;
			HoldingSite.Address = (CString)sqlAddress;
			if (User_ID > 0)
			{
				Has_Access = false;
				for (i = 0; i < (INT)Users_Sites.size(); i++)
				{
					if (HoldingSite.Site_ID == Users_Sites[i].Site_ID)
					{
						Has_Access = true;
					}
				}
				if (Has_Access)
				{
					SiteList.push_back(HoldingSite);
					found++;
				}
			}
			else
			{
				SiteList.push_back(HoldingSite);
				found++;
			}
		}
	}

Exit:
		return SiteList;
}

//
//  FUNCTION: std::vector<SITE_USERS> Get_User_Sites(INT User_ID, INT Site_No)
//
//  PURPOSE: Reads the list of databases the user has access to.

//  ARGUMENTS:	User_ID -> User ID of sites to get
//				Site_No -> Site Number to retrieve, if 0 then get all of them
//
std::vector<SITE_USERS> Get_User_Sites(HWND hWnd, INT User_ID, INT Site_No)
{
	SQLINTEGER sqlSiteUserID, sqlSiteUserIDPtr;
	SQLINTEGER sqlSiteID, sqlSiteIDPtr;
	SQLINTEGER sqlUserID, sqlUserIDPtr;
	SQLINTEGER sqlAccess, sqlAccessPtr;

	std::vector<SITE_USERS> Users_List;
	SITE_USERS Current_List;

	CString UserID = "";
	CString SiteNo = "";
	CString Query = "";

	UserID.Format(L"%d", User_ID);
	SiteNo.Format(L"%d", Site_No);

	SQLConnStatus = (ConnectSQL(hWnd));
	
	if (Site_No > 0)
	{
		Query = "SELECT * FROM " + (CString)SITE_USERS_TABLE + " WHERE User_ID = " + UserID + " AND Site_ID = " + SiteNo;
	}
	else
	{
		Query = "SELECT * FROM " + (CString)SITE_USERS_TABLE + " WHERE User_ID = " + UserID;
	}

	TRYODBC(hdbc1,
		SQL_HANDLE_DBC,
		SQLExecDirect(hstmt1, (SQLWCHAR*)(LPCWSTR)(Query), SQL_NTS));

	// Read the database until no records match
	while (1)
	{
		TRYODBC(hdbc1,
			SQL_HANDLE_DBC,
			SQLFetch(hstmt1));
		TRYODBC(hdbc1,
			SQL_HANDLE_DBC,
			SQLGetData(hstmt1, SU_SITE_USERS_ID, SQL_C_SLONG, &sqlSiteUserID, 0, &sqlSiteUserIDPtr));
		TRYODBC(hdbc1,
			SQL_HANDLE_DBC,
			SQLGetData(hstmt1, SU_SITE_ID, SQL_C_SLONG, &sqlSiteID, 0, &sqlSiteIDPtr));
		TRYODBC(hdbc1,
			SQL_HANDLE_DBC,
			SQLGetData(hstmt1, SU_USER_ID, SQL_C_SLONG, &sqlUserID, 0, &sqlUserIDPtr));
		TRYODBC(hdbc1,
			SQL_HANDLE_DBC,
			SQLGetData(hstmt1, SU_ACCESS, SQL_C_SLONG, &sqlAccess, 0, &sqlAccessPtr));

		Current_List.Site_Users_ID = (INT)sqlSiteUserID;
		Current_List.Site_ID = (INT)sqlSiteID;
		Current_List.User_ID = (INT)sqlUserID;
		Current_List.Access = (INT)sqlAccess;
		Users_List.push_back(Current_List);
	}

Exit:

return Users_List;

}