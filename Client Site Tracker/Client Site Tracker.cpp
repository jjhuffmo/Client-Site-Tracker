// Client Site Tracker.cpp : Defines the entry point for the application.
//
#pragma once
#include "Client Site Tracker.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR cszWindowClass[MAX_LOADSTRING];            // the main window class name
int SBars_Init = 0;								// Status Bars Initialized indicator so we don't access invalid objects
HWND hWndClient;

StatusBar MainSBar;
Status_Var Status_SQL;
Status_Var Status_User;
Status_Var Status_Site;
Status_Var Status_Tickets;
Status_Var Status_Access;

// Primary Status Bar Locations - Defaults
int PSB_SQLStat = 0;			// Database Status (Slot 1)
int PSB_UserStat = 1;			// Active User (Slot 2)
int PSB_Site = 3;				// Site Information (Slot 3)
int PSB_Tickets = 4;			// Tickets Information (Slot 4)
int PSB_Access = 2;				// Current User Access Level (Slot 5)
INT SQLConnStatus = 0;			// SQL Connection Status Flag (0 = Disconnected, 1 = Connected)

Resource_Security MMB_Login, MMB_MySites, MMB_Sys_Management, MMB_MyTickets, MMB_Sites, MMB_Tickets, MMB_CloseSite, MMB_Resources;

std::vector<Resource_Security*> MMB_Security;
std::vector<StatusBar*> All_Status_Bars;
std::vector<SITE> SitesList;

extern DBUSER Current_User;
extern SQLWCHAR* ConnStrIn;

extern void ManageToolBar(HWND hWndParent, HINSTANCE hInst, int hMenu, int Action);
extern void ManageStatusBar(HWND hWndParent, HINSTANCE hInst, int hMenu, int Action, StatusBar* sbVals);
extern BOOL Read_Sites(HWND hWnd, INT User_ID, std::vector<SITE> SiteList);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	// Initialize all status bars;
	InitStatusBars();

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENTSITETRACKER, szWindowClass, MAX_LOADSTRING);    
	LoadStringW(hInstance, IDC_CLIENTSITETRACKER_CHILD, cszWindowClass, MAX_LOADSTRING);
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

	TCHAR Temp_Name[UNLEN + 1];
	DWORD System_Buff = UNLEN + 1;
	if (System_Buff != 0)
	{
		GetUserNameW(Temp_Name, &System_Buff);
		Current_User.User_Name = (LPWSTR)(LPCWSTR)Temp_Name;
		CheckUser(Current_User.User_Name);
	}
	Status_User.Change(Current_User.User_Name);

	MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateMDISysAccel(msg.hwnd, &msg) && !TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
		}
		// After updating the screen messages, update system wide parameters
		// Update System Wide Status
		UpdateStatus(msg.hwnd);
	}
    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance) noexcept
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
    wcex.hbrBackground  = (HBRUSH)(COLOR_APPWORKSPACE+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CLIENTSITETRACKER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	wcex.lpfnWndProc = ChildWndProc;
	wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_CLIENTSITETRACKER));
	wcex.lpszMenuName = (LPCTSTR)NULL;
	wcex.cbWndExtra = 0; // CBWNDEXTRA;
	wcex.lpszClassName = cszWindowClass;

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
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
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
   ShowWindow(hWndClient, SW_SHOW);
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
	CLIENTCREATESTRUCT ccs;

	switch (message)
    {
	case WM_CREATE:
	{
		ccs.idFirstChild = 1200;
		ccs.hWindowMenu = NULL;

		hWndClient = CreateWindowW(L"MDICLIENT", (LPCTSTR)NULL, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
			0, 0, 0, 0, hWnd, nullptr, hInst, (LPSTR)&ccs);
	}
	break;

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
				if (Current_User.User_Access == 0)
				{
					DialogBox(hInst, MAKEINTRESOURCE(IDD_USER), hWnd, LoginPU);

					//CheckUser(Current_User.User_Name);
				}
				else
				{
					CloseDBLinks();
					CheckUser("Logout");
				}

				break;
			case IDM_MGR_SITES:
				if (MMB_Tickets.Enabled)
				{
					MMB_Tickets.Enabled = 0;
				}
				else
				{
					MMB_Tickets.Enabled = 1;
				}
				Validate_Security(hWnd);
				break;
			case IDM_SITE_NEWSITE:
				break;
			case IDM_SITE_OPENSITE:
				
				break;
			case IDM_USER_LISTMYTICKETS:
				if (MMB_Resources.Enabled)
				{
					MMB_Resources.Enabled = 0;
				}
				else
				{
					MMB_Resources.Enabled = 1;
				}
				Validate_Security(hWnd);
				break;
			case IDM_USER_LISTMYSITES:
				Show_Sites(hInst, hWnd, SW_SHOWNORMAL);
				Read_Sites(hWnd, 0, SitesList);
				break;

            default:
				CString MissedMessage;
				MissedMessage.Format(L"%d", message);
				MessageBox(hWnd, (LPCWSTR)MissedMessage, L"Action Not Caught", 0);
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
			if (SBars_Init)
			{
				// Reset the tool bar and status bar when main window needs repainted
				ManageToolBar(hWnd, hInst, IDC_MAIN_TOOLBAR, TB_REFRESH);
				ManageStatusBar(hWnd, hInst, IDC_MAIN_STATUS, SB_REFRESH, &MainSBar);
			}
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
		// Close all database links
		CloseDBLinks();
        break;
	
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		case IDM_SITE_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_USER_SIGNIN:
			if (Current_User.User_Access == 0)
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_USER), hWnd, LoginPU);

				//CheckUser(Current_User.User_Name);
			}
			else
			{
				CloseDBLinks();
				CheckUser("Logout");
			}

			break;
		case IDM_MGR_SITES:
			if (MMB_Tickets.Enabled)
			{
				MMB_Tickets.Enabled = 0;
			}
			else
			{
				MMB_Tickets.Enabled = 1;
			}
			Validate_Security(hWnd);
			break;
		case IDM_SITE_NEWSITE:
			break;
		case IDM_SITE_OPENSITE:

			break;
		case IDM_USER_LISTMYTICKETS:
			if (MMB_Resources.Enabled)
			{
				MMB_Resources.Enabled = 0;
			}
			else
			{
				MMB_Resources.Enabled = 1;
			}
			Validate_Security(hWnd);
			break;
		case IDM_USER_LISTMYSITES:
			Show_Sites(hInst, hWnd, SW_SHOWNORMAL);
			break;

		default:
			CString MissedMessage;
			MissedMessage.Format(L"%d", message);
			MessageBox(hWnd, (LPCWSTR)MissedMessage, L"Action Not Caught", 0);
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		if (SBars_Init)
		{
			// Reset the tool bar and status bar when main window needs repainted
			ManageToolBar(hWnd, hInst, IDC_MAIN_TOOLBAR, TB_REFRESH);
			ManageStatusBar(hWnd, hInst, IDC_MAIN_STATUS, SB_REFRESH, &MainSBar);
		}
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		// Close all database links
		CloseDBLinks();
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for username/password popup.
INT_PTR CALLBACK LoginPU(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDLOGIN)
		{
			// Try logging in as a SQL User
			// This section will be updated in the future to use LogonUserW to use other network users through AD
			CString ConnSt = "";
			TCHAR Name[25];
			TCHAR Pwd[25];

			GetDlgItemText(hDlg, IDC_USERNAME, Name, 25);
			GetDlgItemText(hDlg, IDC_USERPWD, Pwd, 25);

			//ConnSt = _T("DRIVER = { SQL Server }; SERVER = (local); DATABASE = Site_Management; User Id = ") + (CString)Name + _T("; Password = ") + (CString)Pwd + _T(";");

			//ConnStrIn = (SQLWCHAR*)((LPWSTR)(LPCWSTR)ConnSt);

			// Try to autoconnect to SQL
			// Connect to the Site Management Database
			// If we log in correctly, then move forward
			SQLConnStatus = (ConnectSQL((HWND)wParam));

			if (SQLConnStatus)
			{
				CheckUser((CString)Name);
			}
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
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
//  FUNCTION: Validate_Security(void)
//
//  PURPOSE: Anytime the security access value changes, go through and modify system wide settings to lock out or disable controls
//
//  NOTES:  By default, everything will be disabled
//
void Validate_Security(HWND hWnd)
{
	int i = 0;
	int x = 0, y = 0;
	int Max_Menu = 1;

	// If security is 0 (logged out), then reset all options to default
	if (Current_User.User_Access == 0)
	{
		MMB_Resources.Enabled = 0;
		MMB_Tickets.Enabled = 0;
	}
	int Records = MMB_Security.size();

	// Update the Main Menu Bar
	for (i = 0; i < Records; i++)
	{
		Max_Menu = 1;
		// Update the menu positions
		for (x = 0; x < Records; x++)
		{
			if (MMB_Security[x]->Resource_Type == RES_MENU && MMB_Security[x]->Active_Pos > 0)
			{
				Max_Menu += MMB_Security[x]->Location;
			}
		}
		MMB_Security[i]->Update(hInst, hWnd, Current_User.User_Access, Max_Menu);
		for (x = 0; x < Records; x++)
		{
			if (MMB_Security[x]->Resource_Type == RES_MENU && MMB_Security[x]->Active_Pos > 0)
			{
				// If we're not in the right place, see if we need to move things around
				if (MMB_Security[x]->Active_Pos < MMB_Security[x]->Location)
				{
					for (y = 0; y < Records; y++)
					{
						if (MMB_Security[y]->Active_Pos < MMB_Security[x]->Active_Pos)
						{
							MMB_Security[x]->Active_Pos++;
						}
					}

				}
				// If we're not the first menu item, then make sure there's one below us, otherwise move down the line
				int temp = 0;
				if (MMB_Security[x]->Active_Pos > 1)
				{
					for (y = 0; y < Records; y++)
					{
						if (MMB_Security[y]->Active_Pos != 0 && MMB_Security[y]->Active_Pos < MMB_Security[x]->Active_Pos)
						{
							temp++;
						}
					}
					MMB_Security[x]->Active_Pos = 1+temp;
				}
			}
		}
	}
	DrawMenuBar(hWnd);
}

BOOL Show_Sites(HINSTANCE hInstance, HWND Owner, int nCmdShow)
{
	MDICREATESTRUCTW mccs;
	HWND chWnd;

	mccs.szClass = cszWindowClass;
	mccs.hOwner = hInstance;
	mccs.x = mccs.cx = CW_USEDEFAULT;
	mccs.y = mccs.cy = CW_USEDEFAULT;
	//mccs.style = MDIS_ALLCHILDSTYLES;
	mccs.style = 0;
	mccs.szTitle = L"Popup";

	chWnd = (HWND) SendMessage(hWndClient, WM_MDICREATE,0, (LPARAM) (LPMDICREATESTRUCTW) &mccs);
	
	if (!chWnd)
	{
		return FALSE;
	}

	ShowWindow(chWnd, SW_SHOWNORMAL);
	UpdateWindow(chWnd);
}