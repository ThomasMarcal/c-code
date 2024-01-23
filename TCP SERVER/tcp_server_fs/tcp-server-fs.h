/* 
 * [2019] Signet Lab - Programming for ICT
 * All Rights Reserved.
 * 
 * Authors Filippo Campagnaro and Michele Polese 
 */

/**
 * @file tcp-server-fs.h
 * @author Filippo Campagnaro
 * @version 1.0.0
 * @brief Socket TCP server class.
 */

#ifndef H_TCP_SERVER_FS
#define H_TCP_SERVER_FS

#include "ethernet-connector.h"



/**
 * Class TcpServer
 */
class TcpServer : public EthConn
{
public:
  /**
   * Class constructor.
   * @param port_num integer with the port number
   */
  TcpServer(int port_num);

  /**
   * Class destructor.
   */
  virtual ~TcpServer();

  /**
   * Accept a new connection and store the IP.
   * @return false if accept fails
   */
  bool acceptConn();

  /**
   * Receive data from remote host.
   * @param buf pointer to the buffer where the received data is stored
   * @return number of bytes received (-1 if error occurred)
   */
  int receive(std::shared_ptr<std::array<char,MTU>> buf) override;

  /**
   * Send data to remote host.
   * @param buf pointer to the buffer where the data to be sent is stored
   * @param size_to_tx size to transmit
   * @return number of bytes sent (-1 if error occurred)
   */
  int transmit(std::shared_ptr<std::array<char,MTU>> buf, 
    size_t size_to_tx) override;

  /**
   * Print the tcp server to the ostream
   * @param out a ostream
   */
  void print(std::ostream& out) const override;

private:
  int l_sock_fd; /**< listener socket file descriptor */
  int sock_fd; /**< data socket file descriptor */

  /** counter of the number of connections*/
  size_t connections_count;

  /**
   * Set the socket options and bind.
   * @return false as soon as any of the operations it performs fails
   */
  bool optionsAndBind();

};

#endif /* H_TCP_SERVER_FS */
