#include "chat-client.h"

#include <iostream>
#include <string.h>

#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <errno.h>


ChatClient::ChatClient(const std::string& ip, int port)
	: ip{ip}
	, port{port}
	, socket_fd{-1}
	, exit{false}
{
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd == -1){
		std::cerr << "Error in opening socket" << std::endl;
		return;
	}
	
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &address.sin_addr);
	
	if(connect(socket_fd, (struct sockaddr*) &address, sizeof(address)) == -1){
		std::cerr << "Error in connecting to server" << std::endl;
		// perror("DEBUG");

		// printf("Port: %h\n", ntohs(address.sin_port));
		// unsigned char *p = (unsigned char *)address.sin_addr.s_addr;
		// for(int i=0; i<4; i++){
		// 	printf("%d.", *(p+i));
		// }
		return; //Doesn't seem to work
	}
	
	thread_tx = std::unique_ptr<std::thread>(new std::thread(&ChatClient::txChat, this));
	thread_rx = std::unique_ptr<std::thread>(new std::thread(&ChatClient::rxChat, this));
}

ChatClient::~ChatClient(){
	quit();
	if(thread_tx && thread_tx->joinable()){
		thread_tx->join();
	}
	if(thread_rx && thread_rx->joinable()){
		thread_rx->join();
	}
}

int ChatClient::recv(char* buffer){
	int n = ::recv(socket_fd, buffer, MTU, 0);
	if(n < 0){
		std::cerr << "Error in recv" << std::endl;
		// if(socket_fd >= 0){
		// 	close(socket_fd);
		// 	socket_fd = -1;
		// }
		quit();
	}
	return n;
}

int ChatClient::send(const char* buffer, size_t size){
	int bytes_sent = 0;
	while(bytes_sent < size){
		int n = ::send(socket_fd, buffer + bytes_sent, size - bytes_sent, 0);
		if(n < 0){
			std::cerr << "Error in send" << std::endl;
			// if(socket_fd >= 0){
			// 	close(socket_fd);
			// 	socket_fd = -1;
			// }
			quit();
			return n;
		}
		bytes_sent += n;
	}
	return bytes_sent;
}

void ChatClient::quit(){
	exit = true;
	close(socket_fd);
	socket_fd = -1;
}

void ChatClient::txChat(){
	while(!exit){
		std::string message;
		
		std::cout << ">> ";
		std::getline(std::cin, message);
		message += "\n";

		send(message.c_str(), message.length());
	}
}

void ChatClient::rxChat(){
	while(!exit){
		char buffer[MTU+1];
		memset(buffer, 0, MTU+1);

		int n = recv(buffer);
		if(n > 0){
			buffer[n] = '\0';
			std::cout << buffer;
			// std::cout << std::endl; //Tried, doesn't work
			// std::cout << ">> "; //Thought it could be cheap, but it doesn't work too
		}
	}
}
