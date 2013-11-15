AppClose - Windows console application which gracefully closes other application
           by sending close message (WM_CLOSE) to its window.

USAGE:
  AppClose.exe 1234
  AppClose.exe x:\path\app.pid

  where 
    1234            - process id (PID)
    x:\path\app.pid - path to a file which contains 1234


NOTES:
  * Application must have visible window (or console, i.e. launched by cmd /c app_to_kill)
  * PID can be main or child process id
  * tested by closing Apache server which was launched from command-line:
      * "start Apache2.2/bin/httpd.exe"
      * "Apache2.2/bin/httpd.exe -k stop|shutdown"  does not work because in this case Apache is not a service
      * "AppClose.exe x:\path\httpd.pid" will close Apache gracefully
  * can be compile using Visual C++ 6.0
  
(C) Gediminas Lūžys, 2013
