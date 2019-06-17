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

typedef struct ObjetoEstatico //casa ou predio
{
    Ponto pos;
    Ponto scale;
    float max_x, max_z, min_x, min_z;
    int tipo;
} ObjetoEstatico;

typedef struct Inimigo
{
    Ponto pos;
    Ponto scale;
    Ponto alvo;
    float max_x, max_z, min_x, min_z;
} Inimigo;

typedef struct Fuel
{
    Ponto pos;
    Ponto scale;
    float max_x, max_z, min_x, min_z;
    bool visivel;
} Fuel;

typedef struct Player
{
    Ponto pos;
    Ponto alvo;
    float max_x, max_z, min_x, min_z;
    bool vivo;
    float fuelLevel;
} Player;

ObjetoEstatico *objs;
Inimigo *inimigos;
Fuel *capsulas;
Player play = {{0.0f, -0.4f, 5.0f}, {0.0f, 0.0f, -1.0f}, 0.5f, 5.5f, -0.5f, 4.5f, 100.0f, true};

float colisionThrehsold = 0.2f;

int nObjs;
int nInimigos;
int nCapsulas;

ifstream arquivo;

int altura, largura;

GLfloat AspectRatio, AngY=0;
GLuint TexBuilding, TexHouse, TexGrass, TexDirt, TexFuel, TexEnemy;
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

Ponto vetorUnitario(Ponto p0, Ponto p1)
{
    Ponto res;
    Ponto vetor;
    vetor = {p1.x - p0.x, p1.y - p0.y, p1.z - p0.z};
    float modulo = sqrt(pow(vetor.x, 2)+pow(vetor.y, 2)+pow(vetor.z, 2));
    if(modulo == 0) return {0.0f, 0.0f, 0.0f};
    res = {vetor.x/modulo, vetor.y/modulo, vetor.z/modulo};
    return res;
}

Ponto vetorUnitario(Ponto vetor)
{
    Ponto res;
    float modulo = sqrt(pow(vetor.x, 2)+pow(vetor.y, 2)+pow(vetor.z, 2));
    if(modulo == 0) return {0.0f, 0.0f, 0.0f};
    res = {vetor.x/modulo, vetor.y/modulo, vetor.z/modulo};
    return res;
}

Ponto produtoVetorial(Ponto v1, Ponto v2)
{
    Ponto res;
    res.x = (v1.y * v2.z) - (v1.z * v2.y);
    res.y = (v1.z * v2.x) - (v1.x * v2.z);
    res.z = (v1.x * v2.y) - (v1.y * v2.x);
    return res;
}

Ponto calculaNormal(Ponto p1, Ponto p2, Ponto p3)
{
    Ponto v1, v2;

    v1.x = p1.x - p2.x;
    v1.y = p1.y - p2.y;
    v1.z = p1.z - p2.z;

    v2.x = p2.x - p3.x;
    v2.y = p2.y - p3.y;
    v2.z = p2.z - p3.z;

    Ponto pv = produtoVetorial(v1, v2);
    Ponto res = vetorUnitario(pv);
    return res;
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
		//
		for(int i=0; i<NFaces; i++)
        {
            Ponto p1 = {Objeto[i].P1.X, Objeto[i].P1.Y, Objeto[i].P1.Z};
            Ponto p2 = {Objeto[i].P2.X, Objeto[i].P2.Y, Objeto[i].P2.Z};
            Ponto p3 = {Objeto[i].P3.X, Objeto[i].P3.Y, Objeto[i].P3.Z};
            Ponto n = calculaNormal(p1, p2, p3);
            glNormal3f(n.x, n.y, n.z);
            glVertex3f(Objeto[i].P1.X, Objeto[i].P1.Y,  Objeto[i].P1.Z);
            glVertex3f(Objeto[i].P2.X, Objeto[i].P2.Y,  Objeto[i].P2.Z);
            glVertex3f(Objeto[i].P3.X, Objeto[i].P3.Y,  Objeto[i].P3.Z);
		}
	glEnd();


}

bool testaColisaoObjetoEstatico()
{
    int i;
    for(i = 0; i < nObjs; i++)
    {
        if(play.pos.x > objs[i].min_x-colisionThrehsold &&
           play.pos.x < objs[i].max_x+colisionThrehsold &&
           play.pos.z > objs[i].min_z-colisionThrehsold &&
            play.pos.z < objs[i].max_z+colisionThrehsold)
        {
            //if()
            cout << "COLISAO" << endl;
            return true;
        }
    }
    return false;
}


// **********************************************************************
//  void initTexture(void)
//        Define a textura a ser usada
//
// **********************************************************************
void initTexture (void)
{
    TexBuilding = LoadTexture ("Build.png");
    TexHouse = LoadTexture ("House.png");
    TexGrass = LoadTexture("Teste.png");
    TexDirt = LoadTexture("Dirt.png");
    TexFuel = LoadTexture("Fuel.png");
    TexEnemy = LoadTexture("Enemy.png");
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



void armazenaObjetos()
{
    int i, j, obj, objCount = 0, enemyCount = 0, fuelCount = 0;
    for(i = 0; i < altura; i++)
    {
        for(j = 0; j < largura; j++)
        {
            if(mapMatrix[i][j] == 1 || mapMatrix[i][j] == 2 || mapMatrix[i][j] == 4)
            {
                objCount++;
            }
            if(mapMatrix[i][j] == 5)
            {
                fuelCount++;
            }
            if(mapMatrix[i][j] == 6)
            {
                enemyCount++;
            }
            if(mapMatrix[i][j] == 7)
            {
                play.pos.x = i*2;
                play.pos.z = j*2;
                play.max_x = play.pos.x + 0.5f;
                play.min_x = play.pos.x - 0.5f;
                play.max_z = play.pos.z + 0.5f;
                play.min_z = play.pos.z - 0.5f;
                play.fuelLevel = 100.0f;
                play.vivo = true;
            }
        }
    }
    nObjs = objCount;
    nInimigos = enemyCount;
    nCapsulas = fuelCount;
    inimigos = new Inimigo[enemyCount];
    capsulas = new Fuel[fuelCount];
    objs = new ObjetoEstatico[objCount];

    int enemyCount2 = 0;
    int fuelCount2 = 0;
    int objCount2 = 0;

    for(i = 0; i < altura; i++)
    {
        for(j = 0; j < largura; j++)
        {
            if(mapMatrix[i][j] == 1 && objCount2 < objCount)
            {
                objs[objCount2].pos = {i*2.0f, 0.0f, j*2.0f};
                objs[objCount2].scale = {1.0f, 1.0f, 1.0f};
                objs[objCount2].max_x = objs[objCount2].pos.x + 1.0f;
                objs[objCount2].min_x = objs[objCount2].pos.x - 1.0f;
                objs[objCount2].max_z = objs[objCount2].pos.z + 1.0f;
                objs[objCount2].min_z = objs[objCount2].pos.z - 1.0f;
                objs[objCount2].tipo = 0;
                objCount2++;
            }
            if(mapMatrix[i][j] == 2 && objCount2 < objCount)
            {
                objs[objCount2].pos = {i*2.0f, 2.0f, j*2.0f};
                objs[objCount2].scale = {1.0f, 3.0f, 1.0f};
                objs[objCount2].max_x = objs[objCount2].pos.x + 1.0f;
                objs[objCount2].min_x = objs[objCount2].pos.x - 1.0f;
                objs[objCount2].max_z = objs[objCount2].pos.z + 1.0f;
                objs[objCount2].min_z = objs[objCount2].pos.z - 1.0f;
                objs[objCount2].tipo = 1;
                objCount2++;
            }
            if(mapMatrix[i][j] == 4 && objCount2 < objCount)
            {
                objs[objCount2].pos = {i*2.0f, -0.9f, j*2.0f};
                objs[objCount2].scale = {0.1f, 0.1f, 0.1f};
                objs[objCount2].max_x = objs[objCount2].pos.x + 0.1f;
                objs[objCount2].min_x = objs[objCount2].pos.x - 0.1f;
                objs[objCount2].max_z = objs[objCount2].pos.z + 0.1f;
                objs[objCount2].min_z = objs[objCount2].pos.z - 0.1f;
                objs[objCount2].tipo = 2;
                objCount2++;
            }
            if(mapMatrix[i][j] == 5 && fuelCount2 < fuelCount)
            {
                capsulas[fuelCount2].pos = {i*2.0f, 0.0f, j*2.0f};
                capsulas[fuelCount2].scale = {0.2f, 0.2f, 0.2f};
                capsulas[fuelCount2].max_x = capsulas[fuelCount2].pos.x + 0.2f;
                capsulas[fuelCount2].min_x = capsulas[fuelCount2].pos.x - 0.2f;
                capsulas[fuelCount2].max_z = capsulas[fuelCount2].pos.z + 0.2f;
                capsulas[fuelCount2].min_z = capsulas[fuelCount2].pos.z - 0.2f;
                capsulas[fuelCount2].visivel = true;
            }
            if(mapMatrix[i][j] == 6 && enemyCount2 < enemyCount)
            {
                inimigos[enemyCount2].pos = {i*2.0f, -0.5f, j*2.0f};
                inimigos[enemyCount2].scale = {0.5f, 0.5f, 0.5f};
                inimigos[enemyCount2].alvo = vetorUnitario(inimigos[enemyCount2].pos, play.pos);
                inimigos[enemyCount2].max_x = inimigos[enemyCount2].pos.x + 0.5f;
                inimigos[enemyCount2].min_x = inimigos[enemyCount2].pos.x - 0.5f;
                inimigos[enemyCount2].max_z = inimigos[enemyCount2].pos.z + 0.5f;
                inimigos[enemyCount2].min_z = inimigos[enemyCount2].pos.z - 0.5f;
            }
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
	glClearColor(0.0f, 191.0f, 255.0f, 1.0f); // Fundo de tela azul

	leMapa();
	armazenaObjetos();

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
	gluLookAt(play.pos.x, play.pos.y, play.pos.z,
		      play.pos.x+play.alvo.x,play.pos.y+play.alvo.y,play.pos.z+play.alvo.z,
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
void desenhaComTexturaReplicada()
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
void desenhaComTexturaEstendida ()
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

void desenhaTerreno()
{
    int i, j;
    for(i = 0; i < altura; i++)
    {
        for(j = 0; j < largura; j++)
        {
            if(mapMatrix[i][j] == 3) //terra
            {
                glPushMatrix();
                glTranslatef (i*2.0f, -1.0f, j*2.0f);
                glBindTexture (GL_TEXTURE_2D, TexDirt);
                glScalef(1.0f, 0.01f, 1.0f);
                desenhaComTexturaEstendida();
                glPopMatrix();
            }
            else //grama
            {
                glPushMatrix();
                glTranslatef (i*2.0f, -1.0f, j*2.0f);
                glBindTexture (GL_TEXTURE_2D, TexGrass);
                glScalef(1.0f, 0.01f, 1.0f);
                desenhaComTexturaEstendida();
                glPopMatrix();
            }
        }
    }
}

void desenhaObjetoEstatico(ObjetoEstatico obj)
{
    if(obj.tipo == 0) //casa ou predio
    {
        glPushMatrix();
        glTranslatef (obj.pos.x, obj.pos.y, obj.pos.z);
        glBindTexture (GL_TEXTURE_2D, TexHouse);
        glScalef(obj.scale.x, obj.scale.y, obj.scale.z);
        desenhaComTexturaEstendida();
        glPopMatrix();
    }
    else if(obj.tipo == 1)
    {
        glPushMatrix();
        glTranslatef (obj.pos.x, obj.pos.y, obj.pos.z);
        glBindTexture (GL_TEXTURE_2D, TexBuilding);
        glScalef(obj.scale.x, obj.scale.y, obj.scale.z);
        desenhaComTexturaEstendida();
        glPopMatrix();
    }
    else
    {
        glPushMatrix();
		glTranslatef (obj.pos.x, obj.pos.y, obj.pos.z);
        glScalef(obj.scale.x, obj.scale.x, obj.scale.x);
        ExibeObjeto(TriObj,NFacesTriObj);
        glPopMatrix();
    }

}

void desenhaCombustivel(Fuel f)
{
    if(f.visivel == true)
    {
        glPushMatrix();
        glBindTexture (GL_TEXTURE_2D, TexFuel);
        glTranslatef (f.pos.x, f.pos.y, f.pos.z);
        glScalef(f.scale.x, f.scale.x, f.scale.x);
        glTranslatef(0.0f, -2.8f, 0.0f);
        glRotatef(AngY, 0, 1, 0);
        desenhaComTexturaReplicada();
        glPopMatrix();
    }
}

void desenhaInimigo(Inimigo i)
{
    glPushMatrix();
    glBindTexture (GL_TEXTURE_2D, TexEnemy);
    glTranslatef (i.pos.x, i.pos.y, i.pos.z);
    glScalef(i.scale.x, i.scale.x, i.scale.x);
    glTranslatef(0.0f, 0.0f, 0.0f);
    desenhaComTexturaReplicada();
    glPopMatrix();
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

	desenhaTerreno();
	int i, j;

	for (i =0; i<altura; i++)
    {
        for (j=0; j <largura; j++)
        {
            float transX = float(i)*2.0f;
            float transZ = float(j)*2.0f;
            /*glPushMatrix();
                glTranslatef ( transX, -2.0f, transZ );
                //glRotatef(AngY,0,1,0);
                //glColor3f(0.5f,0.0f,0.0f); // Vermelho
                glBindTexture (GL_TEXTURE_2D, TexGrass);
                glScalef(1.0f, 0.01f, 1.0f);
                DesenhaCubo();
            glPopMatrix();*/
        }
    }

    for(i = 0; i < nObjs; i++)
    {
        desenhaObjetoEstatico(objs[i]);
    }

    for(i = 0; i < nCapsulas; i++)
    {
        desenhaCombustivel(capsulas[i]);
    }

    for(i = 0; i < nInimigos; i++)
    {
        desenhaInimigo(inimigos[i]);
    }

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
    AngY+=0.5f;
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
    bool objCol;

	switch ( a_keys )
	{
		case GLUT_KEY_UP:
			if(true)
            {
                objCol = testaColisaoObjetoEstatico();
                if(objCol == true)
                {
                    play.pos.x += play.alvo.x * fraction;
                }
                else
                {
                    play.pos.x += play.alvo.x * fraction;
                    play.pos.z += play.alvo.z * fraction;
                }

                cout << "#############" << endl;
                cout << "POS" << endl;
                cout << play.pos.x << endl;
                cout << play.pos.z << endl;
                cout << "ALVO" << endl;
                cout << play.alvo.x << endl;
                cout << play.alvo.z << endl;
                cout << "#############" << endl;

            }
			break;
	    case GLUT_KEY_DOWN:
			if(true)
            {
                play.pos.x -= play.alvo.x * fraction;
                play.pos.z -= play.alvo.z * fraction;
            }
			break;
        case GLUT_KEY_LEFT:
            angle -= 0.05f;
            play.alvo.x = sin(angle);
            play.alvo.z = -cos(angle);
            break;
        case GLUT_KEY_RIGHT:
            angle += 0.05f;
            play.alvo.x = sin(angle);
            play.alvo.z = -cos(angle);
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



