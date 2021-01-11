# Socket Programming in C
This repo contains source code for a 2 program:

1. Client that sends data over a network.
2. Server that listens for connections on the network and receives data sent by the client.

<br>
Though just a simple programs, the concept introduced are fundamental and cover most if not all there is to socket programming. 
<br>

### Concepts
1. Opening and closing sockets
2. Listening for connections
3. Binding socket to an address
4. Sending and receiving data through sockets
5. Accepting connection on a socket

## Compiling
Requirements:

1. Any  C compiler
2. Cmake 3.0+
3. Windows | unix pc

<b>support for other OS can be added if need be. </b>

If you satisfy all the requirements then you may clone this project and run cmake in it.

```bash
$ git clone https://github.com/jumbuna/socket-programming-in-c.git
$ cd socket-programming-in-c
$ cmake -B build
$ cd build
$ cmake --build .
```

Optional:
```
$ cmake --install .
```

### manual compilation
 ```bash
 $ cd socket-programming-in-c
 $ cd build
 $ cc -osocket-server ../socket-server.c
 $ cc -osocket-client ../socket-client.c
 ```
 ><b>Note:</b> cc to be replaced by your c compiler

If all commands executed successfully then you should have an executable named socket-server[.exe] and socket-client[.exe] in the build directory. 

if your also executed *cmake --install . * then the executables will be installed in your system.


<b style="font-size: 1.2em">Support for windows is still buggy since i initially wrote this program to run on unix. </b>

## Running
open 2 terminals and run the following commands:

<b>term 1</b>
```bash
$ cd path_to_repo_clone
$ cd build
$ ./socket-server -p 5000
```
<b>term 2</b>
```bash
$ cd path_to_repo_clone
$ cd build
$ ./socket-client -r 127.0.0.1:5000 -f path_to_file_to_send
```

```
$ ./socket-server -f <path> -r ip[:port]
```
To see usage information run:
<br>
<b> server</b>
```
$ ./socket-server -h
```
<b>client</b>
```
$ ./socket-client -h
```

