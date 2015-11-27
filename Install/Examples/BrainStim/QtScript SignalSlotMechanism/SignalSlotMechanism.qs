//This example shows how you can make use of Signal/Slot connections

var cDialog = new QDialog();			//Create a QDialog object
var cLayout = new QGridLayout();		//Create a QGridLayout object
var cClickButton = new QPushButton();	//Create a QPushButton object
var cExitButton = new QPushButton();	//Create a QPushButton object

//Set the Column stretch for column index 0 to 1 (maximal)
cLayout.setColumnStretch(0, 1);
//Set the caption of the buttons
cClickButton.text = "Click me!";
cExitButton.text = "Exit script";
//Add the buttons to the layout
cLayout.addWidget(cClickButton, 0, 0);
cLayout.addWidget(cExitButton, 1, 0);
//Set the new layout to be used by the dialog
cDialog.setLayout(cLayout);
//Connect Signal/Slots for the buttons
cClickButton.clicked.connect(onClickButtonClicked);
cExitButton.clicked.connect(onExitButtonClicked);
//Show the dialog
cDialog.show();

//////////////////////////////Functions//////////////////////////////

function onClickButtonClicked()
{
	Log("onClickButtonClicked() slot called");
}

function onExitButtonClicked()
{
	Log("onExitButtonClicked() slot called");
	CleanupScript();
}

//Override virtual function 
cDialog.closeEvent = function()
{
	Log("cDialog closeEvent() detected!");
	CleanupScript();
}

function CleanupScript()
{
	//Disconnect Signals/Slots
	cClickButton.clicked.disconnect(onClickButtonClicked);
	cExitButton.clicked.disconnect(onExitButtonClicked);
	//Set Functions to null
	onClickButtonClicked = null;
	onExitButtonClicked = null;
	cDialog.closeEvent = null;
	CleanupScript = null;
	//Close the Dialog
	cDialog.close();
	//Set all allocated objects to null;
	cDialog = null;
	cLayout = null;
	cClickButton = null;
	cExitButton = null;
	//Call BrainStim.cleanupScript();
	Log("Finished script cleanup, ready for garbage collection!");
	BrainStim.cleanupScript();
}