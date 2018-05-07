
#include "stdafx.h"
#pragma warning(disable:4996)

#include "Functions.h"


bool isFileName(string link)
{
	unsigned int len = link.length(), i = 0, j = 0;
	for (i = len - 1; j <= 5; i--, j++)
		if (link[i] == '.')
			return true;
	return false;
}

bool isValidFolderName(string link)
{
	unsigned int len = link.length();
	if (link[len - 1] != '/')
		return false;
	string invalidChar = ":/\\?*\"><|";
	for (unsigned int i = len - 2; link[i] != '/'; i--)
		for (int j = 0; j < invalidChar.length(); j++)
			if (link[i] == invalidChar[j])
				return false;
	return true;
}

bool isConnectionClosed(string responseMessage)
{
	if (responseMessage.find("keep-alive") < responseMessage.length())
		return false;
	return true;
}

char* string2CharArray(string content)
{
	unsigned int length = content.length(), i = 0;
	char* resault = new char[length + 1];
	for (i = 0; i < length; i++)
		resault[i] = content[i];
	resault[length] = '\0';
	return resault;
}

int string2Int(string stringNumber)
{
	int number = 0, temp = 1;
	for (int i = stringNumber.length() - 1; i >= 0; i--) {
		number += temp*(stringNumber[i] - 48);
		temp *= 10;
	}
	return number;
}

int stringHex2IntDec(string stringNumber)
{
	string hexa = "0123456789abcdef";
	int temp = 1, resault = 0;
	for (int i = stringNumber.length() - 1; i >= 0; i--)
	{
		resault += hexa.find(stringNumber[i])*temp;
		temp *= 16;
	}
	return resault;
}

char* getDomainName(string link)
{
	string host = "";
	for (unsigned int i = strlen("http://"); link[i] != '/'; i++)
		host += link[i];
	return string2CharArray(host);
}

string getFolderName(string link)
{
	
	unsigned int len = link.length(), i = 0;
	for (i = len - 2; link[i] != '/'; i--);
	string folderName = link.substr(i + 1);
	folderName.resize(folderName.length() - 1);
	return folderName;
}

string getFileSaveName(string link, string mssv)
{
	int len = link.length(), i = 0;
	for (i = len - 1; link[i] != '/'; i--);
	string fileName = mssv + "_";
	if (mssv.empty()) fileName = "";
	for (int j = i + 1; j < len; j++)
		fileName += link[j];
	for (i = fileName.find("%20"); i != -1; i = fileName.find("%20"))
		fileName = fileName.replace(i, 3, " ");
	return fileName;
}

bool isEndMessage(string message, string lineSeparator)
{
	if (message.length() < lineSeparator.length())
		return false;
	string end = message.substr(message.length() - lineSeparator.length());
	if (end == lineSeparator)
		return true;
	return false;
}

int getContentLength(string responseMessage)
{
	int position = responseMessage.find("Content-Length");
	if (position == -1) return -1;
	
	position += strlen("Content-Length: ");
	string stringNumber = "";
	for(; responseMessage[position] != '\r'; position++)
		stringNumber += responseMessage[position];
	
	return string2Int(stringNumber);
}

int getChunkLength(int Socket)
{
	char one_char;
	string chunkLength = "";
	do
	{
		int byte = recv(Socket, &one_char, 1, 0);
		if (byte == SOCKET_ERROR)
			return -1;
		chunkLength += one_char;
	} while (!isEndMessage(chunkLength, CRLF));
	chunkLength.resize(chunkLength.length() - 2); // loai di 2 ki tu \r\n
	return stringHex2IntDec(chunkLength);
}

void printFileInfo(string link, int dataLengthLeft)
{
	cout << "\n----------------------------------\n";
	cout << "\t* FileName       : " << getFileSaveName(link, "") << endl;
	cout << "\t* Content-Length : " << dataLengthLeft << " bytes" << endl;
	cout << "------------------------------------\n" << endl;
}

vector<string> getListOfFileName(string htmlContent, string link)
{
	vector<string> fileList;
	int pos = htmlContent.find("href"), i = 0;
	while (pos != -1)
	{
		htmlContent = htmlContent.substr(pos + 6);
		for (i = 0; htmlContent[i] != '\"'; i++);

		string fileName = htmlContent.substr(0, i);
		if (isFileName(fileName) || isValidFolderName(fileName))
			fileList.push_back(link + fileName);
		
		pos = htmlContent.find("href");
	}
	return fileList;
}

void printProgress(int dataLeft, int data)
{
	int percent = (int)(100 - float(dataLeft) / data * 100);
	if (percent % 20 == 0)
		cout << "\t" << percent << "\%";
}