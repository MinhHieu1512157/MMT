
#include "stdafx.h"
#pragma warning(disable:4996)

#include <afxsock.h>
#include <afxinet.h>
#include "Functions.h"


sockaddr_in getServerSocket(string link)
{
	struct sockaddr_in ServerSocket;
	hostent *domain;
	domain = gethostbyname(getDomainName(link));
	char* IP = inet_ntoa(*(struct in_addr *)*domain->h_addr_list);
	ServerSocket.sin_addr.s_addr = inet_addr(IP);
	ServerSocket.sin_family = AF_INET;
	ServerSocket.sin_port = htons(80);
	return ServerSocket;
}

char* getRequestMessage(string link, string type)
{
	string host = getDomainName(link), message = "";
	string http = (type == "--http1.0") ? string("HTTP/1.0") : string("HTTP/1.1");
	string filePath = string(link).substr(host.length() + 7, link.length() - 7 - host.length()); // 7 = strlen("http://")

	if (!isFileName(link))
		message = "GET " + filePath + " " + http + "\r\nHost: " + host + "\r\nUser-Agent: fetch.c\r\n\r\n";
	else
		message = "GET " + filePath + " " + http + "\r\nHost: " + host + "\r\n\r\n";

	return string2CharArray(message);
}

string getResponseMessage(int Socket)
{
	char one_char;
	string responseMessage = "";
	do
	{
		int byte = recv(Socket, &one_char, 1, 0);
		if (byte == 0) break;
		
		// bat loi khi nhan response message
		if (byte == SOCKET_ERROR)
			return "";

		responseMessage += one_char;
	} while (!isEndMessage(responseMessage, CRLFx2));
	return responseMessage;
}

bool getResponseMessage(int Socket, string link, string type, string &response)
{
	response.clear();
	cout << "Send Request: ";
	char* requestMessage = getRequestMessage(link, type);
	if (send(Socket, requestMessage, strlen(requestMessage), 0) < 0) {
		cout << "Failed!" << endl;
		return false;
	}
	cout << "Successful!" << endl;

	cout << "Request:\n" << requestMessage << endl;
	cout << "Get Response: ";
	response = getResponseMessage(Socket);
	if (response.empty()) {
		cout << "Failed!" << endl;
		return false;
	}
	cout << "Successful!" << endl;
	return true;
}

void MakeDir(string folderName)
{
	char* mkdirCommand = string2CharArray("mkdir " + folderName);
	system(mkdirCommand);
}

bool downLoadFileViaContentLength(FILE *f, int dataLength, int Socket)
{
	char buffer[BUFFER_SIZE];
	int dataLengthLeft = dataLength;
	do
	{
		int bufferLength = (dataLengthLeft >= BUFFER_SIZE) ? BUFFER_SIZE : dataLengthLeft;
		int bytes = recv(Socket, buffer, bufferLength, 0);
		if (bytes == SOCKET_ERROR)
			return false;

		fwrite(buffer, bytes, sizeof(char), f);
		dataLengthLeft -= bytes;
		//printProgress(dataLengthLeft, dataLength);

	} while (dataLengthLeft > 0);
	return true;
}

bool downLoadFileViaChunks(FILE *f, int Socket)
{
	char crlf_buffer[2];
	int chunkLength = -1;
	while (1)
	{
		chunkLength = getChunkLength(Socket);
		if (chunkLength == 0)	break;
		if (chunkLength == -1)	return false;
		if (!downLoadFileViaContentLength(f, chunkLength, Socket))
			return false;
		if (recv(Socket, crlf_buffer, 2, 0) < 0) // bo qua 2 ki tu \r\n cuoi moi chunk
			return false;
	}
	if (recv(Socket, crlf_buffer, 2, 0) < 0) // bo qua 4 ki tu \r\n cuoi cung
		return false;
	return true;
}

bool downLoadFile(string fileName, string response, int Socket)
{	
	cout << "Download file [" << fileName << "] : ";
	char* fName = string2CharArray(fileName);
	FILE *f = fopen(fName, "wb");
	if (!f) { cout << "Failed!" << endl; return false; }
	
	bool done = false;
	if (response.find("Length") < response.length())
		done = downLoadFileViaContentLength(f, getContentLength(response), Socket);
	else if (response.find("chunked") < response.length())
		done = downLoadFileViaChunks(f, Socket);
	fclose(f);
	
	if (done) {
		cout << "Successful!" << endl;
		return true;
	}
	cout << "Failed!" << endl;
	return false;
}

bool getWebContentViaLength(string &content, int dataLength, int Socket)
{
	char buffer[BUFFER_SIZE];
	int dataLengthLeft = dataLength;
	do
	{
		int bufferLength = (dataLengthLeft >= BUFFER_SIZE) ? BUFFER_SIZE : dataLengthLeft;
		int bytes = recv(Socket, buffer, bufferLength, 0);
		if (bytes == SOCKET_ERROR)
			return false;

		for (int i = 0; i < bytes; i++)
			content += buffer[i];
		dataLengthLeft -= bytes;
		//printProgress(dataLengthLeft, dataLength);

	} while (dataLengthLeft > 0);
	return true;
}

bool getWebContentViaChunks(int Socket, string &content)
{
	char crlf_buffer[2];
	int chunkLength = -1;
	content.clear();
	while (1)
	{
		chunkLength = getChunkLength(Socket);
		if (chunkLength == 0)	break;
		if (chunkLength == -1)	return false;
		if (!getWebContentViaLength(content, chunkLength, Socket))
			return false;
		if (recv(Socket, crlf_buffer, 2, 0) < 0) // bo qua 2 ki tu \r\n cuoi moi chunk
			return false;
	}
	if (recv(Socket, crlf_buffer, 2, 0) < 0) // bo qua 2 ki tu \r\n cuoi cung
		return false;
	return true;
}

bool getWebContent(int Socket, string response, string &content)
{
	cout << "Preparing to download folder. Getting content of file index.html: \n";
	if (response.find("chunked") < response.length())
	{
		if (getWebContentViaChunks(Socket, content)) {
			cout << "Successful!" << endl;
			return true;
		}
		cout << "Failed!" << endl;
		return false;
	}
	char buffer[BUFFER_SIZE + 1];
	while (1) {
		int bytes = recv(Socket, buffer, BUFFER_SIZE, 0);
		
		if (bytes == 0)		break;
		if (bytes == -1){
			cout << "Failed!" << endl;
			return false;
		}
		content.append(buffer, bytes);
	}
	cout << "Successful!" << endl;
	return true;
}

