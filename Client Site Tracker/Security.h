#pragma once

#include <atlstr.h>
#include "resource.h"
#include <vector>

// Define all the security levels for main program settings - valid ranges are 0-9999
// The indivdual site, ticket, asset, etc. security levels will be set by object

#define SYSTEM_USER		1000	// Minimum to view items
#define SITE_EDITOR		5000	// Minimum to create sites
#define	SYSTEM_ADMIN	9000	// Minimum to change about everything

// Define resource types for identifying control and security
#define RES_MENU	1

class Resource_Security
{
private:
	int result = 0;

public:
	int Resource_Type = 0;				// Type of resource as an int - predefined above
	int Resource_ID = 0;				// Resource ID as identified in the resource tables
	int Min_Security = 0;				// Minimum Security Value - any user access level high than this number will enable it

	// Update the access rights and label if necessary.  If NewValue == Keep then skip updating the text part
	int Update(HWND hWnd, CString NewValue = "Keep")
	{
		switch (Resource_Type)
		{
			// If it's a Menu Item
		case RES_MENU:
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
				result = ModifyMenu(hMenu, Resource_ID, MF_BYCOMMAND | MF_STRING, NULL, (LPCTSTR)NewValue);
			}
			break;
		}
		return result;
	}
} ;

Resource_Security MMB_Login, MMB_MySites;

std::vector<Resource_Security*> All_Security;