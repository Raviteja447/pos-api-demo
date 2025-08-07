//#include "stdafx.h"
#define HX_APP_VARIABLES
#include "../inc/global.h"
#include "../inc/ApiTest.h"

#undef HX_APP_VARIABLES

const APP_MSG App_Msg={
		"Demo-K",				      
		"Demo-Bank",				 
		"D0CCB01",				 
		"VANSTONE",				 
		__DATE__ " " __TIME__,	 
		"",
		0,
		0,
		0,
		"00001001140616"
};
 
int event_main(SET_EVENT_MSG *msg)
{
	return 0;
}
 

void CheckFile(void)
{
	int Len;
	
	FormatFileSystem_Api(0);					 
	ReadCommParam();					 
	ReadCtrlParam();
	ReadOperFile();

}
 void ShowApiMenu(void) {
    int key;
    while (1) {
        ScrCls_Api();
        ScrDisp_Api(LINE2, 0, "1. API GET Test", CDISP);
        ScrDisp_Api(LINE3, 0, "2. API POST Test", CDISP);
        ScrDisp_Api(LINE5, 0, "CANCEL: Exit", CDISP);
        key = WaitKey_Api(30); // Wait for key, 30s timeout

        if (key == KEY1) {
            RunApiGetTest();   // Call GET test
        } else if (key == KEY2) {
            RunApiPostTest();  // Call POST test
        } else if (key == CANCEL) {
            break;             // Exit menu
        }
    }
}
void InitSys(void)
{
	int Key;	

	SetLang_Api(2,3); //API tip can be English
#ifndef VM30_MACHINE
	ScrFontSet_Api(5);
#endif
    CheckFile();
	
	ScrCls_Api();
	ScrDisp_Api(LINE3, 0, APPVERSION, CDISP);
	ScrDisp_Api(LINE4, 0, (char*)App_Msg.Version, CDISP);
	ScrDisp_Api(LINE5, 0, (char*)App_Msg.Descript, CDISP);
	Key = ProFastKey_Api(2);
	if(Key == 159)								//  
	{
		ScrCls_Api();
		ScrDisp_Api(LINE3, 0,"*", CDISP);
		//while(1)
			//Simulation_Api();
	}
	if(Key == 357)	
		RcvFileSysOrder();
	
	KBMute_Api(gCtrlParam.beepForInput);
	CommModuleInit_Api(&G_CommPara);
	CommParamSet_Api(&G_CommPara);

	Common_Init_Api();
	EMV_Init_Api();	
	PayPass_Init_Api();
	PayWave_Init_Api();
	Mir_Init_Api();
	JSpeedy_Init_Api();

	Common_GetParam_Api(&termParam);
	EMV_GetParam_Api(&stEmvParam);

	MagOpen_Api();
	SysConfig_Api("\x02\x01\x01", 3); // when long press [Cancel] key, go to shutdown-menu

#if ( defined(EMVDEBUG))
	EmvAddAppsExp();
	PayPassAddAppExp(0);
	PayWaveAddAppExp();
	MirAddAppExp();
	JSpeedyAddAppsExp();
	AddCapkExample();

	SetMaskterKeyValue(0);
	SetPinkey(0);
#endif

#ifdef _SAVE_APDU_COMMAND_
	Common_DbgEN_Api(1);  //will write APDU command into file "ComLog.dat" 
	//TipAndWait_Api("ComLog.dat");
#endif	

#ifdef _OUTPUTLOG_
	PortOpen_Api(0);
	PortSetBaud_Api(0, 115200, 8, 0, 1);

#endif	
}
 
void DispMainFace(void)
{
	char sDisp[32];
	
	CCBLogo();
	ScrBrush_Api();
}

int main(int argc, char *argv[])
{
	int Result = 1;
	SystemInit_Api(argc, argv);
	InitSys();	
	
	while(1)
	{
		if(Result)
			ScrClsRam_Api();
		DispMainFace();				 
		Result = WaitEvent();
		if(Result == 0xfe)										
			continue;						 
		if(Result != 0)                              
		{                                            
			PhoneHookDisable_Api();                  
			InitPosCom();                                                                     
			switch(Result)							 
			{
				case FUNCTION:	
				  ShowApiMenu();
        continue; 					 
					
				case ENTER:                              
					if(SelectMainMenu() != 0)	 
						continue;                                                
					break;  
				default:
					continue;
			}
			if(PosCom.stTrans.Trans_id)
			{
				TransProcess();
				WaitRemoveICC();
				PiccStop();
			}
		}
	} 
}




