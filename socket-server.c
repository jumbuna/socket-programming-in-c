/* Server program to work with the client on socket-client.c */

/* Cross-platform headers */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_size_t.h>
#include <time.h>

/* printError defined in  socket-client.c */
extern void printError(char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

/* Unix specific headers */
#if defined(__APPLE__) || defined(__MACH__) || defined(__linux__)
#include <arpa/inet.h> //inet_addr(), inet_ntoa(), htons()
#include <netdb.h> //getHostByName()
#include <netinet/in.h>
#include <sys/errno.h> // errno
#include <sys/socket.h> //socket(), connect(), send(), listen(), receive()
#include <unistd.h> //close
extern int errno;
#endif

/* Windows specific headers */
#if defined(__WIN32) || defined(__WIN64)
#include <winsock2.h> // All except errno which is equivalent to WSAGetLastError()
static int errno;
#endif

/* socket and client descriptor */
#if defined(__APPLE__) || defined(__MACH__) || defined(__linux__)
int sd, cd;
#endif
#if defined(__WIN32) || defined(__WIN64)
SOCKET sd, cd;
#endif

/**
 * SIGINT signal handler
 */
void onInterrupt(int signal) {
#if defined(__WIN32) || defined(__WIN64)
  closesocket(sd);
#endif
#if defined(__APPLE__) || defined(__MACH__) || defined(__linux__)
  close(sd);
#endif
printf("Closing opened sockets\n");
exit(0);
}

/**
 * Start a server at given port
 * The server receives any data from the connected client and saves it in a file
 *
 * @param: port -> port to listen on
 */
void startServer(int port) {
  signal(SIGINT, onInterrupt);
#if defined(__WIN32) || defined(__WIN64)
  /* Do intialization */
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    printError("Socket api initialization failed");
  }
#endif
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd == -1) {
// TODO handle specific errors
#if defined(__WIN32) || defined(__WIN64)
    errno = WSAGetLastError();
#endif
    switch (errno) {
    default:
      printError("Socket cannot be created");
    }
  }

/* Get current host name. The length is currently hard coded and hopes are that
 * 256 is enough a size.
 */
// char hostName[256];
// #if defined(__APPLE__) || defined (__MACH__) || defined(__linux__)
//     gethostname(hostName, 256);
// #endif
#if defined(__WIN32) || defined(__WIN64)
// TODO add Windows implementation
#endif
  /* Get ip addresses associated with hostName
   *
   * This method of obtaining the host address is inadvisable
   * you should use getifaddrs() (#include <ifaddr.h>) to get interface
   * addresses
   */
  // struct hostent *hn = gethostbyname(hostName);
  struct sockaddr_in sa;
  sa.sin_addr.s_addr = INADDR_ANY;
  sa.sin_port = htons(port);
  sa.sin_family = AF_INET;
  /* 0 is success */
  if (bind(sd, (struct sockaddr *)&sa, sizeof(struct sockaddr_in))) {
#if defined(__WIN32) || defined(__WIN64)
    errno = WSAGetLastError()
#endif
        switch (errno) {
    default:
      printError("Failed to bind socket to address");
    }
  }
  /* Capacity of listen queue */
  /* 0 is success */
  if (listen(sd, 5)) {
#if defined(__WIN32) || defined(__WIN64)
    errno = WSAGetLastError()
#endif
        switch (errno) {
    default:
      printError("Error creating listening queue.");
    }
  }

  struct sockaddr_in client;

  socklen_t sockLen = sizeof(struct sockaddr_in);
  printf("Waiting for connections on port %d\n", port);
  while (1) {
    cd = accept(sd, (struct sockaddr*) &client, &sockLen);
    if (cd == -1) {
#if defined(__WIN32) || defined(__WIN64)
      errno = WSAGetLastError()
#endif
          switch (errno) {
      default:
        printError("Error occurred accepting a connection.");
      }
    }
    /**
     * Server enters receiving state upon a successful connection
     *
     * The current protocol specifies that the clients start sending data to
     * the server immediately after establishing a connection. We will expand
     * protocol to require clients to first send the name of the file they
     * are sending.
     */
    /* File name for received data , format -> file_ip_port_time.recv */
    char *fn = NULL;
    time_t tm;
    /* current time (secs since epoch) */
    time(&tm);
    if (asprintf(&fn, "file_%s_%ld.recv", inet_ntoa(client.sin_addr), tm) < 0) {
      printError("Error constructing filename");
    }
    /* New file opened in binary write mode */
    FILE *fp = fopen(fn, "wb");
    if (fp == NULL) {
      // TODO handle errors
      printError("Error creating output file");
    }
    /* Buffer */
    char buf[1024];
    size_t bufLen = 1024, /*received bytes */ r;
    /**
     * 0        -> peer closed
     * -ve      -> error
     * +ve      -> Bytes read
     */
    while ((r = recvfrom(cd, buf, bufLen, /*flags*/ 0,
                         (struct sockaddr *) &client, &sockLen)) > 0) {
      if (r == fwrite(buf, 1, r, fp)) {
        continue;
      }
      // TODO handle errors
    }
    if (r == -1) {
      printError("An error occured while receiving data");
    }
    if (r == 0) {
      printf("Transfer complete: %s\n", fn);
    }
    fclose(fp);
    close(cd);
  }
}

/*
 * Print help message
 *
 * @param path -> path to executable i.e argv[0]
 */

void printHelp(char *path) {
  printf("socket-server v1.0\n"
         "Usage: socket-server -[ph] [port]\n"
         "Arguments: \n"
         "-p [port]  - port to listen to. Defaults to 8080\n"
         "-h         - Show this message\n");
}

/*
 * usage: socket-server -[ph] [port]
 *
 * @arg p -> port to listen to. Defaults to 8080
 * @arg h -> help message
 */
int main(int argc, char **argv) {
  int port = 8080;
  char arg;
  while ((arg = getopt(argc, argv, "p:h")) != -1) {
    switch (arg) {
    case 'p':
      port = strtol(optarg, 0, 10);
      break;
    case 'h':
      printHelp(argv[0]);
      exit(0);
    case ':':
      printf("No value for opt -%c\n", optopt);
    default:
      printf("Unknown option -%c\n", optopt);
    }
  }
  startServer(port);
}
