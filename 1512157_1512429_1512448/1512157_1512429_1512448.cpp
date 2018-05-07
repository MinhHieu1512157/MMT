// 1512157_1512429_1512448.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "stdafx.h"
#include "1512157_1512429_1512448.h"
#include <afxsock.h>
#include <afxinet.h>
#include "Functions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable:4996)
// The one and only application object

CWinApp theApp;

using namespace std;

int main(int argc, char *argv[])
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: change error code to suit your needs
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: code your application's behavior here.
			if (AfxSocketInit() == FALSE)
			{
				cout << "\nCannot create Socket Libraray" << endl;
				return 0;
			}
			/*
			if (argc != 3) {
				cout << "Invalid number of parameters!" << endl;
				return 0;
			}
			
			string mssv = string(argv[0]);
			mssv.resize(mssv.length() - 4);
			string temp = string(argv[1]);
			string type, link;
			if (temp.find("--http")) {
				link = temp;
				type = string(argv[2]);
			}
			else {
				type = temp;
				link = string(argv[2]);
			}*/
			
			
			string link = "http://www.ietf.org/rfc/rfc3261.txt";
			string mssv = "1512157_1512429_1512448";
			string type = "--http1.0";
			

			cout << "MSSV: " << mssv << "    -    HTTP type: " << type << endl;
			cout << "Link: " << link << endl;
			
			//---------------------------------------------------------------------------------
			//---------------------------------------------------------------------------------
			cout << "Creating Socket: ";
			int Socket = socket(AF_INET, SOCK_STREAM, 0);
			if (Socket == -1) {
				cout << "Failed!" << endl;  return 0; }
			cout << "Successful!" << endl;
			//---------------------------------------------------------------------------------
			//---------------------------------------------------------------------------------
			cout << "Connecting to Server: ";
			struct sockaddr_in ServerSocket = getServerSocket(link);
			if (connect(Socket, (struct sockaddr*)&ServerSocket, sizeof(ServerSocket)) < 0) {
				cout << "Failed!" << endl; return 0; }
			cout << "Successful!" << endl;
			//---------------------------------------------------------------------------------
			//---------------------------------------------------------------------------------
			string responseMessage = "";
			if (!getResponseMessage(Socket, link, type, responseMessage))
				goto CloseSocket;

			cout << "Response:\n" << responseMessage << endl;
			//goto CloseSocket;
			//---------------------------------------------------------------------------------
			//---------------------------------------------------------------------------------
			if (isFileName(link))
			{
				string fileSaveName = getFileSaveName(link, mssv);
				if (!downLoadFile(fileSaveName, responseMessage, Socket))
					goto CloseSocket;
			}
			else
			{
				//---------------------------------------------------------------------------------
				//---------------------------------------------------------------------------------
				string content = "";
				if (!getWebContent(Socket, responseMessage, content))	
					return 0;

				//---------------------------------------------------------------------------------
				//---------------------------------------------------------------------------------
				string folderName = string(mssv) + "_" + getFolderName(link);
				MakeDir(folderName);
				vector<string> fileNameList = getListOfFileName(content,link);

				for (unsigned int i = 1; i < fileNameList.size(); i++)
				{
					cout << "----------------------------------------------------------\n";
					if (isFileName(fileNameList[i])) {
						cout << "Downloading [" << getFileSaveName(fileNameList[i], "") << "] :\n";
						//---------------------------------------------------------------------------------
						//---------------------------------------------------------------------------------
						if (isConnectionClosed(responseMessage)) {
							cout << "Connection is closed! Re-connecting to Server: ";
							if (shutdown(Socket, 2) != 0) {
								cout << "Failed!" << endl;  return 0; }
							Socket = socket(AF_INET, SOCK_STREAM, 0);
							if (Socket == -1) {
								cout << "Failed!" << endl;  return 0; }
							if (connect(Socket, (struct sockaddr*)&ServerSocket, sizeof(ServerSocket)) < 0) {
								cout << "Failed!" << endl; return 0; }
							cout << "Successful!" << endl;
						}
						//---------------------------------------------------------------------------------
						//---------------------------------------------------------------------------------
						if (!getResponseMessage(Socket, fileNameList[i], type, responseMessage))
							goto CloseSocket;
						string fileSaveName = folderName + "/" + getFileSaveName(fileNameList[i], mssv).substr(mssv.length() + 1);
						if (!downLoadFile(fileSaveName, responseMessage, Socket))
							goto CloseSocket;
					}
					else {
						string subfolderName = getFolderName(fileNameList[i]);
						MakeDir(folderName + "\\" + subfolderName);
						cout << "Creating subfoler: " << subfolderName << " successful!" << endl;
					}
					cout << "----------------------------------------------------------\n\n";
				}
			}
		CloseSocket:
			shutdown(Socket, 2);
		}
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }

    return nRetCode;
}
