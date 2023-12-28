#include "errorwrappers.h"


ssize_t Read(int fd, void* buf, size_t count)
{
    ssize_t nb;
    if ((nb = read(fd, buf, count)) < 0)
    {
        perror("read");
    }
    assert(nb >= 0);
    return nb;
}

ssize_t Write(int fd, void *buf, size_t count)
{
    ssize_t nb;
    if ((nb = write(fd, buf, count)) < 0)
    {
        perror("write");
    }
    assert(nb >= 0);
    return nb;
}

void Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exeptfds, struct timeval* timeout)
{
    int rez = select (nfds, readfds, writefds, exeptfds, timeout);

    if (rez < 0)
    {
        perror("select");
    }

    assert(rez >= 0);
}

int Accept(int fd, struct sockaddr *addr, socklen_t *addr_len)
{
    int rezfd;
    rezfd = accept(fd, addr, addr_len);

    if (rezfd < 0)
    {
        perror("accept");
    }

    assert(rezfd >= 0);
    return rezfd;
}

int Open(const char *path_name, int flags)
{
    int rezfd;
    rezfd = open(path_name, flags);

    if(rezfd < 0)
    {
        perror("open");
    }

    assert(rezfd >= 0);
    return rezfd;
}



