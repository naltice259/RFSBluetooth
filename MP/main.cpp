/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

//#include <stdio.h>
#include "terasic_includes.h"
#include "CUart.h"
#include "CHC05.h"
#include"address_map_nios2.h"
#include "accelerometer.h"



const int MaxList = 9;


void SetConfig(bool bEnable){//setting switch 9 to master
	IOWR(GPIO_CONTROLLER_BASE,0,bEnable);
}

int query_users(void){
    int nChoice = 0;
    scanf("%d", &nChoice);
    printf("%d\r\n", nChoice);
    return nChoice;
}


int main()
{
	accelerometer accelerometer_o;
	CUart Uart;
 	Uart.Open(HC_05_UART_NAME);
 	CHC05 HC05;
 	int nReadLen;
	int RxLen;
 	char szData[45];
 	char ReceiveData[30];
 	int nChoice = 0;
 	char *szDeviceList[MaxList];

 	for(int i=0;i<9;i++)
 		szDeviceList[i] = (char *)malloc(35*sizeof(char));
	
	unsigned char c1 = IORD(PIO_SW_BASE, 0);	// SW[7:0]
	unsigned char c2 = IORD(PIO_SW_BASE, 0)>>8;	// SW[9:8]
	int role = (c2 & 0x02) >> 1;			// SW[9] is role
	int paired = (c2 & 0x01);
	printf("Role is: %d\n", role);
	printf("Paired is: %d\n", paired);
        if (paired == 0) {
 	    printf("HC-05 Master Demo\r\n");
 	    SetConfig(true);
	    HC05.SetupSend(Uart, "AT+ORGL\r\n");
	    usleep(1000000);
	    HC05.SetupSend(Uart, "AT+RESET\r\n");
	    usleep(1000000);
  	    HC05.SetupSend(Uart,"AT+INIT\r\n");
  	    usleep(1000000);
		HC05.SetupSend(Uart,"AT+UART=115200,0,0\r\n"); // Set/Inquire- serial parameter
														//Param1: baud rate, Param2: stop bit, Param3: parity bit
		usleep(1000000);
        }
	else {
	    printf("Already paired!\n");
	}

	if (role == 0 && paired == 0) { //AT+ROLE=0 is slave mode 
	    HC05.SetupSend(Uart,"AT+ROLE=0\r\n");
	    usleep(1000000);
	    SetConfig(false);
	}
	else if (role == 1 && paired == 0) {	//AT+ROLE=1 is master mode
	    HC05.SetupSend(Uart,"AT+ROLE=1\r\n");
	    usleep(1000000);
	    HC05.SetInquireDevice(Uart,1,9,48);
  	    usleep(1000000);
	    HC05.DeviceList(Uart,"AT+INQ\r\n",szDeviceList);

		for (int i = 0; i <= 8; i++) {
			if (!strcmp(szDeviceList[i], "\0") || szDeviceList[i][0] < '0' || szDeviceList[i][0] > '9') {
				continue;
			}
			else {
				printf("Device[%d]:%s\n", i, szDeviceList[i]);
				sprintf(szData, "AT+RNAME?%s\r\n", szDeviceList[i]);
				HC05.GetNameList(Uart, szData);
			}
		}

	    printf("Please input a number to select the desired device.\n");
	    printf("For example, input 0 to select first device.\n");
	    nChoice = query_users();
	    sprintf(szData,"AT+PAIR=%s,%d\r\n",szDeviceList[nChoice],30);
	    HC05.SetupSend(Uart,szData);

	    usleep(1000000);
	    sprintf(szData,"AT+BIND=%s\r\n",szDeviceList[nChoice]);
	    HC05.SetupSend(Uart,szData);
	    SetConfig(false);
	    usleep(1000000);
	    for(int i=0;i<9;i++)
 		free(szDeviceList[i]);
	}
	else {printf("Error! Role is corrupted!\n");}

	volatile int* LEDs = (volatile int*)LED_BASE;


	int temp, x_axis, signed_bit, abs_select_high;

	// Change the activity threshold of the accelerometer
	accelerometer_o.ADXL345_WRITE(0x24, 0x08);

    int cnt = 0;

	// Read the device ID from the accelerometer. Should be 0xE5.
	int device_id = accelerometer_o.ADXL345_READ(0x00) & 0x00FF;
	*LEDs = device_id;



    while(1){
	/*if (HC05.MessageRead(Uart, ReceiveData, sizeof(ReceiveData), &nReadLen))
	{
	    ReceiveData[nReadLen] = '\0';
	    //printf("%s <--RX\n",ReceiveData);
	    //printf("nReadLen: %d\n", nReadLen);
	    unsigned int data = (unsigned int)strtol(ReceiveData, NULL, 16);
	    
	    IOWR(PIO_LED_BASE, 0, data);	//write the received data to the red LEDs
	}*/


	// Read from the INT_SOURCE register and check for ACTIVITY.
	if (accelerometer_o.ADXL345_READ(0x30) & 0x10) {
		// Select and read the lower-order byte of the x-axis value
		x_axis = accelerometer_o.ADXL345_READ(0x32) & 0x00FF;

		// Select and read the higher-order byte of the x-axis value
		temp = accelerometer_o.ADXL345_READ(0x33) & 0x0003;    

		// Combine the two x-axis bytes
		x_axis = ((temp << 8) | x_axis);

		// Only take the sign + 4 MSB
		x_axis = (x_axis & 0x03E0) >> 5;
		signed_bit = (x_axis & 0x10) >> 4;

		// Take the absolute value of the x-axis value
		if (signed_bit)
			x_axis = ~x_axis & 0xF;

		//Write to LED display
		*LEDs = accelerometer_o.LED_val(x_axis, signed_bit);
		IOWR(PIO_LED_BASE, 0, *LEDs);	//write the received data to the red LEDs
	}

		unsigned char c1 = IORD(PIO_SW_BASE, 0);	//read the state of the switches SW[7:0]
		unsigned char c2 = IORD(PIO_SW_BASE, 0) >> 8;	//read the state of the switches SW[9:8]

		char carr[4];
		//sprintf(carr, "%.03x", c2*256 + c1);	//converts this data into a hex string of length 3
		sprintf(carr, " % .04x", accelerometer_o.LED_val(x_axis, signed_bit));
		HC05.MessageSend(Uart, carr, 4);	//the hex string is sen
		//printf("TX --> %.2x\n", c2*256 + c1);
		printf("carr: %s\n", carr);
		usleep(100000);

    }
 	for(int i=0;i<9;i++)
 		free(szDeviceList[i]);
  return 0;
}

