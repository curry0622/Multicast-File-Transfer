/* Receiver/client multicast Datagram example. */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct sockaddr_in localSock;
struct ip_mreq group;
int sd;
int datalen;
char databuf[1024];

/* Print error message and exit program */
void err(char* msg) {
  fputs(msg, stdout);
  exit(-1);
}

int main(int argc, char *argv[]) {
/* Create a datagram socket on which to receive. */
  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sd < 0)
    err("Opening datagram socket error\n");
  else
    printf("Opening datagram socket....OK.\n");

  /* Enable SO_REUSEADDR to allow multiple instances of this */
  /* application to receive copies of the multicast datagrams. */

  int reuse = 1;
  if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0) {
    err("Setting SO_REUSEADDR error\n");
    close(sd);
  }
  else
    printf("Setting SO_REUSEADDR...OK.\n");

  /* Bind to the proper port number with the IP address */
  /* specified as INADDR_ANY. */
  memset((char *) &localSock, 0, sizeof(localSock));
  localSock.sin_family = AF_INET;
  localSock.sin_port = htons(4321);
  localSock.sin_addr.s_addr = INADDR_ANY;
  if(bind(sd, (struct sockaddr*)&localSock, sizeof(localSock))) {
    err("Binding datagram socket error\n");
    close(sd);
  }
  else
    printf("Binding datagram socket...OK.\n");

  /* Join the multicast group 226.1.1.1 on the local address*/
  /* interface. Note that this IP_ADD_MEMBERSHIP option must be */
  /* called for each local interface over which the multicast */
  /* datagrams are to be received. */
  group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");
  /* your ip address */
  group.imr_interface.s_addr = inet_addr("127.0.0.1");
  /* IP_ADD_MEMBERSHIP:  Joins the multicast group specified */
  if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0) {
    err("Adding multicast group error\n");
    close(sd);
  }
  else
    printf("Adding multicast group...OK.\n");

  /* Open file */
  FILE* fptr = fopen("output.txt", "w");
  if (fptr == NULL)
    err("Opening file error/n");

  /* Reading */
  uint64_t return_size = 0;
  uint64_t file_size = 0;
  datalen = sizeof(databuf);
  while (1) {
    bzero(databuf, datalen);
    return_size = recvfrom(sd, databuf, datalen, 0, NULL, NULL);
    if (return_size < 0) {
      err("Reading datagram message error\n");
      close(sd);
    } else if(strcmp(databuf, "END") == 0)
      break;
    else {
      fwrite(databuf, sizeof(char), return_size, fptr);
      file_size += return_size;
    }
  }

  /* Finish */
  printf("Reading datagram message...OK\n");
  printf("Receive file size: %ldKb\n", file_size / 1024);

  return 0;
}
