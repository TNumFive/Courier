#include "Courier.h"

EthIPFrame::EthIPFrame(unsigned char* buffer,int bufferSize){
    if(bufferSize<sizeof(EthIPHeader)){
        perror("buffer size too small, it must be bigger than 34");
        return;
    }
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
EthIPFrame::~EthIPFrame(){
    if(this->option!=nullptr){
        delete this->option;
    }
    if(this->data!=nullptr){
        delete this->data;
    }
}
int EthIPFrame::generateFrame(unsigned char* buffer,int bufferLength){
    if(this->isChecked==true){
        //todo: put data into buffer 
    }
}

int readIPEthernet(int bufferSize=1600){
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
        if(n<sizeof(EthIPHeader)){
            //if small than EthIPHeader ,then it's meanless as it does not contain a normal IP packet
            //which barely happened as we socket ETH_P_IP
            //but just in case
            continue;
        }
        EthIPFrame eif(buffer,n);
        printf("dst_mac");
        for(int i=0;i<6;i++){
            printf(":%02x",eif.header.dst_mac[i]);
        }
        printf("\n");

        printf("src_mac");
        for(int i=0;i<6;i++){
            printf(":%02x",eif.header.src_mac[i]);
        }
        printf("\n");

        printf("src_addr:");
        for(int i=0;i<4;i++){
            printf("%d",eif.header.src_addr[i]);
            if(i!=3){
                printf(".");
            }
        }
        printf("\n");

        printf("dst_addr:");
        for(int i=0;i<4;i++){
            printf("%d",eif.header.dst_addr[i]);
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
    readIPEthernet();
    return 0;
}