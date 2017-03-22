/* Name: Jason Sarwar and Marlon Seaton
 * CS 631
 * Final Project
 * main.c
 */

#include "sws.h"
#include "functions.h"

int main(int argc, char **argv) {

  char *cFlag = NULL; // Arg -c dir
  int dFlag = 0; // Arg -d
  int hFlag = 0; // Arg -h
  char *iFlag = NULL; // Arg -i address
  char *lFlag = NULL; // Arg -l file
  short port = 8080; // Arg -p port
  char *dir = NULL;

  int option = 0; // For Switch Case and getopt
  int x = 0; //Multi-purpose Integer

  if(argc < 2) {
    printf("usage: sws [-dh] [-c dir] [-i address] [-l file] [-p port] dir\n");
    return(EXIT_FAILURE);
  }
  
  x = 1;
  while(argv[x] != NULL) {
    if(argv[x][0] != '-' && argv[x - 1][0] != '-') {
      if(dir != NULL) {	
	printf("Invalid argument: %s\n", argv[x]);
	printf("usage: sws [-dh] [-c dir] [-i address] [-l file] [-p port] dir\n");
	return(EXIT_FAILURE);
      }
      else
	dir = argv[x];
    }
    x++;
  }

  if(dir == NULL) {
    printf("Directory not given\n");
    printf("usage: sws [-dh] [-c dir] [-i address] [-l file] [-p port] dir\n");
    return EXIT_FAILURE;
  }


  while ((option = getopt(argc, argv, "c:dhi:l:p:")) != -1) {
    switch(option) {
    case 'c':
      cFlag = optarg;
      if(cFlag == NULL) {
	printf("-c requires an argument");
	printf("usage: sws [-dh] [-c dir] [-i address] [-l file] [-p port] dir\n");
	return EXIT_FAILURE;
      }
      break;
    case 'd':
      dFlag = 1;
      break;
    case 'h':
      hFlag = 1;
      break;
    case 'i':
      iFlag = optarg;
      if(iFlag == NULL) {
	printf("-i requires an argument");
	printf("usage: sws [-dh] [-c dir] [-i address] [-l file] [-p port] dir\n");
	return EXIT_FAILURE;
      }
      break;
    case 'l':
      lFlag = optarg;
      if(lFlag == NULL) {
	printf("-l requires an argument");
	printf("usage: sws [-dh] [-c dir] [-i address] [-l file] [-p port] dir\n");
	return EXIT_FAILURE;
      }
      break;
    case 'p':
      if((port = atoi(optarg)) <= 0 || port > 65536) {
	printf("Invalid port number: %s\n", optarg);
	return EXIT_FAILURE;
      }
      break;
    case '?':
      //fprintf(stderr, "sws: invalid option -- '%c'\n", optopt);
      return 3;
    default:
      printf("sws: internal error\n");
      return 4;
    }
  }

  if(hFlag == 1) {
    printf("usage: sws [-dh] [-c dir] [-i address] [-l file] [-p port] dir\n");
    printf("-c dir     Allow execution of CGIs from the given directory.\n");
    printf("-d         Enter debugging mode. Does not daemonize, only accepts one connection at a time and enables logging to stdout.\n");
    printf("-h         Print a short usage summary and exit.\n");
    printf("-i address Bind to the given IPv4 or IPv6 address. If not provided, sws will listen on all IPv4 and IPv6 addresses on this host.\n");
    printf("-l file    Log all requests to the given file.\n");
    printf("-p port    Listen on the given port. If not provided, sws will listen on post 8080.\n");
    return EXIT_SUCCESS;
  }


  if(dFlag == 0)
    daemon(1, 0);  

  Server(port, cFlag, lFlag, iFlag, dFlag, dir);

  return 0;
}
