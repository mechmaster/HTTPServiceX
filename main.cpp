﻿#include <iostream>

#include <boost/format.hpp>

#include "Client.h"
#include "Server.h"

#include <lz4.h>
#include <snappy.h>

void query(Client& cl, const std::vector<std::uint8_t>& request)
{
  cl.sendMessage(request);

  int timeout = 0;
  while (true)
  {
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    timeout += 100;
    
    if (!cl.isConnected())
    {
      return;
    }

    if (timeout >= 60000)
    {
        break;
    }

    if (cl.hasResponce())
    {
        break;
    }
  }

  std::vector<std::uint8_t> responce = cl.getResponce();
  
  /*if (responce.size() && responce[1] == 0x01)
  {
    std::string uncompress_packet;
    auto snappy_res = snappy::Uncompress(reinterpret_cast<char*>(&responce[9]), responce.size() - 9, &uncompress_packet);
    if (snappy_res)
    {
        std::cout << "Uncompress succesfull!";
    }
    else
    {
        std::cout << "responce size = " << responce.size() << ". Uncompress failed!";
    }
  }*/
  
  std::cout << std::endl;
  std::cout << "{";
  for (unsigned int i = 0; i < responce.size(); ++i)
  {
      std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned short int>(responce[i]) << ", ";
  }
  std::cout << "}";
  std::cout << std::endl;
}

void non_compression_test(Client& cl)
{
    std::vector<std::uint8_t> startup = {0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x53, 0x00, 0x03, 0x00, 0x0b, 0x44, 0x52,
        0x49, 0x56, 0x45, 0x52, 0x5f, 0x4e, 0x41, 0x4d, 0x45, 0x00, 0x16, 0x44, 0x61, 0x74, 0x61, 0x53, 0x74, 0x61, 0x78, 0x20,
        0x50, 0x79, 0x74, 0x68, 0x6f, 0x6e, 0x20, 0x44, 0x72, 0x69, 0x76, 0x65, 0x72, 0x00, 0x0e, 0x44, 0x52, 0x49, 0x56, 0x45,
        0x52, 0x5f, 0x56, 0x45, 0x52, 0x53, 0x49, 0x4f, 0x4e, 0x00, 0x06, 0x33, 0x2e, 0x31, 0x37, 0x2e, 0x30, 0x00, 0x0b, 0x43,
        0x51, 0x4c, 0x5f, 0x56, 0x45, 0x52, 0x53, 0x49, 0x4f, 0x4e, 0x00, 0x05, 0x33, 0x2e, 0x30, 0x2e, 0x30};
    query(cl, startup);
        
    std::vector<std::uint8_t> loginandpass = {0x04, 0x00, 0x00, 0x01, 0x0f, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x14,
        0x00, 0x63, 0x61, 0x73, 0x73, 0x61, 0x6e, 0x64, 0x72, 0x61, 0x00, 0x63, 0x61, 0x73, 0x73, 0x61, 0x6e, 0x64, 0x72, 0x61};
    query(cl, loginandpass);

    std::vector<std::uint8_t> createkeyspace = {0x04, 0x00, 0x00, 0x02, 0x07, 0x00, 0x00, 0x00, 0x87, 0x00, 0x00, 0x00, 0x74, 0x43,
        0x52, 0x45, 0x41, 0x54, 0x45, 0x20, 0x4b, 0x45, 0x59, 0x53, 0x50, 0x41, 0x43, 0x45, 0x20, 0x69, 0x66, 0x20, 0x6e, 0x6f,
        0x74, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74, 0x73, 0x20, 0x6d, 0x65, 0x63, 0x68, 0x6d, 0x61, 0x73, 0x74, 0x65, 0x72, 0x20,
        0x57, 0x49, 0x54, 0x48, 0x20, 0x72, 0x65, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x3d, 0x20, 0x7b,
        0x20, 0x27, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x27, 0x3a, 0x20, 0x27, 0x53, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x53, 0x74, 0x72,
        0x61, 0x74, 0x65, 0x67, 0x79, 0x27, 0x2c, 0x27, 0x72, 0x65, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x5f,
        0x66, 0x61, 0x63, 0x74, 0x6f, 0x72, 0x27, 0x3a, 0x20, 0x27, 0x33, 0x27, 0x20, 0x7d, 0x3b, 0x00, 0x0a, 0x24, 0x00, 0x00,
        0x03, 0xe8, 0x00, 0x05, 0x9c, 0x06, 0xa0, 0x8b, 0x9c, 0x1f};
    query(cl, createkeyspace);

    std::vector<std::uint8_t> createtable = {0x04, 0x00, 0x00, 0x03, 0x07, 0x00, 0x00, 0x00, 0x71, 0x00, 0x00, 0x00, 0x5e, 0x63,
        0x72, 0x65, 0x61, 0x74, 0x65, 0x20, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x69, 0x66, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x65,
        0x78, 0x69, 0x73, 0x74, 0x73, 0x20, 0x6d, 0x65, 0x63, 0x68, 0x6d, 0x61, 0x73, 0x74, 0x65, 0x72, 0x2e, 0x74, 0x65, 0x73,
        0x74, 0x5f, 0x6d, 0x61, 0x70, 0x31, 0x20, 0x28, 0x69, 0x64, 0x20, 0x69, 0x6e, 0x74, 0x20, 0x70, 0x72, 0x69, 0x6d, 0x61,
        0x72, 0x79, 0x20, 0x6b, 0x65, 0x79, 0x2c, 0x20, 0x63, 0x31, 0x20, 0x6d, 0x61, 0x70, 0x3c, 0x76, 0x61, 0x72, 0x63, 0x68,
        0x61, 0x72, 0x2c, 0x76, 0x61, 0x72, 0x63, 0x68, 0x61, 0x72, 0x3e, 0x29, 0x3b, 0x00, 0x0a, 0x24, 0x00, 0x00, 0x03, 0xe8,
        0x00, 0x05, 0x9c, 0x06, 0xa0, 0x8c, 0x37, 0x7c};
    query(cl, createtable);

    std::vector<std::uint8_t> insertinto = {0x04, 0x00, 0x00, 0x04, 0x07, 0x00, 0x00, 0x00, 0x72, 0x00, 0x00, 0x00, 0x5f, 0x69,
        0x6e, 0x73, 0x65, 0x72, 0x74, 0x20, 0x69, 0x6e, 0x74, 0x6f, 0x20, 0x6d, 0x65, 0x63, 0x68, 0x6d, 0x61, 0x73, 0x74, 0x65,
        0x72, 0x2e, 0x74, 0x65, 0x73, 0x74, 0x5f, 0x6d, 0x61, 0x70, 0x31, 0x20, 0x28, 0x69, 0x64, 0x2c, 0x20, 0x63, 0x31, 0x29,
        0x20, 0x76, 0x61, 0x6c, 0x75, 0x65, 0x73, 0x20, 0x28, 0x31, 0x2c, 0x20, 0x7b, 0x27, 0x68, 0x73, 0x67, 0x6b, 0x61, 0x73,
        0x27, 0x3a, 0x27, 0x6a, 0x68, 0x67, 0x6b, 0x66, 0x6a, 0x64, 0x73, 0x67, 0x68, 0x6b, 0x27, 0x2c, 0x20, 0x27, 0x6a, 0x68,
        0x67, 0x6b, 0x6a, 0x27, 0x3a, 0x27, 0x67, 0x68, 0x73, 0x72, 0x27, 0x7d, 0x29, 0x3b, 0x00, 0x0a, 0x24, 0x00, 0x00, 0x03,
        0xe8, 0x00, 0x05, 0x9c, 0x06, 0xa0, 0x8c, 0xea, 0x8c};
    query(cl, insertinto);

    std::vector<std::uint8_t> select = {0x04, 0x00, 0x00, 0x2c, 0x07, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00, 0x23, 0x73, 0x65,
        0x6c, 0x65, 0x63, 0x74, 0x20, 0x2a, 0x20, 0x66, 0x72, 0x6f, 0x6d, 0x20, 0x6d, 0x65, 0x63, 0x68, 0x6d, 0x61, 0x73, 0x74,
        0x65, 0x72, 0x2e, 0x74, 0x65, 0x73, 0x74, 0x5f, 0x6d, 0x61, 0x70, 0x31, 0x3b, 0x00, 0x0a, 0x2c, 0x00, 0x00, 0x03, 0xe8,
        0x00, 0x00, 0x00, 0x0c, 0x04, 0x00, 0x00, 0x7f, 0x7b, 0x00, 0xf0, 0x7f, 0xff, 0x77, 0x47, 0x00, 0x00, 0x05, 0x9c, 0x06,
        0x02, 0x8c, 0x37, 0xad};
    query(cl, select);
}

void lz4_compression_test(Client& cl)
{
    std::vector<std::uint8_t> startup = {0x04, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x65, 0x00, 0x04, 0x00, 0x0b, 0x44,
        0x52, 0x49, 0x56, 0x45, 0x52, 0x5f, 0x4e, 0x41, 0x4d, 0x45, 0x00, 0x16, 0x44, 0x61, 0x74, 0x61, 0x53, 0x74, 0x61, 0x78,
        0x20, 0x50, 0x79, 0x74, 0x68, 0x6f, 0x6e, 0x20, 0x44, 0x72, 0x69, 0x76, 0x65, 0x72, 0x00, 0x0e, 0x44, 0x52, 0x49, 0x56,
        0x45, 0x52, 0x5f, 0x56, 0x45, 0x52, 0x53, 0x49, 0x4f, 0x4e, 0x00, 0x06, 0x33, 0x2e, 0x31, 0x37, 0x2e, 0x30, 0x00, 0x0b,
        0x43, 0x4f, 0x4d, 0x50, 0x52, 0x45, 0x53, 0x53, 0x49, 0x4f, 0x4e, 0x00, 0x03, 0x6c, 0x7a, 0x34, 0x00, 0x0b, 0x43, 0x51,
        0x4c, 0x5f, 0x56, 0x45, 0x52, 0x53, 0x49, 0x4f, 0x4e, 0x00, 0x05, 0x33, 0x2e, 0x34, 0x2e, 0x34};
    query(cl, startup);
    
    std::vector<std::uint8_t> loginandpass = {0x04, 0x00, 0x00, 0x02, 0x0f, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x14, 0x00,
        0x63, 0x61, 0x73, 0x73, 0x61, 0x6e, 0x64, 0x72, 0x61, 0x00, 0x63, 0x61, 0x73, 0x73, 0x61, 0x6e, 0x64, 0x72, 0x61};
    query(cl, loginandpass);
    
    std::vector<std::uint8_t> createkeyspace = {0x04, 0x01, 0x00, 0x03, 0x07, 0x00, 0x00, 0x00, 0x85, 0x00, 0x00, 0x00, 0x87, 0xf7,
        0x4e, 0x00, 0x00, 0x00, 0x74, 0x43, 0x52, 0x45, 0x41, 0x54, 0x45, 0x20, 0x4b, 0x45, 0x59, 0x53, 0x50, 0x41, 0x43, 0x45,
        0x20, 0x69, 0x66, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74, 0x73, 0x20, 0x6d, 0x65, 0x63, 0x68, 0x6d,
        0x61, 0x73, 0x74, 0x65, 0x72, 0x20, 0x57, 0x49, 0x54, 0x48, 0x20, 0x72, 0x65, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69,
        0x6f, 0x6e, 0x20, 0x3d, 0x20, 0x7b, 0x20, 0x27, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x27, 0x3a, 0x20, 0x27, 0x53, 0x69, 0x6d,
        0x70, 0x6c, 0x65, 0x53, 0x74, 0x72, 0x61, 0x74, 0x65, 0x67, 0x79, 0x27, 0x2c, 0x27, 0x2b, 0x00, 0x70, 0x5f, 0x66, 0x61,
        0x63, 0x74, 0x6f, 0x72, 0x27, 0x00, 0xf0, 0x05, 0x33, 0x27, 0x20, 0x7d, 0x3b, 0x00, 0x0a, 0x24, 0x00, 0x00, 0x03, 0xe8,
        0x00, 0x05, 0x9c, 0x17, 0x39, 0x3e, 0x8d, 0x59};
    query(cl, createkeyspace);
    
    std::vector<std::uint8_t> createtable = {0x04, 0x01, 0x00, 0x04, 0x07, 0x00, 0x00, 0x00, 0x74, 0x00, 0x00, 0x00, 0x71, 0xf3,
        0x49, 0x00, 0x00, 0x00, 0x5e, 0x63, 0x72, 0x65, 0x61, 0x74, 0x65, 0x20, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x69, 0x66,
        0x20, 0x6e, 0x6f, 0x74, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74, 0x73, 0x20, 0x6d, 0x65, 0x63, 0x68, 0x6d, 0x61, 0x73, 0x74,
        0x65, 0x72, 0x2e, 0x74, 0x65, 0x73, 0x74, 0x5f, 0x6d, 0x61, 0x70, 0x31, 0x20, 0x28, 0x69, 0x64, 0x20, 0x69, 0x6e, 0x74,
        0x20, 0x70, 0x72, 0x69, 0x6d, 0x61, 0x72, 0x79, 0x20, 0x6b, 0x65, 0x79, 0x2c, 0x20, 0x63, 0x31, 0x20, 0x6d, 0x61, 0x70,
        0x3c, 0x76, 0x61, 0x72, 0x63, 0x68, 0x61, 0x72, 0x2c, 0x08, 0x00, 0xf0, 0x03, 0x3e, 0x29, 0x3b, 0x00, 0x0a, 0x24, 0x00,
        0x00, 0x03, 0xe8, 0x00, 0x05, 0x9c, 0x17, 0x39, 0x3f, 0x09, 0x5d};
    query(cl, createtable);
    
    std::vector<std::uint8_t> insertinto = {0x04, 0x01, 0x00, 0x05, 0x07, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x72, 0xf0,
        0x63, 0x00, 0x00, 0x00, 0x5f, 0x69, 0x6e, 0x73, 0x65, 0x72, 0x74, 0x20, 0x69, 0x6e, 0x74, 0x6f, 0x20, 0x6d, 0x65, 0x63,
        0x68, 0x6d, 0x61, 0x73, 0x74, 0x65, 0x72, 0x2e, 0x74, 0x65, 0x73, 0x74, 0x5f, 0x6d, 0x61, 0x70, 0x31, 0x20, 0x28, 0x69,
        0x64, 0x2c, 0x20, 0x63, 0x31, 0x29, 0x20, 0x76, 0x61, 0x6c, 0x75, 0x65, 0x73, 0x20, 0x28, 0x31, 0x2c, 0x20, 0x7b, 0x27,
        0x68, 0x73, 0x67, 0x6b, 0x61, 0x73, 0x27, 0x3a, 0x27, 0x6a, 0x68, 0x67, 0x6b, 0x66, 0x6a, 0x64, 0x73, 0x67, 0x68, 0x6b,
        0x27, 0x2c, 0x20, 0x27, 0x6a, 0x68, 0x67, 0x6b, 0x6a, 0x27, 0x3a, 0x27, 0x67, 0x68, 0x73, 0x72, 0x27, 0x7d, 0x29, 0x3b,
        0x00, 0x0a, 0x24, 0x00, 0x00, 0x03, 0xe8, 0x00, 0x05, 0x9c, 0x17, 0x39, 0x3f, 0x81, 0x14};
    query(cl, insertinto);
    
    std::vector<std::uint8_t> select = {0x04, 0x01, 0x00, 0x06, 0x07, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x36, 0xf0, 0x27,
        0x00, 0x00, 0x00, 0x23, 0x73, 0x65, 0x6c, 0x65, 0x63, 0x74, 0x20, 0x2a, 0x20, 0x66, 0x72, 0x6f, 0x6d, 0x20, 0x6d, 0x65,
        0x63, 0x68, 0x6d, 0x61, 0x73, 0x74, 0x65, 0x72, 0x2e, 0x74, 0x65, 0x73, 0x74, 0x5f, 0x6d, 0x61, 0x70, 0x31, 0x3b, 0x00,
        0x0a, 0x24, 0x00, 0x00, 0x03, 0xe8, 0x00, 0x05, 0x9c, 0x17, 0x39, 0x3f, 0xf9, 0x52};
    query(cl, select);
}

void snappy_compression_test(Client& cl)
{
    std::vector<std::uint8_t> startup = {0x04, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x68, 0x00, 0x04, 0x00, 0x0b, 0x44, 0x52,
        0x49, 0x56, 0x45, 0x52, 0x5f, 0x4e, 0x41, 0x4d, 0x45, 0x00, 0x16, 0x44, 0x61, 0x74, 0x61, 0x53, 0x74, 0x61, 0x78, 0x20,
        0x50, 0x79, 0x74, 0x68, 0x6f, 0x6e, 0x20, 0x44, 0x72, 0x69, 0x76, 0x65, 0x72, 0x00, 0x0e, 0x44, 0x52, 0x49, 0x56, 0x45,
        0x52, 0x5f, 0x56, 0x45, 0x52, 0x53, 0x49, 0x4f, 0x4e, 0x00, 0x06, 0x33, 0x2e, 0x31, 0x37, 0x2e, 0x30, 0x00, 0x0b, 0x43,
        0x4f, 0x4d, 0x50, 0x52, 0x45, 0x53, 0x53, 0x49, 0x4f, 0x4e, 0x00, 0x06, 0x73, 0x6e, 0x61, 0x70, 0x70, 0x79, 0x00, 0x0b,
        0x43, 0x51, 0x4c, 0x5f, 0x56, 0x45, 0x52, 0x53, 0x49, 0x4f, 0x4e, 0x00, 0x05, 0x33, 0x2e, 0x34, 0x2e, 0x34};
    query(cl, startup);
    
    std::vector<std::uint8_t> loginandpass = {0x04, 0x00, 0x00, 0x02, 0x0f, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x14, 0x00,
        0x63, 0x61, 0x73, 0x73, 0x61, 0x6e, 0x64, 0x72, 0x61, 0x00, 0x63, 0x61, 0x73, 0x73, 0x61, 0x6e, 0x64, 0x72, 0x61};
    query(cl, loginandpass);
    
    std::vector<std::uint8_t> createkeyspace = {0x04, 0x01, 0x00, 0x03, 0x07, 0x00, 0x00, 0x00, 0x88, 0x87, 0x01, 0xf0, 0x61, 0x00,
        0x00, 0x00, 0x74, 0x43, 0x52, 0x45, 0x41, 0x54, 0x45, 0x20, 0x4b, 0x45, 0x59, 0x53, 0x50, 0x41, 0x43, 0x45, 0x20, 0x69,
        0x66, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74, 0x73, 0x20, 0x6d, 0x65, 0x63, 0x68, 0x6d, 0x61, 0x73,
        0x74, 0x65, 0x72, 0x20, 0x57, 0x49, 0x54, 0x48, 0x20, 0x72, 0x65, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e,
        0x20, 0x3d, 0x20, 0x7b, 0x20, 0x27, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x27, 0x3a, 0x20, 0x27, 0x53, 0x69, 0x6d, 0x70, 0x6c,
        0x65, 0x53, 0x74, 0x72, 0x61, 0x74, 0x65, 0x67, 0x79, 0x27, 0x2c, 0x27, 0x72, 0x65, 0x70, 0x6c, 0x69, 0x09, 0x2b, 0x78,
        0x5f, 0x66, 0x61, 0x63, 0x74, 0x6f, 0x72, 0x27, 0x3a, 0x20, 0x27, 0x33, 0x27, 0x20, 0x7d, 0x3b, 0x00, 0x0a, 0x24, 0x00,
        0x00, 0x03, 0xe8, 0x00, 0x05, 0x9c, 0x17, 0x69, 0x08, 0xff, 0xa5};
    query(cl, createkeyspace);
    
    std::vector<std::uint8_t> createtable = {0x04, 0x01, 0x00, 0x04, 0x07, 0x00, 0x00, 0x00, 0x74, 0x71, 0xf0, 0x70, 0x00, 0x00,
        0x00, 0x5e, 0x63, 0x72, 0x65, 0x61, 0x74, 0x65, 0x20, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x69, 0x66, 0x20, 0x6e, 0x6f,
        0x74, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74, 0x73, 0x20, 0x6d, 0x65, 0x63, 0x68, 0x6d, 0x61, 0x73, 0x74, 0x65, 0x72, 0x2e,
        0x74, 0x65, 0x73, 0x74, 0x5f, 0x6d, 0x61, 0x70, 0x31, 0x20, 0x28, 0x69, 0x64, 0x20, 0x69, 0x6e, 0x74, 0x20, 0x70, 0x72,
        0x69, 0x6d, 0x61, 0x72, 0x79, 0x20, 0x6b, 0x65, 0x79, 0x2c, 0x20, 0x63, 0x31, 0x20, 0x6d, 0x61, 0x70, 0x3c, 0x76, 0x61,
        0x72, 0x63, 0x68, 0x61, 0x72, 0x2c, 0x76, 0x61, 0x72, 0x63, 0x68, 0x61, 0x72, 0x3e, 0x29, 0x3b, 0x00, 0x0a, 0x24, 0x00,
        0x00, 0x03, 0xe8, 0x00, 0x05, 0x9c, 0x17, 0x69, 0x09, 0x8b, 0x03};
    query(cl, createtable);
    
    std::vector<std::uint8_t> insertinto = {0x04, 0x01, 0x00, 0x05, 0x07, 0x00, 0x00, 0x00, 0x74, 0x72, 0xf0, 0x52, 0x00, 0x00,
        0x00, 0x5f, 0x69, 0x6e, 0x73, 0x65, 0x72, 0x74, 0x20, 0x69, 0x6e, 0x74, 0x6f, 0x20, 0x6d, 0x65, 0x63, 0x68, 0x6d, 0x61,
        0x73, 0x74, 0x65, 0x72, 0x2e, 0x74, 0x65, 0x73, 0x74, 0x5f, 0x6d, 0x61, 0x70, 0x31, 0x20, 0x28, 0x69, 0x64, 0x2c, 0x20,
        0x63, 0x31, 0x29, 0x20, 0x76, 0x61, 0x6c, 0x75, 0x65, 0x73, 0x20, 0x28, 0x31, 0x2c, 0x20, 0x7b, 0x27, 0x68, 0x73, 0x67,
        0x6b, 0x61, 0x73, 0x27, 0x3a, 0x27, 0x6a, 0x68, 0x67, 0x6b, 0x66, 0x6a, 0x64, 0x73, 0x67, 0x68, 0x6b, 0x27, 0x2c, 0x20,
        0x27, 0x01, 0x0f, 0x68, 0x6a, 0x27, 0x3a, 0x27, 0x67, 0x68, 0x73, 0x72, 0x27, 0x7d, 0x29, 0x3b, 0x00, 0x0a, 0x24, 0x00,
        0x00, 0x03, 0xe8, 0x00, 0x05, 0x9c, 0x17, 0x69, 0x0a, 0x10, 0x9e};
    query(cl, insertinto);

    std::vector<std::uint8_t> select = {0x04, 0x01, 0x00, 0x06, 0x07, 0x00, 0x00, 0x00, 0x38, 0x36, 0xd4, 0x00, 0x00, 0x00, 0x23,
        0x73, 0x65, 0x6c, 0x65, 0x63, 0x74, 0x20, 0x2a, 0x20, 0x66, 0x72, 0x6f, 0x6d, 0x20, 0x6d, 0x65, 0x63, 0x68, 0x6d, 0x61,
        0x73, 0x74, 0x65, 0x72, 0x2e, 0x74, 0x65, 0x73, 0x74, 0x5f, 0x6d, 0x61, 0x70, 0x31, 0x3b, 0x00, 0x0a, 0x24, 0x00, 0x00,
        0x03, 0xe8, 0x00, 0x05, 0x9c, 0x17, 0x69, 0x0a, 0x8b, 0xc8};
    query(cl, select);
}

void compression_stress_test(Client& cl)
{
    std::vector<std::uint8_t> r1 = {0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00};
query(cl, r1);
std::vector<std::uint8_t> r2 = {0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00};
query(cl, r2);
std::vector<std::uint8_t> r3 = {0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00};
query(cl, r3);

std::vector<std::uint8_t> r4 = {0x04, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x02, 0x00, 0x0b, 0x43, 0x4f, 0x4d, 0x50, 0x52, 0x45, 0x53, 0x53, 0x49,
    0x4f, 0x4e, 0x00, 0x06, 0x73, 0x6e, 0x61, 0x70, 0x70, 0x79, 0x00, 0x0b, 0x43, 0x51, 0x4c, 0x5f, 0x56, 0x45, 0x52, 0x53, 0x49, 0x4f, 0x4e, 0x00, 0x05, 0x33, 0x2e,
    0x34, 0x2e, 0x34};
query(cl, r4);
std::vector<std::uint8_t> r5 = {0x04, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x02, 0x00, 0x0b, 0x43, 0x4f, 0x4d, 0x50, 0x52, 0x45, 0x53, 0x53, 0x49,
    0x4f, 0x4e, 0x00, 0x06, 0x73, 0x6e, 0x61, 0x70, 0x70, 0x79, 0x00, 0x0b, 0x43, 0x51, 0x4c, 0x5f, 0x56, 0x45, 0x52, 0x53, 0x49, 0x4f, 0x4e, 0x00, 0x05, 0x33, 0x2e,
    0x34, 0x2e, 0x34};
query(cl, r5);
std::vector<std::uint8_t> r6 = {0x04, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x02, 0x00, 0x0b, 0x43, 0x4f, 0x4d, 0x50, 0x52, 0x45, 0x53, 0x53, 0x49,
    0x4f, 0x4e, 0x00, 0x06, 0x73, 0x6e, 0x61, 0x70, 0x70, 0x79, 0x00, 0x0b, 0x43, 0x51, 0x4c, 0x5f, 0x56, 0x45, 0x52, 0x53, 0x49, 0x4f, 0x4e, 0x00, 0x05, 0x33, 0x2e,
    0x34, 0x2e, 0x34};
query(cl, r6);

std::vector<std::uint8_t> r7 = {0x04, 0x00, 0x00, 0x02, 0x0f, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x14, 0x00, 0x63, 0x61, 0x73, 0x73, 0x61, 0x6e, 0x64, 0x72,
    0x61, 0x00, 0x63, 0x61, 0x73, 0x73, 0x61, 0x6e, 0x64, 0x72, 0x61};
query(cl, r7);
std::vector<std::uint8_t> r8 = {0x04, 0x00, 0x00, 0x02, 0x0f, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x14, 0x00, 0x63, 0x61, 0x73, 0x73, 0x61, 0x6e, 0x64, 0x72,
    0x61, 0x00, 0x63, 0x61, 0x73, 0x73, 0x61, 0x6e, 0x64, 0x72, 0x61};
query(cl, r8);
std::vector<std::uint8_t> r9 = {0x04, 0x00, 0x00, 0x02, 0x0f, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x14, 0x00, 0x63, 0x61, 0x73, 0x73, 0x61, 0x6e, 0x64, 0x72,
    0x61, 0x00, 0x63, 0x61, 0x73, 0x73, 0x61, 0x6e, 0x64, 0x72, 0x61}; 
query(cl, r9);

std::vector<std::uint8_t> r10 = {0x04, 0x01, 0x00, 0x03, 0x0b, 0x00, 0x00, 0x00, 0x2c, 0x31, 0x68, 0x00, 0x03, 0x00, 0x0f, 0x54, 0x4f, 0x50, 0x4f, 0x4c, 0x4f, 0x47,
    0x59, 0x5f, 0x43, 0x48, 0x41, 0x4e, 0x47, 0x45, 0x00, 0x0d, 0x53, 0x54, 0x41, 0x54, 0x55, 0x53, 0x19, 0x0f, 0x2c, 0x43, 0x48, 0x45, 0x4d, 0x41, 0x5f, 0x43,
    0x48, 0x41, 0x4e, 0x47, 0x45};
query(cl, r10);
std::vector<std::uint8_t> r11 = {0x04, 0x01, 0x00, 0x03, 0x0b, 0x00, 0x00, 0x00, 0x2c, 0x31, 0x68, 0x00, 0x03, 0x00, 0x0f, 0x54, 0x4f, 0x50, 0x4f, 0x4c, 0x4f, 0x47,
    0x59, 0x5f, 0x43, 0x48, 0x41, 0x4e, 0x47, 0x45, 0x00, 0x0d, 0x53, 0x54, 0x41, 0x54, 0x55, 0x53, 0x19, 0x0f, 0x2c, 0x43, 0x48, 0x45, 0x4d, 0x41, 0x5f, 0x43,
    0x48, 0x41, 0x4e, 0x47, 0x45};
query(cl, r11);
std::vector<std::uint8_t> r12 = {0x04, 0x01, 0x00, 0x03, 0x0b, 0x00, 0x00, 0x00, 0x2c, 0x31, 0x68, 0x00, 0x03, 0x00, 0x0f, 0x54, 0x4f, 0x50, 0x4f, 0x4c, 0x4f, 0x47,
    0x59, 0x5f, 0x43, 0x48, 0x41, 0x4e, 0x47, 0x45, 0x00, 0x0d, 0x53, 0x54, 0x41, 0x54, 0x55, 0x53, 0x19, 0x0f, 0x2c, 0x43, 0x48, 0x45, 0x4d, 0x41, 0x5f, 0x43,
    0x48, 0x41, 0x4e, 0x47, 0x45};
query(cl, r12);

std::vector<std::uint8_t> r13 = {0x04, 0x01, 0x00, 0x04, 0x07, 0x00, 0x00, 0x00, 0x23, 0x21, 0x80, 0x00, 0x00, 0x00, 0x1a, 0x53, 0x45, 0x4c, 0x45, 0x43, 0x54, 0x20, 0x2a,
    0x20, 0x46, 0x52, 0x4f, 0x4d, 0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6d, 0x2e, 0x70, 0x65, 0x65, 0x72, 0x73, 0x00, 0x01, 0x00};
query(cl, r13);
std::vector<std::uint8_t> r14 = {0x04, 0x01, 0x00, 0x04, 0x07, 0x00, 0x00, 0x00, 0x23, 0x21, 0x80, 0x00, 0x00, 0x00, 0x1a, 0x53, 0x45, 0x4c, 0x45, 0x43, 0x54, 0x20, 0x2a,
    0x20, 0x46, 0x52, 0x4f, 0x4d, 0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6d, 0x2e, 0x70, 0x65, 0x65, 0x72, 0x73, 0x00, 0x01, 0x00};
query(cl, r14);
std::vector<std::uint8_t> r15 = {0x04, 0x01, 0x00, 0x04, 0x07, 0x00, 0x00, 0x00, 0x23, 0x21, 0x80, 0x00, 0x00, 0x00, 0x1a, 0x53, 0x45, 0x4c, 0x45, 0x43, 0x54, 0x20, 0x2a,
    0x20, 0x46, 0x52, 0x4f, 0x4d, 0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6d, 0x2e, 0x70, 0x65, 0x65, 0x72, 0x73, 0x00, 0x01, 0x00};
query(cl, r15);

std::vector<std::uint8_t> r16 = {0x04, 0x01, 0x00, 0x05, 0x07, 0x00, 0x00, 0x00, 0x35, 0x33, 0xc8, 0x00, 0x00, 0x00, 0x2c, 0x53, 0x45, 0x4c, 0x45, 0x43, 0x54,
    0x20, 0x2a, 0x20, 0x46, 0x52, 0x4f, 0x4d, 0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6d, 0x2e, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x20, 0x57, 0x48, 0x45, 0x52, 0x45, 0x20,
    0x6b, 0x65, 0x79, 0x3d, 0x27, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x27, 0x00, 0x01, 0x00};
query(cl, r16);
std::vector<std::uint8_t> r17 = {0x04, 0x01, 0x00, 0x05, 0x07, 0x00, 0x00, 0x00, 0x35, 0x33, 0xc8, 0x00, 0x00, 0x00, 0x2c, 0x53, 0x45, 0x4c, 0x45, 0x43, 0x54,
    0x20, 0x2a, 0x20, 0x46, 0x52, 0x4f, 0x4d, 0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6d, 0x2e, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x20, 0x57, 0x48, 0x45, 0x52, 0x45, 0x20,
    0x6b, 0x65, 0x79, 0x3d, 0x27, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x27, 0x00, 0x01, 0x00};
query(cl, r17);
std::vector<std::uint8_t> r18 = {0x04, 0x01, 0x00, 0x05, 0x07, 0x00, 0x00, 0x00, 0x35, 0x33, 0xc8, 0x00, 0x00, 0x00, 0x2c, 0x53, 0x45, 0x4c, 0x45, 0x43, 0x54,
    0x20, 0x2a, 0x20, 0x46, 0x52, 0x4f, 0x4d, 0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6d, 0x2e, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x20, 0x57, 0x48, 0x45, 0x52, 0x45, 0x20,
    0x6b, 0x65, 0x79, 0x3d, 0x27, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x27, 0x00, 0x01, 0x00};
query(cl, r18);

std::vector<std::uint8_t> r19 = {0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00};
query(cl, r19);
std::vector<std::uint8_t> r20 = {0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00};
query(cl, r20);

std::vector<std::uint8_t> r21 = {0x04, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x02b, 0x00, 0x02, 0x00, 0x0b, 0x43, 0x4f, 0x4d, 0x50, 0x52, 0x45, 0x53,
    0x53, 0x49, 0x4f, 0x4e, 0x00, 0x06, 0x73, 0x6e, 0x61, 0x70, 0x70, 0x79, 0x00, 0x0b, 0x43, 0x51, 0x4c, 0x5f, 0x56, 0x45, 0x52, 0x53, 0x49, 0x4f, 0x4e,
    0x00, 0x05, 0x33, 0x2e, 0x34, 0x2e, 0x34};
query(cl, r21);
std::vector<std::uint8_t> r22 = {0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00};
query(cl, r22);

std::vector<std::uint8_t> r23 = {0x04, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x02b, 0x00, 0x02, 0x00, 0x0b, 0x43, 0x4f, 0x4d, 0x50, 0x52, 0x45, 0x53,
    0x53, 0x49, 0x4f, 0x4e, 0x00, 0x06, 0x73, 0x6e, 0x61, 0x70, 0x70, 0x79, 0x00, 0x0b, 0x43, 0x51, 0x4c, 0x5f, 0x56, 0x45, 0x52, 0x53, 0x49, 0x4f, 0x4e,
    0x00, 0x05, 0x33, 0x2e, 0x34, 0x2e, 0x34};
query(cl, r23);
std::vector<std::uint8_t> r24 = {0x04, 0x00, 0x00, 0x02, 0x0f, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x14, 0x00, 0x63, 0x61, 0x73, 0x73, 0x61, 0x6e,
    0x64, 0x72, 0x61, 0x00, 0x63, 0x61, 0x73, 0x73, 0x61, 0x6e, 0x64, 0x72, 0x61};
query(cl, r24);

std::vector<std::uint8_t> r25 = {0x04, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x02b, 0x00, 0x02, 0x00, 0x0b, 0x43, 0x4f, 0x4d, 0x50, 0x52, 0x45, 0x53,
    0x53, 0x49, 0x4f, 0x4e, 0x00, 0x06, 0x73, 0x6e, 0x61, 0x70, 0x70, 0x79, 0x00, 0x0b, 0x43, 0x51, 0x4c, 0x5f, 0x56, 0x45, 0x52, 0x53, 0x49, 0x4f, 0x4e,
    0x00, 0x05, 0x33, 0x2e, 0x34, 0x2e, 0x34};
query(cl, r25);

std::vector<std::uint8_t> r26 = {0x04, 0x00, 0x00, 0x02, 0x0f, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x14, 0x00, 0x63, 0x61, 0x73, 0x73, 0x61, 0x6e,
    0x64, 0x72, 0x61, 0x00, 0x63, 0x61, 0x73, 0x73, 0x61, 0x6e, 0x64, 0x72, 0x61};
query(cl, r26);
std::vector<std::uint8_t> r27 = {0x04, 0x00, 0x00, 0x02, 0x0f, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x14, 0x00, 0x63, 0x61, 0x73, 0x73, 0x61, 0x6e,
    0x64, 0x72, 0x61, 0x00, 0x63, 0x61, 0x73, 0x73, 0x61, 0x6e, 0x64, 0x72, 0x61};
query(cl, r27);

std::vector<std::uint8_t> r28 = {0x04, 0x01, 0x000, 0x03, 0x07, 0x00, 0x00, 0x00, 0x88, 0x87, 0x01, 0xf0, 0x61, 0x00, 0x00, 0x00, 0x74, 0x43, 0x52, 0x45,
    0x41, 0x54, 0x45, 0x20, 0x4b, 0x45, 0x59, 0x53, 0x50, 0x41, 0x43, 0x45, 0x20, 0x69, 0x66, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74,
    0x73, 0x20, 0x6d, 0x65, 0x63, 0x68, 0x6d, 0x61, 0x73, 0x74, 0x65, 0x72, 0x20, 0x57, 0x49, 0x54, 0x48, 0x20, 0x72, 0x65, 0x70, 0x6c, 0x69, 0x63, 0x61,
    0x74, 0x69, 0x6f, 0x6e, 0x20, 0x3d, 0x20, 0x7b, 0x20, 0x27, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x27, 0x3a, 0x20, 0x27, 0x53, 0x69, 0x6d, 0x70, 0x6c, 0x65,
    0x53, 0x74, 0x72, 0x61, 0x74, 0x65, 0x67, 0x79, 0x27, 0x2c, 0x27, 0x72, 0x65, 0x70, 0x6c, 0x69, 0x09, 0x2b, 0x78, 0x5f, 0x66, 0x61, 0x63, 0x74, 0x6f,
    0x72, 0x27, 0x3a, 0x20, 0x27, 0x33, 0x27, 0x20, 0x7d, 0x3b, 0x00, 0x0a, 0x24, 0x00, 0x00, 0x03, 0xe8, 0x00, 0x05, 0x8c, 0xea, 0x5c, 0x2d, 0xb6, 0x88};
query(cl, r28);

std::vector<std::uint8_t> r29 = {0x04, 0x01, 0x000, 0x03, 0x07, 0x00, 0x00, 0x00, 0x88, 0x87, 0x01, 0xf0, 0x61, 0x00, 0x00, 0x00, 0x74, 0x43, 0x52, 0x45,
    0x41, 0x54, 0x45, 0x20, 0x4b, 0x45, 0x59, 0x53, 0x50, 0x41, 0x43, 0x45, 0x20, 0x69, 0x66, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74,
    0x73, 0x20, 0x6d, 0x65, 0x63, 0x68, 0x6d, 0x61, 0x73, 0x74, 0x65, 0x72, 0x20, 0x57, 0x49, 0x54, 0x48, 0x20, 0x72, 0x65, 0x70, 0x6c, 0x69, 0x63, 0x61,
    0x74, 0x69, 0x6f, 0x6e, 0x20, 0x3d, 0x20, 0x7b, 0x20, 0x27, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x27, 0x3a, 0x20, 0x27, 0x53, 0x69, 0x6d, 0x70, 0x6c, 0x65,
    0x53, 0x74, 0x72, 0x61, 0x74, 0x65, 0x67, 0x79, 0x27, 0x2c, 0x27, 0x72, 0x65, 0x70, 0x6c, 0x69, 0x09, 0x2b, 0x78, 0x5f, 0x66, 0x61, 0x63, 0x74, 0x6f,
    0x72, 0x27, 0x3a, 0x20, 0x27, 0x33, 0x27, 0x20, 0x7d, 0x3b, 0x00, 0x0a, 0x24, 0x00, 0x00, 0x03, 0xe8, 0x00, 0x05, 0x8c, 0xea, 0x5c, 0x2d, 0xb6, 0x88};
query(cl, r29);

std::vector<std::uint8_t> r30 = {0x04, 0x01, 0x00, 0x04, 0x07, 0x00, 0x00, 0x00, 0x74, 0x71, 0xf0, 0x70, 0x00, 0x00, 0x00, 0x5e, 0x63, 0x72, 0x65, 0x61,
    0x74, 0x65, 0x20, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x69, 0x66, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74, 0x73, 0x20, 0x6d, 0x65,
    0x63, 0x68, 0x6d, 0x61, 0x73, 0x74, 0x65, 0x72, 0x2e, 0x74, 0x65, 0x73, 0x74, 0x5f, 0x6d, 0x61, 0x70, 0x31, 0x20, 0x28, 0x69, 0x64, 0x20, 0x69, 0x6e,
    0x74, 0x20, 0x70, 0x72, 0x69, 0x6d, 0x61, 0x72, 0x79, 0x20, 0x6b, 0x65, 0x79, 0x2c, 0x20, 0x63, 0x31, 0x20, 0x6d, 0x61, 0x70, 0x3c, 0x76, 0x61, 0x72,
    0x63, 0x68, 0x61, 0x72, 0x2c, 0x76, 0x61, 0x72, 0x63, 0x68, 0x61, 0x72, 0x3e, 0x29, 0x3b, 0x00, 0x0a, 0x24, 0x00, 0x00, 0x03, 0xe8, 0x00, 0x05, 0x8c,
    0xea, 0x5c, 0x2e, 0x3d, 0x33};
query(cl, r30);

std::vector<std::uint8_t> r31 = {0x04, 0x01, 0x000, 0x03, 0x07, 0x00, 0x00, 0x00, 0x88, 0x87, 0x01, 0xf0, 0x61, 0x00, 0x00, 0x00, 0x74, 0x43, 0x52, 0x45,
    0x41, 0x54, 0x45, 0x20, 0x4b, 0x45, 0x59, 0x53, 0x50, 0x41, 0x43, 0x45, 0x20, 0x69, 0x66, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74,
    0x73, 0x20, 0x6d, 0x65, 0x63, 0x68, 0x6d, 0x61, 0x73, 0x74, 0x65, 0x72, 0x20, 0x57, 0x49, 0x54, 0x48, 0x20, 0x72, 0x65, 0x70, 0x6c, 0x69, 0x63, 0x61,
    0x74, 0x69, 0x6f, 0x6e, 0x20, 0x3d, 0x20, 0x7b, 0x20, 0x27, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x27, 0x3a, 0x20, 0x27, 0x53, 0x69, 0x6d, 0x70, 0x6c, 0x65,
    0x53, 0x74, 0x72, 0x61, 0x74, 0x65, 0x67, 0x79, 0x27, 0x2c, 0x27, 0x72, 0x65, 0x70, 0x6c, 0x69, 0x09, 0x2b, 0x78, 0x5f, 0x66, 0x61, 0x63, 0x74, 0x6f,
    0x72, 0x27, 0x3a, 0x20, 0x27, 0x33, 0x27, 0x20, 0x7d, 0x3b, 0x00, 0x0a, 0x24, 0x00, 0x00, 0x03, 0xe8, 0x00, 0x05, 0x8c, 0xea, 0x5c, 0x2d, 0xb6, 0x88};
query(cl, r31);

std::vector<std::uint8_t> r32 = {0x04, 0x01, 0x00, 0x04, 0x07, 0x00, 0x00, 0x00, 0x74, 0x71, 0xf0, 0x70, 0x00, 0x00, 0x00, 0x5e, 0x63, 0x72, 0x65, 0x61,
    0x74, 0x65, 0x20, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x69, 0x66, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74, 0x73, 0x20, 0x6d, 0x65,
    0x63, 0x68, 0x6d, 0x61, 0x73, 0x74, 0x65, 0x72, 0x2e, 0x74, 0x65, 0x73, 0x74, 0x5f, 0x6d, 0x61, 0x70, 0x31, 0x20, 0x28, 0x69, 0x64, 0x20, 0x69, 0x6e,
    0x74, 0x20, 0x70, 0x72, 0x69, 0x6d, 0x61, 0x72, 0x79, 0x20, 0x6b, 0x65, 0x79, 0x2c, 0x20, 0x63, 0x31, 0x20, 0x6d, 0x61, 0x70, 0x3c, 0x76, 0x61, 0x72,
    0x63, 0x68, 0x61, 0x72, 0x2c, 0x76, 0x61, 0x72, 0x63, 0x68, 0x61, 0x72, 0x3e, 0x29, 0x3b, 0x00, 0x0a, 0x24, 0x00, 0x00, 0x03, 0xe8, 0x00, 0x05, 0x8c,
    0xea, 0x5c, 0x2e, 0x3d, 0x33};
query(cl, r32);
    
std::vector<std::uint8_t> r33 = {0x04, 0x01, 0x00, 0x04, 0x07, 0x00, 0x00, 0x00, 0x74, 0x71, 0xf0, 0x70, 0x00, 0x00, 0x00, 0x5e, 0x63, 0x72, 0x65, 0x61,
    0x74, 0x65, 0x20, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x69, 0x66, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74, 0x73, 0x20, 0x6d, 0x65,
    0x63, 0x68, 0x6d, 0x61, 0x73, 0x74, 0x65, 0x72, 0x2e, 0x74, 0x65, 0x73, 0x74, 0x5f, 0x6d, 0x61, 0x70, 0x31, 0x20, 0x28, 0x69, 0x64, 0x20, 0x69, 0x6e,
    0x74, 0x20, 0x70, 0x72, 0x69, 0x6d, 0x61, 0x72, 0x79, 0x20, 0x6b, 0x65, 0x79, 0x2c, 0x20, 0x63, 0x31, 0x20, 0x6d, 0x61, 0x70, 0x3c, 0x76, 0x61, 0x72,
    0x63, 0x68, 0x61, 0x72, 0x2c, 0x76, 0x61, 0x72, 0x63, 0x68, 0x61, 0x72, 0x3e, 0x29, 0x3b, 0x00, 0x0a, 0x24, 0x00, 0x00, 0x03, 0xe8, 0x00, 0x05, 0x8c,
    0xea, 0x5c, 0x2e, 0x3d, 0x33};
query(cl, r33);
}

int main(int argc, char **argv)
{
    std::cout << "Cassandra test compression. Version 1.0" << std::endl;
    std::cout << "For use:" << std::endl;
    std::cout << "httpservicex [server_ip [server_port]] compression_type" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "1) httpservicex none" << std::endl;
    std::cout << "2) httpservicex 127.0.0.1 none" << std::endl;
    std::cout << "3) httpservicex 127.0.0.1 9042 snappy" << std::endl;
    std::cout << "Parameter \"compression_type\" may be:" << std::endl;
    std::cout << "none" << std::endl;
    std::cout << "lz4" << std::endl;
    std::cout << "snappy" << std::endl;

  std::string serverIP = "127.0.0.1";
  std::string port = "9042";
  std::string compressionType = "none";

  if (argc == 3)
  {
    serverIP = std::string(argv[1]);
    compressionType = std::string(argv[2]);
  }
  else if (argc == 4)
  {
    serverIP = std::string(argv[1]);
    port = std::string(argv[2]);
    compressionType = std::string(argv[3]);      
  }

  Client cl;
  cl.start(serverIP, port);

  if (argc == 2)
  {
    compressionType = std::string(argv[1]);
  }

  if (compressionType == "none")
  {
    non_compression_test(cl);
  }
  else if (compressionType == "lz4")
  {
    lz4_compression_test(cl);
  }
  else if (compressionType == "snappy")
  {
    snappy_compression_test(cl);
  }
  else
  {
    std::cout << "Fatal error: unknown compression_type!" << std::endl;  
  }
  //compression_stress_test(cl);

  cl.stop();

  return 0;
}

void viewPacketInHex(const std::vector<std::uint8_t>& responce)
{
    std::cout << std::endl;
    for (unsigned int i = 0; i < responce.size(); ++i)
    {
        std::cout << "0x" << std::hex << static_cast<unsigned short int>(responce[i]) << " ";
    }
    std::cout << std::endl;
}

void sendStartUseCompression(Client& cl, const std::string& algorithm = std::string())
{
    std::vector<std::uint8_t> request;

    request = {0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x16, 0x00, 0x02};

    std::string cql_version = "CQL_VERSION";
    std::string version = "3.0.0";

    request.push_back(cql_version.size() << 8);
    request.push_back(cql_version.size());

    for (unsigned int i = 0; i < cql_version.size(); ++i)
    {
        request.push_back(cql_version[i]);
    }

    request.push_back(version.size() << 8);
    request.push_back(version.size());

    for (unsigned int i = 0; i < version.size(); ++i)
    {
        request.push_back(version[i]);
    }

    int body_size = 2;
    body_size += 2 + cql_version.size();
    body_size += 2 + version.size();

    std::string compression = "COMPRESSION";

    request.push_back(compression.size() << 8);
    request.push_back(compression.size());

    for (unsigned int i = 0; i < compression.size(); ++i)
    {
        request.push_back(compression[i]);
    }

    request.push_back(algorithm.size() << 8);
    request.push_back(algorithm.size());

    for (unsigned int i = 0; i < algorithm.size(); ++i)
    {
        request.push_back(algorithm[i]);
    }

    body_size += 2 + compression.size();
    body_size += 2 + algorithm.size();

    request[8] = body_size;
    request[7] = body_size << 8;
    request[6] = body_size << 16;
    request[5] = body_size << 24;

    cl.sendMessage(request);

    while (true)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));

        if (cl.hasResponce())
        {
            break;
        }
    }

    std::vector<std::uint8_t> responce = cl.getResponce();
    viewPacketInHex(responce);
}
