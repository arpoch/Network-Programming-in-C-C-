#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <iostream>
using std::cout;
int main()
{
    struct ifaddrs *addresses;
    if (getifaddrs(&addresses) == -1)
    {
        cout << "getifaddrss() call failed\n";
        return -1;
    }
    struct ifaddrs *address = addresses;
    while (address)
    {
        int family = address->ifa_addr->sa_family;
        if (family == AF_INET || family == AF_INET6)
        {
            cout << "Interface Name  = " << address->ifa_name << '\n';
            cout << "Type = " << (family == (AF_INET) ? "IPv4" : "IPv6") << '\n';

            char ap[100];
            const int family_size = (family == (AF_INET) ? sizeof(struct sockaddr_in)
                                                         : sizeof(struct sockaddr_in6));
            getnameinfo(address->ifa_addr, family_size, ap, sizeof(ap), 0, 0,
                        NI_NUMERICHOST);
            cout << "Textual Address = " << ap << '\n';
            cout << "-------------------------------------\n";
        }
        address = address->ifa_next;
    }
    freeifaddrs(addresses);
    return 0;
}