//CLIENT!!!


#pragma comment(lib,"ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <string>
SOCKET connection;

//client thread to receive message
void clientThread() {
	int bufferLength;
	while (true) {
		recv(connection, (char*)&bufferLength, sizeof(int), NULL); //receive buffer lenght
		char * buffer = new char[bufferLength + 1];//Allocating memory to a dynamic sized char array.
		buffer[bufferLength] = '\0';//Adding the null terminator to the end of the string
		recv(connection, buffer, bufferLength, NULL);
		std::cout << "Message:" << buffer << std::endl;
		
		delete[] buffer; //everytime we use a "new" we should put a "delete" after we use it
		
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

	connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		MessageBoxA(NULL, "Failed to connect to server", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	std::cout << "Connected" << std::endl;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientThread, NULL, NULL, NULL);
	//char MOTD[256];//size of expected message
	//recv(connection, MOTD, sizeof(MOTD), NULL); // Receive message from the server, passing the size , the message
	//std::cout << "MOTD:" << MOTD << std::endl;

	std::string buffer;
	while (true) {
		std::getline(std::cin,buffer);//reading input
		int bufferlength = buffer.size();
		send(connection, (char*)&bufferlength, sizeof(int), NULL); //Send integer that holds the size of the message in bytes
		send(connection, buffer.c_str(), bufferlength, NULL); //c_str() converts the string into and array of chars.
		Sleep(10);
	}

	system("pause");
	return 1;
}