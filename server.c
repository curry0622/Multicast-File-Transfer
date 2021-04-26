/* Send Multicast Datagram code example. */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

struct in_addr localInterface;
struct sockaddr_in groupSock;
int sd;
char databuf[1024];
int datalen = sizeof(databuf);

int main (int argc, char *argv[]) {
  /* Input must have 2 args*/
  if (argc != 2) {
    perror("Sample arg: ./server <file_name>\n");
    exit(1);
  }

  /* Get file name from */
  char* file_name = argv[1];

  /* Get file size */
  struct stat file_stat;
  stat(file_name, &file_stat);
  uint64_t file_size = file_stat.st_size;

  /* Open file */
  FILE* fp = fopen(file_name, "r");
  if (fp == NULL) {
    perror("[ERR] Failed to open file\n");
    exit(1);
  }

  /* Create a datagram socket on which to send. */
  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sd < 0) {
    perror("Opening datagram socket error");
    exit(1);
  }
  else printf("Opening the datagram socket...OK.\n");

  /* Initialize the group sockaddr structure with a */
  /* group address of 226.1.1.1 and port 4321. */
  memset((char *) &groupSock, 0, sizeof(groupSock));
  groupSock.sin_family = AF_INET;
  groupSock.sin_addr.s_addr = inet_addr("226.1.1.1");
  groupSock.sin_port = htons(4321);

  /* Set local interface for outbound multicast datagrams. */
  /* The IP address specified must be associated with a local, */
  /* multicast capable interface. */
  localInterface.s_addr = inet_addr("172.20.10.2");

  /* IP_MULTICAST_IF:  Sets the interface over which outgoing multicast datagrams are sent. */
  if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0) {
    perror("Setting local interface error");
    exit(1);
  }
  else printf("Setting the local interface...OK\n");

  /* Sending file content to client */
  uint64_t read_size = 0;
  while((read_size = fread(databuf, sizeof(char), sizeof(databuf), fp))) {
    if(sendto(sd, databuf, read_size, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0) {
      perror("Sending datagram message error");
      exit(1);
    }
  }

  /* Send end message */
  sendto(sd, "EOF", sizeof("EOF"), 0, (struct sockaddr *) &groupSock, sizeof(groupSock));

  /* Print file size */
  printf("Sending datagram message...OK\n");
  printf("File size: %ldKb\n", file_size / 1024);

  return 0;
}
