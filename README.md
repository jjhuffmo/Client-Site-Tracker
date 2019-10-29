# Client-Site-Tracker
First go at creating a client tracking program in C++.  I haven't programmed in C/C++ in 20 years so it's taking some effort
getting back into the groove.  This is also the first win32 program ever so I'm sure it will be sloppy and non-optimized.
But I'm looking to learn as I do this and to figure things out as I go.  If you have any suggestions or recommendations
(on the coding side for now) then please share.  I want to re-learn the correct way of doing these things.

############################
The basic idea is to store various client information into an MSSQL Database.  Users will be able to add, view, modify,
and delete client information based on access level.  
Data to be recorded will be:

1. Site Information (Location, customer, contacts, etc.)
2. Site Resources (Computers, network equipment, specific hardware/software, etc)
3. Assigned Persons (People responsible for that Site)
4. Tickets (Tasks assigned by person and date with flags and notifications)

I'm sure this will grow as I program, but this is more than enough for the time being.

############################

Site_Management.bak is the database file currently configured and can be restored to MSSQL for testing.  Path is hardcoded into the Database Defs header file and can be changed to match your configuration.
