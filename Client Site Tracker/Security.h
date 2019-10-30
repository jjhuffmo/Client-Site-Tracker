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
public:
	int Resource_Type;				// Type of resource as an int - predefined above
	int Resource_ID;				// Resource ID as identified in the resource tables
	int Min_Security;				// Minimum Security Value - any user access level high than this number will enable it
	//HINSTANCE *hInst;				// Instance pointer
	int mName = 0;					// Menu name via resource pointer
	int result = 0;

	// Update the access rights and label if necessary.  If NewValue == Keep then skip updating the text part
	int Update(HINSTANCE hInst, CString NewValue = "Keep")
	{
		switch (Resource_Type)
		{
			// If it's a Menu Item
		case RES_MENU:
			HMENU hMenu;
			hMenu = LoadMenu(hInst, MAKEINTRESOURCE(mName));
			MENUITEMINFOW ItemDetail = { sizeof(MENUITEMINFOA) };
			//if (GetMenuItemInfo(hMenu, Resource_ID, 0, &ItemDetail))
			//{
				if (Current_User.User_Access >= Min_Security)
				{
					ItemDetail.fState = MFS_DISABLED;
				}
				else
				{
					ItemDetail.fState = MFS_DISABLED;
				}
				ItemDetail.fMask = MIIM_STATE;
				result = SetMenuItemInfoW(hMenu, Resource_ID, 0, &ItemDetail);
				if (NewValue != "Keep")
				{
					ItemDetail.fMask = MIIM_FTYPE | MIIM_STRING;
					//ItemDetail.dwTypeData = (LPWSTR)(LPCWSTR)NewValue;
					ItemDetail.dwTypeData = (LPWSTR) L"Sign Out";
				}
				result = SetMenuItemInfoW(hMenu, Resource_ID, 0, &ItemDetail);
			//}
			//DestroyMenu(hMenu);
			break;
		}
		return result;
	}
	
} ;

Resource_Security MMB_Login;

std::vector<Resource_Security*> All_Security;