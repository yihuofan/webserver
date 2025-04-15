#include "Epoll.h"
#include "util.h"
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 1000

Epoll::Epoll() : epfd(-1), events(nullptr)
{
    epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");
    events = new epoll_event[MAX_EVENTS];
    bzero(events, sizeof(*events) * MAX_EVENTS);
}

Epoll::~Epoll()
{
    if (epfd != -1)
    {
        close(epfd);
        epfd = -1;
    }
    delete[] events;
}

void Epoll::addFd(int fd, uint32_t op)
{
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event error");
}

std::vector<epoll_event> Epoll::poll(int timeout)
{
    std::vector<epoll_event> activeEvents;
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    for (int i = 0; i < nfds; ++i)
    {
        activeEvents.push_back(events[i]);
    }
    return activeEvents;
}
//为什么不直接返回 events？

//    1. events 是一个固定大小的数组,events 的大小是固定的（MAX_EVENTS），而实际的就绪事件数量（nfds）通常远小于 MAX_EVENTS。如果直接返回整个 events 数组，会包含大量未使用的空间，浪费内存和带宽。 使用 std::vector 可以动态调整大小，只存储实际的就绪事件。
//   2. //events 是类的内部成员变量events 是 Epoll 类的一个成员变量，它的生命周期与 Epoll 对象绑定。如果直接返回 events，会导致以下问题：  数据竞争 ：如果调用者在处理 events 的同时，另一个线程调用了 poll 方法，events 的内容会被覆盖，导致数据不一致。数据暴露 ：直接返回 events 会暴露类的内部实现细节，违反封装原则。  通过复制到 activeEvents，可以避免这些问题，确保调用者获得的是独立的数据副本。
// 3. std::vector 更灵活 std::vector 是 C++ 标准库提供的动态数组容器，提供了更丰富的接口（如迭代器、容量管理等），便于后续处理。调用者可以直接使用 std::vector 提供的功能，而不需要手动管理数组的大小或边界。
// 4. 避免修改原始数据如果直接返回 events，调用者可能会无意中修改其内容，影响下一次 poll 的结果。通过复制到 activeEvents，可以确保 events 的内容始终保持原始状态，避免被意外修改。