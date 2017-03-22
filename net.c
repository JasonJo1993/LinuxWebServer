/* Name: Marlon Seaton and Jason Sarwar
 * CS 631
 * Final Project
 * net.c
 */

#include "sws.h"
#include "functions.h"

#define bufMax 1024

void ErrorMessage(char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

void Server(short port, char* cFlag, char* lFlag, char* iFlag, int dFlag, char* dir)
{
   int sockfd; /*file descriptor for a socket*/
   int clisockfd; /*file descriptor for a client*/
   int n = 0;
   int i = 0;
   int requestval = 0;  /* holds the value of the request*/
   int fdlock = 0;     /*File descriptor for file locking*/
   int release = 0;   
   char requestvalstr[3];
   char buffer[bufMax];
   char clientname[INET_ADDRSTRLEN];
   socklen_t socklength, clilen;
   struct sockaddr_in servaddress;
   struct sockaddr_in cliaddress;
   struct stat statinfo;
   pid_t pid;
   char *response = NULL;
   fd_set set;
   struct timeval timeout;
   int status = 0; //For fork wait, if we are in debugging mode

   char command[bufMax], pathname[bufMax], version[bufMax]; /*Needed to extract the requested file from buffer*/

   struct tm *gtime; 
   time_t now;
   char timeBuf[128];

   FILE *logfile;
   
   if( port < 1 || port > 65536 )
     ErrorMessage("Invalid Port Number");

   sockfd = socket(PF_INET, SOCK_STREAM, 0);
   if(sockfd == -1)
     ErrorMessage("socket failure");
   
   socklength = sizeof(servaddress);

   memset(&servaddress,0, socklength);

   if(iFlag == NULL)
     servaddress.sin_addr.s_addr = INADDR_ANY;
   else
     servaddress.sin_addr.s_addr = inet_addr(iFlag);

   servaddress.sin_family = AF_INET;
   servaddress.sin_port = htons(port);
 
   timeout.tv_sec = 300; //Timeout after 5 minutes
   timeout.tv_usec = 0;

   if((bind(sockfd, (struct sockaddr*)&servaddress, socklength)) < 0)
     ErrorMessage("bind failed");

   if(listen(sockfd, 5) < 0)
     ErrorMessage("listen failed");
   
   if(dFlag == 1)
     printf("\nServer Created, Waiting for Clients...\n");
   
  while(1)
  {
    clilen = sizeof(cliaddress);
    FD_ZERO(&set);
    FD_SET(sockfd, &set);
    n = select(sockfd + 1, &set, NULL, NULL, &timeout);

    if(n == -1)
      ErrorMessage("Select failed");
    else if(n == 0) {
      if( (n = write(1, "522 Connection Timed Out\n", 26)) <= 0)
	ErrorMessage("Server could not timeout\n");
      shutdown(sockfd, 2);
      exit(EXIT_SUCCESS);
    }

    if((clisockfd = accept(sockfd, (struct sockaddr*)&cliaddress, &clilen )) < 0 )
    {
      ErrorMessage("Could not connect to client\n");
    }
    else if(dFlag == 1)
      printf("Accepted connection!\n");


   if((pid = fork()) < 0)
     ErrorMessage("Fork failed\n");
   if(pid == 0)
     break;
   else if(dFlag == 1)
     wait(&status);
 }

  FD_ZERO(&set);
  FD_SET(clisockfd, &set);

  n = select(clisockfd + 1, &set, NULL, NULL, &timeout);
  if(n == -1)
    ErrorMessage("Select failed");
  else if(n == 0) {
    if( (n = write(clisockfd, "408 Connection Timed Out\n", 26)) <= 0)
      ErrorMessage("Server could not send timeout\n");
    shutdown(clisockfd, 2);
    exit(EXIT_SUCCESS);
  }
  else {
    bzero(buffer, bufMax);
    if((n = read(clisockfd, buffer, bufMax - 1)) < 0)
      ErrorMessage("Could not read from client\n");
    
    buffer[n] = '\0';

    if(dFlag == 1)
      printf("Request Received: %s\n", buffer);
    
    response = Request(buffer, cFlag, dir, clisockfd);

    /*
    if( (n = write(clisockfd, response, strlen(response))) <= 0)
      ErrorMessage("Server could not make response\n");
    */
    if(lFlag != NULL)
      {
	if(getpeername(clisockfd, (struct sockaddr*)&cliaddress, &clilen) < 0)
	  ErrorMessage("getpeername() failed");
        
	if(dFlag == 0) {
	  logfile = fopen(lFlag, "a+");
	  if(logfile == NULL)
	    ErrorMessage("fopen failed");
	}
	else
	  logfile = stdout;

	if(dFlag == 0) {
	  fdlock = flock(fileno(logfile), LOCK_SH);
	  if(fdlock < 0)
	    ErrorMessage("Lock failed");
	}

	if(inet_ntop(AF_INET,&(cliaddress.sin_addr), clientname, INET_ADDRSTRLEN) < 0)
	  ErrorMessage("inet_ntop failed");
	
	fprintf(logfile,"%s ", clientname);

       time(&now);
       gtime = gmtime(&now);
       if( (strftime(timeBuf, 128, "%a, %d %b %y %H:%M:%S %Z", gtime)) <= 0)
         ErrorMessage("strftime failed\n");


	fprintf(logfile, "%s ", timeBuf);
	fprintf(logfile, "%s " , buffer);

	/*Get the status value of the request*/
	while(*response)
	{
	  if(isdigit(*response))
	  {
	    requestvalstr[i] = *response;
	    i++;
	    if(i == 3)  /* Status value is a 3-digit value*/
	      break;
	  }
	  response++;
	}
        
	requestval = atoi(requestvalstr);
	fprintf(logfile, "%d ", requestval);

	if(requestval == 200) {

	  /*Extract the pathname from buffer*/
	  if(sscanf(buffer,"%s %s %s", command, pathname, version) != 3)
	    return;
	
	  n = strlen(dir);
	  for(i = strlen(pathname) + n; i > 0; i--)
	    pathname[i] = pathname[i - n];
	  
	  for(i = 0; i < n; i++)
	    pathname[i] = dir[i];

	  if(stat(pathname, &statinfo) < 0)
	    ErrorMessage("Stat failed");

	  fprintf(logfile," %ld ",statinfo.st_size);
	}
	else
	  fprintf(logfile," %i ", 0);

	fprintf(logfile, "\n");

	if(dFlag == 0) {
	  release = flock(fdlock, LOCK_UN);
	  if(release < 0)
	    ErrorMessage("Could not unlock");

	  if(fclose(logfile) != 0)
	    ErrorMessage("fclose failed");

	}

      }
    
  }
  
  shutdown(clisockfd, 2);
  close(sockfd);
  exit(EXIT_SUCCESS);

}/*End Server*/

