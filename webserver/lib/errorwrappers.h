#ifndef ERRORWRAPPERS_H
#define ERRORWRAPPERS_H

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>


ssize_t Read(int fd, void *buf, size_t count);

ssize_t Write(int df, void *buf, size_t count);

void Select(int nfds, fd_set *readfds, fd_set *wiretefds, fd_set *exeptfds, struct timeval* timeout);

int Accept(int fd, struct sockaddr *addr, socklen_t *addrlen);

int Open(const char* path_name, int flags);

#endif