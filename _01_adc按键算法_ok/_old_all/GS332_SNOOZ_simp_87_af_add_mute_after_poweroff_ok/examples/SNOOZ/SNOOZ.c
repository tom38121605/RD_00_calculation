
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"


//white noise

//#define NOISEVOLUMETOP 16
#define NOISEVOLUMETOP 20

UINT8 flg_newnoise = 0; 
UINT8 inoisevolume =0;
UINT8 inoisevolume_top = 0;


//key 

//#define LONGKEYCOUNT2_PWM0     75     //3.6s
//#define LONGKEYCOUNT2_PWM1_6     150    //3.6s
//#define LONGKEYCOUNT2_NOPWM      180    //3.6s

//UINT32 LONGKEYCOUNT2 = 150;    //fixed 3.0s+ 
UINT32 LONGKEYCOUNT2 = 90;    


UINT8 flg_keydown1 =0;
UINT8 flg_keydown2 =0;
UINT8 flg_keydown3 =0;

UINT8 ikeyvalue1 =0;
UINT8 ikeyvalue2 =0;
UINT8 ikeyvalue3 =0;

UINT32 ikeynum1 =0;
UINT32 ikeynum2 =0;
UINT32 ikeynum3 =0;

UINT8 flg_keycome1 =0;
UINT8 flg_keycome2 =0;
UINT8 flg_keycome3 =0;

 
UINT8 flg_keydown10 =0;
UINT8 flg_keyvalue10 =0;
UINT8 flg_power =0;

#define ADC_VOL_GAP                   200   

#define ADC_VOLDEC                    (1200 - ADC_VOL_GAP)
#define ADC_VOLADD                    (1750 - ADC_VOL_GAP)   
#define ADC_TONE                      (2075 - ADC_VOL_GAP)   //建议改成1k电阻，这里为2242  -> 2250

#define ADC_VOL_NC                  300   //(ADC_VOL_L  - ADC_VOL_GAP)    
#define ADC_VOL_NC2                 3000  //(ADC_VOL_UP + ADC_VOL_GAP)    


//pwm

UINT8  flg_openpwm =0;
//UINT32 ipwmcount = 0;
UINT8 ipwm_mode=0;


//#define TIMER1_RELOAD_VALUE     84
//#define PWM_PERIOD 	0x1ff


static S16 Harm_Val = 0x00;		// State for harmonic generator
static UINT16 LFSR = 0xff;			// Any non-zero value will work as the seed
static S16 LPF, L_OUT, S_OUT, LSB = 0x100;
static U8   CTR_DIR = 0; 
static U8   FN_MODE = 1;
static U8   WN_MODE = 1;


//static U8 LPF_K = 5;
//static U8 HARM_K = 7;
//static U8 NOISE_K = 7;
//static U8 INCR = 16;


static U8 LPF_K = 5;
static U8 HARM_K = 7;
static U8 NOISE_K = 1;
static U8 INCR = 0;



//BT command 

UINT8 spoweron[5]  = {0xA5, 0x03, 0x84, 0x01, 0x2d};
UINT8 spoweroff[5] = {0xA5, 0x03, 0x84, 0x03, 0x2f};

UINT8 splaypause[5] = {0xA5, 0x03, 0x82, 0x03, 0x2d};
UINT8 svoladd[5]    = {0xA5, 0x03, 0x8b, 0x20, 0x53};
UINT8 svoldec[5]    = {0xA5, 0x03, 0x8b, 0x21, 0x54};

UINT8 spairing[6] =   {0xA5, 0x04, 0x82, 0x07, 0x01, 0x33}; 
UINT8 sBTAUX_Switch[5] = {0xA5, 0x03, 0x8c, 0x20, 0x54};


//UINT8 svoicemode[5]={ };
UINT8 flg_voicemode =0;     //0 -- BT, 1 -- AUX


//timer1

UINT32 itimer1_count1=0;
UINT32 itimer1_count2=0;


//adc

UINT16 BAND_value=0;

#define BAND_3300  1526  

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



void Switch_Delay1ms( UINT32 inum)
{
   UINT32 itemp =0;
   
   if(flg_openpwm==0)
   {
      
      //LONGKEYCOUNT2 = LONGKEYCOUNT2_NOPWM;
      
      itemp=inum;      
      Timer0_Delay1ms(itemp);
   }
   else
   {
      //LONGKEYCOUNT2 = LONGKEYCOUNT2_PWM;
      
      itemp=inum;      
      Timer0_Delay1ms(itemp);
   }
}

//void Switch_Delay1ms( UINT32 inum)   //keep
//{
//   UINT32 itemp =0;
//   
//   if(flg_openpwm==0)
//   {    
//      //LONGKEYCOUNT1 = LONGKEYCOUNT1_NOPWM;
//      //LONGKEYCOUNT2 = LONGKEYCOUNT2_NOPWM;
//      
//      itemp=inum;
//      Timer0_Delay1ms(itemp);
//   }
//   else
//   {
//      switch (ipwm_mode)
//      {
//         
//         case 0: 

//               //LONGKEYCOUNT1 = LONGKEYCOUNT1_PWM;
//               //LONGKEYCOUNT2 = LONGKEYCOUNT2_PWM;
//         
//               //itemp=inum>>3;
//               itemp=inum;
//               Timer0_Delay1ms(itemp); 
//               break;          
//         
//         case 1: 

//               //LONGKEYCOUNT1 = LONGKEYCOUNT1_NOPWM;
//               //LONGKEYCOUNT2 = LONGKEYCOUNT2_NOPWM;
//         
//               itemp=inum;
//               Timer0_Delay1ms(itemp); 
//               break;         
//         
//         case 2: 

//               //LONGKEYCOUNT1 = LONGKEYCOUNT1_NOPWM;
//               //LONGKEYCOUNT2 = LONGKEYCOUNT2_NOPWM;
//         
//               itemp=inum;
//               Timer0_Delay1ms(itemp); 
//               break;         
//         
//         case 3: 

//               //LONGKEYCOUNT1 = LONGKEYCOUNT1_NOPWM;
//               //LONGKEYCOUNT2 = LONGKEYCOUNT2_NOPWM;
//         
//               itemp=inum;
//               Timer0_Delay1ms(itemp); 
//               break;    
//         
//         case 4: 
//         
//               //LONGKEYCOUNT1 = LONGKEYCOUNT1_NOPWM;
//               //LONGKEYCOUNT2 = LONGKEYCOUNT2_NOPWM;

//               itemp=inum;
//               Timer0_Delay1ms(itemp); 
//               break;    
//         
//         case 5: 

//               //LONGKEYCOUNT1 = LONGKEYCOUNT1_NOPWM;
//               //LONGKEYCOUNT2 = LONGKEYCOUNT2_NOPWM;
//         
//               itemp=inum;
//               Timer0_Delay1ms(itemp); 
//               break;    
//         
//         case 6: 

//               //LONGKEYCOUNT1 = LONGKEYCOUNT1_NOPWM;
//               //LONGKEYCOUNT2 = LONGKEYCOUNT2_NOPWM;
//         
//               itemp=inum;
//               Timer0_Delay1ms(itemp); 
//               break;    
//         
//         default:          
//         
//               break;  
//         
//      }
//      
//   }
//}


void count_Delay_ms(UINT32 ms)  //16M
{
   UINT32 j;
   
   while(ms--)
   {
      
      //for(j = 0; j<173; j++)   
      for(j = 0; j<76; j++)   
      {
         _nop_();
         _nop_();
         _nop_();
         _nop_();
      }
   }
   
}

#define UART_LOG 
void iputs0(UINT8 *msg)
{
   #ifdef UART_LOG   
   while(*msg)
		 Send_Data_To_UART0(*msg++);
   
   #endif
}

void iputs0B(UINT8 *msg)
{
   while(*msg)
		 Send_Data_To_UART0(*msg++);
}


void iputbytes0(UINT8 *msg, UINT32 ilen)
{
   while(ilen--)
		 Send_Data_To_UART0(*msg++);
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


UINT8 get_checksum_8(UINT8 *ival, UINT32 inum)
{
   UINT32 i =0;
   UINT8 iret =0;
   UINT16 isum =0;
   
   for (i=0; i<inum; i++)
   {
      isum += ival[i];      
   }
   
   iret = (UINT8)(isum & 0xff);
   
   return iret;
   
}


void set_realvolume(UINT8 ivol)
{
   UINT8 icmd[5]={0xa5, 0x03, 0x8b, 0, 0 };
   
   icmd[3] = ivol;
   icmd[4] = get_checksum_8(icmd,4); 
   
   iputbytes0(icmd,5);
   
}



void set_volume(UINT8 ivol)
{
   UINT8 icmd[5]={0xa5, 0x03, 0x8b, 0, 0 };
   
   if(ivol >10)
      ivol =10;
   
   switch (ivol)
   {
      case 0:
         
         icmd[3] = 0;
         break;
      
      case 1:
         
         icmd[3] = 4;
         break;
      
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
         
         icmd[3] = 4 + (ivol-1) *3 ;
         break;      
       
      default:

         break;
      
   }
   icmd[4] = get_checksum_8(icmd,4); 
   
   iputbytes0(icmd,5);   
}



//void getadcvalue (UINT8 icomindex) 
//{  
//   UINT8 stemp2[20]={0}; 
//   UINT16 AIN_value=0;
//   double iadc =0;
//   
//   
//   //-------------BAND-----------------
//   
//   Enable_ADC_BandGap;	  
//   CKDIV = 0x02;	

//   
//   clr_ADCF;
//   set_ADCS;										 
//   while(ADCF == 0);  

//   BAND_value =  ADCRH ;
//   BAND_value <<=  4 ;
//   BAND_value |=  ADCRL & 0x0f ;      

//   CKDIV = 0;	
//   Disable_ADC;      
//      
//   switch (icomindex)
//   {  
//     
//      case 0:      
//         
//         //-------------AIN5-----------------
//         
//         Enable_ADC_AIN5;	    
//         
//         clr_ADCF;
//         set_ADCS;							 

//         while(ADCF == 0);
//         
//         AIN_value =  ADCRH ;
//         AIN_value <<=  4 ;
//         AIN_value |=  ADCRL & 0x0f ;      
//         
//         iadc =   Bandgap_Voltage * AIN_value  / BAND_value;  //mv        
//         iadcvalue = (UINT16)iadc;
//      
//      
//         //iadcvalue_out = ( (INT16) (iadcvalue -  iadcvalue_delay) >>AVRBITCOUNT ) + iadcvalue_delay;
//         //iadcvalue_delay = iadcvalue_out; 
//      

//         flg_channel=1;
//       
//         Disable_ADC;
//         
//         break;
//      
//         
//      default:

//         break;
//      
//  }   
//   
//}


void getadcvalue () 
{  
   UINT8 stemp2[20]={0}; 
   UINT16 AIN_value=0;
   double iadc =0;
   
   
   //-------------BAND-----------------
   
   Enable_ADC_BandGap;	  
   //CKDIV = 0x02;	

   
   clr_ADCF;
   set_ADCS;										 
   while(ADCF == 0);  

   BAND_value =  ADCRH ;
   BAND_value <<=  4 ;
   BAND_value |=  ADCRL & 0x0f ;      

   //CKDIV = 0;	
   Disable_ADC;      
      
 
   //-------------AIN5-----------------
   
   Enable_ADC_AIN5;	    
   
   clr_ADCF;
   set_ADCS;							 

   while(ADCF == 0);
   
   AIN_value =  ADCRH ;
   AIN_value <<=  4 ;
   AIN_value |=  ADCRL & 0x0f ;      
   
   //iadc =   Bandgap_Voltage * AIN_value  / BAND_value;     //mv        
   iadc =   Bandgap_Voltage * AIN_value  /  BAND_3300;     //mv        
   iadcvalue = (UINT16)iadc;


   //iadcvalue_out = ( (INT16) (iadcvalue -  iadcvalue_delay) >>AVRBITCOUNT ) + iadcvalue_delay;
   //iadcvalue_delay = iadcvalue_out; 

   flg_channel=1;
 
   Disable_ADC;      
   
}


//#define TIMER_DIV12_COUNT_10ms			65536-13334	
//#define TIMER_DIV12_COUNT_3ms				65536-4000
//#define TIMER_DIV12_COUNT_2ms				65536-2667
//#define TIMER_DIV12_COUNT_1ms				65536-1334
//#define TIMER_DIV12_COUNT_500us			65536-667
//#define TIMER_DIV12_COUNT_100us			65536-133
//#define TIMER_DIV12_COUNT_50us			65536-67


//#define TIMER_DIV12_COUNT  TIMER_DIV12_COUNT_1ms	

//void Timer1_ISR (void) interrupt 3 
//{   
//   static UINT8 iscanindex =0;  

//   itimer1_count1++;     
//   itimer1_count2++;     
//   //itimer1_count3++;     
//   

//   TL1 = TIMER_DIV12_COUNT & 0xFF; 		 
//   TH1 = TIMER_DIV12_COUNT >> 8 ;		

//   
//   //getadcvalue(iscanindex);   
//   getadcvalue();   
//   
//   
//   iscanindex=0;
//   //if(iscanindex<4)
//   //   iscanindex++;
//   //else
//   //   iscanindex=0;   
//   
//}

//void timer1_init (void)
//{
//   
//   TIMER1_MODE1_ENABLE;

//   clr_T1M;      //  sysclk/12   
//   //set_T1M;      //  sysclk

//   TL1 = TIMER_DIV12_COUNT & 0xff; 		 
//   TH1 = TIMER_DIV12_COUNT >> 8 ;	
//   

//   set_ET1;                                    
//   set_EA;                                     

//   set_TR1;     //Timer1 run
//   
//}


static void Modify_HIRC_Value(void)      
{
   bit BIT_TMP;
   unsigned char hircmap0,hircmap1;
   unsigned int trimvalue16bit;
   set_IAPEN;
   IAPAL = 0x30;
   IAPAH = 0x00;
   IAPCN = 0X04;
   set_IAPGO;
   hircmap0 = IAPFD;
   IAPAL = 0x31;
   IAPAH = 0x00;
   set_IAPGO;
   hircmap1 = IAPFD;
   clr_IAPEN;
   trimvalue16bit = ((hircmap0<<1)+(hircmap1&0x01));
   trimvalue16bit = trimvalue16bit - 15;
   hircmap1 = trimvalue16bit&0x01;
   hircmap0 = trimvalue16bit>>1;
   TA=0XAA;
   TA=0X55;
   RCTRIM0 = hircmap0;
   TA=0XAA;
   TA=0X55;
   RCTRIM1 = hircmap1;

}

void pwm_init(void)
{  
   
   // set up IO
   P05_PushPull_Mode;
   
   //set up PWM2, this will be the noise source DAC
   PWM2_P05_OUTPUT_ENABLE;
   PWM_INT_PWM2;   
   PWM_FALLING_INT;                   //Setting Interrupt happen when PWM0 falling signal

   PWM_CLOCK_DIV_2;
   PWMPH = 0x01;                     //Setting PWM period
   PWMPL = 0xFF;
   
   PWM2H = 0x00;                     //PWM2, duty = 1/2 PWM period
   PWM2L = 0xFF;   
   
   //set priority  3
   EIP  |= (1<<3);   
   EIPH |= (1<<3);       

   set_EPWM;                         //Enable PWM interrupt
   set_EA;                           
   set_LOAD;
   //set_PWMRUN;

}  


void PWM_ISR (void) interrupt 13
{
   clr_PWMF;                                                 // clear the interrupt flag   
   //P02 = 1;                                                  // just an I/O marker to show activity   
  
   // Harmonics generator   
   if(CTR_DIR)
   {
      Harm_Val = (Harm_Val + INCR);
      if (Harm_Val >= 0x1FE)                                 // ramped to max?
      {   
         Harm_Val = 0x1FE;
         CTR_DIR = 0;
      }
   }
   else
   {
      Harm_Val = (Harm_Val - INCR);
      if (Harm_Val <= 0x0)                                    // ramped to zero?
      {
         Harm_Val = 1;
         CTR_DIR = 1;   
      }
   }

   // White noise generator (Galois LFSR)
   LSB = LFSR & 0x01u;                                         // Get LSB (i.e., the output bit)
   LFSR = (UINT16)LFSR >> 1;                                   // Shift register 
   if (LSB)                                                    // If the output bit is 1, 
         LFSR ^= 0xB400u;                                      // Apply polynomial feedback. 
            
   // Sum the noise and harmonics and apply the gain
   L_OUT = (S16)(((LFSR >> (NOISE_K + 7u))));               // kN . N
   S_OUT = (S16)((Harm_Val >> HARM_K));                     // kH . H
   L_OUT = (S16)(L_OUT + S_OUT);                            // kH.H + kN.N

   // Low Pass Filter (1st order IIR) 
   L_OUT = (S16)(L_OUT - LPF);                              // x[n] - y[n-1]//
   L_OUT = (S16)(L_OUT >> LPF_K);                           // k(x[n] - y[n-1])
   L_OUT = (S16)(L_OUT + LPF);                              // k(x[n] - y[n-1]) + y[n-1]
   LPF = L_OUT;                                             // y[n] => y[n-1]
   
         
   // set size and range of output data, exceeding range will crash the HW   
   L_OUT = (UINT16)(L_OUT & 0x1ff);
   if(L_OUT == 0)
      L_OUT = 0x0001;
   if(L_OUT == 0x01ff)
      L_OUT = 0x01fe;      
            
   // DAC output            
   PWM2H = (UINT16)(L_OUT >> 8) & 0x01;               // high byte   
   PWM2L = L_OUT & 0xff;                              // low byte
   set_LOAD;                                          // latch the updated PWM value to the PWM engine
   
   //P02 = 0;                                           // just an I/O marker to show activity
           
}

//white noise switch
void whitenoise_switch(void)
{
 
   switch(WN_MODE)
   {
      
      //from WN_MODE
      case 1:
      {
         LPF_K = 5;
         HARM_K = 7;
         NOISE_K = 1;
         INCR = 0;      
         break;                     
      }
      case 2:
      {
         LPF_K = 3;
         HARM_K = 7;
         NOISE_K = 2;
         INCR = 0;      
         break;                     
      }
      case 3:
      {
         LPF_K = 1;
         HARM_K = 7;
         NOISE_K = 3;
         INCR = 0;      
         break;                     
      }
      
      
      //from FN_MODE
      case 4:
      {
         LPF_K = 6;
         HARM_K = 1;
         NOISE_K = 1;
         INCR = 5;      
         break;                     
      }
      case 5:
      {
         LPF_K = 4;
         HARM_K = 3;
         NOISE_K = 2;
         INCR = 6;      
         break;                     
      }
      case 6:
      {
         LPF_K = 2;
         HARM_K = 4;
         NOISE_K = 3;
         INCR = 7;      
         break;                     
      }           
      
   }
 
         
}  

void get_volumetop(UINT8 imode)
{

      switch (imode)
      {
         
         case 1: 
            inoisevolume_top = NOISEVOLUMETOP +5+5;  
            break;
         
         case 2: 
            inoisevolume_top = NOISEVOLUMETOP +3;   
            break;
         
         case 3: 
            inoisevolume_top = NOISEVOLUMETOP +1 +8;   
            break;
         
         case 4: 
            inoisevolume_top = NOISEVOLUMETOP +5+3;   
            break;
         
         case 5: 
            inoisevolume_top = NOISEVOLUMETOP +5;   
            break;
         
         case 6: 
            inoisevolume_top = NOISEVOLUMETOP +6;  
            break;
         
         default: 

            break;               
         
      }  
            
}

void powerkey_scan(void)
{
   
      if( P15  && (flg_keydown10==0) )
      {
         
         Switch_Delay1ms(10); 
         

         if(P15)   
         {            
            flg_keydown10=1; 
            flg_keyvalue10=1;            
         }
         
      }
      else if(!P15)
      {

         Switch_Delay1ms(10); 


         if(!P15)               
            flg_keydown10 = 0;  
      }   
   
}

void powerkey_process(void)
{
   
      if(flg_keyvalue10==1)
      {
         flg_keyvalue10=0;
         
         iputs0("power key in\r\n");
         
         if(flg_power==0)
         { 
            
            //open power
            P03=1; 
            flg_power=1;
            Switch_Delay1ms(100);            
            
            P12=1;   //led
            
            
            //onoff start level
            P10=1;   
            Switch_Delay1ms(2000);  
            P10=0;  
            Switch_Delay1ms(100);     
            
  
            //open AMP   --normal
            P11=1; 
            Switch_Delay1ms(1000);        
            
            iputbytes0(spoweron,5);
            Switch_Delay1ms(10);   
            
            
//            //open AMP    //--test
//            //Switch_Delay1ms(7000);            
//            Switch_Delay1ms(1000);            
//            P11=1; 
            

            //wait connected 
            //while ( (flg_connected==1) && (iconncount>=1000) ) iconncount++;
            set_realvolume(5); 
            Switch_Delay1ms(10);               
            
         }            
         else
         {

            Switch_Delay1ms(1000);

            iputbytes0(spoweroff,5);
            Switch_Delay1ms(10);

            Switch_Delay1ms(3500);
            
            P11=0; 
            Switch_Delay1ms(100);
            
        
            //P12=0;   //led    
            
            P03=0;
            flg_power=0;   
            
            
            //close pwm
            //clr_PWMRUN;
            //flg_openpwm =0
            
         }         
         
      }   
      
}

void adckey_scan(void)
{
      
      if(flg_channel==1)
      {
         flg_channel=0;    

         //iputs0("adc scan...\r\n");   
         
        
         //iadc_key = (UINT16)((double)BAND_value / BAND_3300 * iadcvalue);  
         iadc_key= iadcvalue;
         
         //sprintf( sadctemp1, "adckey: %u\r\n", iadc_key);     
         //sprintf( sadctemp2, "BanD:%u,adc:%u\r\n", BAND_value,iadcvalue_avr); 
         

         if(iadc_key >= ADC_VOL_NC2 )
         {

            Switch_Delay1ms(10); 


            if(iadc_key >= ADC_VOL_NC2 )
            {
               //--if(KEY0_NC_PIN==0) {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("no key2\r\n"); Switch_Delay1ms(2); }
               //sprintf( sadcstate, "no key2\r\n");
               
               flg_keydown1 = 0;
               flg_keydown2 = 0;
               flg_keydown3 = 0;
               
            }               
         }         
         
         else if (iadc_key >= ADC_TONE )
         {
         
            Switch_Delay1ms(10); 
 

            if(iadc_key >= ADC_TONE )
            {
 
               //if(flg_keydown3==0)  {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("tone key\r\n"); Switch_Delay1ms(2); }               
               //sprintf( sadcstate, "tone key\r\n");

            
               flg_keydown1 = 0;
               flg_keydown2 = 0;
               flg_keydown3 = 1;   

               ikeyvalue3 = 1;               
               
            }               
         }

         else if (iadc_key >= ADC_VOLADD )
         {

            Switch_Delay1ms(10);  

            if(iadc_key >= ADC_VOLADD )
            {
               //if(flg_keydown2==0)  {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("vol+ key\r\n"); Switch_Delay1ms(2); }
               //sprintf( sadcstate, "vol+ key\r\n");

               flg_keydown1 = 0;
               flg_keydown2 = 1;
               flg_keydown3 = 0;
               
               ikeyvalue2 = 1; 
               
            }               
         }     
         else if(iadc_key >= ADC_VOLDEC )
         {
            
            Switch_Delay1ms(10); 

            if(iadc_key >= ADC_VOLDEC )
            {
               //if(flg_keydown1==0) {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("vol- key\r\n"); Switch_Delay1ms(2); }
               //sprintf( sadcstate, "vol- key\r\n");
               
               flg_keydown1 = 1;
               flg_keydown2 = 0;
               flg_keydown3 = 0;
               
               ikeyvalue1 = 1; 
               
            }               
         }         
          
      }   
   

}

//void adckey_check(void)
//{
//   static UINT8 flg_longkey=0;
//   
//   if (flg_keycome3==1) return;
//   
//   //key3 tone
//   if (flg_keydown3) 
//   {
//      ikeynum3++;   
//      
//      if(ikeynum3>=LONGKEYCOUNT2)
//         {ikeyvalue3 =2; flg_keycome3=1;} 
//      
//      //long key time test 
//      if(flg_longkey==0)
//      {
//         P02=1;
//      }
//      
//      if(ikeynum3>=LONGKEYCOUNT2)
//      {
//         P02=0;
//         flg_longkey=1;
//      }

//   }         
//   else
//   {
//      //long key time test 
//      P02=0;
//      flg_longkey=0;
//      
//      
//      if (ikeynum3<1) 
//          ikeyvalue3 =0;
//      else if (ikeynum3<LONGKEYCOUNT2)
//          {ikeyvalue3 =1; flg_keycome3=1;}    
//      else          
//          {ikeyvalue3 =2; flg_keycome3=1;}  
//          
//   } 
//     
//}   

void adckey_check(void)
{
   //static UINT8 flg_longkey=0;
   static UINT8 flg_downlongkey3=0;
   
   if (flg_downlongkey3==0) 
   {
   
      //key3 tone
      if (flg_keydown3) 
      {
         ikeynum3++;   
         
         if(ikeynum3>=LONGKEYCOUNT2)
            {ikeyvalue3 =2; flg_keycome3=1; flg_downlongkey3=1;} 

      }         
      else
      {
         
         if (ikeynum3<1) 
             ikeyvalue3 =0;
         else if (ikeynum3<LONGKEYCOUNT2)
             {ikeyvalue3 =1; flg_keycome3=1;}    
         else          
             {ikeyvalue3 =2; flg_keycome3=1;}  
             
      } 
      
   }
   else
   {

      if (flg_keydown3==0) 
      {                 
         flg_downlongkey3=0; 
         ikeynum3=0; ikeyvalue3=0;    
      }       
      
   }
   
   
         
}   


void adckey_process(void)
{
   UINT8 stemp[20]={0};
   
   //key1  vol-
   if( (ikeyvalue1==1) && (flg_keydown1==0) )
   {
      ikeyvalue1=0;
      
      iputbytes0(svoldec,5);      
      //set_volume(3);
      
      Switch_Delay1ms(10);       
      
   }

   
   //key2  vol+
   if( (ikeyvalue2==1) && (flg_keydown2==0) )
   {
      ikeyvalue2=0;
      
      iputbytes0(svoladd,5);   
      //set_volume(7);
      
      Switch_Delay1ms(10);       
      
   }
   
   
   //key3 tone
   if  (flg_keycome3==1 )
   {
      flg_keycome3=0;

      if (ikeyvalue3 ==1)      //short 
      {
         
         if(flg_voicemode ==0)  //BT MODE
         {
            iputbytes0(splaypause,5);
            Switch_Delay1ms(10);
            
            ikeynum3=0; ikeyvalue3=0;
         }
         else if(flg_voicemode ==1)  //AUX MODE
         { 
            
            //get new noise mode
            
            //whitenoise_switch(); 
            
            WN_MODE++;
            if(WN_MODE == 7)
               WN_MODE = 1;  


            get_volumetop(WN_MODE);
            
            flg_newnoise = 1;
            
            //mute
            P11=0; 
            Switch_Delay1ms(10);
            
            
            whitenoise_switch(); 
            
            set_realvolume(inoisevolume_top);
            Switch_Delay1ms(10);  
            
            
            //unmute
            Switch_Delay1ms(10);
            P11=1; 
            
            
            
            //ipwm_mode=WN_MODE;  
            
            Switch_Delay1ms(100);  
            sprintf(stemp,"mode: %bu,%bu\r\n",WN_MODE,inoisevolume_top );
            iputs0(stemp);
            Switch_Delay1ms(20);  

            
            ikeynum3=0; ikeyvalue3=0;          
         }
         
      }

      //switch BT and AUX 
      if (ikeyvalue3 ==2)   //long
      {
         
         //P12 = ~P12;

           
         if(flg_voicemode==0)    //switch to AUX  
         {  

            //send cmd 
            iputbytes0(sBTAUX_Switch,5);
            Switch_Delay1ms(10); 

            Switch_Delay1ms(100);
            get_volumetop(1);
            set_realvolume(inoisevolume_top);            
            Switch_Delay1ms(100);            
            

            set_PWMRUN;            
            flg_openpwm =1;
            
            flg_voicemode =1;      //switch to AUX  
            
            P12=0;   //led      //--test     
            
            //inoisevolume_top = NOISEVOLUMETOP -5; //mode1
            //set_realvolume(inoisevolume_top);
            
         }
         else    //switch to BT  
         {

            //send cmd 
            iputbytes0(sBTAUX_Switch,5);
            Switch_Delay1ms(10);            
            
            clr_PWMRUN;
            flg_openpwm =0;            
            
            flg_voicemode =0;      //switch to BT  
            
            P12=1;                 //led   //--test    
            
         } 
         
         ikeynum3=0; ikeyvalue3=0;
      }
       
   }    
}


         
#define UARTDELAYTIMES 6
void main (void)
{
   UINT8 stemp[20]={0};    
   
   
//   P00_Input_Mode;    //USB DET
//   P17_Input_Mode;    //CHARGE DONE
//   P30_Input_Mode;    //BAT DET
   
   
   InitialUART0_Timer3(115200); 
   
   iputs0("start................\r\n"); 
   
   
   //set systemclock
   set_HIRCST;
   

   //adc bandgap
   READ_BANDGAP();   
   sprintf (stemp,"band vol = %6.2f\r\n",Bandgap_Voltage); 
   //--iputs0(stemp);    


   //main power     
   P15_Input_Mode;   
   P03_PushPull_Mode;   
   P03=0;   


   //BT ONOFF
   P10_PushPull_Mode;
   P10=0;
   
   
   //AMP CTRL
   P11_PushPull_Mode;
   P11=0;   
   
   
   //LED  test
   P12_PushPull_Mode;
   P12=1;     


   // test pin
//   //P01_PushPull_Mode;
//   //P01=0; 
//   P16_PushPull_Mode;
//   P16=0; 
   P02_PushPull_Mode;
   P02=0; 
 
   
   //pwm init
   pwm_init();   
   
      
   while(1)
   {
      
      //static UINT32 imaincount =0;
      
      //Switch_Delay1ms(100);  
      
      
//      if(flg_newnoise==1)
//      {
//         
//         if(inoisevolume>0) 
//         {
//            inoisevolume--;
//            set_realvolume(inoisevolume);
//         }
//         else
//         {
//            whitenoise_switch();
//            flg_newnoise =2 ;
//            
//         }         
//         
//      }
//      
//      if(flg_newnoise==2)
//      {
//         if(inoisevolume<inoisevolume_top) 
//         {
//            inoisevolume++;
//            set_realvolume(inoisevolume);
//         }
//         else
//            flg_newnoise=0;            
//      }
      
      
      //power key scan
      powerkey_scan();      

      
      //dowith power key  
      powerkey_process();     

 
      //get adc
      getadcvalue();   
      

      //adc key scan
      adckey_scan();   
      
      
      //P02=~P02;    //--timer line
      
      //adc key check         
      adckey_check();     

      //adc key process
      adckey_process( );           
      

      
   }
      

}







