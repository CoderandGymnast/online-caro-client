// online-caro-client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/* TCPClient.cpp : Defines the entry point for the console application. */

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#undef UNICODE

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <math.h> 
#include <string>
#include <iostream>
#include <thread>
#include <fstream>

#pragma comment (lib, "Ws2_32.lib")

#define BUFF_SIZE 2048
#define RESPONSE_SIZE 3
#define TIME_OUT 3

using namespace std;

int setupDeamonCli(SOCKET deamonCli, sockaddr_in serverAddr);
void worker(SOCKET deamonCli);

int main(int argc, char* argv[]) {

	char* SERVER_ADDR = argv[1];
	u_short SERVER_PORT = atoi(argv[2]);

	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) printf("Version is not supported\n");

	cout << "\nclient started\n" << endl;

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	/* NOTE: deamon process listen to server. */
	SOCKET deamonCli = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int res = setupDeamonCli(deamonCli, serverAddr);
	thread deamon(worker, deamonCli); // NOTE: must init additional threads inside main thread.

	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int tv = 5000; // NOTE: 5-second timeout.
	setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));


	if (connect(client, (sockaddr*)&serverAddr, sizeof(serverAddr))) {
		printf("Error! Cannot connect server. %d\n", WSAGetLastError());
		_getch();
		return 0;
	}

	char buff[BUFF_SIZE];
	char message[BUFF_SIZE + 2];
	int ret;

	do {
		gets_s(buff, BUFF_SIZE);
		if (strlen(buff) == 0) {
			closesocket(client);
			return 0;
		}

		ret = send(client, buff, strlen(buff), 0);
		if (ret == SOCKET_ERROR)
			printf("Error! Cannot send message.\n");
		
		ret = recv(client, buff, BUFF_SIZE, 0);

		if (ret == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAETIMEDOUT) {
				printf("Time-out!\n");
			}
			else printf("Error! Cannot receive message.\n");
		}
		else if (strlen(buff) > 0) {
			buff[ret] = '\0';
			cout <<  "\n(main) response: '" << buff << "'\n" << endl;
		}
	} while (1);

	closesocket(client);

	WSACleanup();

	_getch();
	return 0;
}

int setupDeamonCli(SOCKET deamonCli, sockaddr_in serverAddr) {

	int tv = 60000;
	setsockopt(deamonCli, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));

	if (connect(deamonCli, (sockaddr*)&serverAddr, sizeof(serverAddr))) {
		cout << "error: cannot connect server - " << WSAGetLastError() << endl;
		return 0;
	}

	return 1;
}

void worker(SOCKET deamonCli) {

	char buff[BUFF_SIZE];
	int ret;

	while (1) {
		ret = recv(deamonCli, buff, BUFF_SIZE, 0);
		/*
		if (ret < 0) cout << "\ndeamon error: socket " << (int)deamonCli << " closed" << endl; 
		*/
		if (ret < 0) continue;
		else {
			buff[ret] = 0;
			cout << "\n(deamon) response: '" << buff << "'\n" << endl;
			if (buff[0] == 'H') {
				ofstream file("history.txt");
				if (!file) {
					cout << "(error): could not store history\n" << endl;
				}
				else {
					file << buff;
				}
				file.close();
			}
		}
	}
}

/*8
* [NOTES]:
* Redundant allocation could cause incorrect console displaying.
* "main" function's arguments are considers as a list of characters.
* char* x, y; x != y.
*/

/*
* [CALLING CONVENTION]:
* * Definition: Establish how data is passed between the caller and callee,
* and who responsible for operations such as cleaning out the call stack.
*/


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
