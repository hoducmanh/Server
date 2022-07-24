#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include "process.h"
#include <fstream>
#include <vector>
#include "Data.h"
#include "UserController.h"
using namespace std;
 
map<string, string> account;


int checkUserName(string username) {
	int cnt = 0;//counting times 
	string tmpStr;
	fstream new_file;
	new_file.open("Account.txt", ios::in);
	if (new_file.is_open()) {
		while (getline(new_file, tmpStr)) {
			int pos;
			string key = "|";//find delimiter level 2
			pos = tmpStr.find(key);
			string userName = tmpStr.substr(0, pos);
			string password = tmpStr.substr(pos + 1, tmpStr.length() - pos);
			account[userName] = password;
		}
	}
	else
		cout << "Cannot read file" << endl;
	new_file.close();
	if (account.size() == 0) return 0;
	for (auto& x : account) {
		if (username == x.first)
			return 1;
	}
	return 0;
}

string listUser() {
	fstream new_file;
	string userName;
	string listOfUser;
	new_file.open("Account.txt", ios::in);
	string tmpStr;
	if (new_file.is_open()) {
		while (getline(new_file, tmpStr)) {
			int pos;
			string key = "|";//find delimiter level 2
			pos = tmpStr.find(key);
			string userName = tmpStr.substr(0, pos);
			listOfUser.append(userName);
			listOfUser.append("|");
		}
	}
	else {
		cout << "Cannot read file" << endl;
		return "99";
	}
	new_file.close();
	return listOfUser;
}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
/**
*@function isDumplicateAccount: check if user register with the username that was registerd before .
*
@param newAccount: username that is registerd
*
*@return:
*true: if user register with the username that was registerd before
*false: if user register with the username that was NOT registerd before
**/
bool isDumplicateAccount(string newAccount) {
	string filename("Account.txt");
	string line;
	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '"
			<< filename << "'" << endl;
		return false;
	}
	while (getline(input_file, line)) {
		if (line.substr(0, newAccount.length()).compare(newAccount) == 0) {
			return true;
		}
	}
	input_file.close();
	return false;
}
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
string createAccount(string s) {
	int beginindex = 0; int cnt = 0;
	ofstream outfile;
	outfile.open("Account.txt", ios::app);
	string name = "";
	string pass = "";
	for (int i = 0; i<s.size(); i++) {
		if (s[i] == '|') {
			cnt += 1;
			if (cnt == 1) {
				name += s.substr(beginindex, i - beginindex);
				if (isDumplicateAccount(name)) {
					cout << "Duplicate username" << endl;
					return "13";
				}
				if (name == "") {
					return "99";
				}
				beginindex = i + 1;
			}
			else if (cnt == 2) {
				pass += s.substr(beginindex, i - beginindex);
				if (pass == "") {
					return "99";
				}
				beginindex = i + 1;
			}
			else {
				return "99";
			}
		}
	}
	if (cnt < 2) {
		return "99";
	}
	else {
		cout << "Client successfully register with username: " << name << ", password: "<< pass << endl;
		outfile << name << '|' << pass << endl;
		nameAndPass[name] = pass;
		return "10";
	}
	outfile.close();
}
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
string loginHandling(string s) {
	int beginindex = 1, cnt = 0;
	string name = "";
	string pass = "";
	for (int i = 0; i<s.length(); i++) {
		if (s[i] == '|') {
			cnt++;
			if (cnt == 1) {
				name += s.substr(beginindex - 1, i - beginindex + 1);
				if (checkUserName(name) == 0) {
					cout << "Client login with the username wasn't stored in server!" << endl;
					return "21";
				}
				beginindex = i + 1;
			}
			else if (cnt == 2) {
				pass += s.substr(beginindex, i - beginindex);
				beginindex = i + 1;
			}
			else {
				return "99";
			}
		}
	}
	if (cnt<2) {
		return "99";
	}
	else {
		if (account[name] == pass) {
			//login successful
			cout << "Client successfully login with username: " << name << ", password: " << pass << endl;
			return "20";
		}
		else {
			//login fail
			cout << "Client login fail!" << endl;
			return "21";
		}
	}
}
