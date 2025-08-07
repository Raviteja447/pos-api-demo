#ifndef AFX_PRINTP_H
#define AFX_PRINTP_H
#include "../inc/global.h"

 
void prnShieldPanCardNum(u8 flag, char *prtCard, const char *cardNum);
 
void PrnFormatCardNum(char *pTemp, char *pCardNO, LOG_STRC *pLog, u8 inputmode, u8 ifSecondCard);
 
int PrtTicket(LOG_STRC *pLog, u8 bErrPrint, u8 bRePrint, int isPrint);
 
int PrintData(void);
 
#endif

