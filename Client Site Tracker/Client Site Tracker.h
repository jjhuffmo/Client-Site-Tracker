#pragma once

#include "resource.h"
#include "Database Defs.h"
#include <vector>

const wchar_t* USER_NAME = L"None";				// Currently Logged On User
INT USER_ACCESS;				// Currently Logged On User Access Level - For Security/Access Rights
INT SQLSTATUS = 0;				// Connection to SQL status: 0 = Disconnected, 1 = Connected;
WCHAR ACTIVESITE;				// Currently Open Customer Site

// Toolbar Commands
#define TB_CREATE		1
#define TB_REFRESH		2

// Status Bar Commands
#define SB_CREATE		1
#define SB_REFRESH		2
#define SB_UPDATE		3

// Primary Status Bar Configuration - Will Be Configurable By The User So Store In Variables
int PSB_PARTS = 3;				// Define Primary Status Bar Sections To Display

// Primary Status Bar Locations - Defaults
int PSB_DBSTAT = 1;				// Database Status (Slot 1)
int PSB_SITE = 2;				// Site Information (Slot 2)
int PSB_TICKETS = 3;			// Tickets Information (Slot 3)

class STATUSBAR
{
public:
	int parts = 1;
	int i = 0;
	std::vector<LPARAM> Sec_Text;
	void Update_List(const wchar_t *text, int loc)
	{
		if (loc > (int) Sec_Text.size())
		{
			Initialize();
		}
		// Only update when they're different
		if ((LPARAM)text != Sec_Text[loc])
		{
			Sec_Text[loc] = (LPARAM)text;
		}
	}
	void Initialize()
	{
		for (i = 0; i <= parts; i++)
		{
			Sec_Text.push_back((LPARAM)L"");
		}
	}
} MainSBar;

