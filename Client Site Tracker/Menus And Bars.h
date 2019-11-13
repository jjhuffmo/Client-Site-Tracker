#pragma once
#ifndef MENUS_H
#define MENUS_H

#include "Client Site Tracker.h"

// Toolbar Commands
#define TB_CREATE		1
#define TB_REFRESH		2

// Status Bar Commands
#define SB_CREATE		1
#define SB_REFRESH		2
#define SB_UPDATE		3

// All Status Variables for use in status bars, toolbars, tracking, etc.
class Status_Var
{
public:
	CString Prefix;
	CString Value;
	INT Type = 0;
	INT changed = 1;

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
		if (Sec_Text.size() < Sections.size())
		{
			Initialize();
		}
		for (i = 0; i < INT{ Sections.size() }; i++)
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


// External variables
extern StatusBar MainSBar;
extern Status_Var Status_SQL;
extern Status_Var Status_User;
extern Status_Var Status_Site;
extern Status_Var Status_Tickets;
extern Status_Var Status_Access;

extern int SBars_Init;
extern int PSB_SQLStat;			
extern int PSB_UserStat;			
extern int PSB_Site;				
extern int PSB_Tickets;			
extern int PSB_Access;
extern Resource_Security MMB_Login, MMB_MySites, MMB_Sys_Management, MMB_MyTickets;

extern std::vector<Resource_Security*> All_Security;
extern std::vector<StatusBar*> All_Status_Bars;
extern HINSTANCE hInst;            

// External Procedures/Functions
extern void Validate_Security(HWND hWnd);
void ManageToolBar(HWND hWndParent, HINSTANCE hInst, int hMenu, int Action);
void ManageStatusBar(HWND hWndParent, HINSTANCE hInst, int hMenu, int Action, StatusBar* sbVals);

#endif // !MENUS_H