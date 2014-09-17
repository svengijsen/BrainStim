#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QDialog>
#include <QGLWidget>
#include <QTimer>
#include <QTime>
#include <QDesktopWidget>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QBoxLayout>
#include "Global.h"

#include "glthread.h"
class GLThread;

//class QPaintEvent;
//class QWidget;
//class TreadedContainerDlg;

class ThreadedGLWidgetWrapper : public QGLWidget
{
	Q_OBJECT

signals:
	void UserWantsToClose(void);
	void StateHasChanged(ExperimentObjectState);

public:
	ThreadedGLWidgetWrapper(QWidget *parent = NULL);
	~ThreadedGLWidgetWrapper();

	void setBlockTrials();
	void startRendering();    
	void stopRendering();

public slots:
	//Can be overridden
	virtual bool start();
	virtual bool stop();
	void setDebugMode(bool bMode);

protected:
	bool checkForNextBlockTrial();
	void finalizePaintEvent();
	void SetupLayout(QWidget* layoutWidget);

//Threaded overridden code
	void resizeEvent(QResizeEvent *evt);
	void paintEvent(QPaintEvent *);
	void closeEvent(QCloseEvent *evt);

	GLThread glt;

	//Can be overridden
	virtual void init();
	virtual void initBlockTrial();
	virtual bool loadBlockTrial();
	//virtual void paintEvent(QPaintEvent *event);

	//Example using standard OpenGL rendering commands...
	//void initializeGL()
	//{
	//	// Set up the rendering context, define display lists etc.:
	//	...
	//		glClearColor(0.0, 0.0, 0.0, 0.0);
	//	glEnable(GL_DEPTH_TEST);
	//	...
	//}

	//void resizeGL(int w, int h)
	//{
	//	// setup viewport, projection etc.:
	//	glViewport(0, 0, (GLint)w, (GLint)h);
	//	...
	//		glFrustum(...);
	//	...
	//}

	//void paintGL()
	//{
	//	// draw the scene:
	//	...
	//		glRotatef(...);
	//	glMaterialfv(...);
	//	glBegin(GL_QUADS);
	//	glVertex3f(...);
	//	glVertex3f(...);
	//	...
	//		glEnd();
	//	...
	//}
	//-->updateGL() function
	//see http://doc.qt.nokia.com/stable/qglwidget.html#details

protected slots:
	void incrementTriggerCount();
	void animate();

private:
	void startTriggerTimer(int msTime);
	void stopTriggerTimer();
	bool eventFilter(QObject *target, QEvent *event);

protected:
	bool bForceToStop;
	bool bDebugMode;
	int nElapsedFrameTime;
	int nPaintUpdateTime;
	QTime tFrameTime;
	QRectF rScreenResolution;
	int nMinScreenUpdateTime;
	int nFrameCounter;
	QTime totalRunningTime;
	//TreadedContainerDlg *stimContainerDlg;
	QVBoxLayout *mainLayout;

private:
	QTimer tTriggerTimer;
	QTimer tStimTimer;
	//QDesktopWidget* dDesktopWidget;
	int currentBlockTrial;
	int completedTR;
	int m_TriggerCount;
	int nextTimeThresholdTRs;
};

class TreadedContainerDlg : public QDialog
{
	Q_OBJECT

public:
	TreadedContainerDlg(QWidget *parent = NULL);
	TreadedContainerDlg::~TreadedContainerDlg();

	private slots:
		void reject();

protected:
	void closeEvent(QCloseEvent *e);
};

#endif // GLWIDGET_H
