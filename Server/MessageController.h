#ifndef MessageController
#define MessageController
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include "process.h"
#include <vector>
#include "Data.h"
using namespace std;


vector<string> parseMessage(string s);

vector<string> receiveMessage(socketInfo* sock, DWORD &index, DWORD &nEvents, WSAEVENT *events);

void sendMessage(SOCKET connSocket, string message);
#endif