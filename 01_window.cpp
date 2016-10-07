#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

// main display function
void display (void) {
	// swap frontbuffer with backbuffer
	glClear(GL_COLOR_BUFFER_BIT); 
	glutSwapBuffers();
}

int main (int argc, char** argv) {
	// initialize GLUT
	glutInit(&argc, argv);

	// enable double buffers for displaying and color format RGBA
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	// create window with size
	glutInitWindowSize(1024, 768); 
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Tutorial 01");

	// set render function
	glutDisplayFunc(display);

	// GL draw clear color with rgba values
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//
	glutMainLoop();

	//
	return 0;
}