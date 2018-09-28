/*
  SPIFFS_Debug.cpp - library for communicating to Server and saving DebugLog in SPIFFS.
  Created by amin ;)
  and its Free
*/

#include "Arduino.h"
#include "SPIFFS_Debug.h"

#define debugConf "/debugConf.txt"
#define debug1	"/debug1.txt"
#define debug2	"/debug2.txt"
#define debug1size	2000		//max size about 300kB
#define debug2size	2000		//max size about 300kB
#define lastMsgCount	200		//number of messages that should be saved in debug2

// debugConf contains: 1 byte debug1Full flag + 3 byte debug1StartPointer + 1 byte debug2Full flag + 3 byte debug2StartPointer
// debug1 contains full report
// debug2 contains RTC and last lastMsgCount lines before restart

File fileConf, file1, file2;

int SPIFFS_Debug::readAddrs(File file, int addrs) {
	file.seek(addrs, SeekSet);
	return file.read();
}

void SPIFFS_Debug::writeAddrs(File file, int addrs, int value) {
	file.seek(addrs, SeekSet);
	file.write(value);
}

SPIFFS_Debug::SPIFFS_Debug() {
	
	#ifdef Dragon_debug_spiffs
	if (SPIFFS.begin()) {
		Serial.println("mounted file system");
	} 
	else {
		Serial.println("failed to mount FS");
	}
	#endif
}

bool SPIFFS_Debug::rstDebugFiles() {
	bool result1 = SPIFFS.remove(debug1);
	bool result2 = SPIFFS.remove(debug2);
	bool result3 = SPIFFS.remove(debugConf);
	file1 = SPIFFS.open(debug1, "w+");		// creating new file for further usage
	file1.close();
	file2 = SPIFFS.open(debug2, "w+");		// creating new file for further usage
	file2.close();
	fileConf = SPIFFS.open(debugConf, "w+");		// creating new file for further usage
	if (fileConf) {
		for (int i=0 ; i<8 ; i++) {		// Init fileConf
			writeAddrs(fileConf, i, 0);
		}
	}
	else {
		Serial.println("fileConf open failed");
	}
	fileConf.close();
	if (result1 && result2 && result3 == true) {
		return true;
	}
	else {
		return false;
	}
}

void SPIFFS_Debug::debugOnRst(Date date, Time time) {
	#ifdef Dragon_debug_spiffs
	file1 = SPIFFS.open(debug1, "r");
	file2 = SPIFFS.open(debug2, "r+");
	fileConf = SPIFFS.open(debugConf, "r");
	if (file1 && file2 && fileConf) {
		int confStartPoint;
		int startPoint;
		if (file1.size() < debug1size) {	// just read the last 20 messages
			confStartPoint = file1.size();
			startPoint = confStartPoint-lastMsgCount;
			if (startPoint < 0) {
				startPoint = 0;
			}
		}
		else {
			confStartPoint = readAddrs(fileConf, 3)*(256^2) + readAddrs(fileConf, 2)*256 + readAddrs(fileConf, 1);
			startPoint = confStartPoint-lastMsgCount;
			if (startPoint < 0) {
				startPoint = 0;
			}
		}
		String comment = "";
		comment += "\nTime: ";
		comment += date.year;
		comment += "-";
		comment += date.month;
		comment += "-";
		comment += date.day;
		comment += "\t";
		comment += time.hour;
		comment += ":";
		comment += time.min;
		comment += ":";
		comment += time.sec;
		comment += "\n";
		file1.seek(startPoint, SeekSet);
		
		while (file1.position() < confStartPoint) {
			comment += (char)file1.read();
			yield();
		}
		
		if (file2.size() < debug2size) {		// we have space to write
			file2.seek(file2.size(), SeekSet);
			file2.print(comment);
		}
		else {	// file2 is full.
			fileConf = SPIFFS.open(debugConf, "r+");
			if (fileConf) {
				if (readAddrs(fileConf, 0) == 0) {
					writeAddrs(fileConf, 0, 1);
				}
				int startPoint = readAddrs(fileConf, 7)*(65536) + readAddrs(fileConf, 6)*256 + readAddrs(fileConf, 5);
				file2.seek(startPoint, SeekSet);
				file2.print(comment);
				startPoint = file2.position();
				if (startPoint > file2.size()) {
					while (file2.available()) {		// empty extra datas over useful length
						file2.write(0);
					}
					startPoint = 0;
				}
				//write new startPoint to fileConf
				writeAddrs(fileConf, 7, startPoint/(65536));
				writeAddrs(fileConf, 6, (startPoint/256)%256);
				writeAddrs(fileConf, 5, startPoint%256);
			}
			else {
				Serial.println("fileConf open failed");
			}
		}
	}
	else {
		Serial.println("file1 open failed");
	}
	file1.close();
	file2.close();
	fileConf.close();
	#endif
}

void SPIFFS_Debug::debug(String comment) {
	#ifdef Dragon_debug_serial
	Serial.print("*DD* ");
	Serial.print(comment);
	#endif
	
	#ifdef Dragon_debug_spiffs
	// adding spaces
	String commentLog = "";
	for (int i=0 ; i<spaces ; i++) {
		commentLog += " ";
	}
	commentLog += comment;
	file1 = SPIFFS.open(debug1, "r+");
	if (file1) {
		if (file1.size() < debug1size) {		// we have space to write
			file1.seek(file1.size(), SeekSet);
			file1.print(commentLog);
		}
		else {	// file1 is full.
			fileConf = SPIFFS.open(debugConf, "r+");
			if (fileConf) {
				if (readAddrs(fileConf, 0) == 0) {
					writeAddrs(fileConf, 0, 1);
				}
				int startPoint = readAddrs(fileConf, 3)*(65536) + readAddrs(fileConf, 2)*256 + readAddrs(fileConf, 1);
				file1.seek(startPoint, SeekSet);
				file1.print(commentLog);
				startPoint = file1.position();
				if (startPoint > debug1size) {
					while (file1.available()) {		// empty extra datas over useful length
						file1.write(0);
					}
					startPoint = 0;
				}
				//write new startPoint to fileConf
				writeAddrs(fileConf, 3, startPoint/(65536));
				writeAddrs(fileConf, 2, (startPoint/256)%256);
				writeAddrs(fileConf, 1, startPoint%256);
			}
			else {
				Serial.println("fileConf open failed");
			}
			fileConf.close();
		}
	}
	else {
		Serial.println("file1 open failed");
	}
	file1.close();
	#endif
}

void SPIFFS_Debug::debugSOF(String comment) {
	++spaces;
	debug(comment);
}

void SPIFFS_Debug::debugEOF(String comment) {
	if (spaces < 0) {
		spaces = 0;
	}
	debug(comment);
	--spaces;
}

void SPIFFS_Debug::debugReadDebug1() {
	file1 = SPIFFS.open(debug1, "r");
	if (file1) {
		fileConf = SPIFFS.open(debugConf, "r");
		int startPoint = readAddrs(fileConf, 3)*(256^2) + readAddrs(fileConf, 2)*256 + readAddrs(fileConf, 1);
		file1.seek(startPoint, SeekSet);
		
		Serial.print("file size is: ");
		Serial.println(file1.size());
		Serial.println("====== Reading from debug1 file =======");
		while (file1.available()){
			String lineContent = file1.readStringUntil('\n');
			Serial.println(lineContent);
		}
		file1.seek(0, SeekSet);
		while (file1.position() < startPoint){
			String lineContent = file1.readStringUntil('\n');
			Serial.println(lineContent);
		}
		Serial.println("======       end of file        =======");
	}
	else {
		Serial.println("file open failed");
	}
	file1.close();
}

void SPIFFS_Debug::debugReadDebug2() {
	file2 = SPIFFS.open(debug2, "r");
	if (file2) {
		fileConf = SPIFFS.open(debugConf, "r");
		int startPoint = readAddrs(fileConf, 7)*(256^2) + readAddrs(fileConf, 6)*256 + readAddrs(fileConf, 5);
		file1.seek(startPoint, SeekSet);
		
		Serial.print("file size is: ");
		Serial.println(file2.size());
		Serial.println("====== Reading from debug2 file =======");
		while (file2.available()){
			String lineContent = file2.readStringUntil('\n');
			Serial.println(lineContent);
		}
		file1.seek(0, SeekSet);
		while (file2.position() < startPoint){
			String lineContent = file2.readStringUntil('\n');
			Serial.println(lineContent);
		}
		Serial.println("======       end of file        =======");
	}
	else {
		Serial.println("file open failed");
	}
	file2.close();
}

void SPIFFS_Debug::debugReadDebugConf() {
	fileConf = SPIFFS.open(debugConf, "r");
	if (fileConf) {
		Serial.println("====== Reading from SPIFFS file =======");
		int readedByte;
		int pointer = 0;
		while (fileConf.available()){
			readedByte = fileConf.read();
			Serial.print(pointer);
			Serial.print(":\t");
			Serial.println(readedByte);
			++pointer;
		}
		Serial.print("\n");
	}
	else {
		Serial.println("fileConf open failed");
	}
	fileConf.close();
}