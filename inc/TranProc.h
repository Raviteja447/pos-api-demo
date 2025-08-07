
#ifndef AFX_TRANPROC_H
#define AFX_TRANPROC_H


int TransProcess(void);
 
int OnlineTransProcess(void);
 
int SaleInput(void);
 
int SaleVoidInput(void);
 
int QueBalanceInput(void);
 
void DispResult(int rej);


void ShowLog(LOG_STRC *pLog);
 
BOOL SearchLogByTraceNo(LOG_STRC *pLog, u32 traceNo);
 

void GetPinpadError(int error, char *psDisp);


#endif




