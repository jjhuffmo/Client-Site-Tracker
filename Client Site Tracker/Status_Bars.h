#pragma once

#include <atlstr.h>
#include <vector>
#include "Client Site Tracker.h"

// Define status bars here so they can be changed without parsing through logic
// Remember to update the Init_Status_Bars section in Client Site Tracker.cpp to identify new ones

// Main Status Bar
STATUSBAR MainSBar;
Status_Var Status_SQL;
Status_Var Status_User;
Status_Var Status_Site;
Status_Var Status_Tickets;
All_Project_Status_Bars All_Status_Bars;