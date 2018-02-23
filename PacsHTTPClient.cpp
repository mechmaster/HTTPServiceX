#include <iostream>

#include <boost/thread.hpp>
#include <boost/format.hpp>

#include "PacsHTTPClient.h"

PacsHTTPClient::PacsHTTPClient(boost::asio::io_service& io_service) :
  m_resolver(io_service),
  m_socket(io_service)
{
}

void PacsHTTPClient::clearBuffers()
{
  m_request.consume(m_request.size());
  m_response.consume(m_response.size());
}

void PacsHTTPClient::send(const std::string serverIP, const unsigned short serverPort, const std::string& serviceText)
{
  m_responseText.clear();
  
  // Form the request. We specify the "Connection: close" header so that the
  // server will close the socket after transmitting the response. This will
  // allow us to treat all data up until the EOF as the content.
  std::iostream request_stream(&m_request);

  std::string requestText = boost::str(boost::format("GET %s HTTP/1.1\r\n"
  "Host: %s\r\n"
  "User-Agent: curl/7.51.0\r\n"
  "Accept: */*\r\n\r\n") %serviceText %serverIP);

  request_stream << requestText;
  
  std::cout << "---------- Request text ----------" << std::endl;
  std::cout << requestText << std::endl;
  std::cout << "---------- End text ----------" << std::endl;
  
  // Start an asynchronous resolve to translate the server and service names
  // into a list of endpoints.
  boost::asio::ip::tcp::resolver::query query(serverIP, boost::str(boost::format("%d") %(serverPort)));
  m_resolver.async_resolve(query, boost::bind(&PacsHTTPClient::handle_resolve, this, boost::asio::placeholders::error,
    boost::asio::placeholders::iterator));
}

void PacsHTTPClient::handle_resolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
{
  if (!err) {
    // Attempt a connection to each endpoint in the list until we
    // successfully establish a connection.
    boost::asio::async_connect(m_socket, endpoint_iterator, boost::bind(&PacsHTTPClient::handle_connect, this, boost::asio::placeholders::error));
  } else {
    std::cout << "Error: " << err.message() << "\n";
  }
}

void PacsHTTPClient::handle_connect(const boost::system::error_code& err)
{
  if (!err) {
    // The connection was successful. Send the request.
    boost::asio::async_write(m_socket, m_request, boost::bind(&PacsHTTPClient::handle_write_request, this, boost::asio::placeholders::error));
  } else {
    std::cout << "Error: " << err.message() << "\n";
  }
}

void PacsHTTPClient::handle_write_request(const boost::system::error_code& err)
{
  if (!err) {
    // Read the response status line. The response_ streambuf will
    // automatically grow to accommodate the entire line. The growth may be
    // limited by passing a maximum size to the streambuf constructor.
    boost::asio::async_read_until(m_socket, m_response, "\r\n", boost::bind(&PacsHTTPClient::handle_read_status_line, this,
      boost::asio::placeholders::error));
  } else {
    std::cout << "Error: " << err.message() << "\n";
  }
}

void PacsHTTPClient::handle_read_status_line(const boost::system::error_code& err)
{
  if (err) {
    std::cout << "Error: " << err << "\n";
    return;
  }

  // Check that response is OK.
  std::istream response_stream(&m_response);

  std::string http_version;
  response_stream >> http_version;

  unsigned int status_code;
  response_stream >> status_code;

  std::string status_message;
  std::getline(response_stream, status_message);
  
  std::cout << http_version << std::endl;
  std::cout << status_code << std::endl;
  std::cout << status_message << std::endl;

  if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
    std::cout << "Invalid response\n";
    return;
  }
  
  if (status_code != 200) {
    std::cout << "Response returned with status code ";
    std::cout << status_code << "\n";
  }
  
  // Read the response headers, which are terminated by a blank line.
  boost::asio::async_read_until(m_socket, m_response, "\r\n\r\n", boost::bind(&PacsHTTPClient::handle_read_headers, this,
    boost::asio::placeholders::error));
}

void PacsHTTPClient::handle_read_headers(const boost::system::error_code& err)
{
  if (err)
  {
    std::cout << "Error: " << err << "\n";
    return;
  }

  // Process the response headers.
  std::istream response_stream(&m_response);

  std::string header;
  while (std::getline(response_stream, header) && header != "\r") {
    std::string::size_type tagR = header.find("\r");
    if (tagR != std::string::npos) {
      header.erase(tagR, 1);
    }
    
    std::string::size_type tagN = header.find("\n");
    if (tagN != std::string::npos) {
      header.erase(tagN, 1);
    }
    
    std::string key;
    std::string value;
    
    std::string::size_type pos =  header.find(":");
    if (pos != std::string::npos) {
      key = header.substr(0, pos);
      value = header.substr(pos + 1, header.size());
      
      m_headerList.insert(std::make_pair(key, value));
    }
  }
  
  // Start reading remaining data until EOF.
  boost::asio::async_read(m_socket, m_response, boost::asio::transfer_at_least(1), boost::bind(&PacsHTTPClient::handle_read_content, this,
    boost::asio::placeholders::error));
}

void PacsHTTPClient::handle_read_content(const boost::system::error_code& err)
{
  if (!err)
  {
    std::string text = std::string(boost::asio::buffer_cast<const char*>(m_response.data()), m_response.size());
    m_responseText += text;
    
    m_response.consume(text.size());
    
    // Continue reading remaining data until EOF.
    boost::asio::async_read(m_socket, m_response, boost::asio::transfer_at_least(1), boost::bind(&PacsHTTPClient::handle_read_content, this,
      boost::asio::placeholders::error));
  }
  else if (err != boost::asio::error::eof)
  {
    std::cout << "Error: " << err << "\n";
  }
  else if (err == boost::asio::error::eof)
  {
    std::string text = std::string(boost::asio::buffer_cast<const char*>(m_response.data()), m_response.size());
    m_responseText += text;
  }
}

std::string PacsHTTPClient::getResponseText()
{
  return m_responseText;
}

std::string PacsHTTPClient::getHeaderByName(const std::string name)
{
  std::map<std::string, std::string>::iterator iter = m_headerList.find(name);
  if (iter == m_headerList.end())
  {
    return std::string();
  }
  
  return iter->second;
}
