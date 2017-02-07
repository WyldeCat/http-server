#ifndef _HTTP_SERVER_
#define _HTTP_SERVER_

#include <string>
#include <vector>

class http_server;
class http_response;
class http_request;

class _http_response;
class _http_request;

class _http_response {

private:
  http_response* response_pointer;
  _http_response(); 

public:
  _http_response(_http_request* req); 
  ~_http_response();
  void send();
  void set_status(const char *status);
  void set_body(const char *body);
};

class _http_request {

private:
  http_request* request_pointer;
  friend _http_response;

public:
  typedef enum __method
  {
      GET,
      POST,
      PUT,
      DELETE,
      ERR
  } _method;

  const char* get_IP();
  void set_pointer(http_request *pointer);
  int get_method();
  std::vector<std::string>* get_url();
};

class _http_server {

private:
  http_server* server_pointer;

public:
  _http_server(int (*t)(_http_request*),const char *ip, unsigned short port, unsigned int size, int cnt_threads);
  ~_http_server();

  int start();
  int stop();

};

#endif

