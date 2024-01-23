/*
 * [2019] Signet Lab - Programming for ICT
 * All Rights Reserved.
 * 
 * Authors Filippo Campagnaro and Michele Polese 
*/

#include <string.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char** argv) 
{
  /**
   * EXERCISE 1: RUN THIS CODE:
   * - connect the client to a server running in you local PC:
   *     i) using no parameters (the default values);
   *     ii) using different values of ip and port. 
   * - connect the client to a server running in one of your lab-mate PC:
   *     i) using different values of ip and port. 
   *
   *   To connect to a local server use either the ip address
   *   i) 127.0.0.1 (that is the loopback, i.e. localhost)
   *   ii) the ip of your machine (discover it with `ip addr` or `ifconfig`)
   */

  // READ from argv the command line argument vector, by checking the arguments
  // counter (argc), i.e.,
  //    ./tcp_client has argc = 1 and no command line arguments
  //    ./tcp_client 192.168.100.12 has argc = 2 and the IP address as argument
  //    ./tcp_client 192.168.100.12 55556 has argc = 3 and IP and socket port as 
  //      arguments
  const char* dest_ip = argc == 1 ? "127.0.0.1" : argv[1];
  const int dest_port = argc > 2 ? atoi(argv[2]) : 55555;

  // open a SOCK_STREAM (TCP) socket
  int sckfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sckfd < 0){ 
    std::cout << "ERROR: OPEN SOCKET" << std::endl;
    close(sckfd);
    return -1;
  }

  // SET SOCKET OPTIONS TO RELEASE THE SOCKET ADDRESS IMMEDIATELY AFTER 
  // THE SOCKET IS CLOSED
  int option(1);
  setsockopt(sckfd, SOL_SOCKET, SO_REUSEADDR, 
                  (char*)&option, sizeof(option));

  //SET SERVER DESTINATION ADDRESS 
  struct sockaddr_in dest_addr = {0}; // set all elements of the struct to 0
  dest_addr.sin_family = AF_INET; // address family is AF_INET (IPV4)

  // convert dest_port to network number format
  dest_addr.sin_port = htons(dest_port); 
  // convert dest_ip from char* to network address
  if (inet_pton(AF_INET, dest_ip, &dest_addr.sin_addr) <= 0) { 
    std::cout << "ERROR CONVERTING IP TO INTERNET ADDR" << std::endl;
    close(sckfd); // if conversion fail, close the socket and return error -2
    return -2;
  }

  //CONNECT THE SOCKET TO THE SERVER IP:PORT SPECIFIED INTO DEST_ADDR
  if (connect(sckfd, (struct sockaddr*) &dest_addr, sizeof(dest_addr)) < 0) { 
    std::cout << "ERROR: CONNECT" << std::endl;
    close(sckfd); // if connection failed return
    return -3;
  }

  //TRANSMIT DATA
  const size_t max_size = 512;
  char buf[max_size] = "[data to tx]"; // store the data in a buffer
  size_t data_size = 12;
  int sent_size = send(sckfd,buf,data_size,0); // send the data through sckfd
  if(sent_size < 0) { // the send returns a size of -1 in case of errors
    std::cout << "ERROR: SEND" << std::endl;
    close(sckfd); // if error close the socket and exit
    return -4;
  }


  memset(buf, 0, max_size); // set buffer to zero for next read
  /**
   * EXERCISE 2 (you have to complete this): 
   * INSERT THE CODE FOR RECEIVING AND PRINTING TO STANDARD OUTPUT
  **/
  
  /*
  TO BE PRECISE: a sequence of read operations until we empty the TCP buffer
  int offset = 0;
  for(int recv_size = 0; (recv_size = recv(sckfd, buf+offset, max_size - offset, 0)) > 0; offset += recv_size){}
  */
  int recv_size = recv(sckfd, buf, max_size, 0);
  if(recv_size < 0) { 
    std::cout << "ERROR: RECV" << std::endl;
    close(sckfd); 
    return -5;
  }
  std::cout << buf << std::endl;

  //CLOSE THE SOCKET
  close(sckfd);
}

