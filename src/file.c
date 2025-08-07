//#include "stdafx.h"
#include "../inc/global.h"

void InitCommParam(void)
{
	memset(&G_CommPara, 0, sizeof(struct _COMMPARASTRUC_));
	
   	G_CommPara.NetCdmaGprsSet.CommAuto = 0;
	G_CommPara.HdlcSet.LinkType = 0;												 
	G_CommPara.DialSet.ReDialTimes = 3;												 
	                                                                                 
	G_CommPara.HdlcSet.IfCheckPhone = 1;											 
   	G_CommPara.HdlcSet.IfCheckDialTone = 1;											 
   	G_CommPara.HdlcSet.CheckToneTime = 0x50;										 
   	G_CommPara.HdlcSet.DelayBeforeDial = 1;											 
   	G_CommPara.HdlcSet.DtmfOnTime = 120;											 
	G_CommPara.HdlcSet.DtmfOffTime = 120;                                            
   	G_CommPara.HdlcSet.WaveLostTime = 30;											 
   	G_CommPara.HdlcSet.SendLevel = 10;												 
    memset(G_CommPara.DialSet.PredialNum,0,sizeof(G_CommPara.DialSet.PredialNum));	 
	memset(G_CommPara.DialSet.InputCenterNum,0,20);									 
    memcpy(G_CommPara.DialSet.Tpdu,"\x60\x00\x08\x00\x00", 5);						 
	strcpy(G_CommPara.DialSet.InputCenterNum,"2226");			//02083012240
	strcpy((char *)G_CommPara.DialSet.InputCenterNum1,"2226");
	strcpy((char *)G_CommPara.DialSet.InputCenterNum2,"2226");
	G_CommPara.DialSet.PredialNum[0] = 0;
	
	strcpy((char *)G_CommPara.GprsSet.GprsApn1, "CMNET");
	strcpy((char *)G_CommPara.GprsSet.GprsIp1, "124.207.195.116");
	strcpy((char *)G_CommPara.GprsSet.GprsPort1, "8000");
//	strcpy((char *)G_CommPara.GprsSet.GprsApn2, "CMWAP");
	strcpy((char *)G_CommPara.GprsSet.GprsIp2, "124.207.195.116");
	strcpy((char *)G_CommPara.GprsSet.GprsPort2, "8000");
#if(defined(V71_MACHINE) || defined(V39_MACHINE))
	G_CommPara.CurCommMode = GPRS;
#elif  (defined(_MACHINE_V37))
	G_CommPara.CurCommMode = COM;
#elif  V80B_MACHINE
	G_CommPara.CurCommMode = LAN;
#endif
}
 
void SaveCommParam(void)
{
	u8 result;

	do
	{
	    result = WriteFile_Api(COMMPARAMFILE,(u8*)&G_CommPara, 0, sizeof(struct _COMMPARASTRUC_));
	}while(result != 0);
}
void SaveTermParam(void)
{
	SaveCommParam();
} 
void ReadCommParam(void)
{
    u32 ReadLen;

    ReadLen = sizeof(struct _COMMPARASTRUC_);
    memset(&G_CommPara,0,sizeof(struct _COMMPARASTRUC_));

    if(ReadFile_Api(COMMPARAMFILE, (u8*)&G_CommPara, 0, &ReadLen) == 0)
         return;							 
                                             
    InitCommParam();						 
    SaveCommParam();
}
 
void SaveCtrlParam(void)
{
	u8 result;

	do
	{
		result = WriteFile_Api(CtrlPARAMFILE,(u8*)&gCtrlParam, 0, sizeof(struct _CtrlParam));
	}while(result != 0);
} 
void ReadCtrlParam(void)
{
    char ret;
    unsigned int ReadLen;

    ReadLen = sizeof(struct _CtrlParam);
	ret = ReadFile_Api(CtrlPARAMFILE, (u8*)&gCtrlParam, 0, &ReadLen);
	if( (ret == 0)&&(ReadLen == sizeof(struct _CtrlParam)) )
		return;
	
	Beep_Api(BEEPERROR);
    InitCtrlParam();											 
    SaveCtrlParam();											 
}
 
void InitCtrlParam(void)
{
	memset(&gCtrlParam, 0, sizeof(gCtrlParam));				 	     						 
	gCtrlParam.lTraceNo    = 1;								 
	gCtrlParam.lNowBatchNum= 1;		 				 
                                                             
	strcpy(gCtrlParam.MerchantName, "merchant test");
	strcpy(gCtrlParam.MerchantNo, "105290045110223");
	strcpy(gCtrlParam.TerminalNo, "00119952");
                                                         
	gCtrlParam.beepForInput			= 0;
	gCtrlParam.oprTimeoutValue		= 60;
	gCtrlParam.tradeTimeoutValue	= 60;
	gCtrlParam.ShieldPAN   = 1;
	
	gCtrlParam.pinpad_type = PIN_PED;		 
	gCtrlParam.AKeyIndes = 0;				 
	gCtrlParam.MainKeyIdx =	1;				 
	gCtrlParam.MacKeyIndes = 0;				 
	gCtrlParam.PinKeyIndes = 1;				 		 
	gCtrlParam.DesType = 0;					 
		 
	                                         
	gCtrlParam.SupportICC = 1;				 			 
	gCtrlParam.SupportPICC = PEDPICCCARD;	 
         
	gCtrlParam.SupportSignPad = 0;	 
	gCtrlParam.SignTimeoutS = 150;	 
	gCtrlParam.SignMaxNum = 8;		 
	gCtrlParam.SignBagMaxLen = 900;	 	     
}
 
int ReadLog( LOG_STRC *pLog, int logNo )
{
	u32 off, Len;

	if (gCtrlParam.iTransNum == 0)							 
		return 1;
	
	if( logNo == LAST_REC_LOG )
		off = (gCtrlParam.iTransNum - 1) * LOG_SIZE;
	else
		off = logNo * LOG_SIZE;
	
	Len = LOG_SIZE;
	if( ReadFile_Api(RECORDLOG, (unsigned char *)pLog, off, &Len) == 0)
	{
		if(Len == LOG_SIZE)
			return 0;
	}
	else 
	{
		return E_MEM_ERR;
	}

	return 0;
}
 
void DelLog(int bClearTraceNo)
{
	DelFile_Api(RECORDLOG);
	gCtrlParam.iTransNum = 0;
	
	if(bClearTraceNo)
	{
		gCtrlParam.lTraceNo = 1;
		SaveCtrlParam();
	}
}
 
int SaveLogFile(void)
{
	int ret;
	LOG_STRC  stLog;
	u8 buf[2];
	u8 buff1[2];
	u8 buf2[2];

	memset(buf,0,sizeof(buf));
	memset(buff1,0,sizeof(buff1));
	memset(buf2,0,sizeof(buf2));
	
	//TipAndWaitEx_Api("RECORDLOG2:%d", GetFileSize_Api(RECORDLOG));
	memcpy(&stLog, &PosCom.stTrans, sizeof(stLog));
	ret = WriteFile_Api(RECORDLOG, (u8*)&stLog, gCtrlParam.iTransNum*LOG_SIZE, LOG_SIZE);
	//TipAndWaitEx_Api("RECORDLOG2:%d", GetFileSize_Api(RECORDLOG));
	buf2[0] = ret;
	buf[0] = (u8)gCtrlParam.iTransNum;
	buff1[0] = (u8)LOG_SIZE;
	//TipAndWaitEx_Api("ret:%d", ret);
	if(ret != 0 )
	{
		return (E_MEM_ERR);
		TipAndWait_Api("ret != 0");
	}
	
	gCtrlParam.iTransNum++;								 
	SaveCtrlParam();
	//TipAndWait_Api("SaveCtrlParam");
 
	if(stLog.Trans_id == POS_SALE_VOID)
	{
		ret = UpdateLogFile();
		if(ret != 0)									
			ErrorPrompt("update log failed", 3);
	}
	
	return(0);
}
 
int UpdateLogFile(void)
{
	u16 i;
	LOG_STRC Log;

	memset(&Log, 0, sizeof(Log));
	
	for(i = 0; i < gCtrlParam.iTransNum; i++)
	{
		if(ReadLog(&Log, i) == 0)
		{
			if(Log.lTraceNo == PosCom.stTrans.OldTraceNo)			// 
			{
				Log.ucRecFalg = RECORDVOID;
				if( WriteFile_Api(RECORDLOG,(u8*)&Log, i*sizeof(Log), LOG_SIZE) != 0)
					return E_MEM_ERR;
				else 
					return 0;
			}
		}
	}
	return 1;
}
 
int SaveSignFile(SIGNLOG *ptSignLog)
{
	int ret;
	u32 filelen;

	filelen = GetFileSize_Api(SIGNFILE);
	ret = WriteFile_Api(SIGNFILE, (u8*)ptSignLog, filelen, SIGNLOGSIZE);
	if(ret != 0)
		return E_MEM_ERR;
	
	gCtrlParam.SingRecNum++;								// 
	SaveCtrlParam();
	return 0;
}
 
int ReadSignFile(SIGNLOG *ptSignLog, u16 recnum)
{
	int ret;
	u32 current, filelen;

	filelen = SIGNLOGSIZE;
	current = recnum*SIGNLOGSIZE;
	ret = ReadFile_Api(SIGNFILE, (u8*)ptSignLog, current, &filelen);
	if(ret != 0)
		return E_MEM_ERR;
	return(0);
}
 
int ReadReversalData(void)
{
	u32 RLen;

	RLen = GetFileSize_Api(DUPFILE);
	ReadFile_Api(DUPFILE, (u8*)&PosCom, 0, &RLen);
	return 0;
}
 
int SaveReversalData(char *pReson)
{
	char stemp[4];
	
	DelFile_Api(DUPFILE);
	memcpy(stemp, PosCom.stTrans.szRespCode, 2);	// 
	strcpy(PosCom.stTrans.szRespCode, pReson);
	if(WriteFile_Api(DUPFILE, (u8*)&PosCom, 0, sizeof(PosCom)) != 0x00 )
		return -1;
	memcpy(PosCom.stTrans.szRespCode, stemp, 2);
	return 0;
}
 
int DelReversalData(void)
{
	if(DelFile_Api(DUPFILE) == 0)
	{
		return 0;
	}
	else 
	{
		return (E_MEM_ERR);
	}
}

int printCurrentLog(int LogNo){
	int ret, reprnflag=0, len, cc; 
	LOG_STRC currentLog; 

	len = sizeof(currentLog);
	ret = ReadFile_Api(RECORDLOG,(unsigned char*)&currentLog,LogNo*sizeof(currentLog),(unsigned int*)&len);//读取结构体

	ret = PrtTicket(&currentLog, 0, (u8)reprnflag,1);//print
	if(ret != 0)
		ErrorPrompt("print failed",2);
}

int ShowLogs(){
	int i,a;unsigned char KEY;
	ScrClrLine_Api(LINE1,LINE1);
	DispTitle("Sales Record");
	for (i=0;i<gCtrlParam.iTransNum;i++)
	{
		DispRecord(i); a = 1;
		while (a<10)
		{
			KEY = GetKey_Api();
			switch(KEY){
			case PGDWON:
				if (i==gCtrlParam.iTransNum-1)
				{
					i = i-1;
					ScrClrLine_Api(LINE8, LINE8);
					ScrDisp_Api(LINE8,0,"This is the last log!", CDISP);
				}else{
					ScrClrLine_Api(LINE8, LINE8);
				}
				a = 10;
				break;
			case PGUP:
				if (i == 0)
				{
					i = i-1;
					ScrClrLine_Api(LINE8, LINE8);
					ScrDisp_Api(LINE8,0,"This is the first log!", CDISP);
				}else{
					i = i-2;
					ScrClrLine_Api(LINE8, LINE8);
				}
				a = 10;
				break;
			case ENTER:
				printCurrentLog(i);
				ScrClrLine_Api(LINE8, LINE8);
				i = i-1;
				a = 10;
				break;
			case ESC:
				return ESC;
				a = 10;
			default:
				break;
			}
		}
	}
}


int DispRecord(int LogNo)
{
	LOG_STRC  stLog;
	unsigned int len = sizeof(stLog), ret1, ret2;
	unsigned char ScrBuf1[24], ScrBuf2[24]; 
	unsigned int *length = &len;
	
	ScrClrLine_Api(LINE2, LINE6); 
	ret2 = ReadFile_Api(RECORDLOG, (u8*)&stLog, LogNo*LOG_SIZE, length);//&len

	sprintf(ScrBuf1, "VOUCHER Num:%06ld", stLog.lTraceNo);	
	ScrDisp_Api(LINE2, 0, ScrBuf1, LDISP);

	sprintf(ScrBuf2, "CARD Num:%s", stLog.szCardUnit);
	ScrDisp_Api(LINE3, 0, ScrBuf2, LDISP);
	PrnFormatCardNum(ScrBuf1, stLog.MainAcc, &stLog, stLog.EntryMode[0], 0);
	sprintf(ScrBuf2, "   %s", ScrBuf1);
	ScrDisp_Api(LINE4, 0, ScrBuf2, LDISP);

	FormatAmtToDisp_Api(ScrBuf1, stLog.TradeAmount, 0);
	sprintf(ScrBuf2, "Amount:RMB %s", ScrBuf1);
	ScrDisp_Api(LINE5, 0, (const char*)ScrBuf2, LDISP);
	BcdToAsc_Api(ScrBuf1, stLog.TradeDate, 8);							 
	BcdToAsc_Api(&ScrBuf1[8], stLog.TradeTime, 6);
	strcpy(ScrBuf2, "Date:");
	memcpy(ScrBuf2+5, &ScrBuf1[0], 4);
	memcpy(ScrBuf2+9, "/", 1);
	memcpy(ScrBuf2+10, &ScrBuf1[4], 2);
	memcpy(ScrBuf2+12, "/", 1);
	memcpy(ScrBuf2+13, &ScrBuf1[6], 2);
	memcpy(ScrBuf2+15, " ", 1);
	memcpy(ScrBuf2+16, &ScrBuf1[8], 2);
	memcpy(ScrBuf2+18, ":", 1);
	memcpy(ScrBuf2+19, &ScrBuf1[10], 2);
	memcpy(ScrBuf2+21, ":", 1);
	memcpy(ScrBuf2+22, &ScrBuf1[12], 2);
	ScrBuf2[24] = 0;
	ScrDisp_Api(LINE6, 0, ScrBuf2, LDISP);

	return ret2;
}