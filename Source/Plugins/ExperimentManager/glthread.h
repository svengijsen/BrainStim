#ifndef GLTHREAD_H
#define GLTHREAD_H

#include <QThread>
#include <QSize>
#include <QTime>

class ThreadedGLWidgetWrapper;

class GLThread : public QThread
{
public:
	GLThread(ThreadedGLWidgetWrapper *glWidgetWrapper);
	void resizeViewport(const QSize &size);
	void run();
	void stop();

private:
	bool doRendering;
	bool doResize;
	int w;
	int h;
	int rotAngle;
	ThreadedGLWidgetWrapper *glw;
};

#endif // GLTHREAD_H
