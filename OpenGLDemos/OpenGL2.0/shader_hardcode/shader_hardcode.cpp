#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h>
#include <gl/glut.h>

GLuint program;
GLint  attribute_coord2d;

int init_shaders(void)
{
	GLint compile_ok = GL_FALSE;
	GLint link_ok    = GL_FALSE;

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

	// Vertex shader source code
	const char* vs_source =
#ifdef GL_ES_VERSION_2_0
		"#version 100\n"		// OpenGL ES 2.0
#else
		"#version 120\n"		// OpenGL 2.1
#endif
	"attribute vec2 coord2d;"
	"void main(void) {"
	"    gl_Position = vec4(coord2d, 0.0, 1.0);"
	"}";

	// Compile vertex shader
	glShaderSource(vertex_shader, 1, &vs_source, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE)
	{
		fprintf(stderr, "Error in vertex shader\n");
		return 0;
	}

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	// Fragment shader source code.
	const char* fs_source =
#ifdef GL_ES_VERSION_2_0
		"#version 100\n"		// OpenGL ES 2.0
#else
		"#version 120\n"		// OpenGL 2.1
#endif
		"void main(void) {"
		"    gl_FragColor[0] = gl_FragCoord.x / 640;"
		"    gl_FragColor[1] = gl_FragCoord.y / 480;"
		"    gl_FragColor[2] = 0.5;"
		"}";

	// Compile fragment shader
	glShaderSource(fragment_shader, 1, &fs_source, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_ok);
	if (!compile_ok) 
	{
		fprintf(stderr, "Error in fragment shader\n");
		return 0;
	}

	// Link vertex shader and fragment shader
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok)
	{
		fprintf(stderr, "Link program error\n");
		return 0;
	}

	// Bind shader variables
	const char* attribute_name = "coord2d";
	attribute_coord2d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord2d == -1)
	{
		fprintf(stderr, "Cound not bind attribute %s\n", attribute_name);
		return 0;
	}

	return 1;
}

void onDisplay()
{
	glClearColor(1.0, 1.0, 1.0, 1.0); // Background color
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);
	glEnableVertexAttribArray(attribute_coord2d);

	GLfloat vertices[] = 
	{
		 0.0,  0.8,
		-0.8, -0.8,
		 0.8, -0.8,
	};

	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, vertices);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(attribute_coord2d);

	glFlush();
	//glutSwapBuffers();
}

void cleanup()
{
	glDeleteProgram(program);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

	//If use GLUT_DOUBLE instead of GLUT_SINGLE, you MUST use glutSwapBuffers instead of glFlush in onDisplay function.
	//glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Hardcoded shader");

	// glewInit Must be called after glutInit
	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return EXIT_FAILURE;
	}

	if (init_shaders())
	{
		glutDisplayFunc(onDisplay);
		glutMainLoop();
	}

	cleanup();

	return 0;
}
