#include "GLThread.h"
#include "ThreadedGLWidgetWrapper.h"
#include <QtOpenGL>

GLThread::GLThread(ThreadedGLWidgetWrapper *glWidgetWrapper) : QThread(), glw(glWidgetWrapper)
{
	doRendering = true;
	doResize = false;
}

void GLThread::stop()
{
	doRendering = false;
}

void GLThread::resizeViewport(const QSize &size)
{
	w = size.width();
	h = size.height();
	doResize = true;
}    

void GLThread::run()
{
	int XSize = 640;
	int YSize = 480;

	srand(QTime::currentTime().msec());
	rotAngle = rand() % 360;//Get a random rotation angle..
	
	glw->makeCurrent();//Make sure to call this function first!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glTranslatef(0.375, 0.375, 0);//Displacement trick for exact pixelization
	glOrtho(0, XSize, YSize, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	//To achieve exact pixelization (pixel-precise drawing), it's important to understand that coordinates in OpenGL are technically not tied to 
	//the actual screen pixels, so even if you set up 2D projection as explained above, you still specify the locations with higher than pixel granularity. 
	//That is, (0,0) corresponds to the top-left corner inside the top-left pixel, while (0.5, 0.5) corresponds to the center of that pixel. Forgetting about 
	//this can easily lead to quite unpredictable misplacements of primitives and to unwanted anti-aliasing blur. For example, if you want to lit pixels using 
	//GL_POINTS, you should use coordinates that end with a half (like 10.5 instead of 10), as in principle you want to put that point at the center of the pixel, 
	//not at the corner of it, as in the last case the point will overlap with the neighbor pixels (because an OpenGL "point" of default size 1 is in fact a 1x1 
	//square), hence possibly painting some of those screen pixels. But if for example you draw lines (GL_LINES) or polygons (GL_QUADS, etc.), you want to put 
	//the vertices at the edges of the pixels, so you will often use integer coordinates. If you are not using anti-aliasing, a popular trick to avoid all this 
	//mess is to apply glTranslatef (0.375, 0.375, 0) on the GL_MODELVIEW matrix, and then use integer coordinates everywhere; this displacement will keep the 
	//edges of the primitives away from the places where OpenGL vacillates about which screen pixels to paint.

	glDisable(GL_DEPTH_TEST);//Only 2D painting!
	glClearColor(0.5, 0.2, 0.4, 0);//Set the background color (RGB)
    glClear(GL_COLOR_BUFFER_BIT);//Clear the screen
	glViewport(0, 0, XSize, YSize);
	//glShadeModel(GL_SMOOTH);//Smooth the edges
	//glShadeModel(GL_FLAT);
	
	while (doRendering) {
		if (doResize) {
			glViewport(0, 0, w, h);
			doResize = false;
		}


		// Rendering code goes here
		glClear(GL_COLOR_BUFFER_BIT);

		//Draw a rectangle
		glBegin(GL_QUADS);
		glColor3f(1,0,0);//Red
		glVertex2f(0, 100);
		glVertex2f(0, 100); 
		glVertex2f(0, 100); 
		glVertex2f(0, 100);
		glEnd();

		//Draw a circle
		glBegin(GL_LINE_LOOP);
		glVertex2f(200, 300);
		glVertex2f(200, 300);
		glVertex2f(200, 300);
		glVertex2f(200, 300);
		glEnd();

		//Draw a triangle
		glBegin (GL_TRIANGLES);
		glColor3f (1.0, 0.0, 0.0);
		glVertex2f (5.0, 5.0);
		glColor3f (0.0, 1.0, 0.0);
		glVertex2f (25.0, 5.0);
		glColor3f (0.0, 0.0, 1.0);
		glVertex2f (5.0, 25.0);
		glEnd();

		glw->swapBuffers();
		msleep(40);
	}
}
