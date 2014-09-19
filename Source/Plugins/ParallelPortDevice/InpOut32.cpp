//parallelportplugin
//Copyright (C) 2014  Sven Gijsen
//
//This file is part of BrainStim.
//BrainStim is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#include "InpOut32.h"
#include <windows.h>

////Functions exported from DLL.
////Original InpOut32 function support
void	_stdcall Out32(short PortAddress, short data);
short	_stdcall Inp32(short PortAddress);

//Extra functions
BOOL	_stdcall IsInpOutDriverOpen();  //Returns TRUE if the InpOut driver was opened successfully
BOOL	_stdcall IsXP64Bit();			//Returns TRUE if the OS is 64bit (x64) Windows.

//DLLPortIO function support
UCHAR   _stdcall DlPortReadPortUchar (USHORT port);
void	_stdcall DlPortWritePortUchar(USHORT port, UCHAR Value);

USHORT	_stdcall DlPortReadPortUshort (USHORT port);
void	_stdcall DlPortWritePortUshort(USHORT port, USHORT Value);

DWORD	_stdcall DlPortReadPortDWORD(DWORD port);
void	_stdcall DlPortWritePortDWORD(DWORD port, DWORD Value);

//WinIO function support (Untested and probably does NOT work - esp. on x64!)
PBYTE	_stdcall MapPhysToLin(PBYTE pbPhysAddr, DWORD dwPhysSize, HANDLE *pPhysicalMemoryHandle);
BOOL	_stdcall UnmapPhysicalMemory(HANDLE PhysicalMemoryHandle, PBYTE pbLinAddr);
BOOL	_stdcall GetPhysLong(PBYTE pbPhysAddr, PDWORD pdwPhysVal);
BOOL	_stdcall SetPhysLong(PBYTE pbPhysAddr, DWORD dwPhysVal);


InpOut32::InpOut32(void)
{
}

InpOut32::~InpOut32(void)
{
}

short InpOut32::ReadPort(short PortAddress)
{
	return Inp32(PortAddress);
}

bool InpOut32::ReadPortBit(short PortAddress,double nBitNumber)
{
	return BitRead(Inp32(PortAddress),nBitNumber);
}

void InpOut32::WritePort(short PortAddress, short Value)
{
	Out32(PortAddress,Value);
}

void InpOut32::WritePortBit(short PortAddress, double nBitNumber, bool Value)
{
	if (Value)
	{
		Out32(PortAddress,BitSet(Inp32(PortAddress),nBitNumber));
	}
	else
	{
		Out32(PortAddress,BitReset(Inp32(PortAddress),nBitNumber));
	}
}

short InpOut32::StatusPortRead(short PortBaseAddress)
{
//Reads a parallel port's status port.
//Calculates the status-port address from the port's
//base address, and inverts bit 7 of the byte read.
//The status-port hardware re inverts these bits,
//so the value read matches the value at the connector.
	return (Inp32(PortBaseAddress + 1) ^ 0x80);
}

short InpOut32::DataPortRead(short nBaseAddress) 
{
	return (Inp32(nBaseAddress));
}

void InpOut32::DataPortWrite(short nBaseAddress, int nValue)
{
	Out32(nBaseAddress, nValue);
}

short InpOut32::ControlPortRead(short PortBaseAddress)
{
//Reads a parallel port's control port.
//Calculates the control-port address from the port's
//base address, and inverts bits 0, 1, & 3 of the byte read.
//The control-port hardware re inverts these bits,
//so the value read matches the value at the connector.
	return (Inp32(PortBaseAddress + 2) ^ 0xB);
}


void InpOut32::ControlPortWrite(short PortBaseAddress, short Value)
{
//Writes a Value to a parallel port's control port.
//Calculates the control-port address from the port's
//base address, and inverts bits 0, 1, & 3.
//The control-port hardware re inverts these bits,
//so Value is written to the port connector.
	Out32(PortBaseAddress + 2, (Value ^ 0xB));
}

bool InpOut32::BitRead(int nSource, double nBitNumber) 
{
	//Returns the value (0 or 1) of the requested bit in a Variable.
	double nBitValue;
	//the value of the requested bit
	nBitValue = pow(2,nBitNumber);
	if (((nSource & int(nBitValue)) / nBitValue) > 0) //bitwise AND
	{
		return true;
	} 
	else
	{
		return false;
	}
}

short InpOut32::BitReset(int nSource, double nBitNumber) 
{
	//Resets (clears) the requested bit in a Variable.
	double nBitValue;

	//The value of the requested bit.
	nBitValue = pow(2,nBitNumber);
	return (nSource & (0xFFFF - int(nBitValue)));
}


short InpOut32::BitSet(int nSource, double nBitNumber) 
{
	//Sets the requested bit in a Variable.
	double nBitValue;

	//The value of the requested bit.
	nBitValue = pow(2,nBitNumber);
	return (nSource | int(nBitValue));
}

short InpOut32::BitToggle(int nSource, double nBitNumber)
{
	//Toggles the requested bit in a Variable.
	double nBitValue;
	int nCurrentValue;
	//The value of the requested bit.
	nBitValue = pow(2,nBitNumber);

	//Is the current value 0 or 1?
	nCurrentValue = (nSource & int(nBitValue));
	if (nCurrentValue == 0)
	{
		//If current value = 0, set it
		return (nSource | int(nBitValue));
	}
	else
	{
		//If current value = 1, reset it
		return (nSource & (0XFFFF - int(nBitValue)));
	}
}

InpOut32::ECP_Mode InpOut32::GetEcpMode(short nBaseAddress)
{
	short nEcrAddress;
	short nEcrData;
	//The ECR mode is in bits 5, 6, and 7 of the ECR.
	nEcrAddress = nBaseAddress + 0x402;
	nEcrData = ReadPort(nEcrAddress);
	switch ((nEcrData & 0xE0) / 0x20)
	{
	case  0:
		return ECP_MODE_SPP;
		break;
	case  1:
		return ECP_MODE_PS2;
		break;
	case  2:
		return ECP_MODE_FAST_CENTRONICS;
		break;
	case  3:
		return ECP_MODE_ECP;
		break;
	case  4:
		return ECP_MODE_EPP;
		break;
	case  5:
		return ECP_MODE_NOTDEFINED;
		break;
	case  6:
		return ECP_MODE_TEST;
		break;
	case  7:
		return ECP_MODE_CONFIGURATION;
		break;
	}
    return ECP_MODE_NOTDEFINED;
}

int InpOut32::GetEppTimeoutBit(short nBaseAddress)
{
	//Reads and clears the EPP timeout bit (status port bit 0).
	//Should be done after each EPP operation.
	//The method for clearing the bit varies, so try 3 ways:
	//1. Write 1 to it.
	//2. Write 0 to it.
	//3. Read it again.
	int Result;
	short nStatusPortAddress;
	short nReadAgain;
	nStatusPortAddress = nBaseAddress + 1;
	Result = BitRead(StatusPortRead(nBaseAddress), 0);
	WritePort(nStatusPortAddress, 1);
	WritePort(nStatusPortAddress, 0);
	nReadAgain = BitRead(StatusPortRead(nBaseAddress), 0);
	return Result;
}

bool InpOut32::IsPortEcp(short nBaseAddress)
{
	//Test for the presence of an ECP.
	//If the ECP is idle and the FIFO empty,
	//in the ECP's ECR (at Base Address+402h),
	//bit 1(FIFO full)=0, and bit 0(FIFO empty)=1.
	//The first test is to see if these bits differ from the
	//corresponding bits in the control port (at Base Address+2).
	//If so, a further test is to write 34h to the ECR,
	//then read it back. Bit 1 is read/write, and bit 0 is read-only.
	//If the value read is 35h, the port is an ECP.

	bool IsPortEcpResult = false; //Guilty until proven innocent
	short nEcrBit0;
	short nEcrBit1;
	//short nControlBit0;
	short nControlBit1;
	short nControlPortData;
	short nOriginalControlPortData;
	//short nTestEcrAddress;
	short nOriginalEcrData;
	short nEcrAddress;
	short nEcrData;

	nEcrAddress = nBaseAddress + 0x402;
	//Read ECR bits 0 & 1 and Control Port bit 1.
	nEcrData = ReadPort(nEcrAddress);
	nEcrBit0 = BitRead(nEcrData, 0);
	nEcrBit1 = BitRead(nEcrData, 1);
	nOriginalControlPortData = ControlPortRead(nBaseAddress);
	nControlPortData = nOriginalControlPortData;
	nControlBit1 = BitRead(nControlPortData, 1);
	if(nEcrBit0 == 1 && nEcrBit1 == 0)
	{
		//Compare control bit 1 to ECR bit 1.
		//Toggle the control bit if necessary,
		//to be sure the two registers are different.
		if (nControlBit1 == 0)
		{
			ControlPortWrite(nBaseAddress, 0xF);
			nControlPortData = ControlPortRead(nBaseAddress);
			nControlBit1 = BitRead(nControlPortData, 1);
		}
		if (nEcrBit1 != nControlBit1) 
		{
			nOriginalEcrData = nEcrData;
			WritePort(nEcrAddress, 0x34);
			nEcrData = ReadPort(nEcrAddress);
			if (nEcrData == 0x35)
			{
				IsPortEcpResult = true;
			}
			//Restore the ECR to its original value.
			WritePort(nEcrAddress, nOriginalEcrData);
		}		
	}
	//Restore the control to its original value.
	ControlPortWrite(nBaseAddress, nOriginalControlPortData);
	return IsPortEcpResult;
}

bool InpOut32::IsPortEpp(short nBaseAddress)
{
	//Write to an EPP register, then read it back.
	//If the reads match the writes, it's probably an EPP.
	bool IsPortEppResult = false; //Guilty until proven innocent
	short nByteRead;
	//short nStatusPortData;
	short nEppAddressPort;
	short nTimeoutBit;
	short nStatusPortAddress;
	nStatusPortAddress = nBaseAddress + 1;
	
	//Use EppAddressPort for testing.
	//SPPs, ECPs, and PS/2 ports don't have this register.
	nEppAddressPort = nBaseAddress + 3;
	WritePort(nEppAddressPort, 0x55);

	//Clear the timeout bit after each EPP operation.
	nTimeoutBit = GetEppTimeoutBit(nBaseAddress);
	nByteRead = ReadPort(nEppAddressPort);
	nTimeoutBit = GetEppTimeoutBit(nBaseAddress);
	if (nByteRead == 0x55)
	{
		WritePort(nEppAddressPort, 0xAA);
		nTimeoutBit = GetEppTimeoutBit(nBaseAddress);
		nByteRead = ReadPort(nEppAddressPort);
		nTimeoutBit = GetEppTimeoutBit(nBaseAddress);
		if (nByteRead == 0xAA)
		{
			IsPortEppResult = true;
		}		
	}
	return IsPortEppResult;
}

bool InpOut32::IsPortSpp(short nBaseAddress)
{
	bool IsPortSppResult = false;//Guilty until proven innocent
	//Write two bytes and read them back.
	//If the reads match the writes, the port exists.
	short nByteRead;
	//Attempt to cache the control port data
	short nOriginalControlPortData;
	nOriginalControlPortData = ControlPortRead(nBaseAddress);
	//Be sure that control port bit 5 = 0 (data outputs enabled).
	ControlPortWrite(nBaseAddress, 0xF);
	//Write to the data port
	DataPortWrite(nBaseAddress, 0x55);
	//Read the byte back
	nByteRead = DataPortRead(nBaseAddress);
	//Is it the same?
	if (nByteRead == 0x55)
	{
		//Write another byte to confirm
		DataPortWrite(nBaseAddress, 0xAA);
		//Read the byte back
		nByteRead = DataPortRead(nBaseAddress);
		//Is it the same?
		if (nByteRead == 0xAA)
		{
			IsPortSppResult = true;
		}
	}
	//Attempt to restore the control port data
	ControlPortWrite(nBaseAddress, nOriginalControlPortData);
	return IsPortSppResult;
}

bool InpOut32::IsPortPS2(short nBaseAddress) 
{
	bool IsPortPS2Result = false;//Guilty until proven innocent
	//A PS/2 port must pass the SPP test
	if (IsPortSpp(nBaseAddress) == false)
	{ return false;	}
	//Tests a parallel port's data port for bidirectional ability.
	//First, try to tri-state (disable) the data outputs by
	//setting bit 5 of the Control port.
	//Then write 2 values to the data port and read each back
	//If the values match, the data outputs are not disabled,
	//and the port is not bidirectional.
	//If the values don't match,
	//the data outputs are disabled and the port is bidirectional.
	short nDataInput;
	//short nControlPortData;
	//short nOriginalDataPortData;
	//Attempt to cache the control port data
	short nOriginalControlPortData;
	nOriginalControlPortData = ControlPortRead(nBaseAddress);
	//Set Control port bit 5
	ControlPortWrite(nBaseAddress, 0x2F);
	//Write the first byte and read it back:
	DataPortWrite(nBaseAddress, 0x55);
	nDataInput = DataPortRead(nBaseAddress);
	//If it doesn't match, the port is bidirectional.
	if (nDataInput != 0x55)
	{ IsPortPS2Result = true; }
	//If it matches, write another and read it back.
	if (nDataInput == 0x55) 
	{
		DataPortWrite(nBaseAddress, 0xAA);
		nDataInput = DataPortRead(nBaseAddress);
		//If it doesn't match, the port is bidirectional
		if (nDataInput != 0xAA)
		{ 
			IsPortPS2Result = true; 
		}		
	}
	//Attempt to restore the control port data
	ControlPortWrite(nBaseAddress, nOriginalControlPortData);
	return IsPortPS2Result;
}

bool InpOut32::IsPortOutput(short nBaseAddress)
{
	bool IsPortOutputResult = false; //Guilty until proven innocent
	//Get the value of the control port if bit five is
	//high, then set for input.  If the bit is low,
	//then set for output.  NOTE that on some odd ports,
	//you must set bit seven and this function would be incorrect
	short nControlPortValue;
	nControlPortValue = ControlPortRead(nBaseAddress);
	//0 = Output
	if (BitRead(nControlPortValue, 5) == 0)
	{ IsPortOutputResult = true; }
	return IsPortOutputResult;

}

bool InpOut32::IsPortInput(short nBaseAddress) 
{
	bool IsPortInputResult = false; //Guilty until proven innocent
	//Get the value of the control port if bit five is
	//high, then set for input.  If the bit is low,
	//then set for output.  NOTE that on some odd ports,
	//you must set bit seven and this function would be incorrect
	short nControlPortValue;
	nControlPortValue = ControlPortRead(nBaseAddress);
	if (BitRead(nControlPortValue, 5) == 1)
	{ IsPortInputResult = true;	}
	return IsPortInputResult;
}

InpOut32::PortType InpOut32::GetPortType(short nBaseAddress)
{
	//Test for a port's presence, and if it exists, the type of port.
	//In order, check for the presence of an ECP, EPP, SPP, and PS/2 port.
	//Update the information in the Port array  and the display.
	//Check for ECP
	if (IsPortEcp(nBaseAddress))
	{ return PORT_TYPE_ECP;	}
	//If it's not an ECP, look for an EPP.
	//If BaseAddress = 3BCh, skip the EPP test.
	//EPPs aren't allowed at 3BCh due to possible conflict
	//with video memory.
	if (nBaseAddress != 0x3BC)
	{
		if (IsPortEpp(nBaseAddress))
		{ return PORT_TYPE_EPP; }
	}
	//If it's not an EPP, look for a PS/2.
	if (IsPortPS2(nBaseAddress))
	{ return PORT_TYPE_PS2; }
	//Well, must be an SPP
	if (IsPortSpp(nBaseAddress))
	{ return PORT_TYPE_SPP; }
	//Why did we get here - nothing passed!
	//Is the port address not a parallel port?
	return PORT_TYPE_UNKNOWN;
}

bool InpOut32::SetEcpMode(short nBaseAddress, ECP_Mode nEcpModeValue)
{
	//Why are you calling this on a port that is not ECP?
	if (IsPortEcp(nBaseAddress) == false)
	{ return false; }
	//Why are you setting an invalid mode?
	if (nEcpModeValue == ECP_MODE_SPP || nEcpModeValue == ECP_MODE_PS2 ||  nEcpModeValue == ECP_MODE_FAST_CENTRONICS || nEcpModeValue == ECP_MODE_ECP || nEcpModeValue == ECP_MODE_EPP || nEcpModeValue == ECP_MODE_TEST || nEcpModeValue == ECP_MODE_CONFIGURATION)
	{
		short nEcrAddress;
		short nEcrData;
		nEcrAddress = nBaseAddress + 0x402;
		//Read the ECR & clear bits 5, 6, 7.
		nEcrData = ReadPort(nEcrAddress) & 0x1F;
		//Write the selected value to bits 5, 6, 7.
		nEcrData = nEcrData + (nEcpModeValue * 0x20);
		WritePort(nEcrAddress, nEcrData);
		return true;
	}
	else
	{ 
		return false;
	} 
}

bool InpOut32::ConfigurePortForInput(short nBaseAddress)
{
	//If the port is SPP, then input is not permitted through the DATA port
	if (GetPortType(nBaseAddress) == PORT_TYPE_SPP)
	{
		//If the port is SPP, then input is not permitted through the DATA port
		//Configure the port through the BIOS, etc. for ECP, EPP, PS2, Bi-Direction.
		//Input from an SPP must be done through the Status or Control ports as
		//access to the DATA port is for output only.
		//Please see KB:1320 for more information on the E-Prime Web Support site.
		return false;
	}
	//If the port is and ECP port, configure its compatability mode for PS/2 for Bi-Directional
	if (IsPortEcp(nBaseAddress))
	{
		SetEcpMode(nBaseAddress, ECP_MODE_PS2);
	}
	short nControlValue;
	nControlValue = ControlPortRead(nBaseAddress);
	//Set bits five and seven to indicate the port is for input
	nControlValue = BitSet(nControlValue, 5);
	nControlValue = BitSet(nControlValue, 7);
	//Update the register
	ControlPortWrite(nBaseAddress, nControlValue);
	return true;
}

void InpOut32::ConfigurePortForOutput(short nBaseAddress)
{
	//If the port is and ECP port, configure its compatability mode for standard output only
	if (IsPortEcp(nBaseAddress))
	{ SetEcpMode(nBaseAddress, ECP_MODE_SPP); }
	short nControlValue;
	nControlValue = ControlPortRead(nBaseAddress);
	//RESET bits five and seven to zero indicating output
	nControlValue = BitReset(nControlValue, 5);
	nControlValue = BitReset(nControlValue, 7);
	//Update the register
	ControlPortWrite(nBaseAddress, nControlValue);
}

QString InpOut32::GetPortDescription(short nBaseAddress)
{
	QString strDescription;
	strDescription = strDescription + "Type: ";
	//Get the Type
	PortType nPortType;
	nPortType = GetPortType(nBaseAddress);
	switch (nPortType)
	{
	case PORT_TYPE_SPP:
		strDescription = strDescription + "SPP (Output Only)";
		break;
	case PORT_TYPE_PS2:
		strDescription = strDescription + "PS/2 (Bi-Directional)";
		break;
	case PORT_TYPE_EPP:
		strDescription = strDescription + "EPP";
		break;
	case PORT_TYPE_ECP:
		strDescription = strDescription + "ECP";
		//If ECP, then get compatability mode
		strDescription = strDescription + " Compatability: ";
		ECP_Mode nEcpMode;
		nEcpMode = GetEcpMode(nBaseAddress);
		switch (nEcpMode)
		{
		case ECP_MODE_SPP:
			strDescription = strDescription + "SPP";
			break;
		case ECP_MODE_PS2:
			strDescription = strDescription + "PS/2";
			break;
		case ECP_MODE_FAST_CENTRONICS:
			strDescription = strDescription + "Fast Centronics";
			break;
		case ECP_MODE_EPP:
			strDescription = strDescription + "EPP";
			break;
		case ECP_MODE_ECP:
			strDescription = strDescription + "ECP";
			break;
		case ECP_MODE_TEST:
			strDescription = strDescription + "Test";
			break;
		case ECP_MODE_CONFIGURATION:
			strDescription = strDescription + "Configuration";
			break;
		default:
			strDescription = strDescription + "UNKNOWN(" + nEcpMode + ")";
			break;
		}
		break;
	default:
		strDescription = strDescription + "UNKNOWN(" + nPortType + ")";
		break;
	}
	//Can we currently write to the port?
	if (IsPortOutput(nBaseAddress))
	{ strDescription = strDescription + " DATA PORT: output"; }	
	//Can we currently read from the port?
	if (IsPortInput(nBaseAddress))
	{ strDescription = strDescription + " DATA PORT: input"; }
	//Assign return
	return strDescription;
}
