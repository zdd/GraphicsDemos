#include <gl/glew.h>
#include <gl/glut.h>

const float vertices[][3] =
{
	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	 0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
};

const int indices[][4] = {
	0, 1, 2, 3, // front face
	4, 5, 6, 7, // back face
	5, 0, 3, 6, // left face
	1, 4, 7, 2, // right face
	3, 2, 7, 6, // top face
	1, 0, 5, 4, // bottom face
};

void display(){
	static int last_time = glutGet(GLUT_ELAPSED_TIME);
    int current_time = glutGet(GLUT_ELAPSED_TIME);
    int dt = last_time - current_time;
    last_time = current_time;

	static float theta = 0.0f;
    theta += (float)dt / 1000.0 * 100;
    if (theta > 360.0) 
		theta += -360.0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -2.0);
    glRotatef(theta, 0.0, 1.0, 0.0);

	for (int face = 0; face < 6; ++face)
	{
		glBegin(GL_POLYGON);
		for (int index = 0; index < 4; ++index)
		{
			glColor3fv(vertices[indices[face][index]]);
			glVertex3fv(vertices[indices[face][index]]);
		}
		glEnd();
	}

    glutSwapBuffers();
}

void initializeGL(){
    // Setup Viewport matrix
    glViewport(0, 0, 640, 480);

    //Setup Projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 640 / 480.0, 0.1f, 1000.0f);

    //Initialize Modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0f);

	// Back face culling
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	//glCullFace(GL_BACK);
}

int main(int argc, char** argv){

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(640, 480);
    glutCreateWindow("OpenGL Cube");
    glutDisplayFunc(display);
    glutIdleFunc(display);

    initializeGL();

    glutMainLoop();

    return 0;
}