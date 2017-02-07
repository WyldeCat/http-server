#include "tcp_socket.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

tcp_socket::tcp_socket( )
{ 
  ip = new char[40];
}

tcp_socket::tcp_socket(const char* _ip,unsigned short _port):ip(_ip),port(_port)
{ 
  int option = 1;
  int flag;

  file_descriptor = socket(PF_INET, SOCK_STREAM, 0);
  setsockopt(file_descriptor, SOL_SOCKET, SO_REUSEADDR, &option, sizeof option);
  flag = fcntl(file_descriptor, F_GETFL, 0);
  fcntl(file_descriptor, F_SETFL, flag | O_NONBLOCK);

  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  inet_pton(AF_INET, ip, &server_addr.sin_addr.s_addr);

  bind(file_descriptor,(struct sockaddr*)&server_addr, sizeof server_addr);
  //perror("bind: ");
  listen(file_descriptor, 5);
  //perror("listen: ");
}

tcp_socket::tcp_socket(int fd, sockaddr_in *addr):file_descriptor(fd)
{
  set_socket(fd, addr);
}

void tcp_socket::set_socket(int fd, sockaddr_in *addr)
{
  file_descriptor = fd;
  strcpy((char*)ip, inet_ntoa(addr->sin_addr));
  port = addr->sin_port;
}

const char* tcp_socket::get_IP()
{
  return ip;
}

tcp_socket::~tcp_socket()
{
}

tcp_socket* tcp_socket::accept()
{
  tcp_socket *sock = new tcp_socket();
  this->accept(sock);
  return sock;
}

void tcp_socket::accept(tcp_socket *socket)
{
  sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);

  socket->file_descriptor = ::accept(file_descriptor, (struct sockaddr*)&client_addr, &client_len);
  //strcpy((char*)socket->ip, inet_ntoa(client_addr.sin_addr));
  socket->port = client_addr.sin_port;
}

int tcp_socket::get_file_descriptor()
{ return file_descriptor;
}

int tcp_socket::close_socket()
{
  close(file_descriptor); 
  return 1;
}

const char* tcp_socket::get_ip()
{
  return ip;
}
