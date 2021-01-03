#include <iostream> //c++ io stream
#include <stdio.h>  //standard input output
#include <stdlib.h>
#include <errno.h>    // define error number
#include <sys/time.h> // for gettimeofday()
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>      //Internet address family
#include <netinet/ip.h>      //Definitions for internet protocol version 4
#include <netinet/ip_icmp.h> //Interface Control Message Protocol Definitions
#include <netinet/if_ether.h>

#include <unistd.h>    //standard symbolic constants and types
#include <signal.h>    //define symbolic constants
#include <arpa/inet.h> //definitions for internet operations
#include <string.h>    //memset and string .etc
#include <netdb.h>     // defines the hostent structure
#include <pthread.h>   //for multithread
#include <list>        //for package store
#include <map>         //map for store something

struct EthIPHeader
{
    unsigned char dstMac[6];
    unsigned char srcMac[6];
    unsigned short ethType;
    unsigned char verIhl;
    unsigned char tos;        //服务类型
    unsigned short totLen;    //总长度
    unsigned short id;        //标志
    unsigned short fragOff;   //分片偏移
    unsigned char ttl;        //生存时间
    unsigned char protocol;   //协议
    unsigned short chkSum;    //检验和
    unsigned char srcAddr[4]; //源IP地址
    unsigned char dstAddr[4]; //目的IP地址
};

typedef struct EthIPHeader EthIPHeader;

class EthIPFrame
{
private:
    EthIPHeader header;
    unsigned char version;
    unsigned char ihl;
    unsigned char fragOpt; //options of fragment
    unsigned short fragOff;
    unsigned char optLen;
    unsigned char *option;
    unsigned short dataLen;
    unsigned char *data;
    unsigned short frameSize;
    unsigned short MTU;
    unsigned short fragLen; //fragment length
    bool isChecked;

public:
    //init while do nothing
    EthIPFrame();
    //usually MTU=1500 and frame obtained without FCS
    EthIPFrame(unsigned char *frame, unsigned short frameSize = 1514);
    ~EthIPFrame();
    //setter and getter
    EthIPHeader getHeader();

    void printBinary(unsigned char *binary, unsigned short length, unsigned short columnSize = 4);
    void printFrame();
    int genFrame(unsigned char *frame, unsigned short frameSize);
};
