#include <iostream>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    struct in_addr inaddr;
    int signal;
    signal = inet_pton(AF_INET, argv[1], &inaddr);
    printf("0x%x\n", ntohl(inaddr.s_addr));
    return 0;
}