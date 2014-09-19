var strGlobal = new String(""); //Just a string for global use
var nTriggerCounter = 0;

function myFinalCleanup()//Cleanup
{
	PPDevice.StopGenerateThread();
	PPDevice.StopCaptureThread();	
	//PPDevice.GenerateThreadStarted.disconnect(this, this.mySignalFunction);
	//PPDevice.GenerateThreadTriggered.disconnect(this, this.mySignalFunction);
	PPDevice.CaptureThreadStarted.disconnect(this, this.mySignalFunction);
	PPDevice.CaptureThreadTriggered.disconnect(this, this.mySignalFunction);
	PPDevice.CaptureThreadStopped.disconnect(this, this.mySignalFunction);
	PPDevice = null;
	mySignalFunction = null;
	PauseMills = null;
	myFinalCleanup = null;
	Log("Finished script Cleanup!");
}

function mySignalFunction()
{
	Log("mySignalFunction arguments count: " + arguments.length);
	for (var i = 0; i < arguments.length; ++i)
	Log("mySignalFunction first argument: " + arguments[i]); 	
	
	nTriggerCounter++;
	Log("nTriggerCounter = " + nTriggerCounter);
	if (nTriggerCounter==4)
	{
		PPDevice.StopCaptureThread();
	}	
	if (nTriggerCounter==5)
	{
		myFinalCleanup();
	}
}

function PauseMills(millis)
{
	var date = new Date();
	var curDate = null;
     
	do { curDate = new Date(); }
	while(curDate-date < millis)
}

PPDevice = new ParallelPortDevice(); //Construct a Parallel Port Object
//Configure the Parallel Port
//Log(PPDevice.GetMinimalMainProgramVersion());//Test(""));
Log("The default BaseAdress is: " + PPDevice.BaseAddress); //Read the default Parallel Port BaseAddress
PPDevice.BaseAddress = 4368; //4368 (decimal) == 1110 (hexadecimal) 
Log("The changed BaseAdress is: " + PPDevice.BaseAddress); //Read the changed Parallel Port BaseAddress again

Log("The current Port Description" + PPDevice.GetPortDescription());

//Read/Write some Port Values at the new BaseAddress
for (i=0;i<5;i++) //Create a simple for-loop
{
	PPDevice.PortWrite(64); //64 => only bit6 (0..7) is active
	Log(PPDevice.PortRead());
	Pause(100); //Wait some time, this blocks the script
	PPDevice.PortWrite(33); //33(=1+32) => bit0 and bit5 (0..7) are active 
	Log(PPDevice.PortRead());
	Pause(100);
}

//Create and Stop a capture and generate thread
//PPDevice.GenerateThreadStarted.connect(this, this.mySignalFunction);
//PPDevice.GenerateThreadTriggered.connect(this, this.mySignalFunction);
//PPDevice.PortWrite(0); 
//PPDevice.ConfigurePortForInput();
PPDevice.CaptureThreadStarted.connect(this, this.mySignalFunction);
PPDevice.CaptureThreadTriggered.connect(this, this.mySignalFunction);
PPDevice.CaptureThreadStopped.connect(this, this.mySignalFunction);
PPDevice.StartGenerateThread(4370, 2, 1, 1, 0, 500, 1000);//(const short baseAddress,const short method, const short outputMask, const short activeValue, const short inActiveValue, const int activePulseTime, const int repetitionTime);
PPDevice.StartCaptureThread(4370, 1, 2, 0, 100);//(const short baseAddress, const short mask, const short method, const int postLHDelay = 0, const int postHLDelay = 0);
//Pause(4000);
//PauseMills(4000);//This function doesn't block the Timer thread, but has a high CPU load, it's better to use Timers/Signal/Slots
//void setBaseAddress( short BaseAddress );
//short getBaseAddress() const;
//short PortRead();
//void PortWrite(const short &Value);
//bool IsPortEpp();
//bool IsPortEcp();
//ECP_Mode GetEcpMode();
//PortType GetPortType();
//bool IsPortSpp();
//bool IsPortPS2();
//bool IsPortInput();
//bool IsPortOutput();
//bool ConfigurePortForInput();
//void ConfigurePortForOutput();
//QString GetPortDescription();
//bool BitRead(const short nBitNumber);
//void BitWrite(const short nBitNumber, bool Value);
//short BitReset(const short nBitNumber);
//short BitSet(const short nBitNumber);
//short BitToggle(const short nBitNumber);
//short StatusPortRead();
//short DataPortRead();
//void DataPortWrite(const short Value);
//short ControlPortRead();
//void ControlPortWrite(const short Value);
//int GetEppTimeoutBit();
//bool StartGenerateThread(const short baseAddress,const short method, const short outputMask, const short activeValue, const short inActiveValue, const int activePulseTime, const int repetitionTime);
	//method:
		//0 = Value
		//1 = Pulse
		//2 = Periodical
	//Signals:
		//GenerateThreadTriggered(short);  --> gives value
		//GenerateThreadStarted(QString); --> gives timestamp
		//GenerateThreadStopped(QString);  --> gives timestamp		
//void StopGenerateThread();
//bool StartCaptureThread(const short baseAddress, const short mask, const short method, const int postLHDelay = 0, const int postHLDelay = 0);
	//method:
		//0 = MaskedValueChanged
		//1 = MaskedValueChangedHigh
		//2 = MaskedValueChangedLow
	//Signals:
		//CaptureThreadTriggered(short);  --> gives value
		//CaptureThreadStarted(QString);  --> gives timestamp
		//CaptureThreadStopped(QString);	  --> gives timestamp
//void StopCaptureThread();