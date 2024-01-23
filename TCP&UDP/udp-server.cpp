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

/**
 * Method to replace the buffer with capital letters
 * @param buf the buffer
 * @param size the number of elements in the buffer
 */
void toUpper(char* buf, size_t size) {
  // suggestion: iterate the buffer accessing it as an array 
  // (with []) and apply the c method `toupper` to each element of the array
  for (size_t i = 0; i < size; i++)
  {
    buf[i] = toupper(buf[i]);
  }
  
}

int main(int argc, char** argv) {

  /**
   * EXERCISE 1: RUN THIS CODE:
   * - with clients from you local PC
   * - with clients from one of your lab-mate PC
   * - using no parameters (the default values)
   * - using different parameters of ip and port. 
   * - using no parameters (the default values)
   * - using different parameters of port. In this case connect the ip
   *   To connect to a local server use either the ip address
   *   i) 127.0.0.1 (that is the loopback, i.e. localhost)
   *   ii) the ip of your machine (discover it with `ip addr` or `ifconfig`)
   */

  // READ from argv the command line argument vector, by checking the arguments 
  // counter (argc), i.e.,
  //   ./udp_server has argc = 1 and no command line arguments
  //   ./udp_server 55556 has argc = 2 and the socket port as argument
  const int recv_port = argc ==1 ? 55555 : atoi(argv[1]);

  // open a SOCK_DGRAM (UDP) socket
  int sckfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sckfd < 0){ 
    std::cout << "sckfd < 0" ; return -1;
  }
 int option(1);
 setsockopt(sckfd, SOL_SOCKET, SO_REUSEADDR, 
            (char*)&option, sizeof(option));
  struct sockaddr_in my_addr = {0};
  struct sockaddr_in srcaddr; 
  socklen_t addrlen = sizeof(srcaddr);
  my_addr.sin_family = AF_INET;

  my_addr.sin_port = htons(recv_port);
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);  

  if (bind(sckfd, (struct sockaddr*) &my_addr, sizeof(my_addr))<0) {   
    std::cout << "bind socket address failed" ; return -2;
  }
  const size_t max_size = 256;
  char buffer[max_size] = {0}; 
  
  /**
   * EXERCISE 2 (you have to complete this): 
   * insert the receive code into a while true block, in order to 
   * receive and print data from different clients sequentially, one at a time.
   * Run it with clients placed in several host and see how the ip changes
   */

  // UDP is not connected: we just receive from someone (that may change time 
  // by time) and that's it. We can check how sent something storing its 
  // addess to srcaddr and send him back something through the same socket.

  while(true){
      int rcv_size = recvfrom(sckfd, buffer, max_size, 0, (struct sockaddr *)   
                          &srcaddr, &addrlen);
      if(rcv_size < 0) {
        std::cout << "ERROR: RECV" << std::endl;
        break;
      }
      char* ip = inet_ntoa(srcaddr.sin_addr);
      std::cout << ip << std::endl;
      std::cout << buffer << std::endl; 

      /**
       * EXERCISE 3 (you have to complete this):  
       * send back to the client whatever you received, 
       * - first without modifing (i.e., as it is) 
       * - then it by placing it to capital letters, implementing 
       *   the function void toUpper(char* buf,size_t size). --> MEDIUM/HARD
       */
      
      toUpper(buffer, rcv_size);
      int sent_size = sendto(sckfd,buffer,rcv_size,0, (struct sockaddr *) &srcaddr, addrlen);
      
      /** 
       * TODO: SEND AND CHECK THE SENT SIZE:
       * IF < 0 close sockets and return -6 AN ERROR
       */
      if(sent_size < 0){
        std::cout << "ERROR: SEND" << std::endl;
        break;
      }
      memset(buffer, 0, max_size); // set buffer to zero for next "fresh" read

  }
  close(sckfd);
}

