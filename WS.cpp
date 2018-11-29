#include "WS.h"

WebSock::WebSock(std::string hostname, int port) 
{
	using Poco::Net::HTTPClientSession;
	using Poco::Net::HTTPRequest;
	using Poco::Net::HTTPResponse;
	using Poco::Net::HTTPMessage;
	using Poco::Net::WebSocket;
	Poco::Net::HTTPClientSession httpSession(hostname, port);
	HTTPRequest request(HTTPRequest::HTTP_GET, "/", HTTPMessage::HTTP_1_1);
	request.set("origin", "nav");
	request.set("Sec-WebSocket-Protocol", "nav");
	request.set("host", "smartmirror.me");
	request.set("upgrade", "websocket");
	request.set("connection", "upgrade");
	HTTPResponse resp;
	this->webSocket = new WebSocket(httpSession, request, resp);
/*	char receiveBuff[1024];
	int flags = 0;
	int rlen = this->webSocket->receiveFrame(receiveBuff, 256, flags);
	std::cout << "Received: " << receiveBuff << std::endl;
*/
}

void WebSock::Send(std::string payload)
{	
	using Poco::Net::WebSocket;
	try
	{
		char rbuff[1024];
		int flags = 0;
		int len = this->webSocket->sendFrame(payload.data(), (int)payload.size());
		std::cout << "Sent " << len << " bytes" << std::endl;
	}
	catch(std::exception &e) 
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}
}
