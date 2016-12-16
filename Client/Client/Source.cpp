//CLIENT!!!


#pragma comment(lib,"ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <string>
SOCKET connection;

std::string serverIP = "145.85.48.4";
int serverPort = 1337;

enum Packet {
	//All the different packet types that can be sent
	P_ChatMessage,
	P_Test
};

bool sendInt(int num) {
	int retCheck = send(connection, (char*)&num, sizeof(int), NULL); //send the int

	if (retCheck == SOCKET_ERROR) {
		//connection problem
		return false;
	}

	return true;

}

bool getInt(int &num) {
	int retCheck = recv(connection, (char*)&num, sizeof(int), NULL); //receive buffer lenght
	 //send the int

	if (retCheck == SOCKET_ERROR) {
		//connection problem
		return false;
	}

	return true;//Sucessfully received int

}

bool sendPacketType(Packet packettype) {
	int retCheck = send(connection, (char*)&packettype, sizeof(Packet), NULL); //Sending the packet type

	if (retCheck == SOCKET_ERROR) {
		//connection problem
		return false;
	}

	return true;

}

bool getPacketType(Packet &packettype) {
	int retCheck = recv(connection, (char*)&packettype, sizeof(Packet), NULL); 

	if (retCheck == SOCKET_ERROR) {
		//connection problem
		return false;
	}

	return true;//Sucessfully received int

}

bool sendString(std::string &text) {
	if (!sendPacketType(P_ChatMessage)) {//sending the packat type
		return false;
	}

	int bufferlength = text.size();
	if (!sendInt(bufferlength)) {//sending the length of the message
		return false;
	}

	int retCheck = send(connection, text.c_str(), bufferlength, NULL); //Sending the packet type

	if (retCheck == SOCKET_ERROR) {
		//connection problem
		return false;
	}
	
	return true;

}

bool getString(std::string &text) {
	int bufferLength;

	if (!getInt(bufferLength)) {
		return false;
	}

	char * buffer = new char[bufferLength + 1];//Allocating memory to a dynamic sized char array.
	buffer[bufferLength] = '\0';//Adding the null terminator to the end of the string
	
	int retCheck = recv(connection, buffer, bufferLength, NULL);
	text = buffer;//set the string to received buffer message
	delete[] buffer; //everytime we use a "new" we should put a "delete" after we use it

	if (retCheck == SOCKET_ERROR) {
		//connection problem
		return false;
	}

	return true;//Sucessfully received int

}

bool processPacket(Packet packettype) {
	switch (packettype) {
	case P_ChatMessage:
	{
		std::string message;
		if (!getString(message)) {
			return false;
		}

		std::cout << message << std::endl;

		break;
	}

	default:
		std::cout << "Unrecognized packet:" << packettype << std::endl;
		break;
	}
	return true;
}
//client thread to receive message
void clientThread() {
	Packet packettype;
	while (true) {

		if (!getPacketType(packettype)) {
			break;
		}
		if (!processPacket(packettype)) {
			//if we couldn't process the packet then get out of the loop
			break;
		}
	}
	std::cout << "Lost connection to the server." << std::endl;
	closesocket(connection); //Closing the socket once we are done with the connection
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
	addr.sin_addr.s_addr = inet_addr(serverIP.c_str());//which address will the server be hosted on
	addr.sin_port = htons(serverPort);//port. WHY NO FUNCTION FOR BOTH?
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

	std::string input;
	while (true) {
		std::getline(std::cin,input);//reading input
		if (!sendString(input)) {
			return false;
		}
		Sleep(10);
	}

	system("pause");
	return 1;
}