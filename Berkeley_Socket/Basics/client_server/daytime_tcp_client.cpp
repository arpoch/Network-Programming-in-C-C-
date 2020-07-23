#include <iostream>
#include <unistd.h>     /* read()       */
#include <sys/socket.h> /*  socket()    */
#include <netinet/in.h> /*  sockaddr_in */
#include <sys/types.h>  /*  Generic socket struct to hold information about the sockaddr_in */
#include <arpa/inet.h>  /*  inet_pton   */
#include <string.h>     /*  bzero       */
#define SA struct sockaddr
int main(int argc, char **argv)
{
    int socket_fd = 0, n = 0;
    char buff[50];
    char recvline[50];
    struct sockaddr_in servaddr; //Handling Internet Addresses
    bzero(&servaddr, 0);
    //--------------------------Setting-Up the Server---------------------------------------
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    if ((n = inet_pton(AF_INET, argv[1], &servaddr.sin_addr)) <= 0)
    {
        std::cout << "Error with server addressing\n";
    }
    //--------------------------Creating Socket---------------------------------------------
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        std::cout << "Error creating socket\n";
    }
    else
    {
        std::cout << "Socket Binded successfully at "
                  << inet_ntop(AF_INET, &servaddr.sin_addr, buff, sizeof(buff))
                  << ":" << (ntohs)(servaddr.sin_port) << '\n';
    }
    //--------------------------Connecting to server-----------------------------------------
    if ((n = connect(socket_fd, (const SA *)&servaddr, sizeof(servaddr)) < 0))
    {
        std::cout << "Error while connecting = " << n << '\n';
    }
    else
    {
        std::cout << "Connecting established with server \n";
    }
    //--------------------------Getting Data From the Server---------------------------------
    std::cout << "Message :\n";
    while ((n = read(socket_fd, recvline, 50)) > 0)
    {
        std::cout << recvline;
    }
    if (n < 0)
    {
        std::cout << "Error while reading data\n";
    }
    exit(0);
    return 0;
}