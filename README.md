# Socket Programming in C
This repo contains source code for a program that takes a file and sends it over the internet. Though just a simple program, the concept introduced are fundamental and cover most if not all there is to socket programming.

## Compiling
Requirements:

1. Any  C compiler
2. Cmake 3.0+
3. Windows | unix pc

<b>support for other OS can be added if need be. </b>

If you satisfy all the requirements then you may clone this project and run cmake in it.

```
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

If all commands executed successfully then you should have an executable named socket[.exe] in the build directory. 

if your also executed *cmake --install . * then the socket executable will be installed in your system.


<b style="font-size: 1.2em">Support for windows is still buggy since i initially wrote this program to run on unix. </b>

## Running

```
$ socket -f <path> -r ip[:port]
```
To see usage information run:
```
$ socket -h
```

