#ifndef EventController
#define EventController

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

using namespace std;

#define BUFF_SIZE 2048
#define ENDING_DELIMITER "\r\n"

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
 
string createEvent(string s, map<string, string>&eventNameMap1, int& countEvent, map<string, string>&nameAndPass1);

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
string inviteHandling(string s, map<string, string>&nameAndPass1, map<string, string>&eventNameMap1);

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
string inviteReplyHandling(string s, map<string, string>&nameAndPass1, map<string, string>&eventNameMap1);

/**
*@function requestHandling: handle request message from client.
*
@param s: request message from client
*
@return: code 99 if error occurs
		 code 60 if request success
**/
string requestHandling(string s);

/**
*@function requestReplyHandling: handle request reply message from client.
*
@param s: request message from client
*
@return: code 99 if error occurs
		 code 70 if success
**/
string requestReplyHandling(string s);

/**
*@function listRequest: list all request from other client to requested client.
*
@param user: requested user
*
@return: code 99 if error occurs
         code 70 if success
**/
string listRequest(string user);

/**
*@function getUserAttendList: get all user attend the Event.
*
@param id: a string contain event's id
*
*@return:
*userList List of invitation. Each Invitaion is concat to each other by '#'
**/
string getUserAttendList(string id);

/**
*@function getInvitationList: get all invitation that was sent to user.
*
@param userName: a string contain username who want to see he/she 's Invitation List
*
*@return:
*invitationList List of invitation. Each Invitaion is concat to each other by '#'
**/
string getInvitationList(string userName);

/**
*@function getInvitationReply: get all reply result of the invitations that was sent to other.
*
@param userName: a string name of the user want to check reply
*
*@return:
*invitationReplyList:  List of invitation reply. Each Invitaion reply is concat to each other by '#'
**/
string getInvitationReply(string userName);

/**
*@function getRequestReply: get all reply result of the requests that was sent to event creator.
*
@param userName: a string name of the user want to check request result
*
*@return:
*requestReplyList:  List of request reply. Each request reply is concat to each other by '#'
**/
string getRequestReply(string userName);

/**
*@function getLine: get position of the invitation that was stored in Invitation.txt file.
*
*@param s: the invitation that need to find position
*
*@return:
*0 if the invitation not exist
*pos: if position of the invitation is pos
**/
int getLine(string s);

/**
*@function getEventInforById: get Event infor corresponding with eventId.
*
@param n: event id in Number
*
*@return:
*eventInfor: event Information
**/
string getEventInforById(int n);

/**
*@function getEventList: get all event.
*
@param: no param
*
*@return:
*eventList: List of Event. Each Event is concat to each other by '#'
**/
string getEventList();
#endif