#pragma once

#include <string>
#include <vector>
#include <map>

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/bind.hpp>

class PacsHTTPClient
{
public:
  
  PacsHTTPClient(boost::asio::io_service& io_service);
  
  void send(const std::string serverIP, const unsigned short serverPort, const std::string& serviceText);
  std::string getResponseText();
  std::string getHeaderByName(const std::string name);

  void clearBuffers();
  
private:
  
  void handle_resolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
  void handle_connect(const boost::system::error_code& err);
  void handle_write_request(const boost::system::error_code& err);
  void handle_read_status_line(const boost::system::error_code& err);
  void handle_read_headers(const boost::system::error_code& err);
  void handle_read_content(const boost::system::error_code& err);
  
  boost::asio::ip::tcp::resolver m_resolver;
  boost::asio::ip::tcp::socket m_socket;
  boost::asio::streambuf m_request;
  boost::asio::streambuf m_response;
  
  std::map<std::string, std::string> m_headerList;
  std::string m_responseText;
};
