#ifndef Data
#define Data
#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <string>
#include "process.h"
#include <map>
#include <vector>
using namespace std;

struct socketInfo
{
	SOCKET clientSock;
	sockaddr_in clientAddr;
	string userName;
};
#endif