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

int main(int argc, char** argv) {

  /**
   * EXERCISE 1 (you have to complete this): RUN THIS CODE:
   * - accessing to a server in you local PC
   * - accessing to a server in one of your lab-mate PC
   * - using no parameters (the default values)
   * - using different parameters of ip and port. 
   *   To connect to a local server use either the ip address
   *   i) 127.0.0.1 (that is the loopback, i.e. localhost)
   *   ii) the ip of your machine (discover it with `ip addr` or `ifconfig`)
   */

  // READ from argv the command line argument vector, by checking the arguments
  // counter (argc), i.e.,
  //   ./udp_client has argc = 1 and no command line arguments
  //   ./udp_client 192.168.100.12 has argc = 2 and the IP address as argument
  //   ./udp_client 192.168.100.12 55556 has argc = 3 and IP and socket port as 
  //     arguments
  const char* dest_ip = argc == 1 ? "127.0.0.1" : argv[1];
  const int dest_port = argc > 2 ? atoi(argv[2]) : 55555;

  // open a SOCK_DGRAM (UDP) socket
  int sckfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sckfd < 0) {
    std::cout << "ERROR: OPEN SOCKET";
    close(sckfd);
    return -1;
  }
  int option(1);
  setsockopt(sckfd, SOL_SOCKET, SO_REUSEADDR, 
                  (char*)&option, sizeof(option));
  struct sockaddr_in dest_addr = {0}; 
  dest_addr.sin_family = AF_INET; 
  dest_addr.sin_port = htons(dest_port);

  dest_addr.sin_addr.s_addr = inet_addr(dest_ip);
  if (dest_addr.sin_addr.s_addr < 0) {
    std::cout << "ERROR: conversion host_ip to AF_ADDRESS FAILED";
    close(sckfd);
    return -2;
  }
  const size_t max_size = 256;
  char buffer[max_size] = "ciao";
  size_t size = 4;
  // UDP is not connected: we just send to the server and that's it
  if(sendto(sckfd, buffer, size, 0, 
    (struct sockaddr*) & dest_addr, sizeof(dest_addr )) < 0) { 
    std::cout << "ERROR SENDING" << std::endl;
    close(sckfd);
    return -3;
  }
  memset(buffer, 0, max_size); // set buffer to zero for next "fresh" read
  
  /**
   * EXERCISE 2 (you have to complete this): 
   * INSERT THE CODE FOR RECEIVING AND PRINTING TO STANDARD OUTPUT
  **/

  socklen_t addrlen = sizeof(dest_addr);
  int recv_size = recvfrom(sckfd, buffer, max_size, 0, (struct sockaddr*) & dest_addr, &addrlen);
  if(recv_size < 0) { 
    std::cout << "ERROR: RECV" << std::endl;
    close(sckfd); 
    return -5;
  }
  
  std::cout << buffer << std::endl; // print what rx
  
  close(sckfd);
}

