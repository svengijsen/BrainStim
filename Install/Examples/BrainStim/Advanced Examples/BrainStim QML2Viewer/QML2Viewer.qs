var QML2ViewerObj = new QML2Viewer();
var invokeTimer = new QTimer();
var sScript = CreateQMLScript();

var bRunAsFile = true;  	//Whether we run the QML content from a file or a script string
invokeTimer.interval = 2000; 	//Set the time in milliseconds when the QML function gets invoked

//This function makes sure that everything gets nicely cleaned before the script ends
function ScriptCleanupFunction()//Cleanup
{
	//Disconnect the signal/slots
	ConnectDisconnectScriptFunctions(false);
	//Set all objects to null
	invokeTimer = null;
	QML2ViewerObj = null;	
	//Set all functions to null
	CreateQMLScript = null;
	ScriptCleanupFunction = null;
	UserWantsToClose = null;
	ConnectDisconnectScriptFunctions = null;
	ChangeQMLText = null;
	//Write something to the Log Output Pane so we know that this Function executed successfully.
	Log("Finished script Cleanup!");
	BrainStim.cleanupScript();
}

function UserWantsToClose()
{
	Log("UserWantsToClose");
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
			QML2ViewerObj.UserWantsToClose.connect(this, this.UserWantsToClose);
			invokeTimer.timeout.connect(this, this.ChangeQMLText);
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
			QML2ViewerObj.UserWantsToClose.disconnect(this, this.UserWantsToClose);
			invokeTimer.timeout.disconnect(this, this.ChangeQMLText);
		} 
		catch (e) 
		{
			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e); //If a disconnection fails warn the user!
		}		
	}	
}

function CreateQMLScript()
{
	var sdelimiter = " ";
	var sRetVal = "";	
	sRetVal = sRetVal + "import QtQuick 2.0;";
	sRetVal = sRetVal + sdelimiter + "import BrainStim.Interface 1.0;";
	sRetVal = sRetVal + sdelimiter + "Rectangle";
	sRetVal = sRetVal + sdelimiter + "{";
	sRetVal = sRetVal + sdelimiter + "width: 360;";
	sRetVal = sRetVal + sdelimiter + "height: 360;";
	sRetVal = sRetVal + sdelimiter + "Text";
	sRetVal = sRetVal + sdelimiter + "{";
	sRetVal = sRetVal + sdelimiter + "id: textID;";
	sRetVal = sRetVal + sdelimiter + "text: \"Hello World!\";";
	sRetVal = sRetVal + sdelimiter + "anchors.centerIn: parent;";
	sRetVal = sRetVal + sdelimiter + "font.family: \"Helvetica\";";
	sRetVal = sRetVal + sdelimiter + "font.pointSize: 100;";
	sRetVal = sRetVal + sdelimiter + "font.bold: true;";
	sRetVal = sRetVal + sdelimiter + "renderType: Text.NativeRendering;";
	sRetVal = sRetVal + sdelimiter + "}";
	sRetVal = sRetVal + sdelimiter + "MouseArea ";
	sRetVal = sRetVal + sdelimiter + "{";
	sRetVal = sRetVal + sdelimiter + "anchors.fill: parent;";
	sRetVal = sRetVal + sdelimiter + "onClicked:";
	sRetVal = sRetVal + sdelimiter + "{";
	sRetVal = sRetVal + sdelimiter + "Qt.quit();";
	sRetVal = sRetVal + sdelimiter + "}";
	sRetVal = sRetVal + sdelimiter + "}";
	sRetVal = sRetVal + sdelimiter + "Item"; 
	sRetVal = sRetVal + sdelimiter + "{";
	sRetVal = sRetVal + sdelimiter + "id: root;";
	sRetVal = sRetVal + sdelimiter + "property string someValue: BrainStimAPI.message;";
	sRetVal = sRetVal + sdelimiter + "Component.onCompleted:";
	sRetVal = sRetVal + sdelimiter + "{";
	sRetVal = sRetVal + sdelimiter + "someValue = BrainStimAPI.doSomething();";
	sRetVal = sRetVal + sdelimiter + "textID.text = someValue;";
	sRetVal = sRetVal + sdelimiter + "}";
	sRetVal = sRetVal + sdelimiter + "}";
	sRetVal = sRetVal + sdelimiter + "}";
	return sRetVal;
}

function ChangeQMLText()
{
	if(QML2ViewerObj)
		QML2ViewerObj.invokeQml2Method("functions", "settext", "xxx");
}

invokeTimer.singleShot = true; // in-case if setTimout and false in-case of setInterval 
ConnectDisconnectScriptFunctions(true);
//QML2ViewerObj.setActiveStimuliOutputScreen(1);
QML2ViewerObj.setWindowMode(false,false);
QML2ViewerObj.setTopLeftWindowPosition(200, 100);
//Log(BrainStim.getCurrentDateTimeStamp());
if(bRunAsFile)
	QML2ViewerObj.executeQML2Document(BrainStim.getActiveDocumentFileLocation() + "/Qml2_SimpleText.qml",true);//sSource, bIsFile
else
	QML2ViewerObj.executeQML2Document(sScript,false);
invokeTimer.start();
