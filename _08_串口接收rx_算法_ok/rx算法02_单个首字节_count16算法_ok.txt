#include "STM_Config.h"


uint8 flg_Start1;   //接收第1个开始字节
uint8 flg_Start2;   //接收第2个开始字节
uint8 flg_Start3;   //接收第3个开始字节
uint8 flg_Start4;   //接收第4个开始字节
uint16 iRX_Sum;

uint8 RX_data[RX_BUFFER_SIZE]={0};
uint8 RX_data2[RX_BUFFER_SIZE]={0};
uint16 iRXNum=0;
uint16 iRXNum2=0;
uint8 flg_Received=0;  //已接收到有效数据   


//串口初始化函数
void Uart1_Init(uint32  iBaudRate)
{
   
   uint32 iBaudDiv=0;
   
   //----------------寄存器初始化---------------------   
   (void)UART1_SR;
   (void)UART1_DR;
    
   UART1_CR1=0;
   UART1_CR2=0;
   UART1_CR3=0;
   UART1_CR4=0;
   UART1_CR5=0;
   UART1_GTR=0;
   UART1_PSCR=0;
   
   Uart_RXInit();
   
   
   //----------------寄存器配置---------------------   
   
   //设置数据位 8
   UART1_CR1 &= ~(1<<4);   
   
   //设置停止位 1
   UART1_CR3 &= ~( (1<<4)|(1<<5) );      
   
   //设置奇偶校验位 无
   //UART1_CR1 &= ~( (1<<1)|(1<<2) ); 
   UART1_CR1 &= ~(1<<2) ; 
   

   //设置波特率  (注意点: 要先设置BRR2, 后设置BRR1)
   //baud_div =MASTER_FREQ/iBit;  /*求出分频因子*/
   iBaudDiv = (uint32)( (float)((float)MASTER_FREQ/(float)iBaudRate *10 + 5)/10 );  //求出分频因子
   UART1_BRR2 = iBaudDiv & 0x0f;
   UART1_BRR2 |= ((iBaudDiv & 0xf000) >> 8);
   UART1_BRR1 = ((iBaudDiv & 0x0ff0) >> 4);    //先给BRR2赋值 最后再设置BRR1

   
   //设置打开串口发送和接收
   UART1_CR2 |= (1<<2)|(1<<3);     

   //是SCLK引脚上的输出时钟是否使能  -- 暂时不考虑
   //...
   
   //配置串口的接收中断使能
   UART1_CR2 |= (1<<5);         
   
   //设置打开串口 , 0--打开
   UART1_CR1 &= ~(1<<5);
   
}

//串口接收参数初始化
void Uart_RXInit()   
{
   iRXNum=0;
   iRX_Sum=0;
   
   flg_Start1='f';
   flg_Start2='f';
   flg_Start3='f';
   flg_Start4='f';
    
}

#pragma vector=0x14
__interrupt void UART1_RX_IRQHandler(void)  //数据包: AA 00 03 02 00 04 F7  输出指令: 02 00 04
{ 
   uint8 cUartByte;
   uint8 i;
   uint16 iCheckSum;
   //uint16 iFunction;
   
   if( !(UART1_SR & (1<<5)) )  //如果不是接收中断则退出
      return;
      
   cUartByte=UART1_DR;
   
   //Uart1_PutChar(cUartByte+1);   
   //return;   
   
   if(flg_Start1=='f' && cUartByte!=0xAA) 
      return;
   
   //接收第1个字节
   if(cUartByte==0xAA && flg_Start1=='f') 
   {
      flg_Start1='t';  
      
//test
      //Uart1_Puts("1");

      return;      
   }
   
   //接收第2个字节  -- iRx_Sum高字节
   if(flg_Start2=='f' && flg_Start1=='t') 
   {
      flg_Start2='t';
      
//test
      //Uart1_Puts("2");

      //iRx_Sum=cUartByte;
      return;
   }
   
   //接收第3个字节  -- iRx_Sum低字节
   if(flg_Start3=='f' && flg_Start2=='t') 
   {
      flg_Start3='t';
      
//test
      //Uart1_Puts("3");

      //iRx_Sum=(iRx_Sum<<8)+cUartByte;
      iRX_Sum=cUartByte;
      
      if (iRX_Sum==0)
      {
         Uart_RXInit();
      }
      
      return;
   }
   
   //接收第4个字节  -- 命令字节
   if(flg_Start4=='f' && flg_Start3=='t') 
   {
//test
      //Uart1_Puts("4");

      flg_Start4='t';
      //iFunction=cUartByte;
      
      RX_data[iRXNum++]=cUartByte;
      
      return;
   }   
   
   //接收命令参数和校验和
   if(flg_Start4=='t') 
   {
//test
      //Uart1_Puts("5");

      if( iRXNum<iRX_Sum  )  //接收满iRx_Sum个数据: 命令+参数
      {
         RX_data[iRXNum++]=cUartByte;
         return;
      }
      
      //接收最后一个校验和字节      
      //RX_data[iRXNum++]=cUartByte;
      
      //检查校验和
      //iCheckSum = (uint8)iRX_Sum + (uint8)( iRx_Sum >>8);
      iCheckSum = iRX_Sum;
      for(i=0; i<iRX_Sum; i++)
      {
         iCheckSum=iCheckSum+RX_data[i]; 
      }  
      
      if ( cUartByte == (uint8)( ~((uint8)iCheckSum) +1 )   )
      {
         //把数据复制到缓冲区sRX_Buf2              
         iRXNum2=iRXNum;
         memcpy(RX_data2,RX_data,iRXNum);

         //串口接收初始化
         Uart_RXInit();

         flg_Received = 1;   //串口接收到有效数据包标志
        
         //关串口接收中断考虑__

      }         
      else
      {
         flg_Received = 0;  
     
         //串口接收初始化
         Uart_RXInit();  

         return;
      }
      
   }  
    
}


//串口输出字符
void Uart1_PutChar(uint8 c)
{
   while ( !(UART1_SR & (1<<7)) );  //判断上次发送有没有完成
   UART1_DR = c;
}

//串口输出字符串
void Uart1_Puts(uint8 *s)
{
   while (*s)
   {
      Uart1_PutChar(*s++);
   }
}

//串口输出字节
void Uart1_PutBytes(uint8 *s, uint16 ilen)
{
   unsigned int i;
   
   for(i=0;i<ilen;i++)
   {
      Uart1_PutChar(*s++);
   }
}


/*
//串口接收处理         //推荐－－把这个模块放到主程序去处理
void DoWithUart(void)
{
   RX_data2[RX_BUFFER_SIZE-2]=0;
   
   UART_Puts("   ");
   UART_Puts(RX_data2);
   UART_Puts(" back  ");
   
   flg_Received=0;
}

*/
