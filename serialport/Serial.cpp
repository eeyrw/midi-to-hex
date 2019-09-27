/** Serial.cpp
 *
 * A very simple serial port control class that does NOT require MFC/AFX.
 *
 * @author Hans de Ruiter
 *
 * @version 0.1 -- 28 October 2008
 */

#include <iostream>
using namespace std;

#include "Serial.h"

Serial::Serial(tstring &commPortName, int bitRate)
{
	commHandle = CreateFile(commPortName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
							0, NULL);

	if (commHandle == INVALID_HANDLE_VALUE)
	{
		throw("ERROR: Could not open com port");
	}
	else
	{
		// set timeouts
		COMMTIMEOUTS cto = {MAXDWORD, 0, 0, 0, 0};
		DCB dcb;
		if (!SetCommTimeouts(commHandle, &cto))
		{
			CleanUp();
			throw("ERROR: Could not set com port time-outs");
		}

		// set DCB
		memset(&dcb, 0, sizeof(dcb));
		dcb.DCBlength = sizeof(dcb);
		dcb.BaudRate = bitRate;
		dcb.fBinary = 1;
		dcb.fDtrControl = DTR_CONTROL_ENABLE;
		dcb.fRtsControl = RTS_CONTROL_ENABLE;

		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;
		dcb.ByteSize = 8;

		if (!SetCommState(commHandle, &dcb))
		{
			CleanUp();
			throw("ERROR: Could not set com port parameters");
		}
	}
}

void Serial::CleanUp()
{
	CloseHandle(commHandle);
}

Serial::~Serial()
{
	Serial::CleanUp();
}

int Serial::write(const char *buffer)
{
	DWORD numWritten;
	WriteFile(commHandle, buffer, strlen(buffer), &numWritten, NULL);

	return numWritten;
}

int Serial::write(const char *buffer, int buffLen)
{
	DWORD numWritten;
	WriteFile(commHandle, buffer, buffLen, &numWritten, NULL);

	return numWritten;
}

int Serial::read(char *buffer, int buffLen, bool nullTerminate)
{
	DWORD numRead;
	if (nullTerminate)
	{
		--buffLen;
	}

	BOOL ret = ReadFile(commHandle, buffer, buffLen, &numRead, NULL);

	if (!ret)
	{
		return 0;
	}

	if (nullTerminate)
	{
		buffer[numRead] = '\0';
	}

	return numRead;
}

#define FLUSH_BUFFSIZE 10

void Serial::flush()
{
	char buffer[FLUSH_BUFFSIZE];
	int numBytes = read(buffer, FLUSH_BUFFSIZE, false);
	while (numBytes != 0)
	{
		numBytes = read(buffer, FLUSH_BUFFSIZE, false);
	}
}

string Serial::detectUsbSerialPort(uint32_t pid,uint32_t vid)
{
	HANDLE hSerial; // Handle to the Serial port
	DWORD dwGuids;
	GUID *pGuids;

	HDEVINFO hDevInfo;
	SP_DEVICE_INTERFACE_DATA devInterfaceData;
	SP_DEVINFO_DATA devInfoData;
	unsigned int index, isSuc;

	string comName = "";
	dwGuids = 0;

	isSuc = SetupDiClassGuidsFromName("Ports", NULL, 0, &dwGuids);

	pGuids = (GUID *)malloc(dwGuids * sizeof(GUID));
	ZeroMemory(pGuids, dwGuids * sizeof(GUID));
	isSuc = SetupDiClassGuidsFromName("Ports", pGuids, dwGuids, &dwGuids);

	hDevInfo = SetupDiGetClassDevs(pGuids, NULL, NULL,
								   /*DIGCF_ALLCLASSES | DIGCF_PRESENT |*/ DIGCF_DEVICEINTERFACE);

	index = 0;

	ZeroMemory(&devInfoData, sizeof(SP_DEVINFO_DATA));
	devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	while (SetupDiEnumDeviceInfo(hDevInfo, index, &devInfoData))
	{

		TCHAR szDeviceInstanceID[1024];

		index++;

		isSuc = CM_Get_Device_ID(devInfoData.DevInst,
								 &szDeviceInstanceID[0], sizeof(szDeviceInstanceID), 0);

		//printf("%s\n", &szDeviceInstanceID[0]);

		if (0 == strncmp(&szDeviceInstanceID[0], "USB\\VID_1A86&PID_7523",
						 strlen("USB\\VID_1A86&PID_7523")))
		{

			DWORD requiredSize;
			GUID classGuid;

			SP_DEVICE_INTERFACE_DATA devInterfaceData;
			SP_DEVICE_INTERFACE_DETAIL_DATA *pDevInterfaceDetailData;

			printf("&szDeviceInstanceID[0] = %s\n", &szDeviceInstanceID[0]);

			ZeroMemory(&devInterfaceData, sizeof(SP_DEVICE_INTERFACE_DATA));

			devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
			classGuid = devInfoData.ClassGuid;

			isSuc = SetupDiEnumDeviceInterfaces(hDevInfo, &devInfoData, pGuids,
												0, &devInterfaceData);

			isSuc = SetupDiGetDeviceInterfaceDetail(hDevInfo,
													&devInterfaceData, NULL, NULL, &requiredSize, NULL);

			//printf ("%s\n", GetLastErrorMessage( GetLastError() ) );

			pDevInterfaceDetailData = (SP_INTERFACE_DEVICE_DETAIL_DATA *)malloc(requiredSize);

			pDevInterfaceDetailData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

			isSuc = SetupDiGetDeviceInterfaceDetail(hDevInfo,
													&devInterfaceData, pDevInterfaceDetailData, requiredSize,
													&requiredSize, &devInfoData);

			printf("devInterfaceDetailData.DevicePath = %s\n",
				   pDevInterfaceDetailData->DevicePath);

			// hSerial = CreateFile(pDevInterfaceDetailData->DevicePath,
			// 					 GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
			// 					 FILE_ATTRIBUTE_NORMAL, NULL);

			comName = string(pDevInterfaceDetailData->DevicePath);

			free(pDevInterfaceDetailData);
			pDevInterfaceDetailData = NULL;

			// if (INVALID_HANDLE_VALUE != hSerial)
			// 	break;
		}
	}

	free(pGuids);
	pGuids = NULL;
	SetupDiDestroyDeviceInfoList(hDevInfo);

	return comName;
}