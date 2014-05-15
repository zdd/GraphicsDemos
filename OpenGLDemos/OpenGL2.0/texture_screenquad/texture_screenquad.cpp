#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h>
#include <gl/glut.h>

#include <SOIL.h>

GLuint program;
GLint  attribute_coord3d;
GLint  attribute_texcoord;
GLint  uniform_mytexture;
GLuint texture_id;
GLuint vbo_quad;
GLuint vbo_quad_texcoord;

// Read shader source from file
char* file_read(const char* filename)
{
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
	system("pause"); // Pause the console window to let user see the error.
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
	/*

	0                  1
	 ------------------
	 |                |
	 |                |
	 |                |
	 |                |
	 |                |
	 |                |
   3 ------------------ 2

	*/

	// Quad vertices
	GLfloat quad_vertices[] = 
	{
		-0.8,  0.8, 0.0,	// 0
		 0.8,  0.8, 0.0,	// 1
		 0.8, -0.8, 0.0,	// 2
		-0.8, -0.8, 0.0,	// 3
	};

	glGenBuffers(1, &vbo_quad);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_quad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

	// Quad texture coordinates
	GLfloat quad_texcoords[] =
	{
		0.0, 0.0,	// 0
		1.0, 0.0,   // 1
		1.0, 1.0,   // 2
		0.0, 1.0,   // 3
	};

	glGenBuffers(1, &vbo_quad_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_quad_texcoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_texcoords), quad_texcoords, GL_STATIC_DRAW);
}

void init_textures()
{	
	// Way 2, this is simple than way 1
	glActiveTexture(GL_TEXTURE0);
	texture_id = SOIL_load_OGL_texture("box.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	if (texture_id == 0)
	{
		fprintf(stderr, "%s", "SOIL loading error");
	}
}

int init_shaders(void)
{
	GLuint vertex_shader = create_shader("texture_screenquad.vs", GL_VERTEX_SHADER);
	if (vertex_shader == 0)
		return 0;

	GLuint fragment_shader = create_shader("texture_screenquad.fs", GL_FRAGMENT_SHADER);
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

	// Bind shader variables
	const char* attribute_name = "coord3d";
	attribute_coord3d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord3d == -1)
	{
		fprintf(stderr, "Cound not bind attribute %s\n", attribute_name);
		return 0;
	}

	// Bind vertex texture coordinate
	attribute_name = "texcoord";
	attribute_texcoord = glGetAttribLocation(program, attribute_name);
	if (attribute_texcoord == -1)
	{
		fprintf(stderr, "Cound not bind attribute %s\n", attribute_name);
		return 0;
	}

	return 1;
}

void onDisplay()
{
	// Enable alpha
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(1.0, 1.0, 1.0, 1.0); // Background color
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Set texture
	glUseProgram(program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	uniform_mytexture = glGetUniformLocation(program, "mytexture");
	glUniform1i(uniform_mytexture, 0);

	// Set texture coordinates buffer
	glEnableVertexAttribArray(attribute_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_quad_texcoord);
	glVertexAttribPointer(attribute_texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Set vertex buffer
	glUseProgram(program);
	glEnableVertexAttribArray(attribute_coord3d);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_quad);
	glVertexAttribPointer(attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisableVertexAttribArray(attribute_coord3d);
	glDisableVertexAttribArray(attribute_texcoord);

	glFlush();
}

void cleanup()
{
	glDeleteProgram(program);
	glDeleteBuffers(1, &vbo_quad);
	glDeleteBuffers(1, &vbo_quad_texcoord);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_SINGLE);

	//If use GLUT_DOUBLE instead of GLUT_SINGLE, you MUST use glutSwapBuffers instead of glFlush in onDisplay function.
	//glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(640, 640);
	glutCreateWindow("Screen Quad");

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
		init_textures();
		glutDisplayFunc(onDisplay);
		glutMainLoop();
	}

	cleanup();

	return 0;
}
