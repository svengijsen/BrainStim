function Dialog(parent)
{
	QDialog.call(this, parent);
	var frameStyle = QFrame.Sunken | QFrame.Panel;
	var layout = new QGridLayout;
	layout.setColumnStretch(1, 1);	
	layout.setColumnMinimumWidth(1, 500);
	/////////////////////////////////////////////////////
	this.testLabel = new QLabel;
	this.testLabel.setFrameStyle(frameStyle);
	this.testLabel.text = "Press the button";
	this.testButton = new QPushButton("Test");	
	layout.addWidget(this.testButton, 0, 0);
	layout.addWidget(this.testLabel, 0, 1);
	/////////////////////////////////////////////////////
	this.exitButton = new QPushButton("Exit");	
	layout.addWidget(this.exitButton, 99, 0);
	/////////////////////////////////////////////////////
	this.setLayout(layout);
	this.windowTitle = "Menu Dialog";
}

Dialog.prototype = new QDialog();

Dialog.prototype.testFunction = function()
{
	mainDialog.testLabel.text = "Button Pressed..."
}

Dialog.prototype.keyPressEvent = function(e /*QKeyEvent e*/)
{
	if(e.key() == Qt.Key_Escape)
		this.close();
	else
		QDialog.keyPressEvent(e);
}

Dialog.prototype.closeEvent = function() 
{
	Log("Dialog closeEvent() detected!");
	CleanupScript();
}

function ConnectDisconnectScriptFunctions(Connect)
//This function can connect or disconnect all signal/slot connections defined by the boolean parameter 
{
	if(Connect) //This parameter defines whether we should connect or disconnect the signal/slots.
	{
		Log("... Connecting Signal/Slots");
		try 
		{	
			mainDialog.testButton["clicked()"].connect(mainDialog, mainDialog.testFunction);
			mainDialog.exitButton["clicked()"].connect(this, this.CleanupScript);			
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
			mainDialog.testButton["clicked()"].disconnect(mainDialog, mainDialog.testFunction);
			mainDialog.exitButton["clicked()"].disconnect(this, this.CleanupScript);			
		} 
		catch (e) 
		{
			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e); //If a disconnection fails warn the user!
		}		
	}	
}

function CleanupScript()
{
	mainDialog.close();
	ConnectDisconnectScriptFunctions(false);
	//Functions
	ConnectDisconnectScriptFunctions = null;
	CleanupScript = null;	
	//Objects
	mainDialog = null;
	//Dialog
	Dialog.prototype.keyPressEvent = null;
	Dialog.prototype.closeEvent = null;	
	Dialog.prototype.testFunction = null;
	Dialog.prototype = null;
	Dialog = null;
	//Post
	Log("\nFinished script cleanup, ready for garbage collection!");
	BrainStim.cleanupScript();	
}

var mainDialog = new Dialog();
mainDialog.show();
ConnectDisconnectScriptFunctions(true);