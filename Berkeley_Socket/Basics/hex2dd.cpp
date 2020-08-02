#include <iostream>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    struct in_addr inaddr;
    uint32_t addr;
    char buf[15];
    sscanf(argv[1], "%x", &addr);
    //printf("%x\n", addr);
    inaddr.s_addr = htonl(addr);
    //printf("%x\n", inaddr.s_addr);
    if (!inet_ntop(AF_INET, &inaddr, buf, 15))
    {
        std::cout << "Error\n";
    }
    printf("%s\n", buf);
    return 0;
}