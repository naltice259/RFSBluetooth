/*
 * CHC05.h
 *
 *  Created on: 2016/11/17
 *      Author: Nick
 */
#ifndef CHC05_H__
#define CHC05_H_

#include "terasic_includes.h"




class CHC05 {
protected:
	int NumDeviceMax;


public:
	CHC05();
	virtual ~CHC05();

	bool  SetupSend(CUart Uart,char *cmd);
	bool  MessageSend(CUart Uart,char *cmd, int len);
	bool  SetInquireDevice(CUart Uart,int mode,int NumDevice,int time);
	bool  DeviceList(CUart Uart,char *cmd, char ** szDeviceData);
	bool  GetNameList(CUart Uart,char *cmd);
	bool  CompareData(CUart Uart,char *cmd);

	bool  MessageRead(CUart   &Uart, char *pBuffer, int nBufferSize, int *prx_lenmand);
	bool  CommandRead(CUart   &Uart, char *pBuffer, int nBufferSize, int *prx_lenmand);
};

#endif /* CUart_H_ */



