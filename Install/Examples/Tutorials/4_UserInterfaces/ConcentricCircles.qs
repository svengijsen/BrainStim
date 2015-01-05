function tr(s) { return s; }

function CircleWidget(parent)
{
	QWidget.call(this, parent);
	this.floatBased = false;
	this.antialiased = false;
	this.frameNo = 0;
	this.setBackgroundRole(QPalette.Base);
	this.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding);
}

CircleWidget.prototype = new QWidget();

CircleWidget.prototype.setFloatBased = function(floatBased)
{
	this.floatBased = floatBased;
	this.update();
}

CircleWidget.prototype.setAntialiased = function(antialiased)
{
	this.antialiased = antialiased;
	this.update();
}

CircleWidget.prototype.getMinimumSizeHint = function()
{
	return new QSize(50, 50);
}

CircleWidget.prototype.getSizeHint = function()
{
	return new QSize(180, 180);
}

CircleWidget.prototype.nextAnimationFrame = function()
{
	++this.frameNo;
	this.update();
}

CircleWidget.prototype.paintEvent = function(/* event */)
{
	var painter = new QPainter();
	painter.begin(this);
	painter.setRenderHint(QPainter.Antialiasing, this.antialiased);
	painter.translate(this.width / 2, this.height / 2);
	for (var diameter = 0; diameter < 256; diameter += 9) 
	{
		var delta = Math.abs((this.frameNo % 128) - diameter / 2);
		var alpha = 255 - (delta * delta) / 4 - diameter;
		if (alpha > 0)
		{
			painter.setPen(new QPen(new QColor(0, diameter / 2, 127, alpha), 3));
			if (this.floatBased) 
			{
				painter.drawEllipse(new QRectF(-diameter / 2.0, -diameter / 2.0, diameter, diameter));
			} 
			else 
			{
				painter.drawEllipse(new QRect(-diameter / 2, -diameter / 2, diameter, diameter));
			}
		}
	}
	painter.end();
}


function Window(parent)
{
	QWidget.call(this, parent);
	var aliasedLabel = this.createLabel(tr("Aliased"));
	var antialiasedLabel = this.createLabel(tr("Antialiased"));
	var intLabel = this.createLabel(tr("Int"));
	var floatLabel = this.createLabel(tr("Float"));
	var layout = new QGridLayout();
	layout.addWidget(aliasedLabel, 0, 1);
	layout.addWidget(antialiasedLabel, 0, 2);
	layout.addWidget(intLabel, 1, 0);
	layout.addWidget(floatLabel, 2, 0);
	this.timer = new QTimer(this);
	this.CircleWidgets = Array[];
	this.CircleWidgetCount = 0;
	for (var i = 0; i < 2; ++i) 
	{
		for (var j = 0; j < 2; ++j) 
		{
			//var cw = new CircleWidget();
			this.CircleWidgets[this.CircleWidgetCount] = new CircleWidget(); 
			this.CircleWidgets[this.CircleWidgetCount].setAntialiased(j != 0);
			this.CircleWidgets[this.CircleWidgetCount].setFloatBased(i != 0);
			//this.timer.timeout.connect(this.CircleWidgets[this.CircleWidgetCount], this.CircleWidgets[this.CircleWidgetCount].nextAnimationFrame);
			layout.addWidget(this.CircleWidgets[this.CircleWidgetCount], i + 1, j + 1);
			this.CircleWidgetCount++;
		}
	}
	this.timer.start(100);
	this.setLayout(layout);
	this.windowTitle = tr("Concentric Circles");
}

Window.prototype = new QWidget();

Window.prototype.createLabel = function(text)
{
	var label = new QLabel(text);
	label.alignment = Qt.AlignCenter;
	label.margin = 2;
	label.setFrameStyle(QFrame.Box | QFrame.Sunken);
	return label;
}

Window.prototype.closeEvent = function() 
{
	Log("Window closeEvent() detected!");
	ScriptCleanupFunction();
}

//This function makes sure that everything gets nicely cleaned before the script ends
function ScriptCleanupFunction()//Cleanup
{
	win.timer.stop();
	try
	{
		for (var i = 0; i < win.CircleWidgetCount; ++i) 
		{		
			win.timer.timeout.disconnect(win.CircleWidgets[i], win.CircleWidgets[i].nextAnimationFrame);
		}
	}	
	catch (e) 
	{
		Log(".*. Something went wrong disconnecting Signal/Slots:" + e); //If a connection fails warn the user!
	}
		
	tr = null;
	CircleWidget.prototype.setFloatBased = null;
	CircleWidget.prototype.setAntialiased = null;
	CircleWidget.prototype.getMinimumSizeHint = null;
	CircleWidget.prototype.getSizeHint = null;
	CircleWidget.prototype.nextAnimationFrame = null;
	CircleWidget.prototype.paintEvent = null;
	CircleWidget.prototype = null;
	CircleWidget = null;
	Window.prototype.createLabel = null;
	Window.prototype.closeEvent = null;
	Window.prototype = null;
	Window = null;	
	win = null;
	ScriptCleanupFunction = null;	
	//Write something to the Log Output Pane so we know that this Function executed successfully.
	Log("Finished script Cleanup!");
	StimulGL.cleanupScript();
}

var win = new Window();
try
{
	for (var i = 0; i < win.CircleWidgetCount; ++i) 
	{		
		win.timer.timeout.connect(win.CircleWidgets[i], win.CircleWidgets[i].nextAnimationFrame);
	}
}	
catch (e) 
{
	Log(".*. Something went wrong connecting Signal/Slots:" + e); //If a connection fails warn the user!
}
win.show();
