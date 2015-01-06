var ArrayMouses = new Array();

function Mouse(parent) 
{
	QGraphicsItem.call(this, parent);
	this.angle = 0;
	this.speed = 0;
	this.mouseEyeDirection = 0;
	var adjust = 0.5;
	this._boundingRect = new QRectF(-20 - adjust, -22 - adjust, 40 + adjust, 83 + adjust);
	this._shape = new QPainterPath();
	this._shape.addRect(-10, -20, 20, 40);
	this._brush = new QBrush(Qt.SolidPattern);
	this._tail = new QPainterPath(new QPointF(0, 20));
	this._tail.cubicTo(-5, 22, -5, 22, 0, 25);
	this._tail.cubicTo(5, 27, 5, 32, 0, 30);
	this._tail.cubicTo(-5, 32, -5, 42, 0, 35);
	this._pupilRect1 = new QRectF(-8 + this.mouseEyeDirection, -17, 4, 4);
	this._pupilRect2 = new QRectF(4 + this.mouseEyeDirection, -17, 4, 4);
	this.color = new QColor(Math.random()*256, Math.random()*256, Math.random()*256);
	this.setRotation(Math.random()*360);
	this.moveTimer = new QTimer(this);
}

Mouse.prototype = new QGraphicsItem();

Mouse.prototype.boundingRect = function() 
{ 
	return this._boundingRect; 
}

Mouse.prototype.shape = function() 
{ 
	return this._shape; 
}
	
Mouse.prototype.paint = function(painter, styleOptionGraphicsItem, widget) 
{
	// Body
	painter.setBrush(new QBrush(this.color));
	painter.drawEllipse(-10, -20, 20, 40);
	// Eyes
	this._brush.setColor(Qt.white);
	painter.setBrush(this._brush);
	painter.drawEllipse(-10, -17, 8, 8);
	painter.drawEllipse(2, -17, 8, 8);
	// Nose
	this._brush.setColor(Qt.black);
	painter.setBrush(this._brush);
	painter.drawEllipse(-2, -22, 4, 4);
	// Pupils
	painter.drawEllipse(this._pupilRect1);
	painter.drawEllipse(this._pupilRect2);
	// Ears
	//    if (this.scene().collidingItems(this).length == 0) FIXME: const QGraphicsItem*
	if (this.scene().items(this.pos()).length == 1)
		this._brush.setColor(Qt.darkYellow);
	else
		this._brush.setColor(Qt.red);
	painter.setBrush(this._brush);
	painter.drawEllipse(-17, -12, 16, 16);
	painter.drawEllipse(1, -12, 16, 16);
	// Tail
	painter.setBrush(Qt.NoBrush);
	painter.drawPath(this._tail);	
}

Mouse.prototype.move = function() 
{
	//Log("Move called.");
	// Don't move too far away
	var lineToCenter = new QLineF(Mouse.origo, this.mapFromScene(0, 0));
	if (lineToCenter.length() > 150) 
	{
		var angleToCenter = Math.acos(lineToCenter.dx() / lineToCenter.length());
		if (lineToCenter.dy() < 0)
			angleToCenter = Mouse.TWO_PI - angleToCenter;
		angleToCenter = Mouse.normalizeAngle((Math.PI - angleToCenter) + Math.PI / 2);
		if (angleToCenter < Math.PI && angleToCenter > Math.PI / 4) 
		{
			// Rotate left
			this.angle += (this.angle < -Math.PI / 2) ? 0.25 : -0.25;
		} 
		else if (angleToCenter >= Math.PI && angleToCenter < (Math.PI + Math.PI / 2 + Math.PI / 4)) 
		{
			// Rotate right
			this.angle += (this.angle < Math.PI / 2) ? 0.25 : -0.25;
		}
	} 
	else if (Math.sin(this.angle) < 0) 
	{
		this.angle += 0.25;
	} 
	else if (Math.sin(this.angle) > 0) 
	{
		this.angle -= 0.25;
	}
	// Try not to crash with any other mice
	var polygon = new QPolygonF(
		[ this.mapToScene(0, 0),
		  this.mapToScene(-30, -50),
		  this.mapToScene(30, -50) ] );
	var dangerMice = this.scene().items(polygon);
	for (var i = 0; i < dangerMice.length; ++i) 
	{
		var item = dangerMice[i];
		if (item == this)
			continue;
		var lineToMouse = new QLineF(Mouse.origo, this.mapFromItem(item, 0, 0));
		var angleToMouse = Math.acos(lineToMouse.dx() / lineToMouse.length());
		if (lineToMouse.dy() < 0)
			angleToMouse = Mouse.TWO_PI - angleToMouse;
		angleToMouse = Mouse.normalizeAngle((Math.PI - angleToMouse) + Math.PI / 2);
		if (angleToMouse >= 0 && angleToMouse < (Math.PI / 2)) 
		{
			// Rotate right
			this.angle += 0.5;
		} 
		else if (angleToMouse <= Mouse.TWO_PI && angleToMouse > (Mouse.TWO_PI - Math.PI / 2)) 
		{
			// Rotate left
			this.angle -= 0.5;
		}
	}
	// Add some random movement
	if (dangerMice.length < 1 && Math.random() < 0.1) 
	{
		if (Math.random() > 0.5)
			this.angle += Math.random() / 5;
		else
			this.angle -= Math.random() / 5;
	}
	this.speed += (-50 + Math.random() * 100) / 100.0;
	var dx = Math.sin(this.angle) * 10;
	this.mouseEyeDirection = (Math.abs(dx / 5) < 1) ? 0 : dx / 5;
	this.setRotation(this.rotation() + dx);
	this.setPos(this.mapToParent(0, -(3 + Math.sin(this.speed) * 3)));
}

Mouse.normalizeAngle = function(angle) 
{
	while (angle < 0)
		angle += Mouse.TWO_PI;
	while (angle > Mouse.TWO_PI)
		angle -= Mouse.TWO_PI;
	return angle;
}

CollidingMice.prototype = new QWidget();

CollidingMice.prototype.closeEvent = function() 
{
	Log("Mouse closeEvent() detected!");
	ScriptCleanupFunction();
}

function CollidingMice(parent) 
{
	QWidget.call(this, parent);
	var scene = new QGraphicsScene(this);
	scene.setSceneRect(-300, -300, 600, 600);
	scene.itemIndexMethod = QGraphicsScene.NoIndex;
	for (var i = 0; i < CollidingMice.MOUSE_COUNT; ++i) 
	{
		ArrayMouses[i] = new Mouse(this);
		ArrayMouses[i].setPos(Math.sin((i * 6.28) / CollidingMice.MOUSE_COUNT) * 200, Math.cos((i * 6.28) / CollidingMice.MOUSE_COUNT) * 200);
		scene.addItem(ArrayMouses[i]);		
	}
	var view = new QGraphicsView(scene, this);
	view.setRenderHint(QPainter.Antialiasing);
	view.backgroundBrush = new QBrush(new QPixmap("../Common/Images/Additional/cheese.png"));
	view.cacheMode = new QGraphicsView.CacheMode(QGraphicsView.CacheBackground);
	view.dragMode = QGraphicsView.ScrollHandDrag;
	view.viewportUpdateMode = QGraphicsView.FullViewportUpdate;
	var layout = new QGridLayout();
	layout.addWidget(view, 0, 0);
	this.setLayout(layout);
	this.setWindowTitle("Colliding Mice");
	this.resize(600, 500);
}

//This function makes sure that everything gets nicely cleaned before the script ends
function ScriptCleanupFunction()//Cleanup
{
	try
	{
		for (var i = 0; i < CollidingMice.MOUSE_COUNT; ++i) 
		{		
			ArrayMouses[i].moveTimer.timeout.disconnect(ArrayMouses[i], ArrayMouses[i].move);
		}
	}	
	catch (e) 
	{
		Log(".*. Something went wrong disconnecting Signal/Slots:" + e); //If a connection fails warn the user!
	}
	for (var i = 0; i < CollidingMice.MOUSE_COUNT; ++i) 
	{	
		ArrayMouses[i] = null;	
	}
	Mouse.prototype.paint = null;
	Mouse.prototype.move = null;
	Mouse.prototype.boundingRect = null;
	Mouse.prototype.shape = null;
	Mouse.prototype.closeEvent = null;
	Mouse.normalizeAngle = null;
	Mouse.TWO_PI = null;
	Mouse.origo = null;
	Mouse.prototype = null;	
	Mouse = null;	
	CollidingMice.MOUSE_COUNT = null;
	CollidingMice.prototype.closeEvent = null;
	CollidingMice.prototype = null;	
	CollidingMice = null;
	collidingMice = null;
	ArrayMouses = null;
	ScriptCleanupFunction = null;	
	//Write something to the Log Output Pane so we know that this Function executed successfully.
	Log("Finished script Cleanup!");
	BrainStim.cleanupScript();
}

CollidingMice.MOUSE_COUNT = 7;
Mouse.TWO_PI = Math.PI * 2;
Mouse.origo = new QPointF(0, 0);

var collidingMice = new CollidingMice(null);

try
{
	for (var i = 0; i < CollidingMice.MOUSE_COUNT; ++i) 
	{	
		ArrayMouses[i].moveTimer.timeout.connect(ArrayMouses[i], ArrayMouses[i].move);
		ArrayMouses[i].moveTimer.singleShot = false;
		ArrayMouses[i].moveTimer.start(1000 / 33);
	}
}	
catch (e) 
{
	Log(".*. Something went wrong connecting Signal/Slots:" + e); //If a connection fails warn the user!
}


collidingMice.show();