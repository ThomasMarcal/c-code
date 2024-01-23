#include <chat-client.h>
#include <string.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

ChatClient::ChatClient(std::string ip_addr, int port_num)
:
ip(ip_addr),
port(port_num),
exit(false),
sock_fd(-1)
{
  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0){ 
    std::cout << "ERROR: OPEN SOCKET" << std::endl;
    close(sock_fd);
    return;
  }

  thr_rx = std::unique_ptr<std::thread>(new std::thread(&ChatClient::run,this));
}

ChatClient::~ChatClient() {
  quit();
  if(thr_rx && thr_rx->joinable()) {
    thr_rx->join();
  }
  if(thr_tx && thr_tx->joinable()) {
    thr_tx->join();
  }
  std::cout << "joint" << std::endl;
}

void ChatClient::quit() {
  exit = true;
  if(sock_fd >= 0){
    close(sock_fd);
    sock_fd = -1;
  }
}

void ChatClient::run() {
  char buf[C_MTU];
  
  if(!connect())
	  return;
  
  while(!exit) {
    if(thr_tx == nullptr) {
        thr_tx = std::unique_ptr<std::thread>(
          new std::thread (&ChatClient::txChat,this));
    }
  if(recv(buf) <= 0) {
    std::cout << "read size less than zero size" << std::endl;
    close(sock_fd);
    sock_fd = -1;
    break;
  }
  std::cout << buf;
  memset(buf,0,C_MTU); 
  }
}

void ChatClient::txChat() {
  while(!exit) {
    std::string to_tx;
    std::getline(std::cin, to_tx);
    if(exit) {
      break;
    }
    if(send(to_tx.c_str(),to_tx.length()) >= 0) {
      send("\n",1);
    }
  }
}

bool ChatClient::connect() {
  int option(1);
  setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, 
                  (char*)&option, sizeof(option));

  struct sockaddr_in dest_addr = {0}; 
  dest_addr.sin_family = AF_INET; 
  dest_addr.sin_port = htons(port); 

  if (inet_pton(AF_INET, ip.c_str(), &dest_addr.sin_addr) <= 0) { 
    std::cout << "ERROR: INET_PTON" << std::endl;
    close(sock_fd); 
    return false;
  }

  if (::connect(sock_fd, (struct sockaddr*) &dest_addr, sizeof(dest_addr)) < 0) { 
    std::cout << "ERROR: CONNECT" << std::endl;
    close(sock_fd);
    return false;
  }
  
  return true;
}

int ChatClient::recv(char* buf)  
{
  int rcv_size = ::recv(sock_fd,buf,C_MTU,0);
  if(rcv_size < 0) {
    std::cout << "ERROR: RECV" << std::endl;
    if(sock_fd >= 0) {
      close(sock_fd);
      sock_fd = -1;
    }
  }
  return rcv_size;
}


int ChatClient::send(const char* buf, size_t size_to_tx) 
{
  if(sock_fd<0) {
    return -1;
  }
  int sent_size = ::send(sock_fd,buf,size_to_tx,0); 
  if(sent_size < 0) { 
    std::cout << "ERROR: SEND" << std::endl;
    if(sock_fd >= 0) {
      close(sock_fd);
      sock_fd = -1;
  }
  return -4;
  }
  return sent_size;
}
