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

/**
*	@function parseMessage: Split string to get message, store them into vecter result
*	@param s: a string needs to be parsed
*	@return: vector string result
**/
vector<string> parseMessage(string s);

/**
*	@function: receiveMessage: receive message stream from client until reaches ending delimiter
*	@param sock: current client socket
*	@param index
*	@param events
*	@param event
*	@return: vecter string contain message from client
**/
vector<string> receiveMessage(socketInfo* sock, DWORD &index, DWORD &nEvents, WSAEVENT *events);

/**
*	@function sendMessage: send message to client
*	@param connSocket: client connecting socket
*	@param message: a string contain message
*	@return: NULL
**/
void sendMessage(SOCKET connSocket, string message);
#endif