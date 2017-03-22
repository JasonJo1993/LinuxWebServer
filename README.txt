Names: Jason Sarwar and Marlon Seaton
CS 631
Final Project - SWS Milestone 1
HW 3
November 7, 2016

The files in this submission are main.c, net.c, http.c, client.c, and Makefile.
The Makefile produces an executable called sws and another executable called client.
sws can accept all of the command-line options like the man page. However, only the 
-p flag works. It is used to set the port for the server. If not set, the port will be
8080. Once sws starts running, it will wait for connections in the foreground. We have
also included a Linux Client executable called client. It takes two arguments: the 
server hostname (such as eva, avalon, avatar, rainman, etc.) and port number.

Usage:
./sws -p <port number>
./client <host> <port number>
