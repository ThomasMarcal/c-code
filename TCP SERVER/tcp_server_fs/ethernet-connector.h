/* 
 * [2019] Signet Lab - Programming for ICT
 * All Rights Reserved.
 * 
 * Authors Filippo Campagnaro and Michele Polese 
 */

/**
 * @file etherenet-connector.h
 * @author Filippo Campagnaro
 * @version 1.0.0
 * @brief Base Ethernet connector.
 */

#ifndef H_ETHERNET_C_FS
#define H_ETHERNET_C_FS

#include <string>
#include <memory>
#include <array>

constexpr size_t MTU = 512; /** Maximum transmission unit*/

/**
 * class EthConn containing the API of an Ethernet connector
 */
class EthConn
{
public:
  /** String with the LOCAL_HOST IPv4 address. */
  static const std::string LOCAL_HOST; 
  
  /**
   * Class constructor.
   * @param ip_addr string containing the human readable IP address in IPv4
   * @param port_num integer with the port number
   */
  EthConn(const std::string& ip_addr, int port_num);

  /**
   * Class constructor.
   * @param port_num integer with the port number
   */
  EthConn(int port_num);

  /**
   * Class destructor.
   */
  virtual ~EthConn() {}

  /**
   * Receive data from remote host.
   * @param buf pointer to the buffer where the received data is stored
   * @return number of bytes received (-1 if error occurred)
   */
  virtual int receive(std::shared_ptr<std::array<char,MTU>> buf) = 0;

  /**
   * Send data to remote host.
   * @param buf pointer to the buffer where the data to be sent is stored
   * @param size_to_tx size to transmit
   * @return number of bytes sent (-1 if error occurred)
   */
  virtual int transmit(std::shared_ptr<std::array<char,MTU>> buf, 
    size_t size_to_tx) = 0;

  /**
   * Get the IP address of the last remote host connected.
   * @return the ip address
   */
  //TODO: implement the method getIp()
  std::string getIp(){
	  return ip;
  } 

  /**
   * Get the socket port number.
   * @return the port number
   */
  //TODO: implement the method getPort()
  int getPort() const{
	  return port;
  }
  /**
   * Print the connector to the ostream
   * @param out a ostream
   */
  virtual void print(std::ostream& out) const;

protected:
  /** string containing the human readable IP address */
  std::string ip; 
  /** integer with the port number*/
  int port;

};

/**
 * Overloaded operator<< for a EthConn
 * @param out a std::ostream object
 * @param conn reference to the connector to be printed
 * @return the manipulated ostream object
 */
std::ostream& operator<<(std::ostream& out, const EthConn& conn);

#endif /* H_ETHERNET_C_FS */
