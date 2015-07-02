Log("MainControlDialog.qs included...");

function Dialog(parent)
{
	QDialog.call(this, parent);

	//Extend here...
}

//Extend the prototype...
Dialog.prototype = new QDialog();

Dialog.prototype.keyPressEvent = function(e /*QKeyEvent e*/)
{
	if(e.key() == Qt.Key_Escape)
	{
		this.close();
	}
	else
	{
		QDialog.keyPressEvent(e);
	}
}

Dialog.prototype.closeEvent = function() 
{
	Log("Dialog closeEvent() detected!");
	_MainControlDialogCleanup();
}

function _MainControlDialogCleanup() 
{
	Log("Dialog cleanup() called(begin)");
	//Cleanup the Dialog prototype
	//Dialog.prototype.addWidget = null;
	Dialog.prototype.keyPressEvent = null;
	Dialog.prototype.closeEvent = null;
	Dialog.prototype = null;
	Dialog = null;
	//Cleanup local functions
	_MainControlDialogCleanup = null;
	//Finally call the Brainstim cleanupScript() method to force a garbage collection
	BrainStim.cleanupScript();
	Log("Dialog cleanup() called(end)");
}

/*
//example usage...
mainDialog = new Dialog();
mainDialog.show();

//Set the dialogs title
mainDialog.windowTitle = "Custom Menu Dialog Title";
//Create and configure Layout
var customLayout = new QGridLayout;
customLayout.setColumnStretch(1, 1);	
customLayout.setColumnMinimumWidth(1, 500);
//Create and configure new label
var customLabel1 = new QLabel;
customLabel1.setFrameStyle(QFrame.Sunken | QFrame.Panel);
//Create and configure new button
var customButton1 = new QPushButton("Button #1");
//Add created controls to layout and set the new layout
customLayout.addWidget(customButton1, 0, 0);
customLayout.addWidget(customLabel1, 0, 1);
mainDialog.setLayout(customLayout);

Pause(3000);

mainDialog.close();
mainDialog = null;
*/