/* 
 * [2019] Signet Lab - Programming for ICT
 * All Rights Reserved.
 * 
 * Authors Filippo Campagnaro and Michele Polese 
 */

#include <tcp-server-fs.h>
#include <basic-utilities.h>
#include <iostream>
#include <cctype>

/**
 * change the C string to uppercase
 * @param buffer pointing to the C string 
 * @param size of the string
 */
void toUpperCase(std::shared_ptr<std::array<char,MTU>> buffer, size_t size) 
{
  // TODO (this is the last thing to do during this exam): change the C string 
  // stored into the buffer to uppercase.
  // SUGGESTION 1: to directly access the underlying C-array of buffer, you 
  // can use the method buffer->data(), then you can access the first element 
  // with buffer->data()[0]; 
  // SUGGESTION 2: to change a SINGLE char i to uppercase, you can use 
  // i = toupper(i); you need do do that for all the elements of the array
  	for(size_t i = 0; i < size; ++i) {
		buffer->data()[i] = toupper(buffer->data()[i]);
	}
}


/**
 * Method used to accept a new connection from an opened server
 * @param server pointer to an EthConn, that points to a TcpServer
 * @return false if the object is not a TcpServer or if the accept fails
 */
bool acceptTcpConn(std::shared_ptr<EthConn> server) 
{
  auto casted_server = std::dynamic_pointer_cast<TcpServer>(server);
  if(casted_server != nullptr && casted_server->acceptConn()) {
    std::cout << "server accepted client " << casted_server->getIp() << std::endl;
    return true;
  }
  return false;
}

int main(int argc, char** argv) 
{

  // READ from argv the command line argument vector, by checking the arguments
  // counter (argc), i.e.,
  //   ./tcp_server has argc = 1 and no command line arguments
  //   ./tcp_server 55556 has argc = 2 and the socket port as argument
  const int listen_port = argc ==1 ? 55555 : atoi(argv[1]);
  std::shared_ptr<EthConn> server = std::make_shared<TcpServer>(listen_port);
  
  if(!acceptTcpConn(server)) {
    return (int)TestResult::FAILED;
  }

  auto rx_buffer = std::make_shared<std::array<char,MTU>>();

  int rx_size = server->receive(rx_buffer);
  if(rx_size>0) {
    std::cout << "received = " << rx_buffer->data() << std::endl;
  } else {
    return (int)TestResult::FAILED;
  }

  toUpperCase(rx_buffer,rx_size);

  std::cout << "upper case = " << rx_buffer->data() << std::endl;

  if(server->transmit(rx_buffer,rx_size) < 0) {
    return (int)TestResult::FAILED;
  }

  std::cout << *server << std::endl;
  return (int)TestResult::PASSED;

}
