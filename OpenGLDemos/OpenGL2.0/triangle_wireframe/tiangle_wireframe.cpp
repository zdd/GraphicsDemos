#include <windows.h>
#include <gl/glut.h>

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT) ;

	// Wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_TRIANGLES) ;
		glVertex3f( 0.0, 1.0, 0.0) ;
		glVertex3f(-0.5, 0.0, 0.0) ;
		glVertex3f( 0.5, 0.0, 0.0) ;
	glEnd() ;

	glFlush() ;
}

int main(int argc, char **argv)
{
	
	glutInit(&argc, argv) ;
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB) ;
	glutCreateWindow("Triangle") ;
	glutDisplayFunc(display) ;
	glutMainLoop() ;

	return 0 ;
}