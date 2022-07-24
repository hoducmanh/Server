#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include "process.h"
#include <map>
#include <vector>
#include <fstream>
#include "Data.h"
#include "UserController.h"
#define BUFF_SIZE 2048
#define ENDING_DELIMITER "\r\n"
using namespace std;

map<string, string> eventNameMap; //key: user, value:event's id
map<string, string> eventNameData;// read from event file
map<string, string> userAndEvent;//key:id, value:username

/**
*@function deleteLine: delete line n th.
*
*@param file_name: pointer point to file
@param n: line postion need to delete
*
*@return:
*non return value
**/
void delete_line(const char *file_name, int n)
{
	ifstream is(file_name);
	ofstream ofs;
	ofs.open("temp.txt", ofstream::out);
	char c;
	int line_no = 1;
	while (is.get(c))
	{
		if (c == '\n')
			line_no++;
		if (line_no != n)
			ofs << c;
	}
	ofs.close();
	is.close();
	remove(file_name);
	rename("temp.txt", file_name);
}

int getLineRequest(string s) {
	string filename("Request.txt");
	string line, eventId = "";
	int pos = 0, beginindex;
	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '"
			<< filename << "'" << endl;
		return 0;
	}
	while (getline(input_file, line)) {
		pos++;
		if (line == s) {
			return pos;
		}
	}
	return 0;
	input_file.close();
}

void listEventId() {
	string tmpStr;
	fstream new_file;
	new_file.open("EventId.txt", ios::in);
	if (new_file.is_open()) {
		while (getline(new_file, tmpStr)) {
			int pos;
			string key = "|";//find delimiter level 2
			pos = tmpStr.find(key);
			string eventId = tmpStr.substr(0, pos);
			string userName = tmpStr.substr(pos + 1, tmpStr.length() - pos);
			eventNameMap[eventId] = userName;
		}
	}
	else
		cout << "Cannot read file" << endl;
	new_file.close();
}

void listNameAndPass() {
	string tmpStr;
	string username;
	string password;
	fstream new_file;
	new_file.open("Account.txt", ios::in);
	if (new_file.is_open()) {
		while (getline(new_file, tmpStr)) {
			int pos;
			string key = "|";//find delimiter level 2
			pos = tmpStr.find(key);
			username = tmpStr.substr(0, pos);
			password = tmpStr.substr(pos + 1, 1);
			nameAndPass[username] = password;
		}
	}
	else
		cout << "Cannot read file" << endl;
	new_file.close();
}

void listUserAndEvent() {
	string tmpStr;
	fstream new_file;
	new_file.open("EventId.txt", ios::in);
	if (new_file.is_open()) {
		while (getline(new_file, tmpStr)) {
			int pos;
			string key = "|";//find delimiter level 2
			pos = tmpStr.find(key);
			string userName = tmpStr.substr(0, pos);
			string eventId = tmpStr.substr(pos + 1, tmpStr.length() - pos);
			userAndEvent[eventId] = userName;
		}
	}
	else
		cout << "Cannot read file" << endl;
	new_file.close();
}

int checkEventId(string eventId) {
	listEventId();
	if (eventNameMap.size() == 0) return 0;//ok to create event
	for (auto& x : eventNameMap) {
		if (eventId == x.first)
			return 1;
	}
	return 0;
}

string requestHandling(string s) {
	int cnt = 0;
	string sender; //REQUEST sender|receiver|id|
	string receiver;
	string eventId;
	string tmpStr;
	for (int i = 0; i < s.size(); i++) {
		if (s[i] == '|') {
			if (cnt == 0) {//find sender
				tmpStr = s.substr(0, i);
				sender = tmpStr;
				s.erase(0, i + 1);
				i = 0;
				cnt++;
			}
			else if (cnt == 1) {//find receiver
				tmpStr = s.substr(0, i);
				receiver = tmpStr;
				s.erase(0, i + 1);
				i = 0;
				cnt++;
			}
			else if (cnt == 2) {//find event's name
				tmpStr = s.substr(0, i);
				eventId = tmpStr;
				s.erase(0, i + 1);
				i = 0;
				cnt++;
			}
		}
	}
	if (checkEventId(eventId) == 0) {
		cout << "event doesn't exist" << endl;
		return "99"; //event doesnt exist
	}//manh 
	if (checkUserName(receiver) == 0) {
		cout << "receiver doesn't exist" << endl;
		return "99"; // receiver doesnt exist
	}
	ofstream ofs("Request.txt");
	string pushStr = receiver + "|" + sender + "|" + eventId + "|";
	ofs.write(pushStr.data(), pushStr.size());	
	return "60";// Send request successfully
}

string requestReplyHandling(string s) {
	int cnt = 0;
	string sender;
	string receiver;
	string eventId;
	string reply;
	string tmpStr;
	for (int i = 0; i < s.size(); i++) {
		if (s[i] == '|') {
			if (cnt == 0) {//find sender
				tmpStr = s.substr(0, i);
				sender = tmpStr;
				s.erase(0, i + 1);
				i = 0;
				cnt++;
			}
			else if (cnt == 1) {//find receiver
				tmpStr = s.substr(0, i);
				receiver = tmpStr;
				s.erase(0, i + 1);
				i = 0;
				cnt++;
			}
			else if (cnt == 2) {//find event's ID
				tmpStr = s.substr(0, i);
				eventId = tmpStr;
				s.erase(0, i + 1);
				i = 0;
				cnt++;
			}
			else if (cnt == 3) {
				tmpStr = s.substr(0, i);
				reply = tmpStr;
			}
		}
	}
	if (cnt > 3 || cnt < 3) return "99";
	if (checkEventId(eventId) == 0) {
		cout << "event doesn't exist" << endl;
		return "99"; //event doesnt exist
	}
	if (checkUserName(receiver) == 0) { //EVENT_REQUEST_REPLY nguoigui|nguoinhan|tensk|reply(OK/DENY)
		cout << "receiver doesn't exist" << endl;
		return "99"; // receiver doesnt exist
	}
	if (reply == "OK" || reply == "DENY") {
		cout << "accecpt request" << endl;
		ofstream ofs("RequestReply.txt");
		string pushStr = receiver + '|' + sender + '|' + eventId + '|' + reply + '|';
		ofs.write(pushStr.data(), pushStr.size());
		string request = sender + '|' + receiver + '|' + eventId + '|';
		int n =getLineRequest(request);
		if (n == 0) {
			return "99";
		}
		delete_line("Request.txt", n);
		return "70";//reply successfully
	}
	return "99";
}

string listRequest(string requester) {
	string listOfRequest;
	fstream new_file;
	new_file.open("Request.txt", ios::in);
	string tmpStr;
	if (new_file.is_open()) {
		while (getline(new_file, tmpStr)) {
			int pos;
			string key = "|";//find delimiter level 2
			pos = tmpStr.find(key);
			string userName = tmpStr.substr(0, pos);
			if (userName == requester) {
				string info = tmpStr.substr(pos + 1, tmpStr.length() - pos);
				string s = info + '#';
				listOfRequest.append(s);
			}
		}
	}
	else
		cout << "Cannot read file" << endl;
	new_file.close();
	return listOfRequest;
}


/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/**
*@function getLine: get position of the invitation that was stored in Invitation.txt file.
*
*@param s: the invitation that need to find position
*
*@return:
*0 if the invitation not exist
*pos: if position of the invitation is pos
**/
int getLine(string s) {
	string filename("Invitation.txt");
	string line, eventId = "";
	int pos = 0, beginindex;
	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '"
			<< filename << "'" << endl;
		return 0;
	}
	while (getline(input_file, line)) {
		pos++;
		if (line == s) {
			return pos;
		}
	}
	return 0;
	input_file.close();
}

/**
*@function createEvent: Create an event. Store event's id, name, time, place, description in Event.txt; eventId and creator is stored in EventId.txt
*
@param s: a string contain event information : creator, event's name, time, place, description
@param eventNameMap: a map contain eventId and username(creator) - eventId is key, username is value. An element is added when create event successfully
@param countEvent: eventId that is create automatically
@param nameAndPass: a map contain username and password of user had register. username is key, pass is value. If username not exist, password is empty
*
*@return:
*99 if request is SYSTAX ERROR, username(creator) is not exist, eventInfor is empty
*30 if Create event successfully
**/

string createEvent(string s, map<string, string>&eventNameMap, int& countEvent, map<string, string>&nameAndPass) {
	//EVENTCREATE creator|eventName|eventPlace|eventTime | eventDescription | \r\n
	int beginindex = 0, cnt = 0;
	ofstream outfileEvent, outfileEventName;
	outfileEvent.open("Event.txt", ios::app);
	outfileEventName.open("EventId.txt", ios::app);
	string eventCreator = "";
	string eventName = "";
	string eventPlace = "";
	string eventTime = "";
	string eventDesc = "";
	string eventId = "";
	countEvent++;
	eventId += to_string(countEvent);

	for (int i = 0; i < s.length(); i++) {
		if (s[i] == '|') {
			cnt++;
			if (cnt == 1) {
				eventCreator += s.substr(beginindex, i - beginindex);
				cout << eventCreator << endl;
				cout << nameAndPass[eventCreator] << endl;
				if (eventCreator == "" || nameAndPass[eventCreator] == "") {
					cout << "1" << endl;
					countEvent--;
					return "99";
				}
				beginindex = i + 1;
			}
			else if (cnt == 2) {
				eventName += s.substr(beginindex, i - beginindex);
				if (eventName == "") {
					countEvent--;
					cout << "2" << endl;
					return "99";
				}
				beginindex = i + 1;
			}
			else if (cnt == 3) {
				eventPlace += s.substr(beginindex, i - beginindex);
				if (eventPlace == "") {
					cout << "3" << endl;
					countEvent--;
					return "99";
				}
				beginindex = i + 1;
			}
			else if (cnt == 4) {
				eventTime += s.substr(beginindex, i - beginindex);
				if (eventTime == "") {
					cout << "4" << endl;
					countEvent--;
					return "99";
				}
				beginindex = i + 1;
			}
			else if (cnt == 5) {
				eventDesc += s.substr(beginindex, i - beginindex);
				if (eventDesc == "") {
					cout << "5" << endl;
					countEvent--;
					return "99";
				}
				beginindex = i + 1;
			}
			else {
				cnt = 0;
				countEvent--;
				cout << "6" << endl;
				return "99";
			}
		}
	}
	if (cnt<5) {
		cnt = 0;
		cout << "7" << endl;
		return "99";
	}
	else {
		outfileEvent << eventId << '|' << eventName << '|' << eventPlace << '|' << eventTime << '|' << eventDesc << '|' << endl;
		outfileEventName << eventId << '|' << eventCreator << endl;
		cout << "Create event successfully" << endl;
		eventNameMap[eventId] = eventCreator;

		return "30";
	}
	outfileEvent.close();
	outfileEventName.close();
}
/**
*@function inviteHandling: Create an invitation.
*
@param s: a string contain invitation information : inviter (creator - user that create the event), reveiver, eventId
@param eventNameMap: a map contain eventId and username(creator) - eventId is key. Used to check if the event was created by inviter
@param nameAndPass: a map contain username and password of user had register. username is key, pass is value. If username not exist, password is empty
*
*@return:
*99 if request is SYSTAX ERROR, creator or reviver is not exist, the event not was created by inviter
*40 if Create invitation successfully
**/
string inviteHandling(string s, map<string, string>&nameAndPass, map<string, string>&eventNameMap) {
	//INVITE Sender|Receiver|eventId|\r\n
	cout << s << endl;
	int beginindex = 0, cnt = 0;
	ofstream outfile;
	outfile.open("Invitation.txt", ios::app);
	string eventCreator = "";
	string receiver = "";
	string eventId = "";
	for (int i = 0; i < s.length(); i++) {
		if (s[i] == '|') {
			cnt++;
			if (cnt == 1) {
				eventCreator += s.substr(beginindex, i - beginindex);
				cout << eventCreator << endl;
				if (nameAndPass[eventCreator] == "") {
					return "99";
				}
				beginindex = i + 1;
			}
			else if (cnt == 2) {
				receiver += s.substr(beginindex, i - beginindex);
				//Wrong receiver's name
				if (nameAndPass[receiver] == "") {
					cout << receiver << endl;
					return "99";
				}
				beginindex = i + 1;
			}
			else if (cnt == 3) {
				eventId += s.substr(beginindex, i - beginindex);
			}
			else {
				return "99";
			}
		}
	}
	if (cnt<3) {
		return "99";
	}
	else {
		outfile << eventCreator << '|' << receiver << '|' << eventId << endl;
		cout << "Send invitation successfully" << endl;
		return "40";
	}
	outfile.close();
}
/**
*@function inviteReplyHandling: Accept or deny invitation. Once the invitation is responsed, it is deleted in file Invitaion.txt.
The response is written in file InvitationReply.txt
*
@param s: a string contain reply information : inviter (creator - user that create the event), reveiver, eventId, OK-accept/DENY-deny
@param eventNameMap: a map contain eventId and username(creator) - eventId is key. Used to check if the event was created by inviter
@param nameAndPass: a map contain username and password of user had register. username is key, pass is value. If username not exist, password is empty
*
*@return:
*99 if request is SYSTAX ERROR, creator or reviver is not exist, the event not was created by inviter
*50 if response successfully
*51 if the invitation is not in file Invitation.txt
**/
string inviteReplyHandling(string s, map<string, string>&nameAndPass, map<string, string>&eventNameMap) {
	//INVITEREPLY Sender|Receiver|eventId|OK/DENY|
	ofstream outfile;
	outfile.open("InvitationReply.txt", ios::app);
	int cnt = 0, beginindex = 0;
	string eventCreator = "";
	string receiver = "";
	string eventId = "";
	string reply = "";
	string invite = "";

	for (int i = 0; i < s.length(); i++) {
		if (s[i] == '|') {
			cnt++;
			if (cnt == 1) {
				receiver += s.substr(beginindex, i - beginindex);
				if (nameAndPass[receiver] == "") {
					return "99";
				}
				beginindex = i + 1;
			}
			else if (cnt == 2) {
				eventCreator += s.substr(beginindex, i - beginindex);
				if (nameAndPass[eventCreator] == "") {
					return "99";
				}
				beginindex = i + 1;
			}
			else if (cnt == 3) {
				eventId += s.substr(beginindex, i - beginindex);
				beginindex = i + 1;
			}
			else if (cnt == 4) {
				reply += s.substr(beginindex, i - beginindex);
				beginindex = i + 1;
			}
			else {
				return "99";
			}
		}
	}
	if (cnt < 4) {
		return "99";
	}
	else {
		outfile << receiver << '|' << eventCreator << '|' << eventId << '|' << reply << endl;
		invite = eventCreator + '|' + receiver + '|' + eventId;
		int n = getLine(invite);
		if (n == 0) {
			return "51";
		}
		delete_line("Invitation.txt", n);
		return "50";
	}
	outfile.close();
}
/**
*@function getInvitationList: get all invitation that was sent to user.
*
@param userName: a string contain username who want to see he/she 's Invitation List
*
*@return:
*invitationList List of invitation. Each Invitaion is concat to each other by '#'
**/
string getInvitationList(string userName) {
	string invitationList = "";
	string filename("Invitation.txt");
	string line;
	int first = 0, second = 0, cnt = 0, lineNum = 0;
	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '"
			<< filename << "'" << endl;
		return "Server can not access to DB. Try Again!";
	}
	while (getline(input_file, line)) {
		for (int i = 0; i < line.length(); i++) {
			if (line[i] == '|') {
				cnt++;
				if (cnt == 1) {
					first = i + 1;
				}
				if (cnt == 2) {
					second = i + 1;
				}
			}
		}
		if (line.substr(first, second - first - 1).compare(userName) == 0) {
			invitationList += line;
		}
		invitationList += "#";
		cnt = 0;
	}
	return invitationList;
	input_file.close();
}
/**
*@function getEventList: get all event.
*
@param: no param
*
*@return:
*eventList: List of Event. Each Event is concat to each other by '#'
**/
string getEventList() {
	string eventList = "";
	string filename("Event.txt");
	string line;
	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '"
			<< filename << "'" << endl;
		return "99";
	}
	while (getline(input_file, line)) {
		eventList += line;
		eventList += "#";
	}
	return eventList;
	input_file.close();
}
/**
*@function getUserAttendList: get all user attend the Event.
*
@param id: a string contain event's id
*
*@return:
*userList List of invitation. Each Invitaion is concat to each other by '#'
**/
string getUserAttendList(string id) {
	string userList = "";
	string filename("InvitationReply.txt");
	string line; int cnt = 0, beginindex;
	ifstream input_file_rep(filename);
	ifstream input_file_req("RequestReply.txt");
	if (!input_file_rep.is_open() || !input_file_req.is_open()) {
		cerr << "Could not open the file" << endl;
		return "99";
	}
	while (getline(input_file_rep, line)) {
		beginindex = line.length() - 1;
		if (line != "") {
			if (line.substr(beginindex - 1, 2) == "OK"&& line.substr(beginindex - 7, 5) == id) {
				userList += line.substr(0, line.find('|', 0)) + "#";
			}
			else
				continue;
		}
		else {
			continue;
		}
	}
	while (getline(input_file_req, line)) {
		beginindex = line.length() - 1;
		if (line != "") {
			if (line.substr(beginindex - 1, 2) == "OK"&& line.substr(beginindex - 7, 5) == id) {
				userList += line.substr(0, line.find('|', 0)) + "#";
			}
			else
				continue;
		}
		else {
			continue;
		}
	}

	input_file_rep.close();
	input_file_req.close();
	return userList;
}
/**
*@function getEventInforById: get Event infor corresponding with eventId.
*
@param n: event id in Number
*
*@return:
*eventInfor: event Information
**/
string getEventInforById(int n) {
	string eventInfor = ""; int cnt = 9999;
	string filename("Event.txt");
	string line;
	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '"
			<< filename << "'" << endl;
		return "99";
	}
	while (getline(input_file, line)) {
		cnt++;
		if (cnt == n) {
			eventInfor += line;
			input_file.close();
			return eventInfor;
		}
	}
}
