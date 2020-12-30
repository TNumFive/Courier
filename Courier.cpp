#include "Courier.h"

EthIPFrame::EthIPFrame(){
    memset(&this->header,0,sizeof(EthIPHeader));
    this->option=NULL;
    this->data=NULL;
    this->isChecked=false;
}

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
    }else{
        this->option=NULL;
    }
    int data_len=this->header.tot_len-this->header.ver_ihl&0x0f*4;
    if(data_len>0){
        this->data=new unsigned char[data_len];
        memcpy(this->data,&buffer[this->header.ver_ihl&0x0f*4],data_len);
    }else{
        this->data=NULL;
    }
    this->isChecked=false;
}
EthIPFrame::~EthIPFrame(){
    if(this->option!=NULL){
        delete [] this->option;
    }
    if(this->data!=NULL){
        delete [] this->data;
    }
}

int EthIPFrame::generateFrame(unsigned char* buffer,int bufferSize){
    int option_len=this->header.ver_ihl&0x0f*4-20;
    int data_len=this->header.tot_len-this->header.ver_ihl&0x0f*4;
    if(bufferSize<sizeof(EthIPHeader)+option_len+data_len){
        return -1;
    }
    memset(buffer,0,bufferSize);
    memcpy(buffer,&this->header,sizeof(EthIPHeader));
    if(option_len>0){
        memcpy(buffer+sizeof(EthIPHeader),this->option,option_len);
    }
    if(data_len>0){
        memcpy(buffer+sizeof(EthIPHeader)+option_len,this->option,data_len);
    }
    return 0;
}

int readIPEthernet(int bufferSize=1600){

    unsigned char inet[]={192,168,31,36};
    unsigned char ether[]={0x00,0x0c,0x29,0x1f,0x2f,0xf4};
    unsigned char gateway[]={0x40,0x31,0x3c,0x29,0xad,0xd8};
    unsigned char dns[]={224,0,0,251};
    int sock,n,counter=0;
    unsigned char buffer[bufferSize];
    if(0>(sock=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_IP)))){
        perror("socket");
        return -1;
    }
    while (true)
    {
        counter++;
        n=recvfrom(sock,buffer,bufferSize,0,NULL,NULL);
        if(n<sizeof(EthIPHeader)){
            //if small than EthIPHeader ,then it's meanless as it does not contain a normal IP packet
            //which barely happened as we socket ETH_P_IP
            //but just in case
            continue;
        }
        EthIPFrame eif(buffer,n);
        printf("============================\n");
        printf("~%d\n",counter);
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
        printf("\n");//224.0.0.251
        if((!strncmp((char *)eif.header.dst_addr,(char *)inet,4)&&strncmp((char *)eif.header.dst_mac,(char *)ether,6))
            ||strncmp((char *)eif.header.dst_addr,(char *)dns,4)
            ){
            //send to my mac while not send to my ip
            printf("route this package\n");
            memcpy(eif.header.dst_mac,gateway,6);
            eif.generateFrame(buffer,bufferSize);
            if(-1==sendto(sock,buffer,bufferSize,0,NULL,0)){
                counter=101;
            }
        }
        printf("============================\n");
        if(counter>100){
            close(sock);
            break;
        }
    }
    return 0;
}

int main(int argc,char** argv){
    std::cout<<"Hello Courier\n";
    readIPEthernet();
    return 0;
}