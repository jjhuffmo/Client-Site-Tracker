#pragma once

#include "Client Site Tracker.h"

// Database Users
struct DBUSER {
public:
	int User_ID = 0;
	CString User_Name;
	int User_Access = 0;
	std::vector<INT> Sites;
};

// Site Info
struct SITE {
public:
	int Site_ID = 0;
	CString Short_Name;
	CString Full_Name;
	CString Customer_Name;
	CString Address;
};

// Site Info
struct SITE_USERS
{
public:
	int Site_Users_ID = 0;
	int Site_ID = 0;
	int User_ID = 0;
	int Access = 0;
};

struct Active_Sites
{
	SITE Site_Info;
	SITE_USERS Site_User;
};

// Define resource types for identifying control and security
#define RES_MENU		1		// Entire Menu
#define RES_MENUITEM	2		// Single item on menu

class Resource_Security
{
private:
	int result = 0;
	int i = 0;
	int found = 0;
	HMENU hMenu = NULL, NewMenu = NULL;

public:
	int Resource_Type = 0;				// Type of resource as an int - predefined above
	UINT Resource_ID = 0;				// Resource ID as identified in the resource tables
	int Min_Security = 0;				// Minimum Security Value - any user access level high than this number will enable it
	int Enabled = 1;					// Enable/disable bit for turning options on and off based on open windows
	int Location = 0;					// Location on a menu for menu resources
	int Active_Pos = 0;					// Active Position it's displayed, 0 means not displayed

	CString Label = "Default";

	// Update the access rights and label if necessary. 
	// NewValue means different things to different resources:
	//		RES_MENU - The title of the pop-up menu to see if it exists or to give it a title when created
	//		RES_MENUITEM - Update the menu text or if NewValue == Keep then skip updating the text part
	int Update(HINSTANCE hInst, HWND hWnd, int AccessLevel = 0, int Max_Menu = 0, CString NewValue = "Keep")
	{
		found = 0;
		switch (Resource_Type)
		{
			// If it's a main menu item
		case RES_MENU:
			hMenu = GetMenu(hWnd);

			if (AccessLevel >= Min_Security && Enabled)  // Valid security to see
			{
				// If it doesn't exist, insert it in the proper spot
				if (Active_Pos == 0)
				{
					NewMenu = LoadMenu(hInst, MAKEINTRESOURCE(Resource_ID));
					if (Location < Max_Menu)
					{
						result = InsertMenuW(hMenu, Location, MF_BYPOSITION | MF_POPUP, (UINT_PTR)NewMenu, Label);
						Active_Pos = Location;
					}
					else
					{
						int temp = GetMenuItemCount(hMenu) - 1;
						result = InsertMenuW(hMenu, temp, MF_BYPOSITION | MF_POPUP, (UINT_PTR)NewMenu, Label);
						Active_Pos = temp;
					}
					DestroyMenu(NewMenu);
				}
			}
			else
			{
				if (Active_Pos > 0)
				{
					result = DeleteMenu(hMenu, Active_Pos, MF_BYPOSITION);
					DestroyMenu(NewMenu);
					Active_Pos = 0;
				}
			}
			break;

			// If it's a Menu Item
		case RES_MENUITEM:
			hMenu = GetMenu(hWnd);
			if (AccessLevel >= Min_Security && Enabled)
			{
				result = EnableMenuItem(hMenu, Resource_ID, MF_ENABLED);
			}
			else
			{
				result = EnableMenuItem(hMenu, Resource_ID, MF_GRAYED);
			}
			if (NewValue != "Keep")
			{
				result = ModifyMenu(hMenu, Resource_ID, MF_BYCOMMAND | MF_STRING, Resource_ID, (LPCTSTR)NewValue);
			}
			break;
		}
		return result;
	}
};

class SitesMDI
{
public:
	HWND hwnd = nullptr;		// Window Handle to the MDI form
	INT Site_Access = 0;	// Access level for THIS site for THIS user
	SITE Site;				// Site for the window
	BOOL Changed = false;	// Marker to determine if any changes have been made in the window
};
