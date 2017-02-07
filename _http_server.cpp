#include "_http_server.h"
#include "http_server.hpp"

#include <thread>
#include <cstdio>
#include <string>
#include <vector>
#include <pthread.h>

/* -----------http response----------- */

_http_response::_http_response(_http_request *req)
{
  response_pointer = new http_response(req->request_pointer);
}
_http_response::~_http_response()
{
  delete response_pointer;
}
void _http_response::send()
{
  response_pointer->send();
}

void _http_response::set_status(const char *status)
{
  response_pointer->set_status(status);
}

void _http_response::set_body(const char *body)
{
  response_pointer->set_body(body);
}

/* -----------http request----------- */

const char* _http_request::get_IP()
{
  return request_pointer->get_IP();
}

void _http_request::set_pointer(http_request *pointer)
{
  request_pointer = pointer;
}

int _http_request::get_method()
{
  return request_pointer->get_method();
}

std::vector<std::string>* _http_request::get_url()
{
  return request_pointer->get_url();
}

/* -----------http server----------- */

_http_server::_http_server(int (*t)(_http_request*), const char *ip, unsigned short port, unsigned int size, int cnt_threads)
{
  server_pointer = new http_server(t, ip, port, size, cnt_threads);
}

_http_server::~_http_server( )
{
  delete server_pointer;
}

int _http_server::start()
{
  server_pointer->start();
}

int _http_server::stop()
{
  server_pointer->stop();
}
