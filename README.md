# SPIFFS_Debug
you can log the routine of program in SPIFFS area of ESP8266

logging the routine in SPIFFS area helps developers to find the problem of their program when they let it to run for hours or days.
for example a common problem of most products is that they hangs or restarts after some hours or days without any reason. this library helps them to findout where and when the problem occured.

it contains 3 files in SPIFFS area:
- 'debug1.txt' file that saves the log with 'debug' or 'debugSOF' or 'debugEOF' functions.
- 'debug2.txt' file which saves the last 'lastMsgCount' Bytes of 'debug1.txt' file when 'debugOnRst' is called. usally at the beginning of program
- 'debugConf.txt' file which contains some necessary informations for working with this library. but user doesn't need it.

u can use Test_Functions example to findout how to use functions.
