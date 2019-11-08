#pragma once

#include <atlstr.h>
#include "resource.h"
#include <vector>
#include <WinUser.h>

// Define all the security levels for main program settings - valid ranges are 0-9999
// The indivdual site, ticket, asset, etc. security levels will be set by object

#define SYSTEM_USER		1000	// Minimum to view items
#define SITE_EDITOR		5000	// Minimum to create sites
#define	SYSTEM_ADMIN	9000	// Minimum to change about everything

// Define resource types for identifying control and security
#define RES_MENU		1		// Entire Menu
#define RES_MENUITEM	2		// Single item on menu

class Resource_Security
{
private:
	int result = 0;
	int i = 0;
	int found = 0;
	HMENU hMenu, NewMenu;
	MENUITEMINFO MenuItem = { sizeof(LPMENUITEMINFO) };
	LPWSTR entry = NULL;

public:
	int Resource_Type = 0;				// Type of resource as an int - predefined above
	UINT Resource_ID = 0;				// Resource ID as identified in the resource tables
	int Min_Security = 0;				// Minimum Security Value - any user access level high than this number will enable it
	int Location = 0;					// Location on a menu for menu resources
	CString Label = "Default";

	// Update the access rights and label if necessary. 
	// NewValue means different things to different resources:
	//		RES_MENU - The title of the pop-up menu to see if it exists or to give it a title when created
	//		RES_MENUITEM - Update the menu text or if NewValue == Keep then skip updating the text part
	int Update(HINSTANCE hInst, HWND hWnd, CString NewValue = "Keep")
	{
		found = 0;
		switch (Resource_Type)
		{
		// If it's a main menu item
		case RES_MENU:
			hMenu = GetMenu(hWnd);
			MenuItem.fMask = MIIM_STRING | MIIM_ID;
			MenuItem.fType = MIIM_STRING;
			MenuItem.cbSize = sizeof(MENUITEMINFOW);

			// Check to see if the Menu is visible, if not then add it. 
			for (i = 0; i <= GetMenuItemCount(hMenu)-1; i++)
			{
				MenuItem.dwTypeData = NULL;
				result = GetMenuItemInfoW(hMenu, i, true, &MenuItem);
				if (sizeof(entry) < MenuItem.cch++)
				{
					entry = (LPWSTR)malloc(MenuItem.cch++);
				}
				MenuItem.dwTypeData = entry;
				MenuItem.cch++;
				result = GetMenuItemInfoW(hMenu, i, true, &MenuItem);
				if ((CString)entry == Label)
				{
					found = i;
				}
			}
			if (Current_User.User_Access >= Min_Security)  // Valid security to see
			{
				// If it doesn't exist, insert it in the proper spot
				if (found == 0)	
				{
					NewMenu = LoadMenu(hInst, MAKEINTRESOURCE(Resource_ID));
					if (Location >  GetMenuItemCount(hMenu) -1)
					{
						result = InsertMenuW(hMenu, GetMenuItemCount(hMenu) - 1, MF_BYPOSITION | MF_POPUP, (UINT_PTR)NewMenu, Label);
					}
					else
					{
						result = InsertMenuW(hMenu, Location, MF_BYPOSITION | MF_POPUP, (UINT_PTR)NewMenu, Label);
					}
					DestroyMenu(NewMenu);
				}
			}
			else
			{
				if (found > 0)
				{
					result = DeleteMenu(hMenu, found, MF_BYPOSITION);
					DestroyMenu(NewMenu);
				}
			}
			//free(entry);
			//DrawMenuBar(hWnd);
			break;

		// If it's a Menu Item
		case RES_MENUITEM:
			HMENU hMenu;
			hMenu = GetMenu(hWnd);
			if (Current_User.User_Access >= Min_Security)
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
} ;

Resource_Security MMB_Login, MMB_MySites, MMB_Sys_Management, MMB_MyTickets;

std::vector<Resource_Security*> All_Security;