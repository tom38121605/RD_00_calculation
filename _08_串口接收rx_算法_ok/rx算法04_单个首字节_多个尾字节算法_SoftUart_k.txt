#include "STM_Config.h"


volatile uint8 iRec;
volatile uint8 iBitCount;


uint8 f_Start1;   //接收第1个开始字节
uint8 f_End1;     //接收第1个结束字节
uint8 f_End2;     //接收第2个结束字节


//变量定义
uint8 iRecData[SOFTRX_SIZE];
uint8 iRecData2[SOFTRX_SIZE];
uint16 iRecNum;
uint16 iRecNum2;
uint8 flg_Reced;   //已接收完一帧数据



//模拟串口初始化
void SoftUart_Init()
{
   //延时52us, 波特率19200 
   Timer1_Init(DIV_19200,COUNT_19200,0);    
   
   //模拟串口IO初始化
   SoftUart_IOInit(); 
   
   //模拟串口接收参数初始化
   SoftUart_RXInit();   
}
   
//模拟串口接收端口初始化  PD2--带中断的数据端口
void SoftUart_IOInit()   
{
   //设置PD3为带上拉输入, 屏蔽外部中断     --RX
   //PD_DDR|=(0<<3);   // 设置数据方向寄存器 1--输出，0--输入
   PD_DDR&=~(0<<3);   // 设置数据方向寄存器 1--输出，0--输入
   PD_CR1|=(1<<3);   // 设置带上拉电阻输入   
   PD_CR2&=~(1<<3);   // 屏蔽外部中断   
   
   
   //设置PD2为推挽快速输出, 初始高电平  --TX
   PD_DDR|=(1<<2);   // 设置数据方向寄存器 1--输出，0--输入 
   PD_CR1|=(1<<2);   // 设置推挽输出 
   PD_CR2|=(1<<2);   // 设置输出频率 1--10M(Fast)，0--2M
   PD_ODR|=(1<<2);   // 设置初始电平为高电平 
    
}

//模拟串口接收参数初始化
void SoftUart_RXInit()   
{
   iRecNum=0;
   f_Start1='f';
   f_End1='f';
   f_End2='f';
    
}

// Timer1 初始化
void Timer1_Init(uint16 iSplit, uint16 iCount, uint8 iRepeat)  
{  
   //flg_Timer1=0; 
   
   //iTimeCount =0;

   //=========Timer1寄存器设置============
      
   //计数次数设置
   TIM1_ARRH = (uint8)(iCount >> 8);
   TIM1_ARRL = (uint8)(iCount);


   //预分频设置 
   TIM1_PSCRH = (uint8)(iSplit >> 8);
   TIM1_PSCRL = (uint8)(iSplit);
  
   
   //选择边沿对齐方式
   TIM1_CR1 &= ~( (1<<5)|(1<<6) );
   
   //向上计数设置 
   TIM1_CR1 &= ~(1<<4);    

   
   //重复计数次数设置
   TIM1_RCR = iRepeat;
  
   //计数寄存器初值设置
   TIM1_CNTRH = 0;
   TIM1_CNTRL = 0;   
      
   //预装载不使能 
   //TIM1_CR1 &= ~(1<<7);
   TIM1_CR1 |= (1<<7);
   
   //允许更新中断 
   //TIM1_IER |= (1<<0);
   
   //使能定时器  --打开定时器 
   //TIM1_CR1 |= (1<<0);
   
}  


//模拟串口接收下降沿中断           
#pragma vector=8
__interrupt void EXTI_PORTD_IRQHandler(void)
{
   
   //  ------------ 关闭外部中断 -----------------

   PD_CR2&=~(1<<3);   // 暂时关闭外部中断   


   //  ------------ 启动定时器中断 -----------------
   
   //计数次数设置
   //TIM1_ARRH = (uint8)(iCount >> 8);
   TIM1_ARRL = (uint8)(78);
   
   TIM1_IER |= (1<<0); //允许更新中断 
   TIM1_CR1 |= (1<<0); //打开定时器     
   
   //  ------------ 开始bit计数 -----------------
   iBitCount=0;
   
}

//模拟串口接收字符串
void SoftUart_Receive()  // 输入数据包: AT#CW10086 0D 0A (拨号 10086)  //输出: AT#CW10086 0D 0A
{   
      //用软件模拟接收
      if( ( PD_IDR & (1<<3) )==0    )
      {
         
         //PA_ODR &= ~(1<<3);         
         Delay_52us();   
         Delay_5us();   
         
         //接收1个字符
         for(iBitCount=1; iBitCount<9; iBitCount++) 
         { 
            iRec>>=1; 

            if(  PD_IDR & (1<<3)   )
               iRec|=0X80; 
        
            //PA_ODR ^= (1<<3);
            Delay_52us();   
         }   
         
         //无效数据的处理
         if(f_Start1=='f' && iRec!=SOFTUART_RX_START1) return ;
         
         //接收第1个字节
         if(iRec==SOFTUART_RX_START1 && f_Start1=='f') 
         {
            f_Start1='t';
            iRecData[iRecNum]=iRec;
            iRecNum++;
            
            return;
         }
         
         //开始接收指令后面的数据
         if(f_Start1=='t')  
         {
            iRecData[iRecNum]=iRec;
            iRecNum++;
            
            if (iRec==SOFTUART_RX_END1 && f_End1=='f')
            {
               f_End1='t';
               return ;
            }
            if (iRec==SOFTUART_RX_END2 && f_End2=='f')
            {
               iRecNum2=iRecNum;
               memcpy(iRecData2,iRecData,iRecNum);
               flg_Reced=1;
            
               //iRecNum=0;
               //f_Start1='f';
               //f_End1='f';
               //f_End2='f';
               SoftUart_RXInit();
            }  
           
         }         

      }  //if( ( PD_IDR & (1<<2) )==0    )
           
}


//模拟串口接收定时器Timer1中断
#pragma vector=0xD
__interrupt void TIM1_UPD_OVF_TRG_BRK_IRQHandler(void)
{
   iBitCount++; 

   if(iBitCount<9) 
   { 
      iRec>>=1; 

      if( !( PD_IDR & (1<<3) )  )
         iRec|=0X80; 
   } 
   
   //RX_data1[RX_counter1] = iRec; 
   //RX_counter1++;        // Increment counter  
   //if (RX_counter1 >= 14)  
   //{  
   //   RX_data1[12]=0; 
   //   puts( RX_data1,12);  //由另一硬件串口输出到PC 
   //   RX_counter1 = 0; 
   //} 
   
   //SoftUart_PutChar(iRec);
   
   
   Uart1_PutChar(iRec);
   

   //TIMSK&=~(1<<TOIE2);   //close  
   //GICR|= (1<<6);      //使能int0中断 

   //计数次数设置
   //TIM1_ARRH = (uint8)(iCount >> 8);
   TIM1_ARRL = (uint8)(52);   
   
   
   TIM1_IER &= ~(1<<0);   //不允许更新中断 
   TIM1_CR1 &= ~(1<<0);   //关闭定时器中断   
   
   PD_CR2|=(1<<3);        // 打开外部中断   
   
   iBitCount=0; 
   
}


//模拟串口输出字符
void SoftUart_PutChar(uint8 cSend)     
{

   //用软件模拟发送1个字符      
   //发送起始位
   PD_ODR &= ~(1<<2);

   //PA_ODR &= ~(1<<3);         
   Delay_52us();  

   for(iBitCount=1; iBitCount<9; iBitCount++) 
   {  
      if ( cSend& 0x01 )
         PD_ODR |= (1<<2);
      else
         PD_ODR &= ~(1<<2);           
   
      cSend>>=1; 

      //PA_ODR ^= (1<<3);
      Delay_52us();   
   }  

   //发送停止位
   PD_ODR |= (1<<2);   
   Delay_52us();  

}


//模拟串口输出字符串
void SoftUart_Puts(uint8 *s)       
{
   while (*s)
   {
      SoftUart_PutChar(*s++);
   }
  
}

//模拟串口输出字节
void SoftUart_PutBytes(uint8 *s, uint16 ilen)  
{
   unsigned int i;
   
   for(i=0;i<ilen;i++)
   {
      SoftUart_PutChar(*s++);
   }
   
}


/*//处理模拟串口接收，通常放到主程序处理
void DoWithSoftUart(void)        
{
  ; 
 
}
*/