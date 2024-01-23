/*
 * [2019] Signet Lab - Programming for ICT
 * All Rights Reserved.
 * 
 * Authors Filippo Campagnaro and Michele Polese 
*/

#include <chat-server.h>
#include <string.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

ChatServer::ChatServer(int port_num)
:
port(port_num),
ip("127.0.0.1"),
exit(false),
l_sock_fd(-1),
sock_fd(-1)
{
  l_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (l_sock_fd < 0){ 
    std::cout << "ERROR: OPEN SOCKET" << std::endl;
    close(l_sock_fd);
    return;
  }
  if(!optionsAndBind()) {
    std::cout << "ERROR: BIND" << std::endl;
    close(l_sock_fd);
    return;
  }
  thr_rx = std::unique_ptr<std::thread>(new std::thread(&ChatServer::run,this));
}

ChatServer::~ChatServer() {
  quit();
  if(thr_rx && thr_rx->joinable()) {
    thr_rx->join();
  }
  if(thr_tx && thr_tx->joinable()) {
    thr_tx->join();
  }
  std::cout << "joint" << std::endl;
}

void ChatServer::quit() {
  exit = true;
  if(l_sock_fd >= 0) {
    close(l_sock_fd);
    l_sock_fd = -1;
  }
  if(sock_fd >= 0){
    close(sock_fd);
    sock_fd = -1;
  }
}

void ChatServer::run() {
  char buf[C_MTU];
  while(!exit) {
    if(accept()) {
      if(thr_tx == nullptr) {
        thr_tx = std::unique_ptr<std::thread>(
          new std::thread (&ChatServer::txChat,this));
      }
      while(!exit) {
        if(recv(buf) <= 0) {
          std::cout << "read size less than zero size" << std::endl;
          close(sock_fd);
          sock_fd = -1;
          break;
        }
        std::cout << buf;
        memset(buf,0,C_MTU); // this cleans the buffer for a new read
      }
    }
  }
}

void ChatServer::txChat() {
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

bool ChatServer::optionsAndBind() {
  int option(1);
  setsockopt(l_sock_fd, SOL_SOCKET, SO_REUSEADDR, 
                  (char*)&option, sizeof(option));
  struct sockaddr_in my_addr = {0}; // set all elements of the struct to 0
  my_addr.sin_family = AF_INET; // address family is AF_INET (IPV4)

  // convert listener_port to network number format
  my_addr.sin_port = htons(port); 
  // accept connection from all interfaces of the Network Interface Card (NIC)
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
  // bind the socket to the port
  if (bind(l_sock_fd,(struct sockaddr*)&my_addr, sizeof(my_addr)) < 0) { 
    std::cout << "ERROR: BIND SOCKET" << std::endl;
    if(l_sock_fd>=0) {
      close(l_sock_fd);
    }
    return false;
  }

  // Accept at max 1 client at a time: by changing 1 to 5 in the next line 
  // you accept at most the connection of 5 clients in parallel.
  if (listen(l_sock_fd, 1) < 0) { 
    std::cout << "ERROR: LISTEN SOCKET" << std::endl;
    
    return false;
  //ERR
  }
  return true;
}

bool ChatServer::accept() {
  struct sockaddr_in client_addr;
  socklen_t addr_l = sizeof(client_addr);
  sock_fd = ::accept(l_sock_fd, (struct sockaddr*) &client_addr, &addr_l);
  if(sock_fd < 0) {
    std::cout << "ERROR: ACCEPT CONNECTION" << std::endl;
    if(l_sock_fd>=0) {
      close(l_sock_fd);
      l_sock_fd = - 1;
    }
    return false;
  }
  // The next line prints the client address, converting to char* 
  // the network address (inet_ntoa)
  std::cout << "New connection from " 
            << inet_ntoa(client_addr.sin_addr) << std::endl; 
  ip = inet_ntoa(client_addr.sin_addr);
  return true;
}


int ChatServer::recv(char* buf)  
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


int ChatServer::send(const char* buf, size_t size_to_tx) 
{
  if(sock_fd<0) {
    return -1;
  }
  int sent_size = ::send(sock_fd,buf,size_to_tx,0); // send the data through sckfd
  if(sent_size < 0) { // the send returns a size of -1 in case of errors
    std::cout << "ERROR: SEND" << std::endl;
    if(sock_fd >= 0) {
      close(sock_fd);
      sock_fd = -1;
    } // if error close the socket and exit
    return -4;
  }
  return sent_size;
}
