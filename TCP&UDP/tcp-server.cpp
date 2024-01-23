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
 * Method to revert the elements of the buffer
 * @param buf the buffer
 * @param size the number of elements in the buffer
 */
void revertBuffer(char* buf, size_t size) 
{
  // suggestions: 
  // - iterate it up to half the buffer size, 
  // - use a temporary char variable..
  // - debug by printing your results at each cycle iteration
  // - pay attention to the index!!
  //   Not place buffer[size] as first, because this is a 
  //   zero-terminated string..  
	for(size_t i=0; i<size/2; i++){
			char tmp = buf[i];
			buf[i] = buf[size-i-1];
			buf[size-i-1] = tmp;
	}  
}

int main(int argc, char** argv) {

  // READ from argv the command line argument vector, by checking the arguments
  // counter (argc), i.e.,
  //   ./tcp_server has argc = 1 and no command line arguments
  //   ./tcp_server 55556 has argc = 2 and the socket port as argument
  const int listen_port = argc ==1 ? 55555 : atoi(argv[1]);

  /**
   * EXERCISE 1: RUN THIS CODE:
   * - start the server in your local PC
   *     i) using no parameters (the default values).
   *     ii) using a different port number.
   * - connect to the server clients created in you local PC
   * - connect to the server clients created in one of your lab-mate PC
   * 
   *   To connect to a local server use either the ip address
   *   i) 127.0.0.1 (that is the loopback, i.e. localhost)
   *   ii) the ip of your machine (discover it with `ip addr` or `ifconfig`)
   */

  // open a SOCK_STREAM (TCP) socket
  int scklist = socket(AF_INET, SOCK_STREAM, 0);
  if (scklist < 0){ 
    std::cout << "ERROR: OPEN SOCKET" << std::endl;
    close(scklist);
    return -1;
  }
  // SET SOCKET OPTIONS TO RELEASE THE SOCKET ADDRESS IMMEDIATELY AFTER 
  // THE SOCKET IS CLOSED
  int option(1);
  setsockopt(scklist, SOL_SOCKET, SO_REUSEADDR, 
                  (char*)&option, sizeof(option));
  struct sockaddr_in my_addr = {0}; // set all elements of the struct to 0
  my_addr.sin_family = AF_INET; // address family is AF_INET (IPV4)

  // convert listener_port to network number format
  my_addr.sin_port = htons(listen_port); 
  // accept connection from all interfaces of the Network Interface Card (NIC)
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
  // bind the socket to the port
  if (bind(scklist,(struct sockaddr*)&my_addr, sizeof(my_addr)) < 0) { 
    std::cout << "ERROR: BIND SOCKET" << std::endl;
    close(scklist);
    return -2;
  }

  // Accept at max 1 client at a time: by changing 1 to 5 in the next line 
  // you accept at most the connection of 5 clients in parallel.
  if (listen(scklist, 1) < 0) { 
    std::cout << "ERROR: LISTEN SOCKET" << std::endl;
    close(scklist);
    return -3;
  //ERR
  }

  struct sockaddr_in client_addr;
  socklen_t addr_l = sizeof(client_addr);

  /**
   * EXERCISE 2 (you have to complete this): 
   * insert the "accept - receive - send" code (up to the last close(sockfd)) 
   * into a "while true" block, in order to receive and print data from 
   * different clients sequentially, one at a time. Notice: you must remove
   * close(scklist); at line 109 and 123, the return -4 and -5 replacing them 
   * with a break; to exit the cicle without closing the listener!
   */

  // THE NEXT LINE DOES 3 OPERATIONS, DESCRIBED IN THE FOLLOWING.
  // 1) It accepts the connection of a new client.
  // 2) it creates a new socket used to perform the actual data transmission 
  //    between client ad server. If the socket creation fails, it returns -1.
  // 3) It Saves into client_addr the address of the client connected, so you 
  //    can know who is sending what.
  while(true){
      int sockfd = accept(scklist, (struct sockaddr*) &client_addr, &addr_l);
      if(sockfd < 0) {
        std::cout << "ERROR: ACCEPT CONNECTION" << std::endl;
        continue;
      }
      // The next line prints the client address, converting to char* 
      // the network address (inet_ntoa)
      std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << std::endl; 

      const size_t max_size = 256;
      char buf[max_size] = {0};

      int rcv_size = recv(sockfd,buf,max_size,0);
      if(rcv_size < 0) {
        std::cout << "ERROR: RECV" << std::endl;
        close(sockfd);
	continue;
      }
      std::cout << buf << std::endl;
      
      /**
       * EXERCISE 3 (you have to complete this): 
       * send back to the client whatever you received, 
       * - first without modifing (i.e., as it is) 
       * - then by inverting the buffer order, implementing 
       *   the function void revertBuffer(char* buf,size_t size).--> HARD
       */
      
      revertBuffer(buf,rcv_size);
      int sent_size = send(sockfd,buf,rcv_size,0);
      /** 
        * TODO: SEND AND CHECK THE SENT SIZE:
        * IF < 0 close sockets and return -6
        */
      if(sent_size < 0){
        std::cout << "ERROR: SEND" << std::endl;
        close(sockfd);
	continue;
      }
      memset(buf, 0, max_size);

      close(sockfd);
  }
  close(scklist);
}
