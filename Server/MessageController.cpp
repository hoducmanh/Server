#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include "process.h"
#include <map>
#include <vector>
#include "Data.h"
#define BUFF_SIZE 2048
#define ENDING_DELIMITER "\r\n"
using namespace std;

vector<string> parseMessage(string s) {
	vector<string> result;
	int pos = 0;
	string token;
	while ((pos = s.find(ENDING_DELIMITER)) != string::npos) {//find all positions of ENDING_DELIMITER

		token = s.substr(0, pos);
		result.push_back(token);
		s.erase(0, pos + 2);
	}
	return result;
}

vector<string> receiveMessage(socketInfo* client, DWORD &index, DWORD &nEvents, WSAEVENT *events) {
	string str = "";
	char buff[BUFF_SIZE];
	while (1) {
		int ret = recv(client[index].clientSock, buff, BUFF_SIZE - 1, 0);
		if (ret == 0 || ret == SOCKET_ERROR) {//client sent nothing or client disconnected
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				cout << "Client disconnected! " << endl;
				closesocket(client[index].clientSock);
				WSACloseEvent(events[index]);
				//delete that client from array of clients and events
				client[index] = client[nEvents - 1];
				events[index] = events[nEvents - 1];
				client[nEvents - 1].clientSock = 0;
				events[nEvents - 1] = 0;
				nEvents--;
				break;
			}
		}
		else {
			buff[ret] = 0;
			str.append(buff);
			if (buff[ret - 2] == '\r' && buff[ret - 1] == '\n') break;
		}
	}
	vector<string> result;
	result = parseMessage(str);
	return result;
}

void sendMessage(SOCKET connSocket, string message) {
	message.append(ENDING_DELIMITER);
	int byteCounting = message.size();
	string sendToClientMessage;
	string tmpStr = message;
	//sending all messages to client until there is none left
	while (byteCounting > 0) {
		sendToClientMessage = tmpStr.substr(0, BUFF_SIZE - 1);
		int retToClient = send(connSocket, sendToClientMessage.c_str(), sendToClientMessage.size(), 0);
		if (retToClient == SOCKET_ERROR) {
			printf("Error %d: Cannot send data.", WSAGetLastError());
		}
		byteCounting -= retToClient;
		tmpStr.erase(0, retToClient);
	}
}


