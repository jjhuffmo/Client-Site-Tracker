// Client Site Tracker.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Client Site Tracker.h"
#include "Database Defs.h"
#include "Text_Arrays.h"
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
void UpdateStatus(void);
void ManageToolBar(HWND hWndParent, HINSTANCE hInst, int hMenu, int Action);
void ManageStatusBar(HWND hWndParent, HINSTANCE hInst, int hMenu, int Action, STATUSBAR* sbVals);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	// Initialize all settings (may change to registry settings or files later
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

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENTSITETRACKER));

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
            case ID_SITE_EXIT:
                DestroyWindow(hWnd);
                break;
			case ID_USER_SIGNIN:
				ConnectSQL(hWnd);
				break;
			case ID_SITE_NEWSITE:
				Status_SQL.Change("Clicked");
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
		UpdateStatus();

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
//	SB_RESIZE - Changes the size of the status bar based on window size
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

void InitStatusBars(void)
{
	// Add all Status Bars here for definitions

	// Main Status Bar (Primary Window)

	// Define each section before pushing definitions onto stack
	// SQL Connection Status
	Status_SQL.Prefix = "SQL: ";
	Status_SQL.Type = PSB_SQLStat;
	Status_SQL.Change("Disconnected");

	// User Information
	Status_User.Prefix = "User: ";
	Status_User.Type = PSB_UserStat;
	Status_User.Change("No User");

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
	MainSBar.Sections.push_back(&Status_Site);
	MainSBar.Sections.push_back(&Status_Tickets);
	MainSBar.group_changed = 1;

	// Create All Status Bars
	All_Status_Bars.Status_Bars.push_back(&MainSBar);
	
}

void UpdateStatus(void)
{
	//Update All System Wide Status Objects (Status Bars/Tool Bars/etc)
	int i = 0;
	if (SQLConnStatus)
	{
		Status_SQL.Change("Connected");
	}
	else
	{
		Status_SQL.Change("Disconnected");
	}

	for (i = 0; i < (int)All_Status_Bars.Status_Bars.size(); i++)
	{
			All_Status_Bars.Status_Bars[i]->Update_SBar();
	}
}

INT ConnectSQL(HWND hWnd)
{
	// Try to log in using Windows authentication
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
			//GetDesktopWindow(),
			NULL,
			ConnStrIn,
			SQL_NTS,
			NULL,
			0,
			NULL,
			SQL_DRIVER_NOPROMPT));

	

	TRYODBC(hdbc1,
		SQL_HANDLE_DBC,
		SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1));

	return 1;

Exit:
	// Return failure if all calls weren't successfully completed
	//MessageBoxW(hWnd, L"Failed To Log Into SQL Database", L"Database Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
	return 0;
}

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