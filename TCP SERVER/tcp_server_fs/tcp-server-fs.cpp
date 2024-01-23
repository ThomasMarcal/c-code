/* 
 * [2019] Signet Lab - Programming for ICT
 * All Rights Reserved.
 * 
 * Authors Filippo Campagnaro and Michele Polese 
 */

#include <tcp-server-fs.h>
#include <string.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

TcpServer::TcpServer(int port_num)
:
EthConn(port_num),
l_sock_fd(-1),
sock_fd(-1),
connections_count(0)
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
}

TcpServer::~TcpServer() 
{
  if(l_sock_fd >= 0) {
    close(l_sock_fd);
  }
  if(sock_fd >= 0) {
    close(sock_fd);
  }
}

bool TcpServer::optionsAndBind() 
{
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
    close(l_sock_fd);
    return false;
  }

  // Accept at max 1 client at a time: by changing 1 to 5 in the next line 
  // you accept at most the connection of 5 clients in parallel.
  if (listen(l_sock_fd, 1) < 0) { 
    std::cout << "ERROR: LISTEN SOCKET" << std::endl;
    close(l_sock_fd);
    return false;
  //ERR
  }
  return true;
}

bool TcpServer::acceptConn() 
{
  struct sockaddr_in client_addr;
  socklen_t addr_l = sizeof(client_addr);
  sock_fd = accept(l_sock_fd, (struct sockaddr*) &client_addr, &addr_l);
  
  // TODO1: if the file descriptor is not valid (i.e., the accept operation 
  // failed), it prints to std::cout and error and returns false.
  // TODO2: otherwise, it stores and prints the ip address of the client, 
  // obtained by converting to char* the network address (sin_addr) using the 
  // method inet_ntoa. Then it increments the connection counter and returns true.
  
  if (sock_fd < 0) {
	  std::cout << "Error : sock_fd < 0" << std::endl;
	  return false;
  }
  
  std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << std::endl;
  connections_count = connections_count + 1;
  return true;
}


int TcpServer::receive(std::shared_ptr<std::array<char,MTU>> buf)  
{
  if(sock_fd < 0) {
    return -1;
  }

  int rcv_size = recv(sock_fd,buf->data(), buf->size(),0);
  if(rcv_size < 0) {
    std::cout << "Error : rcv_size < 0" << std::endl;
    close(sock_fd);
    sock_fd = -1;
    return -1;
  }
  return rcv_size;
  // TODO: implement the receive method that receives from the data socket
  // and stores the received data into the heap allocated buffer buf
  // and returns the received data size. If the recv operation fails,
  // it prints an error message in std::cout, closes the socket, sets its file 
  // descriptor to -1 and returns -1.
  // SUGGESTION1: use the method recv.
  // SUGGESTION2: access the underlying C-array of buffer with buf->data().
}


int TcpServer::transmit(std::shared_ptr<std::array<char,MTU>> buf, 
  size_t size_to_tx) 
{
  if(sock_fd < 0) {
    return -1;
  }
  int sent_bytes = send(sock_fd, buf->data(), size_to_tx, 0);
  if (sent_bytes < 0) {
	std::cout << "Error : sent_bytes < 0" << std::endl;
    close(sock_fd);
    sock_fd = -1;
    return -1;
  }
  return sent_bytes;
  // TODO: implement the transmit method that transmits through the data socket
  // and returns the transmitted data size. If the transmit operation fails,
  // it prints an error message in std::cout, closes the socket, sets its file 
  // descriptor to -1 and returns -1.
  // SUGGESTION1: use the method send.
  // SUGGESTION2: access the underlying C-array of buffer with buf->data().
}

void TcpServer::print(std::ostream& out) const
{
  out << "TcpServer(localhost:" << std::to_string(port) 
      << "), last host connected = " << ip 
      << ", number of  connections so far = " 
      << std::to_string(connections_count);
}
