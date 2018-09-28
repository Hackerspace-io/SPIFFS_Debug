/*
  SPIFFS_Debug.h - library for communicating to Server and saving DebugLog in SPIFFS.
  Created by amin ;)
  and its Free
*/
#ifndef SPIFFS_Debug_H
#define SPIFFS_Debug_H

//#define SPIFFS_Debug_serial
//#define SPIFFS_Debug_spiffs

#include <FS.h>		//It seems that it should be first
#include "Arduino.h"

struct Date {
	int year;
	int month;
	int day;
};

struct Time {
	int hour;
	int min;
	int sec;
};

class SPIFFS_Debug
{
  private:
    int readAddrs(File file, int addrs);
    void writeAddrs(File file, int addrs, int value);
	int spaces = 0;
	
  public:
	SPIFFS_Debug();
	
	bool rstDebugFiles();
	void debugOnRst(Date date, Time time);
	void debug(String comment);
	void debugSOF(String comment);	// for Start of function
	void debugEOF(String comment);	// for End of function
	void debugReadDebug1();
	void debugReadDebug2();
	void debugReadDebugConf();
	void debugAddSpace();
	void debugRemoveSpace();
  
	
};

#endif