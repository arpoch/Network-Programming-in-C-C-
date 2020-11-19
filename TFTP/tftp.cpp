#include <iostream>
#include <fstream>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <cassert>

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
    char data[512]; //MODE
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

//----------------------------Declaration of RRQ/WRQ-------------------------------------------
void Reading_rq(const int sock_fd, struct Request_Packet &REQ,
                struct sockaddr_in &client_tftp, const socklen_t addrlen);

void Write_rq(const int sock_fd, const struct Request_Packet &REQ,
              struct sockaddr_in &client_tftp, socklen_t addrlen);

//----------------------------Definition of ACK Send-------------------------------------------
void ACK_send(int sock_fd, int dblock_n, struct sockaddr_in &client_tftp, const socklen_t addrlen)
{
    int ack_send = 0;
    //Acknowledgement Packet
    ACK.code = htons((uint16_t)OP_Code::ACK);
    ACK.block = htons(dblock_n++);
    ack_send = sendto(sock_fd, &ACK, sizeof(ACK), 0, (SA *)&client_tftp, addrlen);
    assert((ack_send < 0, "Acknowledgement not send"));
    std::cout << "Acknowledgement of data packect " << ntohs(ACK.block) << " send\n";
}
//----------------------------Definition of ACK Recv-------------------------------------------
void ACK_recv(int sock_fd)
{
    int ack_rev = 0;
    ack_rev = recvfrom(sock_fd, &ACK, sizeof(ACK), 0, NULL, NULL);
    assert((ack_rev < 0, "Acknowledgement not recived"));
    std::cout << "Acknowledgement of data packect " << ntohs(ACK.block) << " recived\n";
}
//---------------------------------------MAIN--------------------------------------------------
int main(int argc, char **argv)
{
    //Definitions & Declarations
    uint16_t port = 0;
    int sock_fd = 0;
    struct sockaddr_in server_tftp, client_tftp;
    //Initializing
    bzero(&server_tftp, sizeof(server_tftp));
    bzero(&client_tftp, sizeof(client_tftp));
    //Creating Socket
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    assert((sock_fd == -1, "Error while creating socket"));
    //Address Struct
    sscanf(argv[1], "%hu", &port);
    //std::cout << sock_fd << '\n';
    server_tftp.sin_family = AF_INET;
    server_tftp.sin_addr.s_addr = htonl(INADDR_ANY);
    server_tftp.sin_port = htons(port);
    //Binding the Address Struct with Socket
    if (bind(sock_fd, (SA *)&server_tftp, sizeof(server_tftp)) < 0)
    {
        std::cout << "Error while Binding with socket id = " << -1 << '\n';
        exit(1);
    }
    //Listening for client with readfrom
    std::cout << "Listening for client....\n";
    socklen_t addrlen = sizeof(client_tftp);
    int bytes_read = 0;
    bytes_read = recvfrom(sock_fd, &REQ, sizeof(REQ), 0, (SA *)&client_tftp, &addrlen);
    //Reciving Client Data, it should read min 4 bytes of data
    assert((bytes_read == -1, "Error while reciving datagram"));
    //Proccessing Client PORT
    uint16_t hop_code = ntohs(REQ.code);
    //Request By Client
    if (hop_code == (uint16_t)OP_Code::RRQ)
    {
        char addrr_buff[16];
        printf("Reading Request Packet from client at address = %s : %u\n",
               inet_ntop(AF_INET, &client_tftp.sin_addr.s_addr, addrr_buff, sizeof(addrr_buff)),
               ntohs(client_tftp.sin_port));
        //Sending Reading Packet
        Reading_rq(sock_fd, REQ, client_tftp, addrlen);
    }
    else if (hop_code == (uint16_t)OP_Code::WRQ)
    {
        char addrr_buff[16];
        printf("Write Request Packet from client at address = %s : %u\n",
               inet_ntop(AF_INET, &client_tftp.sin_addr.s_addr, addrr_buff, sizeof(addrr_buff)),
               ntohs(client_tftp.sin_port));
        //ACK Send
        ACK_send(sock_fd, 0, client_tftp, addrlen);
        //Sending Write Packet
        Write_rq(sock_fd, REQ, client_tftp, addrlen);
    }
    else
    {
        printf("Undefined OPCODE \n");
        exit(1);
    }
    return 0;
}
//------------------------------------END MAIN----------------------------------------------------

void Reading_rq(const int sock_fd, struct Request_Packet &REQ,
                struct sockaddr_in &client_tftp, const socklen_t addrlen)
{
    std::cout << "File requested by the client " << REQ.filename << "\nMode of transfer " << REQ.mode << '\n';
    int dblock_n = 0;
    //Opening File
    std::string mode, filename;
    mode = REQ.mode;
    filename = REQ.filename;
    std::ifstream inFile;
    //Mode
    if (mode == "octet")
        inFile.open(filename, std::ios::binary);
    else if (mode == "netascii")
    {
        inFile.open(filename);
    }
    //Data Transfering
    std::string slength = "512";
    long int nlength = std::stol(slength);
    char Reader_buff[512];
    if (mode == "octet")
    {
        int bytes_written = 0, ack_rev = 0;
        while (nlength == 512)
        {
            inFile.read(Reader_buff, nlength);
            slength = std::to_string(inFile.gcount());
            nlength = std::stol(slength);
            inFile.seekg(inFile.tellg());
            //Preparing DATA Packet to send to client
            DATA.code = htons(3);
            DATA.block = htons(++dblock_n);
            std::cout << "Number of bytes read  = " << nlength << '\n';
            if (nlength >= 0)
                memcpy((DATA.data), Reader_buff, nlength); //Error can occur due to conversion on unsigned long int into size_t
            //Sending DATA Packet
            bytes_written = sendto(sock_fd, &DATA, (4 + nlength), 0,
                                   (SA *)&client_tftp, addrlen);
            assert((bytes_written < 0, "Error while sending data to client"));
            std::cout << "Data Packect Send\n";
            //Recive ACK
            ACK_recv(sock_fd);
        }
    }
    else if (mode == "netascii")
    {
        int bytes_written = 512, ack_rev = 0;
        while ((inFile.getline(Reader_buff, nlength, '\n')))
        {
            //inFile.get(Reader_buff, nlength);
            slength = std::to_string(inFile.gcount());
            nlength = std::stol(slength);
            //Preparing DATA Packet to send to client
            DATA.code = htons(3);
            DATA.block = htons(++dblock_n);
            std::cout << "Number of bytes read  = " << nlength << '\n';
            if (nlength >= 0)
                strcpy(DATA.data, Reader_buff);
            bytes_written = sendto(sock_fd, &DATA, (4 + nlength), 0,
                                   (SA *)&client_tftp, addrlen);
            assert((bytes_written < 0, "Error while sending data to client"));
            std::cout << "Data Packect Send\n";
            //Recive ACK
            ACK_recv(sock_fd);
        }
    }
}

void Write_rq(const int sock_fd, const struct Request_Packet &REQ,
              struct sockaddr_in &client_tftp, socklen_t addrlen)
{
    std::cout << "File requested by the client " << REQ.filename << "\nMode of transfer " << REQ.mode << '\n';
    //Opening File
    std::string mode, filename;
    mode = REQ.mode;
    filename = REQ.filename;
    std::ofstream inFile;
    inFile.open(filename, std::ios::ate | std::ios::binary);
    //Data Transfering
    char Reader_buff[512];
    int bytes_written = 516;
    while (bytes_written == 516)
    {
        if (mode == "octet")
        {
            //Reciving DATA Packet
            bytes_written = recvfrom(sock_fd, &DATA, sizeof(DATA), 0, (SA *)&client_tftp, &addrlen);
            assert((bytes_written < 0, "Error while reciving data to client"));
            std::cout << "Data Packect Recived\n";
            //Data from DATA packet
            memcpy(Reader_buff, DATA.data, (bytes_written - 4));
            //Writing Data in file
            inFile.write(Reader_buff, (bytes_written - 4));
            //Send ACK
            ACK_send(sock_fd, ntohs(DATA.block), client_tftp, addrlen);
        }
    }
}