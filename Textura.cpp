// **********************************************************************
// PUCRS/Escola Politécnica
// COMPUTAÇÃO GRÁFICA
//
// Programa básico para criar aplicacoes 3D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************

#include <iostream>
#include <cmath>
#include <ctime>
#include<fstream>

using namespace std;

#ifdef WIN32
#include <windows.h>
#include "gl\glut.h"
    static DWORD last_idle_time;
#else
    #include <sys/time.h>
    static struct timeval last_idle_time;
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#include "SOIL/SOIL.h"

#include "TextureClass.h"

typedef struct Ponto
{
    float x,y,z;
} Ponto;

Ponto alvo = {0.0f, 0.0f, -1.0f};
Ponto obs = {0.0f, -0.4f, 5.0f};
Ponto novoAlvo = alvo;

ifstream arquivo;

int altura, largura;

GLfloat AspectRatio, AngY=0;
GLuint TEX1, TEX2, TEX3, TEXDirt, TEXFuel;
float alvoNovoX=0, alvoNovoY=0, alvoNovoZ=-8;
float posX=0, posY=0, posZ=0;
float mov = 3;
float step= 0.1;

int mapMatrix[10][10];
// angle of rotation for the camera direction
float angle=0.0;
// actual vector representing the camera's direction
float lx=0.0f,lz=-1.0f;
// XZ position of the camera
float x=0.0f,z=5.0f;

typedef struct  // Struct para armazenar um ponto
{
    float X,Y,Z;
    void Set(float x, float y, float z)
    {
        X = x; Y = y; Z = z;
    }

} TPoint;


typedef struct // Struct para armazenar um tri‚ngulo
{
    TPoint P1, P2, P3;
} TTriangle;


TTriangle TriObj[14000]; // Vetor que armazena um objeto de atÈ 6000 faces
int NFacesTriObj; // Vari·vel que armazena o n˙mero real de faves do objeto

// *********************************************************************
//   DESCRICAO DO FORMATO ".TRI"
// *********************************************************************

/*
 A primeira linha do arquivo contÈm o n˙mero de tri‚ngulos que forma o objeto
 As demais linhas descrevemas os tri‚ngulos:
 x1 y1 z1 x2 y2 z2 x3 y3 z3
 onde
 x1 y1 z1 : primeiro vÈrtice do tri‚ngulo
 x2 y2 z2 : segundo vÈrtice do tri‚ngulo
 x3 y3 z3 : terceiro vÈrtice do tri‚ngulo
 Um exemplo com dois tri‚ngulos:
 2
 0 0 10 10 0 10 5 10 10
 10 0 10 20 0 10 15 10 10

 */
// **********************************************************************
// void LeObjeto(char *Nome)
//
//
// **********************************************************************
void LeObjeto (char *Nome, TTriangle Objeto[], int &NFaces)
{
    // ***************
    // Exercicio
    //      complete esta rotina fazendo a leitura do objeto
    // ***************

    ifstream arq;
    arq.open(Nome, ios::in);
    arq >> NFaces;
    float x,y,z;
    for (int i=0;i<NFaces;i++)
    {
        arq >> x >> y >> z; // Vertice 1
        //cout << x << " , " << y << " , " << z << endl;
        Objeto[i].P1.Set(x,y,z);
        arq >> x >> y >> z; // Vertice 2
        //cout << x << " , " << y << " , " << z << endl;
        Objeto[i].P2.Set(x,y,z);
        arq >> x >> y >> z; // Vertice 3
        //cout << x << " , " << y << " , " << z << endl;
        Objeto[i].P3.Set(x,y,z);

    }



}

// **********************************************************************
// void ExibeObjeto (TTriangle **Objeto)
//
//
// **********************************************************************
void ExibeObjeto (TTriangle *Objeto, int NFaces)
{
    glBegin ( GL_TRIANGLES );
		// Front Face
		//glNormal3f(0,0,1);
		for(int i=0; i<NFaces; i++)
        {
            glVertex3f(Objeto[i].P1.X, Objeto[i].P1.Y,  Objeto[i].P1.Z);
            glVertex3f(Objeto[i].P2.X, Objeto[i].P2.Y,  Objeto[i].P2.Z);
            glVertex3f(Objeto[i].P3.X, Objeto[i].P3.Y,  Objeto[i].P3.Z);
		}
	glEnd();


}


// **********************************************************************
//  void initTexture(void)
//        Define a textura a ser usada
//
// **********************************************************************
void initTexture (void)
{
    TEX1 = LoadTexture ("Build.png");
    TEX2 = LoadTexture ("House.png");
    TEX3 = LoadTexture("Grass.png");
    TEXDirt = LoadTexture("Dirt.png");
    TEXFuel = LoadTexture("Fuel.png");
}


// **********************************************************************
//  void DefineLuz(void)
//
//
// **********************************************************************
void DefineLuz(void)
{
  // Define cores para um objeto dourado
  GLfloat LuzAmbiente[]   = {0.4, 0.4, 0.4f } ;
  GLfloat LuzDifusa[]   = {0.7, 0.7, 0.7};
  GLfloat LuzEspecular[] = {0.9f, 0.9f, 0.9 };
  GLfloat PosicaoLuz0[]  = {3.0f, 3.0f, 0.0f };
  GLfloat Especularidade[] = {1.0f, 1.0f, 1.0f};

   // ****************  Fonte de Luz 0

  glEnable ( GL_COLOR_MATERIAL );

   // Habilita o uso de iluminaÁ„o
  glEnable(GL_LIGHTING);

  // Ativa o uso da luz ambiente
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LuzAmbiente);
  // Define os parametros da luz número Zero
  glLightfv(GL_LIGHT0, GL_AMBIENT, LuzAmbiente);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, LuzDifusa  );
  glLightfv(GL_LIGHT0, GL_SPECULAR, LuzEspecular  );
  glLightfv(GL_LIGHT0, GL_POSITION, PosicaoLuz0 );
  glEnable(GL_LIGHT0);

  // Ativa o "Color Tracking"
  glEnable(GL_COLOR_MATERIAL);

  // Define a reflectancia do material
  glMaterialfv(GL_FRONT,GL_SPECULAR, Especularidade);

  // Define a concentraçãoo do brilho.
  // Quanto maior o valor do Segundo parametro, mais
  // concentrado será o brilho. (Valores válidos: de 0 a 128)
  glMateriali(GL_FRONT,GL_SHININESS,51);

}

void leMapa()
{
    const char* file[] = {"mapa.txt"};
    arquivo.open(file[0]);


    if(!arquivo)
    {
        cout << "Erro ao abrir arquivo " << file[0] << endl;
        exit(1);
    }
    cout << "Lendo arquivo" << endl;
    arquivo >> altura;
    arquivo >> largura;
    cout << altura << endl;
    cout << largura << endl;
    for(int i = 0; i < altura; i++)
    {
        for(int j = 0; j < largura; j++)
        {
            arquivo >> mapMatrix[i][j];
        }
    }

}


// **********************************************************************
//  void init(void)
//		Inicializa os par‚metros globais de OpenGL
//
// **********************************************************************
void init(void)
{
	glClearColor(0.0f, 191.0f, 255.0f, 1.0f); // Fundo de tela preto

	leMapa();

	glShadeModel(GL_SMOOTH);
	glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
	glEnable(GL_DEPTH_TEST);
	glEnable ( GL_CULL_FACE );

    // Obtem o tempo inicial
#ifdef WIN32
    last_idle_time = GetTickCount();
#else
    gettimeofday (&last_idle_time, NULL);
#endif

    initTexture();

}
// **********************************************************************
//  void PosicUser()
//
//
// **********************************************************************
void PosicUser()
{
	// Set the clipping volume
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70,AspectRatio,0.01,200);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(obs.x, obs.y, obs.z,
		      obs.x+alvo.x,obs.y+alvo.y,obs.z+alvo.z,
			  0.0f,1.0f,0.0f);

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

	AspectRatio = 1.0f * w / h;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);
    //cout << "Largura" << w << endl;

	PosicUser();

}
void DesenhaCombustivel()
{
    glBegin ( GL_QUADS );
    // Front Face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    // Back Face
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    // Top Face
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    // Bottom Face
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    // Right face
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    // Left Face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();
}
// **********************************************************************
//   void DesenhaCubo (GLuint nro_da_textura)
//
//
// **********************************************************************
void DesenhaCubo ()
{
    glBegin ( GL_QUADS );
    // Front Face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(0.25f, 0.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(0.25f, 1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    // Back Face
    glTexCoord2f(0.75f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.75f, 1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.5f, 1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.5f, 0.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    // Top Face
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    // Bottom Face
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    // Right face
    glTexCoord2f(0.5f, 0.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.5f, 1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.25f, 1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.25f, 0.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    // Left Face
    glTexCoord2f(0.75f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.75f, 1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();

}

void CriaObjetoMapa(float transX, float transZ, int obj)
{
    switch ( obj )
    {
    case 0:
        glPushMatrix();
        glTranslatef ( transX, -1.0f, transZ );
        glBindTexture (GL_TEXTURE_2D, TEX3);
        glScalef(1.0f, 0.01f, 1.0f);
        DesenhaCubo();
        glPopMatrix();
        break;
    case 1:
        glPushMatrix();
        glTranslatef ( transX, 0.0f, transZ );
        //glRotatef(AngY,0,1,0);
        //glColor3f(0.5f,0.0f,0.0f); // Vermelho
        glBindTexture (GL_TEXTURE_2D, TEX2);
        DesenhaCubo();
        glPopMatrix();
        break;
    case 2:
        glPushMatrix();
        glTranslatef ( transX, 2.0f, transZ );
        //glRotatef(AngY,0,1,0);
        //glColor3f(0.5f,0.0f,0.0f); // Vermelho
        glBindTexture (GL_TEXTURE_2D, TEX1);
        glScalef(1.0f, 3.0f, 1.0f);
        DesenhaCubo();
        glPopMatrix();
        break;
    case 3:
        glPushMatrix();
        glTranslatef ( transX, -1.0f, transZ );
        glBindTexture (GL_TEXTURE_2D, TEXDirt);
        glScalef(1.0f, 0.01f, 1.0f);
        DesenhaCubo();
        glPopMatrix();
        break;
    case 4:
        //glPushMatrix();
        //glTranslatef ( transX, -1.0f, transZ );
        //glBindTexture (GL_TEXTURE_2D, TEX3);
        //glScalef(1.0f, 0.01f, 1.0f);
        //DesenhaCubo();
        //glPopMatrix();

        glPushMatrix();
		glTranslatef ( transX, -0.9f, transZ );
        //glRotatef(AngY,0,1,0);
		//glColor3f(0.5f,0.0f,0.0f); // Vermelho
        glScaled(0.1f, 0.1f, 0.1f);
        ExibeObjeto(TriObj,NFacesTriObj);
        glPopMatrix();
        break;

    }
    if(obj != 0)
    {
        glPushMatrix();
        glTranslatef ( transX, -1.0f, transZ );
        glBindTexture (GL_TEXTURE_2D, TEX3);
        glScalef(1.0f, 0.01f, 1.0f);
        DesenhaCubo();
        glPopMatrix();
    }

}

// **********************************************************************
//  void display( void )
//
//
// **********************************************************************
void display( void )
{

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glLoadIdentity();
	DefineLuz();

	PosicUser();

	glMatrixMode(GL_MODELVIEW);

	for (int i =0; i<altura; i++)
    {
        for (int j=0; j <largura; j++)
        {
            float transX = float(i);
            float transZ = float(j);
            CriaObjetoMapa(transX*2, transZ*2, mapMatrix[i][j]);
            /*glPushMatrix();
                glTranslatef ( transX, -2.0f, transZ );
                //glRotatef(AngY,0,1,0);
                //glColor3f(0.5f,0.0f,0.0f); // Vermelho
                glBindTexture (GL_TEXTURE_2D, TEX3);
                glScalef(1.0f, 0.01f, 1.0f);
                DesenhaCubo();
            glPopMatrix();*/
        }
    }


	glPushMatrix();
		glTranslatef ( 2.0f, 0.0f, -5.0f );
        //glRotatef(AngY,0,1,0);
		//glColor3f(0.5f,0.0f,0.0f); // Vermelho
        //glColor3f(0.0f,0.6f,0.0f);
        glScaled(0.1f, 0.1f, 0.1f);
        ExibeObjeto(TriObj,NFacesTriObj);
	glPopMatrix();

	glPushMatrix();
		glTranslatef ( -2.0f, 1.0f, -5.0f );
		//glRotatef(AngY,0,1,0);
		//glColor3f(0.0f,0.6f,0.0f); // Verde
        glBindTexture (GL_TEXTURE_2D, TEXFuel);
        glScaled(0.1f, 0.1f, 0.1f);
		DesenhaCombustivel();
		//glTranslatef ( -2.0f, 2.0f, -5.0f );
	glPopMatrix();

	glutSwapBuffers();
}

// **********************************************************************
//  void animate ( unsigned char key, int x, int y )
//
//
// **********************************************************************
void animate()
{
    static float dt;
    static float AccumTime=0;

#ifdef _WIN32
    DWORD time_now;
    time_now = GetTickCount();
    dt = (float) (time_now - last_idle_time) / 1000.0;
#else
    // Figure out time elapsed since last call to idle function
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
    dt = (float)(time_now.tv_sec  - last_idle_time.tv_sec) +
    1.0e-6*(time_now.tv_usec - last_idle_time.tv_usec);
#endif
    AccumTime +=dt;
    if (AccumTime >=3) // imprime o FPS a cada 3 segundos
    {
        cout << 1.0/dt << " FPS"<< endl;
        AccumTime = 0;
    }
    //cout << "AccumTime: " << AccumTime << endl;
    // Anima cubos
    AngY++;
    // Sa;va o tempo para o próximo ciclo de rendering
    last_idle_time = time_now;

        //if  (GetAsyncKeyState(32) & 0x8000) != 0)
          //  cout << "Espaco Pressionado" << endl;

    // Redesenha
    glutPostRedisplay();
}

// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
//
//
// **********************************************************************
void keyboard ( unsigned char key, int x, int y )
{
    float yrotrad;
	switch ( key )
	{
    case 27:        // Termina o programa qdo
      exit ( 0 );   // a tecla ESC for pressionada
      break;
    case 'w':
        //PosicaoNova = PosicaoAtual + VetorAlvoUnitario * TamanhoDoPasso
        //AlvoNovo = AlvoAtual + + VetorAlvoUnitario * TamanhoDoPasso
        posX = posX + 1*step;
        posZ = posZ + 1*step;

        alvoNovoX = alvoNovoX + 0.1*step;
        alvoNovoZ = alvoNovoX + 0.1*step;
        break;
    case 's':
        posX = posX - 1*step;
        posZ = posZ -1*step;

        alvoNovoX = alvoNovoX - 1*step;
        alvoNovoZ = alvoNovoX - 1*step;
        break;
    case 'a':
        //AlvoNovo.X = AlvoAtual.X*cos(alfa) + AlvoAtual.Z*sen(alfa)
        //AlvoNovo.Y =  AlvoAtual.Y
        //AlvoNovo.Z = -AlvoAtual.X*sen(alfa) + AlvoAtual.Z*cos(alfa)
        yrotrad = (1 / 180 * 3.141592654f);
        alvoNovoX -= float(cos(yrotrad)) * 0.2;
        alvoNovoZ -= float(sin(yrotrad)) * 0.2;
        break;
    case 'd':
        yrotrad = (1 / 180 * 3.141592654f);
        alvoNovoX += float(cos(yrotrad)) * 0.2;
        alvoNovoZ += float(sin(yrotrad)) * 0.2;
        break;

    default:
            cout << key;
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
    float fraction = 0.15f;

	switch ( a_keys )
	{
		case GLUT_KEY_UP:       // When Up Arrow Is Pressed...
			//glutFullScreen ( ); // Go Into Full Screen Mode
			obs.x += alvo.x * fraction;
			obs.z += alvo.z * fraction;
			cout << "#############" << endl;
			cout << obs.x << endl;
			cout << obs.z << endl;
			cout << "#############" << endl;
			break;
	    case GLUT_KEY_DOWN:     // When Down Arrow Is Pressed...
			//glutInitWindowSize  ( 700, 500 );
			obs.x -= alvo.x * fraction;
			obs.z -= alvo.z * fraction;
			break;
        case GLUT_KEY_LEFT:
            angle -= 0.05f;
            alvo.x = sin(angle);
            alvo.z = -cos(angle);
            break;
        case GLUT_KEY_RIGHT:
            angle += 0.05f;
            alvo.x = sin(angle);
            alvo.z = -cos(angle);
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
int main ( int argc, char** argv )
{
	glutInit            ( &argc, argv );
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
	glutInitWindowPosition (0,0);
	glutInitWindowSize  ( 700, 500 );
	glutCreateWindow    ( "Computacao Grafica - Exemplo Basico 3D" );

	init ();
    //system("pwd");

	glutDisplayFunc ( display );
	glutReshapeFunc ( reshape );
	glutKeyboardFunc ( keyboard );
	glutSpecialFunc ( arrow_keys );
	glutIdleFunc ( animate );

	glEnable(GL_DEPTH_TEST);

	char Nome[] = "cactus.tri";
	LeObjeto(Nome, TriObj, NFacesTriObj);

	glutMainLoop ( );
	return 0;
}



