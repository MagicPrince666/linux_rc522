#ifndef __MFRC_TASK_H_
#define __MFRC_TASK_H_

#include <fcntl.h>
#include <sys/ioctl.h>
//////////////////////////////////
//端口定义
extern int led_fd;
extern int reset_fd;
extern int irq_fd;
extern char in;
//蜂鸣器引脚定义
#define     MF522_IRQ    read(irq_fd,&in,1)

#define     MF522_RST_1 write(reset_fd,"1",1)
#define     MF522_RST_0 write(reset_fd,"0",1)

#define     LED_GREEN_1 write(led_fd,"1",1)
#define     LED_GREEN_0 write(led_fd,"0",1)


/////////////////////////////////////////////////////////////////////
//函数原型
/////////////////////////////////////////////////////////////////////
void InitializeSystem();    
void MFRC_main();                               
                                    
#endif
