
#ifndef I2C_VCNL3020_H
#define I2C_VCNL3020_H

//#include "Common.h"


extern UINT8 flg_addr_log;


void Init_I2C_soft(void);
void VCNL3020_WriteOneByte(UINT8 iaddr, UINT8 idata1);
UINT8 VCNL3020_ReadOneByte(UINT8 iaddr);



#endif


