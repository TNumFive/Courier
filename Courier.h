#include <iostream> //c++ io stream
#include <stdio.h>  //standard input output 
#include <stdlib.h>
#include <errno.h> // define error number
#include <sys/time.h> // for gettimeofday()
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> //Internet address family
#include <netinet/ip.h> //Definitions for internet protocol version 4
#include <netinet/ip_icmp.h> //Interface Control Message Protocol Definitions
#include <netinet/if_ether.h>

#include <unistd.h> //standard symbolic constants and types
#include <signal.h> //define symbolic constants
#include <arpa/inet.h> //definitions for internet operations
#include <string.h> //memset and string .etc
#include <netdb.h> // defines the hostent structure
#include <pthread.h>  //for multithread
#include <list>//for package store
#include <map>//map for store something

struct EthIPHeader{
    unsigned char dst_mac[6];
    unsigned char src_mac[6];
    unsigned short eth_type;
    unsigned char ver_ihl;
    unsigned char tos;   //服务类型
    unsigned short tot_len; //总长度
    unsigned short id;    //标志
    unsigned short frag_off; //分片偏移
    unsigned char ttl;   //生存时间
    unsigned char protocol; //协议
    unsigned short chk_sum; //检验和
    unsigned char src_addr[4]; //源IP地址
    unsigned char dst_addr[4]; //目的IP地址
};

typedef struct EthIPHeader EthIPHeader;

class EthIPFrame{
public:
    EthIPHeader header;
    unsigned char* option;
    unsigned char* data;
    bool isChecked;
    EthIPFrame(){};//init while do nothing
    EthIPFrame(unsigned char* buffer,int bufferSize);
    ~EthIPFrame();
    int generateFrame(unsigned char* buffer,int bufferLength);
};