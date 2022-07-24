
#ifndef UserController
#define UserController
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include "process.h"
#include <vector>
#include <map>
#include "Data.h"
using namespace std;

static map<string, string> nameAndPass;

string loginHandling(string s);

string createAccount(string s);

int checkUserName(string username);

string listUser();
#endif