var dialog_object;
var openFileNameButton;
var previousButton;
var nextButton;
var imageLabel;
var currentFrameLabel;
var nCurrentImageIndex = 0;

var QGraphicsScene_Object = new QGraphicsScene();
var QGraphicsView_Object = new QGraphicsView(QGraphicsScene_Object);
var QGraphicsPixmapItem_Object = new QGraphicsPixmapItem();
var QPixmap_Object = new QPixmap();
var QImage_Object = new QImage();
var ImageProcessor_Object = new ImageProcessor();

function CleanupScript()
{
	openFileNameButton.clicked.disconnect(dialog_object,dialog_object.setOpenFileName);
	previousButton.clicked.disconnect(dialog_object,dialog_object.previousImage);
	nextButton.clicked.disconnect(dialog_object,dialog_object.nextImage);
	
	QImage_Object = null;
	QPixmap_Object = null;
	QGraphicsScene_Object = null;
	QGraphicsView_Object = null;
	QGraphicsPixmapItem_Object = null;
	ImageProcessor_Object = null;
	openFileNameButton = null;
	previousButton = null;
	nextButton = null;
	imageLabel = null;
	currentFrameLabel = null;
	Dialog.prototype.setOpenFileName = null;
	Dialog.prototype.showImage = null;
	Dialog.prototype.previousImage = null;
	Dialog.prototype.nextImage = null;
	Dialog.prototype.EnableImageBrowsing = null;
	dialog_object = null;
	Dialog = null;
	CleanupScript = null;
	StimulGL.cleanupScript();
	Log("CleanupScript() exited successfully")
}

function Dialog(parent)
{
	QDialog.call(this, parent);
	var frameStyle = QFrame.Sunken | QFrame.Panel;
	this.openFileNameLabel = new QLabel;
	this.openFileNameLabel.setFrameStyle(frameStyle);
	openFileNameButton = new QPushButton("Select *.(C)DAT File");
	previousButton = new QPushButton("Previous <<");
	nextButton = new QPushButton(">> Next");
	currentFrameLabel = new QLabel("*");
	this.EnableImageBrowsing(0x0);
	//openFileNameButton["clicked()"].connect(this, this.setOpenFileName);
	this.useNative = new QCheckBox(this);
	this.useNative.text = "Use native file dialog";
	this.useNative.checked = true;
	imageLabel = new QLabel;
	var layout = new QGridLayout;
	layout.setColumnStretch(1, 4);
	layout.setColumnMinimumWidth(2, 250);
	
	layout.addWidget(imageLabel,0,1);//QGraphicsView_Object,0,1);
	layout.addWidget(previousButton, 1, 0);
	layout.addWidget(currentFrameLabel, 1, 1);
	layout.addWidget(nextButton, 1, 2);
	layout.addWidget(openFileNameButton, 2, 0);
	layout.addWidget(this.openFileNameLabel, 2, 1);
	layout.addWidget(this.useNative, 3, 0);
	this.setLayout(layout);
	this.windowTitle = "(C)DAT File Viewer";
}

Dialog.prototype = new QDialog();
Dialog.prototype.EnableImageBrowsing = function(enableMask)
{
	//1: prev, 2: next, 
	
	previousButton.setEnabled(enableMask & 0x1);
	nextButton.setEnabled(enableMask & 0x2);
}

Dialog.prototype.setOpenFileName = function()
{
	var options = 0;
	if (!this.useNative.checked)
		options |= QFileDialog.DontUseNativeDialog;
	var selectedFilter;
	var fileName = QFileDialog.getOpenFileName(this, "Select *.(C)DAT File", this.openFileNameLabel.text, "All Files (*);;(C)DAT Files (*.cdat *.dat)", selectedFilter, QFileDialog.Options(options));
	if (fileName.length == 0)
	{
		Log("No valid file choosen...");
		return;		
	}
	this.openFileNameLabel.text = fileName;
	nCurrentImageIndex = 0;
	this.showImage(fileName,nCurrentImageIndex);
}

Dialog.prototype.showImage = function(imgPath,nIndex)
{
	Log("showImage");
	//QGraphicsPixmapItem_Object =
	//QImage_Object = new QImage(0,0);
	var nNrOfSubFrames = ImageProcessor_Object.getCDATFileNrOfSubFrames(imgPath);
	if((nIndex >= 0) && (nNrOfSubFrames > 0) && (nNrOfSubFrames>nIndex))
	{
		QImage_Object = ImageProcessor_Object.getImageFromCDATFile(imgPath,nIndex);
		Log("\tWidth: " + QImage_Object.width() + ", Height: " + QImage_Object.height());
		if(QImage_Object.width() > 0)
		{
			imageLabel.setPixmap(QPixmap.fromImage(QImage_Object));
			//Put the dialog in the screen center
			var screenRect = new QRect();
			screenRect = QApplication.desktop().screenGeometry();
			this.move(0,0);//screenRect.width(),screenRect.x());//0,0);//screen.center() - this->rect().center() );
			//this.setGeometry(10,10,this.width(),this.height());
	//		QGraphicsScene_Object.addItem(QGraphicsPixmapItem_Object);
	//		QGraphicsView_Object.show();
			currentFrameLabel.setText("Current frame is index " + nIndex + " (Total " + nNrOfSubFrames + " item(s))");
			this.EnableImageBrowsing(0x3);
			return true;
		}
	}
	return false;
	//this.EnableImageBrowsing(0x0);	
}

Dialog.prototype.previousImage = function()
{	
	Log("previousImage");
	if(this.showImage(this.openFileNameLabel.text,nCurrentImageIndex-1))
		nCurrentImageIndex--;
}

Dialog.prototype.nextImage = function()
{
	Log("nextImage");
	if(this.showImage(this.openFileNameLabel.text,nCurrentImageIndex+1))
		nCurrentImageIndex++;
}

//QGraphicsPixmapItem_Object	QPixmap::fromImage(image));
//QGraphicsScene_Object->addItem(QGraphicsPixmapItem_Object);
//QGraphicsScene_Object->show();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

dialog_object = new Dialog();

openFileNameButton.clicked.connect(dialog_object,dialog_object.setOpenFileName);
previousButton.clicked.connect(dialog_object,dialog_object.previousImage);
nextButton.clicked.connect(dialog_object,dialog_object.nextImage);

dialog_object.exec();
CleanupScript();
