// Client Site Tracker.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Client Site Tracker.h"
#include "Database Defs.h"
#include "Security.h"
#include "Status_Bars.h"
#include <sql.h>
#include <mbstring.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <Windows.h>
#include <tchar.h>
#include <sal.h>
#include <string>
#include <CommCtrl.h>
#include <vector>
#include <algorithm>


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
INT					ConnectSQL(HWND hWnd);
void				CloseDBLinks(void);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void HandleDiagnosticRecord(SQLHANDLE      hHandle,
	SQLSMALLINT    hType,
	RETCODE        RetCode);

void InitStatusBars(void);
void UpdateStatus(HWND hWnd);
void ManageToolBar(HWND hWndParent, HINSTANCE hInst, int hMenu, int Action);
void ManageStatusBar(HWND hWndParent, HINSTANCE hInst, int hMenu, int Action, STATUSBAR* sbVals);
int CheckUser(CString New_User);
void Validate_Security(HWND hWnd);
void SetSecurity(void);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

	// Initialize all status bars
	InitStatusBars();


    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENTSITETRACKER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	SetSecurity();

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENTSITETRACKER));

	// Initialize all settings (may change to registry settings or files later
	// Get the current user if possible, if not then return Unknown to indicate an issue
	LPDWORD System_Buff = (LPDWORD)malloc(SYSBUFF);
	LPWSTR Temp_Name = (LPWSTR)malloc(SYSBUFF * sizeof(LPWSTR));
	if (System_Buff != 0)
	{
		GetUserNameW(Temp_Name, System_Buff);
		Current_User.User_Name = (LPWSTR)(LPCWSTR)Temp_Name;
		CheckUser(Current_User.User_Name);
	}
	Status_User.Change(Current_User.User_Name);

	MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENTSITETRACKER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CLIENTSITETRACKER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   // Try to autoconnect to SQL
   // Connect to the Site Management Database
   // If we log in correctly, then move forward
   SQLConnStatus = (ConnectSQL(hWnd));

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   // Generate the main Toolbar and the main Status Bar
   ManageToolBar(hWnd, hInst, IDC_MAIN_TOOLBAR, TB_CREATE);
   ManageStatusBar(hWnd, hInst, IDC_MAIN_STATUS, SB_CREATE, &MainSBar);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	/*int result;
	HMENU hMenu;
	hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDC_CLIENTSITETRACKER));
	LPCWSTR DTData;
	UINT_PTR NewData = NULL;

	//result = ModifyMenu(hMenu, 1, MF_BYPOSITION, MF_DISABLED, NULL );
	MENUITEMINFOW ItemDetail = { sizeof(MENUITEMINFO) };
	ItemDetail.cbSize = sizeof(MENUITEMINFO);
	*/
	switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_SITE_EXIT:
                DestroyWindow(hWnd);
                break;
			case IDM_USER_SIGNIN:
				CheckUser(Current_User.User_Name);
				break;
			case IDM_SITE_NEWSITE:
				Validate_Security(hWnd);
				DrawMenuBar(hWnd);
				break;
			case IDM_SITE_OPENSITE:
				DrawMenuBar(hWnd);
				//DestroyMenu(hMenu);
				break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
			// Reset the tool bar and status bar when main window needs repainted
			ManageToolBar(hWnd, hInst, IDC_MAIN_TOOLBAR, TB_REFRESH);
			ManageStatusBar(hWnd, hInst, IDC_MAIN_STATUS, SB_REFRESH, &MainSBar);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
		// Close all database links
		CloseDBLinks();
        break;
	
    default:
		// Update System Wide Status
		UpdateStatus(hWnd);

		// Refresh the Main Window Status Bar
		ManageStatusBar(hWnd, hInst, IDC_MAIN_STATUS, SB_UPDATE, &MainSBar);

        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

//
//  FUNCTION: ManageToolBar(HWND hWndParent, HINSTANCE hInst, int Action)
//
//  PURPOSE: Manages the primary toolbar
//
//  TB_CREATE - Creates the toolbar
//	TB_RESIZE - Changes the size of the toolbar based on window size
//	TB_MODIFY - Modifies the toolbar based on active options
//
void ManageToolBar(HWND hWndParent, HINSTANCE hInst, int hMenu, int Action)
{
	HWND hTool;

	INITCOMMONCONTROLSEX cctrls;
	switch (Action)
		{
		case TB_CREATE:
			// Ensure that the common control DLL is loaded. 
			cctrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
			cctrls.dwICC = ICC_BAR_CLASSES;

			InitCommonControlsEx(&cctrls);

			// Create Toolbar
			hTool = CreateWindowEx(0, TOOLBARCLASSNAMEW, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
				hWndParent, (HMENU)hMenu, GetModuleHandle(NULL), NULL);
			// Send the TB_BUTTONSTRUCTSIZE message, which is required for
			// backward compatibility.
			SendMessage(hTool, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
			break;

		case TB_REFRESH:
			SendMessage(GetDlgItem(hWndParent, hMenu), WM_SIZE, 0, 0);
			break;
		}
}

//
//  FUNCTION: ManageStatusBar(HWND hWndParent, HINSTANCE hInst, int hMenu, int cParts, int Action)
//
//  PURPOSE: Manages the primary toolbar
//
//  SB_CREATE - Creates the status bar
//	SB_REFRESH - Redraws the status bar based on window size
//	SB_UPDATE - Updates the contents of the status bar
//
void ManageStatusBar(HWND hWndParent, HINSTANCE hInst, int hMenu, int Action, STATUSBAR* sbVals)
{
	HWND hwndStatus;
	RECT rcClient;
	int paParts[10];
	int i, j, nWidth;
	int rightEdge;
	INITCOMMONCONTROLSEX cctrls;
	int cParts = (int) sbVals->Sections.size();

	switch (Action)
	{
	case SB_CREATE:
		// Ensure that the common control DLL is loaded. 
		cctrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
		cctrls.dwICC = ICC_BAR_CLASSES;

		InitCommonControlsEx(&cctrls);

		// Create the status bar.
		hwndStatus = CreateWindowEx(
			0,							// no extended styles
			STATUSCLASSNAMEW,			// name of status bar class
			(PCTSTR)NULL,				// no text when first created
			SBARS_SIZEGRIP |			// includes a sizing grip
			WS_CHILD | WS_VISIBLE,		// creates a visible child window
			0, 0, 0, 0,					// ignores size and position
			hWndParent,					// handle to parent window
			(HMENU)hMenu,				// child window identifier
			hInst,						// handle to application instance
			NULL);						// no window creation data
		SendMessage(hwndStatus, SB_SETTEXT, (WPARAM)0, (LPARAM)L"Initializing:");

	case SB_REFRESH:
		//SendMessage(GetDlgItem(hWndParent, hMenu), WM_SIZE, 0, 0);
		// Get the coordinates of the parent window's client area.
		GetClientRect(hWndParent, &rcClient);

		// Calculate the right edge coordinate for each part, and
		// copy the coordinates to the array.
		nWidth = rcClient.right / cParts;
		rightEdge = nWidth;
		for (i = 0; i < cParts; i++)
		{
			paParts[i] = rightEdge;
			rightEdge += nWidth;
			// Tell the status bar to create the window parts.
			SendMessage(GetDlgItem(hWndParent, hMenu), SB_SETPARTS, (WPARAM)cParts, (LPARAM)
				paParts);
		}
		SendMessage(GetDlgItem(hWndParent, hMenu), WM_SIZE, (WPARAM)cParts, (LPARAM) paParts);
		break;

	case SB_UPDATE:
		// Update the sections with data
		hwndStatus = GetDlgItem(hWndParent, hMenu);
		if (sbVals->group_changed)
		{
			j = 0;
			for (i = 0; i < (int)sbVals->Sections.size(); i++)
			{
				//if (sbVals->changed[i])
				//{

					LPARAM holding = (LPARAM)(LPCWSTR)sbVals->Sec_Text[i+1];
					SendMessage(hwndStatus, SB_SETTEXT, (WPARAM)i, holding);
					
						sbVals->changed[i] = 0;
				//		j++;
					
				//}
				//else
				//{
				//	j++;
				//}
			}
			//if (j >= int(sbVals->Sections.size()))
			//{
			//	sbVals->group_changed = 0;

			//}
		}

		break;
	}
}

//
//  FUNCTION: InitStatusBars(void)
//
//  PURPOSE: Initializes all the status bars in the system by creating default entries and building the bar content objects
//
//  NOTES:  All status bars that will be used in the program should be initialized here and added to the 
//			All_Status_Bars group so the content will be updated consistently
//
void InitStatusBars(void)
{
	// Main Status Bar (Primary Window)

	// Define each section before pushing definitions onto stack
	// SQL Connection Status
	Status_SQL.Prefix = "SQL: ";
	Status_SQL.Type = PSB_SQLStat;
	Status_SQL.Change("Disconnected");

	// User Information
	Status_User.Prefix = "User: ";
	Status_User.Type = PSB_UserStat;
	//Status_User.Change("No User");

	// User Access Level
	Status_Access.Prefix = "Access Level: ";
	Status_Access.Type = PSB_Access;

	// Site Information
	Status_Site.Prefix = "Site: ";
	Status_Site.Type = PSB_Site;
	Status_Site.Change("No Site Selected");

	// Ticket Information
	Status_Tickets.Prefix = "Ticket: ";
	Status_Tickets.Type = PSB_Tickets;
	Status_Tickets.Change("No Ticket Selected");

	// Load each section into the Status Bar
	MainSBar.Sections.push_back(&Status_SQL);
	MainSBar.Sections.push_back(&Status_User);
	MainSBar.Sections.push_back(&Status_Access);
	MainSBar.Sections.push_back(&Status_Site);
	MainSBar.Sections.push_back(&Status_Tickets);
	MainSBar.group_changed = 1;

	// Create All Status Bars
	All_Status_Bars.Status_Bars.push_back(&MainSBar);
	
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
	SQLWCHAR *sqlUserName = (SQLWCHAR*)malloc(USER_SIZE);
	SQLINTEGER sqlUserNamePtr;
	SQLINTEGER sqlAccessLevel = 0, sqlAccessLevelPtr;
	SQLRETURN results = 0;
	CString Query = "SELECT * FROM " + (CString)USER_TABLE + " WHERE User_Name = '" + New_User + "'";

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
//  FUNCTION: UpdateStatus(void)
//
//  PURPOSE: Updates all the status bar(s) content in the project
//
//  NOTES:  This section should not be used to do control of variables, but just reporting the status
//
void UpdateStatus(HWND hWnd)
{
	//Update All System Wide Status Objects (Status Bars/Tool Bars/etc)
	int i = 0;
	CString Access = L"0";

	if (SQLConnStatus)
	{
		Status_SQL.Change("Connected");
	}
	else
	{
		Status_SQL.Change("Disconnected");
	}
	Access.Format(L"%d", Current_User.User_Access);
	Status_Access.Change(Access);
	Status_User.Change(Current_User.User_Name);
	if (Status_User.changed)
	{
		if (Current_User.User_Name != "Not Logged In")
		{
			MMB_Login.Update(hWnd, L"Sign Out");
		}
		else
		{
			MMB_Login.Update(hWnd, L"Sign In");
		}
	}

	// Validate security access
	if (Status_Access.changed)
	{
		Validate_Security(hWnd);
	}

	for (i = 0; i < (int)All_Status_Bars.Status_Bars.size(); i++)
	{
			All_Status_Bars.Status_Bars[i]->Update_SBar();
	}
}

//
//  FUNCTION: INT ConnectSQL(HWND hWnd)
//
//  PURPOSE: Attempts to log into the database using Windows credentials.  Used when the program is first run.
//
//  NOTES:  If this function fails to connect, a flag is set and the user will have to manually sign into the database in ManConnectSQL()
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
//  FUNCTION: SetSecurity(void)
//
//  PURPOSE: Sets the security levels for resources, also initializes the resource class for changing text if necessary

//
void SetSecurity(void)
{
	// Main Status Bar (Primary Window)
	MMB_Login.Resource_Type = RES_MENU;
	MMB_Login.Resource_ID = IDM_USER_SIGNIN;
	MMB_Login.Min_Security = 0;

	// Main Status Bar (Primary Window)
	MMB_MySites.Resource_Type = RES_MENU;
	MMB_MySites.Resource_ID = IDM_USER_LISTMYSITES;
	MMB_MySites.Min_Security = 6000;


	// Push all definitions on to the stack
	All_Security.push_back(&MMB_Login);
	All_Security.push_back(&MMB_MySites);
}


//
//  FUNCTION: Validate_Security(void)
//
//  PURPOSE: Anytime the security access value changes, go through and modify system wide settings to lock out or disable controls
//
//  NOTES:  By default, everything will be disabled
//
void Validate_Security(HWND hWnd)
{
	HMENU NewMenu;
	int result = 0;
	int i = 0;
	MENUITEMINFO MenuItem = { sizeof(LPMENUITEMINFO) };
	CString entry;

	int Records = All_Security.size();

	for (i = 0; i < Records; i++)
	{
		All_Security[i]->Update(hWnd);
	}

	MenuItem.fMask = MIIM_STRING | MIIM_ID;
	MenuItem.fType = MIIM_STRING;
	MenuItem.cbSize = sizeof(MENUITEMINFOW);

	// Check to see if there's a Site2 Menu, if not then add it. 
	for (i = 0; i <= GetMenuItemCount(GetMenu(hWnd)); i++)
	{
		MenuItem.dwTypeData = NULL;
		result = GetMenuItemInfoW(GetMenu(hWnd), i, true, &MenuItem);

		entry = (LPWSTR)MenuItem.fType;
	}
	NewMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDC_SITE));
	result = AppendMenuW(GetMenu(hWnd), MF_POPUP, (UINT_PTR)NewMenu, L"Site2" );
	DestroyMenu(NewMenu);
}