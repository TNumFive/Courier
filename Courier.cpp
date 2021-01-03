#include "Courier.h"

EthIPFrame::EthIPFrame()
{
    //set everything to zero other than default
    memset(this, 0, sizeof(EthIPFrame));
}

EthIPFrame::EthIPFrame(unsigned char *frame, unsigned short frameSize)
{
    if (frameSize < sizeof(EthIPHeader))
    {
        perror("frame size too small, it must be bigger than 34");
        memset(this, 0, sizeof(EthIPFrame));
        return;
    }
    this->frameSize = frameSize;
    this->MTU = frameSize - 14;
    memcpy((void *)&this->header, frame, sizeof(EthIPHeader));
    this->version = this->header.verIhl >> 4;
    this->ihl = this->header.verIhl & 0x0f;
    this->fragOpt = this->header.fragOff >> 13;
    this->fragOff = this->header.fragOff && 0x1fff;
    this->optLen = this->ihl * 4 - 20;
    if (this->optLen > 0)
    {
        this->option = new unsigned char[this->optLen];
        memcpy(this->option, &frame[20], this->optLen);
    }
    else
    {
        this->option = NULL;
    }
    this->fragLen = ((this->MTU - this->ihl) / 8) * 8;
    if ((this->fragOpt & 2 == 0)     //MF=0
        || (this->fragOff & 1 == 1)) //DF=1
    {
        this->dataLen = this->header.totLen % this->fragLen;
    }
    else
    {
        this->dataLen = this->fragLen;
    }
    if (this->dataLen > 0)
    {
        this->data = new unsigned char[this->dataLen];
        memcpy(this->data, &frame[this->header.verIhl & 0x0f * 4], this->dataLen);
    }
    else
    {
        this->data = NULL;
    }
    this->isChecked = true;
}

EthIPFrame::~EthIPFrame()
{
    if (this->option != NULL)
    {
        delete[] this->option;
    }
    if (this->data != NULL)
    {
        delete[] this->data;
    }
}

EthIPHeader EthIPFrame::getHeader(){
    return this->header;
}

void EthIPFrame::printBinary(unsigned char *binary, unsigned short length, unsigned short columnSize)
{
    int index = 0;
    while (index < length)
    {
        int leftToPrint = (length - index) < columnSize ? (length - index) : columnSize;
        for (int i = 0; i < columnSize; i++)
        {
            if (i < leftToPrint)
            {
                printf("%02x ", binary[index + i]);
            }
            else
            {
                printf("   ");
            }
        }
        printf("\t");
        for (int i = 0; i < columnSize; i++)
        {
            if (i < leftToPrint)
            {
                if (binary[index + i] == 0)
                {
                    printf("\\0");
                }
                else
                {
                    printf("%c ", binary[index + i]);
                }
            }
            else
            {
                break;
            }
        }
        printf("\n");
        index += columnSize;
    }
}
void EthIPFrame::printFrame()
{
    printf("src_mac(0x)");
    for (int i = 0; i < 6; i++)
    {
        printf(":%02x", this->header.srcMac[i]);
    }
    printf("\n");
    printf("dst_mac(0x)");
    for (int i = 0; i < 6; i++)
    {
        printf(":%02x", this->header.dstMac[i]);
    }
    printf("\n");
    printf("eth_type(0x):%04x\n", this->header.ethType);
    printf("version(0x):%02x\n", this->version);
    printf("ihl(0x):%02x\n", this->ihl);
    printf("tos(0x):%02x\n", this->header.tos);
    printf("tot_len(0x):%04x\n", this->header.totLen);
    printf("id(0x):%04x\n", this->header.id);
    printf("frag_opt(0x):%02x\n", this->fragOpt);
    printf("frag_off(0x):%04x\n", this->fragOff);
    printf("ttl(0x):%02x\n", this->header.ttl);
    printf("protocol(0x):%02x\n", this->header.protocol);
    printf("chk_sum(0x):%04x\n", this->header.chkSum);
    printf("src_addr(d):");
    for (int i = 0; i < 4; i++)
    {
        printf("%d", this->header.srcAddr[i]);
        if (i != 3)
        {
            printf(".");
        }
    }
    printf("\n");
    printf("dst_addr(d):");
    for (int i = 0; i < 4; i++)
    {
        printf("%d", this->header.dstAddr[i]);
        if (i != 3)
        {
            printf(".");
        }
    }
    printf("\n");
    if (this->optLen > 0)
    {
        printf("option(0x):%04x\n", this->optLen);
        this->printBinary(this->option, this->optLen, 4);
    }
    if (this->dataLen > 0)
    {
        printf("data(0x):%04x\n", this->dataLen);
        this->printBinary(this->data, this->dataLen, 4);
    }
}

int readEthIPFrame(unsigned short bufferSize = 1518)
{
    int sock, n, counter = 0;
    unsigned char buffer[bufferSize];
    if (0 > (sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP))))
    {
        perror("socket");
        return -1;
    }
    while (true)
    {
        n = recvfrom(sock, buffer, bufferSize, 0, NULL, NULL);
        if (n < sizeof(EthIPHeader))
        {
            //if small than EthIPHeader ,then it's meanless as it does not contain a normal IP packet
            //which barely happened as we socket ETH_P_IP
            //but just in case
            continue;
        }
        EthIPFrame eif(buffer, n);
        if (eif.getHeader().ethType == 99)
        { //not icmp
            counter++;
            printf("============================\n");
            printf("~%d\n", counter);
            eif.printFrame();
            printf("============================\n");
        }
        if (counter >= 100)
        {
            close(sock);
            break;
        }
    }
    return 0;
}

int main()
{
    printf("hello Courier!\n");
    readEthIPFrame();
    return 0;
}