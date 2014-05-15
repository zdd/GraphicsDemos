#include <windows.h>
#include <stdio.h>
#include <gl/glew.h>
#include <gl/glut.h>

GLuint vertex_buffer;

struct vertex
{
	GLfloat x, y, z; // position
};

void init_buffers()
{
	// Vertex buffer data
	GLfloat vertices[] =
	{
		 0.0,  1.0, 0.0,
		-1.0, -1.0, 0.0,
		 1.0, -1.0, 0.0
	};

	// If you got an access violation here, make sure you have called glew first(after the call of glutInit)
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void display()
{
	glClearColor(1.0, 1.0, 1.0, 1.0); // Background color
	glClear(GL_COLOR_BUFFER_BIT);

	// vertex color
	glColor3f(1.0, 0.0, 0.0);

	glVertexPointer(3, GL_FLOAT, 0, 0);

	glEnableClientState(GL_VERTEX_ARRAY);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glFlush();
}

void cleanup()
{
	glDeleteBuffers(1, &vertex_buffer);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutCreateWindow("Triangle");

	// glewInit Must be called after glutInit
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		printf("%S", "Initialize glew failed");
	}

	init_buffers();
	glutDisplayFunc(display);
	glutMainLoop();
	cleanup();

	return 0;
}