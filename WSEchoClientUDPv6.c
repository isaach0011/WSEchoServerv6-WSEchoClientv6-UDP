// Author: Isaac Hill
// Assignment: CS 2690 Program 3
// File: WSEchoClientUDPv6.c
// Instructor: Imtiaz Parvez
// Class: CS 2690  Section: 001
// Date Written: 3/30/2023
// Last update: 4/10/2023
// Windows Version: Windows 10    Visual Studio Version: Visual Studio Community 2019
// Description: This is a program that will get the IPv6 of the server to connect to, the server port,
//              and the message to send to the server and recieve back.
// 
// Usage: WSEchoClientUDPv6 <server IPv6 address> <server port> <"message to echo">
// Companion server is WSEchoServerUDPv6
// Server usage: WSEchoServerUDPv6 <server port>
//
// This program is coded in conventional C programming style, with the 
// exception of the C++ style comments.
//
// I declare that the following source code was written by me or provided
// by the instructor. I understand that copying source code from any other 
// source or posting solutions to programming assignments (code) on public
// Internet sites constitutes cheating, and that I will receive a zero 
// on this project if I violate this policy.
// ----------------------------------------------------------------------------

// Minimum required header files for C Winsock program
#include <stdio.h>       // for print functions
#include <stdlib.h>      // for exit() 
#include <winsock2.h>	 // for Winsock2 functions
#include <ws2tcpip.h>    // adds support for getaddrinfo & getnameinfo for v4+6 name resolution

#define MAXECHO 50    // using a default of 50

void DisplayFatalErr(char* errMsg); // writes error message before abnormal termination

void main(int argc, char* argv[])   // argc is # of strings following command, argv[] is array of ptrs to the strings
{
	WSADATA wsaData;                // contains details about WinSock DLL implementation
	struct sockaddr_in6 serverInfo;	// standard IPv6 structure that holds server socket info
	struct sockaddr_in6 fromInfo;   // standard IPv6 structure that holds the recieved message's server socket info
	int numArgs, len, size;         // numArgs: contains the number of arguments   len: contains current length of phrase  size: contains size of serverInfo
	unsigned short serverPort;      // contains the server port number
	char* serverIPaddr, * phrase;   // serverIPaddr: contains the server IP address   phrase: contains the message to pass to server and recieve back
	SOCKET sock;                    // contains the IPv6 TCP stream socket
	char echoBuffer[MAXECHO];       // contains the returned message from the server
	int fromSize;                   // contains the size of fromInfo

	numArgs = argc;
	//If there are not 4 arguments (3 not including file) give an error
	if (numArgs != 4)
	{
		fprintf(stderr, "Incorrect number of command line arguments. 3 needed. Exiting...\n");
		exit(1);
	}

	// Grabs command line arguments and gets length of phrase and size of serverInfo
	serverIPaddr = argv[1];
	serverPort = atoi(argv[2]);
	phrase = argv[3];
	len = strlen(phrase) + 1;
	size = sizeof(serverInfo);

	// Initializes Winsock 2.0 DLL. If it fails, give an error
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
	{
		fprintf(stderr, "Error Initializing Winsock 2.0. Exiting...\n");
		exit(1);
	}

	// Creates an IPv6 UDP stream socket. If it fails, give an error
	sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		DisplayFatalErr("socket() function failed.");
	}

	// Prints message letting user know that the socket has been successfully created. Waits for input before proceeding.
	printf("Socket created successfully. Press any key to continue...");
	getchar();

	// Zeros out the sockaddr_in6 structure and loads server info into it
	memset(&serverInfo, 0, sizeof(serverInfo));
	serverInfo.sin6_family = AF_INET6;
	serverInfo.sin6_port = htons(serverPort);
	inet_pton(AF_INET6, serverIPaddr, &serverInfo.sin6_addr);

	// Calls sento() to send message to the server. If it fails, give an error.
	if (sendto(sock, phrase, len, 0, (struct sockaddr*)&serverInfo, size) < 0)
	{
		DisplayFatalErr("sendto() function failed.");
	}

	// Calls recvfrom() to get message back from the server. If it is not the same length as the message sent, give an error.
	fromSize = sizeof(fromInfo);
	if ((recvfrom(sock, echoBuffer, len, 0, (struct sockaddr*)&fromInfo, &fromSize)) != len)
	{
		DisplayFatalErr("recvfrom() function failed.");
	}

	// Checks to make sure the addresses of the server the message was sent to and the address the message was recieved from are the same.
	// If they are not the same, give an error.
	if (memcmp((struct sockaddr*)&serverInfo, (struct sockaddr*)&fromInfo, fromSize) == 0)
	{
		printf("%s", echoBuffer);
	}
	else
	{
		fprintf(stderr, "IP address of server not the same as recieved IP. Exiting...\n");
		exit(1);
	}
}