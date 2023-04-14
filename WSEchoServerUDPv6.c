// Author: Isaac Hill
// Assignment: CS 2690 Program 3
// File: WSEchoServerUDPv6.c
// Instructor: Imtiaz Parvez
// Class: CS 2690  Section: 001
// Date Written: 3/30/2023
// Last update: 4/10/2023
// Windows Version: Windows 10    Visual Studio Version: Visual Studio Community 2019
// Description: This is a program that will set up a IPv6 UDP echo server that can recieve
//              messages from the client program and will send the same message back.
// 
// Usage: WSEchoServerUDPv6 <server port> (If no server port is given, it will use a default port of 5001)
// Companion client is WSEchoClientv6
// Client usage: WSEchoClientv6 <server IPv6 address> <server port> <"message to echo">
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

void DisplayFatalErr(char* errMsg);    // writes error message before abnormal termination

void main(int argc, char* argv[])   // argc is # of strings following command, argv[] is array of ptrs to the strings
{
	WSADATA wsaData;                      // contains details about WinSock DLL implementation
	SOCKET serverSock;                    // contains the server's IPv6 TCP stream socket
	struct sockaddr_in6 serverInfo;       // standard IPv6 structure that holds the server socket info
	struct sockaddr_in6 clientInfo;       // standard IPv6 structure that holds the client socket info
	int clientSize;                       // contains the size of clientInfo
	int numArgs;                          // contains the number of arguments
	unsigned short serverPort;            // contains the server's port number
	char clientIPaddr[INET6_ADDRSTRLEN];  // contains the IP address of the client
	int clientPort;                       // contains the client's port number
	char echoBuffer[MAXECHO];             // contains the received message from the client
	int len;                              // contains the length of the recieved message


	// Update numArgs to equal the amount of arguments given
	numArgs = argc;

	// If there were more than 2 arguments given, give an error
	if (numArgs > 2)
	{
		fprintf(stderr, "Incorrect number of command line arguments. 1 optional with Port Number. Exiting...\n");
		exit(1);
	}
	// Else if the number of arguments is 1 (no port number given), use the default server port number
	else if (numArgs == 1)
	{
		serverPort = 5001;
	}
	// Else (2 arguments were given), grab the port number given by the user
	else
	{
		serverPort = atoi(argv[1]);
	}

	// Initializes Winsock 2.0 DLL. If it fails, give an error
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
	{
		fprintf(stderr, "Error Initializing Winsock 2.0. Exiting...\n");
		exit(1);
	}

	// Creates an IPv6 UDP stream socket. If it fails, give an error
	serverSock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	if (serverSock == INVALID_SOCKET)
	{
		DisplayFatalErr("socket() function failed.");
	}

	// Zeros out the sockaddr_in6 structure and loads server info into it
	memset(&serverInfo, 0, sizeof(serverInfo));
	serverInfo.sin6_family = AF_INET6;
	serverInfo.sin6_port = htons(serverPort);
	// Loads "use wildcard" into the sin6_addr
	serverInfo.sin6_addr = in6addr_any;

	// Binds the server socket to the serverInfo and if it returns SOCKET_ERROR, give an error
	if (bind(serverSock, (struct sockaddr*)&serverInfo, sizeof(serverInfo)) == SOCKET_ERROR)
	{
		DisplayFatalErr("bind() function failed.");
	}

	// Displays a message saying that the server is ready for UDP client connections
	printf("\nIH's IPv6 echo server is ready for UDP connection...\n");

	// Forever loop can be exited with CTRL-C
	for (;;)
	{
		// Calls recvfrom() to get message sent by the the client and puts the client information into clientInfo.
		// If it fails, give an error
		clientSize = sizeof(clientInfo);
		if ((len = recvfrom(serverSock, echoBuffer, MAXECHO, 0, (struct sockaddr*)&clientInfo, &clientSize)) < 0)
		{
			DisplayFatalErr("recvfrom() function failed.");
		}

		// Grabs the client's IP address and store it in clientIPAddr
		inet_ntop(AF_INET6, &(clientInfo.sin6_addr), clientIPaddr, sizeof(clientIPaddr));
		// Grabs the client's port number and store it in clientPort
		clientPort = ntohs(clientInfo.sin6_port);

		// Displays the client IP address, the client port, and the server port
		printf("Processing the client at %s, client port %d, server port %d\n", clientIPaddr, clientPort, serverPort);

		// Calls sento() to send message recieved back to client that is connected. If the amount sent is not the length that was recieved, give an error.
		clientSize = sizeof(clientInfo);
		if (sendto(serverSock, echoBuffer, len, 0, (struct sockaddr*)&clientInfo, &clientSize) != len)
		{
			DisplayFatalErr("sendto() function failed.");
		}
	}
}