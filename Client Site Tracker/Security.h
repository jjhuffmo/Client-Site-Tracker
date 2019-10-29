#pragma once

#include <atlstr.h>
#include "resource.h"

// Define all the security levels for main program settings - valid ranges are 0-9999
// The indivdual site, ticket, asset, etc. security levels will be set by object

#define SYSTEM_USER		1000	// Minimum to view items
#define SITE_EDITOR		5000	// Minimum to create sites
#define	SYSTEM_ADMIN	9000	// Minimum to change about everything

class
{
public:
	int Resource_Type;
	int Resource_ID;
	int Min_Security;

	void Update_Resource()
	{
		switch (Resource_Type)
		{
		}
	}
} Resource_Security;