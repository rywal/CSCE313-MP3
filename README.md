# Client and Data Server

Simple implementation of a client and data server. When the client is run, a processes is forked off and loads a data server. The two executables communicate to each other via "request channels" which provide a simple inter-process communication pipe. `RequestChannel` interface was provided by the course and was implemented by our group. 

_Created as part of CSCE 313 at Texas A&M University_

Contributors:
* Ryan Walters
* Garrett Haynes
