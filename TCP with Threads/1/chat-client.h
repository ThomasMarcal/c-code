#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

//#include <iostream>
#include "memory"
#include "thread"
#include "string"


class ChatClient {
	
	public:

		ChatClient(const std::string& ip, int port);
		
		ChatClient(const ChatClient& chatClient) = delete;
		ChatClient operator=(const ChatClient& chatClient) = delete;
		
		~ChatClient();

		int send(const char* buffer, size_t size);
		
		int recv(char* buffer);
	
	private:	
		void txChat(); // is this private?
		void rxChat();

		void quit();
		
	private:
		std::string ip;
		int port;
		
		int socket_fd;
		
		std::unique_ptr<std::thread> thread_tx;
		std::unique_ptr<std::thread> thread_rx;

		bool exit;
	
	public:
		static const unsigned int MTU = 512;
};

#endif
