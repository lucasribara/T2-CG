// **********************************************************************
//	SecondTexture.cpp   --   Exemplo de mapeamento de texturas.
//		Este programa foi criado a partir de um fonte obtido em 
//	http://nehe.gamedev.net, na "Lesson 06"
// **********************************************************************

		
#include <windows.h>
#include <stdio.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <glut.h>
#include "tgaload.h"

// Qtd m�xima de texturas a serem usadas no programa
#define MAX_NO_TEXTURES 2

#define OBJETO_ESQUERDA 0
#define OBJETO_DIREITA  1

// vetor com os n�meros das texturas
GLuint texture_id[MAX_NO_TEXTURES];

float xrot;
float yrot;
float zrot;
float ratio;


// **********************************************************************
//  void initTexture(void)
//		Define a textura a ser usada 
//
// **********************************************************************
void initTexture (void)
{

	image_t temp_image; // vari�vel que ir� armazenar a textura a ser usada

	// Habilita o uso de textura 
	glEnable ( GL_TEXTURE_2D );

	// Define a forma de armazenamento dos pixels na textura (1= alihamento por byte)
	glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

	// Define quantas texturas ser�o usadas no programa 
	glGenTextures (1, texture_id);  // 1 = uma textura;
									// texture_id = vetor que guardas os n�meros das texturas

	// Define os n�meros da textura dos cubos
	texture_id[OBJETO_ESQUERDA] = 1001;
	texture_id[OBJETO_DIREITA] = 1002;

	// ****
	// Define a textura do objeto da ESQUERDA
	// ****

	// Define que tipo de textura ser� usada
	// GL_TEXTURE_2D ==> define que ser� usada uma textura 2D (bitmaps)
	// texture_id[OBJETO_ESQUERDA]  ==> define o n�mero da textura 
	glBindTexture ( GL_TEXTURE_2D, texture_id[OBJETO_ESQUERDA] );
	// Carrega a primeira imagem .TGA 
	tgaLoad  ( "TCG1.tga", &temp_image, TGA_FREE | TGA_LOW_QUALITY );


	// ****
	// Define a textura do objeto da DIREITA
	// ****

	// Define que tipo de textura ser� usada
	// GL_TEXTURE_2D ==> define que ser� usada uma textura 2D (bitmaps)
	// texture_id[OBJETO_ESQUERDA]  ==> define o n�mero da textura 
	glBindTexture ( GL_TEXTURE_2D, texture_id[OBJETO_DIREITA] );

	// carrega a segunda imagem TGA 
	tgaLoad  ( "gremio2.tga", &temp_image, TGA_FREE | TGA_LOW_QUALITY );

}
// **********************************************************************
//  void init(void)
//		Inicializa os par�metros globais de OpenGL
//
// **********************************************************************
void init(void)
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glEnable ( GL_COLOR_MATERIAL );
	glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
	
	glEnable ( GL_CULL_FACE );	
}
// **********************************************************************
//  void reshape( int w, int h )
//		trata o redimensionamento da janela OpenGL
//
// **********************************************************************
void reshape( int w, int h )
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	ratio = 1.0f * w / h;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set the clipping volume
	gluPerspective(80,ratio,1,200);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 30,
		      0,0,10,
			  0.0f,1.0f,0.0f);
}
// **********************************************************************
//   void DesenhaCubo (GLuint nro_da_textura)
//
//
// **********************************************************************
void DesenhaCubo (GLuint nro_da_textura)
{
	// Desenha Cubo 1

	// define qual das texturas usar
	glBindTexture ( GL_TEXTURE_2D, nro_da_textura);
	
	glBegin ( GL_QUADS );
		// Front Face
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		// Back Face
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		// Top Face
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		// Bottom Face
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		// Right face
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		// Left Face
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	glEnd();
	

}

// **********************************************************************
//  void display( void )
//
//
// **********************************************************************
void display( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity ( );

	
	glPushMatrix();
		glTranslatef ( -3.0, 0.0, -5.0 );
		glRotatef ( xrot, 1.0, 0.0, 0.0 );
		glRotatef ( yrot, 0.0, 1.0, 0.0 );
		glRotatef ( zrot, 0.0, 0.0, 1.0 );

		DesenhaCubo (texture_id[OBJETO_ESQUERDA]);	

	glPopMatrix();

	glPushMatrix();
		glTranslatef ( +3.0, 0.0, -5.0 );
		glRotatef ( xrot, 1.0, 0.0, 0.0 );
		glRotatef ( yrot, 0.0, 1.0, 0.0 );
		glRotatef ( zrot, 0.0, 0.0, 1.0 );

		DesenhaCubo (texture_id[OBJETO_DIREITA]);	

	glPopMatrix();
	
	xrot+=0.3f;
	yrot+=0.2f;
	zrot+=0.4f;
	glutSwapBuffers();
}

// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
//
//
// **********************************************************************
void keyboard ( unsigned char key, int x, int y )  
{
	switch ( key ) 
	{
    case 27:        // When Escape Is Pressed...
      exit ( 0 );   // Exit The Program
      break;        // Ready For Next Case
    default:        // Now Wrap It Up
      break;
  }
}

// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )  
//
//
// **********************************************************************
void arrow_keys ( int a_keys, int x, int y )  
{
	switch ( a_keys ) 
	{
		case GLUT_KEY_UP:     // When Up Arrow Is Pressed...
			glutFullScreen ( ); // Go Into Full Screen Mode
			break;
	    case GLUT_KEY_DOWN:               // When Down Arrow Is Pressed...
			glutInitWindowSize  ( 700, 500 ); 
			break;
		default:
			break;
	}
}

// **********************************************************************
//  void main ( int argc, char** argv )
//
//
// **********************************************************************
void main ( int argc, char** argv )   
{
	glutInit            ( &argc, argv ); 
	glutInitDisplayMode ( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA ); 
	glutInitWindowPosition (0,0);
	glutInitWindowSize  ( 700, 500 ); 
	glutCreateWindow    ( "T�picos em Computa��o Gr�fica - Teste com Texturas." ); 
		
	init ();
	initTexture ();
		
	glutDisplayFunc ( display );  
	glutReshapeFunc ( reshape );
	glutKeyboardFunc ( keyboard );
	glutSpecialFunc ( arrow_keys );
	glutIdleFunc ( display );
	glutMainLoop ( );          
}




