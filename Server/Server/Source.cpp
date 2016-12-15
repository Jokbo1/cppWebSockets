//SERVER!!!
 
#pragma comment(lib,"ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
SOCKET connections[100]; //creating and array to store all connections
int connectionCounter = 0; //use as index of the array

void clientHandlerThread(int index) {

	int bufferLength;
	while (true) {
		recv(connections[index], (char*)&bufferLength, sizeof(int), NULL); // Receiving the length o the message
		char * buffer = new char[bufferLength];//Allocating memory to a dynamic sized char array.
		recv(connections[index], buffer, bufferLength, NULL);//Receiving the actual message
		
		
		
		for (int i = 0; i < connectionCounter; i++) {
			if (i == index) {
				continue;//Skip this index
			}
			send(connections[i], (char*)&bufferLength, sizeof(int), NULL);
			send(connections[i], buffer, bufferLength, NULL);
		}

		delete[] buffer;//clearing out memory we reserved 
	}
}


int main() {
	//This code starts up the winsock
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) {
		//If WSAStartup returns anything other than 0 then an error has ocurred
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR); //Show the specific error
		exit(1);
	}

	SOCKADDR_IN addr; //Address that we will bind our listeing socket to
	int addrlen = sizeof(addr); //length of the address
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");//which address will the server be hosted on
	addr.sin_port = htons(1111);//port. WHY NO FUNCTION FOR BOTH?
	addr.sin_family = AF_INET;//Specifies an IPv4 type of socket
	
	//Create a socket that will listen to new connections

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);//Socket to listen for new connections
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));//Bind the address to the socket 
	listen(sListen, SOMAXCONN);//Listening to socket with a maximum connection tries of SOMAXCONN->0x7ffffff
	std::cout << "Server listening on 127.0.0.1:1111" << std::endl;
	//After we start listening, we need to declare a new connection
	SOCKET newConnection;

	for (int i = 0; i < 100; i++) {
		//why loop through all? just increment the counter
		newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //If accept return 0 then it failed

		if (newConnection == 0) {
			std::cout << "Failed to accept client's connection" << std::endl; //How to get the IP of the client?
		}
		else {
			std::cout << "Client connected on port 1111" << std::endl; //put port and IP on variables up top for easily changing them
			std::string MOTD = "Welcome! This is the Message Of The Day.";
			int MOTDLen = MOTD.size();
			send(newConnection, (char*)&MOTDLen, sizeof(int), NULL);
			send(newConnection, MOTD.c_str(), sizeof(MOTD), NULL);//Send message to a new connection. A pointer to the new message, size of it, and flags
			connections[i] = newConnection;
			connectionCounter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientHandlerThread, (LPVOID)(i), NULL, NULL);
		}
	}
	
	system("pause");
	return 0;
	//Get packet type
	//Get message size
	//Get message
}