
#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include "_http_server.h"
#include "tcp_socket.hpp"
#include "event_handler.hpp"

#include <thread>
#include <mutex>
#include <vector>
#include <string>

class http_server;

class http_response {

public:
  http_response(http_request* req);
  void send();
  void set_status(const char* _status);
  void set_body(const char* _body);
  void set_socket(tcp_socket *_socket);

private:
  tcp_socket* socket;
  const char* status;
  const char* body;

};

class http_request {

public:
  typedef enum __method
  {
      GET,
      POST,
      PUT,
      DELETE,
      ERR
  } _method; 

  http_request();
  http_request(tcp_socket* _socket);

  const char* get_IP();
  int get_method();
  tcp_socket* get_socket();
  std::vector<std::string>* get_url();

private:
  void set_request(tcp_socket* _socket);
  tcp_socket* socket;
  _method method;
  std::vector<std::string> url;
  friend http_server;
};


class http_server {

public:
  http_server(int (*handler)(_http_request*),const char *ip, unsigned short port,unsigned int _size, int _cnt_threads);
  ~http_server();
  int start();
  int stop();

private:
  void routine(int thread_idx);

private:
  int cnt_threads;
  int size;
  std::mutex* mutexes;
  std::thread** server_th;
  tcp_socket* server_sock;
  event_handler* main_handler;
  
  int (*handler)(_http_request*);

};

#endif
