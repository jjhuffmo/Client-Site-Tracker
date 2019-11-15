#pragma once

#ifndef SECURITY_H
#define SECURITY_H

#include "Client Site Tracker.h"
#include "Database Defs.h"

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
					if (Location <  Max_Menu)
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
			//HMENU hMenu;
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
} ;

#endif // !SECURITY_H
