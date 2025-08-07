#include "../inc/global.h"

int SPadSignTest()
{
	int i = 0;
	int Ret = 0;
	int Len = 0;
	int font = 0;
	u8 *pData=NULL;
	
	KBMute_Api(0);
	pData = (u8*)malloc(10*1024);
	//TipAndWait_Api("af pData");
	if(pData == NULL){
		return -1;
	}
	memset(pData,0,sizeof(10*1024));
	ScrBackLight_Api(0xffff);
	ScrCls_Api();
	Ret = SPadOpen_Api(0, 0);
	if(Ret != 0)
	{
		return Ret;
	}

	Ret = SPadMakeSign_Api("Aisino",12,pData,120);
	if(Ret != 0)
	{
		free(pData);
		return Ret;
	}
	SPadClose_Api();
	ScrCls_Api();

	memcpy(&Len,&pData[2],4);//第一第二字节分别是“BM”,第三个字节起4字节是长度
	SaveWholeFile_Api("sig.bmp",pData,Len);
	//TipAndWaitEx_Api("Len%d",Len);
	PrnClrBuff_Api();
	//PrnLogo_Api("sig.bmp",1);
	PrnStart_Api();
	free(pData);
	return 0;
}

int SPadSignExTest()
{
	int i = 0;
	int Ret = 0;
	int Len = 0;
	int font = 0;
	u8 *pData=NULL;
	
	KBMute_Api(0);
	pData = (u8*)malloc(10*1024);
	if(pData == NULL){
		return -1;
	}
	memset(pData,0,sizeof(10*1024));
	ScrBackLight_Api(0xffff);
	ScrCls_Api();
	Ret = SPadOpenEx_Api(1,38400);
	if(Ret != 0)
		return Ret;
	Ret = SPadMakeSignEx_Api("Aisino",12,pData,120);
	if(Ret != 0)
	{
		free(pData);
		return Ret;
	}
	SPadCloseEx_Api();
	ScrCls_Api();

	memcpy(&Len,&pData[2],4);//第一第二字节分别是“BM”,第三个字节起4字节是长度
	SaveWholeFile_Api("sig.bmp",pData,Len);
	//TipAndWaitEx_Api("Len%d",Len);
	PrnClrBuff_Api();
	//PrnLogo_Api("sig.bmp",1);
	PrnStart_Api();
	free(pData);
	return 0;
}