#include "../../include/EventLoop.hpp"
#include <stdio.h> // TODO juste pour enlever l'erreur mais a virer
using namespace webserv;

bool EventLoop::runGetMethod(const http::Request &req) {
  (void)req;
  return printf("runGetMethod ok");
}

bool EventLoop::runDeletMethod(const http::Request &req) {
  (void)req;
  return printf("runDeletMethod ok");
}

bool EventLoop::runPostMethod(const http::Request &req) {
  (void)req;
  return printf("runPostMethod ok");
}
