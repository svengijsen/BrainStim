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


#pragma once
#include <math.h>
#include <QObject>

//const short PORT_TYPE_UNKNOWN = 0;
//const short PORT_TYPE_SPP = 1;
//const short PORT_TYPE_PS2 = 2;
//const short PORT_TYPE_EPP = 3;
//const short PORT_TYPE_ECP = 4;
//
//const short ECP_MODE_SPP = 0;
//const short ECP_MODE_PS2 = 1;
//const short ECP_MODE_FAST_CENTRONICS = 2;
//const short ECP_MODE_ECP = 3;
//const short ECP_MODE_EPP = 4;
//const short ECP_MODE_NOTDEFINED = 5;
//const short ECP_MODE_TEST = 6;
//const short ECP_MODE_CONFIGURATION = 7;

class InpOut32
{

public:
	enum PortType{PORT_TYPE_UNKNOWN,PORT_TYPE_SPP,PORT_TYPE_PS2,PORT_TYPE_EPP,PORT_TYPE_ECP};
	enum ECP_Mode{ECP_MODE_SPP,ECP_MODE_PS2,ECP_MODE_FAST_CENTRONICS,ECP_MODE_ECP,ECP_MODE_EPP,ECP_MODE_NOTDEFINED,ECP_MODE_TEST,ECP_MODE_CONFIGURATION};

	long ReadMemory(long memAddress);

	InpOut32(void);
	virtual ~InpOut32(void);

	short ReadPort(short PortAddress);
	bool ReadPortBit(short PortAddress, double nBitNumber);
	void WritePort(short PortAddress, short Value);
	void WritePortBit(short PortAddress, double nBitNumber, bool Value);
	bool IsPortEpp(short nBaseAddress);
	bool IsPortEcp(short nBaseAddress);
	ECP_Mode GetEcpMode(short nBaseAddress);
	PortType GetPortType(short nBaseAddress);
	bool IsPortSpp(short nBaseAddress);
	bool IsPortPS2(short nBaseAddress);
	bool IsPortInput(short nBaseAddress);
	bool IsPortOutput(short nBaseAddress);
	bool ConfigurePortForInput(short nBaseAddress);
	void ConfigurePortForOutput(short nBaseAddress);
	QString GetPortDescription(short nBaseAddress);

	//private:
	bool BitRead(int nSource, double nBitNumber);
	short BitReset(int nSource, double nBitNumber);
	short BitSet(int nSource, double nBitNumber);
	short BitToggle(int nSource, double nBitNumber);
	short StatusPortRead(short PortBaseAddress);
	short DataPortRead(short nBaseAddress);
	void DataPortWrite(short nBaseAddress, int nValue);
	short ControlPortRead(short PortBaseAddress);
	void ControlPortWrite(short PortBaseAddress, short Value);
	int GetEppTimeoutBit(short nBaseAddress);
	bool SetEcpMode(short nBaseAddress, ECP_Mode nEcpModeValue);	
};



