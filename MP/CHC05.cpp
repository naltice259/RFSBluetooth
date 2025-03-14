/*
 * CHC05.cpp
 *
 *  Created on: 2016/11/17
 *      Author: Nick
 */
#include "CUart.h"
#include "CHC05.h"
#include "sys/alt_dev.h"
#include "altera_avalon_uart.h"

CHC05::CHC05():
	// TODO Auto-generated constructor stub
	NumDeviceMax(9)
{

}

CHC05::~CHC05() {
	// TODO Auto-generated destructor stub


}



bool CHC05::MessageRead(CUart   &Uart, char *pBuffer, int nBufferSize, int *prx_lenmand){
	int RxLen;
	char Data;
	bool bDone = false;
	int Len = 0;
	alt_u32 timeout;
	//if (!Uart.NoneBlockingRead((char *)&Data, sizeof(Data), &RxLen) || RxLen == 0)
	//		return false;
	*(pBuffer + 0) = Data;

	timeout = alt_nticks() + (alt_ticks_per_second());
	int maxCnt = 500;
	int cnt = 0;
	Len = 0;
	while(!bDone){
		printf("Entered while!\n");
		if (Uart.NoneBlockingRead((char *)&Data, sizeof(Data),&RxLen) && RxLen == sizeof(Data)){
			*(pBuffer + Len) = Data;
			Len++;
			if (Len == 3)
				bDone = true;
		
		}else if (Len >= nBufferSize){
			// buffer full
			bDone = true;
		}else if (alt_nticks() > timeout || cnt > maxCnt){
			// timeout
			bDone = true;
		}else{
			printf("Waiting for message...\n");
			cnt++;
		}
		usleep(10);

	}
	*prx_lenmand = Len;
	return true;
}



bool CHC05::CommandRead(CUart   &Uart, char *pBuffer, int nBufferSize, int *prx_lenmand){
	int RxLen;
	char Data;
	bool bDone = false;
	int Len = 0;
	alt_u32 timeout;

	if (!Uart.Read((char *)&Data, sizeof(Data), &RxLen) || RxLen == 0)
			return false;

	*(pBuffer + 0) = Data;

	// read until find '\r'
	//timeout = alt_nticks() + alt_ticks_per_second()/1000;
	timeout = alt_nticks() + (alt_ticks_per_second());
	Len = 1;
	while(!bDone){
		if (Uart.NoneBlockingRead((char *)&Data, sizeof(Data), &RxLen) && RxLen == sizeof(Data)){
			*(pBuffer + Len) = Data;
			Len++;
			if (Data == '\n')
				bDone = true;
		}else if (Len >= nBufferSize){
			// buffer full
			bDone = true;
		}else if (alt_nticks() > timeout){
			// timeout
			bDone = true;
		}else{
		}
		usleep(10);

	}
	*prx_lenmand = Len;
	return true;
}


bool CHC05::SetupSend(CUart Uart,char *cmd)
{
	bool bFindOK=false;
	int  nReadLen;
    char szData[35];
    alt_u32 timeout;
    printf("TX-->%s",cmd);

    timeout = alt_nticks() + (alt_ticks_per_second()*30);

	Uart.WriteString(cmd, strlen(cmd));
	while(!bFindOK){
	  if(CommandRead(Uart, szData, sizeof(szData), &nReadLen)){
	      szData[nReadLen] = 0;
	      printf("RX<--%s \n",szData);
	  }
	  if(!(strcmp(szData, "OK\r\n")))
		  bFindOK = true;
	  if(alt_nticks() > timeout){
		  printf("Time Out\n");
		  return false;
	  }

	}
	return bFindOK;
}


bool CHC05::MessageSend(CUart Uart,char *cmd, int len)
{
	bool bOK=false;

	bOK = Uart.WriteString(cmd, len);

	return bOK;
}


bool  CHC05::SetInquireDevice(CUart Uart,int mode,int NumDevice,int time){
	bool bOK = false;
	char szInquire[30];
	sprintf(szInquire,"AT+INQM=%d,%d,%d\r\n",mode,NumDevice,time);
	bOK = SetupSend(Uart,szInquire);
	return bOK;
}

bool CHC05::GetNameList(CUart Uart, char *cmd){

	bool bFindOK=false;
    alt_u32 timeout;
    int i=0;
	int  nReadLen;
    char szData[35];
    timeout = alt_nticks() + (alt_ticks_per_second()*15);
    printf("TX-->%s",cmd);
    Uart.WriteString(cmd, strlen(cmd));
    while(!bFindOK){
    	if(CommandRead(Uart, szData, sizeof(szData), &nReadLen)){
    		szData[nReadLen] = 0;
    		if(!(strcmp(szData,"OK\r\n"))){
    			 bFindOK = true;
    		}else
    			printf("%s\n",szData);

    	}

  	   if(alt_nticks() > timeout){
  		  printf("Time Out\n");
  		  return false;
  	  }
    }
	return bFindOK;
}


bool CHC05::DeviceList(CUart Uart, char *cmd, char **szDeviceData){
	bool bFindOK=false;
	int  nReadLen;
    char szData[36];
    alt_u32 count=0;
    alt_u32 timeout;
    char Address1[9][8],Address2[9][8],Address3[9][8];
    timeout = alt_nticks() + (alt_ticks_per_second()*30);
    printf("TX-->%s",cmd);
	Uart.WriteString(cmd, strlen(cmd));
	while(!bFindOK){
	  if(CommandRead(Uart, szData, sizeof(szData), &nReadLen)){
	      szData[nReadLen] = 0;
		  if(!(strcmp(szData, "OK\r\n"))){
			  bFindOK = true;
			  printf("RX<--%s\n",szData);
		  }
		  else{
	        printf("RX<--%s\n",szData);
            strcpy(szDeviceData[count],szData);
            sscanf(szDeviceData[count],"+INQ:%[^:]:%[^:]:%[^,],%[^,],%s",Address1[count],Address2[count],Address3[count]);
            sprintf(szDeviceData[count],"%s,%s,%s",Address1[count],Address2[count],Address3[count]);
            count++;
		  }
	  }

	  if(alt_nticks() > timeout){
		  printf("Time Out\n");
		  return false;
	  }
	}
	if(bFindOK){
	      for(int j=0;j<=8;j++){
	          for(int i=0;i<=8;i++){
	        	  if(i!=j){
	    	        if(!strcmp(szDeviceData[j],szDeviceData[i])){
	    	        	szDeviceData[i][0]='\0';
	    	        }
	        	  }
	          }
	      }

	      for(int i=0;i<=8;i++){
	          for(int j=0;j<=8;j++){
	        	  if(i<j){
	    	        if(szDeviceData[i][0]=='\0' && szDeviceData[j][0]!='\0'){
	    	        	 strcpy(szDeviceData[i],szDeviceData[j]);
	    	        	 szDeviceData[j][0]='\0';
	    	        }
	        	  }
	          }
	      }
	}
	return bFindOK;
}




