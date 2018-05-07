#pragma once

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#define BUFFER_SIZE 4096
#define CRLF "\r\n"
#define CRLFx2 "\r\n\r\n"
using namespace std;


// ExtraFunctions.cpp -------------------------------------------------------------
int string2Int(string stringNumber);
int stringHex2IntDec(string stringNumber);
bool isFileName(string link);
bool isValidFolderName(string link);
bool isConnectionClosed(string responseMessage);
bool isEndMessage(string message, string lineSeparator);
char* string2CharArray(string content);
char* getDomainName(string link);
string getFileSaveName(string link, string mssv);
string getFolderName(string link);
int getContentLength(string responseMessage);
int getChunkLength(int Socket);
void printFileInfo(string link, int dataLengthLeft);
void printProgress(int dataLeft, int data);
vector<string> getListOfFileName(string htmlContent, string link);


// MainFunctions.cpp -------------------------------------------------------------
sockaddr_in getServerSocket(string link);
char* getRequestMessage(string link, string type);
string getResponseMessage(int Socket);
bool getResponseMessage(int Socket, string link, string type, string &response);
bool downLoadFileViaContentLength(FILE *f, int dataLength, int Socket);
bool downLoadFileViaChunks(FILE *f, int Socket);
bool downLoadFile(string fileName, string response, int Socket);
bool getWebContentViaLength(string &content, int dataLength, int Socket);
bool getWebContentViaChunks(int Socket, string &content);
bool getWebContent(int Socket, string response, string &content);
void MakeDir(string folderName);

