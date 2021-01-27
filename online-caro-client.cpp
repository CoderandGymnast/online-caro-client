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

#define USERNAME_OPERATION '1'
#define PASSWORD_OPERATION '2'
#define LOGOUT_OPERATION '3'

#define USERNAME_SUCCESS "10"
#define USERNAME_NOT_FOUND "11"
#define ACCOUNT_IS_BLOCKED "12"
#define ACCOUNT_IS_LOGGED_IN "13"
#define PASSWORD_SUCCESS "20"
#define PASSWORD_INCORRECT "21"
#define LOGOUT_SUCCESS "30"

#define RESPONSE_SIZE 3

void processResponse(char* response);
void printOperation();
void processState();
void constructMessage(char* data, char* message);
void processUsername(char* response);
void processPassword(char* response);
void processLogout(char* response);

int state = 1; /* 1: Enter username; 2: Enter password; 3: Logged in.  */

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
		printOperation();
		gets_s(buff, BUFF_SIZE);
		if (strlen(buff) == 0) {
			closesocket(client);
			return 0;
		}

		if (state == int((LOGOUT_OPERATION)-48)) {
			if (strcmp(buff, "0")) {
				printf("Unknown operation. Please, enter again.\n");
				continue;
			}
		}

		constructMessage(buff, message);

		ret = send(client, message, strlen(message), 0);
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
			processResponse(buff);
		}
	} while (1);

	closesocket(client);

	WSACleanup();

	_getch();
	return 0;
}

/*
* Process response from server.
* @param     response     server response.
*/
void processResponse(char* response) {
	char code = response[0];
	switch (code) {
	case USERNAME_OPERATION:
		processUsername(response);
		break;
	case PASSWORD_OPERATION:
		processPassword(response);
		break;
	case LOGOUT_OPERATION:
		processLogout(response);
		break;
	default:
		printf("[ERROR]: Unknown response\n");
		break;
	}
}

/*
* Print the required operation for the user.
*/
void printOperation() {
	switch (state) {
	case 1:
		printf("Enter username: ");
		break;
	case 2:
		printf("Enter password: ");
		break;
	case 3:
		printf("1. Enter '0': Log out.\n2. Press 'Enter': Exit.\n\nEnter operation: ");
		break;
	default:
		printf("[ERROR]: Unknown state\n");
		break;
	}
}

/*
* Process the state of this application.
*/
void processState() {
	if (1 <= state && state <= 2) state++;
	else state = 1;
}

/*
* Construct the message according to the form: CODE + DATA.
* @param     data    [IN] The data is going to be sent to the server.
* @param     message    [OUT] The constructed message.
*/
void constructMessage(char* data, char* message) {
	sprintf(message, "%d", state);
	strcat(message, data);
}

/*
* Process the username from the server for the 'username' request.
* @param     response     The response from the server.
*/
void processUsername(char* response) {
	if (strcmp(response, USERNAME_SUCCESS) == 0) processState();
	else if (strcmp(response, USERNAME_NOT_FOUND) == 0) {
		printf("Username was not found. Please, enter again.\n");
		state = int(USERNAME_OPERATION) - 48;
	}
	else if (strcmp(response, ACCOUNT_IS_BLOCKED) == 0) {
		printf("This account is blocked. Please, use others.\n");
		state = int(USERNAME_OPERATION) - 48;
	}
	else if (strcmp(response, ACCOUNT_IS_LOGGED_IN) == 0) {
		printf("This account is already logged in. Please, use others.\n");
	}
	else {
		printf("[ERROR]: Unknown operation\n");
	}
}

/*
* Process the password from the server for the 'password' request.
* @param     response     The response from the server.
*/
void processPassword(char* response) {
	if (strcmp(response, PASSWORD_SUCCESS) == 0) {
		processState();
		printf("Logged in\n\n");
	}
	else if (strcmp(response, PASSWORD_INCORRECT) == 0) {
		printf("Password was not found. Please, enter again.\n");
		state = int(PASSWORD_OPERATION) - 48;
	}
	else if (strcmp(response, ACCOUNT_IS_BLOCKED) == 0) {
		printf("This account is blocked. Please, use others.\n");
		state = int(USERNAME_OPERATION) - 48;
	}
	else printf("[ERROR]: Unknown operation\n");
}

/*
* Process the response from the server for the 'log out' request.
* @param     response     The response from the server.
*/
void processLogout(char* response) {
	if (strcmp(response, LOGOUT_SUCCESS) == 0) {
		printf("Logged out\n\n");
		processState();
	}
	else printf("[ERROR]: Unknown operation\n");
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
