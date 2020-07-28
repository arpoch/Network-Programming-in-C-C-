#include <iostream>
#include <fstream>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#define SA struct sockaddr
enum class OP_Code
{
    RRQ = 1,
    WRQ,
    DATA,
    ACK,
    ERROR
};
struct Request_Packet
{
    uint16_t code;
    char filename[8];
    char mode[8];
} REQ;
struct Data_Packet
{
    uint16_t code;
    uint16_t block;
    char data[512]; //MODE is NETASCII as we have used char array
} DATA;
struct Ack_Packet
{
    uint16_t code;
    uint16_t block;
} ACK;
struct Error_Packet
{
    uint16_t code;
    uint16_t ErrorCode;
    std::string ErrMsg;
} ERROR;

int main(int argc, char **argv)
{
    //------------------------------------Definitions & Declarations----------------------------
    char addrr_buff[16];
    uint16_t port = 0, dblock_n = 0;
    int sock_fd = 0, bind_fd = 0;
    std::string mode, filename;
    struct sockaddr_in server_tftp, client_tftp;
    //-------------------------------------------------------------------------------------------
    bzero(&server_tftp, sizeof(server_tftp));
    bzero(&client_tftp, sizeof(client_tftp));
    //-----------------------------------Creating Socket--------------------------------------
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1)
    {
        std::cout << "Error while creating socket\n";
        exit(1);
    }
    //----------------------------------Address Struct-----------------------------------------
    sscanf(argv[1], "%hu", &port);
    //std::cout << sock_fd << '\n';
    server_tftp.sin_family = AF_INET;
    server_tftp.sin_addr.s_addr = htonl(INADDR_ANY);
    server_tftp.sin_port = htons(port);
    //---------------------------------Binding the Address Struct with Socket------------------
    if ((bind_fd = bind(sock_fd, (SA *)&server_tftp, sizeof(server_tftp))) == -1)
    {
        std::cout << "Error while Binding with socket id = " << bind_fd << '\n';
        exit(1);
    }
    //--------------------------------Listening for client with readfrom------------------------
    std::cout << "READFROM....\n";
    socklen_t addrlen = sizeof(client_tftp);
    int bytes_read = 0;
    bytes_read = recvfrom(sock_fd, &REQ, sizeof(REQ), 0, (sockaddr *)&client_tftp, &addrlen);
    //Reciving Client Data, it should read min 4 bytes of data
    if (bytes_read < 0)
    {
        std::cout << "Error while reciving datagram\n";
        exit(1);
    }
    //--------------------------------Proccessing Requests---------------------------------------
    uint16_t hop_code = ntohs(REQ.code);
    if (hop_code == 1)
    {
        printf("Reading Data request from client address = %s : %u\n",
               inet_ntop(AF_INET, &client_tftp.sin_addr.s_addr, addrr_buff, sizeof(addrr_buff)),
               ntohs(client_tftp.sin_port));
    }
    else
    {
        printf("Undefined OPCODE = \n");
        exit(1);
    }
    std::cout << "File requested by the client " << REQ.filename << "\nMode of transfer " << REQ.mode << '\n';
    //---------------------------------Opening File----------------------------------------------
    mode = REQ.mode;
    filename = REQ.filename;
    std::ifstream inFile;
    inFile.open(filename, std::ios::binary);
    //---------------------------------Data Transfering------------------------------------------
    std::string slength = "512";
    long int nlength = std::stol(slength);
    char Reader_buff[512];
    int bytes_written = 0;
    while (nlength == 512)
    {
        if (mode == "octet")
        {
            inFile.read(Reader_buff, nlength);
            slength = std::to_string(inFile.gcount());
            nlength = std::stol(slength);
            inFile.seekg(inFile.tellg());
            //Preparing DATA Struct to send to client
            DATA.code = htons(3);
            ++dblock_n;
            DATA.block = htons(dblock_n);
            if (nlength > 0)
                memcpy((DATA.data), Reader_buff, nlength); //Error can occur due to conversion on unsigned long int into size_t
            std::cout << "value in buffer = " << DATA.data;
            //Sending DATA Struct
            bytes_written = sendto(sock_fd, &DATA, (4 + nlength), 0,
                                   (struct sockaddr *)&client_tftp, addrlen);
            if (bytes_written < 0)
            {
                std::cout << "Error while sending data to client\n";
                //exit(1);
            }
            std::cout << "Data Packett Send\n";
            //---------------------------------Acknowledgement-------------------------------------------
        }
    }
    return 0;
}
