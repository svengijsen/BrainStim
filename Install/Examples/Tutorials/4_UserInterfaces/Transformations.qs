function tr(s) { return s; }

Operation = { NoTransformation: 0, Rotate: 1, Scale: 2, Translate: 3 };

function RenderArea(parent)
{
	QWidget.call(this, parent);
	var newFont = new QFont(this.font);
	newFont.setPixelSize(12);
	this.font = newFont;
	var fontMetrics = new QFontMetrics(newFont);
	this.xBoundingRect = fontMetrics.boundingRect(tr("x"));
	this.yBoundingRect = fontMetrics.boundingRect(tr("y"));
	this.shape = new QPainterPath();
	this.operations = new Array();
}

RenderArea.prototype = new QWidget();

RenderArea.prototype.setOperations = function(operations)
{
	this.operations = operations;
	this.update();
}

RenderArea.prototype.setShape = function(shape)
{
	this.shape = shape;
	this.update();
}

RenderArea.prototype.getMinimumSizeHint = function()
{
	return new QSize(182, 182);
}

RenderArea.prototype.getSizeHint = function()
{
	return new QSize(232, 232);
}

RenderArea.prototype.paintEvent = function(event)
{
	var painter = new QPainter();
	painter.begin(this);
	painter.setRenderHint(QPainter.Antialiasing);
	painter.fillRect(event.rect(), new QBrush(new QColor(Qt.white)));
	painter.translate(66, 66);
	painter.save();
	this.transformPainter(painter);
	this.drawShape(painter);
	painter.restore();
	this.drawOutline(painter);
	this.transformPainter(painter);
	this.drawCoordinates(painter);
	painter.end();
}

RenderArea.prototype.drawCoordinates = function(painter)
{
	painter.setPen(new QColor(Qt.red));
	painter.drawLine(0, 0, 50, 0);
	painter.drawLine(48, -2, 50, 0);
	painter.drawLine(48, 2, 50, 0);
	painter.drawText(60 - this.xBoundingRect.width() / 2, 0 + this.xBoundingRect.height() / 2, tr("x"));
	painter.drawLine(0, 0, 0, 50);
	painter.drawLine(-2, 48, 0, 50);
	painter.drawLine(2, 48, 0, 50);
	painter.drawText(0 - this.yBoundingRect.width() / 2, 60 + this.yBoundingRect.height() / 2, tr("y"));
}

RenderArea.prototype.drawOutline = function(painter)
{
	painter.setPen(new QPen(new QColor(Qt.darkGreen)));
	painter.setPen(new QPen(Qt.DashLine));
	painter.setBrush(new QBrush(Qt.NoBrush));
	painter.drawRect(0, 0, 100, 100);
}

RenderArea.prototype.drawShape = function(painter)
{
	painter.fillPath(this.shape, new QColor(Qt.blue));
}

RenderArea.prototype.transformPainter = function(painter)
{
	for (var i = 0; i < this.operations.length; ++i) 
	{
		switch (this.operations[i]) 
		{
		case Operation.Translate:
			painter.translate(50, 50);
			break;
		case Operation.Scale:
			painter.scale(0.75, 0.75);
			break;
		case Operation.Rotate:
			painter.rotate(60);
			break;
		case Operation.NoTransformation:
		default:
		    ;
		}
	}
}

function Window(parent)
{
	QWidget.call(this, parent);
	this.originalRenderArea = new RenderArea();
	this.shapeComboBox = new QComboBox();
	this.shapeComboBox.addItem(tr("Clock"));
	this.shapeComboBox.addItem(tr("House"));
	this.shapeComboBox.addItem(tr("Text"));
	this.shapeComboBox.addItem(tr("Truck"));
	var layout = new QGridLayout();
	layout.addWidget(this.originalRenderArea, 0, 0);
	layout.addWidget(this.shapeComboBox, 1, 0);
	this.transformedRenderAreas = new Array(Window.NumTransformedAreas);
	this.operationComboBoxes = new Array(Window.NumTransformedAreas);
	for (var i = 0; i < Window.NumTransformedAreas; ++i) 
	{
		this.transformedRenderAreas[i] = new RenderArea();
		this.operationComboBoxes[i] = new QComboBox;
		this.operationComboBoxes[i].addItem(tr("No transformation"));
		this.operationComboBoxes[i].addItem(tr("Rotate by 60\xB0"));
		this.operationComboBoxes[i].addItem(tr("Scale to 75%"));
		this.operationComboBoxes[i].addItem(tr("Translate by (50, 50)"));
		layout.addWidget(this.transformedRenderAreas[i], 0, i + 1);
		layout.addWidget(this.operationComboBoxes[i], 1, i + 1);
	}
	this.setLayout(layout);
	this.setupShapes();
	this.shapeSelected(0);
	this.windowTitle = tr("Transformations");
}

Window.prototype = new QWidget();

Window.NumTransformedAreas = 3;

Window.prototype.setupShapes = function()
{
	var truck = new QPainterPath();
	var clock = new QPainterPath();
	var house = new QPainterPath();
	var text = new QPainterPath();
	truck.setFillRule(Qt.WindingFill);
	truck.moveTo(0.0, 87.0);
	truck.lineTo(0.0, 60.0);
	truck.lineTo(10.0, 60.0);
	truck.lineTo(35.0, 35.0);
	truck.lineTo(100.0, 35.0);
	truck.lineTo(100.0, 87.0);
	truck.lineTo(0.0, 87.0);
	truck.moveTo(17.0, 60.0);
	truck.lineTo(55.0, 60.0);
	truck.lineTo(55.0, 40.0);
	truck.lineTo(37.0, 40.0);
	truck.lineTo(17.0, 60.0);
	truck.addEllipse(17.0, 75.0, 25.0, 25.0);
	truck.addEllipse(63.0, 75.0, 25.0, 25.0);	
	clock.addEllipse(-50.0, -50.0, 100.0, 100.0);
	clock.addEllipse(-48.0, -48.0, 96.0, 96.0);
	clock.moveTo(0.0, 0.0);
	clock.lineTo(-2.0, -2.0);
	clock.lineTo(0.0, -42.0);
	clock.lineTo(2.0, -2.0);
	clock.lineTo(0.0, 0.0);
	clock.moveTo(0.0, 0.0);
	clock.lineTo(2.732, -0.732);
	clock.lineTo(24.495, 14.142);
	clock.lineTo(0.732, 2.732);
	clock.lineTo(0.0, 0.0);
	house.moveTo(-45.0, -20.0);
	house.lineTo(0.0, -45.0);
	house.lineTo(45.0, -20.0);
	house.lineTo(45.0, 45.0);
	house.lineTo(-45.0, 45.0);
	house.lineTo(-45.0, -20.0);
	house.addRect(15.0, 5.0, 20.0, 35.0);
	house.addRect(-35.0, -15.0, 25.0, 25.0);
	var font = new QFont();
	font.setPixelSize(50);
	var fontBoundingRect = new QFontMetrics(font).boundingRect(tr("Qt"));	
	text.addText(-new QPointF(fontBoundingRect.center()), font, tr("Qt"));
	this.shapes = new Array();
	this.shapes.push(clock);
	this.shapes.push(house);
	this.shapes.push(text);
	this.shapes.push(truck);
}

Window.prototype.operationChanged = function()
{
	var operations = new Array();
	for (var i = 0; i < Window.NumTransformedAreas; ++i) 
	{
		var index = this.operationComboBoxes[i].currentIndex;
		operations.push(index);
		this.transformedRenderAreas[i].setOperations(operations.slice());
	}
}

Window.prototype.shapeSelected = function(index)
{
	var shape = this.shapes[index];
	this.originalRenderArea.setShape(shape);
	for (var i = 0; i < Window.NumTransformedAreas; ++i)
		this.transformedRenderAreas[i].setShape(shape);
}

Window.prototype.closeEvent = function() 
{
	Log("Screenshot closeEvent() detected!");
	ScriptCleanupFunction();
}

//This function makes sure that everything gets nicely cleaned before the script ends
function ScriptCleanupFunction()//Cleanup
{
	try
	{
		for (var i = 0; i < Window.NumTransformedAreas; ++i) 
		{
			win.operationComboBoxes[i]["activated(int)"].disconnect(win, win.operationChanged);	
		}
		win.shapeComboBox["activated(int)"].disconnect(win, win.shapeSelected);
	}	
	catch (e) 
	{
		Log(".*. Something went wrong disconnecting Signal/Slots:" + e); //If a connection fails warn the user!
	}
	tr = null;
	Operation = null;
	RenderArea.prototype.setOperations = null;
	RenderArea.prototype.setShape = null;
	RenderArea.prototype.getMinimumSizeHint = null;
	RenderArea.prototype.getSizeHint = null;
	RenderArea.prototype.paintEvent = null;
	RenderArea.prototype.drawCoordinates = null;
	RenderArea.prototype.drawOutline = null;
	RenderArea.prototype.drawShape = null;
	RenderArea.prototype.transformPainter = null;
	RenderArea.prototype = null;	
	RenderArea = null;	
	Window.NumTransformedAreas = null;
	Window.prototype.setupShapes = null;
	Window.prototype.operationChanged = null;
	Window.prototype.shapeSelected = null;
	Window.prototype.closeEvent = null;
	Window.prototype = null;
	Window = null;
	win = null;	
	ScriptCleanupFunction = null;	
	//Write something to the Log Output Pane so we know that this Function executed successfully.
	Log("Finished script Cleanup!");
	BrainStim.cleanupScript();
}

var win = new Window();
try
{
	for (var i = 0; i < Window.NumTransformedAreas; ++i) 
	{
		win.operationComboBoxes[i]["activated(int)"].connect(win, win.operationChanged);	
	}
	win.shapeComboBox["activated(int)"].connect(win, win.shapeSelected);
}	
catch (e) 
{
	Log(".*. Something went wrong connecting Signal/Slots:" + e); //If a connection fails warn the user!
}
win.show();