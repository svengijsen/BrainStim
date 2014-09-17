#include <QtGui>
#include <stdlib.h>
#include <math.h>
#include "ThreadedGLWidgetWrapper.h"

ThreadedGLWidgetWrapper::ThreadedGLWidgetWrapper(QWidget *parent) : QGLWidget(parent), glt(this)
{
	init();
	//stimContainerDlg = NULL;
	bForceToStop = false;
	bDebugMode = false;
	#ifdef Q_WS_MACX
		nMinScreenUpdateTime = 1; // make param in interface and recommend per platform
	#else
		nMinScreenUpdateTime = 1; // on Win (with recent OGL drivers), "swapBuffers" will wait for n retraces as indicated by "setSwapInterval" command (works like DX "flip")
	#endif
	setAutoFillBackground(false);
	setAutoBufferSwap(false); // in order to have control over time point for buffer swap
	//dDesktopWidget = QApplication::desktop();
	rScreenResolution = QApplication::desktop()->screenGeometry();//dDesktopWidget->screenGeometry();//availableGeometry();
	setFixedSize(rScreenResolution.width(), rScreenResolution.height());
	//setMinimumSize(200, 200);
	setWindowTitle(tr("Painting a Scene"));
	//resize(320, 240);

	//stimContainerDlg = new TreadedContainerDlg();//parent parameter?
	//stimContainerDlg->setAttribute(Qt::WA_DeleteOnClose);
	//retinoMapWdg = new RetinoMap_glwidget(stimContainerDlg);
	//retinoMapWdg->setObjectName("RetinoMap_RenderWidgetGL");
	//stimContainerDlg->installEventFilter(this);//re-route all stimContainerDlg events to this->bool Retinotopic_Mapping::eventFilter(QObject *target, QEvent *event)
	parent->installEventFilter(this);
	mainLayout = NULL;
}

ThreadedGLWidgetWrapper::~ThreadedGLWidgetWrapper()
{
	//if (stimContainerDlg)
	//{
	//	stimContainerDlg->close();
	//	//if(bDestruct)
	//	//{
	//		delete stimContainerDlg;
	//		stimContainerDlg = NULL;
	//	//}
	//}
	if (mainLayout)
	{
		delete mainLayout;
		mainLayout = NULL;
	}
}

void ThreadedGLWidgetWrapper::startRendering()
{
	glt.start();
}

void ThreadedGLWidgetWrapper::stopRendering()
{
	glt.stop();
	glt.wait();
}

void ThreadedGLWidgetWrapper::resizeEvent(QResizeEvent *evt)
{
	glt.resizeViewport(evt->size());
}

void ThreadedGLWidgetWrapper::paintEvent(QPaintEvent *)
{
	// Handled by the GLThread.
}

//void ThreadedGLWidgetWrapper::paintEvent(QPaintEvent *event)
//{
//	nFrameCounter += 1;	
//}

void ThreadedGLWidgetWrapper::closeEvent(QCloseEvent *evt)
{
	stopRendering();
	QGLWidget::closeEvent(evt);
}

bool ThreadedGLWidgetWrapper::eventFilter(QObject *target, QEvent *event)
{
	//if (target == stimContainerDlg) 
	//{
		if (event->type() == QEvent::KeyPress) 
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
			switch (keyEvent->key())
			{
			case Qt::Key_Escape:
				if((keyEvent->modifiers() & Qt::ControlModifier) && (keyEvent->modifiers() & Qt::AltModifier))
				{
					emit UserWantsToClose();
					//abortExperiment();
				}
				break;
			}

			//if(keyEvent->text() == ui.USBCharEdit->text()) // for fMRI triggering via USB port (simulated keyboard)
			//{
			//	updateTriggerCount();
			//	return true;
			//}
		}
	//}
	//return QDialog::eventFilter(target, event);
	return true;
}

void ThreadedGLWidgetWrapper::SetupLayout(QWidget* layoutWidget)
{
	mainLayout = new QVBoxLayout;
	mainLayout->setAlignment(Qt::AlignCenter);
	mainLayout->setMargin(0);
	mainLayout->addWidget(layoutWidget);
	//stimContainerDlg->setLayout(mainLayout);
	//if(m_RunFullScreen)
		//stimContainerDlg->showFullScreen();
	//else
	//	stimContainerDlg->show();
}

void ThreadedGLWidgetWrapper::setDebugMode(bool bMode)
{
	bDebugMode = bMode;
}

void ThreadedGLWidgetWrapper::init()
{
	currentBlockTrial = -1;
	completedTR = 0;
	m_TriggerCount = 0;
	nFrameCounter = 0;
	nElapsedFrameTime = 0;
	nPaintUpdateTime = 0;
	nextTimeThresholdTRs = 0;
}

bool ThreadedGLWidgetWrapper::start()
{
	init();
	totalRunningTime.start();
	//renderWdg->trialTime.start();
	//renderWdg->frameTime.start();	
	checkForNextBlockTrial();
	startTriggerTimer(2000);
	return true;
}

bool ThreadedGLWidgetWrapper::stop()
{
	stopTriggerTimer();
	bForceToStop = true;
	return true;
}

void ThreadedGLWidgetWrapper::startTriggerTimer(int msTime)
{
	tTriggerTimer.setSingleShot(false);
	connect(&tTriggerTimer, SIGNAL(timeout()), this, SLOT(incrementTriggerCount()));
	tTriggerTimer.start(msTime);
}

void ThreadedGLWidgetWrapper::stopTriggerTimer()
{
	tTriggerTimer.stop();
	disconnect(&tTriggerTimer, SIGNAL(timeout()), this, SLOT(incrementTriggerCount()));
}

void ThreadedGLWidgetWrapper::incrementTriggerCount()
{
	m_TriggerCount++;
}

void ThreadedGLWidgetWrapper::initBlockTrial()
{
	bForceToStop = false;
	nFrameCounter = 0;
#ifdef Q_WS_MACX
	nMinScreenUpdateTime = 1; // make param in interface and recommend per platform
#else
	nMinScreenUpdateTime = 1; // on Win (with recent OGL drivers), "swapBuffers" will wait for n retraces as indicated by "setSwapInterval" command (works like DX "flip")
#endif
}

bool ThreadedGLWidgetWrapper::loadBlockTrial()
{

	return true;
}

bool ThreadedGLWidgetWrapper::checkForNextBlockTrial()
{
	if (bForceToStop)
		return false;
	int total_elapsed_time = totalRunningTime.elapsed();//The Total time past since the experiment started
	//if(m_TriggerMode == 0)
	//{
	//	completedTR = total_elapsed_time / m_TrTime;//The time past in number of Tr's
	//}
	//else
	//{
		completedTR = m_TriggerCount;
	//}
	bool goToNextBlockTrial = false;
	bool bFirstBlock = false;
	if(currentBlockTrial == -1) //First Block Trial? (Start/Init)
	{
		goToNextBlockTrial = true;
		bFirstBlock = true;
	}
	else
	{
		goToNextBlockTrial = completedTR > nextTimeThresholdTRs;//Go to next Block Trial when the total completed Tr's is larger than the next Threshold Tr.
	}
	if(goToNextBlockTrial)//When we init/start or switch from a Block Trial 
	{
		//qDebug() << "ThreadedGLWidgetWrapper::goToNextBlockTrial(current=" << currentBlockTrial << ", total_elapsed_time=" << total_elapsed_time << ")";
		currentBlockTrial++;//Increment the Block Trial Counter
		if(currentBlockTrial >= 100)//---->>//BlockTrialFiles.size())//No more Block Trials left?
		{
			return false;
		}
		nextTimeThresholdTRs += 2;//----->>> BlockTrialsDurationInTRs[currentBlockTrial];//increment the nextTimeThresholdTRs
		loadBlockTrial();
		//loadBlockTrialFile(BlockTrialFiles[currentBlockTrial], true);
		nFrameCounter = 0;
		initBlockTrial();
		tFrameTime.restart();
		if (bFirstBlock)
		{
			emit StateHasChanged(ExperimentObject_Started);
		}

		tStimTimer.singleShot(10, this, SLOT(animate()));
		return true;
	}
	return false;
}

void ThreadedGLWidgetWrapper::animate()
{
	nElapsedFrameTime = tFrameTime.elapsed();//Calculate the elapsed time since started
	tFrameTime.restart();
	repaint();
	//update();
	//	elapsed = frameTime.elapsed();//Calculate the elapsed time since started
	//	frameTime.restart();
	//	repaint();//calls the below void GLWidget::paintEvent(QPaintEvent *event)
	//You should usually use 'update', as this will allow multiple queued paint events to be 'collapsed' into a single event. 
	//The update method will call updateGL for QGLWidgets. The 'repaint' method should be used if you want an immediate repaint.
	//If you have hooked up a timer to periodically call 'update', then failure to repaint regularly usually indicates 
	//that you're putting stress on the CPU.
	//}
}

void ThreadedGLWidgetWrapper::finalizePaintEvent()
{
	while(1)//Don't go too fast...
	{
		nElapsedFrameTime = tFrameTime.elapsed();
		if(nElapsedFrameTime > nMinScreenUpdateTime)
			break;
	}
	swapBuffers();
	if( !checkForNextBlockTrial() ) //Check whether we need to prepare for an new block Trial, otherwise directly call onAnimate()
	{
		nPaintUpdateTime = tFrameTime.elapsed();
		if(bForceToStop)
		{
			//this->setUpdatesEnabled(false);
			emit StateHasChanged(ExperimentObject_Stopped);
			return;
		}
		// if next trial/block reached, the function will prepare for it and calls "onAnimate()" when ready, otherwise we compute next frame
		QTimer::singleShot(10, this, SLOT(animate()));
	}
}