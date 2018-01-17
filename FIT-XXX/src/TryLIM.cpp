/*
 * TryLIM.cpp 
 * Example of decoding.
 *
*/
#include "stdafx.h"

#include "EquipmentComm/EquipmentComm.h"
#include "LIM/lim.h"


#define IO_OUTNUM  3
#define IO_INNUM   2
char *szIP = "192.168.1.201";

int g_IO_OutSts[IO_OUTNUM];
int g_IO_InSts[IO_INNUM];

bool connected = false;
int nFrameNum = 0;

#define	MAX_FRAME_NUM    100
#define	USER_DATA        NULL
#define	USER_TASK_CID    1

// LIM_CODE_LDBCONFIG
void LIM_CODE_LDBCONFIG_Decoding(LIM_HEAD* lim)
{
	if (lim->nCode != LIM_CODE_LDBCONFIG)
		return;
	printf("#%d LMD Frame: LIM_CODE_LDBCONFIG--------------------\n", nFrameNum);
	ULDINI_Type *ldtype = (ULDINI_Type*)LIM_ExData(lim);
	printf("----Product Info\n");
	printf("Type: %s\n", ldtype->szType);
	printf("Manufacturer: %s\n", ldtype->szManufacturer);
	printf("ReleaseDate: %s\n", ldtype->szReleaseDate);
	printf("SerialNo: %s\n", ldtype->szSerialNo);

	printf("----Network Configuration\n");
	printf("szMAC: %s\n", ldtype->szMAC);
	printf("szIP: %s\n", ldtype->szIP);
	printf("szMask: %s\n", ldtype->szMask);
	printf("szGate: %s\n", ldtype->szGate);
	printf("szDNS: %s\n", ldtype->szDNS);

	printf("----Measurement Parameters\n");
	printf("MR: %d cm\n", ldtype->nMR);
	printf("ESAR: %d¡ã(%d¡ã~%d¡ã) \n", ldtype->nESAR, ldtype->nESA[0]/1000, ldtype->nESA[1]/1000);
	printf("SAR: %d¡ã(%d¡ã~%d¡ã) \n", ldtype->nSAR, ldtype->nSA[0], ldtype->nSA[1]);
	printf("SAV: %d \n", ldtype->nSAV);
	printf("SAP: %3.3f¡ã \n", ldtype->nSAP / 1000.);
	printf("PF: %d \n", ldtype->nPF);

	printf("\t------------------------------------------\n\n");
}

// LIM_CODE_LMD LIM
void LIM_CODE_LMD_Decoding(LIM_HEAD*lim)
{
	if (lim->nCode != LIM_CODE_LMD)
		return;
	LMD_INFO* lmd_info = LMD_Info(lim);  
	LMD_D_Type* lmd = LMD_D(lim);       

	//LMD_INFO
	printf("#%d LMD Frame: LIM_CODE_LMD--------------------\n", nFrameNum);

	printf("Range<%5.2fM> Angle<%5.3f¡ã- %5.3f¡ã> AngleStep<%3.3f¡ã>\nRPM<%drpm> MeasurementData_Num<%d>\n", \
		lmd_info->nRange / 100., lmd_info->nBAngle / 1000., \
		lmd_info->nEAngle / 1000., lmd_info->nAnglePrecision / 1000., \
		lmd_info->nRPM, lmd_info->nMDataNum);


	printf("    LMD  %5.3f¡ã:%d cm", (float)(lmd_info->nBAngle + 0 * (float)lmd_info->nAnglePrecision) / 1000.0, lmd[0]);
	printf("    %5.3f¡ã:%d cm", (float)(lmd_info->nBAngle + 1 * (float)lmd_info->nAnglePrecision) / 1000.0, lmd[1]);
	printf("... ... ... ...");
	printf("%5.3f¡ã:%d cm\n",
		(float)(lmd_info->nBAngle + (lmd_info->nMDataNum - 1) * (float)lmd_info->nAnglePrecision) / 1000.0, lmd[(lmd_info->nMDataNum - 1)]);
	printf("\t------------------------------------------\n\n");
}

// LIM_CODE_LMD_RSSI LIM
void LIM_CODE_LMD_RSSI_Decoding(LIM_HEAD *lim)
{
	if (lim->nCode != LIM_CODE_LMD_RSSI)
		return;
	LMD_INFO* lmd_info = LMD_Info(lim); 
	LMD_D_Type* lmd = LMD_D(lim);      
	LMD_D_RSSI_Type *lmdrssi = LMD_D_RSSI(lim);

	printf("#%d LMD Frame: LIM_CODE_LMD_RSSI--------------------\n", nFrameNum);


	printf("Range<%5.2fM> Angle<%5.3f¡ã- %5.3f¡ã> AngleStep<%3.3f¡ã>\nRPM<%drpm> MeasurementData_Num<%d>\n", \
		lmd_info->nRange / 100., lmd_info->nBAngle / 1000., \
		lmd_info->nEAngle / 1000., lmd_info->nAnglePrecision / 1000., \
		lmd_info->nRPM, lmd_info->nMDataNum);

	printf("    LMD  %5.3f¡ã:%d cm RSSI:%d", (float)(lmd_info->nBAngle + 0 * (float)lmd_info->nAnglePrecision) / 1000.0, lmd[0], lmdrssi[0]);
	printf("    %5.3f¡ã:%d cm RSSI:%d", (float)(lmd_info->nBAngle + 1 * (float)lmd_info->nAnglePrecision) / 1000.0, lmd[1], lmdrssi[1]);
	printf("... ... ... ...");
	printf("%5.3f¡ã:%d cm RSSI:%d\n",
		(float)(lmd_info->nBAngle + (lmd_info->nMDataNum - 1) * (float)lmd_info->nAnglePrecision) / 1000.0, lmd[(lmd_info->nMDataNum - 1)], lmdrssi[(lmd_info->nMDataNum - 1)]);
	printf("\t------------------------------------------\n\n");
}

// LIM_CODE_FMSIG LIM
void LIM_CODE_FMSIG_Decoding(LIM_HEAD *lim)
{
	if (lim->nCode != LIM_CODE_FMSIG)
		return;
	int fieldIdx;
	int alm;
	int attentionW, attentionA;
	int alertW, alertA;
	int alarmW, alarmA;

	fieldIdx = lim->Data[0];
	alm = lim->Data[1];

	alarmA = (alm & 0x01);
	alarmW = (alm & 0x02) >> 1;
	alertA = (alm & 0x04) >> 2;
	alertW = (alm & 0x08) >> 3;
	attentionA = (alm & 0x10) >> 4;
	attentionW = (alm & 0x20) >> 5;

	printf("#%d LMD Frame: LIM_CODE_FMSIG--------------------\n", nFrameNum);
	printf("field_%d FMSIG: %d  %d  %d\n", fieldIdx, alarmA, alertA, attentionA);
	printf("  whole FMSIG: %d  %d  %d\n", alarmW, alertW, attentionW);
	printf("\t------------------------------------------\n\n");
}

// LIM_CODE_IOSTATUS LIM
void LIM_CODE_IOSTATUS_Decoding(LIM_HEAD *lim)
{
	if (lim->nCode != LIM_CODE_IOSTATUS)
		return;

	int iosts = lim->Data[0];

	for (int i = 0; i < IO_OUTNUM; i++)
		g_IO_OutSts[i] = ((iosts & (1 << i)) >> i);

	for (int i = IO_OUTNUM; i < IO_OUTNUM + IO_INNUM; i++)
		g_IO_InSts[i - IO_OUTNUM] = ((iosts & (1 << i)) >> i);

	printf("#%d LMD Frame: LIM_CODE_IOSTATUS--------------------\n", nFrameNum);
	for (int i = 0; i < IO_OUTNUM; i++)
		printf(" OUT_%d: %d", i + 1, g_IO_OutSts[i]);
	printf("\n");
	for (int i = 0; i < IO_INNUM; i++)
		printf(" IN_%d: %d", i + 1, g_IO_InSts[i]);
	printf("\n\t------------------------------------------\n\n");
}

// LIM_CODE_ALARM LIM
void LIM_CODE_ALARM_Decoding(LIM_HEAD *lim)
{
	int type;
	if (lim->nCode != LIM_CODE_ALARM)
		return;
	type = lim->Data[0];
	printf("#%d LMD Frame: LIM_CODE_ALARM--------------------\n", nFrameNum);
	if (type == LIM_DATA_ALARMCODE_INTERNAL)
		printf("Equipment Alarm: Internal error\n");
	else if (type == LIM_DATA_ALARMCODE_Occluded)
		printf("Equipment Alarm: Occluded or too dirty\n");
	else if (type == LIM_DATA_ALARMCODE_High_Temperature)
		printf("Equipment Alarm: Temperature too high\n");
	else if (type == LIM_DATA_ALARMCODE_Low_Temperature)
		printf("Equipment Alarm: Temperature too low\n");

	printf("\t------------------------------------------\n\n");
}

// LIM_CODE_DISALARM LIM
void LIM_CODE_DISALARM_Decoding(LIM_HEAD *lim)
{
	int type;
	if (lim->nCode != LIM_CODE_DISALARM)
		return;
	type = lim->Data[0];
	printf("#%d LMD Frame: LIM_CODE_DISALARM--------------------\n", nFrameNum);
	if (type == LIM_DATA_ALARMCODE_INTERNAL)
		printf("Equipment DisAlarm: Internal error\n");
	else if (type == LIM_DATA_ALARMCODE_Occluded)
		printf("Equipment DisAlarm: Occluded or too dirty\n");
	else if (type == LIM_DATA_ALARMCODE_High_Temperature)
		printf("Equipment DisAlarm: Temperature too high\n");
	else if (type == LIM_DATA_ALARMCODE_Low_Temperature)
		printf("Equipment DisAlarm: Temperature too low\n");

	printf("\t------------------------------------------\n\n");
}


void CALLBACK EqCommDataCallBack(int _cid, unsigned int _lim_code, void* _lim, int _lim_len, int _paddr)
{
	printf("EqCommDataCallBack _cid=%d, _lim_code=%d, _lim_len=%d, _paddr=%d\n",
		_cid, _lim_code, _lim_len, _paddr);

	LIM_HEAD *lim = (LIM_HEAD*)_lim;
	unsigned int checksum = LIM_CheckSum(lim);
	if (checksum != lim->CheckSum)          
	{
		printf("\tLIM checksum error!\n");
		return;
	}

	if (LIM_CODE_LDBCONFIG == lim->nCode)
		LIM_CODE_LDBCONFIG_Decoding(lim);

	if (LIM_CODE_LMD == lim->nCode)
	{
		nFrameNum++;
		LIM_CODE_LMD_Decoding(lim);
	}

	if (LIM_CODE_LMD_RSSI == lim->nCode)
	{
		nFrameNum++;
		LIM_CODE_LMD_RSSI_Decoding(lim);
	}

	if (LIM_CODE_FMSIG == lim->nCode)
		LIM_CODE_FMSIG_Decoding(lim);

	if (LIM_CODE_IOSTATUS == lim->nCode)
		LIM_CODE_IOSTATUS_Decoding(lim);

	if (LIM_CODE_ALARM == lim->nCode)
		LIM_CODE_ALARM_Decoding(lim);

	if (LIM_CODE_DISALARM == lim->nCode)
		LIM_CODE_DISALARM_Decoding(lim);

}


void CALLBACK EqCommStateCallBack(int _cid, unsigned int _state_code, char* _ip, int _port, int _paddr)
{
	printf("EqCommStateCallBack _cid=%d, _state_code=%d, _ip=%s, _port=%d _paddr=%d\n",
		_cid, _state_code, _ip, _port, _paddr);

	if (_state_code == EQCOMM_STATE_OK)   
	{
		LIM_HEAD* lim = NULL;
		LIM_Pack(lim, USER_TASK_CID, LIM_CODE_GET_LDBCONFIG, NULL); 
		SendLIM(USER_TASK_CID, lim, lim->nLIMLen);     
		LIM_Release(lim);                              

		connected = true;
	}
	else
	{
		connected = false;

		printf("Connection failed.code:%d\n",_state_code);
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	LIM_HEAD* lim = NULL;

	printf("GetEquipmentCommVersion version =%d\n", GetEquipmentCommVersion()); 

	EquipmentCommInit(USER_DATA, EqCommDataCallBack, EqCommStateCallBack);     

	OpenEquipmentComm(USER_TASK_CID, szIP, LIM_USER_PORT);                     

	while (MAX_FRAME_NUM > nFrameNum)
	{
		Sleep(5000); 

		if (connected == true)
		{
			LIM_HEAD* lim = NULL;
			LIM_Pack(lim, USER_TASK_CID, LIM_CODE_FMSIG_QUERY,NULL); 
			lim->Data[0] = 0;
			SendLIM(USER_TASK_CID, lim, lim->nLIMLen);              
			LIM_Release(lim);                                       
			Sleep(1000);

			LIM_Pack(lim, USER_TASK_CID, LIM_CODE_HB, NULL); 
			SendLIM(USER_TASK_CID, lim, lim->nLIMLen);
			LIM_Release(lim);
			Sleep(1000);

			
			LIM_Pack(lim, USER_TASK_CID, LIM_CODE_IOREAD,NULL); 
			SendLIM(USER_TASK_CID, lim, lim->nLIMLen);
			LIM_Release(lim);
			Sleep(1000);

			LIM_Pack(lim, USER_TASK_CID, LIM_CODE_IOSET, NULL); 
			lim->Data[0] = 0; // value
			lim->Data[1] = 1; // index
			SendLIM(USER_TASK_CID, lim, lim->nLIMLen);
			LIM_Release(lim);
			Sleep(1000);

			LIM_Pack(lim, USER_TASK_CID, LIM_CODE_IOSET_RELEASE, NULL);
			SendLIM(USER_TASK_CID, lim, lim->nLIMLen);
			LIM_Release(lim);
			Sleep(1000);

			LIM_Pack(lim, USER_TASK_CID, LIM_CODE_START_LMD);
			SendLIM(USER_TASK_CID, lim, lim->nLIMLen);
			LIM_Release(lim);
			Sleep(500);

			LIM_Pack(lim, USER_TASK_CID, LIM_CODE_STOP_LMD); 
			SendLIM(USER_TASK_CID, lim, lim->nLIMLen);
			LIM_Release(lim);

		}

		if (!connected)
			break;
	}

	if (connected)
	{
		
		LIM_HEAD* lim = NULL;

		LIM_Pack(lim, USER_TASK_CID, LIM_CODE_IOSET_RELEASE, NULL); 
		SendLIM(USER_TASK_CID, lim, lim->nLIMLen);
		LIM_Release(lim);

		LIM_Pack(lim, USER_TASK_CID, LIM_CODE_STOP_LMD); 
		SendLIM(USER_TASK_CID, lim, lim->nLIMLen);       
		LIM_Release(lim);                              
		Sleep(1000);
	}

	CloseEquipmentComm(USER_TASK_CID); 

	EquipmentCommDestory(); 

	printf("press any key to exit\n");
	getchar();

	return 0;
}

