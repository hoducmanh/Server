
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

/**
*@function checkUserName: check if the username exist or not.
*
@param username: username that nees to be checked
*
@return: 1 if username exist, 0 if username doesnt exist
**/
int checkUserName(string username);

/**
*@function isDumplicateAccount: check if user register with the username that was registerd before .
*
@param newAccount: username that is registerd
*
*@return:
*true: if user register with the username that was registerd before
*false: if user register with the username that was NOT registerd before
**/
bool isDumplicateAccount(string newAccount);

/**
*@function loginHandling: Login for user.
*
@param s: a string contain user infor: name and pass
*
*@return:
*99: if request is SYSTAX ERROR
*21: login fail
*20: login successful
**/
string loginHandling(string s);

/**
*@function createAccount: Register for user.
*
@param s: a string contain user infor: name and pass
*
*@return:
*99: if request is SYSTAX ERROR
*13: dumplicate username
*12: register successfu
**/
string createAccount(string s);

/**
*@function listUser: listing registered username and send to client that request
*
@return: message that contain username list
**/
string listUser();
#endif