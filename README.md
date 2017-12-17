# chatwinsock

The chatwinsock is a simple chattool using winsock2.

## Files

`Console.h`:Simply the color character output and contain a `logW()` which try to realize part of the loading view of linux.

`socketshock.cpp`:Including the main funcition.

`socketshock.dev`:Tne config of Dec-C++ project.

## How to complie

You may need add `-lwsock32 -lWs2_32` for linker.

Warming!:As the winsock2 is for windows,you can't complie chatwinsock on linux.

## How to use

First input 0 ro 1 to select module.Then input the ip address under your local net(like 127.0.0.1).