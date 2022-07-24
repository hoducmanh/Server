// Server.cpp : Defines the entry point for the console application.
//TODO:
//add delete reqest in requestReplyHandling
//add eventUser(users that participate in particular event)
//fix inviteHandling
//fix inviteReplyHandling
//fix refreshInvite
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <fstream>
#include "process.h"
#include <map>
#include <vector>
#include "Data.h"
#pragma comment(lib, "Ws2_32.lib")
#define PORT 5500
#define BUFF_SIZE 2048
#define SERVER_ADDR "127.0.0.1"
#define MAX_CLIENT 64
#define ENDING_DELIMITER "\r\n"
#include "MessageController.h"
#include "EventController.h"
#include "UserController.h"
using namespace std;

vector<string> eventsInfo;
char recv_buff[BUFF_SIZE];

map<string, string>eventNameMap1;
map<string, string>nameAndPass1;
int countEvent = 9999;

void loadEvent(map<string, string>&eventNameMap, int &countEvent) {
	string filename("EventId.txt");
	string line, eventId = "";
	int cnt, beginindex;
	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '"
			<< filename << "'" << endl;
		return;
	}
	while (getline(input_file, line)) {
		countEvent++;
		cnt = 0; beginindex = 0; eventId.erase();
		for (int i = 1; i < line.length(); i++) {

			if (line[i] == ' ') {
				cnt++;
				if (cnt == 1) {
					eventId = eventId + line.substr(0, i - beginindex);
					cnt = cnt + 1; beginindex = i + 1;
				}
				else {
					eventNameMap[eventId] = line.substr(beginindex, line.length() - beginindex);
					break;
				}

			}
		}
	}
	input_file.close();
}

void loadAccount(map<string, string>&nameAndPass1) {
	string filename("Account.txt");
	string line;
	string name = "";
	string pass = "";
	int cnt = 0, beginindex = 0;
	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '"
			<< filename << "'" << endl;
		return;
	}
	cout << "All account we have: " << endl;
	while (getline(input_file, line)) {
		cnt = 0; beginindex = 0; name = "";
		for (int i = 1; i < line.length(); i++) {

			if (line[i] == '|') {
				cnt++;
				if (cnt == 1) {
					name = name + line.substr(0, i - beginindex);
					beginindex = i + 1;
					nameAndPass1[name] = line.substr(beginindex, line.length() - beginindex);
					cout << "name: " << name << endl;
					cout << "pass: " << nameAndPass1[name] << endl;
					cnt = 0;
					break;
				}

			}
		}
	}
	cout << "**********************END*************************" << endl;
	input_file.close();
}

void messageHandling(socketInfo &client, socketInfo* sock, DWORD &index, DWORD &nEvents, WSAEVENT* events)
{
	//receive message from clients
	vector<string> recvMessage = receiveMessage(sock, index, nEvents, events);
	if (recvMessage.size() == 0) return;
	//handling each type of message
	for (int i = 0; i < recvMessage.size(); i++) {
		int pos = recvMessage[i].find(" ");
		string header = recvMessage[i].substr(0, pos);
		recvMessage[i].erase(0, pos + 1);
		string tmpStr = recvMessage[i];

		if (header == "CREATEUSER") {
			string code = createAccount(tmpStr);
			sendMessage(client.clientSock, code);
		}
		else if (header == "USER")
		{
			int pos = tmpStr.find("|");
			client.userName = tmpStr.substr(0, pos);
			string code = loginHandling(tmpStr);
			sendMessage(client.clientSock, code);
			//passed
		}
		else if (header == "BYE")
		{
			cout << client.userName << " has logged out" << endl;
			client.userName = "";
			string code = "120";
			sendMessage(client.clientSock, code);
			//passed
		}
		else if (header == "CREATEEVENT")
		{
			string code = createEvent(tmpStr, eventNameMap1, countEvent, nameAndPass1);;
			sendMessage(client.clientSock, code);
			//passed
		}
		else if (header == "INVITE")
		{
			string code = inviteHandling(tmpStr, nameAndPass1, eventNameMap1);
			sendMessage(client.clientSock, code);

		}
		else if (header == "INVITEREPLY")
		{
			string code = inviteReplyHandling(tmpStr, nameAndPass1, eventNameMap1);
			sendMessage(client.clientSock, code);
		}
		else if (header == "REQUEST")
		{
			string code = requestHandling(tmpStr);
			sendMessage(client.clientSock, code);
			//passed
		}
		else if (header == "REQUESTREPLY")
		{
			string code = requestReplyHandling(tmpStr);
			sendMessage(client.clientSock, code);
			//passed
		}
		else if (header == "REFRESHINVITE")
		{
			string code = getInvitationList(tmpStr);
			sendMessage(client.clientSock, code);
		}
		else if (header == "REFRESHREQUEST")
		{
			string listOfRequest = listRequest(tmpStr);
			sendMessage(client.clientSock, listOfRequest);
			//passed
		}
		else if (header == "LISTUSER")
		{
			string listOfUser = listUser();
			sendMessage(client.clientSock, listOfUser);
			//passed
		}
		else if (header == "LISTEVENT")
		{
			getEventList();
		}
		else {
			sendMessage(client.clientSock, "99");
		}
	}
}


unsigned __stdcall worker_thread(void *param) {
	// resource initialization
	DWORD		nEvents = 0;
	DWORD		index;
	socketInfo	socks[MAX_CLIENT];
	WSAEVENT	events[WSA_MAXIMUM_WAIT_EVENTS];
	WSANETWORKEVENTS sockEvent;
	SOCKET connSock;
	sockaddr_in clientAddr;
	int thread_status = 0, clientAddrLen = sizeof(clientAddr);

	//get listenSock from parent worker thread
	SOCKET listenSock = ((SOCKET*)param)[0];
	if (((SOCKET*)param)[1] != INVALID_SOCKET)
	{
		SOCKET connSock = ((SOCKET*)param)[1];
		//add connSock to array of clients, 
		//create an event and assign it to connSock with reading and closing event
		socks[1].clientSock = connSock;
		events[1] = WSACreateEvent();
		cout << "New client connected" << endl;
		WSAEventSelect(socks[1].clientSock, events[1], FD_READ | FD_CLOSE);
		nEvents++;
	}
	//set first element of client array with listenSock
	socks[0].clientSock = listenSock;
	events[0] = WSACreateEvent(); //create new events
	nEvents++;

	// Assign an event types FD_ACCEPT and FD_CLOSE
	// with the listening socket and newEvent   
	WSAEventSelect(socks[0].clientSock, events[0], FD_ACCEPT | FD_CLOSE);

	for (int i = 1; i < WSA_MAXIMUM_WAIT_EVENTS; i++) {
		if (i == 1 && ((SOCKET*)param)[1] != INVALID_SOCKET) continue;
		socks[i].clientSock = 0;
	}

	HANDLE worker_thread_handler;
	while (1) {
		//wait for network events on all socket
		index = WSAWaitForMultipleEvents(nEvents, events, FALSE, WSA_INFINITE, FALSE);
		if (index == WSA_WAIT_FAILED) {
			printf("Error %d: WSAWaitForMultipleEvents() failed\n", WSAGetLastError());
		}

		index = index - WSA_WAIT_EVENT_0;
		//identify type of event happened
		WSAEnumNetworkEvents(socks[index].clientSock, events[index], &sockEvent);

		//reset event
		WSAResetEvent(events[index]);
		if (index == 0) {
			if (sockEvent.lNetworkEvents & FD_ACCEPT) {
				if (sockEvent.iErrorCode[FD_ACCEPT_BIT] != 0) {
					printf("FD_ACCEPT failed with error %d\n", sockEvent.iErrorCode[FD_READ_BIT]);
				}

				if ((connSock = accept(socks[index].clientSock, (sockaddr *)&clientAddr, &clientAddrLen)) == SOCKET_ERROR) {
					printf("Error %d: Cannot permit incoming connection.\n", WSAGetLastError());
				}

				//Add new socket into socks array
				int i;
				if (nEvents == WSA_MAXIMUM_WAIT_EVENTS) {
					//check if there is no thread was created before
					if (thread_status == 0)
					{
						SOCKET param[2];
						param[0] = listenSock;
						param[1] = connSock;
						printf("Maximum clients reached: new worker thread will be created.\n");
						thread_status = 1;
						worker_thread_handler = (HANDLE)_beginthreadex(0, 0, worker_thread, (void*)param, 0, 0);
					}
				}
				else {
					socks[nEvents].clientSock = connSock;
					events[nEvents] = WSACreateEvent();
					socks[nEvents].clientAddr = clientAddr;
					WSAEventSelect(socks[nEvents].clientSock, events[nEvents], FD_READ | FD_CLOSE);
					nEvents++;
				}
			}
		}
		else {
			if (sockEvent.lNetworkEvents & FD_READ) {
				//Receive message from client
				if (sockEvent.iErrorCode[FD_READ_BIT] != 0) {
					printf("FD_READ failed with error %d\n", sockEvent.iErrorCode[FD_READ_BIT]);
					continue;
				}
				//reset event
				WSAResetEvent(events[index]);
				//call function to handle
				messageHandling(socks[index], socks, index, nEvents, events);
			}

			if (sockEvent.lNetworkEvents & FD_CLOSE) {
				if (sockEvent.iErrorCode[FD_CLOSE_BIT] != 0) {
					printf("An account has unexpectedly disconnected \n");
				}
				//Release socket and event
				closesocket(socks[index].clientSock);
				WSACloseEvent(events[index]);
				socks[index] = socks[nEvents - 1];
				events[index] = events[nEvents - 1];
				socks[nEvents - 1].clientSock = 0;
				events[nEvents - 1] = 0;
				nEvents--;
			}
		}
	}
	return 0;

}



int main(int argc, char* argv[])
{

	//Initiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Winsock 2.2 is not supported\n");
		return 0;
	}

	//Construct LISTEN socket	
	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);


	if (bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error %d: Cannot associate a local address with server socket.", WSAGetLastError());
		return 0;
	}

	// Listen request from client
	if (listen(listenSock, 10)) {
		printf("Error %d: Cannot place server socket in state LISTEN.", WSAGetLastError());
		return 0;
	}

	printf("Server started!\n");
	loadAccount(nameAndPass1);
	loadEvent(eventNameMap1, countEvent);
	// bind listen sock to the first worker thread
	SOCKET param[2];
	param[0] = listenSock;
	param[1] = INVALID_SOCKET;

	_beginthreadex(0, 0, worker_thread, (void*)param, 0, 0);
	while (1) {
		// keep the server loop 
	}
	return 0;
}
