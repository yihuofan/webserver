#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //创建IPv4,TCP套接字
    struct sockaddr_in serv_addr;
    //sockaddr_in专门用于存储IPv4地址
    bzero(&serv_addr, sizeof(serv_addr));
    //bzero函数将serv_addr的内存清零避免使用未初始化的垃圾值
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);
    //将服务器的 IP 地址设置为 127.0.0.1（本地回环地址）。inet_addr() 将点分十进制的 IP 地址字符串转换为网络字节序的 32 位整数。
    //将服务器的端口号设置为 8888。htons() 将主机字节序转换为网络字节序，以确保在不同平台之间的兼容性。

    bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));
    //将套接字与指定的地址和端口绑定。
    listen(sockfd, SOMAXCONN);
    //监听套接字，等待客户端连接。SOMAXCONN 表示系统允许的最大连接数。
    struct sockaddr_in clnt_addr;
    //用于存储客户端的地址信息。
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    bzero(&clnt_addr, sizeof(clnt_addr));
    //清零客户端地址结构体，避免使用未初始化的垃圾值
    int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);
    //bind只需要传入serv_addr的大小即可，而accept需要写入客户端socket长度，所以需要定义一个类型为socklen_t的变量，并传入这个变量的地址。另外，accept函数会阻塞当前程序，直到有一个客户端socket被接受后程序才会往下运行。
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
    return 0;
}