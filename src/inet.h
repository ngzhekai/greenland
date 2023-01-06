/* inet.h */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> //bzero requires
#include <unistd.h>
#include <error.h>
#include <errno.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h> /* gethostbyname() */
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_TCP_PORT 25000
#define SERV_UDP_PORT 35001
#define CLI_UDP_PORT 35002

#define FILENAME "database.txt"
#define BUFFER_SIZE 1024
