
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"


#define SNOOZGO     
//#define SNOOZBABY   
//#define SNOOZBUTTON


volatile UINT8 flg_stopsendpair =0;
volatile UINT8 flg_switchaux =0;

volatile UINT8 flg_pairing =0; 
volatile UINT16 ipairingcount =0;


UINT8 flg_reset =0;
UINT8 flg_standby =0;


//dataflash
#define   FIRSTCONFIGREG       0x4400
#define   VOICEMODEREG         0x4401
#define   BTVOLREG             0x4402
#define   NOISEVOLREG          0x4403
#define   NOISEMODEREG         0x4404


//i2c

#define   FIRSTI2CREG       0x01

#define I2C_CLOCK_50KHz     79 
#define I2C_CLOCK_100KHz    39 
#define I2C_CLOCK_200KHz    19 
#define I2C_CLOCK_400KHz    9 

#define SLAVEADDR_SLA       0xA0 
#define SLAVEADDR_WR        0
#define SLAVEADDR_RD        1

bit I2C_Reset_Flag;


//white noise

//#define NOISEBASEVOLTOP    13
#define NOISEBASEVOLTOP    2


#define NOISEBASEVOLSTART  10



//UINT8 flg_newnoise = 0; 
UINT8 inoisevolume =0;
UINT8 inoisevolume_top = 0;

UINT8 inoisebasevol =0;
//UINT8 inoisebasevol_back =0;

#define BTVOLTOP    26
#define BTVOLSTART  8

UINT8 ibtvol =0;
//UINT8 ibtvol_back =0;


//key 

UINT8   KEY0_NC_NULL  =0; 

//#define LONGKEYCOUNT2_PWM0     75     //3.6s
//#define LONGKEYCOUNT2_PWM1_6     150    //3.6s
//#define LONGKEYCOUNT2_NOPWM      180    //3.6s

//UINT32 LONGKEYCOUNT2 = 150;    //fixed 3.0s+ 

//UINT32 LONGKEYCOUNT2 = 90;    
UINT32 LONGKEYCOUNT2 = 180;    


UINT8 flg_keydown1 =0;
UINT8 flg_keydown2 =0;
UINT8 flg_keydown3 =0;
UINT8 flg_keydown4 =0;
UINT8 flg_keydown12 =0;
UINT8 flg_keydown20 =0;


UINT8 ikeyvalue1 =0;
UINT8 ikeyvalue2 =0;
UINT8 ikeyvalue3 =0;
UINT8 ikeyvalue4 =0;
UINT8 ikeyvalue12 =0;

UINT32 ikeynum1 =0;
UINT32 ikeynum2 =0;
UINT32 ikeynum3 =0;
UINT32 ikeynum4 =0;
UINT32 ikeynum12 =0;

UINT8 flg_keycome1 =0;
UINT8 flg_keycome2 =0;
UINT8 flg_keycome3 =0;
UINT8 flg_keycome4 =0;
UINT8 flg_keycome12 =0;

 
UINT8 flg_keydown10 =0;
UINT8 ikeyvalue10 =0;
UINT8 flg_power =0;

UINT32 ikeynum10=0;
UINT8 flg_keycome10 =0;

UINT8 ikeytouch1 =0;
UINT8 ikeytouch2 =0;


//---------V1.2-----------

//#define ADC_VOL_GAP                   200   

//#define ADC_VOLDEC                    (1200 - ADC_VOL_GAP)
//#define ADC_VOLADD                    (1750 - ADC_VOL_GAP)   
//#define ADC_TONE                      (2075 - ADC_VOL_GAP)   //建议改成1k电阻，这里为2242  -> 2250

//#define ADC_VOL_NC                  300   //(ADC_VOLDEC  - ADC_VOL_GAP)  
// 
//#define ADC_VOL_NC2                 2300  //(ADC_TONE + ADC_VOL_GAP)    


//---------V1.4-----------

#define ADC_VOL_GAP                   200   //150

//#define ADC_VOLBOTH                   (870 - ADC_VOL_GAP)
#define ADC_VOLBOTH                   (871 - ADC_VOL_GAP)

//#define ADC_VOLDEC                    (1200 - ADC_VOL_GAP)
#define ADC_VOLDEC                    (1184 - ADC_VOL_GAP)

#define ADC_VOLADD                    (1650 - ADC_VOL_GAP)   
#define ADC_TONE                      (2200 - ADC_VOL_GAP)   

//#define ADC_VOLBOTH2                   (870 + ADC_VOL_GAP)
#define ADC_VOLBOTH2                   (871 + ADC_VOL_GAP)


//#define ADC_VOLDEC2                    (1200 + ADC_VOL_GAP)
#define ADC_VOLDEC2                    (1184 + ADC_VOL_GAP)

#define ADC_VOLADD2                    (1650 + ADC_VOL_GAP)   
#define ADC_TONE2                      (2200 + ADC_VOL_GAP)  


#define ADC_BLUETOOTH                    (2626 - ADC_VOL_GAP)   //SNOOZ BUTTON
#define ADC_BLUETOOTH2                   (2626 + ADC_VOL_GAP)   //SNOOZ BUTTON


#define ADC_VOL_NC                  300   //(ADC_VOLDEC  - ADC_VOL_GAP)  
#define ADC_VOL_NC2                 (2200 + ADC_VOL_GAP)    //(ADC_BLUETOOTH + ADC_VOL_GAP)  


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
//static U8   FN_MODE = 1;
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
UINT8 spoweron2[5]  = {0xA5, 0x03, 0x84, 0x05, 0x31};  //add

UINT8 spoweroff[5] = {0xA5, 0x03, 0x84, 0x03, 0x2f};
UINT8 spoweroff2[5] = {0xA5, 0x03, 0x8c, 0x01, 0x35};  //add

UINT8 splaypause[5] = {0xA5, 0x03, 0x82, 0x03, 0x2d};
UINT8 svoladd[5]    = {0xA5, 0x03, 0x8b, 0x20, 0x53};
UINT8 svoldec[5]    = {0xA5, 0x03, 0x8b, 0x21, 0x54};

UINT8 spairing[6] =   {0xA5, 0x04, 0x82, 0x07, 0x01, 0x33}; 
UINT8 sBTAUX_Switch[5] = {0xA5, 0x03, 0x8c, 0x20, 0x54};

UINT8 sreset[5] = {0xA5, 0x03, 0x82, 0x08, 0x32};    //reset, clear hisory device

UINT8 seqcmd0[5] = {0xA5, 0x03, 0x8a, 0x00, 0x32};   //BT

UINT8 seqcmd1[5] = {0xA5, 0x03, 0x8a, 0x04, 0x36};   //WN1
UINT8 seqcmd2[5] = {0xA5, 0x03, 0x8a, 0x05, 0x37};   //WN2
UINT8 seqcmd3[5] = {0xA5, 0x03, 0x8a, 0x06, 0x38};   //WN3

UINT8 seqcmd4[5] = {0xA5, 0x03, 0x8a, 0x01, 0x33};   //FN1
UINT8 seqcmd5[5] = {0xA5, 0x03, 0x8a, 0x02, 0x34};   //FN2
UINT8 seqcmd6[5] = {0xA5, 0x03, 0x8a, 0x03, 0x35};   //FN3

UINT8 spairingvoice[5] = {0xA5, 0x03, 0x8d, 0x21, 0x56};   //pairing voice  2.7S
UINT8 sswitchvoice[5]  = {0xA5, 0x03, 0x8d, 0x22, 0x57};   //switch voice   0.5S


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



//-------------------------------------------------------------------------
#define     CID_READ            0x0B
#define     DID_READ            0x0C

#define     PAGE_ERASE_AP       0x22
#define     BYTE_READ_AP        0x00
#define     BYTE_PROGRAM_AP     0x21
#define     PAGE_SIZE           128u

#define     ERASE_FAIL          0x70
#define     PROGRAM_FAIL        0x71
#define     IAPFF_FAIL          0x72
#define     IAP_PASS            0x00

static bit EA_Save_bit;

void Enable_IAP_Mode(void)
{
   EA_Save_bit=EA;
	EA=0;
	TA=0xAA;
	TA=0x55;
	CHPCON|=0x01 ;
	TA=0xAA;
	TA=0x55;
	IAPUEN|=0x01;
	EA=EA_Save_bit;
}

void Disable_IAP_Mode(void)
{
  EA_Save_bit=EA;
  EA=0;
  TA=0xAA;
  TA=0x55;
  IAPUEN&=~0x01;
  TA=0xAA;
  TA=0x55;
  CHPCON&=~0x01;
  EA=EA_Save_bit;
}
 
void Trigger_IAP(void)
{
	EA_Save_bit=EA;
	EA=0;
	TA=0xAA;
	TA=0x55;
	IAPTRG|=0x01;
	EA=EA_Save_bit;
}

uint8_t WriteDataToOnePage(uint16_t u16_addr,const uint8_t *pDat,uint8_t num)
{
  uint8_t i,offset;
  uint8_t code *pCode;
  uint8_t xdata xd_tmp[128];

	Enable_IAP_Mode(); 
	offset=u16_addr&0x007F;
	i = PAGE_SIZE - offset;
	if(num>i)num=i;
  pCode = (uint8_t code *)u16_addr;
  for(i=0;i<num;i++)
  {
    if(pCode[i]!=0xFF)break;
  }
	if(i==num)
	{
	  IAPCN =BYTE_PROGRAM_AP;
		IAPAL = u16_addr;
		IAPAH = u16_addr>>8;
		for(i=0;i<num;i++)
		{
			IAPFD = pDat[i];
			Trigger_IAP();
			IAPAL++;
		}
		for(i=0;i<num;i++)
		{
			if(pCode[i]!=pDat[i])break;
		}
		if(i!=num) goto WriteDataToPage20;
	}
	else
	{
		WriteDataToPage20:
		pCode = (uint8_t code *)(u16_addr&0xff80);
		for(i=0;i<128;i++)
		{
			xd_tmp[i] = pCode[i];
		}
		for(i=0;i<num;i++)
		{
			xd_tmp[offset+i] = pDat[i];
		}
		do
		{
			IAPAL = (u16_addr&0xff80);
			IAPAH = u16_addr>>8;
			IAPCN = PAGE_ERASE_AP;
			IAPFD = 0xFF;	
			Trigger_IAP(); 
			IAPCN =BYTE_PROGRAM_AP;
			for(i=0;i<128;i++)
			{
				IAPFD = xd_tmp[i];
				Trigger_IAP();
				IAPAL++;
			}
			for(i=0;i<128;i++)
			{
				if(pCode[i]!=xd_tmp[i])break;
			}
		}while(i!=128);
		
	}
	Disable_IAP_Mode();
	
	return num;
}	


void WriteDataFlash(uint16_t u16_addr,uint8_t *pDat,uint16_t num)
{
	uint8_t CPageAddr,EPageAddr,cnt;
	CPageAddr=u16_addr>>7;
	EPageAddr=(u16_addr+num)>>7;
	while(CPageAddr!=EPageAddr)
	{
		cnt=WriteDataToOnePage(u16_addr,pDat,128);
		u16_addr+=cnt;
		pDat+=cnt;
		num-=cnt;
		CPageAddr=u16_addr>>7;
	}
	if(num)
	{
		WriteDataToOnePage(u16_addr,pDat,num);
	}
}

void ReadDataFlash(UINT8 code *iaddr,uint8_t *pDat,uint16_t num)
{
	uint16_t i;
   
	for(i=0;i<num;i++)
      pDat[i] = *(iaddr+i);
}


void Init_I2C(void)
{
   P13_OpenDrain_Mode;				 
   P14_OpenDrain_Mode;	
   
   clr_I2CEN; 

   clr_I2CPX;   

   // Set I2C clock rate  
   I2CLK =  I2C_CLOCK_100KHz;    

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
        //iputs0("WR Send STAw' error\r\n");
        iputs0("write\r\n");
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
      //iputs0("RD Send STAR' error\r\n");
      iputs0("Read\r\n");
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

void set_eq(UINT8 ieqtype)
{
   
   switch (ieqtype)
   {
      case 0:  //BT
         
         iputbytes0(seqcmd0,5);
         break;
      
      case 1:  //WN1
         
         iputbytes0(seqcmd1,5);
         break;
      
      case 2:  //WN2
         
         iputbytes0(seqcmd2,5);  
         break;      
      
      case 3:  //WN3
         
         iputbytes0(seqcmd3,5); 
         break;
      
      case 4:  //FN1
         
         iputbytes0(seqcmd4,5);
         break;
      
      case 5:  //FN2
         
         iputbytes0(seqcmd5,5);
         break;      
       
      case 6:  //FN3
         
         iputbytes0(seqcmd6,5); 
         break;      
       
      default:

         break;
      
   } 

   Switch_Delay1ms(10);  
   
}


void set_realvolume(UINT8 ivol)
{
   UINT8 icmd[5]={0xa5, 0x03, 0x8b, 0, 0 };
   
   icmd[3] = ivol;
   icmd[4] = get_checksum_8(icmd,4); 
   
   iputbytes0(icmd,5);
   
}


//#define VOLDELAYMIN 40
#define VOLDELAYMIN 75


//volume: from ivol to 0, step 2
void setvolumedown(UINT8 ivol, UINT16 idelaytime )
{
   UINT16 istepdelay =0;  

   istepdelay = (UINT16)(idelaytime/ivol);    
   
   if(istepdelay < VOLDELAYMIN)
      istepdelay = VOLDELAYMIN;
   
   while (ivol)
   {
      set_realvolume(ivol);                  
      Switch_Delay1ms(istepdelay);  
      
      if(ivol >=2)
         {ivol--; ivol--; } 
      else
         break;         
   } 
   
 
   ivol =0;
   set_realvolume(ivol);                  
   Switch_Delay1ms(istepdelay);     

}  


////volume: from ivol to 0, step 1
//void setvolumedown(UINT8 ivol, UINT16 idelaytime )
//{
//   UINT16 istepdelay =0;  

//   istepdelay = (UINT16)(idelaytime/ivol);    
//   
//   if(istepdelay < VOLDELAYMIN)
//      istepdelay = VOLDELAYMIN;
//   
//   while (ivol)
//   {
//      set_realvolume(ivol);                  
//      Switch_Delay1ms(istepdelay);  
//      ivol--;    
//   } 
//   
//   set_realvolume(0);                  
//   Switch_Delay1ms(istepdelay);     

//}  


//void setvolumedown(UINT8 ivol, UINT16 idelaytime )
//{
//   UINT16 istepdelay =0;  

//   istepdelay = (UINT16)(idelaytime/ivol);    
//   
//   if(istepdelay < VOLDELAYMIN)
//      istepdelay = VOLDELAYMIN;
//   
//   while (ivol)
//   {
//      ivol--;                  
//      set_realvolume(ivol);                  
//      Switch_Delay1ms(istepdelay);                    
//   } 

//}  


//void setvolumedown2(UINT8 ivol, UINT16 idelaytime )
//{
//   UINT16 istepdelay =0;  

//   istepdelay = (UINT16)(idelaytime/ivol);    
//   
//   if(istepdelay < VOLDELAYMIN)
//      istepdelay = VOLDELAYMIN;
//   
//   while (ivol>1)
//    {
//      ivol--;                  
//      //ivol--;                  
//      set_realvolume(ivol);                  
//      Switch_Delay1ms(istepdelay);                    
//   } 

//} 


//volume: from 0 to ivol , step 2
void setvolumeup(UINT8 ivol, UINT16 idelaytime )
{
   UINT16 istepdelay =0;  
   UINT8 istepvol =0;  

   istepdelay = (UINT16)(idelaytime/ivol);     
   
   if(istepdelay < VOLDELAYMIN)
      istepdelay = VOLDELAYMIN;   
   
   while (istepvol<=ivol)
   {
      set_realvolume(istepvol);                  
      Switch_Delay1ms(istepdelay); 

      if(ivol <=1)
         break;
      else if( istepvol<= (ivol-2) )      
         {istepvol++; istepvol++; }
      else
        break;          
   }   
 
   if(istepvol<ivol)
   {
      set_realvolume(ivol);                  
      Switch_Delay1ms(istepdelay); 
   }      

} 



////volume: from 0 to ivol, step 1
//void setvolumeup(UINT8 ivol, UINT16 idelaytime )
//{
//   UINT16 istepdelay =0;  
//   UINT8 istepvol =0;  

//   istepdelay = (UINT16)(idelaytime/ivol);     
//   
//   if(istepdelay < VOLDELAYMIN)
//      istepdelay = VOLDELAYMIN;   
//   
//   while (istepvol<=ivol)
//   {
//      set_realvolume(istepvol);                  
//      Switch_Delay1ms(istepdelay);                    
//      istepvol++;     
//   } 

//} 



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
   //P0f = 1;                                                  // just an I/O marker to show activity   

//clr_EA;
   
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
//   L_OUT = (S16)(L_OUT - LPF);                              // x[n] - y[n-1]//
//   L_OUT = (S16)(L_OUT >> LPF_K);                           // k(x[n] - y[n-1])
//   L_OUT = (S16)(L_OUT + LPF);                              // k(x[n] - y[n-1]) + y[n-1]
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
   
   //P0f = 0;                                           // just an I/O marker to show activity
   
//set_EA;
           
}



////white noise switch
//void whitenoise_switch(void)
//{
// 
//   switch(WN_MODE)
//   {
//      
// 
//      case 1:     //WN1
//      {
//         LPF_K = 5;
//         HARM_K = 7;
//         NOISE_K = 1;
//         INCR = 0;      
//         break;                     
//      }
//      case 2:     //WN2
//      {
//         LPF_K = 5;
//         HARM_K = 7;
//         NOISE_K = 1;
//         INCR = 0;      
//         break;   
//      }
//      case 3:     //WN2
//      {
//         LPF_K = 5;
//         HARM_K = 7;
//         NOISE_K = 1;
//         INCR = 0;      
//         break;                    
//      }
//      

////      case 4:     //FN1
////      {
////         LPF_K = 6;
////         HARM_K = 1;
////         NOISE_K = 3;
////         INCR = 3;      
////         break;                     
////      }
//      case 4:     //FN1
//      {
//         LPF_K = 6;
//         HARM_K = 1;
//         NOISE_K = 3;  //3
//         INCR = 3;      
//         break;                     
//      }
//      
//      
//      case 5:     //FN2
//      {
//         LPF_K = 6;
//         HARM_K = 1;
//         NOISE_K = 2;
//         INCR = 4;      
//         break;                    
//      }
//      case 6:     //FN3
//      {
//         LPF_K = 6;
//         HARM_K = 1;
//         NOISE_K = 2;
//         INCR = 5;      
//         break;                       
//      }           
//      
//   }
//         
//}  

//white noise switch
void whitenoise_switch(void)
{
 
   switch(WN_MODE)
   {      
 
      case 1:     //WN1
      {
         LPF_K = 1;   //6
         HARM_K = 7;
         NOISE_K = 1;
         INCR = 0;      
         break;                     
      }
      case 2:     //WN2
      {
         LPF_K = 1;  //6
         HARM_K = 7;
         NOISE_K = 1;
         INCR = 0;      
         break;   
      }
      case 3:     //WN2
      {
         LPF_K = 1;   //6
         HARM_K = 7;
         NOISE_K = 1;
         INCR = 0;      
         break;                    
      }      

      case 4:     //FN1
      {
         LPF_K = 1;  //6
         HARM_K = 2;  //
         NOISE_K = 3;  //2/3
         INCR = 3;      
         break;                     
      }
      
      
      case 5:     //FN2
      {
         LPF_K = 1;  //6
         HARM_K = 2; //1
         NOISE_K = 2;
         INCR = 4;      
         break;                    
      }
      case 6:     //FN3
      {
         LPF_K = 1;  //6
         HARM_K = 2;  //1
         NOISE_K = 2;
         INCR = 5;      
         break;                       
      }           
      
   } 
         
}  


UINT8 get_noisevolume(UINT8 imode)
{   
   UINT8 ivol =0;

   switch (imode)
   {
      
      case 1:  //WN1
         ivol = inoisebasevol +4;  
         break;
      
      case 2:  //WN2
         ivol = inoisebasevol +2;   
         break;
      
      
      case 3:  //WN3
         ivol = inoisebasevol +0;   
         break;
      

      case 4:  //FN1
         ivol = inoisebasevol +12;    //11
         break;
      
      
      case 5:  //FN2
         ivol = inoisebasevol +9;   
         break;
      
      case 6:  //FN3
         ivol = inoisebasevol +9;  
         break;
      
      default: 

         break;       
   } 

   return ivol;   
            
}
 

//void poweron(void)
//{
//   
//   flg_standby =0;
//   
//   
//   if(flg_voicemode ==0)  //BT MODE power on
//   {      
//      
//      //open power
//      P03=1; 
//      flg_power=1;  
//      Switch_Delay1ms(10);            
//      
//      P12=1;   //led
//      
//      
//      //onoff start level
//      P10=1;   
//      //Switch_Delay1ms(2000);  
//      //Switch_Delay1ms(700);  //ok
//      Switch_Delay1ms(1000);  
//      P10=0;  
//      Switch_Delay1ms(100);   

//      
//      iputbytes0(spoweron,5);
//      //Switch_Delay1ms(10); 
//      Switch_Delay1ms(500); 

//      //--Switch_Delay1ms(1000); 
//      //open AMP   --normal
//      P11=1; 
//      Switch_Delay1ms(10); 
//      
//      
//      //send eq cmd  
//      Switch_Delay1ms(100);
//      set_eq(0); 
//      Switch_Delay1ms(10);                     


//      //wait connected 
//      //while ( (flg_connected==1) && (iconncount>=1000) ) iconncount++;
//      
//      Switch_Delay1ms(3800);
//      
//      //--set_realvolume(ibtvol); 
//      //Switch_Delay1ms(10);   
//   }
//   
//   else    //Noise MODE  power on
//   {
//      //UINT16 iswitchdelay =0; 

//      //open power
//      P03=1; 
//      flg_power=1;
//      Switch_Delay1ms(100);            
//      
//      P12=1;   //led
//      
//      //onoff start level
//      P10=1;   
//      //Switch_Delay1ms(700);  //ok
//      Switch_Delay1ms(1000);  
//      P10=0;  
//      Switch_Delay1ms(10); 


//      iputbytes0(spoweron2,5);
//      //iputbytes0(spoweron,5);
//      Switch_Delay1ms(200); 


//      //send eq cmd  
//      set_eq(WN_MODE); 
//      Switch_Delay1ms(500);  
//      
//     
//      //set new noise
//      whitenoise_switch(); 
//      Switch_Delay1ms(10);  
//      

//      //Switch_Delay1ms(1000);               
//      //Switch_Delay1ms(500);               
//      set_realvolume(0);               
//      Switch_Delay1ms(100); 
//      
//      pwm_init();   
//      set_PWMRUN;    
//      flg_voicemode =1;      //switch to AUX 


//      //open AMP
//      P11=1; 
//      Switch_Delay1ms(10);
//      
//      //volume up
//      inoisevolume=get_noisevolume(WN_MODE);
//      setvolumeup(inoisevolume, 500);

//      P12=0;   //led      
//      
//      //iputs0("led off\r\n");
//      
//      
//      //inoisevolume=get_noisevolume(WN_MODE);               
//      //set_realvolume(inoisevolume); 
//      //Switch_Delay1ms(50);      
//      
//   }     
//   
//}


void poweron_BT(void)
{
 
   Switch_Delay1ms(10);            
   
   
   //onoff start level
   P10=1;   
   //Switch_Delay1ms(2000);  
   //Switch_Delay1ms(700);  //ok
   Switch_Delay1ms(1000);  
   P10=0;  
   Switch_Delay1ms(100);   

   
   iputbytes0(spoweron,5);
   Switch_Delay1ms(500); 

   //--Switch_Delay1ms(1000); 
   //open AMP   --normal
   P11=1; 
   Switch_Delay1ms(10); 
   
   
   //send eq cmd  
   Switch_Delay1ms(100);
   set_eq(0); 
   Switch_Delay1ms(10);  

   
   Switch_Delay1ms(2000);
   //--P12=1;   //back led
   Switch_Delay1ms(1800);
   
   
   
   //--set_realvolume(ibtvol); 
   //Switch_Delay1ms(10);    
   
}


void poweron_AUX(void)
{
   
   //Switch_Delay1ms(100);   
   Switch_Delay1ms(10);   
   
   //onoff start level
   P10=1;   
   //Switch_Delay1ms(700);  //ok
   Switch_Delay1ms(1000);  
   P10=0;  
   Switch_Delay1ms(10); 


   iputbytes0(spoweron2,5);
   //iputbytes0(spoweron,5);
   Switch_Delay1ms(200); 


   //send eq cmd  
   set_eq(WN_MODE); 
   Switch_Delay1ms(500);  
   
  
   //set new noise
   whitenoise_switch(); 
   Switch_Delay1ms(10);  
   
        
   set_realvolume(0);               
   Switch_Delay1ms(100); 
   
   pwm_init();   
   set_PWMRUN;    
   flg_voicemode =1;      //switch to AUX 


   //open AMP
   P11=1; 
   Switch_Delay1ms(10);
   
   //volume up
   inoisevolume=get_noisevolume(WN_MODE);
   setvolumeup(inoisevolume, 500);
   
   P12=1;   //back led
   
   //inoisevolume=get_noisevolume(WN_MODE);               
   //set_realvolume(inoisevolume); 
   //Switch_Delay1ms(50); 
   
}


//void poweroff(void)
//{
//   
//   UINT8 sbuf1[10] ={0};
//   
//   //pairing led
//   P01=0;
//   
//   //charge led
//   P16=0;     
//   
//   //close pwm
//   clr_EPWM;
//   clr_PWMRUN;            
//   flg_openpwm =0;  
//   
//   
//   if(flg_voicemode ==1)
//   {
//      
//      //volume down
//      inoisevolume=get_noisevolume(WN_MODE);      
//      setvolumedown(inoisevolume,500);       
//      
//      set_realvolume(0);
//      Switch_Delay1ms(10);     

//      P11=0;
//   }   
//      

//   if(flg_voicemode ==0)  //BT MODE power off
//   {
//      Switch_Delay1ms(150);  //100
//      
//      P12=0;   //led  

//      iputbytes0(spoweroff,5);
//      Switch_Delay1ms(10);

//      Switch_Delay1ms(3500);  
//      
//      
//      P11=0; 
//      Switch_Delay1ms(10);

//   }
//   else    //Noise MODE power off
//   {
//      UINT8 stemp1[20]={0};
//      UINT16 iswitchdelay=0;
//      
//      P12=0;   //led         
//      
//      
//      //volume down
//      //inoisevolume=get_noisevolume(WN_MODE);      
//      //setvolumedown(inoisevolume,500); 
//      

//      //set_realvolume(0);
//      //Switch_Delay1ms(3800);   
//     
//      
//      //iputbytes0(spoweroff2,5);
//      //iputbytes0(spoweroff,5);
//      //Switch_Delay1ms(10);   
//   }
//   

//   //open i2c
//   set_I2CEN;
//   //Init_I2C();  
//   
//   //save the datas
//   if(flg_reset ==0)
//   {
//      sbuf1[0] = 1;
//      sbuf1[1] = flg_voicemode;
//      sbuf1[2] = ibtvol;
//      sbuf1[3] = inoisebasevol;
//      sbuf1[4] = WN_MODE;
//   }
//   else
//   {
//      
//      sbuf1[0] = 1;
//      sbuf1[1] = 0;
//      sbuf1[2] = BTVOLSTART;
//      sbuf1[3] = NOISEBASEVOLSTART;
//      sbuf1[4] = 1;      
//   }   
//   
//   //--WriteDataFlash(FIRSTCONFIGREG,sbuf1,5);
//   //Switch_Delay1ms(10);
//   
//   //write datas to i2c
//   i2c_write(FIRSTI2CREG, sbuf1, 5);
//   Switch_Delay1ms(10);
//   
//   
//   P03=0;       //power off
//   flg_power=0;     
//   
//   flg_standby =1;
//}


void poweroff_AUX(void)
{
   
   UINT8 sbuf1[10] ={0};
   
   //UINT8 stemp1[20]={0};
   //UINT16 iswitchdelay=0;
   
   //pairing led
   P01=0;
   
   //charge led
   P16=0;     
   
   
   //volume down
   inoisevolume=get_noisevolume(WN_MODE);      
   setvolumedown(inoisevolume,500);       
   
   
   //close pwm
   clr_EPWM;
   clr_PWMRUN;            
   flg_openpwm =0;  
   

   P11=0;
   P12=0;   //back led   

   //set_realvolume(0);
   //Switch_Delay1ms(3800); 
   
   //iputbytes0(spoweroff2,5);
   //iputbytes0(spoweroff,5);
   //Switch_Delay1ms(10);   
   

   //----------------------I2C -------------------------
  
   //open i2c
   set_I2CEN;
   //Init_I2C();  
   
   //save the datas
   if(flg_reset ==0)
   {
      sbuf1[0] = 1;
      sbuf1[1] = flg_voicemode;
      sbuf1[2] = ibtvol;
      sbuf1[3] = inoisebasevol;
      sbuf1[4] = WN_MODE;
   }
   else
   {
      
      sbuf1[0] = 1;
      sbuf1[1] = 0;
      sbuf1[2] = BTVOLSTART;
      sbuf1[3] = NOISEBASEVOLSTART;
      sbuf1[4] = 1;      
   }   
   
   //--WriteDataFlash(FIRSTCONFIGREG,sbuf1,5);
   //Switch_Delay1ms(10);
   
   //write datas to i2c
   i2c_write(FIRSTI2CREG, sbuf1, 5);
   Switch_Delay1ms(10);
   
   //----------------------I2C ---end----------------------
   
   
   P03=0;       //power off
   flg_power=0;     
   
   flg_standby =1;
}

void poweroff_BT(void)
{
   
   UINT8 sbuf1[10] ={0};
   
   //pairing led
   P01=0;
   
   //charge led
   P16=0;     
   

   Switch_Delay1ms(150);  //100
   
   P12=0;   //back led  

   iputbytes0(spoweroff,5);
   Switch_Delay1ms(10);

   Switch_Delay1ms(3500);  
   
   
   P11=0; 
   Switch_Delay1ms(10);



   //----------------------I2C -------------------------
   //open i2c
   set_I2CEN;
   //Init_I2C();  
   
   //save the datas
   if(flg_reset ==0)
   {
      sbuf1[0] = 1;
      sbuf1[1] = flg_voicemode;
      sbuf1[2] = ibtvol;
      sbuf1[3] = inoisebasevol;
      sbuf1[4] = WN_MODE;
   }
   else
   {
      
      sbuf1[0] = 1;
      sbuf1[1] = 0;
      sbuf1[2] = BTVOLSTART;
      sbuf1[3] = NOISEBASEVOLSTART;
      sbuf1[4] = 1;      
   }   
   
   //--WriteDataFlash(FIRSTCONFIGREG,sbuf1,5);
   //Switch_Delay1ms(10);
   
   //write datas to i2c
   i2c_write(FIRSTI2CREG, sbuf1, 5);
   Switch_Delay1ms(10);

   //----------------------I2C ---end----------------------
   
   
   P03=0;         //power off
   flg_power=0;     
   
   flg_standby =1;
}




void switch2AUX(void)
{
   UINT8 sbuf1[10] ={0};      
   
//   //volume down 
//   setvolumedown(8, 500);   //in tts , can't sent volume command
   
   
   //close AMP                       
   P11=0; 
   Switch_Delay1ms(10);     

   P12=1;   //back led  
   

   //send cmd 
   iputbytes0(sBTAUX_Switch,5);
   Switch_Delay1ms(1500);
   
   
   set_realvolume(0);                  
   Switch_Delay1ms(100);    
   
   
   //send eq cmd  
   set_eq(WN_MODE); 
   Switch_Delay1ms(100);          

      
   //open AMP                     
   P11=1;
   Switch_Delay1ms(10);  


   set_PWMRUN;            
   flg_openpwm =1;


   //volume up
   inoisevolume=get_noisevolume(WN_MODE);   
   setvolumeup(inoisevolume, 500);
   Switch_Delay1ms(10);  

   flg_voicemode =1;      //switch to AUX  
     
     
   //write to i2c
   set_I2CEN;
   sbuf1[1] = flg_voicemode;    
   i2c_write(FIRSTI2CREG+1, sbuf1+1, 1);
   Switch_Delay1ms(10);   
   clr_I2CEN; 
         
}  


void switch2BT(void)
{   
   UINT8 sbuf1[10] ={0};  
   
   flg_stopsendpair =0;   
   
   //volume down 
   inoisevolume=get_noisevolume(WN_MODE);
   setvolumedown(inoisevolume, 500);

   
   //close pwm
   clr_PWMRUN;
   flg_openpwm =0;        
   

   //play switch tone  
   //set_realvolume(1);                  
   //witch_Delay1ms(80);      
   iputbytes0(sswitchvoice,5);
   Switch_Delay1ms(1000); 

   
   
   //close AMP                       
   P11=0; 
   Switch_Delay1ms(10);  
   

   //send switch2BT cmd          
   iputbytes0(sBTAUX_Switch,5);         
   Switch_Delay1ms(500); 

   
   //send eq cmd  
   set_eq(0); 
   Switch_Delay1ms(100);     

   
   //reback bt vol
   //set_realvolume(ibtvol); 
   //Switch_Delay1ms(10); 
   
   
   //open AMP                     
   P11=1;
   Switch_Delay1ms(10); 
   
   flg_voicemode =0;      //switch to BT              
   P12=0;     //back led           


   //write to i2c
   set_I2CEN;
   sbuf1[1] = flg_voicemode;    
   i2c_write(FIRSTI2CREG+1, sbuf1+1, 1);
   Switch_Delay1ms(10);   
   clr_I2CEN; 
         
}



void powerkey_scan(void)
{
   
   if( P15)
   {      
      Switch_Delay1ms(10); 
      
      if(P15)           
         flg_keydown10=1;             
   }
   
   else if(!P15)
   {
      if(flg_keydown10==1)      //add
         Switch_Delay1ms(10);
      
      if(!P15)               
         flg_keydown10 = 0;  
   }      
   
}


#define SHORTPWKEYCOUNT  1  
#define LONGPWKEYCOUNT1  20    
#define LONGPWKEYCOUNT1B 30    

#define LONGPWKEYCOUNT2 200  //5s+-

void powerkey_check(void)
{
   static UINT8 flg_downlongkey1=0;
   //static UINT32 ilongpowercount2=0;
   
   if (flg_downlongkey1==0) 
   {
   
      //powerkey
      if (flg_keydown10) 
      {
         ikeynum10++;            

         
         if(flg_power==0)
         {
            if(ikeynum10>=LONGPWKEYCOUNT1B)
               {ikeyvalue10 =2; flg_keycome10=1; flg_downlongkey1=1;}             
         }
         else
         {            
            if(ikeynum10>=LONGPWKEYCOUNT2)
               {ikeyvalue10 =3; flg_keycome10=1; flg_downlongkey1=1;} 
         }

      }         
      else
      {   
         
         if (ikeynum10<1)   
             ikeyvalue10 =0;         
         else if (ikeynum10<LONGPWKEYCOUNT1B)    
             {ikeyvalue10 =1; flg_keycome10=1;}   
         else if (ikeynum10<LONGPWKEYCOUNT2)
             {ikeyvalue10 =2; flg_keycome10=1;}    
         else          
             {ikeyvalue10 =3; flg_keycome10=1;}  
             
             
      } 
      
   }
   else
   {

      if (flg_keydown10==0) 
      {                 
         flg_downlongkey1=0; 
         ikeynum10=0; ikeyvalue10=0;    
      }       
      
   }   
         
}


void powerkey_process(void)
{      

   // (AUX and backled)   on/off
   if(ikeyvalue10 == 1)
   {
      //--ikeyvalue10 = 0;
      

#ifdef SNOOZGO   
      
      if  ( (flg_voicemode==0) && (P12==0) )     //aux off, led off
            flg_switchaux =1;    //switch2AUX();                     

      if  ( (flg_voicemode==1) && (P12==1) )     //aux on, led on 
            switch2BT();        
#endif

      ikeynum10=0; ikeyvalue10=0;        
   }   
   
   
   //power on/off
   if(ikeyvalue10 == 2)
   {
      //--ikeyvalue10 = 0;
      
      //iputs0("power key in\r\n");
      
      if(flg_power==0)
      { 
         //poweron();
         
         flg_standby =0;   
      
         //open power
         P03=1; 
         
         if(flg_voicemode ==0)  //0 -- BT, 1 -- AUX
            poweron_BT(); 
         else
            poweron_AUX();   

         flg_power=1;           
         
      }            
      else
      {
         //--poweroff();
         
         if(flg_voicemode ==0)  //0 -- BT, 1 -- AUX
            poweroff_BT();
         else
            poweroff_AUX();   
                     
      }    

      ikeynum10=0; ikeyvalue10=0;  
   } 


   //control led
   if(ikeyvalue10 == 3)
   {
      //--ikeyvalue10 = 0;

      if (flg_voicemode==0)     //0 -- BT, 1 -- AUX
         P12 = ~P12;
      else         
         P12 = ~P12;

      ikeynum10=0; ikeyvalue10=0;        
   }    
        
}

#define ADCKEYDELAY  10  //30
void adckey_scan(void)
{
      
   if(flg_channel==1)
   {
      flg_channel=0;    

      //iputs0("adc scan...\r\n");   
      
     
      //iadc_key = (UINT16)((double)BAND_value / BAND_3300 * iadcvalue);  
      iadc_key= iadcvalue;
      
      sprintf( sadctemp1, "adckey: %u\r\n", iadc_key);     
      //sprintf( sadctemp2, "BanD:%u,adc:%u\r\n", BAND_value,iadcvalue_avr); 
      

      if(iadc_key >= ADC_VOL_NC2 )
      {

         Switch_Delay1ms(ADCKEYDELAY); 

         if(iadc_key >= ADC_VOL_NC2 )
         {
            //if(KEY0_NC_NULL==0) {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("no key2\r\n"); Switch_Delay1ms(2); }
            //sprintf( sadcstate, "no key2\r\n");
            
            KEY0_NC_NULL =1;
            flg_keydown1 = 0;
            flg_keydown2 = 0;
            flg_keydown3 = 0;
            flg_keydown4 = 0;   
            flg_keydown12 = 0;   
            flg_keydown20 = 0;            
            
         }   
         
      } 

#ifdef SNOOZBUTTON
      
//      else if ( (iadc_key >= ADC_BLUETOOTH ) && (iadc_key <= ADC_BLUETOOTH2 ) )   //keep
//      {
//      
//         Switch_Delay1ms(ADCKEYDELAY); 

//         if ( (iadc_key >= ADC_BLUETOOTH ) && (iadc_key <= ADC_BLUETOOTH2 ) )
//         {

//            if(flg_keydown4==0)  {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("btn key\r\n"); Switch_Delay1ms(2); }               
//            //sprintf( sadcstate, "btn key\r\n");

//         
//            KEY0_NC_NULL =0;
//            flg_keydown1 = 0;
//            flg_keydown2 = 0;
//            flg_keydown3 = 0;   
//            flg_keydown4 = 1;   
//            flg_keydown12 = 0;
//            flg_keydown20 = 0;

//            ikeyvalue4 = 1;           
//         }
//         
//      } 
      
#endif      
      
      //else if (iadc_key >= ADC_TONE )
      else if ( (iadc_key >= ADC_TONE) && (iadc_key<= ADC_TONE2) )
      {
      
         Switch_Delay1ms(ADCKEYDELAY); 

         if( (iadc_key >= ADC_TONE) && (iadc_key<= ADC_TONE2) )
         {

            if(flg_keydown3==0)  {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("tone key\r\n"); Switch_Delay1ms(2); }               
            //sprintf( sadcstate, "tone key\r\n");

         
            KEY0_NC_NULL =0;
            flg_keydown1 = 0;
            flg_keydown2 = 0;
            flg_keydown3 = 1;   
            flg_keydown4 = 0;   
            flg_keydown12 = 0;
            flg_keydown20 = 0;

            ikeyvalue3 = 1;           
         }
         
      }

      //else if (iadc_key >= ADC_VOLADD )
      else if ( (iadc_key >= ADC_VOLADD )  &&  (iadc_key <= ADC_VOLADD2 )  )
      {

         Switch_Delay1ms(ADCKEYDELAY);  

         if( (iadc_key >= ADC_VOLADD )  &&  (iadc_key <= ADC_VOLADD2 )  )
         {
            //--if(flg_keydown2==0)  {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("vol+ key\r\n"); Switch_Delay1ms(2); }
            //sprintf( sadcstate, "vol+ key\r\n");

            if(flg_keydown2==0)
               ikeytouch2=1;               
            
            KEY0_NC_NULL =0;
            flg_keydown1 = 0;
            flg_keydown2 = 1;
            flg_keydown3 = 0;
            flg_keydown4 = 0;   
            flg_keydown12 = 0;
            flg_keydown20 = 0;
            
            //--ikeyvalue2 = 1; 
         }     
         
      }     
      //else if(iadc_key >= ADC_VOLDEC )
      else if( (iadc_key >= ADC_VOLDEC ) && (iadc_key <= ADC_VOLDEC2 ) )
      {
         
         Switch_Delay1ms(ADCKEYDELAY); 

         if( (iadc_key >= ADC_VOLDEC ) && (iadc_key <= ADC_VOLDEC2 ) )
         {
            //--if(flg_keydown1==0) {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("vol- key\r\n"); Switch_Delay1ms(2); }
            //sprintf( sadcstate, "vol- key\r\n");

            if(flg_keydown1==0)
               ikeytouch1=1;               
            
            
            KEY0_NC_NULL =0;
            flg_keydown1 = 1;
            flg_keydown2 = 0;
            flg_keydown3 = 0;
            flg_keydown4 = 0;   
            flg_keydown12 = 0;
            flg_keydown20 = 0;
            
            //ikeyvalue1 = 1; 
         }               
      }   

      //else if(iadc_key >= ADC_VOLBOTH )
      else if( (iadc_key >= ADC_VOLBOTH ) && (iadc_key <= ADC_VOLBOTH2 ) )
      {
         
         Switch_Delay1ms(ADCKEYDELAY); 

         if( (iadc_key >= ADC_VOLBOTH ) && (iadc_key <= ADC_VOLBOTH2 ) )
         {
            //if(flg_keydown12==0) {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("vol22 key\r\n"); Switch_Delay1ms(2); }
            //sprintf( sadcstate, "vol22 key\r\n");      
            
            KEY0_NC_NULL =0;
            flg_keydown1 = 0;
            flg_keydown2 = 0;
            flg_keydown3 = 0;
            flg_keydown4 = 0;   
            flg_keydown12 = 1;
            flg_keydown20 = 0;
            
            //ikeyvalue12 = 1; 
         }   
         
      } 

      //else if(iadc_key <= ADC_VOL_NC )
      //{
      //   
      //   Switch_Delay1ms(ADCKEYDELAY); 

      //   if(iadc_key <= ADC_VOL_NC )
      //   {
      //      if(KEY0_NC_NULL2==0) {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("no key-\r\n"); Switch_Delay1ms(2); }
      //      //sprintf( sadcstate, "vol0 key\r\n");      
      //      
      //      KEY0_NC_NULL =0;
      //      flg_keydown1 = 0;
      //      flg_keydown2 = 0;
      //      flg_keydown3 = 0;
      //      flg_keydown4 = 0;   
      //      flg_keydown12 = 0;
      //      flg_keydown20 = 0;
      //      KEY0_NC_NULL2 =1;
      
      //   }   
      //   
      //} 


      else 
      {         

         //--if(flg_keydown20==0) {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("err key\r\n"); Switch_Delay1ms(2); }
         //sprintf( sadcstate, "vol20 key\r\n");      
         
         KEY0_NC_NULL =0;
         flg_keydown1 = 0;
         flg_keydown2 = 0;
         flg_keydown3 = 0;
         flg_keydown4 = 0;   
         flg_keydown12 = 0;
         
         flg_keydown20 = 1;         
         
      }
      
       
   }  

}

//#define ADCKEYDELAY  10  //30
//void adckey_scan(void)
//{
//      
//   if(flg_channel==1)
//   {
//      flg_channel=0;    

//      //iputs0("adc scan...\r\n");   
//      
//     
//      //iadc_key = (UINT16)((double)BAND_value / BAND_3300 * iadcvalue);  
//      iadc_key= iadcvalue;
//      
//      //sprintf( sadctemp1, "adckey: %u\r\n", iadc_key);     
//      //sprintf( sadctemp2, "BanD:%u,adc:%u\r\n", BAND_value,iadcvalue_avr); 
//      

//      if(iadc_key >= ADC_VOL_NC2 )
//      {

//         Switch_Delay1ms(ADCKEYDELAY); 

//         if(iadc_key >= ADC_VOL_NC2 )
//         {
//            //if(KEY0_NC_NULL==0) {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("no key2\r\n"); Switch_Delay1ms(2); }
//            //sprintf( sadcstate, "no key2\r\n");
//            
//            KEY0_NC_NULL =1;
//            flg_keydown1 = 0;
//            flg_keydown2 = 0;
//            flg_keydown3 = 0;
//            flg_keydown4 = 0;   
//            flg_keydown12 = 0;        
//            
//         }   
//         
//      } 

//#ifdef SNOOZBUTTON
//      
//      //else if (iadc_key >= ADC_BLUETOOTH )    //keep
//      //{

//      //   Switch_Delay1ms(ADCKEYDELAY); 

//      //   if(iadc_key >= ADC_BLUETOOTH )
//      //   {

//      //      if(flg_keydown4==0)  {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("btn key\r\n"); Switch_Delay1ms(2); }               
//      //      //sprintf( sadcstate, "btn key\r\n");

//      //   
//      //      KEY0_NC_NULL =0;
//      //      flg_keydown1 = 0;
//      //      flg_keydown2 = 0;
//      //      flg_keydown3 = 0;   
//      //      flg_keydown4 = 1;   
//      //      flg_keydown12 = 0;

//      //      ikeyvalue4 = 1;           
//      //   }
//      //   
//      //} 
//      
//#endif      
//      
//      //else if (iadc_key >= ADC_TONE )
//      else if ( (iadc_key >= ADC_TONE) && (iadc_key<= ADC_TONE2) )
//      {
//      
//         Switch_Delay1ms(ADCKEYDELAY); 

//         if( (iadc_key >= ADC_TONE) && (iadc_key<= ADC_TONE2) )
//         {

//            //--if(flg_keydown3==0)  {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("tone key\r\n"); Switch_Delay1ms(2); }               
//            //sprintf( sadcstate, "tone key\r\n");

//         
//            KEY0_NC_NULL =0;
//            flg_keydown1 = 0;
//            flg_keydown2 = 0;
//            flg_keydown3 = 1;   
//            flg_keydown4 = 0;   
//            flg_keydown12 = 0;

//            ikeyvalue3 = 1;           
//         }
//         
//      }

//      //else if (iadc_key >= ADC_VOLADD )
//      else if ( (iadc_key >= ADC_VOLADD )  &&  (iadc_key <= ADC_VOLADD2 )  )
//      {

//         Switch_Delay1ms(ADCKEYDELAY);  

//         if( (iadc_key >= ADC_VOLADD )  &&  (iadc_key <= ADC_VOLADD2 )  )
//         {
//            //if(flg_keydown2==0)  {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("vol+ key\r\n"); Switch_Delay1ms(2); }
//            //sprintf( sadcstate, "vol+ key\r\n");

//            if(flg_keydown2==0)
//               ikeytouch2=1;               
//            
//            KEY0_NC_NULL =0;
//            flg_keydown1 = 0;
//            flg_keydown2 = 1;
//            flg_keydown3 = 0;
//            flg_keydown4 = 0;   
//            flg_keydown12 = 0;
//            
//            //--ikeyvalue2 = 1; 
//         }     
//         
//      }     
//      //else if(iadc_key >= ADC_VOLDEC )
//      else if( (iadc_key >= ADC_VOLDEC ) && (iadc_key <= ADC_VOLDEC2 ) )
//      {
//         
//         Switch_Delay1ms(ADCKEYDELAY); 

//         if( (iadc_key >= ADC_VOLDEC ) && (iadc_key <= ADC_VOLDEC2 ) )
//         {
//            //--if(flg_keydown1==0) {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("vol- key\r\n"); Switch_Delay1ms(2); }
//            //sprintf( sadcstate, "vol- key\r\n");

//            if(flg_keydown1==0)
//               ikeytouch1=1;               
//            
//            
//            KEY0_NC_NULL =0;
//            flg_keydown1 = 1;
//            flg_keydown2 = 0;
//            flg_keydown3 = 0;
//            flg_keydown4 = 0;   
//            flg_keydown12 = 0;
//            
//            //ikeyvalue1 = 1; 
//         }               
//      }   

//      //else if(iadc_key >= ADC_VOLBOTH )
//      else if( (iadc_key >= ADC_VOLBOTH ) && (iadc_key <= ADC_VOLBOTH2 ) )
//      {
//         
//         Switch_Delay1ms(ADCKEYDELAY); 

//         if( (iadc_key >= ADC_VOLBOTH ) && (iadc_key <= ADC_VOLBOTH2 ) )
//         {
//            //if(flg_keydown12==0) {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("vol22 key\r\n"); Switch_Delay1ms(2); }
//            //sprintf( sadcstate, "vol22 key\r\n");      
//            
//            KEY0_NC_NULL =0;
//            flg_keydown1 = 0;
//            flg_keydown2 = 0;
//            flg_keydown3 = 0;
//            flg_keydown4 = 0;   
//            flg_keydown12 = 1;
//            
//            //ikeyvalue12 = 1; 
//         }   
//         
//      } 

//      //else if(iadc_key <= ADC_VOL_NC )
//      //{
//      //   
//      //   Switch_Delay1ms(ADCKEYDELAY); 

//      //   if(iadc_key <= ADC_VOL_NC )
//      //   {
//      //      if(flg_keydown12==0) {iputs0(sadctemp1); Switch_Delay1ms(2); iputs0("no key-\r\n"); Switch_Delay1ms(2); }
//      //      //sprintf( sadcstate, "vol22 key\r\n");      
//      //      
//      //      KEY0_NC_NULL =0;
//      //      flg_keydown1 = 0;
//      //      flg_keydown2 = 0;
//      //      flg_keydown3 = 0;
//      //      flg_keydown4 = 0;   
//      //      flg_keydown12 = 0;

//      //   }   
//      //   
//      //}       
//       
//   }  

//}




#define VOLKEYCOUNT1   7 
#define VOLKEYCOUNT1B  8 
#define VOLKEYCOUNT2   35

#define TONEKEYCOUNT1  8 
#define LONGKEYCOUNT3  400   //5S-
void adckey_check(void)
{   
   static UINT8 flg_downlongkey3=0;
   static UINT8 flg_downlongkey12=0;
   
   //key1 vol-   
   if (flg_keydown1) 
   {
      ikeynum1++;
   }         
   else
   {      
      ikeynum1 =0;
      ikeytouch1 = 0;
   }   
   
         
   //key1 vol-
   if( ( (ikeytouch1 == 1 ) && (ikeynum1 >=VOLKEYCOUNT1) ) || (ikeynum1 >=VOLKEYCOUNT2) ) 
   {
      ikeynum1 = 0;
      ikeytouch1 = 0;
      ikeyvalue1=1;
   }
   

   //key2 vol+   
   if (flg_keydown2) 
   {  
      ikeynum2++;
   }         
   else
   {      
      ikeynum2 =0;
      ikeytouch2=0;
   }  
   
   //key2 vol+
   if( ( (ikeytouch2 == 1 ) && (ikeynum2 >=VOLKEYCOUNT1) ) || (ikeynum2 >=VOLKEYCOUNT2) ) 
   {
      ikeynum2 = 0;
      ikeytouch2 = 0;
      ikeyvalue2=1;
   }
   
   
   //key3 tone
   if (flg_downlongkey3==0) //flg_downlongkey3本身就相当于flg_keyup (只有keyup=1后，flg_downlongkey3才能恢复为0)
   {   
      
      if (flg_keydown3)  //如果在这里加一个flg_keyup==1防止长按后连按，则逻辑上更标准一些   （不需要，看上面）
      {
         ikeynum3++;   
         
         if(ikeynum3>=LONGKEYCOUNT2)
            {ikeyvalue3 =2; flg_keycome3=1; flg_downlongkey3=1;}  //如判断连按，这里需加一句flg_keyup=0  （不需要，看上面）

      }         
      else
      {
         
         //if (ikeynum3<1) 
         if (ikeynum3<TONEKEYCOUNT1) 
            {ikeyvalue3 =0; }
         
         else if( (ikeynum3>=TONEKEYCOUNT1) && (ikeynum3<LONGKEYCOUNT2) )
             {ikeyvalue3 =1; flg_keycome3=1;}  
             
         //else          
         else if(ikeynum3>=LONGKEYCOUNT2)               
             {ikeyvalue3 =2; flg_keycome3=1;}  
             
         ikeynum3=0;    
             
      } 
      
   }
   else
   {

      if (flg_keydown3==0) 
      {                 
         flg_downlongkey3=0; 
         ikeynum3=0; ikeyvalue3=0;    
      }  
      else
         ;//ikeynum3=0;      //这样的处理，会重新计数，会触发连按   //所以要注释
      
   }   
   
   //key12 volboth
   if (flg_downlongkey12==0) 
   {   
      
      if (flg_keydown12) 
      {
         ikeynum12++;   
         
         //P0f =1;
         if(ikeynum12>=LONGKEYCOUNT3)
            {ikeyvalue12 =2; flg_keycome12=1; flg_downlongkey12=1;  
             //P0f=0;
            } 

      }         
      else
      {
         P16=0;  
         
         if (ikeynum12<1) 
             ikeyvalue12 =0;
         else if (ikeynum12<LONGKEYCOUNT3)
             {ikeyvalue12 =1; flg_keycome12=1;}    
         else          
             {ikeyvalue12 =2; flg_keycome12=1;}  
             
      } 
      
   }
   else
   {

      if (flg_keydown12==0) 
      {                 
         flg_downlongkey12=0; 
         ikeynum12=0; ikeyvalue12=0;    
      }       
      else 
         ; //keynum3=0;      
   }      
         
}   
 

void switchnoise(void)
{
   UINT8 stemp[38]={0};
   UINT8 sbuf1[10] ={0};   
   
      
   //volume down 
   inoisevolume=get_noisevolume(WN_MODE);
   setvolumedown(inoisevolume, 500);
   
   
   //close AMP                       
   P11 =0; 
   Switch_Delay1ms(10);  
   
   clr_PWMRUN;
   flg_openpwm =0;    

    
   //get new noise mode
   WN_MODE++;
   if(WN_MODE >= 7)
      WN_MODE = 1;  

   
   //set new noise
   whitenoise_switch(); 
   Switch_Delay1ms(10);    
   

   //send eq cmd     
   set_eq(WN_MODE); 
   Switch_Delay1ms(100);  
   
   
   //open AMP     
   pwm_init();   
   set_PWMRUN;
   flg_openpwm =1;     

   P11 =1; 
   Switch_Delay1ms(10);  


   //play switch tone   
   if( WN_MODE ==1) 
   {      
      //set_realvolume(1);
      ////Switch_Delay1ms(75); //ok  
      //Switch_Delay1ms(80);   
      
      clr_PWMRUN;
      flg_openpwm =0;  
         
      iputbytes0(sswitchvoice,5);
      Switch_Delay1ms(1000); 

      pwm_init();   
      set_PWMRUN;
      flg_openpwm =1;
   }


   //volume up
   inoisevolume=get_noisevolume(WN_MODE);
   setvolumeup(inoisevolume, 500);

   
   
   //write to i2c
   set_I2CEN;
   sbuf1[4] = WN_MODE;
   i2c_write(FIRSTI2CREG+4, sbuf1+4, 1);
   Switch_Delay1ms(10);   
   clr_I2CEN;  

   //sprintf(stemp,"mode: %bu,%bu,%bu\r\n",WN_MODE,inoisebasevol,inoisevolume );
   //iputs0(stemp);
   //Switch_Delay1ms(20); 
            
} 


void adckey_process(void)
{
   UINT8 sbuf1[10] ={0};
   
   //key1  vol-
   //if( (ikeyvalue1==1) && (flg_keydown1==0) )
   if (ikeyvalue1==1)
   {
      ikeyvalue1=0;    

      if(flg_reset ==1) return;      
      
      if(flg_voicemode==0)   //0 -- BT, 1 -- AUX
      {         
         iputbytes0(svoldec,5);  
         Switch_Delay1ms(10);          
         
         //if(ibtvol >0)
         //   ibtvol--;         

         //set_realvolume(ibtvol); 
         //Switch_Delay1ms(10); 
      } 
      else
      {
         if(inoisebasevol >0)
            inoisebasevol--;
         
         inoisevolume=get_noisevolume(WN_MODE);
         set_realvolume(inoisevolume); 
         Switch_Delay1ms(10);  
         
         
         //write to i2c
         set_I2CEN;
         sbuf1[3] = inoisebasevol;      
         i2c_write(FIRSTI2CREG+3, sbuf1+3, 1);
         Switch_Delay1ms(10);   
         clr_I2CEN;        
         
      }   
      
//--iputs0("v- in\r\n");      
      
   }

   
   //key2  vol+
   //if( (ikeyvalue2==1) && (flg_keydown2==0) )
   if( ikeyvalue2==1)
   {
      ikeyvalue2=0;
      
      if(flg_reset ==1) return;      

      if(flg_voicemode==0)   //0 -- BT, 1 -- AUX
      {        
         iputbytes0(svoladd,5); 
         Switch_Delay1ms(10);         

         //if( ibtvol < BTVOLTOP )
         //   ibtvol++;    

         //set_realvolume(ibtvol); 
         //Switch_Delay1ms(10);       
      } 
      else
      {
         if( inoisebasevol < NOISEBASEVOLTOP )
            inoisebasevol++;
         
         inoisevolume=get_noisevolume(WN_MODE);
         set_realvolume(inoisevolume); 
         Switch_Delay1ms(10);  
         
         
         //write to i2c
         set_I2CEN;
         sbuf1[3] = inoisebasevol;      
         i2c_write(FIRSTI2CREG+3, sbuf1+3, 1);
         Switch_Delay1ms(10);   
         clr_I2CEN;         
      } 

//--iputs0("v+ in\r\n");
      

   }   


   //key3 tone
   if  (flg_keycome3==1 )
   {
      flg_keycome3=0;

      if (ikeyvalue3 ==1)      //short 
      {
         
         ikeynum3=0; ikeyvalue3=0; 
         
////         if(flg_voicemode ==0)  //BT MODE
////         {
////            iputbytes0(splaypause,5);
////            Switch_Delay1ms(10);
////         }
////         else if(flg_voicemode ==1)  //AUX MODE
////         {      
////            switchnoise();  
////         }

         
#ifdef SNOOZGO   
         
         if(flg_voicemode==0)    //0 -- BT, 1-- AUX               
            //switch2AUX();  
            flg_switchaux =1;         

         else  
            switch2BT();
         
#endif
         
iputs0("tonekey in\r\n");
         
      }

      //switch BT and AUX 
      if (ikeyvalue3 ==2)   //long
      {  
         
         //ikeynum3=0; ikeyvalue3=0;     //这样的处理，会重新计数，会触发连按       （不需要考虑这里）  
         ikeyvalue3=0;                   //这里的处理，不会重新计数，不会触发连按       （不需要考虑这里）  

////#ifdef SNOOZGO   
////         
////         if(flg_voicemode==0)    //0 -- BT, 1-- AUX               
////            //switch2AUX();  
////            flg_switchaux =1;         

////         else  
////            switch2BT();
////         
////#endif

         
iputs0("tonekey in2\r\n");      
         
      }
       
   }  

   
   //key12 volboth
   if  (flg_keycome12==1 )
   {
      flg_keycome12=0;

      if (ikeyvalue12 ==1)      //short 
      {         
         ikeynum12=0; ikeyvalue12=0;         
         
         ;  //nothing       
      }

      //restore factory settings
      if (ikeyvalue12 ==2)   //long
      {
         ikeynum12=0; ikeyvalue12=0;         

         flg_reset =1; 
         
         
         //send cmd 
         iputbytes0(sreset,5); 
         Switch_Delay1ms(1000);  
         
         
         //power off
         if(flg_voicemode ==0)  //0 -- BT, 1 -- AUX
            poweroff_BT();
         else
            poweroff_AUX();  
         
      }
       
   }    
   
   if (ikeyvalue4==1)
   {
      ikeyvalue4=0;
      
      ;  //for snooz button
   }       
   
   
}
      
void waitpairing(void)
{
   if (flg_pairing ==1) 
      return;
   
   flg_switchaux =0;         
   switch2AUX();    
}   



#define SVERNO	"1564"         
#define UARTDELAYTIMES 6
#define PAIR_LEDCOUNT   50      //500MS+

//#define PAIR_TONECOUNT  100     //1S+
//#define PAIR_TONECOUNT  320     //3.4S+ 
//#define PAIR_TONECOUNT  310     //3.3S+     
//#define PAIR_TONECOUNT  300     //3.2S+     
#define PAIR_TONECOUNT  330       //3.5S+     
  
//#define PAIR_SWITHCOUNT  330       //ok   
#define PAIR_SWITHCOUNT  380      //4.0s+ 


void main (void)
{
   UINT8 stemp[20]={0};    
   UINT8 sbuf1[10]={0};
   UINT8 sbuf2[10]={0};   
   
   P00_Input_Mode;    //USB DET
   P17_Input_Mode;    //CHARGE DONE
   P30_Input_Mode;    //BAT DET
   
   
   InitialUART0_Timer3(115200); 
   Timer0_Delay1ms(10); 
   
//   iputs0("start................\r\n"); 
   iputs0("start");  
   iputs0(SVERNO);  
   iputs0("............\r\n");  
   Timer0_Delay1ms(10);      
   
   
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
   
   
   //LED P12
   P12_PushPull_Mode;
   P12 = 0;  //back led


   //---------V1.4---add--------

   //LED pairing 
   P01_PushPull_Mode;
   P01=0;  

   //LED charge
   P16_PushPull_Mode;
   P16=1;  
   
   //paring det
   P02_Input_Mode; 
   
   
//   P00_Input_Mode;    //USB DET
//   P17_Input_Mode;    //CHARGE DONE
//   P30_Input_Mode;    //BAT DET


   //---------V1.4----end-------   
    
   //pwm init
   pwm_init();     


   //i2c init
   Init_I2C();     
   
   //read from i2c
   i2c_read(FIRSTI2CREG, sbuf2, 5); 
   
   //close i2c
   Timer0_Delay1ms(2); 
   clr_I2CEN; 
   
   
   //read the datas
   //--ReadDataFlash( FIRSTCONFIGREG,sbuf2,5);    
   
   //sprintf (stemp,"reg: %bu,%bu,%bu,%bu,%bu\r\n",sbuf2[0],sbuf2[1],sbuf2[2],sbuf2[3],sbuf2[4]); 
   sprintf (stemp,"reg: %bu,%bu,%bu,%bu,%bu\r\n",sbuf2[0],sbuf2[1],sbuf2[4],sbuf2[3],sbuf2[2]); 
   iputs0(stemp); 
 
   
   if(sbuf2[0]==1)
   {
      flg_voicemode = sbuf2[1];
      
      //ibtvol        = sbuf2[2];         //--keep
      ibtvol        = BTVOLSTART;
      
      inoisebasevol = sbuf2[3];   
      WN_MODE       = sbuf2[4];  
            
   }
   else
   { 
      flg_voicemode = 0;      
      ibtvol = BTVOLSTART;      
      inoisebasevol = NOISEBASEVOLSTART;      
      WN_MODE =1;
   }   
   
   inoisevolume=get_noisevolume(WN_MODE);
   sprintf(stemp,"mode: %bu,%bu,%bu\r\n",WN_MODE,inoisebasevol,inoisevolume );
   iputs0(stemp);
   Switch_Delay1ms(20);


   //i2c data check
   if(flg_voicemode>1)
      flg_voicemode =0;   
   if(inoisebasevol > NOISEBASEVOLTOP)
      inoisebasevol =NOISEBASEVOLSTART;   
   if( (WN_MODE ==0) || (WN_MODE >6) )
      WN_MODE = 1;   
 
   
   flg_standby =1;
   flg_reset =0;
   
   
   //test
   flg_voicemode =1;
   WN_MODE =1;
   inoisebasevol =0;
   
   
   whitenoise_switch();
   
   if(flg_voicemode==0)
      flg_stopsendpair =0;
   else
      flg_stopsendpair =1;
   
   
#ifdef SNOOZBABY   
   flg_voicemode =1;   
#endif   


   while(1)
   {
      
      //static UINT32 imaincount =0;  
      static UINT8 flg_pairfirst =0;     
      
            
      //powerkey scan
      powerkey_scan();      

      //powerkey check
      powerkey_check();  
      
      //dowith power key  
      powerkey_process();    

    
      
      if(flg_standby ==1)
      {
         P01 =0;
         P16 =1;   
      }      
      else
      {   
      
         if(flg_voicemode==0)
         {
         
            if(P02)   //pairing
            {
               static UINT32 ipairledcount =0;
               static UINT32 ipairtonecount =0;
               
    
               if (ipairledcount++>PAIR_LEDCOUNT)
               {
                  ipairledcount =0;
                  P01 = ~P01;                  
               }            
               
               ipairtonecount++;
               
               if( flg_pairfirst ==0  )
               {
                  
                  if (ipairtonecount>5)
                  {
                     ipairtonecount =0;    
                     
                     flg_pairfirst =1;
                     
                     if( flg_stopsendpair ==0) 
                     {
                        flg_pairing =1; ipairingcount =0;
                        
                        iputbytes0(spairingvoice,5);                  
                        Switch_Delay1ms(10); 
                     }   
                  }                  
               }
               else if (ipairtonecount>PAIR_TONECOUNT)
               {
                  ipairtonecount =0;                       

                  if(flg_stopsendpair ==0) 
                  {
                     flg_pairing=1; ipairingcount =0;
                     
                     iputbytes0(spairingvoice,5);                  
                     Switch_Delay1ms(10); 
                  }                     
               }            
               
               P16=0;  
            }
            else
            {
               P01 =0;   //pair  led   
               P16 =1;   //charge led
               
               flg_pairfirst=0;
            }
            
         }
         else
         {
            P01 =0;
            P16 =1;
         }      
         
         
//         if(imaincount++>=300)
//         {
//            imaincount =0;
//            
//            sprintf(stemp,"mode: %bu,%bu,%bu\r\n",WN_MODE,inoisebasevol,inoisevolume );
//            //--iputs0(stemp);
//            Switch_Delay1ms(20); 
//         }
         

//         if(imaincount++>=50)
//         {
//            imaincount =0;  
//            
//            sprintf(stemp,"p,c,s,a: %bu,%d,%bu,%bu\r\n",flg_pairing,ipairingcount,flg_stopsendpair,flg_switchaux );
//            iputs0(stemp);
//            Switch_Delay1ms(20);       
//           
//         }         
         
         
         //get adc
         getadcvalue();   
         

         //adc key scan
         adckey_scan();   
         
         
         //P0f =~P0f;    //--timer line
         
         //adc key check         
         adckey_check();     

         //adc key process
         adckey_process( );   
         
         
         if( (flg_switchaux==1) || (ikeynum3 >0) )
         {
            flg_stopsendpair =1;             
            
            //if(flg_switchaux==1) 
            //   waitpairing();      

            if(flg_switchaux==1) 
            {
              if (flg_pairing ==1) 
                  goto nextstep;
               
               flg_switchaux =0;         
               switch2AUX(); 
            }            
         }      

         if(ikeynum3==0)
         {
            if(flg_voicemode==0)
               flg_stopsendpair =0;
            else
               flg_stopsendpair =1;
         }            
   
         
nextstep:
         if(flg_pairing ==1)
         {         
            ipairingcount++; 
            
            if(ipairingcount >=PAIR_SWITHCOUNT)
            {
               ipairingcount =0;
               flg_pairing=0;
            }
         }          
        
      }      
      
   }

}






