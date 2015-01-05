function tr(s) { return s; }

function Dialog(parent)
{
	QDialog.call(this, parent);
	this.errorMessageDialog = new QErrorMessage(this);
	var frameStyle = QFrame.Sunken | QFrame.Panel;
	this.integerLabel = new QLabel;
	this.integerLabel.setFrameStyle(frameStyle);
	this.integerButton = new QPushButton(tr("QInputDialog::get&Integer()"));
	this.doubleLabel = new QLabel;
	this.doubleLabel.setFrameStyle(frameStyle);
	this.doubleButton = new QPushButton(tr("QInputDialog::get&Double()"));
	this.itemLabel = new QLabel;
	this.itemLabel.setFrameStyle(frameStyle);
	this.itemButton = new QPushButton(tr("QInputDialog::getIte&m()"));
	this.textLabel = new QLabel;
	this.textLabel.setFrameStyle(frameStyle);
	this.textButton = new QPushButton(tr("QInputDialog::get&Text()"));
	this.colorLabel = new QLabel;
	this.colorLabel.setFrameStyle(frameStyle);
	this.colorButton = new QPushButton(tr("QColorDialog::get&Color()"));
	this.fontLabel = new QLabel;
	this.fontLabel.setFrameStyle(frameStyle);
	this.fontButton = new QPushButton(tr("QFontDialog::get&Font()"));
	this.directoryLabel = new QLabel;
	this.directoryLabel.setFrameStyle(frameStyle);
	this.directoryButton = new QPushButton(tr("QFileDialog::getE&xistingDirectory()"));
	this.openFilePaths = ".";
	this.openFileNameLabel = new QLabel;
	this.openFileNameLabel.setFrameStyle(frameStyle);
	this.openFileNameButton = new QPushButton(tr("QFileDialog::get&OpenFileName()"));
	this.openFileNamesLabel = new QLabel;
	this.openFileNamesLabel.setFrameStyle(frameStyle);
	this.openFileNamesButton = new QPushButton(tr("QFileDialog::&getOpenFileNames()"));
	this.saveFileNameLabel = new QLabel;
	this.saveFileNameLabel.setFrameStyle(frameStyle);
	this.saveFileNameButton = new QPushButton(tr("QFileDialog::get&SaveFileName()"));
	this.criticalLabel = new QLabel;
	this.criticalLabel.setFrameStyle(frameStyle);
	this.criticalButton = new QPushButton(tr("QMessageBox::critica&l()"));
	this.informationLabel = new QLabel;
	this.informationLabel.setFrameStyle(frameStyle);
	this.informationButton = new QPushButton(tr("QMessageBox::i&nformation()"));
	this.questionLabel = new QLabel;
	this.questionLabel.setFrameStyle(frameStyle);
	this.questionButton = new QPushButton(tr("QMessageBox::&question()"));
	this.warningLabel = new QLabel;
	this.warningLabel.setFrameStyle(frameStyle);
	this.warningButton = new QPushButton(tr("QMessageBox::&warning()"));
	this.errorLabel = new QLabel;
	this.errorLabel.setFrameStyle(frameStyle);
	this.errorButton = new QPushButton(tr("QErrorMessage::show&M&essage()"));
	this.useNative = new QCheckBox(this);
	this.useNative.text = tr("Use native file dialog");
	this.useNative.checked = true;
	var layout = new QGridLayout;
	layout.setColumnStretch(1, 1);
	layout.setColumnMinimumWidth(1, 250);
	layout.addWidget(this.integerButton, 0, 0);
	layout.addWidget(this.integerLabel, 0, 1);
	layout.addWidget(this.doubleButton, 1, 0);
	layout.addWidget(this.doubleLabel, 1, 1);
	layout.addWidget(this.itemButton, 2, 0);
	layout.addWidget(this.itemLabel, 2, 1);
	layout.addWidget(this.textButton, 3, 0);
	layout.addWidget(this.textLabel, 3, 1);
	layout.addWidget(this.colorButton, 4, 0);
	layout.addWidget(this.colorLabel, 4, 1);
	layout.addWidget(this.fontButton, 5, 0);
	layout.addWidget(this.fontLabel, 5, 1);
	layout.addWidget(this.directoryButton, 6, 0);
	layout.addWidget(this.directoryLabel, 6, 1);
	layout.addWidget(this.openFileNameButton, 7, 0);
	layout.addWidget(this.openFileNameLabel, 7, 1);
	layout.addWidget(this.openFileNamesButton, 8, 0);
	layout.addWidget(this.openFileNamesLabel, 8, 1);
	layout.addWidget(this.saveFileNameButton, 9, 0);
	layout.addWidget(this.saveFileNameLabel, 9, 1);
	layout.addWidget(this.criticalButton, 10, 0);
	layout.addWidget(this.criticalLabel, 10, 1);
	layout.addWidget(this.informationButton, 11, 0);
	layout.addWidget(this.informationLabel, 11, 1);
	layout.addWidget(this.questionButton, 12, 0);
	layout.addWidget(this.questionLabel, 12, 1);
	layout.addWidget(this.warningButton, 13, 0);
	layout.addWidget(this.warningLabel, 13, 1);
	layout.addWidget(this.errorButton, 14, 0);
	layout.addWidget(this.errorLabel, 14, 1);
	layout.addWidget(this.useNative, 15, 0);
	this.setLayout(layout);
	this.windowTitle = tr("Standard Dialogs");
}

Dialog.MESSAGE = tr("<p>Message boxes have a caption, a text, "
	       + "and any number of buttons, each with standard or custom texts."
	       + "<p>Click a button to close the message box. Pressing the Esc button "
	       + "will activate the detected escape button (if any).");

Dialog.prototype = new QDialog();

Dialog.prototype.setInteger = function()
{
	//var i = QInputDialog.getInteger(this, tr("QInputDialog::getInteger()"), tr("Percentage:"), 25, 0, 100, 1, Qt.WindowFlags(0));
	var i = QInputDialog.getDouble(this, tr("QInputDialog::getInteger()"), tr("Percentage:"), 25, 0, 100, 0, Qt.WindowFlags(0));
	if (i != null)
		this.integerLabel.text = i;
}

Dialog.prototype.setDouble = function()
{
	var d = QInputDialog.getDouble(this, tr("QInputDialog::getDouble()"), tr("Amount:"), 37.56, -10000, 10000, 2, Qt.WindowFlags(0));
	if (d != null)
		this.doubleLabel.text = d;
}

Dialog.prototype.setItem = function()
{
	var items = Array[tr("Spring"), tr("Summer"), tr("Fall"), tr("Winter")];
	var item = QInputDialog.getItem(this, tr("QInputDialog::getItem()"), tr("Season:"), items, 0, false, Qt.WindowFlags(0));
	if (item != null & item.length != 0)
		this.itemLabel.text = item;
}

Dialog.prototype.setText = function()
{
	var text = QInputDialog.getText(this, tr("QInputDialog::getText()"), tr("User name:"), QLineEdit.Normal, QDir.home().dirName(), Qt.WindowFlags(0));
	if (text != null && text.length != 0)
		this.textLabel.text = text;
}

Dialog.prototype.setColor = function()
{
	var color = QColorDialog.getColor(new QColor(Qt.green), this);
	if (color.isValid()) 
	{
		this.colorLabel.text = color.name();
		this.colorLabel.palette = new QPalette(color);
		this.colorLabel.autoFillBackground = true;
	}
}

Dialog.prototype.setFont = function()
{
	var font = QFontDialog.getFont(new QFont(this.fontLabel.text), this);
	if (font != null) 
	{
		this.fontLabel.text = font.key();
		this.fontLabel.font = font;
	}
}

Dialog.prototype.setExistingDirectory = function()
{
	var options = QFileDialog.DontResolveSymlinks | QFileDialog.ShowDirsOnly;
	if (!this.useNative.checked)
		options |= QFileDialog.DontUseNativeDialog;
	var directory = QFileDialog.getExistingDirectory(this, tr("QFileDialog::getExistingDirectory()"), this.directoryLabel.text, QFileDialog.Options(options));
	if (directory.length != 0)
		this.directoryLabel.text = directory;
}

Dialog.prototype.setOpenFileName = function()
{
	var options = 0;
	if (!this.useNative.checked)
		options |= QFileDialog.DontUseNativeDialog;
	var selectedFilter;
	var fileName = QFileDialog.getOpenFileName(this,
				tr("QFileDialog::getOpenFileName()"),
				this.openFileNameLabel.text,
				tr("All Files (*);;Text Files (*.txt)"),
				selectedFilter,
				QFileDialog.Options(options));
	if (fileName.length != 0)
		this.openFileNameLabel.text = fileName;
}

Dialog.prototype.setOpenFileNames = function()
{
	var options = 0;
	if (!this.useNative.checked)
		options |= QFileDialog.DontUseNativeDialog;
	var selectedFilter;
	var files = QFileDialog.getOpenFileNames(
				this, tr("QFileDialog::getOpenFileNames()"),
				this.openFilesPath,
				tr("All Files (*);;Text Files (*.txt)"),
				selectedFilter,
				options);
	if (files.length != 0) 
	{
		this.openFilesPath = files[0];
		this.openFileNamesLabel.text = "[" + files.join(", ") + "]";
	}
}

Dialog.prototype.setSaveFileName = function()
{
	var options = 0;
	if (!this.useNative.checked)
		options |= QFileDialog.DontUseNativeDialog;
	var selectedFilter;
	var fileName = QFileDialog.getSaveFileName(this,
				tr("QFileDialog::getSaveFileName()"),
				this.saveFileNameLabel.text,
				tr("All Files (*);;Text Files (*.txt)"),
				selectedFilter,
				options);
	if (fileName.length != 0)
		this.saveFileNameLabel.text = fileName;
}

Dialog.prototype.criticalMessage = function()
{
	var reply = QMessageBox.critical(this, tr("QMessageBox::critical()"),
	Dialog.MESSAGE, QMessageBox.StandardButtons(QMessageBox.Abort, QMessageBox.Retry, QMessageBox.Ignore));
	if (reply == QMessageBox.Abort)
		this.criticalLabel.text = tr("Abort");
	else if (reply == QMessageBox.Retry)
		this.criticalLabel.text = tr("Retry");
	else
		this.criticalLabel.text = tr("Ignore");
}

Dialog.prototype.informationMessage = function()
{
	var reply = QMessageBox.information(this, tr("QMessageBox::information()"), Dialog.MESSAGE);
	if (reply == QMessageBox.Ok)
		this.informationLabel.text = tr("OK");
	else
		this.informationLabel.text = tr("Escape");
}

Dialog.prototype.questionMessage = function()
{
	var reply = QMessageBox.question(this, tr("QMessageBox::question()"),
	Dialog.MESSAGE, QMessageBox.StandardButtons(QMessageBox.Yes, QMessageBox.No, QMessageBox.Cancel));
	if (reply == QMessageBox.Yes)
		this.questionLabel.text = tr("Yes");
	else if (reply == QMessageBox.No)
		this.questionLabel.text = tr("No");
	else
		this.questionLabel.text = tr("Cancel");
}

Dialog.prototype.warningMessage = function()
{
	var msgBox = new QMessageBox(QMessageBox.Warning, tr("QMessageBox::warning()"), Dialog.MESSAGE, 0, this);
	msgBox.addButton(tr("Save &Again"), QMessageBox.AcceptRole);
	msgBox.addButton(tr("&Continue"), QMessageBox.RejectRole);
	if (msgBox.exec() == QMessageBox.AcceptRole)
		this.warningLabel.text = tr("Save Again");
	else
		this.warningLabel.text = tr("Continue");
}

Dialog.prototype.errorMessage = function()
{
	this.errorMessageDialog.showMessage(
	    tr("This dialog shows and remembers error messages. "
	       + "If the checkbox is checked (as it is by default), "
	       + "the shown message will be shown again, "
	       + "but if the user unchecks the box the message "
	       + "will not appear again if QErrorMessage::showMessage() "
	       + "is called with the same message."));
	this.errorLabel.text = tr("If the box is unchecked, the message "
			   + "won't appear again.");
}

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
	ScriptCleanupFunction();
}

//This function makes sure that everything gets nicely cleaned before the script ends
function ScriptCleanupFunction()//Cleanup
{
	try
	{
	dialog.integerButton["clicked()"].disconnect(dialog, dialog.setInteger);
	dialog.doubleButton["clicked()"].disconnect(dialog, dialog.setDouble);
	dialog.itemButton["clicked()"].disconnect(dialog, dialog.setItem);
	dialog.textButton["clicked()"].disconnect(dialog, dialog.setText);
	dialog.colorButton["clicked()"].disconnect(dialog, dialog.setColor);
	dialog.fontButton["clicked()"].disconnect(dialog, dialog.setFont);
	dialog.directoryButton["clicked()"].disconnect(dialog, dialog.setExistingDirectory);
	dialog.openFileNameButton["clicked()"].disconnect(dialog, dialog.setOpenFileName);
	dialog.openFileNamesButton["clicked()"].disconnect(dialog, dialog.setOpenFileNames);
	dialog.saveFileNameButton["clicked()"].disconnect(dialog, dialog.setSaveFileName);
	dialog.criticalButton["clicked()"].disconnect(dialog, dialog.criticalMessage);
	dialog.informationButton["clicked()"].disconnect(dialog, dialog.informationMessage);
	dialog.questionButton["clicked()"].disconnect(dialog, dialog.questionMessage);
	dialog.warningButton["clicked()"].disconnect(dialog, dialog.warningMessage);
	dialog.errorButton["clicked()"].disconnect(dialog, dialog.errorMessage);
	}	
	catch (e) 
	{
		Log(".*. Something went wrong disconnecting Signal/Slots:" + e); //If a connection fails warn the user!
	}
	
	Dialog.prototype.keyPressEvent = null;
	Dialog.prototype.setInteger = null;
	Dialog.prototype.setDouble = null;
	Dialog.prototype.setItem = null;
	Dialog.prototype.setText = null;
	Dialog.prototype.setColor = null;
	Dialog.prototype.setFont = null;
	Dialog.prototype.setExistingDirectory = null;
	Dialog.prototype.setOpenFileName = null;
	Dialog.prototype.setOpenFileNames = null;
	Dialog.prototype.setSaveFileName = null;
	Dialog.prototype.criticalMessage = null;
	Dialog.prototype.informationMessage = null;
	Dialog.prototype.questionMessage = null;
	Dialog.prototype.warningMessage = null;
	Dialog.prototype.errorMessage = null;
	Dialog.prototype.closeEvent = null;
	Dialog.prototype = null;	
	Dialog = null;
	translator = null;
	dialog = null;	
	tr = null;
	ScriptCleanupFunction = null;	
	//Write something to the Log Output Pane so we know that this Function executed successfully.
	Log("Finished script Cleanup!");
	StimulGL.cleanupScript();
}

var translatorFileName = "qt_";
translatorFileName += QLocale.system().name();
var translator = new QTranslator(App);
if (translator.load(translatorFileName, QLibraryInfo.location(QLibraryInfo.TranslationsPath)))
	qApp.installTranslator(translator);
var dialog = new Dialog();
try
{
	dialog.integerButton["clicked()"].connect(dialog, dialog.setInteger);
	dialog.doubleButton["clicked()"].connect(dialog, dialog.setDouble);
	dialog.itemButton["clicked()"].connect(dialog, dialog.setItem);
	dialog.textButton["clicked()"].connect(dialog, dialog.setText);
	dialog.colorButton["clicked()"].connect(dialog, dialog.setColor);
	dialog.fontButton["clicked()"].connect(dialog, dialog.setFont);
	dialog.directoryButton["clicked()"].connect(dialog, dialog.setExistingDirectory);
	dialog.openFileNameButton["clicked()"].connect(dialog, dialog.setOpenFileName);
	dialog.openFileNamesButton["clicked()"].connect(dialog, dialog.setOpenFileNames);
	dialog.saveFileNameButton["clicked()"].connect(dialog, dialog.setSaveFileName);
	dialog.criticalButton["clicked()"].connect(dialog, dialog.criticalMessage);
	dialog.informationButton["clicked()"].connect(dialog, dialog.informationMessage);
	dialog.questionButton["clicked()"].connect(dialog, dialog.questionMessage);
	dialog.warningButton["clicked()"].connect(dialog, dialog.warningMessage);
	dialog.errorButton["clicked()"].connect(dialog, dialog.errorMessage);
}	
catch (e) 
{
	Log(".*. Something went wrong connecting Signal/Slots:" + e); //If a connection fails warn the user!
}

dialog.show();

