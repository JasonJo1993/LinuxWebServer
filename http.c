/* Name: Marlon Seaton and Jason Sarwar
 * CS 631
 * Final Project
 * http.c
 */

#include "sws.h"
#include "functions.h"

char* Not200ok(char* status, int fd) {

  struct tm *gtime; 
  time_t now;
  char timeBuf[128];

  if( (write(fd, "HTTP/1.0 ", 9)) <= 0)
    ErrorMessage("Write response to client failed\n");

  if( (write(fd, status, strlen(status))) <= 0)
    ErrorMessage("Write response to client failed\n");

  if( (write(fd, "\nDate: ", 7)) <= 0)
    ErrorMessage("Write response to client failed\n");

  time(&now);
  gtime = gmtime(&now);
  if( (strftime(timeBuf, 128, "%a, %d %b %y %H:%M:%S %Z", gtime)) <= 0)
    ErrorMessage("strftime failed\n");
 
  if( (write(fd, timeBuf, strlen(timeBuf))) <= 0)
    ErrorMessage("Write response to client failed\n");

  if( (write(fd, "\nServer: sws/1.0", 16)) <= 0)
    ErrorMessage("Write response to client failed\n");

  if( (write(fd, "\nContent-Type: Nothing returned\n", 32)) <= 0)
    ErrorMessage("Write response to client failed\n");

  if( (write(fd, "Content-Length: 0\n", 18)) <= 0)
    ErrorMessage("Write response to client failed\n");

  return status;

}


char* Supported(char* pathname, char* cFlag, char* dir, int fd, char* command)
{

  char* ret = NULL;
  char* fullpath = NULL;
  char* fullpathWithIndex = NULL;
  int stdoutBackup = dup(STDOUT_FILENO);
  int stderrBackup = dup(STDERR_FILENO);

  struct stat statinfo;
  struct tm *gtime;
  time_t now;
  char timeBuf[128];
  char fileBuf[128];

  int n = 100;
  int reqFile = 0;

  n = strncasecmp(pathname, "/cgi-bin/", 9);

  if(n == 0 && cFlag != NULL)
  {

    fullpath = (char*)malloc(sizeof(char) * (strlen(cFlag) + strlen(&pathname[9]) + 2));
    strcat(fullpath, cFlag);
    strcat(fullpath, "/");
    strcat(fullpath, &pathname[9]);

    if(stat(fullpath, &statinfo) != 0)
    {
      free(fullpath);
      return Not200ok("404 Not Found", fd);
    }

    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    
    if(!S_ISDIR(statinfo.st_mode)) {
      Not200ok("200 OK", fd);
      if((execl(fullpath, fullpath, NULL )) < 0)  
	return Not200ok("500 Internal Server Error", fd);
    }
    else {
      return Not200ok("404 Not Found", fd);
    }

    dup2(stdoutBackup, STDOUT_FILENO);
    dup2(stderrBackup, STDERR_FILENO);

  }/*End outer if statement*/
  else
  {

    fullpath = (char*)malloc(sizeof(char) * (strlen(dir) + strlen(pathname) + 2));
    strcat(fullpath, dir);
    strcat(fullpath, "/");
    strcat(fullpath, pathname);

    if(stat(fullpath, &statinfo) != 0)
    {
      free(fullpath);
      return Not200ok("404 Not Found", fd);
    }
    else {
      if(!S_ISDIR(statinfo.st_mode)) {
	reqFile = open(fullpath, O_RDONLY);

      }
      else {
	
      fullpathWithIndex = (char*)malloc(sizeof(char) * (strlen(fullpath) + 12)  );
      strcat(fullpathWithIndex, fullpath);
      strcat(fullpathWithIndex, "/index.html");
      
      if(stat(fullpathWithIndex, &statinfo) != 0) {
	// ls command with the fullpath directory
       dup2(fd, STDOUT_FILENO);
       dup2(fd, STDERR_FILENO);

	if((execl("/bin/ls", "ls", "-1", fullpath, NULL)) < 0)
	  return Not200ok("500 Internal Server Error", fd);

       dup2(stdoutBackup, STDOUT_FILENO);
       dup2(stderrBackup, STDERR_FILENO);

      }
      else {
  	 reqFile = open(fullpathWithIndex, O_RDONLY);

      }

      free(fullpathWithIndex);

      }
      

  }


  }

  if(reqFile != 0) {

	if( (write(fd, "HTTP/1.0 ", 9)) <= 0)
	  return Not200ok("500 Internal Server Error", fd);
	
	if( (write(fd, "200 OK", 6)) <= 0)
	  return Not200ok("500 Internal Server Error", fd);

	if( (write(fd, "\nDate: ", 7)) <= 0)
	  return Not200ok("500 Internal Server Error", fd);
	
	time(&now);
	gtime = gmtime(&now);
	if( (strftime(timeBuf, 128, "%a, %d %b %y %H:%M:%S %Z", gtime)) <= 0)
	  return Not200ok("500 Internal Server Error", fd);
	
	if( (write(fd, timeBuf, strlen(timeBuf))) <= 0)
	  return Not200ok("500 Internal Server Error", fd);
	
	if( (write(fd, "\nServer: sws/1.0", 16)) <= 0)
	  return Not200ok("500 Internal Server Error", fd);
  
	if( (write(fd, "\nContent-Type: text/html\n", 25)) <= 0)
	  return Not200ok("500 Internal Server Error", fd);

	if( (write(fd, "Content-Length: ", 16)) <= 0)
	  return Not200ok("500 Internal Server Error", fd);

	if((sprintf(fileBuf, "%lu", statinfo.st_size)) < 0)
	  return Not200ok("500 Internal Server Error", fd);

	if( (write(fd, fileBuf, strlen(fileBuf))) <= 0)
	  return Not200ok("500 Internal Server Error", fd);

	if( (write(fd, "\n\n", 2)) <= 0)
	  return Not200ok("500 Internal Server Error", fd);

	if(strcasecmp(command, "GET") == 0) {
	  if( (write(fd, basename(fullpath), strlen(basename(fullpath)))) <= 0)
	    return Not200ok("500 Internal Server Error", fd);

	  if( (write(fd, ":\n\n", 3)) <= 0)
	    return Not200ok("500 Internal Server Error", fd);

	  n = 1;
	  while(n > 0) {
	    if( (n = read(reqFile, fileBuf, 128)) < 0)
	      return Not200ok("500 Internal Server Error", fd);
	    if( (write(fd, fileBuf, n)) < 0)
	      return Not200ok("500 Internal Server Error", fd);
	  }
	}

	close(reqFile);

  }


  ret = "200 OK";

  free(fullpath);
  return ret;

}

/*This function supports user request using the GET and HEAD command*/
char* Request(char* userequest, char* cFlag, char* dir, int fd)
{
  /* command must be GET or HEAD, otherwise report not implemented*/
  
  char command[BUFSIZ], pathname[BUFSIZ], version[BUFSIZ];
  char* ret = NULL;

  if(sscanf(userequest,"%s %s %s", command, pathname, version) != 3)
    return Not200ok("400 Bad Request", fd);

  if( ((strcasecmp(command, "GET") == 0) || (strcasecmp(command, "HEAD") == 0)) &&  strcasecmp(version, "HTTP/1.0") == 0)
    ret = Supported(pathname, cFlag, dir, fd, command);
  else if( (strcasecmp(command, "POST") == 0) || (strcasecmp(command, "PUT") == 0) || (strcasecmp(command, "DELETE") == 0) \
	|| (strcasecmp(command, "PATCH") == 0) || (strcasecmp(command, "COPY") == 0) || (strcasecmp(command, "VIEW") == 0) \
	|| (strcasecmp(command, "OPTIONS") == 0) || (strcasecmp(command, "LINK") == 0) || (strcasecmp(command, "UNLINK") == 0) \
	|| (strcasecmp(command, "PURGE") == 0) || (strcasecmp(command, "LOCK") == 0) || (strcasecmp(command, "UNLOCK") == 0) \
	|| (strcasecmp(command, "PROPFIND") == 0) )
    ret = Not200ok("501 Not Implemented", fd);
  else if( (strcasecmp(version, "HTTP/1.0") != 0) && (strncasecmp(version, "HTTP/", 5) == 0) )
    ret = Not200ok("505 Version Not Supported", fd);
  else
    ret = Not200ok("400 Bad Request", fd);

  return ret;
}




