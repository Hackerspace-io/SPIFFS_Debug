#include <SPIFFS_Debug.h>

SPIFFS_Debug Debugger;    // makes an object from SPIFFS_Debug class.
int counter = 0;

unsigned long last_time = 0;

void func1();
void func2();
void func3();
void func4();

void setup() {

  Date date;    // "Date" type is defined in SPIFFS_Debug.h
  date.year = 1397;
  date.month = 5;
  date.day = 25;

  Time time;    // "Time" type is defined in SPIFFS_Debug.h
  time.hour = 14;
  time.min = 46;
  time.sec = 7;

  Serial.begin(115200);
  Serial.println("Start of Sketch");

  Debugger.rstDebugFiles();   // for the first time running this program, uncomment this line to make necessary files, otherwise comment it
  Debugger.debugOnRst(date, time);    // every time you reset your ESP, it will write date and time in 'debug2' file

  // testing nested debug
  Debugger.debug("A_1\n");  // writes "A_1" inside of 'debug1' file.
  func1();
  Debugger.debug("A_2\n");

}

void loop() {

  if (Serial.available()) {
    char input = Serial.read();
    if (input == 'r') {
      Debugger.rstDebugFiles();   // makes necessary files empty. if not exist, creates them.
    }
    if (input == 'w') {   // writes 100 points in 'debug1' file.
      int ms = millis();
      for (int i = 0 ; i < 100 ; i++) {
        String debugString = "Point_";
        debugString += counter;
        debugString += "\n";
        Debugger.debug(debugString);
        ++counter;
      } Serial.print("Avg time: ");
      Serial.println((millis() - ms) / 100);
    }
    if (input == 'a') {
      Debugger.debugReadDebug1();   // reads 'debug1' file.
    }
    if (input == 'b') {
      Debugger.debugReadDebug2();   // reads 'debug2' file.
    }
    if (input == 'c') {
      Debugger.debugReadDebugConf();  // reads 'debugConf' file. (for testing process)
    }
    while (Serial.available()) {
      Serial.read();
    }
  }

}

void func1() {
  Debugger.debugSOF("C_1\n");   // use SOF at the beginning of every func to make debug structure nested.
  Serial.println("we are inside of func1");
  Debugger.debug("C_2\n");      // now u can mark every point of func like this.
  func2();
  Debugger.debugEOF("C_3\n");   // use EOF at the end of every func to make debug structure nested.
}

void func2() {
  Debugger.debugSOF("D_1\n");
  Serial.println("we are inside of func2");
  Debugger.debug("D_2\n");
  func3();
  Debugger.debug("D_3\n");
  func4();
  Debugger.debugEOF("D_4\n");
}

void func3() {
  Debugger.debugSOF("E_1\n");
  Serial.println("we are inside of func3");
  Debugger.debug("E_2\n");
  Debugger.debugEOF("E_3\n");
}

void func4() {
  Debugger.debugSOF("F_1\n");
  Serial.println("we are inside of func4");
  Debugger.debug("F_2\n");
  Debugger.debugEOF("F_3\n");
}
