#ifndef _WS_H_
#define _WS_H_
#include "Common.h"
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPMessage.h>
#include <Poco/Net/WebSocket.h>
#include <Poco/Net/HTTPClientSession.h>
#endif

class WebSock 
{
public:
	WebSock(std::string hostname, int port);
	~WebSock();
	void Send(std::string payload);
private:
	std::string HostName;
	int PortNum;
	Poco::Net::WebSocket * webSocket;
	void InitWS(std::string hostname, int port);
};
