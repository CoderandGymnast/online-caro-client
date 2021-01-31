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

#pragma comment (lib, "Ws2_32.lib")

#define BUFF_SIZE 2048
#define RESPONSE_SIZE 3

using namespace std;

int main(int argc, char* argv[]) {

	char* SERVER_ADDR = argv[1];
	u_short SERVER_PORT = atoi(argv[2]);

	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) printf("Version is not supported\n");

	printf("Client started\n");

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int tv = 10000;
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
		cout << "message: ";
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
			if (WSAGetLastError() == WSAETIMEDOUT)
				printf("Time-out!\n");
			else printf("Error! Cannot receive message.\n");
		}
		else if (strlen(buff) > 0) {
			buff[ret] = '\0';
			cout << buff << endl;
		}
	} while (1);

	closesocket(client);

	WSACleanup();

	_getch();
	return 0;
}

/*
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
