// Client Site Tracker.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Client Site Tracker.h"
#include "Database Defs.h"
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
BOOL Init_Login = 0;

void HandleDiagnosticRecord(SQLHANDLE      hHandle,
	SQLSMALLINT    hType,
	RETCODE        RetCode);

void DisplayResults(HSTMT       hStmt,
	SQLSMALLINT cCols);

void AllocateBindings(HSTMT         hStmt,
	SQLSMALLINT   cCols,
	BINDING** ppBinding,
	SQLSMALLINT* pDisplay);


void DisplayTitles(HSTMT    hStmt,
	DWORD    cDisplaySize,
	BINDING* pBinding);

void SetConsole(DWORD   cDisplaySize,
	BOOL    fInvert);

void UpdateStatus(void);
void ManageToolBar(HWND hWndParent, HINSTANCE hInst, int hMenu, int Action);
void ManageStatusBar(HWND hWndParent, HINSTANCE hInst, int hMenu, int cParts, int Action, STATUSBAR sbVals);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	// Create main window status bar
	MainSBar.parts = PSB_PARTS;
	MainSBar.Update_List(L"Disconnected", PSB_DBSTAT);
	MainSBar.Update_List(L"No Site Selected", PSB_SITE);
	MainSBar.Update_List(L"No Ticket Selected", PSB_TICKETS);

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

   // Generate the main Toolbar and the main Status Bar
   ManageToolBar(hWnd, hInst, IDC_MAIN_TOOLBAR, TB_CREATE);
   ManageStatusBar(hWnd, hInst, IDC_MAIN_STATUS, PSB_PARTS, SB_CREATE, MainSBar);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   
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
	// Connect to the Site Management Database
	if (Init_Login == 0)
	{
		SQLSTATUS = ConnectSQL(hWnd);
		if (SQLSTATUS == 1)
		{
			Init_Login = 1;
		}
		else
		{
			DestroyWindow(hWnd);
		}
	}
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
			ManageStatusBar(hWnd, hInst, IDC_MAIN_STATUS, PSB_PARTS, SB_REFRESH, MainSBar);
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
		ManageStatusBar(hWnd, hInst, IDC_MAIN_STATUS, PSB_PARTS, SB_UPDATE, MainSBar);
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
void ManageStatusBar(HWND hWndParent, HINSTANCE hInst, int hMenu, int cParts, int Action, STATUSBAR sbVals)
{
	HWND hwndStatus;
	RECT rcClient;
	int paParts[10];
	int i, nWidth;
	int rightEdge;
	INITCOMMONCONTROLSEX cctrls;

	switch (Action)
	{
	case SB_CREATE:
		// Ensure that the common control DLL is loaded. 
		cctrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
		cctrls.dwICC = ICC_BAR_CLASSES;

		InitCommonControlsEx(&cctrls);

		// Create the status bar.
		hwndStatus = CreateWindowEx(
			0,                       // no extended styles
			STATUSCLASSNAMEW,         // name of status bar class
			(PCTSTR)NULL,           // no text when first created
			SBARS_SIZEGRIP |         // includes a sizing grip
			WS_CHILD | WS_VISIBLE,   // creates a visible child window
			0, 0, 0, 0,              // ignores size and position
			hWndParent,              // handle to parent window
			(HMENU)hMenu,       // child window identifier
			hInst,                   // handle to application instance
			NULL);                   // no window creation data

	case SB_REFRESH:
		SendMessage(GetDlgItem(hWndParent, hMenu), WM_SIZE, 0, 0);
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
		break;

	case SB_UPDATE:
		// Update the sections with data
		hwndStatus = GetDlgItem(hWndParent, hMenu);
		for (i = 1 ; i <= sbVals.parts; i++)
		{
			SendMessageW(hwndStatus, SB_SETTEXT, i-1, sbVals.Sec_Text[i]);
		}
		break;

	}

}

void UpdateStatus(void)
{
	const wchar_t* SQLStatus;
	const wchar_t* UName;
		
	if (SQLSTATUS == 1)
	{
		SQLStatus = L"SQL: Connected";
	}
	else
	{
		SQLStatus = L"SQL: Disconnected";
	}
	std::wstring hold(SQLStatus);
	hold += " User: ";
	std::wstring hold2(UName);
	hold2 += USER_NAME;
	hold += hold2;
	const wchar_t* DBStatus = hold.c_str();

	MainSBar.Update_List(DBStatus, PSB_DBSTAT);

	/*
	// Get Menu Handle
	HMENU MainMenu = GetMenu(hWnd);

	// Take the system variables and update the status displays as necessary

	// If successfully logged in, then change the menu item to say so
	ModifyMenuW(MainMenu, ID_USER_SIGNIN, MF_BYCOMMAND | MF_STRING, ID_USER_SIGNIN, (LPCWSTR)L"Logoff");
	*/
}

INT ConnectSQL(HWND hWnd)
{
	// Try to log in using Windows authentication
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv1) == SQL_ERROR)
	{
		fwprintf(stderr, L"Unable to allocate an environment handle\n");
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
	MessageBoxW(hWnd, L"Failed To Log Into SQL Database", L"Database Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
	return 0;
}

/************************************************************************
/* DisplayResults: display results of a select query
/*
/* Parameters:
/*      hStmt      ODBC statement handle
/*      cCols      Count of columns
/************************************************************************/

void DisplayResults(HSTMT       hStmt,
	SQLSMALLINT cCols)
{
	BINDING* pFirstBinding, * pThisBinding;
	SQLSMALLINT     cDisplaySize;
	RETCODE         RetCode = SQL_SUCCESS;
	int             iCount = 0;

	// Allocate memory for each column 

	AllocateBindings(hStmt, cCols, &pFirstBinding, &cDisplaySize);

	// Set the display mode and write the titles

	DisplayTitles(hStmt, cDisplaySize + 1, pFirstBinding);


	// Fetch and display the data

	bool fNoData = false;

	do {
		// Fetch a row

		if (iCount++ >= gHeight - 2)
		{
			int     nInputChar;
			bool    fEnterReceived = false;

			while (!fEnterReceived)
			{
				wprintf(L"              ");
				SetConsole(cDisplaySize + 2, TRUE);
				wprintf(L"   Press ENTER to continue, Q to quit (height:%hd)", gHeight);
				SetConsole(cDisplaySize + 2, FALSE);

				nInputChar = _getch();
				wprintf(L"\n");
				if ((nInputChar == 'Q') || (nInputChar == 'q'))
				{
					goto Exit;
				}
				else if ('\r' == nInputChar)
				{
					fEnterReceived = true;
				}
				// else loop back to display prompt again
			}

			iCount = 1;
			DisplayTitles(hStmt, cDisplaySize + 1, pFirstBinding);
		}

		TRYODBC(hStmt, SQL_HANDLE_STMT, RetCode = SQLFetch(hStmt));

		if (RetCode == SQL_NO_DATA_FOUND)
		{
			fNoData = true;
		}
		else
		{

			// Display the data.   Ignore truncations

			for (pThisBinding = pFirstBinding;
				pThisBinding;
				pThisBinding = pThisBinding->sNext)
			{
				if (pThisBinding->indPtr != SQL_NULL_DATA)
				{
					wprintf(pThisBinding->fChar ? DISPLAY_FORMAT_C : DISPLAY_FORMAT,
						PIPE,
						pThisBinding->cDisplaySize,
						pThisBinding->cDisplaySize,
						pThisBinding->wszBuffer);
				}
				else
				{
					wprintf(DISPLAY_FORMAT_C,
						PIPE,
						pThisBinding->cDisplaySize,
						pThisBinding->cDisplaySize,
						L"<NULL>");
				}
			}
			wprintf(L" %c\n", PIPE);
		}
	} while (!fNoData);

	SetConsole(cDisplaySize + 2, TRUE);
	wprintf(L"%*.*s", cDisplaySize + 2, cDisplaySize + 2, L" ");
	SetConsole(cDisplaySize + 2, FALSE);
	wprintf(L"\n");

Exit:
	// Clean up the allocated buffers

	while (pFirstBinding)
	{
		pThisBinding = pFirstBinding->sNext;
		free(pFirstBinding->wszBuffer);
		free(pFirstBinding);
		pFirstBinding = pThisBinding;
	}
}

/************************************************************************
/* AllocateBindings:  Get column information and allocate bindings
/* for each column.
/*
/* Parameters:
/*      hStmt      Statement handle
/*      cCols       Number of columns in the result set
/*      *lppBinding Binding pointer (returned)
/*      lpDisplay   Display size of one line
/************************************************************************/

void AllocateBindings(HSTMT         hStmt,
	SQLSMALLINT   cCols,
	BINDING** ppBinding,
	SQLSMALLINT* pDisplay)
{
	SQLSMALLINT     iCol;
	BINDING* pThisBinding, * pLastBinding = NULL;
	SQLLEN          cchDisplay, ssType;
	SQLSMALLINT     cchColumnNameLength;

	*pDisplay = 0;

	for (iCol = 1; iCol <= cCols; iCol++)
	{
		pThisBinding = (BINDING*)(malloc(sizeof(BINDING)));
		if (!(pThisBinding))
		{
			fwprintf(stderr, L"Out of memory!\n");
			exit(-100);
		}

		if (iCol == 1)
		{
			*ppBinding = pThisBinding;
		}
		else
		{
			pLastBinding->sNext = pThisBinding;
		}
		pLastBinding = pThisBinding;


		// Figure out the display length of the column (we will
		// bind to char since we are only displaying data, in general
		// you should bind to the appropriate C type if you are going
		// to manipulate data since it is much faster...)

		TRYODBC(hStmt,
			SQL_HANDLE_STMT,
			SQLColAttribute(hStmt,
				iCol,
				SQL_DESC_DISPLAY_SIZE,
				NULL,
				0,
				NULL,
				&cchDisplay));


		// Figure out if this is a character or numeric column; this is
		// used to determine if we want to display the data left- or right-
		// aligned.

		// SQL_DESC_CONCISE_TYPE maps to the 1.x SQL_COLUMN_TYPE. 
		// This is what you must use if you want to work
		// against a 2.x driver.

		TRYODBC(hStmt,
			SQL_HANDLE_STMT,
			SQLColAttribute(hStmt,
				iCol,
				SQL_DESC_CONCISE_TYPE,
				NULL,
				0,
				NULL,
				&ssType));


		pThisBinding->fChar = (ssType == SQL_CHAR ||
			ssType == SQL_VARCHAR ||
			ssType == SQL_LONGVARCHAR);

		pThisBinding->sNext = NULL;

		// Arbitrary limit on display size
		if (cchDisplay > DISPLAY_MAX)
			cchDisplay = DISPLAY_MAX;

		// Allocate a buffer big enough to hold the text representation
		// of the data.  Add one character for the null terminator

		pThisBinding->wszBuffer = (WCHAR*)malloc((cchDisplay + 1) * sizeof(WCHAR));

		if (!(pThisBinding->wszBuffer))
		{
			fwprintf(stderr, L"Out of memory!\n");
			exit(-100);
		}

		// Map this buffer to the driver's buffer.   At Fetch time,
		// the driver will fill in this data.  Note that the size is 
		// count of bytes (for Unicode).  All ODBC functions that take
		// SQLPOINTER use count of bytes; all functions that take only
		// strings use count of characters.

		TRYODBC(hStmt,
			SQL_HANDLE_STMT,
			SQLBindCol(hStmt,
				iCol,
				SQL_C_TCHAR,
				(SQLPOINTER)pThisBinding->wszBuffer,
				(cchDisplay + 1) * sizeof(WCHAR),
				&pThisBinding->indPtr));


		// Now set the display size that we will use to display
		// the data.   Figure out the length of the column name

		TRYODBC(hStmt,
			SQL_HANDLE_STMT,
			SQLColAttribute(hStmt,
				iCol,
				SQL_DESC_NAME,
				NULL,
				0,
				&cchColumnNameLength,
				NULL));

		pThisBinding->cDisplaySize = max((SQLSMALLINT)cchDisplay, cchColumnNameLength);
		if (pThisBinding->cDisplaySize < NULL_SIZE)
			pThisBinding->cDisplaySize = NULL_SIZE;

		*pDisplay += pThisBinding->cDisplaySize + DISPLAY_FORMAT_EXTRA;

	}

	return;

Exit:

	exit(-1);

	return;
}


/************************************************************************
/* DisplayTitles: print the titles of all the columns and set the
/*                shell window's width
/*
/* Parameters:
/*      hStmt          Statement handle
/*      cDisplaySize   Total display size
/*      pBinding        list of binding information
/************************************************************************/

void DisplayTitles(HSTMT     hStmt,
	DWORD     cDisplaySize,
	BINDING* pBinding)
{
	WCHAR           wszTitle[DISPLAY_MAX];
	SQLSMALLINT     iCol = 1;

	SetConsole(cDisplaySize + 2, TRUE);

	for (; pBinding; pBinding = pBinding->sNext)
	{
		TRYODBC(hStmt,
			SQL_HANDLE_STMT,
			SQLColAttribute(hStmt,
				iCol++,
				SQL_DESC_NAME,
				wszTitle,
				sizeof(wszTitle), // Note count of bytes!
				NULL,
				NULL));

		wprintf(DISPLAY_FORMAT_C,
			PIPE,
			pBinding->cDisplaySize,
			pBinding->cDisplaySize,
			wszTitle);
	}

Exit:

	wprintf(L" %c", PIPE);
	SetConsole(cDisplaySize + 2, FALSE);
	wprintf(L"\n");

}


/************************************************************************
/* SetConsole: sets console display size and video mode
/*
/*  Parameters
/*      siDisplaySize   Console display size
/*      fInvert         Invert video?
/************************************************************************/

void SetConsole(DWORD dwDisplaySize,
	BOOL  fInvert)
{
	HANDLE                          hConsole;
	CONSOLE_SCREEN_BUFFER_INFO      csbInfo;

	// Reset the console screen buffer size if necessary

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	if (hConsole != INVALID_HANDLE_VALUE)
	{
		if (GetConsoleScreenBufferInfo(hConsole, &csbInfo))
		{
			if (csbInfo.dwSize.X < (SHORT)dwDisplaySize)
			{
				csbInfo.dwSize.X = (SHORT)dwDisplaySize;
				SetConsoleScreenBufferSize(hConsole, csbInfo.dwSize);
			}

			gHeight = csbInfo.dwSize.Y;
		}

		if (fInvert)
		{
			SetConsoleTextAttribute(hConsole, (WORD)(csbInfo.wAttributes | BACKGROUND_BLUE));
		}
		else
		{
			SetConsoleTextAttribute(hConsole, (WORD)(csbInfo.wAttributes & ~(BACKGROUND_BLUE)));
		}
	}
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

	MainSBar.Update_List(L"Disonnected", PSB_DBSTAT);
}
