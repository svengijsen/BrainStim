function tr(s) { return s; }

function Screenshot(parent) 
{ 
	QWidget.call(this, parent);
	this.screenshotLabel = new QLabel();
	this.screenshotLabel.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding);
	this.screenshotLabel.alignment = Qt.Alignment(Qt.AlignCenter);
	this.screenshotLabel.setMinimumSize(240, 160);
	this.createOptionsGroupBox();
	this.createButtonsLayout();
	this.mainLayout = new QVBoxLayout();
	this.mainLayout.addWidget(this.screenshotLabel,0 ,0);
	this.mainLayout.addWidget(this.optionsGroupBox,0 ,0);
	this.mainLayout.addLayout(this.buttonsLayout);
	this.setLayout(this.mainLayout);
	this.shootScreen();
	this.delaySpinBox.setValue(5);
	this.singleShot = new QTimer();
	this.singleShot.singleShot = true;	
	this.windowIcon = new QIcon("classpath:com/trolltech/images/qt-logo.png");
	this.windowTitle = tr("Screenshot");
	this.resize(600, 400);	
}

Screenshot.prototype = new QWidget();

Screenshot.prototype.resizeEvent = function(event) 
{	
	var scaledSize = this.originalPixmap.size();
	scaledSize.scale(this.screenshotLabel.size, Qt.AspectRatioMode.KeepAspectRatio);
	if (this.screenshotLabel.pixmap != null || scaledSize != this.screenshotLabel.pixmap.size())
		this.updateScreenshotLabel();
}

Screenshot.prototype.newScreenshot = function() 
{
	if ( this.hideThisWindowCheckBox.checked)
		this.hide();
	this.newScreenshotButton.setDisabled(true);
	//FIXME    
	// QTimer.singleShot(this.delaySpinBox.value * 1000, 
	//                      this, this.shootScreen);  
	this.singleShot.start(this.delaySpinBox.value * 1000);
}

Screenshot.prototype.saveScreenshot = function() 
{
	var format = "png";
	var initialPath = QDir.currentPath() + tr("/untitled.") + format;
	var filter = tr(format.toUpperCase() + " Files (*." + format + ");;All Files (*)");
	var fileName = QFileDialog.getSaveFileName(this, tr("Save As"), initialPath, filter, null, null);
	// new QFileDialog.Option.Filter(filter)); //FIXME
	if (fileName != "")
		this.originalPixmap.save(fileName); //, format); //FIXME
}

Screenshot.prototype.shootScreen = function() 
{
	if (this.delaySpinBox.value != 0)
		QApplication.beep();
	this.originalPixmap = null;
	this.originalPixmap = QPixmap.grabWindow(QApplication.desktop().winId());
	this.updateScreenshotLabel();
	this.newScreenshotButton.enabled = true;
	if (this.hideThisWindowCheckBox.checked)
		this.show();
}

Screenshot.prototype.updateCheckBox = function() 
{
	if (this.delaySpinBox.value)
		this.hideThisWindowCheckBox.setDisabled(true);
	else
		this.hideThisWindowCheckBox.setDisabled(false);
}

Screenshot.prototype.createOptionsGroupBox = function() 
{
	this.optionsGroupBox = new QGroupBox(tr("Options"));
	this.delaySpinBox = new QSpinBox();
	this.delaySpinBox.suffix = tr(" s");
	this.delaySpinBox.maximum = 60;
	this.delaySpinBoxLabel = new QLabel(tr("Screenshot Delay:"));
	this.hideThisWindowCheckBox = new QCheckBox(tr("Hide This Window"));
	this.optionsGroupBoxLayout = new QGridLayout();
	this.optionsGroupBoxLayout.addWidget(this.delaySpinBoxLabel, 0, 0);
	this.optionsGroupBoxLayout.addWidget(this.delaySpinBox, 0, 1);
	this.optionsGroupBoxLayout.addWidget(this.hideThisWindowCheckBox, 1, 0, 1, 2);
	this.optionsGroupBox.setLayout(this.optionsGroupBoxLayout);
}

Screenshot.prototype.createButtonsLayout = function() 
{
	this.newScreenshotButton = new QPushButton(tr("New Screenshot"));
	this.saveScreenshotButton = new QPushButton(tr("Save Screenshot"));
	this.quitScreenshotButton = new QPushButton(tr("Quit"));
	this.buttonsLayout = new QHBoxLayout();
	this.buttonsLayout.addStretch();
	this.buttonsLayout.addWidget(this.newScreenshotButton, 0, 0);
	this.buttonsLayout.addWidget(this.saveScreenshotButton, 0, 0);
	this.buttonsLayout.addWidget(this.quitScreenshotButton, 0, 0);
}

Screenshot.prototype.updateScreenshotLabel = function() 
{
	this.screenshotLabel.setPixmap(this.originalPixmap.scaled(this.screenshotLabel.size, Qt.KeepAspectRatio, Qt.SmoothTransformation));
}

Screenshot.prototype.closeEvent = function() 
{
	Log("Screenshot closeEvent() detected!");
	ScriptCleanupFunction();
}

//This function makes sure that everything gets nicely cleaned before the script ends
function ScriptCleanupFunction()//Cleanup
{
	screenshot.singleShot.stop();
	try
	{
		screenshot.newScreenshotButton.clicked.disconnect(screenshot, screenshot.newScreenshot);
		screenshot.saveScreenshotButton.clicked.disconnect(screenshot, screenshot.saveScreenshot);
		screenshot.quitScreenshotButton.clicked.disconnect(screenshot, screenshot.close);			
		screenshot.delaySpinBox['valueChanged(int)'].disconnect(screenshot, screenshot.updateCheckBox);
		screenshot.singleShot.timeout.disconnect(screenshot, screenshot.shootScreen);
	}	
	catch (e) 
	{
		Log(".*. Something went wrong disconnecting Signal/Slots:" + e); //If a connection fails warn the user!
	}
	tr = null;
	Screenshot.prototype.resizeEvent = null;
	Screenshot.prototype.newScreenshot = null;
	Screenshot.prototype.saveScreenshot = null;
	Screenshot.prototype.shootScreen = null;
	Screenshot.prototype.updateCheckBox = null;
	Screenshot.prototype.createOptionsGroupBox = null;
	Screenshot.prototype.createButtonsLayout = null;
	Screenshot.prototype.updateScreenshotLabel = null;
	Screenshot.prototype.closeEvent = null;
	Screenshot.prototype = null;
	Screenshot = null;
	screenshot.singleShot = null;
	screenshot = null;	
	ScriptCleanupFunction = null;	
	//Write something to the Log Output Pane so we know that this Function executed successfully.
	Log("Finished script Cleanup!");
	StimulGL.cleanupScript();
}

var screenshot = new Screenshot();
try
{
	screenshot.newScreenshotButton.clicked.connect(screenshot, screenshot.newScreenshot);
	screenshot.saveScreenshotButton.clicked.connect(screenshot, screenshot.saveScreenshot);
	screenshot.quitScreenshotButton.clicked.connect(screenshot, screenshot.close);	
	screenshot.delaySpinBox['valueChanged(int)'].connect(screenshot, screenshot.updateCheckBox);
	screenshot.singleShot.timeout.connect(screenshot, screenshot.shootScreen);
}	
catch (e) 
{
	Log(".*. Something went wrong connecting Signal/Slots:" + e); //If a connection fails warn the user!
}
screenshot.show();