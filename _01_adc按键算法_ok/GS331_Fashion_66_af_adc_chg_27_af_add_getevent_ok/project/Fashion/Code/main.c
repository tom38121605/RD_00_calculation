#include "N76E003.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Common.h"
#include "Delay.h"
#include <string.h>


#include "i2c_VCNL3020.h"


unsigned char temp _at_ 0x08;
unsigned char idata itemp _at_ 0x80;
unsigned char xdata xtemp _at_ 0x80;



//i2c

#define I2C_CLOCK_50KHz     79 
#define I2C_CLOCK_100KHz    39 
#define I2C_CLOCK_200KHz    19 
#define I2C_CLOCK_400KHz    9 

//#define SLAVEADDR_SLA       0x26  
#define SLAVEADDR_SLA       (0x44 << 1 )  

#define SLAVEADDR_WR               0
#define SLAVEADDR_RD               1

bit I2C_Reset_Flag;
 
//result value

UINT8 flg_sensor_init =0;

UINT16 RESULT1_COMEOUT =0;
UINT16 RESULT1_COMEIN =0;
UINT16 RESULT2_COMEOUT =0;
UINT16 RESULT2_COMEIN =0;

#define RESULT_LOWEST  80


//#define CENTER_RANGE  40
//#define PROX_GAP  15

//#define CENTER_RANGE  30
//#define PROX_GAP  15
#define PROX_IN_GAP   45
#define PROX_OUT_GAP  15


#define SENSOR_R_PIN   P04
#define SENSOR_R_PUSHPULL_MODE        P04_PushPull_Mode

#define SENSOR_L_PIN   P30
#define SENSOR_L_PUSHPULL_MODE        P30_PushPull_Mode

UINT16 iLTA_value1=0; 
UINT16 iresultvalue1=0; 
UINT16 iresultvalue1_out=0;
UINT16 iresultvalue1_avr=0;
UINT16 iresultvalue1_delay=0;
UINT32 iresultcount1=0;
//UINT32 iresultvalue1_sum = 0;

UINT16 iLTA_value2=0; 
UINT16 iresultvalue2=0; 
UINT16 iresultvalue2_out=0;
UINT16 iresultvalue2_avr=0;
UINT16 iresultvalue2_delay=0;
UINT32 iresultcount2=0;
//UINT32 iresultvalue2_sum = 0;


#define AVRBITCOUNT    4
#define AVRREADYCOUNT  200

UINT8 stemp11[20]={0};
UINT8 stemp12[20]={0};
UINT8 stemp13[20]={0};
UINT8 stemp15[20]={0};

UINT8 stemp21[20]={0};
UINT8 stemp22[20]={0};
UINT8 stemp23[20]={0};
UINT8 stemp25[20]={0};


//sensor pin
#define SENSOR_L_PIN                  P30   
#define SENSOR_L_PUSHPULL_MODE        P30_PushPull_Mode

#define SENSOR_R_PIN                  P04   
#define SENSOR_R_PUSHPULL_MODE        P04_PushPull_Mode

  
//adc
UINT16 BAND_value=0;

#define BAND_3300 1448

#define RESISTOR_UP_K     10
#define RESISTOR_DOWN_K   15
 
UINT16 iadcvalue=0;
  
UINT16 iadcvalue_out=0;
 
UINT16 iadcvalue_avr=0;
 
UINT16 iadcvalue_delay=0;
 
UINT8 iadccount=0;

UINT8 flg_channel =0;

UINT8 sadctemp1[20];
UINT8 sadctemp2[20];

UINT8 sadcstate[20];

//double fadc_key=0;
UINT16 iadc_key=0;


//key 
//#define KEY0_NC_PIN                  P17   //--test
//#define KEY0_NC_PUSHPULL_MODE        P17_PushPull_Mode
UINT8   KEY0_NC_PIN  =0; 

#define KEY1_L_PIN                   P11
#define KEY1_L_PUSHPULL_MODE         P11_PushPull_Mode

#define KEY2_CENTER_PIN              P00
#define KEY2_CENTER_PUSHPULL_MODE    P00_PushPull_Mode

#define KEY3_DOWN_PIN                P10
#define KEY3_DOWN_PUSHPULL_MODE      P10_PushPull_Mode

#define KEY4_R_PIN                   P12
#define KEY4_R_PUSHPULL_MODE         P12_PushPull_Mode

#define KEY5_UP_PIN                  P15
#define KEY5_UP_PUSHPULL_MODE        P15_PushPull_Mode


#define ADC_VOL_GAP                 200   

//#define ADC_VOL_L                   (800  - ADC_VOL_GAP)     
//#define ADC_VOL_CENTER              (1300 - ADC_VOL_GAP)
//#define ADC_VOL_DOWN                (1900 - ADC_VOL_GAP)   
//#define ADC_VOL_R                   (2300 - ADC_VOL_GAP)
//#define ADC_VOL_UP                  (2700 - ADC_VOL_GAP)

#define ADC_VOL_UP                  (800  - ADC_VOL_GAP)     
#define ADC_VOL_CENTER              (1300 - ADC_VOL_GAP)
#define ADC_VOL_L                   (1900 - ADC_VOL_GAP)   
#define ADC_VOL_DOWN                (2300 - ADC_VOL_GAP)
#define ADC_VOL_R                   (2700 - ADC_VOL_GAP)


#define ADC_VOL_NC                  300   //(ADC_VOL_L  - ADC_VOL_GAP)    
#define ADC_VOL_NC2                 3000  //(ADC_VOL_UP + ADC_VOL_GAP)    


//led current
#define IRLED_CURRENT   1    //1 = 10 mA, бн., 20 = 200 mA (2 = 20 mA = DEFAULT)
#define IRLED_CURRENT2  1    
UINT8 icur =0; 

 

//timer1
UINT32 itimer1_count=0;
UINT32 itimer1_count2=0;
//UINT32 itimer1_count3=0;

UINT32 iltascan=0;

   
void iputs0(UINT8 *msg)
{
   while(*msg)
		 Send_Data_To_UART0(*msg++);
}


//void Init_I2C(void)
//{
//   P13_OpenDrain_Mode;				 
//   P14_OpenDrain_Mode;				 

//   // Set I2C clock rate  
//   //I2CLK =  I2C_CLOCK_400KHz; 
//   I2CLK =  I2C_CLOCK_100KHz; 
//   //I2CLK =  I2C_CLOCK_50KHz; 

//   // Enable I2C  
//   set_I2CEN;                                   
//}

void Init_I2C_P13_P14(void)
{
   P13_OpenDrain_Mode;				 
   P14_OpenDrain_Mode;	
   
   clr_I2CEN; 

   clr_I2CPX;   

   // Set I2C clock rate  
   I2CLK =  I2C_CLOCK_400KHz;    

   // Enable I2C  
   set_I2CEN;   
   
}

void Init_I2C_P02_P16(void)
{
   P02_OpenDrain_Mode;				 
   P16_OpenDrain_Mode;	
   
   clr_I2CEN; 
   
   set_I2CPX;   

   // Set I2C clock rate  
   I2CLK =  I2C_CLOCK_400KHz; 


   // Enable I2C  
   set_I2CEN;                                   
}


void I2C_SI_Check(void)
{
   if (I2STAT == 0x00)
   {
      I2C_Reset_Flag = 1;
      set_STO;
      SI = 0;
      if(SI)
      {
         clr_I2CEN;
         set_I2CEN;
         clr_SI;
         clr_I2CEN;		
      } 	
   }	
   
}


void i2c_write( UINT8 iaddr, UINT8 *sbuf, UINT8 ilen )
{
   
    UINT8  u8Count;

    /* Step1 */
    set_STA;                                /* Send Start bit to I2C EEPROM */         
    clr_SI;
    while (!SI);
   
    if (I2STAT != 0x08)                     /* 0x08:  A START condition has been transmitted*/
    {
        I2C_Reset_Flag = 1;
        iputs0("WR Send STAw' error\r\n");
        goto Write_Error_Stop;
    }
    
    clr_STA; 
    

    /* Step2 */
    //--clr_STA;                                /* Clear STA and Keep SI value in I2CON */
    I2DAT = SLAVEADDR_SLA | SLAVEADDR_WR;         /* Send (SLA+W) to EEPROM */
    clr_SI;
    while (!SI);
    if (I2STAT != 0x18)                     /* 0x18: SLA+W has been transmitted; ACK has been received */             
    {
        I2C_Reset_Flag = 1;       
        //iputs0("I2C 'Send SLA+W' error\r\n");
       
        goto Write_Error_Stop;
    }

    /* Step3 */
    I2DAT = iaddr;             /* Send EEPROM's  Address */
    clr_SI;
    while (!SI);
    if (I2STAT != 0x28)                     /* 0x28:  Data byte in S1DAT has been transmitted; ACK has been received */
    {
        I2C_Reset_Flag = 1;
        //iputs0("I2C 'Send sub address' error\r\n");
        goto Write_Error_Stop;
    }
 

    /* Step5 */
    /* Write data to I2C EEPROM */ 

    for (u8Count = 0; u8Count < ilen; u8Count++)
    {
    
      I2DAT = sbuf[u8Count];                    /* Send data to EEPROM */
      clr_SI;
      while (!SI);
      if (I2STAT != 0x28)                 /* 0x28:  Data byte in S1DAT has been transmitted; ACK has been received */
      {
         I2C_Reset_Flag = 1;
         //iputs0("I2C 'Write Data' error\r\n");
         goto Write_Error_Stop;
      }   
   }
 
   
   /* Step6 */

   set_STO;                           
   clr_SI;
   while (STO);
   

//iputs0("I2C write ok\r\n");   //--test
   
 
Write_Error_Stop: 

   
   if (I2C_Reset_Flag)
   {
      
      set_STO;                           
      clr_SI;
      while (STO);

      I2C_SI_Check();
      I2C_Reset_Flag	= 0;
      //iputs0("I2C Write error, test stop\r\n");
      
      //test
      I2CEN=0;
      I2CEN=1;	      
   }	

		
}


void i2c_read( UINT8 iaddr, UINT8 *sbuf, UINT8 ilen )
{
    UINT8  u8Count; 
   

    /* Step1 */
    set_STA;                                /* Send Start bit to I2C EEPROM */
    clr_SI;
    while (!SI);
    if (I2STAT != 0x08)                     /* 0x08:  A START condition has been transmitted*/
    {
      I2C_Reset_Flag = 1;
      iputs0("RD Send STAR' error\r\n");
      goto Read_Error_Stop;
    }

    /* Step2 */
    I2DAT = (SLAVEADDR_SLA | SLAVEADDR_WR);       /* Send (SLA+W) to EEPROM */
    clr_STA;                                /* Clear STA and Keep SI value in I2CON */    
    clr_SI;
    while (!SI);
    if (I2STAT != 0x18)                     /* 0x18: SLA+W has been transmitted; ACK has been received */              
    {
      I2C_Reset_Flag = 1;
      //iputs0("I2C 'Send SLA+W' error\r\n");
      goto Read_Error_Stop;
    }

    /* Step3 */
    I2DAT = iaddr;             /* Send I2C EEPROM's Address */
    clr_SI;
    while (!SI);
    if (I2STAT != 0x28)                     /* 0x28:  Data byte in S1DAT has been transmitted; ACK has been received */              
    {
      I2C_Reset_Flag = 1;
      //iputs0("I2C 'Send I2C High Byte Address' error\r\n");
      goto Read_Error_Stop;
    }
 
    /* Step5 */
    set_STA;                                /* Repeated START */
    clr_SI; 
    while (!SI);
    if (I2STAT != 0x10)                     /* 0x10: A repeated START condition has been transmitted */
    {
      I2C_Reset_Flag = 1;
      //iputs0("I2C 'Send STA' error\r\n");
      goto Read_Error_Stop;
    }

    /* Step6 */
    clr_STA;                                /* Clear STA and Keep SI value in I2CON */
    I2DAT = (SLAVEADDR_SLA | SLAVEADDR_RD);       /* Send (SLA+R) to EEPROM */
    clr_SI;
    while (!SI);
    if (I2STAT != 0x40)                     /* 0x40:  SLA+R has been transmitted; ACK has been received */              
    {
      I2C_Reset_Flag = 1;
      //iputs0("I2C 'Send SLA+R' error\r\n");
      goto Read_Error_Stop;
    }

    /* Step7 */                             /* Verify I2C EEPROM data */
    for (u8Count = 0; u8Count <ilen; u8Count++)
    {
        set_AA;                             /* Set Assert Acknowledge Control Bit */
        clr_SI;
        while (!SI);
        if (I2STAT != 0x50)                 /* 0x50:Data byte has been received; NOT ACK has been returned */              
        {
            I2C_Reset_Flag = 1;
            //iputs0("I2C 'No Ack' error\r\n");
            goto Read_Error_Stop;
        }
       
        sbuf[u8Count] = I2DAT;
    }

    /* Step8 */
    clr_AA;                                 /* Send a NACK to disconnect 24xx64 */
    clr_SI;
    while (!SI);
    if (I2STAT != 0x58)                     /* 0x58:Data byte has been received; ACK has been returned */
    {
        I2C_Reset_Flag = 1;
        //iputs0("I2C 'Ack' error\r\n");
        goto Read_Error_Stop;
    }
    
	/* Step9 */    
	 clr_SI;
    set_STO;
    
    while (STO);
    
    
//iputs0("I2C Read ok\r\n");   //--test
        
 
Read_Error_Stop: 
   if (I2C_Reset_Flag)
   {
      I2C_SI_Check();
      //--iputs0("I2C Read error, test stop\r\n");
      I2C_Reset_Flag = 0;
      
      //test
      I2CEN=0;
      I2CEN=1;	
   }
   
   
   
}

 

//void set_current(UINT8 icur)
//{

//   UINT8 sbuf1[10]={0};
//   UINT8 sbuf2[10]={0};
//   //UINT8 stemp[20]={0};   
//   
//   //set the IR LED current
//   sbuf2[0]=0; 
//   i2c_read(REG03, sbuf2, 1);    
//   
//   sbuf2[0] &= 0xc0;  
//   sbuf2[0]|= icur;  
//   i2c_write(REG03, sbuf2, 1);
//   Timer0_Delay1ms(10);  
//   
//}

//void set_current2(UINT8 icur)
//{

//   UINT8 sbuf1[10]={0};
//   UINT8 sbuf2[10]={0};
//   //UINT8 stemp[20]={0};   
//   
//   //set the IR LED current
//   //sbuf2[0]=0; 
//   
//   //i2c_read(REG03, sbuf2, 1);
//   sbuf2[0]=VCNL3020_ReadOneByte(REG03);
//  
//   sbuf2[0] &= 0xc0;  
//   sbuf2[0]|= icur; 
//   
//   //i2c_write(REG03, sbuf2, 1);
//   VCNL3020_WriteOneByte(REG03, sbuf2[0]);
//   Timer0_Delay1ms(10);  
//   
//}

double  Bandgap_Voltage;
void READ_BANDGAP(void)
{
      UINT8 BandgapHigh,BandgapLow,BandgapMark;
      double Bandgap_Value,Bandgap_Voltage_Temp;

      set_IAPEN;
      IAPCN = READ_UID;
      IAPAL = 0x0d;
      IAPAH = 0x00;
      set_IAPGO;
      BandgapLow = IAPFD;
      BandgapMark = BandgapLow&0xF0;
			
		if (BandgapMark==0x80)
		{
				BandgapLow = BandgapLow&0x0F;
				IAPAL = 0x0C;
				IAPAH = 0x00;
				set_IAPGO;
				BandgapHigh = IAPFD;
				Bandgap_Value = (BandgapHigh<<4)+BandgapLow;
				Bandgap_Voltage_Temp = Bandgap_Value*3/4;
				Bandgap_Voltage = Bandgap_Voltage_Temp - 33;			//the actually banggap voltage value is similar this value.
		}
		if (BandgapMark==0x00)
		{
				BandgapLow = BandgapLow&0x0F;
				IAPAL = 0x0C;
				IAPAH = 0x00;
				set_IAPGO;
				BandgapHigh = IAPFD;
				Bandgap_Value = (BandgapHigh<<4)+BandgapLow;
				Bandgap_Voltage= Bandgap_Value*3/4;
		}
		if (BandgapMark==0x90)
		{
				IAPAL = 0x0E;
				IAPAH = 0x00;
				set_IAPGO;
				BandgapHigh = IAPFD;
				IAPAL = 0x0F;
				IAPAH = 0x00;
				set_IAPGO;
				BandgapLow = IAPFD;
				BandgapLow = BandgapLow&0x0F;
				Bandgap_Value = (BandgapHigh<<4)+BandgapLow;
				Bandgap_Voltage= Bandgap_Value*3/4;
		}
		clr_IAPEN;
      
//			printf ("\n BG High = %bX",BandgapHigh); 
//			printf ("\n BG Low = %bX",BandgapLow); 
//			printf ("\n BG ROMMAP = %e",Bandgap_Voltage); 
      
}



void adcscan (UINT8 icomindex) 
{  
   UINT8 stemp2[20]={0}; 
   UINT16 AIN_value=0;
   double iadc =0;
   
   
   //-------------BAND-----------------
   
   Enable_ADC_BandGap;	  
   CKDIV = 0x02;	

   
   clr_ADCF;
   set_ADCS;										 
   while(ADCF == 0);  

   BAND_value =  ADCRH ;
   BAND_value <<=  4 ;
   BAND_value |=  ADCRL & 0x0f ;      

   CKDIV = 0;	
   Disable_ADC;      
      
   switch (icomindex)
   {  
     
      case 0:      
         
         //-------------AIN4-----------------
         
         Enable_ADC_AIN4;	    
         
         clr_ADCF;
         set_ADCS;							 

         while(ADCF == 0);
         
         AIN_value =  ADCRH ;
         AIN_value <<=  4 ;
         AIN_value |=  ADCRL & 0x0f ;      
         
         iadc =   Bandgap_Voltage * AIN_value  / BAND_value ;  //mv        
         iadcvalue = (UINT16)iadc;
      
      
         iadcvalue_out = ( (INT16) (iadcvalue -  iadcvalue_delay) >>AVRBITCOUNT ) + iadcvalue_delay;
         iadcvalue_delay = iadcvalue_out; 


         flg_channel=1;
       
         Disable_ADC;
         
         break;
      
         
      default:

         break;
      
  }   
   
}


#define TIMER_DIV12_COUNT_10ms			65536-13334	
#define TIMER_DIV12_COUNT_3ms				65536-4000
#define TIMER_DIV12_COUNT_2ms				65536-2667
#define TIMER_DIV12_COUNT_1ms				65536-1334
#define TIMER_DIV12_COUNT_500us			65536-667
#define TIMER_DIV12_COUNT_100us			65536-133
#define TIMER_DIV12_COUNT_50us			65536-67


#define TIMER_DIV12_COUNT  TIMER_DIV12_COUNT_1ms	

void Timer1_ISR (void) interrupt 3 
{   
   static UINT8 iscanindex =0;  

   itimer1_count++;     
   itimer1_count2++;     
   //itimer1_count3++;     
   

   TL1 = TIMER_DIV12_COUNT & 0xFF; 		 
   TH1 = TIMER_DIV12_COUNT >> 8 ;		

   
   adcscan(iscanindex);   
   
   
   iscanindex=0;
   //if(iscanindex<4)
   //   iscanindex++;
   //else
   //   iscanindex=0;   
   
}

void timer1_init (void)
{
   
   TIMER1_MODE1_ENABLE;

   clr_T1M;      //  sysclk/12   
   //set_T1M;      //  sysclk

   TL1 = TIMER_DIV12_COUNT & 0xff; 		 
   TH1 = TIMER_DIV12_COUNT >> 8 ;	
   

   set_ET1;                                    
   set_EA;                                     

   set_TR1;     //Timer1 run
   
}


//void set_config(void)
//{
//   UINT8 sbuf2[10]={0};
//   //UINT8 stemp[20]={0}; 
//   
//   //set period measurement
//   sbuf2[0]=0; 
//   i2c_read(REG00, sbuf2, 1);    

//   sbuf2[0] |= (1<<0 )| (1<<1); 
//   
//   i2c_write(REG00, sbuf2, 1);
//   Timer0_Delay1ms(10); 
//   
//   //sbuf2[0]=0; 
//   //i2c_read(REG00, sbuf2, 1);    
//   //sprintf (stemp,"reg00 = %bu\r\n",sbuf2[0]); 
//   //iputs0(stemp);
//   //Timer0_Delay1ms(10);    
//   
//}

//void set_config2(void)
//{
//   UINT8 sbuf2[10]={0};
//   //UINT8 stemp[20]={0};
//    
//   //set period measurement
//   sbuf2[0] = VCNL3020_ReadOneByte(REG00);
//   sbuf2[0] |= (1<<0 )| (1<<1); 

//   VCNL3020_WriteOneByte(REG00, sbuf2[0]); 
//   Timer0_Delay1ms(10); 
//   
//   //sbuf2[0] = VCNL3020_ReadOneByte(REG00);   
//   //sprintf (stemp,"reg00 = %bu\r\n",sbuf2[0]); 
//   //iputs0(stemp);
//   //Timer0_Delay1ms(10);    
//       
//}


//void get_resultvalue1(void)
//{  
//   UINT8 sbuf1[10]={0};
//   UINT8 sbuf2[10]={0};   
//            
//   sbuf2[0]=0; sbuf2[1]=0; 
//   i2c_read(REG07, sbuf2, 1);   
//   i2c_read(REG08, sbuf2+1, 1);         
//   
//   iresultvalue1 = (UINT16)(sbuf2[0] *256 + sbuf2[1]);
//   sprintf (stemp12,"new1 = %u\r\n", iresultvalue1 );       

//   iresultvalue1_out = ( (INT16) (iresultvalue1 -  iresultvalue1_delay) >>AVRBITCOUNT ) + iresultvalue1_delay;
//   iresultvalue1_delay = iresultvalue1_out; 


//   //if(iresultcount1++>AVRREADYCOUNT)
//   //{
//   //   iresultcount1 =AVRREADYCOUNT+10;
//   //   iresultvalue1_avr = iresultvalue1_out;
//   //}
//   //else
//   //   iresultvalue1_avr = iresultvalue1;  
//   iresultvalue1_avr = iresultvalue1;

//   sprintf (stemp13,"avr1-R = %u\r\n", iresultvalue1_avr );     
//}      


//void get_resultvalue2(void)
//{  
//   UINT8 sbuf1[10]={0};
//   UINT8 sbuf2[10]={0};   
//            
//   sbuf2[0]=0; sbuf2[1]=0; 
//   
//   //i2c_read(REG07, sbuf2, 1);   
//   //i2c_read(REG08, sbuf2+1, 1);  
//   sbuf2[0]=VCNL3020_ReadOneByte(REG07);   
//   sbuf2[1]=VCNL3020_ReadOneByte(REG08);  
//   
//   
//   iresultvalue2 = (UINT16)(sbuf2[0] *256 + sbuf2[1]);
//   sprintf (stemp22,"new2 = %u\r\n", iresultvalue2 );       

//   iresultvalue2_out = ( (INT16) (iresultvalue2 -  iresultvalue2_delay) >>AVRBITCOUNT ) + iresultvalue2_delay;
//   iresultvalue2_delay = iresultvalue2_out; 


//   //if(iresultcount2++>AVRREADYCOUNT)
//   //{
//   //   iresultcount2 =AVRREADYCOUNT+10;
//   //   iresultvalue2_avr = iresultvalue2_out;
//   //}
//   //else
//   //   iresultvalue2_avr = iresultvalue2;      
//   iresultvalue2_avr = iresultvalue2;     

//   sprintf (stemp23,"avr2-L = %u\r\n", iresultvalue2_avr );     
//}      


#define AVERAGENUM    8 
#define BITMOVE16     3 
UINT16 get16_avr( UINT16 inew)
{
   static UINT8 icount =0;								
   static UINT32 isum=0;		
	
   static UINT16 iavr=0;	
   static UINT16 iold[AVERAGENUM] ={0};   

   
   if (icount<=AVERAGENUM)
      icount++;
   else
      icount=AVERAGENUM+1;


   if (icount<=AVERAGENUM)
   {								
      iavr=inew;
      isum = isum + inew;
      
      iold[icount-1] = inew;
   }
   else
   {								
      isum = isum + inew - iold[0];
      
      //iavr=(UINT16)(isum/AVERAGENUM);	  
      iavr=(isum>>BITMOVE16);	

      memcpy(&iold[0],&iold[1],(AVERAGENUM-1)*sizeof(UINT16));

      iold[(AVERAGENUM-1)] = inew;			
   }	

   return iavr;	
}  

UINT16 get16_avr2( UINT16 inew)
{
   static UINT8 icount =0;								
   static UINT32 isum=0;		
	
   static UINT16 iavr=0;	
   static UINT16 iold[AVERAGENUM] ={0};   

   
   if (icount<=AVERAGENUM)
      icount++;
   else
      icount=AVERAGENUM+1;


   if (icount<=AVERAGENUM)
   {								
      iavr=inew;
      isum = isum + inew;
      
      iold[icount-1] = inew;
   }
   else
   {								
      isum = isum + inew - iold[0];
      
      //iavr=(UINT16)(isum/AVERAGENUM);	  
      iavr=(isum>>BITMOVE16);	

      memcpy(&iold[0],&iold[1],(AVERAGENUM-1)*sizeof(UINT16));

      iold[(AVERAGENUM-1)] = inew;			
   }	

   return iavr;	
}  

//void get_resultvalue1(void)
//{  
//   UINT8 sbuf1[10]={0};
//   UINT8 sbuf2[10]={0};  

//   Init_I2C_P13_P14();      
//            
//   //get count0
//   sbuf2[0]=0; sbuf2[1]=0; 
//   i2c_read(0x21, sbuf2, 1);   
//   i2c_read(0x20, sbuf2+1, 1);        
//   
//   //clr_I2CEN;
//   
//   iresultvalue1 = (UINT16)(sbuf2[0] *256 + sbuf2[1]);
//   sprintf (stemp12,"new1 = %u\r\n", iresultvalue1 );   
// 
////iputs0(stemp12);   
//   

//   iresultvalue1_out = ( (INT16) (iresultvalue1 -  iresultvalue1_delay) >>AVRBITCOUNT ) + iresultvalue1_delay;
//   iresultvalue1_delay = iresultvalue1_out; 


//   //if(iresultcount1++>AVRREADYCOUNT)
//   //{
//   //   iresultcount1 =AVRREADYCOUNT+10;
//   //   iresultvalue1_avr = iresultvalue1_out;
//   //}
//   //else
//   //   iresultvalue1_avr = iresultvalue1;
//   
//   //iresultvalue1_avr = iresultvalue1;  
//   iresultvalue1_avr = get16_avr(iresultvalue1);     

//   sprintf (stemp13,"a1=%u\r\n", iresultvalue1_avr );   
//  
//}      

//void get_resultvalue2(void)
//{  
//   UINT8 sbuf1[10]={0};
//   UINT8 sbuf2[10]={0};  

//   Init_I2C_P02_P16();      
//            
//   //get count0
//   sbuf2[0]=0; sbuf2[1]=0; 
//   i2c_read(0x21, sbuf2, 1);   
//   i2c_read(0x20, sbuf2+1, 1);        
//   
//   //clr_I2CEN;
//   
//   iresultvalue2 = (UINT16)(sbuf2[0] *256 + sbuf2[1]);
//   sprintf (stemp22,"new2 = %u\r\n", iresultvalue2 );      
//   
////iputs0(stemp22); 
////iputs0("\r\n");     

//   iresultvalue2_out = ( (INT16) (iresultvalue2 -  iresultvalue2_delay) >>AVRBITCOUNT ) + iresultvalue2_delay;
//   iresultvalue2_delay = iresultvalue2_out; 


//   //if(iresultcount2++>AVRREADYCOUNT)
//   //{
//   //   iresultcount2 =AVRREADYCOUNT+10;
//   //   iresultvalue2_avr = iresultvalue2_out;
//   //}
//   //else
//   //   iresultvalue2_avr = iresultvalue2;  
//   
//   
//   //iresultvalue2_avr = iresultvalue2;   
//   iresultvalue2_avr = get16_avr2(iresultvalue2);   

//   sprintf (stemp23,"a2=%u\r\n", iresultvalue2_avr );   
//  
//}      

void get_LTA_value1(void)
{  
   UINT8 sbuf1[10]={0};
   UINT8 sbuf2[10]={0};  

   //Init_I2C_P13_P14();   
   
            
   //get LTA0
   sbuf2[0]=0; sbuf2[1]=0; 
   i2c_read(0x31, sbuf2, 1);   
   i2c_read(0x30, sbuf2+1, 1);        
   
   iLTA_value1 = (UINT16)(sbuf2[0] *256 + sbuf2[1]);
   sprintf (stemp11,"L1 = %u\r\n", iLTA_value1 ); 
   //iputs0(stemp11); 
   
}


void get_resultvalue1(void)
{  
   UINT8 sbuf1[10]={0};
   UINT8 sbuf2[10]={0};  

   //Init_I2C_P13_P14();   
   

   //get count0
   sbuf2[0]=0; sbuf2[1]=0; 
   i2c_read(0x21, sbuf2, 1);   
   i2c_read(0x20, sbuf2+1, 1);        
   
   iresultvalue1 = (UINT16)(sbuf2[0] *256 + sbuf2[1]);
   sprintf (stemp12,"new1 = %u\r\n", iresultvalue1 ); 
   //iputs0(stemp12);  
   
   
   //iresultvalue1_avr = iresultvalue1;  
   iresultvalue1_avr = get16_avr(iresultvalue1);     

   //sprintf (stemp13,"a1=%u\r\n", iresultvalue1_avr );   
   sprintf (stemp13,"L1,a1=%u,%u\r\n", iLTA_value1,iresultvalue1_avr );   
  
}   

void get_event1(void)
{  
   UINT8 sbuf1[10]={0};
   UINT8 sbuf2[10]={0};  
   

   //get count0
   sbuf2[0]=0; sbuf2[1]=0; 
   i2c_read(0x11, sbuf2, 1);   
   i2c_read(0x12, sbuf2+1, 1);        

   sprintf (stemp15,"ev1,pr1=%bx,%bx\r\n", sbuf2[0],sbuf2[1] );   
  
} 


void get_LTA_value2(void)
{  
   UINT8 sbuf1[10]={0};
   UINT8 sbuf2[10]={0};  

   //Init_I2C_P02_P16();      

   //get LTA0
   sbuf2[0]=0; sbuf2[1]=0; 
   i2c_read(0x31, sbuf2, 1);   
   i2c_read(0x30, sbuf2+1, 1);        
   
   iLTA_value2 = (UINT16)(sbuf2[0] *256 + sbuf2[1]);
   sprintf (stemp21,"L2 = %u\r\n", iLTA_value2 ); 
   //iputs0(stemp21);  
   
}


void get_resultvalue2(void)
{  
   UINT8 sbuf1[10]={0};
   UINT8 sbuf2[10]={0};  

   //Init_I2C_P02_P16();   
  
            
   //get count0
   sbuf2[0]=0; sbuf2[1]=0; 
   i2c_read(0x21, sbuf2, 1);   
   i2c_read(0x20, sbuf2+1, 1);        
   
   iresultvalue2 = (UINT16)(sbuf2[0] *256 + sbuf2[1]);
   sprintf (stemp22,"new2 = %u\r\n", iresultvalue2 );   
   //iputs0(stemp22); 
 
      
   //iresultvalue2_avr = iresultvalue2;   
   iresultvalue2_avr = get16_avr2(iresultvalue2);   

   //sprintf (stemp23,"a2=%u\r\n", iresultvalue2_avr );   
   sprintf (stemp23,"L2,a2=%u,%u\r\n", iLTA_value2,iresultvalue2_avr );   
  
}      

void get_event2(void)
{  
   UINT8 sbuf1[10]={0};
   UINT8 sbuf2[10]={0};  
   

   //get count0
   sbuf2[0]=0; sbuf2[1]=0; 
   i2c_read(0x11, sbuf2, 1);   
   i2c_read(0x12, sbuf2+1, 1);   
 
   sprintf (stemp25,"ev2,pr2=%bx,%bx\r\n", sbuf2[0],sbuf2[1] );   
  
}

#define     CFG_READ            0xC0
#define     CFG_ERASE           0xE2
#define     CFG_BYTE_PROGRAM    0xE1 
void Enable_WDT_Reset_Config(void)
{
   set_IAPEN;
   set_CFUEN;
   
   IAPAL = 0x04;
   IAPAH = 0x00;
   IAPFD = 0x0F;
   IAPCN = CFG_BYTE_PROGRAM;
   set_IAPGO;  
   
   //trigger IAP
   while((CHPCON&SET_BIT6)==SET_BIT6);          //check IAPFF (CHPCON.6)
   
   clr_CFUEN;
   clr_IAPEN;
      
}

void gpio_init(void)
{
   
   //key init   
   KEY1_L_PIN=0;
   KEY2_CENTER_PIN=0;
   KEY3_DOWN_PIN=0;
   KEY4_R_PIN=0;
   KEY5_UP_PIN=0;
   
   KEY1_L_PUSHPULL_MODE;
   KEY2_CENTER_PUSHPULL_MODE;
   KEY3_DOWN_PUSHPULL_MODE;
   KEY5_UP_PUSHPULL_MODE;
   KEY4_R_PUSHPULL_MODE;
   
   
   //sensor level init
   SENSOR_R_PIN=0;
   SENSOR_L_PIN=0;
   
   SENSOR_R_PUSHPULL_MODE;
   SENSOR_L_PUSHPULL_MODE;
   
}


void set_config(void)
{
   UINT8 sbuf2[10]={0};
   
   //set ATI 
   sbuf2[0]=0; 
   i2c_read(0x43, sbuf2, 1);    

   sbuf2[0] &= ~( (1<<0 )| (1<<1) ); 
   
   i2c_write(0x43, sbuf2, 1);
   Timer0_Delay1ms(10);  


   //set Prox threshold
   sbuf2[0] = 0x16; 
   
   i2c_write(0x60, sbuf2, 1);
   Timer0_Delay1ms(10);     
   
   
   //set Hysteresis
   sbuf2[0]=0; 
   i2c_read(0x80, sbuf2, 1);    

   sbuf2[0] &= ~( (1<<0 )| (1<<1) ); 
   sbuf2[0] += 1;    //Hysteresis 1/4
      
   i2c_write(0x80, sbuf2, 1);
   Timer0_Delay1ms(10);     
   
   
}

void set_config2(void)
{
   UINT8 sbuf2[10]={0};
   
   //set ATI =00
   sbuf2[0]=0; 
   i2c_read(0x43, sbuf2, 1);    

   sbuf2[0] &= ~( (1<<0 )| (1<<1) ); 
   
   i2c_write(0x43, sbuf2, 1);
   Timer0_Delay1ms(10);   
   
   
   //set Prox threshold
   sbuf2[0] = 0x16; 
   
   i2c_write(0x60, sbuf2, 1);
   Timer0_Delay1ms(10);     
   
   
   //set Hysteresis
   sbuf2[0]=0; 
   i2c_read(0x80, sbuf2, 1);    

   sbuf2[0] &= ~( (1<<0 )| (1<<1) ); 
   sbuf2[0] += 1;    //Hysteresis 1/4
      
   i2c_write(0x80, sbuf2, 1);
   Timer0_Delay1ms(10);      
   
}

#define UARTDELAYTIMES   5
void main (void) 
{
   UINT8 sbuf1[10]={0};
   UINT8 sbuf2[10]={0};
   UINT8 stemp[20]={0};  
    
   
   gpio_init();
   
   
   //uart0 init
   InitialUART0_Timer3(115200);
   TI = 1;
   //set_ES;					//For interrupt enable
   set_EA;	

   iputs0("start................\r\n");
      
   
   Timer0_Delay1ms(10);   

   
   //adc bandgap
   READ_BANDGAP();   
   sprintf (stemp,"band vol = %6.2f\r\n",Bandgap_Voltage); 
   //--iputs0(stemp);      


   //--- sensor init ---   


   Timer0_Delay1ms(1000);
   
   flg_sensor_init =1;
      
   //get Bound1    
  
   Init_I2C_P13_P14();
   {
      set_config(); 
      get_LTA_value1(); 
      get_resultvalue1(); 
   }   
   
   if(iLTA_value1 >=RESULT_LOWEST)
   {
      //RESULT1_COMEOUT = iLTA_value1 - CENTER_RANGE  + PROX_GAP ;
      //RESULT1_COMEIN = iLTA_value1 - CENTER_RANGE  - PROX_GAP ;  
      RESULT1_COMEOUT = iLTA_value1 - PROX_OUT_GAP ;
      RESULT1_COMEIN = iLTA_value1 - PROX_IN_GAP ;  
      
   }
   else
   {
      RESULT1_COMEOUT = 0;
      RESULT1_COMEIN = 0;   
      
      flg_sensor_init =0;

      iputs0("sensor1 init err\r\n"); 
      Timer0_Delay1ms(10);      
   }

   iputs0(stemp13); Timer0_Delay1ms(10); 

   //sprintf (stemp,"up1,low1 = %u,%u\r\n",RESULT1_COMEOUT,RESULT1_COMEIN); 
   //iputs0(stemp);     
   //Timer0_Delay1ms(10);
   

   //get Bound2 
   Init_I2C_P02_P16();
   {
      set_config2();
      
      get_LTA_value2(); 
      get_resultvalue2();
   }   

   if(iLTA_value2 >=RESULT_LOWEST)
   {
      //RESULT2_COMEOUT = iLTA_value2 - CENTER_RANGE + PROX_GAP ;
      //RESULT2_COMEIN = iLTA_value2 - CENTER_RANGE - PROX_GAP ;
      RESULT2_COMEOUT = iLTA_value2 - PROX_OUT_GAP ;
      RESULT2_COMEIN = iLTA_value2 - PROX_IN_GAP ;
   }
   else
   {
      RESULT2_COMEOUT = 0;
      RESULT2_COMEIN = 0;   
      
      flg_sensor_init =0;

      iputs0("sensor2 init err\r\n"); 
      Timer0_Delay1ms(10);      
   }   
   iputs0(stemp23); Timer0_Delay1ms(10); 

   //sprintf (stemp,"up2,low2 = %u,%u\r\n",RESULT2_COMEOUT, RESULT2_COMEIN); 
   //iputs0(stemp); 
   //Timer0_Delay1ms(10);    
   
   iputs0("\r\n"); Timer0_Delay1ms(2); 

      
   //timer1 init
   timer1_init();     
   
   //WDT init
   Enable_WDT_Reset_Config();
   set_WDTR;                        
   set_WDCLR;	   


   //P17_PushPull_Mode;  //--test


   while(1)
   {		

      //--static UINT32 imaincount =0 ; 
      
      set_WDCLR;      

//iadc =   Bandgap_Voltage * AIN_value  / BAND_value ;  //mv        
//iadcvalue = (UINT16)iadc;      
      
      
      if(flg_channel==1)
      {
         flg_channel=0;       
         
         //if(iadccount++>AVRREADYCOUNT)
         //{
         //   iadccount =AVRREADYCOUNT+10;
         //   iadcvalue_avr = iadcvalue_out;            
         //}
         //else         
         //   iadcvalue_avr = iadcvalue;
         iadcvalue_avr = iadcvalue;
 
         
         iadc_key = (UINT16)((double)BAND_value / BAND_3300 * iadcvalue_avr);
         
         //sprintf( sadctemp1, "BAND: %u\r\n", BAND_value); 
         sprintf( sadctemp1, "adc: %u\r\n", iadc_key); 
         sprintf( sadctemp2, "BAND: %u, iadc: %u\r\n", BAND_value,iadcvalue_avr);          
         
         
         if(iadc_key <= ADC_VOL_NC )
         {
            Timer0_Delay1ms(10); 

            if(iadc_key <= ADC_VOL_NC )
            {
               //--if(KEY0_NC_PIN==0) {iputs0(sadctemp1); Timer0_Delay1ms(2); iputs0("no key\r\n"); Timer0_Delay1ms(2); }               
               sprintf( sadcstate, "no key\r\n"); 
               
               KEY0_NC_PIN = 1;
               KEY1_L_PIN = 0;
               KEY2_CENTER_PIN = 0;
               KEY3_DOWN_PIN = 0;
               KEY4_R_PIN = 0;
               KEY5_UP_PIN = 0;
               
            }               
         }
         else if(iadc_key >= ADC_VOL_NC2 )
         {
            Timer0_Delay1ms(10); 

            if(iadc_key >= ADC_VOL_NC2 )
            {
               //--if(KEY0_NC_PIN==0) {iputs0(sadctemp1); Timer0_Delay1ms(2); iputs0("no key2\r\n"); Timer0_Delay1ms(2); }
               sprintf( sadcstate, "no key2\r\n");
               
               KEY0_NC_PIN = 1;
               KEY1_L_PIN = 0;
               KEY2_CENTER_PIN = 0;
               KEY3_DOWN_PIN = 0;
               KEY4_R_PIN = 0;
               KEY5_UP_PIN = 0;
               
            }               
         }         
         
         else if (iadc_key >= ADC_VOL_R )
         {
            Timer0_Delay1ms(10); 

            if(iadc_key >= ADC_VOL_R )
            {
               if(KEY4_R_PIN==0)  {iputs0(sadctemp1); Timer0_Delay1ms(2); iputs0("right key\r\n"); Timer0_Delay1ms(2); }
               sprintf( sadcstate, "right key\r\n");
            
               KEY0_NC_PIN = 0;
               KEY1_L_PIN = 0;
               KEY2_CENTER_PIN = 0;
               KEY3_DOWN_PIN = 0;
               KEY5_UP_PIN = 0;
               KEY4_R_PIN = 1;  
               
            }               
         }

         else if (iadc_key >= ADC_VOL_DOWN )
         {
            Timer0_Delay1ms(10); 

            if(iadc_key >= ADC_VOL_DOWN )
            {
               if(KEY3_DOWN_PIN==0)  {iputs0(sadctemp1); Timer0_Delay1ms(2); iputs0("down key\r\n"); Timer0_Delay1ms(2); }
               sprintf( sadcstate, "down key\r\n");

               KEY0_NC_PIN = 0;
               KEY1_L_PIN = 0;
               KEY2_CENTER_PIN = 0;
               KEY3_DOWN_PIN = 1;
               KEY4_R_PIN = 0;  
               KEY5_UP_PIN = 0;
               
            }               
         }     
         else if(iadc_key >= ADC_VOL_L )
         {
            Timer0_Delay1ms(10); 

            if(iadc_key >= ADC_VOL_L )
            {
               if(KEY1_L_PIN==0) {iputs0(sadctemp1); Timer0_Delay1ms(2); iputs0("left key\r\n"); Timer0_Delay1ms(2); }
               sprintf( sadcstate, "left key\r\n");
               
               KEY0_NC_PIN = 0;
               KEY1_L_PIN = 1;
               KEY2_CENTER_PIN = 0;
               KEY3_DOWN_PIN = 0;
               KEY4_R_PIN = 0;  
               KEY5_UP_PIN = 0;
               
            }               
         } 
         else if (iadc_key >= ADC_VOL_CENTER )
         {
            Timer0_Delay1ms(10);  

            if(iadc_key >= ADC_VOL_CENTER )
            {
               if(KEY2_CENTER_PIN==0)  { iputs0(sadctemp1); Timer0_Delay1ms(2); iputs0("center key\r\n"); Timer0_Delay1ms(2);}
               sprintf( sadcstate, "center key\r\n");

               KEY0_NC_PIN = 0;
               KEY1_L_PIN = 0;
               KEY2_CENTER_PIN = 1;
               KEY3_DOWN_PIN = 0;
               KEY4_R_PIN = 0;   
               KEY5_UP_PIN = 0;
               
            }               
         }         
         else if (iadc_key >= ADC_VOL_UP )
         {
            Timer0_Delay1ms(10); 

            if(iadc_key >= ADC_VOL_UP )
            {
               if(KEY5_UP_PIN==0)  {iputs0(sadctemp1); Timer0_Delay1ms(2); iputs0("up key\r\n"); Timer0_Delay1ms(2); }
               sprintf( sadcstate, "up key\r\n");

               //KEY0_NC_PIN = 0;
               KEY1_L_PIN = 0;
               KEY2_CENTER_PIN = 0;
               KEY3_DOWN_PIN = 0;
               KEY4_R_PIN = 0;   
               KEY5_UP_PIN = 1;
               
            }               
         }         
          
      }    

      
      //get resultvalue
      if(itimer1_count>=200)
      {
         itimer1_count=0;   

         iltascan++;          
         
         if (iltascan>=50)
         {

            iltascan=0;
            
            //get LTA value1          
            Init_I2C_P13_P14();
            {
               get_LTA_value1(); 
            }

            //get LTA value2 
            Init_I2C_P02_P16();
            {
               get_LTA_value2(); 
            }  
         }
         else
         {
            
            //get the result value1          
            Init_I2C_P13_P14();
            {
               get_resultvalue1(); 
               get_event1(); 
            }

            //get the result value2 
            Init_I2C_P02_P16();
            {
               get_resultvalue2();
               get_event2(); 
            }    
            
//            if( (flg_sensor_init==1) && (iresultvalue1>0) && (iresultvalue2>0) )
//            {

//               //result value1 check
//               if (iresultvalue1_avr >= RESULT1_COMEOUT)  
//                  {SENSOR_R_PIN=0;   }
//                  
//               //else if ( (iresultvalue1_avr <= RESULT1_COMEIN) && (oldvalue1[1] <= RESULT1_COMEIN) && (oldvalue1[2] <= RESULT1_COMEIN) )  
//               else if (iresultvalue1_avr <= RESULT1_COMEIN)    
//                  {SENSOR_R_PIN=1; }
//                  //{SENSOR_R_PIN=1; iputs0(stemp13); iputs0("prox1 in\r\n"); }
//                  

//               //result value2 check
//               if (iresultvalue2_avr >= RESULT2_COMEOUT)
//                  {SENSOR_L_PIN=0;  }
//                  
//               //else if ( (iresultvalue2_avr <= RESULT2_COMEIN) && (oldvalue2[1] <= RESULT2_COMEIN) && (oldvalue2[2] <= RESULT2_COMEIN) )  
//               else if (iresultvalue2_avr <= RESULT2_COMEIN)
//                  {SENSOR_L_PIN=1; }         
//                   //{SENSOR_L_PIN=1; iputs0(stemp23); iputs0("prox2 in\r\n");}    
//               
//            }

         }            
                  
      }       

      
      if( (itimer1_count2>=2000)  && (itimer1_count>=20) )
      {
         itimer1_count2=0;           
          
//         ////adc
//         //iputs0(sadctemp1); 
//         //Timer0_Delay1ms(UARTDELAYTIMES); 

//         //iputs0(sadctemp2); 
//         //Timer0_Delay1ms(UARTDELAYTIMES); 

//         //iputs0(sadcstate); 
//         //Timer0_Delay1ms(UARTDELAYTIMES); 

//         //iputs0 ("\r\n" );       
//         //Timer0_Delay1ms(UARTDELAYTIMES);  

                  
         ////i2c
         //iputs0(stemp11); 
         //Timer0_Delay1ms(UARTDELAYTIMES); 

         //iputs0(stemp12); 
         //Timer0_Delay1ms(UARTDELAYTIMES); 
         
         iputs0(stemp13); 
         Timer0_Delay1ms(UARTDELAYTIMES);  
         
         iputs0(stemp15); 
         Timer0_Delay1ms(UARTDELAYTIMES);  
         
         
         //iputs0(stemp21); 
         //Timer0_Delay1ms(UARTDELAYTIMES); 

         //iputs0(stemp22); 
         //Timer0_Delay1ms(UARTDELAYTIMES); 
         
         iputs0(stemp23); 
         Timer0_Delay1ms(UARTDELAYTIMES);          
         
         iputs0(stemp25); 
         Timer0_Delay1ms(UARTDELAYTIMES);          
         
         iputs0 ("\r\n" );       
         Timer0_Delay1ms(2);
      }  
      
     
//      if( (itimer1_count2>=2000)  && (itimer1_count>=20) )
//      {
//         itimer1_count2=0;  
//                
//         iputs0(stemp13); 
//         Timer0_Delay1ms(UARTDELAYTIMES);  

//         iputs0(stemp23); 
//         Timer0_Delay1ms(UARTDELAYTIMES);      

//         iputs0 ("\r\n" );       
//         Timer0_Delay1ms(2);         
//      }    
      
   }   
		
}


