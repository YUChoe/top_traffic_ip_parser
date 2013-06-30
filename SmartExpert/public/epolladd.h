void Epolladd(int efd, int fd, struct epoll_event *event);

void Epolladd(int efd, int fd, struct epoll_event *event)
{
	struct epoll_event ev;
    //ev.events = EPOLLIN|EPOLLERR|EPOLLHUP;
    ev.events = EPOLLIN|EPOLLERR|EPOLLHUP|EPOLLET;
    ev.data.fd = fd;
    epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev);
}

