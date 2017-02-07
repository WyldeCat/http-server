/*

wrapping epoll as event handler

*/

#include "tcp_socket.hpp"

#include <sys/epoll.h>

class event_handler;


class event {

public:
  static const int READ = EPOLLIN;
  static const int WRITE = EPOLLOUT;
  event(int _type, tcp_socket *_socket);

  tcp_socket* get_socket();

private:
  event();
  tcp_socket* socket;
  epoll_event* epoll_ev;
  friend event_handler;
};


class event_handler {

public:
  event_handler(int _size, int _cnt_threads);
  int add(event* ev); 
  int del(event* ev);
  int add(int thread_idx, int type, tcp_socket *socket);
  int del(int thread_idx, tcp_socket *socket);
  int wait(int thread_idx, int timeout);
  event* get_ith_event(int thread_idx, int i);

private:
  int cnt_threads;
  int epoll_fd;
  int size;
  event** evnts;
  epoll_event** _evnts;
  event* evnt;

};
