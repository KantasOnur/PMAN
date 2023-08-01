# PMan 
This program allows multiple processes to run in the background, lists various information about each process run by the program, and finally allows the user to modify the program's state. 

## Compile and Run
Type "make" to compile and "./PMan" to run.

## Commands
- bg [exe] -> runs a background process 
- bglist -> lists the background processes their names and pids.
- bgkill [pid] -> terminates the background process. 
- bgstop [pid] -> stops the background process. 
- bgstart [pid] -> restarts the already stopped background process. 
- pstat [pid] -> lists comm, state, utime, stime, rss, voluntary_ctxt_switches and nonvoluntary_ctxt_switches for the process.
