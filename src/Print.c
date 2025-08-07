                 //#include "stdafx.h"
#include "../inc/global.h"


 
void prnShieldPanCardNum(u8 flag, char *prtCard, const char *cardNum)
{
	u8 cardlen;
	
	strcpy(prtCard, cardNum);
	if(flag)
	{
		cardlen = strlen(cardNum);
		if(cardlen > 10)
		{
			if(flag == 1)	// 
				memset(prtCard+6, '*', cardlen-4-6);
			else
				memset(prtCard, '*', cardlen-4);
		}
	}
}
 
void PrintMerchantName(u8 printertype)
{
	u8 Num, k;
	char PrnBuf[128], sTemp[128];

	memset(PrnBuf, 0, sizeof(PrnBuf));
	strcpy(PrnBuf, "MERCHANT NAME:");

	PrnStr_Api(PrnBuf);
	PrnStr_Api(gCtrlParam.MerchantName);
}
 
void PrnFormatCardNum(char *pOutput, char *pCardNO, LOG_STRC *pLog, u8 inputmode, u8 ifSecondCard)
{
	prnShieldPanCardNum(gCtrlParam.ShieldPAN, pOutput, pCardNO);
	
	if( (pLog->IccFallBack)&&(ifSecondCard == 0) )	// 
	{
		strcat(pOutput, " (F)");
	}
	else if(inputmode == PAN_KEYIN)// 
	{
		strcat(pOutput, " (M)");
	}
	else if(inputmode == PAN_ICCARD)// 
	{
		strcat(pOutput, " (I)");
	}
	else if(inputmode == PAN_MAGCARD)// 
	{
		strcat(pOutput, " (S)");
	}
}
 

void PrnDefaultFont(void)
{
	//PrnFontSet_Api(16, 24, 0);
	PrnFontSet_Api(24, 24, 0);
}
int PrintSign()
{
	u8 *bitmapdata = NULL, tSignLog[1024];
	int ret, wide, high, datalen;
	int len = 1000, slen;
	
	memset(tSignLog, 0, sizeof(tSignLog));
	ret = ReadFile_Api(SIGNFILE, (u8 *)tSignLog, 0, &len);
	
	slen = tSignLog[0]*256+tSignLog[1];
	if((ret != 0) || (len != slen+2))
	{
		DelFile_Api(SIGNFILE);
		return -1;
	}
#ifndef Q380_MACHINE
	JBIGMemDecode_Api(tSignLog+2, slen, &bitmapdata, &datalen, &wide, &high);
#endif
	PrnLeftIndSet_Api(80);
	
	ret = PrnLogo_Api(bitmapdata, 0);
	
	PrnLeftIndSet_Api(0);
	free(bitmapdata);
	//DelFile_Api(SIGNFILE);  下一笔交易有签名时再删
	return ret;
}

int PrtTicket_Hot(LOG_STRC *pLog, u8 bErrPrint, u8 bRePrint, u8 curpage, int isPrint)
{
	char PrnBuf[128], sTemp[128]; int ret1, ret2;
	if(bErrPrint == 0)
	{
		PrnFontSet_Api(24, 24, 0x33);
		PrnStr_Api("    RECEIPT");

		PrnFontSet_Api(24, 24, 0);
		if(curpage == 0)
		    PrnStr_Api("                  MERCHANT COPY");
		else if(curpage == 1)
            PrnStr_Api("                CARDHOLDER COPY");
		else 
            PrnStr_Api("                      BANK COPY");
	}
	else
	{
		PrnFontSet_Api(24, 24, 0x33);
		PrnStr_Api("  ERROR VOUCHER");
	}
	PrnStr_Api("-------------------------------------------" );
	//PrnDefaultFont();
 
	PrintMerchantName(0);
 
	PrnStr_Api("MERCHANT NO.: ");
	sprintf(sTemp, "%s", gCtrlParam.MerchantNo);//下一行
	if(strlen(pLog->MerchTermNO))
		sprintf(PrnBuf, "%s%s%s", sTemp, "#", pLog->MerchTermNO); 
	else
		strcpy(PrnBuf, sTemp);
	PrnStr_Api(PrnBuf);

	sprintf(PrnBuf, "TERMINAL NO.: %s", pLog->TerminalNo);
	PrnStr_Api(PrnBuf);

	sprintf(PrnBuf, "OPERATOR NO.: %02d", pLog->OperatorNo);
	PrnStr_Api(PrnBuf);
	PrnStr_Api("-------------------------------------------");

	strcpy(sTemp, "CUP");
	sprintf(PrnBuf, "ISSUER: %s", sTemp);
	PrnStr_Api(PrnBuf);
	strcpy(sTemp, "CCB");		
	sprintf(PrnBuf, "ACQUIRER: %s", sTemp);
	PrnStr_Api(PrnBuf);
	//卡号
	/*if(pLog->SecondAcc[0] != 0)
		PrnStr_Api("TRANSFER OUT: ");
	else
	{
		sprintf(PrnBuf, "CARD NO./CARD TYPE:%s", pLog->szCardUnit);
		PrnStr_Api(PrnBuf);
	}*/

	PrnFormatCardNum(sTemp, pLog->MainAcc, pLog, pLog->EntryMode[0], 0);
	sprintf(PrnBuf, "   %s", sTemp);
	ScrCls_Api();
	ScrDisp_Api(LINE2,0,PrnBuf,CDISP);
	WaitKey_Api();
		
	if(bErrPrint == 0)	 
	{
		PrnFontSet_Api(24, 24, 0);
		PrnStr_Api(PrnBuf);
		PrnDefaultFont();
	}
 
	//PrnFontSet_Api(16, 24, 0);
	memset(PrnBuf, 0, sizeof(PrnBuf));
	if(memcmp(pLog->ExpDate, "\x0\x0", 2) != 0)
		sprintf(PrnBuf, "(EXP DATE): %02x/%02x", pLog->ExpDate[0], pLog->ExpDate[1]);
	PrnStr_Api("-------------------------------------------");
 
	PrnStr_Api("TRANS TYPE:");
	
	ConverTranNameCh(pLog->Trans_id, sTemp);
	ConverTranNameEn(pLog->Trans_id, &sTemp[strlen(sTemp)]);
 

	sprintf(PrnBuf, "BATCH NO.:  %06d", gCtrlParam.lNowBatchNum);
	PrnStr_Api(PrnBuf);          
		
	sprintf(PrnBuf, "VOUCHER NO.:  %06ld", pLog->lTraceNo);	
	PrnStr_Api(PrnBuf);
 
	if(strlen(pLog->AuthCode) != 0)													// 										
	{
		sprintf(PrnBuf, "AUTH NO.:  %.6s", pLog->AuthCode);
		PrnStr_Api(PrnBuf);
	}
 
	sprintf(PrnBuf, "REFER NO.:  %s", pLog->SysReferNo);
	PrnStr_Api(PrnBuf);
 
	PrnStr_Api("DATE/TIME:");             
	
	BcdToAsc_Api(sTemp, pLog->TradeDate, 8);							 
	BcdToAsc_Api(&sTemp[8], pLog->TradeTime, 6);
	strcpy(PrnBuf, "    ");
	memcpy(PrnBuf+4, &sTemp[0], 4);
	memcpy(PrnBuf+8, "/", 1);
	memcpy(PrnBuf+9, &sTemp[4], 2);
	memcpy(PrnBuf+11, "/", 1);
	memcpy(PrnBuf+12, &sTemp[6], 2);
	memcpy(PrnBuf+14, " ", 1);
	memcpy(PrnBuf+15, &sTemp[8], 2);
	memcpy(PrnBuf+17, ":", 1);
	memcpy(PrnBuf+18, &sTemp[10], 2);
	memcpy(PrnBuf+20, ":", 1);
	memcpy(PrnBuf+21, &sTemp[12], 2);
	PrnBuf[23] = 0;
    PrnStr_Api(PrnBuf);                
	if(bErrPrint == 0)	 
	{
		PrnFontSet_Api(24, 24, 0);
		FormatAmtToDisp_Api(sTemp, pLog->TradeAmount, 0);
		PrnStr_Api("AMOUNT:");
		sprintf(PrnBuf, "  RMB %s", sTemp);
		//ret2 = WriteFile_Api(RECORD,(unsigned char*)sTemp,0,5);
		//TipAndWaitEx_Api("ret2=%s",sTemp);
		PrnHTSet_Api(1);
		PrnStr_Api(PrnBuf);
		PrnFontSet_Api(24, 24, 0);	 
	}
	PrnDefaultFont();
	if(bErrPrint != 0)	 
	{
		PrnStr_Api("(REFERENCE)-------");
		PrnStr_Api( "reversal failed, please deal it to counter" );
	}
	else
	{
		if(curpage == 0 && isPrint == 0){
			PrnStr_Api("CARDHOLDER SIGNATURE:");
			
		}
		 
		#ifdef V71_MACHINE
		if(PosCom.stTrans.EntryMode[0] != PAN_PAYWAVE && PosCom.stTrans.EntryMode[0] != PAN_PAYPASS){
			PrnLogo_Api("sig.bmp",1);
			//TipAndWait_Api("PrnLogo_Api");
			//TipAndWait_Api("DelFile_Api");
			DelFile_Api("sig.bmp");
		}
		#elif  V80B_MACHINE
			if(gCtrlParam.SupportSignPad != 0 && PosCom.stTrans.EntryMode[0] != PAN_PAYWAVE && PosCom.stTrans.EntryMode[0] != PAN_PAYPASS){
				PrnLogo_Api("sig.bmp",1);
				//TipAndWait_Api("PrnLogo_Api");
				//TipAndWait_Api("DelFile_Api");
				DelFile_Api("sig.bmp");
			}
		#endif

		PrnStr_Api("\n\n");
		
		if(bRePrint == TRUE)
			PrnStr_Api( "*********************************again" );
		
		PrnFontSet_Api(24, 24, 0);
		PrnStr_Api("I ACKNOWLEDGE SATISFACTORY RECEIPT OF RELATIVE");
		PrnStr_Api("GOODS/SERVICES");

		PrnStr_Api("(REFERENCE)");		
		switch(pLog->Trans_id)
		{
			case POS_SALE_VOID:
				sprintf(PrnBuf, "original voucher:%06ld", pLog->OldTraceNo); 
				PrnStr_Api(PrnBuf);
				break;
			default:
				break;
		}		
		 
		if(pLog->ucPanSeqNo != 0)
		{
			sprintf(PrnBuf, "CSN:%02d", pLog->ucPanSeqNo);
			PrnStr_Api(PrnBuf);
		}
		 
		if( (pLog->EntryMode[0] == PAN_ICCARD )&&(pLog->nIccDataLen != 0) )
		{
			memset(sTemp, 0, sizeof(sTemp));
			BcdToAsc_Api(sTemp, pLog->sAppCrypto, 16);
			sprintf(PrnBuf, "TC:%s", sTemp);
			PrnStr_Api(PrnBuf);
			sprintf(PrnBuf, "AID:%s", pLog->szAID);
			PrnStr_Api(PrnBuf);
			memset(sTemp, 0, sizeof(sTemp));
			BcdToAsc_Api(sTemp, pLog->sTVR, 10);
			sprintf(PrnBuf, "TVR:%s", sTemp);
			PrnStr_Api(PrnBuf);
			
			memset(sTemp, 0, sizeof(sTemp));
			BcdToAsc_Api(sTemp, pLog->sAIP, 4);
			sprintf(PrnBuf, "AIP:%s", sTemp);
			strcat(PrnBuf, "    ");
			memset(sTemp, 0, sizeof(sTemp));
			BcdToAsc_Api(sTemp, pLog->sTSI, 4);
			sprintf(PrnBuf+strlen(PrnBuf), "TSI:%s", sTemp);
			PrnStr_Api(PrnBuf);
			
			memset(sTemp, 0, sizeof(sTemp));
			BcdToAsc_Api(sTemp, pLog->sATC, 4);
			sprintf(PrnBuf, "ATC:%s", sTemp);
			strcat(PrnBuf, "    ");
			memset(sTemp, 0, sizeof(sTemp));
			BcdToAsc_Api(sTemp, pLog->szUnknowNum, 8);
			sprintf(PrnBuf+strlen(PrnBuf), "UNUM:%s", sTemp);
			PrnStr_Api(PrnBuf);
			
			sprintf(PrnBuf, "APPLAB:%s", pLog->szAppLable);
			PrnStr_Api(PrnBuf);
			//sprintf(PrnBuf, "TermCap:%02x%02x%02x", stEmvParam.Capability[0], stEmvParam.Capability[1], stEmvParam.Capability[2]);
			//PrnStr_Api(PrnBuf);
		}
	}
	PrnStr_Api("----------x----------x----------\n\n\n");
	//TipAndWait_Api("DelFile_Api");
	//DelFile_Api("sig.bmp");
	return 0;
}

int PrtTicket(LOG_STRC *pLog, u8 bErrPrint, u8 bRePrint, int isPrint)
{
	u8 j, key;
	char PrnBuf[128], sTemp[128];

	memset(PrnBuf, 0, sizeof(PrnBuf));
	memset(sTemp, 0, sizeof(sTemp));
	for(j = 0 ; j < 1; j ++)
	{
		ScrCls_Api();
		if(j != 0)	 
		{
			ScrDisp_Api(LINE3, 0, "Press ENTER To Continue", CDISP);
			KBFlush_Api();
			key = WaitEnterAndEscKey_Api(3);
			if(key == ESC)				 
				break;
		}
		else
			ScrDisp_Api(LINE3, 0, "Printing", FDISP|CDISP); 

		PrnClrBuff_Api();
		PrnSetGray_Api(15);
		PrtTicket_Hot(pLog, bErrPrint, bRePrint, j, isPrint);
		PrintData();
	}
	return 0;
}
 

int PrintData(void)
{
	int key, ret;

	while(1)
	{
		ScrCls_Api();
		ScrDisp_Api(LINE3, 0, "Printing", FDISP|CDISP); 		
		ret = PrnStart_Api();
		ScrCls_Api();
		switch(ret)
		{
			case 0:
				PrnClrBuff_Api();
				return 0;
				break;
			case 2:
				ScrDisp_Api(LINE3, 0, "Paper Out", CDISP);
				break;
			case 3:
				ScrDisp_Api(LINE3, 0, "printer too hot", CDISP); 
				break;
			case 4:
				ScrDisp_Api(LINE2, 0, "put your phone back", CDISP);
				ScrDisp_Api(LINE3, 0, "press any key to continue", CDISP);
				break;
			default:
				ScrDisp_Api(LINE2, 0, "print problem", CDISP);
				ScrDisp_Api(LINE3, 0, "press ENTER to retry", CDISP);
				break;
		}
		
		key = WaitAnyKey_Api(100);
		if(key == TIMEOUT || key == ESC)
		{
			PrnClrBuff_Api();
			return -1;
		}
	}
}
 