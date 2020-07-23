#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include <time.h>

#define SA struct sockaddr
int main(int argc, char **argv)
{
    int socket_fd = 0, connect_fd = 0, n;
    struct sockaddr_in servaddr, clientaddr;
    char buff[50];
    bzero(&servaddr, 0);
    bzero(&clientaddr, 0);
    //--------------------------------Server Settings-------------------------------
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //--------------------------------Creating Socket-------------------------------
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        std::cout << "Error while creating socket\n";
    }
    else
    {
        std::cout << "Socket Created Successfully\n";
    }
    //-------------------------------Binding Socket----------------------------------
    if ((n = bind(socket_fd, (const SA *)&servaddr, (sizeof(servaddr)))) < 0)
    {
        std::cout << "Error binding the socket = " << n << '\n';
    }
    else
    {
        std::cout << "Socket Binded successfully on port = " << (ntohs)(servaddr.sin_port) << " , " << servaddr.sin_addr.s_addr << '\n';
    }
    //-------------------------------Listening to the Request------------------------
    if ((listen(socket_fd, 5)) < 0)
    {
        std::cout << "Error Listening socket\n";
    }
    else
    {
        std::cout << "Listening for Requests.....\n";
    }
    //-------------------------------Accepting the Request----------------------------
    connect_fd = accept(socket_fd, (SA *)&clientaddr, (socklen_t *)(&clientaddr));
    printf("%u\n", clientaddr.sin_port);
    if (connect_fd < 0)
    {
        std::cout << "Error accepting request\n";
    }
    else
    {
        std::cout << "Request Accepted\n";
    }
    //-------------------------------Response-----------------------------------------
    write(connect_fd, "Hello World\n", sizeof("Hello World\n"));
    std::cout << "Message send to client on socket = "
              << (inet_ntop(AF_INET, &clientaddr.sin_addr, buff, sizeof(buff)))
              << ":" << (ntohs)(clientaddr.sin_port) << '\n';
    close(connect_fd);
    return 0;
}