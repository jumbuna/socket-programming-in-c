#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* unix headers (linux && mac) */
#if defined(__APPLE__) || defined (__MACH__) || defined (__linux__)
    #include <netinet/in.h>
    #include <sys/_types/_size_t.h>
    #include <sys/errno.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <arpa/inet.h> // hton[sl] , inet_*
#endif
/* end unix headers */

/* Windows headers */
#if defined (__WIN32) || defined (__WIN64)
    #include <winsock2.h>
#endif
/* end windows headers */


#if defined(__APPLE__) || defined (__MACH__) || defined (__linux__)
    extern int errno;
#endif

#if defined (__WIN32) || defined (__WIN64)
    int errno = 0;
#endif

/**
 * Print an error message to stderr
 *
 * All error are fatal
 *
 * Moved the printing to a routine to allow for future
 * changes on how to print the errors
 */
 void printError(char *msg) {
     fprintf(stderr, "%s\n", msg);
     exit(1);
 }

 /**
  *
  * Extract address and port component from a given ip
  * 
  * Port defaults to 8080 if none is given
  */
void extractAddressAndPort(char *ipp, char **ip, int *port) {
    *ip = ipp;
    *port = 8080;
    if((ipp = strchr(ipp, ':')) != NULL) {
        // Terminate at address part
        *ipp = 0;
        *port = strtol(++ipp, 0, 10);
    }
}


/**
 * 
 * This routine takes a file name and sends it over the network 
 * to the given endpoint
 * 
 * We are going to use TCP protocol for the transfer
 * 
 * Upon successful transfer the file returns 0 else it returns
 * a -ve number and sets errno to the error
 * 
 * @param file - file to send.
 * @param endpoint - ip[:port] to send file to.
 */
int sendFile(char *file, char *endPoint) {
    #if defined(__WIN32) || defined (__WIN64)
        WSADATA wsa;
        if( WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
            printError("Socket api initialization failed");
        }
    #endif
    /* the socket descriptor */
    #if defined(__APPLE__) || defined (__MACH__) || defined (__linux__)
        int sd;
    #endif
    #if defined(__WIN32) || defined (__WIN64)
        SOCKET sd;
    #endif
    sd = socket(PF_INET, SOCK_STREAM, 0);
    /* exit if socket was not created */
    if(sd == -1) {
        /**
         * Handle all the cases that may have have led to
         * this function failing
         */
        #if defined (__WIN32) || defined (__WIN64)
            errno = WSAGetLastError();
        #endif
        switch (errno) {
            #if defined(__APPLE__) || defined (__MACH__) || defined (__linux__)
                case EACCES : printError("Permision denied\n");
                case ENOBUFS: printError("Insufficient buffer spaces.\
                Please try again later.\n");
                case ENOMEM: printError("Insufficient memory\n");
            #endif
            #if defined (__WIN32) || defined (__WIN64)
                case INVALID_SOCKET : printError("Could not create socket.")
            #endif
            default: printError("Failed to acquire socket descriptor");
        }
    }
    /* Structure to define the end point */
    struct sockaddr_in remote;
    /* address component of the end point */
    char *address;
    /* Port component of the end point */
    int port;
    extractAddressAndPort(endPoint, &address, &port);
    /**
     * inet_addr converts a given address in '.' notation
     * to a internet address number
     *
     * @documentation: man inet
     */
    remote.sin_addr.s_addr = inet_addr(address);
    /**
     * Remote port to connect to.
     * htons changes the byte order if necessary
     *
     * @documentation: main htons
     */
    remote.sin_port = htons(port);
    remote.sin_family = AF_INET;
    /* open the file to send in binary mode */
    FILE *f = fopen(file, "rb");
    if(f == NULL) {
        switch(errno) {
            default: printError("Error opening file to send");
        }
    }
    /**
     * It's now time to establish a connection to the remote
     * server
     */
    if(connect(sd, (struct sockaddr*)(&remote), sizeof(struct sockaddr_in)) == -1) {
        /* -1 == error */
        #if defined (__WIN32) || defined (__WIN64)
            errno = WSAGetLastError();
        #endif
        switch(errno) {
            default: printError("Failed to establish connection with remote peer");
        }
    }
    /* Buffer size */
    size_t bufSize = 1024;
    /* Buffer */
    char buf[bufSize];
    /* Read file chunks into buffer */
    size_t rd = 0;
    while(1) {
        rd = fread(buf, 1, bufSize, f);
        /* test if an error occured during the reading */
        if(ferror(f)) {
            printError("Error reading file");
        }
        rd = send(sd, buf, rd, 0);
        /* An error occurred during the transfer */
        if(rd == -1) {
            #if defined (__WIN32) || defined (__WIN64)
                errno = WSAGetLastError();
            #endif
            switch (errno) {
             default: printError("An error occured during the transfer");
            }
        }
        /* test if end-of-file is reached */
        if(feof(f)) {
            break;
        }
    }
    printf("File transfer complete closing files/sockets.\n");
    fclose(f);
    #if defined(__APPLE__) || defined (__MACH__) || defined (__linux__)
        close(sd);
    #endif
    #if defined (__WIN32) || defined (__WIN64)
        closesocket(sd);
        WSACleanup();
    #endif
    return 0;
}

/**
 * Print the help message
 * @param path - location of the executable, argv[0]
 */
void printHelp(char *path) {
    printf(
        "Socket v1.0\n"\
        "Usage: %s -f file -r ip\n"\
        "Arguments:\n"\
        "-r, --remote  - remote ipaddress. format a.b.c.d[:p]\n" \
        "-f, --file    - file to send\n"
        "-h, --help    - print this message\n", path
        );
}

/** 
 * Entry point
 * 
 * Our program takes the following arguments
 * -r,--remote -> remote endpoint
 * -h,--help -> show help message
 * -f,--file -> file to send
 *  
 */
int main(int argc, char **argv) {

    /* attempt to get values for -r & -f */
    char *ip = NULL;
    char *file = NULL;
    char arg;
    while((arg = getopt(argc, argv, "f:r:")) != -1) {
        switch(arg) {
            case 'r' : ip = optarg; break;
            case 'f' : file = optarg; break;
            case 'h' : printHelp(argv[0]); exit(0);
            case ':' : printf("Missing value for -%c\n", optopt); exit(1);
            case '?' : printf("unknown argument : %c\n", optopt);
        }
    }
    /* file == NULL | ip  == NULL if arguments were absent*/
    if(file == NULL || ip == NULL) {
        file == NULL ? printf("No file given.\n") : printf("No remote ip given.\n");
        goto notify;
    }
    
    sendFile(file, ip);
    return 0;

    notify:
        printf("Run %s -h or %s --help to get help\n", argv[0], argv[0]);
        return 1;
}