function tr(s) { return s; }

function Window(parent) 
{
	QWidget.call(this, parent);
	var echoGroup = new QGroupBox(tr("Echo"));
	var echoLabel = new QLabel(tr("Mode:"));
	this.echoComboBox = new QComboBox();
	this.echoComboBox.addItem(tr("Normal"));
	this.echoComboBox.addItem(tr("Password"));
	this.echoComboBox.addItem(tr("PasswordEchoOnEdit"));
	this.echoComboBox.addItem(tr("No Echo"));
	this.echoLineEdit = new QLineEdit();
	this.echoLineEdit.setFocus();
	var validatorGroup = new QGroupBox(tr("Validator"));
	var validatorLabel = new QLabel(tr("Type:"));
	this.validatorComboBox = new QComboBox();
	this.validatorComboBox.addItem(tr("No validator"));
	this.validatorComboBox.addItem(tr("Integer validator"));
	this.validatorComboBox.addItem(tr("Double validator"));
	this.validatorLineEdit = new QLineEdit();
	var alignmentGroup = new QGroupBox(tr("Alignment"));
	var alignmentLabel = new QLabel(tr("Type:"));
	this.alignmentComboBox = new QComboBox();
	this.alignmentComboBox.addItem(tr("Left"));
	this.alignmentComboBox.addItem(tr("Centered"));
	this.alignmentComboBox.addItem(tr("Right"));
	this.alignmentLineEdit = new QLineEdit();
	var inputMaskGroup = new QGroupBox(tr("Input mask"));
	var inputMaskLabel = new QLabel(tr("Type:"));
	this.inputMaskComboBox = new QComboBox;
	this.inputMaskComboBox.addItem(tr("No mask"));
	this.inputMaskComboBox.addItem(tr("Phone number"));
	this.inputMaskComboBox.addItem(tr("ISO date"));
	this.inputMaskComboBox.addItem(tr("License key"));
	this.inputMaskLineEdit = new QLineEdit();
	var accessGroup = new QGroupBox(tr("Access"));
	var accessLabel = new QLabel(tr("Read-only:"));
	this.accessComboBox = new QComboBox;
	this.accessComboBox.addItem(tr("False"));
	this.accessComboBox.addItem(tr("True"));
	this.accessLineEdit = new QLineEdit();
	var echoLayout = new QGridLayout;
	echoLayout.addWidget(echoLabel, 0, 0);
	echoLayout.addWidget(this.echoComboBox, 0, 1);
	echoLayout.addWidget(this.echoLineEdit, 1, 0, 1, 2);
	echoGroup.setLayout(echoLayout);
	var validatorLayout = new QGridLayout;
	validatorLayout.addWidget(validatorLabel, 0, 0);
	validatorLayout.addWidget(this.validatorComboBox, 0, 1);
	validatorLayout.addWidget(this.validatorLineEdit, 1, 0, 1, 2);
	validatorGroup.setLayout(validatorLayout);
	var alignmentLayout = new QGridLayout;
	alignmentLayout.addWidget(alignmentLabel, 0, 0);
	alignmentLayout.addWidget(this.alignmentComboBox, 0, 1);
	alignmentLayout.addWidget(this.alignmentLineEdit, 1, 0, 1, 2);
	alignmentGroup.setLayout(alignmentLayout);
	var inputMaskLayout = new QGridLayout;
	inputMaskLayout.addWidget(inputMaskLabel, 0, 0);
	inputMaskLayout.addWidget(this.inputMaskComboBox, 0, 1);
	inputMaskLayout.addWidget(this.inputMaskLineEdit, 1, 0, 1, 2);
	inputMaskGroup.setLayout(inputMaskLayout);
	var accessLayout = new QGridLayout;
	accessLayout.addWidget(accessLabel, 0, 0);
	accessLayout.addWidget(this.accessComboBox, 0, 1);
	accessLayout.addWidget(this.accessLineEdit, 1, 0, 1, 2);
	accessGroup.setLayout(accessLayout);
	var layout = new QGridLayout;
	layout.addWidget(echoGroup, 0, 0);
	layout.addWidget(validatorGroup, 1, 0);
	layout.addWidget(alignmentGroup, 2, 0);
	layout.addWidget(inputMaskGroup, 0, 1);
	layout.addWidget(accessGroup, 1, 1);
	this.setLayout(layout);
	this.setWindowTitle(tr("Line Edits"));
}

Window.prototype = new QWidget();

Window.prototype.echoChanged = function(index) 
{
	switch (index) 
	{
	case 0:
		this.echoLineEdit.echoMode = QLineEdit.Normal;
		break;
	case 1:
		this.echoLineEdit.echoMode = QLineEdit.Password;
		break;
	case 2:
		this.echoLineEdit.echoMode = QLineEdit.PasswordEchoOnEdit;
		break;
	case 3:
		this.echoLineEdit.echoMode = QLineEdit.NoEcho;
		break;
	}
}

Window.prototype.validatorChanged = function(index) 
{
	switch (index) 
	{
	case 0:
		this.validatorLineEdit.setValidator(null);
		break;
	case 1:
		this.validatorLineEdit.setValidator(new QIntValidator(this.validatorLineEdit));
		break;
	case 2:
		this.validatorLineEdit.setValidator(new QDoubleValidator(-999.0, 999.0, 2, this.validatorLineEdit));
		break;
	}
	this.validatorLineEdit.clear();
}

Window.prototype.alignmentChanged = function(index) 
{
	switch (index) 
	{
	case 0:
		this.alignmentLineEdit.alignment = Qt.Alignment(Qt.AlignLeft);
		break;
	case 1:
		this.alignmentLineEdit.alignment = Qt.Alignment(Qt.AlignCenter);
		break;
	case 2:
		this.alignmentLineEdit.alignment = Qt.Alignment(Qt.AlignRight);
		}
}

Window.prototype.inputMaskChanged = function(index)
{
	switch (index) 
	{
	case 0:
		this.inputMaskLineEdit.inputMask = "";
		break;
	case 1:
		this.inputMaskLineEdit.inputMask = "+99 99 99 99 99;_";
		break;
	case 2:
		this.inputMaskLineEdit.inputMask = "0000-00-00";
		this.inputMaskLineEdit.text = "00000000";
		this.inputMaskLineEdit.cursorPosition = 0;
		break;
	case 3:
		this.inputMaskLineEdit.inputMask = ">AAAAA-AAAAA-AAAAA-AAAAA-AAAAA;#";
	}
}

Window.prototype.accessChanged = function(index) 
{
	switch (index) 
	{
	case 0:
		this.accessLineEdit.readOnly = false;
		break;
	case 1:
		this.accessLineEdit.readOnly = true;
		}
}

Window.prototype.closeEvent = function() 
{
	Log("Window closeEvent() detected!");
	ScriptCleanupFunction();
}

//This function makes sure that everything gets nicely cleaned before the script ends
function ScriptCleanupFunction()//Cleanup
{
	try
	{
		win.echoComboBox["activated(int)"].disconnect(win, "echoChanged");
		win.validatorComboBox["activated(int)"].disconnect(win, "validatorChanged");
		win.alignmentComboBox["activated(int)"].disconnect(win, "alignmentChanged");
		win.inputMaskComboBox["activated(int)"].disconnect(win, "inputMaskChanged");
		win.accessComboBox["activated(int)"].disconnect(win, "accessChanged");
	}	
	catch (e) 
	{
		Log(".*. Something went wrong disconnecting Signal/Slots:" + e); //If a connection fails warn the user!
	}	
	tr = null;
	Window.prototype.echoChanged = null;
	Window.prototype.validatorChanged = null;
	Window.prototype.alignmentChanged = null;
	Window.prototype.inputMaskChanged = null;
	Window.prototype.accessChanged = null;
	Window.prototype.closeEvent = null;
	Window.prototype = null;
	Window = null;
	win = null;	
	ScriptCleanupFunction = null;	
	//Write something to the Log Output Pane so we know that this Function executed successfully.
	Log("Finished script Cleanup!");
	StimulGL.cleanupScript();
}

var win = new Window(null);
try
{
	win.echoComboBox["activated(int)"].connect(win, "echoChanged");
	win.validatorComboBox["activated(int)"].connect(win, "validatorChanged");
	win.alignmentComboBox["activated(int)"].connect(win, "alignmentChanged");
	win.inputMaskComboBox["activated(int)"].connect(win, "inputMaskChanged");
	win.accessComboBox["activated(int)"].connect(win, "accessChanged");
}	
catch (e) 
{
	Log(".*. Something went wrong connecting Signal/Slots:" + e); //If a connection fails warn the user!
}
win.show();