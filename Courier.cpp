#include "Courier.h"

EthFrame::EthFrame(unsigned char* buffer){
    memcpy((void*)&this->header,buffer,sizeof(EthIPHeader));
    int option_len=this->header.ver_ihl&0x0f*4-20;
    if(option_len>0){
        this->option=new unsigned char[option_len];
        memcpy(this->option,&buffer[20],option_len);
    }
    int data_len=this->header.tot_len-this->header.ver_ihl&0x0f*4;
    if(data_len>0){
        this->data=new unsigned char[data_len];
        memcpy(this->data,&buffer[this->header.ver_ihl&0x0f*4],data_len);
    }
}
EthFrame::~EthFrame(){
    if(this->option!=nullptr){
        delete this->option;
    }
    if(this->data!=nullptr){
        delete this->data;
    }
}

int readAllEthernet(int bufferSize=1600){
    int sock,n;
    unsigned char buffer[bufferSize];
    if(0>(sock=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_IP)))){
        perror("socket");
        return -1;
    }
    while (true)
    {
        printf("============================\n");
        n=recvfrom(sock,buffer,bufferSize,0,NULL,NULL);
        EthFrame ef(buffer);
        printf("dst_mac");
        for(int i=0;i<6;i++){
            printf(":%02x",ef.header.dst_mac[i]);
        }
        printf("\n");

        printf("src_mac");
        for(int i=0;i<6;i++){
            printf(":%02x",ef.header.src_mac[i]);
        }
        printf("\n");

        printf("src_addr:");
        for(int i=0;i<4;i++){
            printf("%d",ef.header.src_addr[i]);
            if(i!=3){
                printf(".");
            }
        }
        printf("\n");

        printf("dst_addr:");
        for(int i=0;i<4;i++){
            printf("%d",ef.header.dst_addr[i]);
            if(i!=3){
                printf(".");
            }
        }
        printf("\n");
        break;
    }
    
    return 0;
}

int main(int argc,char** argv){
    std::cout<<"Hello Courier\n";
    readAllEthernet();
    return 0;
}