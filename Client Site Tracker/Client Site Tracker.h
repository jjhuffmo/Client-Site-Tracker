#pragma once
#include <vector>
#include <string.h>
#include <atlstr.h>
#include <mbstring.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <Windows.h>
#include <tchar.h>
#include <sal.h>
#include <string>
#include <CommCtrl.h>
#include <algorithm>
#include <lmcons.h>
#include "resource.h"
#include "Database Defs.h"

// Buffers and variable identifiers
#define SYSBUFF 256

// Toolbar Commands
#define TB_CREATE		1
#define TB_REFRESH		2

// Status Bar Commands
#define SB_CREATE		1
#define SB_REFRESH		2
#define SB_UPDATE		3

// Primary Status Bar Configuration - Will Be Configurable By The User So Store In Variables

// Primary Status Bar Locations - Defaults
int PSB_SQLStat = 0;			// Database Status (Slot 1)
int PSB_UserStat = 1;			// Active User (Slot 2)
int PSB_Site = 3;				// Site Information (Slot 3)
int PSB_Tickets = 4;			// Tickets Information (Slot 4)
int PSB_Access = 2;				// Current User Access Level (Slot 5)

// All Status Variables for use in status bars, toolbars, tracking, etc.
class Status_Var
{
public:
	CString Prefix;
	CString Value;
	int Type = 0;
	int changed = 1;

	void Change(CString NewValue)
	{
		if (NewValue != Value)
		{
			Value = NewValue;
			changed = 1;
		}
	};
};

class StatusBar
{
private:
	int i = 0;
	int j = 0;
	int k = 0;

public:
	//int parts =1;
	int group_changed = 0;
	std::vector<CString> Sec_Text;
	std::vector<int> changed;
	std::vector<Status_Var*> Sections;

	// Procedure to update the status bar values/text
	void Update_SBar(void)
	{
		//LPARAM holding;
		// If the location is bigger than the vector, initialize the vector properly
		if ((int) Sec_Text.size() < (int)Sections.size())
		{
			Initialize();
		}
		for (i = 0; i < (int) Sections.size(); i++)
		{
			if (Sections[i]->changed)
			{
				//holding = Sections[i]->Prefix + Sections[i]->Value;
				Sec_Text[Sections[i]->Type] = Sections[i]->Prefix + Sections[i]->Value;;
				Sections[i]->changed = 0;
				changed[Sections[i]->Type] = 1;
				group_changed = 1;
			}
		}
	};
	void Initialize(void)
	{
		k = (int)Sections.size() - (int)Sec_Text.size();
		for (j = 0; j <= k; j++)
		{
			// Initialize group with empty text and set to changed.
			//Sec_Text.push_back((LPARAM)L"");
			Sec_Text.push_back("");
			changed.push_back(1);
		}
	};
	
};

std::vector<StatusBar*> All_Status_Bars;
