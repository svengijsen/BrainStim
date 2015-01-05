function tr(s) { return s; }

function DigitalClock(parent) 
{ 
	QLCDNumber.call(this, parent);
	this.segmentStyle = QLCDNumber.Filled;
	this.timer = new QTimer(this);
	this.timer.singleshot = false;
	this.timer.start(1000);
	this.showTime();
	this.windowTitle = tr("Digital Clock");
	this.resize(150, 60);
}

DigitalClock.prototype = new QLCDNumber();

DigitalClock.prototype.showTime = function()
{
	var time = QTime.currentTime();
	var format = "hh";
	format += ((time.second() % 2) == 0) ? " " : ":";
	format += "mm";
	var text = time.toString(format);
	this.display(text);
	//Log(text);
}

DigitalClock.prototype.closeEvent = function() 
{
	Log("DigitalClock closeEvent() detected!");
	ScriptCleanupFunction();
}

//This function makes sure that everything gets nicely cleaned before the script ends
function ScriptCleanupFunction()//Cleanup
{
	clock.timer.timeout.disconnect(clock, clock.showTime);
	tr = null;
	DigitalClock.prototype.showTime = null;
	DigitalClock.prototype.closeEvent = null;
	DigitalClock.prototype = null;
	DigitalClock = null;
	clock = null;
	ScriptCleanupFunction = null;	
	//Write something to the Log Output Pane so we know that this Function executed successfully.
	Log("Finished script Cleanup!");
	StimulGL.cleanupScript();
}

var clock = new DigitalClock(5);
clock.timer.timeout.connect(clock, clock.showTime);
clock.show();