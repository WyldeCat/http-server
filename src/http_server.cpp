#include "http_server.hpp"

#include <cstdio>
#include <cstring>
#include <thread>

#include <unistd.h>
#include <signal.h>
#include <pthread.h>

/* -----------http response----------- */

http_response::http_response(http_request *req)
{
  socket = req->get_socket();
}

void http_response::send()
{
  int fd = socket->get_file_descriptor();
  int length = strlen(body);
  //printf("%s\n",body);
  //printf("%d\n",length);
  int decimal_length = 0, tmp = 1;
  while(tmp<=length) { decimal_length++; tmp*=10; }

  const char *status_prefix = "HTTP/1.1 ";
  const char *content_type_prefix = "Content-Type: application/json; charset=utf-8\r\n";
  const char *content_length_prefix = "Content-Length: ";//10\r\n";
  char content_length[decimal_length+1]; for(int i=decimal_length-1;i>=0;i--){ content_length[i] = '0'+length%10; length/=10; }
  content_length[decimal_length]=0;
  const char *connection_prefix = "Connection: close\r\n\r\n";

  write(fd, status_prefix, strlen(status_prefix)); write(fd,status,strlen(status)); write(fd,"\r\n",strlen("\r\n"));
  write(fd, content_type_prefix, strlen(content_type_prefix));
  write(fd, content_length_prefix, strlen(content_length_prefix)); write(fd,content_length,strlen(content_length)); write(fd,"\r\n",strlen("\r\n"));
  write(fd, connection_prefix, strlen(connection_prefix));
  write(fd,body,strlen(body));
}

void http_response::set_status(const char* _status)
{
  status = _status;
}

void http_response::set_body(const char *_body)
{
  body = _body;
}

/* -----------http request----------- */

http_request::http_request() { }
http_request::http_request(tcp_socket* _socket):socket(_socket)
{
  this->set_request(socket);  
}

tcp_socket* http_request::get_socket()
{
  return socket;
}
const char* http_request::get_IP()
{
  return socket->get_IP();
}

int http_request::get_method()
{
  return method;
}

std::vector<std::string>* http_request::get_url()
{
  return &url;
}

void http_request::set_request(tcp_socket* _socket)
{
  socket = _socket;
  url.clear();

  int fd = socket->get_file_descriptor(), readn;
  char buf[1024]={0,};
  char* i,* j;
  char* end_line;

  // TODO : socket read
  readn = read(fd, buf, 1024);

  if(readn<=0)
  {
    method = ERR;
    return;
  }

  if(strlen(buf)!=0) 
  {
    end_line = i = buf; 
    while(*end_line!='\r' && *end_line!=0) end_line++;

    // METHOD

    switch(*i)
    {
      case 'G':
        method = GET;
        break;
      case 'P':
        method = (*(i+1)=='O') ? POST : PUT;
        break;
      case 'D':
        method = DELETE;
        break;
      default:
        method = ERR;
        break;
    }
    while((*i)!=' ') i++;
    i++;

    // URL

    char* st=NULL;

    while(1)
    {
      if((*i)=='/')
      {
        if(st!=NULL)
        {
          url.push_back(std::string(st,i-st));
        }
        st = i+1;
      }
      else if((*i)==' ')
      {
        if(*(i-1)!='/')
        {
          url.push_back(std::string(st,i-st));
        }
        break;
      }
      i++;
    }
    i++;

    // TODO : Update
  }
}

/* -----------http server----------- */

http_server::http_server(int (*t)(_http_request*), const char *ip, unsigned short port, unsigned int _size, int _cnt_threads):handler(t),size(_size),cnt_threads(_cnt_threads)
{
  server_sock = new tcp_socket(ip,port);
  if(cnt_threads>1) server_th = new std::thread*[cnt_threads-1];
}

http_server::~http_server()
{
  delete(server_sock);
}


int http_server::start()
{
  mutexes = new std::mutex[size+50];
  main_handler = new event_handler(size, cnt_threads);  
  main_handler->add(0, event::READ, server_sock);

  for(int i=0;i<cnt_threads-1;i++)
    server_th[i] = new std::thread(&http_server::routine, this, i+1);

  this->routine(0);

  return (main_handler!=NULL) ? 0 : -1;
}

int http_server::stop()
{
  /*
     TODO
     pthread_cancel(server_th->native_handle());
   */

  return 1;
}


void http_server::routine(int thread_idx)
{
  int n;

  tcp_socket client_sock,* tmp;

  http_request req;
  _http_request _req;
  _req.set_pointer(&req);

  event* evnt;

  int server_sockfd = server_sock->get_file_descriptor();
  fprintf(stderr,"server sockfd : %d %d\n",server_sockfd, thread_idx);

  while(1)
  {
    n = main_handler->wait(thread_idx, -1);
    for(int i=0;i<n;i++)
    {
      evnt = main_handler->get_ith_event(thread_idx, i);
      tmp = evnt->get_socket();

      if(server_sockfd == tmp->get_file_descriptor())
      {
        while(1)
        {
          server_sock->accept(&client_sock);
          if(client_sock.get_file_descriptor() == -1) break;
          main_handler->add(thread_idx, event::READ, &client_sock);
        }
      }
      else 
      { 
        mutexes[tmp->get_file_descriptor()].lock();
        while(1)
        { 
          req.set_request(tmp);
          if(req.method == http_request::ERR)
          {
            main_handler->del(evnt);
            tmp->close_socket();
            break;
          }
          handler(&_req);
        }
        mutexes[tmp->get_file_descriptor()].unlock();
      }
    }
  }
}

