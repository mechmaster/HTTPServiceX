#include <iostream>

#include <boost/format.hpp>

#include "Client.h"
#include "Server.h"

int main(int argc, char **argv)
{
  std::string serverIP = "vk.com";
  std::string serviceText = "/";

  Client cl;
  cl.start(serverIP, "80");

  std::string request = boost::str(boost::format("GET %s HTTP/1.1\r\n"
    "Host: %s\r\n"
    "User-Agent: curl/7.51.0\r\n"
    "Accept: */*\r\n\r\n") %serviceText %serverIP);

  cl.sendMessage(request);

  while (true)
  {
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
  }

  cl.stop();

  std::cout << "Hello, world!" << std::endl;
  return 0;
}

/*#include <iostream>
#include <boost/asio.hpp>
#include <boost/format.hpp>

int main(int argc, char* argv[])
{
  std::string serverIP = "74.125.232.239";

  boost::asio::io_service io_service;
  boost::asio::ip::tcp::socket server_s(io_service);
  
  boost::system::error_code error;  
  boost::asio::ip::tcp::resolver::query query(serverIP, "80");
  boost::asio::ip::tcp::resolver resolver(io_service);
  boost::asio::ip::tcp::resolver::iterator destination = resolver.resolve(query, error);
  boost::asio::ip::tcp::endpoint endpoint;
  
  while (destination != boost::asio::ip::tcp::resolver::iterator())
  {
    endpoint = *destination++;
    std::cout<<endpoint<<std::endl;
  }

  server_s.connect(endpoint, error);
  if (error)
  {
    std::cout << "Error connect to server: " << error << std::endl;
  }
  
  std::string serviceText = "/";
  
  std::string responce;
  responce.resize(4096);
  std::string request = boost::str(boost::format("GET %s HTTP/1.1\r\n"
    "Host: %s\r\n"
    "User-Agent: curl/7.51.0\r\n"
    "Accept: *//*\/*r\n\r\n") %serviceText %serverIP);

  server_s.write_some(boost::asio::buffer(request));
  unsigned int responceSize = server_s.read_some(boost::asio::buffer(&responce[0], responce.size()));
  
  responce.resize(responceSize);

  //io_service.run();

  std::cout << "Server answer: " << responce << std::endl;
  return 0;
}*/
