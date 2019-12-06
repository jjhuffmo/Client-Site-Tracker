#pragma once
#ifndef CLIENT_H
#define CLIENT_H

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
#include <sql.h>
#include <sqlext.h>
#include <sql.h>
#include <sqlext.h>
#include <memory>
#include "framework.h"
#include "resource.h"

#include "Classes_Structs.h"
#include "Database Defs.h"
#include "Security.h"
#include "Menus And Bars.h"

// Variables and functions
ATOM                MyRegisterClass(HINSTANCE hInstance) noexcept;
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    ChildWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    LoginPU(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	List_Sites(HWND, UINT, WPARAM, LPARAM);

// Forward declarations of functions included in this code module:
void InitStatusBars(void);
void UpdateStatus(HWND hWnd);
int CheckUser(CString New_User);
void Validate_Security(HWND hWnd);
void SetSecurity(void);
BOOL Show_Sites(int SiteID);
BOOL Load_Site_Window(HWND hWnd, INT SiteID, INT UserID);

// Buffers and variable identifiers
#define SYSBUFF 256



#endif // !CLIENT_H
