

#include <stdio.h>
#include <GL/glew.h>

// #include <OpenGL/gl.h>
// #include <OpenGL/glu.h>
#include <GLUT/glut.h>
// #include <GL/freeglut.h>
// #include "math_3d.h"


static void Render(void) {
	GLuint VBO;

	GLfloat Vertices[] = {
	    -0.5f, -0.5f, 0.0f,
	     0.5f, -0.5f, 0.0f,
	     0.0f,  0.5f, 0.0f
	}; 
	
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0]*9), &Vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	
	glDisableVertexAttribArray(0);
	 
	glutSwapBuffers();
}

GLuint dVBO = 0;

void DotRender() {
	glClear(GL_COLOR_BUFFER_BIT);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, dVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_POINTS, 0, 1);
	
	glDisableVertexAttribArray(0);
	 
	glutSwapBuffers();
}

void CreateDot() {
	GLfloat Vertices[] = {
	    -0.5f, -0.5f, 0.0f
	}; 
	
	glGenBuffers(1, &dVBO);
	glBindBuffer(GL_ARRAY_BUFFER, dVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowSize(1024, 768); 
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Tutorial 03");

	// glutDisplayFunc(Render);
	glutDisplayFunc(DotRender);

	glewExperimental = GL_TRUE;
	GLenum res = glewInit();
	if (res != GLEW_OK) {
	    fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
	    return 1;
	}

	printf("GL version: %s\n", glGetString(GL_VERSION));

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	CreateDot();
	
	glutMainLoop();

	return 0;
}