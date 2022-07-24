
#include <iostream>
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <stdlib.h>
#include <vector>
#define PORT 5500
#define BUFF_SIZE 2048
#define SERVER_ADDR "127.0.0.1"
#define ENDING_DELIMITER "\r\n"
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
vector<string> result; //store parsed data


void parseMessage(string s) {
	int pos = 0;
	string token;
	while ((pos = s.find(ENDING_DELIMITER)) != string::npos) {//find all positions of ENDING_DELIMITER
		token = s.substr(0, pos);
		result.push_back(token);
		s.erase(0, pos + 2);
	}
}

/**
*	@function sendMessage: send message to client
*	@param connSocket: client connecting socket
*	@param message: a string contain message
*	@return: NULL
**/
void sendMessage(SOCKET connSocket, string message) {
	message.append(ENDING_DELIMITER);
	int byteCounting = message.size();
	string sendToServerMessage;
	string tmpStr = message;
	//sending all messages to client until there is none left
	while (byteCounting > 0) {
		sendToServerMessage = tmpStr.substr(0, BUFF_SIZE - 1);
		int retToServer = send(connSocket, sendToServerMessage.c_str(), sendToServerMessage.size(), 0);
		if (retToServer == SOCKET_ERROR) {
			printf("Error %d: Cannot send data.", WSAGetLastError());
		}
		byteCounting -= retToServer;
		tmpStr.erase(0, retToServer);
	}
}

/**
*	@function inputMessage: type in message
*	@return: a string contain message
**/
string inputMessage() {
	string message;
	getline(cin, message);
	return message;
}


int main(int argc, char* argv[]) {

	
	//Step 1: Initiate Winsock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Winsock 2.2 is not supported\n");
		return 0;
	}

	//Step 2: Construct socket
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client == INVALID_SOCKET) {
		printf("Error %d: Cannot create server socket.", WSAGetLastError());
		return 0;
	}

	//(optional) Set time-out for receiving
	//int tv = 10000; //time-out interval: 10000ms
	//setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));

	//Step 3: Specify server address
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);

	//Step 4: Request to connect server
	if (connect(client, (sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error %d: Cannot connect server.", WSAGetLastError());
		return 0;
	}

	printf("Connected server!\n");

	//Step 5: Communicate with server
	while (1) {
		char buff[BUFF_SIZE];
		string currentUser = "Empty";
		while (1) {
			cout << "Current user: " << currentUser << endl;
			cout << "================MENU================" << endl;
			cout << "1.Test Create Account" << endl;
			cout << "2.Test Login" << endl;
			cout << "3.Test Create Event" << endl;
			cout << "4.Test Logout" << endl;
			cout << "5.Test Invite" << endl;
			cout << "6.Test Invite Reply" << endl;
			cout << "7.Test Request" << endl;
			cout << "8.Test Request Reply" << endl;
			cout << "9.Test Refresh Request" << endl;
			cout << "10.Test Refresh Invite" << endl;
			cout << "11.Test List User" << endl;
			cout << "12.Test List Event" << endl;
			cout << "Please choose one of 3 options:" << endl;
			string selectionStr;
			int selection;
			while (1) {//type in selection until it is a integer value
				int isDigit = 1;
				selectionStr = inputMessage();
				for (int i = 0; i < selectionStr.size(); i++) {
					if (isalpha(selectionStr[i])) {//check every elements of selectionStr if there is one non-integer element, re-do type-in process
						isDigit = 0;
						break;
					}
				}
				if (isDigit == 0) {
					cout << "Please type in integer value" << endl;
				}
				else break;
			}
			selection = stoi(selectionStr);
			switch (selection) {
			case 1: {
				string username;
				string password;
				cout << "Type in your username: ";
				username = inputMessage();
				cout << "Type in your password: ";
				password = inputMessage();
				string messageToServer = "CREATEUSER " + username + "|" + password + "|";
				string messageToClient;
				sendMessage(client, messageToServer);
				while (1) {
					int retToClient = recv(client, buff, BUFF_SIZE, 0);
					messageToClient.append(buff);
					if (messageToClient.find(ENDING_DELIMITER)) break;
				}
				parseMessage(messageToClient);
				for (int i = 0; i < result.size(); i++) {
					cout << result[i] << endl;
				}
				messageToClient.erase();
				result.clear();
				break;
			}
			case 2: {
				string messageToServer;
				string messageToClient;
				string username;
				string password;
				cout << "Type in your username: ";
				username = inputMessage();
				cout << "Type in your password: ";
				password = inputMessage();
				messageToServer = "USER " + username + "|" + password + "|";
				sendMessage(client, messageToServer);
				while (1) {
					int retToClient = recv(client, buff, BUFF_SIZE, 0);
					messageToClient.append(buff);
					if (messageToClient.find(ENDING_DELIMITER)) break;
				}
				parseMessage(messageToClient);
				for (int i = 0; i < result.size(); i++) {
					cout << result[i] << endl;
				}
				result.clear();
				break;
			}
			case 3: {
				string messageToServer;
				string messageToClient;
				messageToServer = "CREATEEVENT ";
				string creator, evName, evTime, evPlace, evDes;

				cout << "Type in your username: ";
				creator = inputMessage();
				cout << "Type in event name: ";
				evName = inputMessage();
				cout << "Type in event time: ";
				evTime = inputMessage();
				cout << "Type in event place: ";
				evPlace = inputMessage();
				cout << "Type in event description: ";
				evDes = inputMessage();
				
				messageToServer = messageToServer + creator + "|" + evName + "|" + evPlace + "|" + evTime + "|" + evDes + "|";
				cout << messageToServer << endl;
				sendMessage(client, messageToServer);
				while (1) {
					int retToClient = recv(client, buff, BUFF_SIZE, 0);
					messageToClient.append(buff);
					if (messageToClient.find(ENDING_DELIMITER)) break;
				}
				parseMessage(messageToClient);
				for (int i = 0; i < result.size(); i++) {
					cout << result[i] << endl;
				}
				result.clear();//clear vector for next use
				break;
			}
			case 4: {//LOGOUT
				string messageToServer;
				string messageToClient;
				messageToServer = "BYE ";
				cout << messageToServer << endl;
				sendMessage(client, messageToServer);
				while (1) {
					int retToClient = recv(client, buff, BUFF_SIZE, 0);
					messageToClient.append(buff);
					if (messageToClient.find(ENDING_DELIMITER)) break;
				}
				parseMessage(messageToClient);
				for (int i = 0; i < result.size(); i++) {
					cout << result[i] << endl;
				}
				result.clear();//clear vector for next use
				break;
			}
			case 5: {//INVITE
				string messageToServer;
				string messageToClient;
				string sender;
				string receiver;
				string id;
				cout << "Type in your sender: ";
				sender = inputMessage();
				cout << "Type in your receiver: ";
				receiver = inputMessage();
				cout << "Type in your eventId: ";
				id = inputMessage();
				messageToServer = "INVITE " + sender + "|" + receiver + "|" +  id + "|";
				sendMessage(client, messageToServer);
				while (1) {
					int retToClient = recv(client, buff, BUFF_SIZE, 0);
					messageToClient.append(buff);
					if (messageToClient.find(ENDING_DELIMITER)) break;
				}
				parseMessage(messageToClient);
				for (int i = 0; i < result.size(); i++) {
					cout << result[i] << endl;
				}
				result.clear();
				break;
			}
			case 6: {//INVITEREPLY
				string messageToServer;
				string messageToClient;
				string sender;
				string receiver;
				string id;
				cout << "Type in your sender: ";
				sender = inputMessage();
				cout << "Type in your receiver: ";
				receiver = inputMessage();
				cout << "Type in your eventId: ";
				id = inputMessage();
				messageToServer = "INVITEREPLY " + sender + "|" + receiver + "|" + id + "|" + "OK" + "|";
				sendMessage(client, messageToServer);
				while (1) {
					int retToClient = recv(client, buff, BUFF_SIZE, 0);
					messageToClient.append(buff);
					if (messageToClient.find(ENDING_DELIMITER)) break;
				}
				parseMessage(messageToClient);
				for (int i = 0; i < result.size(); i++) {
					cout << result[i] << endl;
				}
				result.clear();
				break;
			}

			case 7: {//REQUEST
				string messageToServer;
				string messageToClient;
				string sender;
				string receiver;
				string id;
				cout << "Type in your sender: ";
				sender = inputMessage();
				cout << "Type in your receiver: ";
				receiver = inputMessage();
				cout << "Type in your eventId: ";
				id = inputMessage();
				messageToServer = "REQUEST " + sender + "|" + receiver + "|" + id + "|";
				sendMessage(client, messageToServer);
				while (1) {
					int retToClient = recv(client, buff, BUFF_SIZE, 0);
					messageToClient.append(buff);
					if (messageToClient.find(ENDING_DELIMITER)) break;
				}
				parseMessage(messageToClient);
				for (int i = 0; i < result.size(); i++) {
					cout << result[i] << endl;
				}
				result.clear();
				break;
			}

			case 8: {//REQUESTREPLY
				string messageToServer;
				string messageToClient;
				string sender;
				string receiver;
				string id;
				cout << "Type in your sender: ";
				sender = inputMessage();
				cout << "Type in your receiver: ";
				receiver = inputMessage();
				cout << "Type in your eventId: ";
				id = inputMessage();
				messageToServer = "REQUESTREPLY " + sender + "|" + receiver + "|" + id + "|" + "OK" + "|";
				sendMessage(client, messageToServer);
				while (1) {
					int retToClient = recv(client, buff, BUFF_SIZE, 0);
					messageToClient.append(buff);
					if (messageToClient.find(ENDING_DELIMITER)) break;
				}
				parseMessage(messageToClient);
				for (int i = 0; i < result.size(); i++) {
					cout << result[i] << endl;
				}
				result.clear();
				break;
			}

			case 9: {//REFRESHREQUEST
				string messageToServer;
				string messageToClient;
				string sender;
				string id;
				cout << "Type in your sender: ";
				sender = inputMessage();
				messageToServer = "REFRESHREQUEST " + sender;
				
				sendMessage(client, messageToServer);
				while (1) {
					int retToClient = recv(client, buff, BUFF_SIZE, 0);
					messageToClient.append(buff);
					if (messageToClient.find(ENDING_DELIMITER)) break;
				}
				parseMessage(messageToClient);
				for (int i = 0; i < result.size(); i++) {
					cout << result[i] << endl;
				}
				result.clear();
				break;
			}

			case 10: {//REFRESHINVITE
				string messageToServer;
				string messageToClient;
				string sender;
				cout << "Type in your sender: ";
				sender = inputMessage();
				messageToServer = "REFRESHINVITE " + sender;
				sendMessage(client, messageToServer);
				while (1) {
					int retToClient = recv(client, buff, BUFF_SIZE, 0);
					messageToClient.append(buff);
					if (messageToClient.find(ENDING_DELIMITER)) break;
				}
				parseMessage(messageToClient);
				for (int i = 0; i < result.size(); i++) {
					cout << result[i] << endl;
				}
				result.clear();
				break;
			}

			case 11: {//LISTUSER
				string messageToServer;
				string messageToClient;
				messageToServer = "LISTUSER \r\n" ;
				sendMessage(client, messageToServer);
				while (1) {
					int retToClient = recv(client, buff, BUFF_SIZE, 0);
					messageToClient.append(buff);
					if (messageToClient.find(ENDING_DELIMITER)) break;
				}
				parseMessage(messageToClient);
				for (int i = 0; i < result.size(); i++) {
					cout << result[i] << endl;
				}
				result.clear();
				break;
			}

			case 12: {//LISTEVENT
				string messageToServer;
				string messageToClient;

				if (messageToServer == "") break;
				messageToServer = "LISTUSER ";
				sendMessage(client, messageToServer);
				while (1) {
					int retToClient = recv(client, buff, BUFF_SIZE, 0);
					messageToClient.append(buff);
					if (messageToClient.find(ENDING_DELIMITER)) break;
				}
				parseMessage(messageToClient);
				for (int i = 0; i < result.size(); i++) {
					cout << result[i] << endl;
				}
				result.clear();
				break;
			}
			default: {
				cout << "Unknown action, please choose again" << endl;
				continue;
			}
			}
		}
	}
}




