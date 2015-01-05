/********************************************************************************************************
**
** This example shows how to make use of the Qt bindings, it uses the QWidget class to create an analog clock
**
********************************************************************************************************/ 

var timer = new QTimer(this);
var painter = new QPainter();
var time = new Date();

function AnalogClock(parent) 
{
	QWidget.call(this, parent);
	this.setWindowTitle("Analog Clock");
	this.resize(200, 200);
}

AnalogClock.prototype = new QWidget();
AnalogClock.prototype.paintEvent = function() 
{
	time = new Date();
	var side = Math.min(this.width, this.height);
	painter.begin(this);
	painter.setRenderHint(QPainter.Antialiasing);
	painter.translate(this.width / 2, this.height / 2);
	painter.scale(side / 200.0, side / 200.0);
	painter.setPen(new QPen(Qt.NoPen));
	painter.setBrush(new QBrush(AnalogClock.hourColor));
	painter.save();
	painter.rotate(30.0 * ((time.getHours() + time.getMinutes() / 60.0)));
	painter.drawConvexPolygon(AnalogClock.hourHand);
	painter.drawLine(0, 0, 100, 100);
	painter.restore();
	painter.setPen(AnalogClock.hourColor);
	for (var i = 0; i < 12; ++i) 
	{
		painter.drawLine(88, 0, 96, 0);
		painter.rotate(30.0);
	}
	painter.setPen(new QPen(Qt.NoPen)); 
	painter.setBrush(new QBrush(AnalogClock.minuteColor));
	painter.save();
	painter.rotate(6.0 * (time.getMinutes() + time.getSeconds() / 60.0));
	painter.drawConvexPolygon(AnalogClock.minuteHand);
	painter.restore();
	painter.setPen(AnalogClock.minuteColor);
	for (var j = 0; j < 60; ++j) 
	{
		if ((j % 5) != 0)
			painter.drawLine(92, 0, 96, 0);
		painter.rotate(6.0);
	}
	painter.end();
}

AnalogClock.prototype.closeEvent = function() 
{
	ScriptCleanupFunction();
}

function ConnectDisconnectScriptFunctions(Connect)
//This function can connect or disconnect all signal/slot connections defined by the boolean parameter 
{
	if(Connect) //This parameter defines whether we should connect or disconnect the signal/slots.
	{
		Log("... Connecting Signal/Slots");
		try 
		{	
			timer.timeout.connect(clock, "update()");
		} 
		catch (e) 
		{
			Log(".*. Something went wrong connecting the Signal/Slots:" + e); //If a connection fails warn the user!
		}		
	}
	else
	{
		Log("... Disconnecting Signal/Slots");
		try 
		{	
			timer.timeout.disconnect(clock, "update()");
		} 
		catch (e) 
		{
			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e); //If a disconnection fails warn the user!
		}		
	}	
}

//This function makes sure that everything gets nicely cleaned before the script ends
function ScriptCleanupFunction()//Cleanup
{
	timer.stop();
	//Disconnect the signal/slots
	ConnectDisconnectScriptFunctions(false);
	//Set all constructed objects to null 
	timer=null;
	clock = null;
	painter = null;
	time = null;
	//Set all functions to null
	AnalogClock.prototype.closeEvent = null;
	AnalogClock.prototype.paintEvent = null;	
	AnalogClock.prototype = null;
	AnalogClock.hourColor = null;
	AnalogClock.minuteColor = null;
	AnalogClock.hourHand = null;
	AnalogClock.minuteHand = null;		
	AnalogClock = null;
	ScriptCleanupFunction = null;
	ConnectDisconnectScriptFunctions = null;
	//Write something to the Log Output Pane so we know that this Function executed successfully.
	Log("Finished script Cleanup!");
	StimulGL.cleanupScript();
}



AnalogClock.hourColor = new QColor(127, 0, 127);
AnalogClock.minuteColor = new QColor(0, 127, 127, 191);
AnalogClock.hourHand = new QPolygon([new QPoint(7, 8), new QPoint(-7, 8),  new QPoint(0, -40)]);
AnalogClock.minuteHand = new QPolygon([new QPoint(7, 8), new QPoint(-7, 8), new QPoint(0, -70)]);
var clock = new AnalogClock();
ConnectDisconnectScriptFunctions(true);
timer.start(1000);
clock.show();





