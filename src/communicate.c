//#include "stdafx.h"
#include "../inc/global.h"

typedef struct {
	int Timer;
	int Sec;
}JumpSecStru;

JumpSecStru   g_MyJumpSec = {0};

int CommProcess(void)
{
	u8 PinEntryTimes, tmp[32], i;
	u8 recvBuf[SENDRECVPACKLEN], sendBuf[SENDRECVPACKLEN];
	u16 PackLen;
	int ret;

	memset(recvBuf, 0, sizeof(recvBuf));
	memset(sendBuf, 0, sizeof(sendBuf));

#ifdef _RECVDATA_DEBUG_	//make this transaction as offline transaction 
	PosCom.stTrans.IccOnline = 0;
	memset(PosCom.stTrans.SysReferNo, 0, sizeof(PosCom.stTrans.SysReferNo));
	BcdToAsc_Api(PosCom.stTrans.SysReferNo, PosCom.stTrans.TradeDate, 8);
	BcdToAsc_Api(PosCom.stTrans.SysReferNo+8, PosCom.stTrans.TradeTime, 4);
	BcdToAsc_Api(PosCom.stTrans.AuthCode, PosCom.stTrans.TradeTime, 6);;
#endif

	DispTradeTitle(PosCom.stTrans.Trans_id);
	if(PosCom.stTrans.IccOnline == 0)
	{
		strcpy(PosCom.stTrans.szRespCode, "00");
		TraceNoInc();						// 
		return 0;
	}

	//online process
	ScrClrLine_Api(LINE2, LINE5);
	ScrDisp_Api(LINE3, 0, "Processing", CDISP);
	ScrDisp_Api(LINE4, 0, "please Wait", CDISP);		
	memset(sendBuf, 0, sizeof(sendBuf));
	ret = CommMakeSendbuf(sendBuf, &PackLen);
	if(ret != 0)
		return ret;

	memset(recvBuf, 0, sizeof(recvBuf));
	ret = SendRecvPacket(sendBuf, PackLen, recvBuf, &PackLen);
	if(ret != 0)
		return ret;

	//Resolve Packet....   		
	return ret;
}
 
int CommMakeSendbuf(u8 *sendBuf, u16 *pLen)
{
	//build request message 
	return 0;
}

 

int ModemConnect(unsigned char Timeout)
{
	int ret;
	char dispbuf[32];
	
	ScrBackLight_Api(60);		// 
	while(1)
	{
		ScrClrLine_Api(LINE2, LINE5);
		ScrDisp_Api(LINE3, 0, "connecting...", CDISP);
		ScrDisp_Api(LINE4, 0, "<ESC>-quit", CDISP);
		
		LedLightOff_Api(0x0f);   
		LedLightOn_Api(0x01);    
		
		ScrBackLight_Api(Timeout+10);
		StartJumpSec();
		CommStart_Api();
		ret = CommCheck_Api(60);
		if(ret == 9)
		{
			ScrClrLine_Api(LINE2, LINE5);
			ScrDisp_Api(LINE2, 0, "put phone back to base", CDISP);
			ScrDisp_Api(LINE3, 0, "any key to redail", CDISP);
			StopJumpSec();
			LedLightOff_Api(0x01);   
			LedLightOn_Api(0x04);    
			WaitAnyKey_Api(60);
		}
		else		 
		{
			break;
		}
	}

	switch(ret)
	{
		case 0:
			break;
		case 1: 
			ScrDisp_Api(LINE2, 0, "busy", CDISP);
			ret = ERRCODE_ENGAGED;
			break;
		case 2: 
			ScrDisp_Api(LINE2, 0, "attach tephone line", CDISP);
			ret = ERRCODE_NOLINE;
			break;
		case 3: 
			ScrDisp_Api(LINE2, 0, "busy", CDISP);
			ret = ERRCODE_ENGAGED;
			break;
		case 4: 
			ScrDisp_Api(LINE2, 0, "wave lost", CDISP);
			ret = ERRCODE_WAVELOST;
			break;
		case ESC:
			ScrDisp_Api(LINE2, 0, "user cancel", CDISP);
			ret = DIAL_CANCEL;
			break;
		case 8:		 
		case 0xFF:
		default:
			sprintf(dispbuf, "connect failed %02x", ret);
			ScrDisp_Api(LINE2, 0, dispbuf, CDISP);
			ret = ERRCODE_CONNECTFAIL;
			break;
	}
	
	if(ret != 0)
	{
		CommHangUp();
		WaitAnyKey_Api(1);
	}
	StopJumpSec();
	LedLightOff_Api(0x01);   
	LedLightOn_Api(0x04);    
	
	return ret;
}
 
int SendPacket(unsigned char *SendData, unsigned short PacketLen)
{
	return( CommTxd_Api(SendData, PacketLen, 0) );
}
 
int RecvPacket(u8 *Packet, unsigned short *PacketLen, int WaitTime)
{
	u8 Ret;
	
	LedLightOff_Api(0x0f);   
	LedLightOn_Api(0x08);    
	
	StartJumpSec();
	
	Ret = CommRxd_Api(Packet, PacketLen, 0, 1, gCtrlParam.tradeTimeoutValue*1000);
	
	StopJumpSec();
	LedLightOff_Api(0x08);   
	LedLightOn_Api(0x04);    

	if( Ret== 0x00)
	{
		return 0;
	}
	else
	{
		CommHangUp();
		return E_REVTIMEOUT;
	}
}
 
int SendRecvData(u8 *SendBuf, long Senlen, u8 *RecvBuf, u16 *RecvLen, int psWaitTime)
{
	int ret;
	
	ScrDisp_Api(LINE3, 0, "sending", CDISP);
	if(SendPacket(SendBuf, (u16)Senlen) != 0)
		return E_SEND_PACKET;						 
	
	ScrDisp_Api(LINE3, 0, "receiving", CDISP);
	*RecvLen = 0;
	ret = RecvPacket(RecvBuf, (u16*)RecvLen, psWaitTime);

	return ret;
}
 
int SendRecvPacket(u8 *SendBuf, u16 Senlen, u8 *RecvBuf, u16 *pRecvLen)
{
	int ret;
	
	ret = Connect(60);
	if(ret != 0)
	{
		if(ret < 0)
			return NO_DISP;
		
		ScrClrLine_Api(LINE2, LINE5);
		ScrDisp_Api(LINE2, 0, "dail failed", CDISP);
		ScrDisp_Api(LINE3, 0, "redail or not", CDISP);
		ScrDisp_Api(LINE5, 0, "YES      NO", CDISP);
		ret = WaitEnterAndEscKey_Api(20);
		if(ret != ENTER)
			return E_ERR_CONNECT;							 
		else                                                 
		{                                                    
			ScrClrLine_Api(LINE2, LINE5);                    
			if(Connect(60) != 0)                             
				return E_ERR_CONNECT;                        
		}                                                    
	}                                                        
	                                                                                                              
	TraceNoInc();                                                                                             
                                      
	//if(SaveReversalData("06") != 0)   //for transactions that have reversal						 
	ret = SendRecvData(SendBuf, Senlen, RecvBuf, pRecvLen, gCtrlParam.tradeTimeoutValue);//60
	if(ret != 0)
	{
		if(ret == E_REVTIMEOUT)
		{
			ReadReversalData();
			SaveReversalData("98");	 
		}                            
		else                         
			ret = E_RECV_PACKET;	 
	}                                
	                                 
	return ret;
}
 
void CommHangUp()
{	
	CommClose_Api();
	return;
}
 
int Connect(unsigned char TimeOut)
{
	switch(ModemConnect(TimeOut))
	{
		case 0:
			return 0;
		case DIAL_CANCEL:
			return DIAL_CANCEL;
		default:
			return ERRCODE_DIALFAIL;				 
	}
}


void DispJumpSec(void)
{
	char DispBuf[20];
	
	memset(DispBuf, 0, sizeof(DispBuf));
	g_MyJumpSec.Sec++;
	sprintf(DispBuf, "%d", g_MyJumpSec.Sec);
	ScrDisp_Api(LINE5, 0, DispBuf, CDISP);
}
 
int StartJumpSec(void)
{
	g_MyJumpSec.Timer = CommStartJumpSec_Api(DispJumpSec);
	return 0;
} 
void StopJumpSec(void)
{
	g_MyJumpSec.Sec = 0;
	CommStopJumpSec_Api(g_MyJumpSec.Timer);
}




