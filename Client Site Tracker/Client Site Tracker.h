#pragma once

#include "resource.h"
#include "Database Defs.h"
#include <vector>
#include <string.h>
#include <atlstr.h>

// Status Variables (For Status Bars, Toolboxes, Pop-ups, etc)
Status_Var SQLStatus;									// Connection to SQL status: (Disconnected or Connected)
Status_Var User_Name;									// Currently Logged On User
Status_Var Active_Site;									// Currently Open Customer Site
Status_Var Active_Ticket;								// Currently Open Ticket
INT User_Access;										// Currently Logged On User Access Level - For Security/Access Rights


// Toolbar Commands
#define TB_CREATE		1
#define TB_REFRESH		2

// Status Bar Commands
#define SB_CREATE		1
#define SB_REFRESH		2
#define SB_UPDATE		3

// Primary Status Bar Configuration - Will Be Configurable By The User So Store In Variables
int PSB_Parts = 4;				// Define Primary Status Bar Sections To Display

// Primary Status Bar Locations - Defaults
int PSB_SQLStat = 1;			// Database Status (Slot 1)
int PSB_UserStat = 2;			// Active User (Slot 2)
int PSB_Site = 3;				// Site Information (Slot 3)
int PSB_Tickets = 4;			// Tickets Information (Slot 4)

class Status_Var
{
public:
	CString Prefix;
	CString Value;
	int Type;
	int changed = 0;

	void Change(CString NewValue)
	{
		if (NewValue != Value)
		{
			Value = NewValue;
			changed = 1;
		}
	}
};

class STATUSBAR
{
private:
	int i = 0;

public:
	int parts =1;
	int group_changed = 0;
	std::vector<LPARAM> Sec_Text;
	std::vector<int> changed;

	// Procedure to update the status bar values/text
	void Update_SBar(Status_Var Tag)
	{
		LPARAM holding = (LPARAM)(LPCWSTR) Tag.Value;

		// If the location is bigger than the vector, initialize the vector properly
		if (Tag.Type > (int) Sec_Text.size())
		{
			Initialize();
		}
		Sec_Text[Tag.Type] = holding;
		changed[Tag.Type] = 1;
		group_changed = 1;
	}
	void Initialize()
	{
		for (i = 0; i <= parts; i++)
		{
			// Initialize group with empty text and set to changed.
			Sec_Text.push_back((LPARAM)L"");
			changed.push_back(1);
		}
	}
} MainSBar;