//#include "stdafx.h"
#include "../inc/global.h"


int TransProcess(void)
{
	int ret;
	
	ret = OnlineTransProcess();	
	DispResult(ret);		 
	if(ret)
		ret = NO_DISP;
	CommHangUp(); 
	return ret;
}
 
int OnlineTransProcess(void)
{
	int ret = 0, reprnflag=0, tret = 0;
	
	ScrCls_Api();
	DispTradeTitle(PosCom.stTrans.Trans_id);
	switch(PosCom.stTrans.Trans_id)
	{                                                    
		case POS_SALE:					 
			ret = SaleInput();           
 			if(ret != 0) return ret;     
			break;                                             
		case POS_SALE_VOID:				 
			ret = SaleVoidInput();       
			if(ret != 0) return ret;     
			break;                       
		case POS_QUE:					 
			ret = QueBalanceInput();     
			if(ret != 0)                 
				return ret;              
			break;                       
		 default:
			return ESC;
	} 
	ret = CommProcess();
	CommDebugInfo("EntryMode aa", PosCom.stTrans.EntryMode, sizeof(PosCom.stTrans.EntryMode), 1);
	if(PosCom.stTrans.EntryMode[0] == PAN_PAYWAVE)
		tret = PaywaveTransComplete();
	else if(PosCom.stTrans.EntryMode[0] == PAN_ICCARD && EmvIsFullTrans() == 0x01)
	{
		tret = EMVICCOnlineTransComplete(ONLINE_FAILED);
		//script process	 
	}

	if(ret != 0)
		return ret;

	if(memcmp(PosCom.stTrans.szRespCode,"00",2) == 0)							 
	{		
		if(tret != EMV_OK)												 
		{     
			//emv save reversal data									 
			return tret;
		}	

		if(PosCom.stTrans.Trans_id == POS_SALE || PosCom.stTrans.Trans_id == POS_SALE_VOID)			 
		{
			ret = SaveLogFile();//存储结构体到文件
			if(ret != 0)
				ErrorPrompt("operate file failed",2);
		}
		
		if(PosCom.stTrans.Trans_id == POS_SALE || PosCom.stTrans.Trans_id == POS_SALE_VOID)		 
		{
			ret = PrtTicket(&PosCom.stTrans, 0, (u8)reprnflag, 0);
			if(ret != 0)
				ErrorPrompt("print failed",2);
		}		 
	}
	return 0;
}

void GetPwd(int result1, int result2){

}
 
int SaleInput(void)
{
	int ret;
	
	DispTitle("Sale");
	ScrDisp_Api(LINE2, 0, "Please input amount:", LDISP);
	if(GetAmount(PosCom.stTrans.TradeAmount) != 0)
		return 1;

	PosCom.HaveInputAmt = 1;
	ret = GetCard(MASK_INCARDNO_MAGCARD|MASK_INCARDNO_ICC|MASK_INCARDNO_PICC, CARD_EMVFULLNOCASH|CARD_QPASSONLINE);
	CommDebugInfo("GetCard", (u8 *)&ret, sizeof(ret), 1);
	if(ret != 0)
		return (ret);

	if(PosCom.stTrans.EntryMode[0] == PAN_KEYIN || PosCom.stTrans.EntryMode[0] == PAN_MAGCARD ) 	
	{
		ret = EnterPIN(0);
		if(ret)
			return ret;
	}
	return 0;
}
 
int SaleVoidInput(void)
{
	int ret, key;
	u32 lTraceNo;
	char inBuf[32];
	LOG_STRC tLog;

	DispTitle("Void");
	if(CheckManagerPassWd() != 0)
		return NO_DISP;
	if(gCtrlParam.iTransNum == 0)
	{
		ErrorPrompt("No Transaction Records", 4);
    	return NO_DISP;
	}

	ScrClrLine_Api(LINE2, LINE5);
	ScrDisp_Api(LINE2, 0, "Please Input The", LDISP);
	ScrDisp_Api(LINE3, 0, "Voucher No.S:", LDISP);
	memset(inBuf, 0, sizeof(inBuf));
	ret = GetScanf(MMI_NUMBER, 1, 6, inBuf, INPUT_TIMEOUT_VAL, LINE4, LINE4, MAX_LCDWIDTH);
	if(ret == ENTER)
	{
		lTraceNo = AscToLong_Api((u8*)&inBuf[1], (u8)strlen(&inBuf[1]) );
		if(SearchLogByTraceNo(&tLog, lTraceNo))
		{
			ShowLog(&tLog);
			key = WaitEnterAndEscKey_Api(30);
			if(key != ENTER)
				return NO_DISP;
		}
		else
		{
			ErrorPrompt("Could Not Find\nThe Log", 3);
			return NO_DISP;
		}
	}
	else
	{
		return NO_DISP;
	}
	if((tLog.Trans_id != POS_SALE)||(tLog.ucRecFalg != RECORDNORMAL))	 
	{
		ErrorPrompt("Not Allowed\nTo Cancel", 3);
		return NO_DISP;
	}
 
	memcpy(&PosCom.stTrans, &tLog, LOG_SIZE);
	PosCom.stTrans.Trans_id = POS_SALE_VOID;
	PosCom.stTrans.OldTraceNo = lTraceNo;
	

	DispTitle("Void");
	ret = GetCard(MASK_INCARDNO_MAGCARD|MASK_INCARDNO_ICC, CARD_EMVSIMPLE);
	if(ret != 0)
		return (ret);

	if(strcmp(PosCom.stTrans.MainAcc, tLog.MainAcc) != 0)
	{
		ErrorPrompt("Current Trading Card NO.", 3);
		return NO_DISP;
	}

	ret = EnterPIN(0);
	if(ret != 0)
		return (ret);

	return 0;
}
 
 
int QueBalanceInput(void)
{
	int ret;

	DispTitle("Get Balance");	
	PosCom.HaveInputAmt = 1;		 
	ret = GetCard(MASK_INCARDNO_MAGCARD|MASK_INCARDNO_ICC, CARD_EMVSIMPLE);
	if(ret != 0)
		return (ret);
								 
	ret = EnterPIN(0);
	if (ret != 0)
		return (ret);
	
	return 0;
}
 
void DispResult(int rej)
{
	char DispBuf[32], sTemp[32];

	memset(DispBuf, 0, sizeof(DispBuf));
	memset(sTemp, 0, sizeof(sTemp));

	ScrClrLine_Api(LINE2, LINE5);
	if(rej == NO_DISP)
		return;
	if(rej == 0)
	{
		if(memcmp(PosCom.stTrans.szRespCode,"00",2) == 0)
		{
			switch(PosCom.stTrans.Trans_id)
			{
				case POS_QUE:	
					memset(DispBuf, 0, sizeof(DispBuf));
					memset(sTemp, 0, sizeof(sTemp));
					memcpy(sTemp, "\x00\x00\x01\x00\x00\x00", 6);
					FormatAmtToDisp_Api(DispBuf, sTemp, 0);
					if(gCtrlParam.pinpad_type == PIN_PED)
					{
						ScrCls_Api();
						ScrDisp_Api(LINE2, 0, "Balance :", FDISP|LDISP);
						ScrDisp_Api(LINE4, 0, DispBuf, RDISP); 
					}
					else
					{
						ScrCls_Api();
						ScrDisp_Api(LINE4, 0, "Please check PINPAD", CDISP); 
						PPScrCls_Api();
						PPDisAmt_Api(DispBuf, 0);				 
						WaitEnterAndEscKey_Api(6);
						PPScrCls_Api();
						return;
					}
					break;
				default:
					ScrCls_Api();
					ErrorPrompt("TXN Succeeded", 3);
					return;
			}
			WaitEnterAndEscKey_Api(30);
			ScrClrLine_Api(LINE2, LINE5);
		}
	}
	else
	{
		ScrCls_Api();
		if(rej == E_TRANS_FAIL)
		{
			if( (memcmp(PosCom.stTrans.szRespCode, "00", 2) == 0)||(strlen(PosCom.stTrans.szRespCode) == 0) )
				ScrDisp_Api(LINE3, 0, "Transaction failed", CDISP);//strcpy(DispBuf, "交易失败");
			else
			{
				//GetRespDiscribe(PosCom.stTrans.szRespCode, DispBuf);
				ScrDisp_Api(LINE3, 0, "Transaction failed", CDISP);
				sprintf(DispBuf, "[%02s]", PosCom.stTrans.szRespCode);
				ScrDisp_Api(LINE4, 0, DispBuf, CDISP);
			}
			WaitAnyKey_Api(5);
			ScrCls_Api();
		}
		else
		{
			ConvErrCode(rej, DispBuf);
			ErrorPrompt(DispBuf, 5);
		}
	}	
	return;
}
 

void ShowLog(LOG_STRC *pLog)
{
	char buf[32], tdName[40];
	
	memset(buf, 0, sizeof(buf));
	memset(tdName, 0, sizeof(tdName));
	
	ConverTranNameCh((u8)pLog->Trans_id, tdName);
	if(pLog->ucRecFalg == RECORDVOID)
		strcat(tdName, "(Revoked)");


	ScrCls_Api();
	ScrDisp_Api(LINE1, 0, tdName, LDISP);

	sprintf(buf, "CAED NO.:(%12s)", pLog->SysReferNo);
	ScrDisp_Api(LINE2, 0, buf, LDISP);
	ScrDisp_Api(LINE3, 0, pLog->MainAcc, FDISP);

	strcpy(buf, "Amount: ");
	FormatAmtToDisp_Api(buf+strlen(buf), pLog->TradeAmount, 0);
	ScrDisp_Api(LINE4, 0, buf, LDISP);
	
	sprintf(buf, "Time: %02x/%02x %02x:%02x:%02x", pLog->TradeDate[2], pLog->TradeDate[3],
	pLog->TradeTime[0], pLog->TradeTime[1], pLog->TradeTime[2]);
	ScrDisp_Api(LINE5, 0, buf, LDISP);	
}
 
 
BOOL SearchLogByTraceNo(LOG_STRC *pLog, u32 traceNo)
{
	int i;
	
	for(i = 0; i < (int)gCtrlParam.iTransNum; ++i)
	{
		if(ReadLog(pLog, i) == 0)
		{
			if(pLog->lTraceNo == traceNo)
				return TRUE;
		}
	}
	return FALSE;
}
 
