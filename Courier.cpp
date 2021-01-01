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
                if(binary[index + i]==0)
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

int main()
{
    printf("hello Courier!\n");
    
    return 0;
}