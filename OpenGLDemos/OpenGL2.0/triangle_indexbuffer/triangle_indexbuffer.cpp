#include <windows.h>
#include <stdio.h>
#include <gl/glew.h>
#include <gl/glut.h>

GLuint vertex_buffer;	// Vertex buffer id
GLuint index_buffer;	// Index buffer id

struct vertex
{
	GLfloat x, y, z; // position
};

// Initialize vertex buffer and index buffer
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
	// Generate vertex buffer
	glGenBuffers(1, &vertex_buffer);
	// Bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	// Set vertex buffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Index buffer data
	GLubyte indices[] =
	{
		0, 1, 2
	};

	// Generate index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void display()
{
	glClearColor(1.0, 1.0, 1.0, 1.0); // Background color
	glClear(GL_COLOR_BUFFER_BIT);

	// Vertex color
	glColor3f(1.0, 0.0, 0.0);
	
	// Vertex format
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE , 0);

	glFlush();
}

void cleanup()
{
	// Disable client state
	glDisableClientState(GL_VERTEX_ARRAY);

	// Delete vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vertex_buffer);

	// Delete index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &index_buffer);
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