#ifndef H_CHAT_CLIENT
#define H_CHAT_CLIENT

#include "memory"
#include "thread"
#include "string"

constexpr size_t C_MTU = 512; /** <Chat MTU */

/**
 * Class ChatServer
 */
class ChatClient
{
public:
  /**
   * Class constructor.
   * @param port_num integer with the port number
   */
  ChatClient(std::string ip_addr, int port_num);

  /**
   * Class destructor.
   */
  ~ChatClient();

  /**
   * Receives data from remote host.
   * @param buf buffer where the received data is stored
   * @return number of bytes received (-1 if error occurred)
   */
  int recv(char* buf);

  /**
   * Send data to remote host.
   * @param buf pointer to the buffer where the data to be sent is stored
   * @param size_to_tx size to transmit
   * @return number of bytes sent (-1 if error occurred)
   */
  int send(const char* buf, size_t size_to_tx);

private:
  int port; /**< listen socket port number */
  std::string ip; /**< address of who is connected */
  bool exit; /**< true if run should end */
  int sock_fd; /**< data socket file descriptor */
  std::unique_ptr<std::thread> thr_rx; /**< thread for transmission */
  std::unique_ptr<std::thread> thr_tx; /**< thread for reception */

  /**
   * Function called by the destructor to close the sockets
   */
  void quit();

  /**
   * Connect the client to the server
   */
  bool connect();

  /**
   * Function executed in a thread dispatched by the constructor 
   * to start the server listener
   */
  void run();

  /**
   * Function executed in a thread dispatched by the run function 
   * to start the transmission
   */
  void txChat();

};

#endif /* CHAT_CLIENT */

