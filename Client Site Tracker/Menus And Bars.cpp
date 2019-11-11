#include "Menus And Bars.h"

// Global Variables
extern DBUSER Current_User;


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
void ManageStatusBar(HWND hWndParent, HINSTANCE hInst, int hMenu, int Action, StatusBar* sbVals)
{
	HWND hwndStatus;
	RECT rcClient;
	int paParts[10];
	int i, j, nWidth;
	int rightEdge;
	INITCOMMONCONTROLSEX cctrls;
	int cParts = (int)sbVals->Sections.size();

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
		if (cParts > 0)
		{
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
			SendMessage(GetDlgItem(hWndParent, hMenu), WM_SIZE, (WPARAM)cParts, (LPARAM)paParts);
		}
		break;

	case SB_UPDATE:
		// Update the sections with data
		hwndStatus = GetDlgItem(hWndParent, hMenu);
		if (sbVals->group_changed && SBars_Init)
		{
			j = 0;
			for (i = 0; i < cParts; i++)
			{
				if (sbVals->changed[i])
				{
					SendMessage(hwndStatus, SB_SETTEXT, (WPARAM)i, (LPARAM)(LPCWSTR)sbVals->Sec_Text[i]);
					sbVals->changed[i] = 0;
					j++;
				}
				else
				{
					j++;
				}
			}
			if (j >= int(sbVals->Sections.size()))
			{
				sbVals->group_changed = 0;
			}
			DrawMenuBar(hWndParent);
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
	//MainSBar.Sections.push_back(&Status_SQL);
	MainSBar.Sections.push_back(&Status_SQL);
	MainSBar.Sections.push_back(&Status_User);
	MainSBar.Sections.push_back(&Status_Access);
	MainSBar.Sections.push_back(&Status_Site);
	MainSBar.Sections.push_back(&Status_Tickets);
	MainSBar.group_changed = 1;

	// Create All Status Bars
	All_Status_Bars.push_back(&MainSBar);

	SBars_Init = 1;
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
			MMB_Login.Update(hInst, hWnd, Current_User.User_Access, L"Sign Out");
		}
		else
		{
			MMB_Login.Update(hInst, hWnd, Current_User.User_Access, L"Sign In");
		}
	}

	// Validate security access
	if (Status_Access.changed)
	{
		Validate_Security(hWnd);
	}

	for (i = 0; i < (int)All_Status_Bars.size(); i++)
	{
		All_Status_Bars[i]->Update_SBar();
	}

	// Refresh the Main Window Status Bar
	ManageStatusBar(hWnd, hInst, IDC_MAIN_STATUS, SB_UPDATE, &MainSBar);
}

//
//  FUNCTION: SetSecurity(void)
//
//  PURPOSE: Sets the security levels for resources, also initializes the resource class for changing text if necessary

//
void SetSecurity(void)
{
	// Main Status Bar User Sign In/Out
	MMB_Login.Resource_Type = RES_MENUITEM;
	MMB_Login.Resource_ID = IDM_USER_SIGNIN;
	MMB_Login.Min_Security = 0;

	// Main Status Bar User My Sites
	MMB_MySites.Resource_Type = RES_MENUITEM;
	MMB_MySites.Resource_ID = IDM_USER_LISTMYSITES;
	MMB_MySites.Min_Security = 1;

	// Main Status Bar User My Tickets
	MMB_MyTickets.Resource_Type = RES_MENUITEM;
	MMB_MyTickets.Resource_ID = IDM_USER_LISTMYTICKETS;
	MMB_MyTickets.Min_Security = 1;

	// Main Status Bar System Manager
	MMB_Sys_Management.Resource_Type = RES_MENU;
	MMB_Sys_Management.Resource_ID = IDC_SYS_MANAGEMENT;
	MMB_Sys_Management.Min_Security = 5000;
	MMB_Sys_Management.Label = L"&Management";
	MMB_Sys_Management.Location = 1;

	// Push all definitions on to the stack
	All_Security.push_back(&MMB_Login);
	All_Security.push_back(&MMB_MySites);
	All_Security.push_back(&MMB_MyTickets);
	All_Security.push_back(&MMB_Sys_Management);
}
