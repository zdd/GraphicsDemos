#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gl/glew.h>
#include <gl/glut.h>

GLuint program;
GLuint vbo_triangle;			// vertex buffer
GLint  attribute_coord2d;		// vertex coordinates
GLint  attribute_v_color;		// vertex color
GLint  uniform_fade;			// color fade

// Read shader source from file
char* file_read(const char* filename)
{
	if ((_access(filename, 0)) == -1)
	{
		fprintf(stderr, "File %s not exists!\n", filename);
		return NULL;
	}

	FILE* input = NULL;
	fopen_s(&input, filename, "rb");
	if (input == NULL)
		return NULL;

	if (fseek(input, 0, SEEK_END) == -1)
		return NULL;

	long size = ftell(input);
	if (size == -1)
		return NULL;

	if (fseek(input, 0, SEEK_SET) == -1)
		return NULL;

	char* content = (char*)malloc((size_t)size + 1);
	if (content == NULL)
		return NULL;

	fread(content, 1, (size_t)size, input);
	if (ferror(input))
	{
		free(content);
		return NULL;
	}

	fclose(input);
	content[size] = '\0';

	return content;
}

// Print shader errors
void print_log(GLuint object)
{
	GLint log_length = 0;

	if (glIsShader(object))
	{
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	}
	else if (glIsProgram(object))
	{
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	}
	else
	{
		fprintf(stderr, "printlog: Not a shader or a program\n");
		return;
	}

	char* log = (char*)malloc(log_length);

	if (glIsShader(object))
	{
		glGetShaderInfoLog(object, log_length, NULL, log);
	}
	else if (glIsProgram(object))
	{
		glGetProgramInfoLog(object, log_length, NULL, log);
	}

	fprintf(stderr, "%s", log);
	free(log);
}

GLuint create_shader(const char* filename, GLenum type)
{
	const GLchar* source = file_read(filename);
	if (source == NULL)
	{
		fprintf(stderr, "Error opening %s: ", filename);
		system("pause");
		return 0;
	}

	// Read in shader source from file
	const GLchar* sources[2] = 
	{
#ifdef GL_ES_VERSION_2_0
		"#version 100\n"
		"#define GLES2\n",
#else
		"#version 120\n",
#endif
		source
	};

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 2, sources, NULL);
	free((void*)source);

	glCompileShader(shader);
	GLint compile_ok = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE)
	{
		fprintf(stderr, "%s compile failed!\n", filename);
		print_log(shader);
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

void init_buffers()
{
	//// Create and bind vertex buffers
	//GLfloat triangle_vertices[] = 
	//{
	//	 0.0,  0.8,
	//	-0.8, -0.8,
	//	 0.8, -0.8,
	//};

	//glGenBuffers(1, &vbo_triangle);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

	//// Create and bind color buffers
	//GLfloat triangle_colors[] =
	//{
	//	1.0, 1.0, 0.0,
	//	0.0, 0.0, 1.0,
	//	1.0, 0.0, 0.0,
	//};

	//glGenBuffers(1, &vbo_triangle_colors);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_colors);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_colors), triangle_colors, GL_STATIC_DRAW);

	struct vertex
	{
		GLfloat x, y;		// position
		GLfloat r, g, b;	// colors
	};

	vertex vertices[] = 
	{
		{ 0.0,  0.8, 1.0, 1.0, 0.0},
		{-0.8, -0.8, 0.0, 0.0, 1.0},
		{ 0.8, -0.8, 1.0, 0.0, 0.0},
	};

	glGenBuffers(1, &vbo_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

int init_shaders(void)
{
	GLuint vertex_shader = create_shader("shader_colorbuffer.vs", GL_VERTEX_SHADER);
	if (vertex_shader == 0)
		return 0;

	GLuint fragment_shader = create_shader("shader_colorbuffer.fs", GL_FRAGMENT_SHADER);
	if (fragment_shader == 0)
		return 0;

	// Link vertex shader and fragment shader
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	GLint link_ok = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok)
	{
		fprintf(stderr, "glLinkProgram: ");
		print_log(program);
	}

	// Bind shader variable coord2d
	const char* attribute_name = "coord2d"; // the name must be same as the one in shader file
	attribute_coord2d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord2d == -1)
	{
		fprintf(stderr, "Cound not bind attribute %s\n", attribute_name);
		return 0;
	}

	// Bind shader variable v_color
	attribute_name = "v_color"; // the name must be same as the one in shader file
	attribute_v_color = glGetAttribLocation(program, attribute_name);
	if (attribute_coord2d == -1)
	{
		fprintf(stderr, "Cound not bind attribute %s\n", attribute_name);
		return 0;
	}

	const char* uniform_name;
	uniform_name = "fade";
	uniform_fade = glGetUniformLocation(program, uniform_name);
	if (uniform_fade == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		return 0;
	}

	return 1;
}

void onDisplay()
{
	// Enable alpha
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);

	glEnableVertexAttribArray(attribute_coord2d);
	glEnableVertexAttribArray(attribute_v_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);

	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glVertexAttribPointer(attribute_v_color, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(attribute_coord2d);
	glDisableVertexAttribArray(attribute_v_color);

	glutSwapBuffers();
}

void onIdle()
{
	float cur_fade = sinf(glutGet(GLUT_ELAPSED_TIME) / 1000.0 * (2 * 3.1415) / 5) / 2 + 0.5; // 0 -> 1 -> 0 every 5 seconds
	glUseProgram(program);
	glUniform1f(uniform_fade, cur_fade);
	glutPostRedisplay();
}

void cleanup()
{
	glDeleteProgram(program);
	glDeleteBuffers(1, &vbo_triangle);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Shader - fading");

	// glewInit Must be called after glutInit
	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return EXIT_FAILURE;
	}

	if (init_shaders())
	{
		init_buffers();
		glutDisplayFunc(onDisplay);
		glutIdleFunc(onIdle);
		glutMainLoop();
	}

	cleanup();

	return 0;
}
