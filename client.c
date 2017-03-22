/* Name: Jason Sarwar and Marlon Seaton
 * CS 631
 * Final Project
 * client.c
 */

#include "sws.h"
#include "functions.h"
#define bufMax 512

int sockfd = 0; /* File descriptor for socket */

void clientExit(int sig) {
  if( write(sockfd, "EXIT", 4) < 0)
    ErrorMessage("Error disconnecting\n");
  printf("Disconnected from server\n");
  exit(EXIT_SUCCESS);
}


int main(int argc, char** argv)
{
  socklen_t socklength;
  struct sockaddr_in srv;
  struct hostent *host;
  short port;
  char buffer[bufMax];  
  int n = 0;

  if(argc > 3)
    ErrorMessage("Usage: ./client <serverhost> <port>\n");
  
  if( (port = atoi(argv[2])) <= 0 || port > 65535 ) 
    ErrorMessage("Invalid Port Number\n");

  if( (host = gethostbyname(argv[1])) == NULL )
    ErrorMessage("Could not get server host name\n");

  sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if(sockfd == -1)
    ErrorMessage("socket failure\n");

  srv.sin_family = AF_INET;
  srv.sin_port = htons(port);
  memcpy(&srv.sin_addr.s_addr, host -> h_addr, host -> h_length);

  socklength = sizeof(srv);

  if((connect(sockfd, (struct sockaddr*)&srv, socklength )) < 0 )
     ErrorMessage("connect failed\n");
  
  signal(SIGINT, clientExit);

  while(1) {
    if( (write(1, "\nSend Request: ", 15)) < 0)
      ErrorMessage("Write failed\n");

    bzero(buffer, bufMax);
    if( (n = read(0, buffer, bufMax - 1)) < 0)
      ErrorMessage("Error taking terminal input\n");

    buffer[n - 1] = '\0';

    if( (write(sockfd, buffer, n)) < 0)
      ErrorMessage("Error sending request to server\n");

    if( (n = read(sockfd, buffer, bufMax)) < 0)
      ErrorMessage("Error receiving response from server\n");

    if( (write(1, buffer, n)) < 0)
      ErrorMessage("Error writing response to terminal\n");

  }



  close(sockfd);
  return 0;
}
