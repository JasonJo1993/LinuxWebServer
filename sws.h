/* Name: Jason Sarwar and Marlon Seaton
 * CS 631
 * Final Project
 * Header File
 * sws.h
 */

#ifndef _SWS_H_
#define _SWS_H_

#ifndef NULL
#define NULL ((void*) 0)
#endif

#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h> /* read, write, close, getopt, execl */
#include <stdlib.h> /* exit */
#include <stdio.h> /* printf, fprintf */
#include <string.h> /* memset, memcpy, strcmp, strncmp, strstr */
#include <strings.h> /* bzero */
#include <sys/types.h> /* open */
#include <sys/stat.h> /* struct stat */
#include <fcntl.h> /* open */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr, struct sockaddr_in */
#include <netdb.h> /* struct hostent */
#include <signal.h> /* signal, SIGINT */
#include <sys/select.h> /* select */
#include <time.h>
#include <errno.h> /* errno, strerror */
#include <libgen.h> /* basename */

#endif
