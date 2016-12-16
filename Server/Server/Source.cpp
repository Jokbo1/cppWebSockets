//SERVER!!!
 
#pragma comment(lib,"ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
SOCKET connections[100]; //creating and array to store all connections
int connectionCounter = 0; //use as index of the array

std::string serverIP = "145.85.48.4";
int serverPort = 1337;

enum Packet {
	P_ChatMessage,
	P_Test
};

bool sendInt(int index, int num) {
	int retCheck = send(connections[index], (char*)&num, sizeof(int), NULL); //send the int

	if (retCheck == SOCKET_ERROR) {
		//connection problem
		return false;
	}

	return true;

}


bool getInt(int index,int &num) {
	int retCheck = recv(connections[index], (char*)&num, sizeof(int), NULL); //receive buffer lenght
																	 //send the int

	if (retCheck == SOCKET_ERROR) {
		//connection problem
		return false;
	}

	return true;//Sucessfully received int

}

bool sendPacketType(int index,Packet packettype) {
	int retCheck = send(connections[index], (char*)&packettype, sizeof(Packet), NULL); //Sending the packet type

	if (retCheck == SOCKET_ERROR) {
		//connection problem
		return false;
	}

	return true;

}

bool getPacketType(int index,Packet &packettype) {
	int retCheck = recv(connections[index], (char*)&packettype, sizeof(Packet), NULL);

	if (retCheck == SOCKET_ERROR) {
		//connection problem
		return false;
	}

	return true;//Sucessfully received int

}

bool sendString(int index,std::string &text) {
	if (!sendPacketType(index,P_ChatMessage)) {//sending the packat type
		return false;
	}

	int bufferlength = text.size();
	if (!sendInt(index,bufferlength)) {//sending the length of the message
		return false;
	}

	int retCheck = send(connections[index], text.c_str(), bufferlength, NULL); //Sending the packet type

	if (retCheck == SOCKET_ERROR) {
		//connection problem
		return false;
	}

	return true;

}

bool getString(int index,std::string &text) {
	int bufferLength;

	if (!getInt(index,bufferLength)) {
		return false;
	}

	char * buffer = new char[bufferLength + 1];//Allocating memory to a dynamic sized char array.
	buffer[bufferLength] = '\0';//Adding the null terminator to the end of the string

	int retCheck = recv(connections[index], buffer, bufferLength, NULL);
	text = buffer;//set the string to received buffer message
	delete[] buffer; //everytime we use a "new" we should put a "delete" after we use it

	if (retCheck == SOCKET_ERROR) {
		//connection problem
		return false;
	}

	return true;//Sucessfully received int

}


bool processPacket(int index,Packet packettype) {
	switch (packettype) {
		case P_ChatMessage:
		{
			std::string message;

			if (!getString(index, message)) {
				return false;
			}

			for (int i = 0; i < connectionCounter; i++) {
				if (i == index) {
					continue;//skip to next user because this is the sender
				}

				if (!sendString(i, message)) {
					//send message to connection at index 1
					std::cout << "Failed to send message from client " << index + 1 << " to client :" << i << std::endl;
				}
				std::cout << "Processed chat message packet from user : " << index + 1 << std::endl;
				break;
			}
		}

		default:
			std::cout << "Unrecognized packet:" << packettype << std::endl;
			break;
		}
	return true;
}
void clientHandlerThread(int index) {
	Packet packettype;
	while (true) {

		if (!getPacketType(index,packettype)) {
			break;
		}
		if (!processPacket(index,packettype)) {
			//if we couldn't process the packet then get out of the loop
			break;
		}
	}
	std::cout << "Lost connection to client "<< index+1 << std::endl;
	closesocket(connections[index]); //Closing the socket once we are done with the connection
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
	
	//Create a socket that will listen to new connections

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);//Socket to listen for new connections
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));//Bind the address to the socket 
	listen(sListen, SOMAXCONN);//Listening to socket with a maximum connection tries of SOMAXCONN->0x7ffffff
	std::cout << "Server listening on " << serverIP.c_str() << std::endl;
	//After we start listening, we need to declare a new connection
	SOCKET newConnection;

	for (int i = 0; i < 100; i++) {
		//why loop through all? just increment the counter
		newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //If accept return 0 then it failed

		if (newConnection == 0) {
			std::cout << "Failed to accept client's connection" << std::endl; //How to get the IP of the client?
		}
		else {
			std::cout << "Client connected on port "<< serverPort << std::endl; //put port and IP on variables up top for easily changing them
			connections[i] = newConnection;
			connectionCounter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientHandlerThread, (LPVOID)(i), NULL, NULL);

			std::string MOTD = "Welcome to the coolest server in the UNIverse!";
			sendString(i, MOTD);
	
		}
	}
	
	system("pause");
	return 0;
	//Get packet type
	//Get message size
	//Get message
}