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

int readAllEthernet(int bufferSize=1600){
    int sock,n;
    char buffer[bufferSize];
    ethhdr *eth;
    iphdr *iph;

    if(0>(sock=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_IP)))){
        perror("socket");
        return -1;
    }
    while (true)
    {
        printf("============================\n");
        n=recvfrom(sock,buffer,bufferSize,0,NULL,NULL);
        printf("%d bytes read\n",n);
        eth=(ethhdr*)buffer;
        printf("Dest MAC addr:%02x:%02x:%02x:%02x:%02x:%02x\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
		printf("Source MAC addr:%02x:%02x:%02x:%02x:%02x:%02x\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
        printf("\n");
        EthIPHeader* eih=(EthIPHeader*)buffer;
        printf("Dest MAC addr:%02x:%02x:%02x:%02x:%02x:%02x\n",eih->dst_mac[0],eih->dst_mac[1],eih->dst_mac[2],eih->dst_mac[3],eih->dst_mac[4],eih->dst_mac[5]);
		printf("Source MAC addr:%02x:%02x:%02x:%02x:%02x:%02x\n",eih->src_mac[0],eih->src_mac[1],eih->src_mac[2],eih->src_mac[3],eih->src_mac[4],eih->src_mac[5]);
        printf("\n");
        iph=(iphdr*)(buffer+sizeof(ethhdr));
        if(iph->version==4){
            {
                unsigned char *sd,*dd;
                sd=(unsigned char*)&iph->saddr;
                dd=(unsigned char*)&iph->daddr;
                printf("Source Host:%d.%d.%d.%d Dest host: %d.%d.%d.%d\n", sd[0], sd[1], sd[2], sd[3], dd[0], dd[1], dd[2], dd[3]);  
                printf("ver_ihl:%d_%d\n",iph->version,iph->ihl);
            }
            // for(int i=14;i<14+sizeof(iphdr);i++){
            //     printf("%02x\t",buffer[i]);
            //     if((i-13)%4==0){
            //         printf("\n");
            //     }
            // }
            printf("\n");
            {
                printf("Source Host:%d.%d.%d.%d Dest host: %d.%d.%d.%d\n", eih->src_addr[0], eih->src_addr[1], eih->src_addr[2], eih->src_addr[3], eih->dst_addr[0], eih->dst_addr[1], eih->dst_addr[2], eih->dst_addr[3]);
                printf("ver_ihl:%d_%d\n",eih->ver_ihl>>4,eih->ver_ihl&0x0f);
            }  
            break;
        }
        sizeof(struct in_addr);
    }
    
    return 0;
}

int main(int argc,char** argv){
    std::cout<<"Hello Courier\n";
    readAllEthernet();
    return 0;
}