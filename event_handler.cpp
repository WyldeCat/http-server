#include "event_handler.hpp"

#include <stdlib.h>
#include <stdio.h>

event::event(int type, tcp_socket* _socket):socket(_socket)
{ 
  epoll_ev = (epoll_event*)malloc(sizeof(epoll_event));
  epoll_ev->events = type;
  epoll_ev->data.fd = socket->get_file_descriptor();
}
event::event()
{
  epoll_ev = (epoll_event*)malloc(sizeof(epoll_event));
}
tcp_socket* event::get_socket()
{
  return this->socket;
}


event_handler::event_handler(int _size, int _cnt_threads):size(_size), cnt_threads(_cnt_threads)
{
  _evnts = new epoll_event*[cnt_threads];
  evnts = new event*[cnt_threads];
  for(int i=0;i<cnt_threads;i++)
  {
      _evnts[i] = new epoll_event[size];
      evnts[i] = new event[size];
      for(int j=0;j<size;j++) evnts[i][j].socket = new tcp_socket();
  }

  evnt = new event[cnt_threads];
  epoll_fd = epoll_create(_size);
}

int event_handler::add(event* ev)
{
  //fprintf(stderr,"add : %d, %d\n",ev->socket->get_file_descriptor(),ev->epoll_ev->events);
  return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ev->socket->get_file_descriptor(), ev->epoll_ev);
}
int event_handler::add(int thread_idx, int type, tcp_socket *socket)
{
  int state;
  evnt[thread_idx].epoll_ev->events = type | EPOLLET;
  evnt[thread_idx].socket = socket;
  evnt[thread_idx].epoll_ev->data.fd = socket->get_file_descriptor();
  state = event_handler::add(&evnt[thread_idx]);
  return state;
}

int event_handler::del(event* ev)
{
  return epoll_ctl(epoll_fd, EPOLL_CTL_DEL, ev->socket->get_file_descriptor(), NULL);
}
int event_handler::del(int thread_idx, tcp_socket *socket)
{
  int state;
  evnt[thread_idx].socket = socket;
  state = event_handler::del(&evnt[thread_idx]);
  return state;
}


int event_handler::wait(int thread_idx, int timeout)
{
  int state;
  sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  // should remember..

  state = epoll_wait(epoll_fd, _evnts[thread_idx], size, timeout);

  for(int i=0;i<state;i++)
  {
    getpeername(_evnts[thread_idx][i].data.fd, (struct sockaddr*)&client_addr, &client_len);
    evnts[thread_idx][i].socket->set_socket(_evnts[thread_idx][i].data.fd, &client_addr); 
  }

  return state;
}

event* event_handler::get_ith_event(int thread_idx, int i)
{
  return &evnts[thread_idx][i];
}

