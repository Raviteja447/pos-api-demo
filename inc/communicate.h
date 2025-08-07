#ifndef AFX_COMMUNICATE_H
#define AFX_COMMUNICATE_H


#define DIAL_CANCEL				-1				 
#define ERRCODE_CONNECTFAIL 	6				 
#define ERRCODE_MDM_FAIL		12				 
#define ERRCODE_NOLINE			13				 
#define ERRCODE_WAVELOST		14				 
#define ERRCODE_NOVOICE			15				 
#define ERRCODE_LINEOFF			16				 
#define ERRCODE_ENGAGED			17				 
#define ERRCODE_MODEMINIERR		18				 
#define ERRCODE_DIALFAIL		19				 
                                                 	 
#define SENDRECVPACKLEN			2048             
 

int CommProcess(void);
 
int CommMakeSendbuf(u8 *sendBuf, u16 *pLen);
 
int ModemConnect(unsigned char Timeout);
 
int SendPacket(unsigned char *SendData, unsigned short PacketLen); 
int RecvPacket(u8 *Packet, unsigned short *PacketLen, int WaitTime); 
int SendRecvData( unsigned char *SendBuf, long Senlen, unsigned char *RecvBuf, unsigned short *RecvLen,int psWaitTime);
 
int SendRecvPacket(u8 *SendBuf, u16 Senlen, u8 *RecvBuf, u16 *pRecvLen);
 
void CommHangUp(void);
 
int Connect(unsigned char TimeOut);

int StartJumpSec(void);
 
void StopJumpSec(void);


#endif

