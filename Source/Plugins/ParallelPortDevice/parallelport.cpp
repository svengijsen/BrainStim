//parallelportplugin
//Copyright (C) 2015  Sven Gijsen
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


#include "parallelport.h"

/*! \brief The ParallelPort Device constructor.
*
*   The BaseAddress should be set to the decimal value of the ports base address.
*   You do not need to specify the parent object, 
*	the BrainStim script engine automatically retrieves the parent role
*/
ParallelPort::ParallelPort(short BaseAddress, QObject *parent) : QObject(parent)
{
	currentScriptEngine = NULL;
	nBaseAddress = BaseAddress;
	generateThread = NULL;
	captureThread = NULL;
}

/*! \brief The ParallelPort Device destructor.
*
*   You do not need call the destructor. 
*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
*/
ParallelPort::~ParallelPort()
{
	if (captureThread)
	{
		delete captureThread;
		//captureThread = NULL;
	}
	if (generateThread)
	{
		delete generateThread;
		//generateThread = NULL;
	}
}

bool ParallelPort::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
{
	if (engine)
	{
		currentScriptEngine = reinterpret_cast<QScriptEngine *>(engine);
		//QObject *someObject = this;//new MyObject;
		QScriptValue objectValue = currentScriptEngine->newQObject(this);
		currentScriptEngine->globalObject().setProperty(strObjectScriptName, objectValue);
		return true;
	}
	return false;
}

void ParallelPort::setBaseAddress( short BaseAddress )
{
/*! Sets the Parallel Ports base address to the (decimal) value of the parameter BaseAddress. */
	nBaseAddress = BaseAddress;
}

short ParallelPort::getBaseAddress() const
{
/*! Returns the current Parallel Ports base address (decimal), see #setBaseAddress. */
	return nBaseAddress;
}

short ParallelPort::PortRead()
{
/*! Returns the current port value(byte) at the ports base address. */
	return InpOut32Object.ReadPort(nBaseAddress);
}

void ParallelPort::PortWrite(const short &Value)
{
/*! Writes the value(Value) to the current port at the ports base address. */
	InpOut32Object.WritePort(nBaseAddress, Value);	
}

bool ParallelPort::IsPortEpp()
{
/*! Returns whether the current port at the ports base address is in EPP mode. */
	return InpOut32Object.IsPortEpp(nBaseAddress);
}

bool ParallelPort::IsPortEcp()
{
/*! Returns whether the current port at the ports base address is in ECP mode. */
	return InpOut32Object.IsPortEcp(nBaseAddress);
}
//ECP_Mode ParallelPort::GetEcpMode()
//PortType ParallelPort::GetPortType()
bool ParallelPort::IsPortSpp()
{
/*! Returns whether the current port at the ports base address is in SPP mode. */
	return InpOut32Object.IsPortSpp(nBaseAddress);
}
bool ParallelPort::IsPortPS2()
{
/*! Returns whether the current port at the ports base address is in PS2 mode. */
	return InpOut32Object.IsPortPS2(nBaseAddress);
}
bool ParallelPort::IsPortInput()
{
/*! Returns whether the current port at the ports base address is in Input mode. */
	return InpOut32Object.IsPortInput(nBaseAddress);
}
bool ParallelPort::IsPortOutput()
{
/*! Returns whether the current port at the ports base address is in Output mode. */
	return InpOut32Object.IsPortOutput(nBaseAddress);
}
bool ParallelPort::ConfigurePortForInput()
{
/*! Returns whether the current port at the ports base address could be successfully configured for Input mode. */
	return InpOut32Object.ConfigurePortForInput(nBaseAddress);
}
void ParallelPort::ConfigurePortForOutput()
{
/*! Returns whether the current port at the ports base address could be successfully configured for Output mode. */
	InpOut32Object.ConfigurePortForOutput(nBaseAddress);
}
QString ParallelPort::GetPortDescription()
{
/*! Returns a QString containing the current port description at the ports base address. */
	return InpOut32Object.GetPortDescription(nBaseAddress);
}
bool ParallelPort::BitRead(const short nBitNumber)
{
/*! Returns the bit value from bit number(nBitNumber) read from the current port at the ports base address. */
	return InpOut32Object.BitRead(nBaseAddress,nBitNumber);
}
void ParallelPort::BitWrite(const short nBitNumber, bool Value)
{
/*! Writes the bit value(Value) to bit number(nBitNumber) of the current port at the ports base address. */
	InpOut32Object.WritePortBit(nBaseAddress,nBitNumber,Value);
}
short ParallelPort::BitReset(const short nBitNumber)
{
/*! Sets the bit at bit number(nBitNumber) of the current port at the ports base address to false(0). */
	return InpOut32Object.BitReset(nBaseAddress,nBitNumber);
}
short ParallelPort::BitSet(const short nBitNumber)
{
/*! Sets the bit at bit number(nBitNumber) of the current port at the ports base address to true(1). */
	return InpOut32Object.BitSet(nBaseAddress,nBitNumber);
}
short ParallelPort::BitToggle(const short nBitNumber)
{
/*! Toggles the bit at bit number(nBitNumber) of the current port at the ports base address. */
	return InpOut32Object.BitToggle(nBaseAddress,nBitNumber);
}
short ParallelPort::StatusPortRead()
{
/*! Returns the current port value(byte) from the status port(current port base address+1). */
	return InpOut32Object.StatusPortRead(nBaseAddress);
}
short ParallelPort::DataPortRead()
{
/*! Returns the current port value(byte) from the current port base address. */
	return InpOut32Object.DataPortRead(nBaseAddress);	
}
void ParallelPort::DataPortWrite(const short Value)
{
/*! Writes the value(Value) to the current port base address. */
	InpOut32Object.DataPortWrite(nBaseAddress, Value);
}
short ParallelPort::ControlPortRead()
{
/*! Returns the current port value(byte) from the control port(current port base address+2). */
	return InpOut32Object.ControlPortRead(nBaseAddress);
}
void ParallelPort::ControlPortWrite(const short Value)
{
/*! Writes the value(Value) to the control port(current port base address+2). */
	InpOut32Object.ControlPortWrite(nBaseAddress, Value);
}
int ParallelPort::GetEppTimeoutBit()
{
/*! Returns the value of the EPP TimeOut bit from the current port base address. */
	return InpOut32Object.GetEppTimeoutBit(nBaseAddress);
}
//bool ParallelPort::SetEcpMode(ECP_Mode nEcpModeValue)

//! For starting a new Parallel Port Generate thread, only one instance can be active at the same moment.
/*!
    \param baseAddress the base address.
	\param method the generation method to use(see the ParallelPortNameSpace.GenerationMethod).
	\param outputMask a bit-mask pattern (one byte) representing which output bits can be changed by this generate thread.
	\param activeValue a bit-mask pattern (one byte) representing the Active output value.
	\param inActiveValue a bit-mask pattern (one byte) representing the In-active output value..
	\param activePulseTime the duration(in msec) for an pulse Active output value.
	\param repetitionTime the repetition-time (in msec) for an full pulse cycle(Active plus In-active).
    \return Whether the thread could be successfully started.
    \sa StopGenerateThread()
*/
bool ParallelPort::StartGenerateThread(const short baseAddress,const short method, const short outputMask, const short activeValue, const short inActiveValue, const int activePulseTime, const int repetitionTime)
{
	ParallelPortNameSpace::GenerationMethod GenMethod;
	if (method == 0) 
        {GenMethod = ParallelPortNameSpace::Value;}
	else if (method == 1) 
    {GenMethod = ParallelPortNameSpace::Pulse;}
	else 
    {GenMethod = ParallelPortNameSpace::Periodical;}

	generateThread = new ppGenerateThread(baseAddress,GenMethod,outputMask,activeValue,inActiveValue,activePulseTime,repetitionTime,this);
		
	connect(generateThread, SIGNAL(generateThreadTriggered(int)), this, SIGNAL(GenerateThreadTriggered(int)));
	connect(generateThread, SIGNAL(generateThreadStarted(QString)), this, SIGNAL(GenerateThreadStarted(QString)));
	connect(generateThread, SIGNAL(generateThreadStopped(QString)), this, SIGNAL(GenerateThreadStopped(QString)));
	generateThread->start();
	return true;
}

void ParallelPort::StopGenerateThread()
{
/*! Stops the Generate Thread(if started), see #StartGenerateThread. */
	generateThread->stop();
}

//! For starting a new Parallel Port Capture thread, only one instance can be active at the same moment.
/*!
    \param baseAddress the base address.
	\param mask a bit-mask pattern (one byte) representing which bits should be taken into account for the capture thread.
	\param method the capture method to use(see the ParallelPortNameSpace.CaptureMethod).
	\param postLHDelay the delay-time (in msec) after a Low->High Transition when the capture thread does nothing.
	\param postHLDelay the delay-time (in msec) after a High->Low Transition when the capture thread does nothing.
    \return Whether the thread could be successfully started.
    \sa StopCaptureThread()
*/
bool ParallelPort::StartCaptureThread(const short baseAddress, const short mask, const short method,const int postLHDelay, const int postHLDelay)
{
	ParallelPortNameSpace::CaptureMethod CapDecMethod;
	if (method == 0) 
    {CapDecMethod = ParallelPortNameSpace::MaskedValueChanged;}
	else if (method == 1) 
    {CapDecMethod = ParallelPortNameSpace::MaskedValueChangedHigh;}
	else 
    {CapDecMethod = ParallelPortNameSpace::MaskedValueChangedLow;}

	captureThread = new ppCaptureThread(baseAddress,mask,CapDecMethod,postLHDelay,postHLDelay,this);

	connect(captureThread, SIGNAL(recieveThreadTriggered(int)), this, SIGNAL(CaptureThreadTriggered(int)));
	connect(captureThread, SIGNAL(recieveThreadStarted(QString)), this, SIGNAL(CaptureThreadStarted(QString)));
	connect(captureThread, SIGNAL(recieveThreadStopped(QString)), this, SIGNAL(CaptureThreadStopped(QString)));
	captureThread->start();
	return true;
}

void ParallelPort::StopCaptureThread()
{
/*! Stops the Capture Thread(if started), see #StartCaptureThread. */
	if (captureThread)
	{
		captureThread->stop();
	}
}


