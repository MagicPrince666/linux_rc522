#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "MFRC_Task.h"
#include "MFRC522.h"
#include "spi.h"

void gpio_init(void);
void gpio_close(void);

const unsigned char data1[16] = {0x12,0x34,0x56,0x78,0xED,0xCB,0xA9,0x87,0x12,0x34,0x56,0x78,0x01,0xFE,0x01,0xFE};
//M1卡的某一块写为如下格式，则该块为钱包，可接收扣款和充值命令
//4字节金额（低字节在前）＋4字节金额取反＋4字节金额＋1字节块地址＋1字节块地址取反＋1字节块地址＋1字节块地址取反 
unsigned char data2[4]  = {0,0,0,0x01};
unsigned char DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 

int led_fd = -1;
int reset_fd = -1;
int irq_fd = -1;
char in = 0;

unsigned char g_ucTempbuf[20];


int main(int argc, char **argv)
{
    unsigned char status,i;
	unsigned int temp;
    InitializeSystem( );
    PcdReset();
    PcdAntennaOff();
    PcdAntennaOn();
    printf("Scanning card\n");
    while ( 1 )
    {
        status = PcdRequest(PICC_REQALL, g_ucTempbuf);//寻卡
        if (status != MI_OK)
        {
            printf("status = %d\n", status);
            sleep(1);
            PcdReset();
            PcdAntennaOff(); 
            PcdAntennaOn(); 
			continue;
        }
			     
        printf("card type:");
        for(i=0;i<2;i++)
        {
            temp=g_ucTempbuf[i];
            printf("%X",temp);
        }
        printf("\n");

        status = PcdAnticoll(g_ucTempbuf);//防冲撞
        if (status != MI_OK)
        {    continue;    }

        ////////以下为超级终端打印出的内容////////////////////////

        printf("card ID:");	//超级终端显示,
        for(i=0;i<4;i++)
        {
            temp=g_ucTempbuf[i];
            printf("%X",temp); 
        }
        printf("\n");
        ///////////////////////////////////////////////////////////

        status = PcdSelect(g_ucTempbuf);//选定卡片
        if (status != MI_OK)
        {    continue;    }

        status = PcdAuthState(PICC_AUTHENT1A, 1, DefaultKey, g_ucTempbuf);//验证卡片密码
        if (status != MI_OK)
        {    continue;    }

        status = PcdWrite(1, data1);//写块
        if (status != MI_OK)
        {    continue;    }

        while(1)
		{
            status = PcdRequest(PICC_REQALL, g_ucTempbuf);//寻卡
            if (status != MI_OK) {
                printf("status = %d\n", status);
                sleep(1);
                PcdReset();
                PcdAntennaOff(); 
                PcdAntennaOn(); 
                continue;
            }

            status = PcdAnticoll(g_ucTempbuf);//防冲撞
            if (status != MI_OK)
            {    continue;    }

            status = PcdSelect(g_ucTempbuf);//选定卡片
            if (status != MI_OK)
            {    continue;    }

            status = PcdAuthState(PICC_AUTHENT1A, 1, DefaultKey, g_ucTempbuf);//验证卡片密码
            if (status != MI_OK)
            {    continue;    }

            status = PcdValue(PICC_DECREMENT,1,data2);//扣款
            if (status != MI_OK)
            {    continue;    }

            status = PcdBakValue(1, 2);//块备份
            if (status != MI_OK)
            {    continue;    }

            status = PcdRead(2, g_ucTempbuf);//读块
            if (status != MI_OK)
            {    continue;    }

            printf("read block:");	//超级终端显示,
            for(i=0;i<16;i++)
            {
                temp=g_ucTempbuf[i];
                printf("%X",temp);
            }

            printf("\n");
            LED_GREEN_0;
            printf("beeeeeeee\n");//刷卡声
            usleep(100000);
            LED_GREEN_1;
            usleep(100000);
            LED_GREEN_0;
            usleep(200000);
            LED_GREEN_1;
            PcdHalt();
		}
        usleep(200000);
    }
    SPI_Close();
    gpio_close();
}

void gpio_init(void)
{
    FILE* set_export;

    //打开设备节点
	set_export = fopen ("/sys/class/gpio/export", "w");
	if(set_export == NULL)printf ("Can't open /sys/class/gpio/export!\n");
	else {
        fprintf(set_export,"11");
    }
	fclose(set_export);

    set_export = fopen ("/sys/class/gpio/export", "w");
	if(set_export == NULL)printf ("Can't open /sys/class/gpio/export!\n");
	else {
        fprintf(set_export,"39");
    }
	fclose(set_export);

    set_export = fopen ("/sys/class/gpio/export", "w");
	if(set_export == NULL)printf ("Can't open /sys/class/gpio/export!\n");
	else {
        fprintf(set_export,"42");
    }
	fclose(set_export);

	//设置成输出
	set_export = fopen ("/sys/class/gpio/gpio11/direction", "w");
	if(set_export == NULL)printf ("Can't open /sys/class/gpio/gpio11/direction!\n");
	else fprintf(set_export,"out");
	fclose(set_export);//设置
    set_export = fopen ("/sys/class/gpio/gpio42/direction", "w");
	if(set_export == NULL)printf ("Can't open /sys/class/gpio/gpio42/direction!\n");
	else fprintf(set_export,"out");
	fclose(set_export);//设置
    //设置成输入
    set_export = fopen ("/sys/class/gpio/gpio39/direction", "w");
	if(set_export == NULL)printf ("Can't open /sys/class/gpio/gpio39/direction!\n");
	else fprintf(set_export,"in");
	fclose(set_export);//设置
}

void gpio_close(void)
{
    close(led_fd);
    close(reset_fd);
    close(irq_fd);

    FILE* set_export;
    set_export = fopen ("/sys/class/gpio/unexport", "w");
	if(set_export == NULL)printf ("Can't open /sys/class/gpio/unexport!\n");
	else {
        fprintf(set_export,"11");
    }
	fclose(set_export);

    set_export = fopen ("/sys/class/gpio/unexport", "w");
	if(set_export == NULL)printf ("Can't open /sys/class/gpio/unexport!\n");
	else {
        fprintf(set_export,"39");
    }
	fclose(set_export);

    set_export = fopen ("/sys/class/gpio/unexport", "w");
	if(set_export == NULL)printf ("Can't open /sys/class/gpio/unexport!\n");
	else {
        fprintf(set_export,"42");
    }
	fclose(set_export);
}
/////////////////////////////////////////////////////////////////////
//系统初始化
/////////////////////////////////////////////////////////////////////

void InitializeSystem()
{
    SPI_Open();
    gpio_init();
    led_fd = open ("/sys/class/gpio/gpio11/value", O_RDWR);
    reset_fd = open ("/sys/class/gpio/gpio39/value", O_RDWR);
    irq_fd = open ("/sys/class/gpio/gpio42/value", O_RDWR);
    write(led_fd, "1", 1);
    write(reset_fd, "0", 1);
}
