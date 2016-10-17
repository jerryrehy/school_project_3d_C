#include "stdafx.h"
#include "stdio.h"
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#define GLEW_STATIC
#include "glew.h"
#include "wglew.h"
#include "glut.h"
#include <windows.h>
#include "Camera.cpp"
#include <math.h>
#include "Model.h"
#include "Model.cpp"
#include "pthread.h"

using namespace std;
using namespace cv;

#define PI 3.14159265
#define STEP_SIZE 2
#define TEXTURE	0

Model *MujObjekt;
CCamera camera;
Mat vyskova_mapa;
std::vector<cv::Mat> komponenty_vyskove_mapy;

bool fall = true,
	gameover = false,
	enemy = true;

float 
	rot_angle = 0.0,
	naklon = 0.0,
	naklon2 = 0.0,
	framesPs = 0.0
;

int 
	frame_count = 0,
	new_frame_count = 0,
	width = 0,
	height = 0,
	teziste_x = 0,
	teziste_y = 0,
	teziste_old_x = 0,
	teziste_old_y = 0,
	oldX=-100,
	oldY=400,
	mouseX=0,
	mouseY=0,
	newX=-100,
	newY=400,
	c = 200,
	bul_num = 0,
	bul_num2 = 0,
	d = 0,
	enemy_life = 5
;

enum {
    None,
    LeftButton,
    RightButton,
    MiddleButton
} status = None;

GLfloat color_grey[]={0.7, 0.7, 0.7, 1.0};
GLfloat color_white[]={1.0, 1.0, 1.0, 1.0};
GLfloat color_R[]={1.0, 0.0, 0.0, 1.0};
GLfloat color_G[]={0.0, 1.0, 0.0, 1.0};
GLfloat color_B[]={0.0, 0.0, 1.0, 1.0};
GLfloat color_Ra[]={1.0, 0.0, 0.0, 0.3};
GLfloat color_Ga[]={0.0, 1.0, 0.0, 0.3};
GLfloat color_Ba[]={0.0, 0.0, 1.0, 0.3};

GLint vertices[] = {
	-50, -50, -45,
	 50, -30, -45,
	  0, 60, -45,
	  0, 0, 45  
};

GLubyte triangles[] = {
	0, 1, 2,
	0, 1, 3,
	1, 2, 3,
	0, 2, 3
};

GLfloat lightpos[] = {
	-100.0,
	-500.0,
	-500.0,
	0.0
};

GLuint textures[1];

typedef struct  Avatars {
	float	  AvWeight;
	float	  AvHeight;
	int		  AvLife;

	void kresli_avatar()
	{
		glScalef(0.3f, 0.3f, 0.3f);
		glEnable(GL_TEXTURE_2D);
		MujObjekt->draw();
		glDisable(GL_TEXTURE_2D);
	}
} Avatar;

typedef struct  Bullet {
	bool	  bis;
	float	  b_x;
	float	  b_y;
	float     b_z;
	float	  tg_x;
	float	  tg_y;
	float	  tg_z;
	int		  bNum;
	tVector3  smer;
} Bullet;

Avatar player1;
Bullet bul_pack[21];
Bullet bul_pack2[21];

Bullet bulInit(Bullet bul, int i){
	bul.bis = false;
	bul.bNum = i;
	bul.b_x = 0;
	bul.b_y = 0;
	bul.b_z = 0;
	bul.tg_x = 0;
	bul.tg_y = 0;
	bul.tg_z = 0;
	bul.smer.x = 0;
	bul.smer.y = 0;
	bul.smer.z = 0;
	return bul; 
}
bool collision(Bullet blt){
	float a = abs((blt.b_x + blt.tg_x) - (teziste_x + 512));
	float b = abs((blt.b_y + blt.tg_y) - 148);
	float c = abs((blt.b_z + blt.tg_z) - (teziste_y + 512));
	float hodnota = sqrt( pow(a, 2) + pow(b, 2) + pow(c, 2) );
	if(hodnota <= 33){
		return true;
	}
	else return false;
}
bool collision2(Bullet blt){
	float a = abs((blt.b_x + blt.tg_x) - (camera.mPos.x + 7));
	float b = abs((blt.b_y + blt.tg_y) - c + 2);
	float c = abs((blt.b_z + blt.tg_z) - (camera.mPos.z + 7));
	float hodnota = sqrt( pow(a, 2) + pow(b, 2) + pow(c, 2) );
	if(hodnota <= 33){
		return true;
	}
	else return false;
}
bool bulletCollision(Bullet blt, Bullet blt2){
	float a = abs((blt.b_x + blt.tg_x) - (blt2.b_x + blt2.tg_x));
	float b = abs((blt.b_y + blt.tg_y) - (blt2.b_y + blt2.tg_y));
	float c = abs((blt.b_z + blt.tg_z) - (blt2.b_z + blt2.tg_z));
	float hodnota = sqrt( pow(a, 2) + pow(b, 2) + pow(c, 2) );
	if(hodnota <= 6){
		return true;
	}
	else return false;
}
bool mapCollision(Bullet blt, Mat mapa){
	int a = (int)blt.b_x + (int)blt.tg_x; // X
	float b = blt.b_y + blt.tg_y;  // Vyska Y
	int c = (int)blt.b_z + (int)blt.tg_z;  // Z
	float d = mapa.at<uchar>(a,c); // Udaj terenu
	int hodnota = (int) abs(b - d);
	if(hodnota <= 1){
		return true;
	}
	else return false;
}
tVector3 smerOdHrace(tVector3 start){
	float bul_speed = 2.9;
	tVector3 strela;
	
	strela.x = teziste_x + 512 - start.x+7;
	strela.y = 148 - c + 2;
	strela.z = teziste_y + 512 - start.z+7;

	int velikost = sqrt( pow(strela.x, 2) + pow(strela.y, 2) + pow(strela.z, 2) );

	strela.x = (strela.x * bul_speed) / velikost;
	strela.y = (strela.y * bul_speed) / velikost;
	strela.z = (strela.z * bul_speed) / velikost;

	return strela; 
}
tVector3 smerOdEnemy(tVector3 start){
	float bul_speed = 12.8;
	tVector3 strela;
	
	strela.x = (-1)*(teziste_x + 512 - start.x+7);
	strela.y = (-1)*(148 - c+2);
	strela.z = (-1)*(teziste_y + 512 - start.z+7);

	int velikost = sqrt( pow(strela.x, 2) + pow(strela.y, 2) + pow(strela.z, 2) );

	strela.x = (strela.x * bul_speed) / velikost;
	strela.y = (strela.y * bul_speed) / velikost;
	strela.z = (strela.z * bul_speed) / velikost;

	return strela; 
}
Bullet kresli_bullet(Bullet blt, int clr)
{
	if(clr == 1){
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_G);
	}
	if(clr == 2){
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_R);
	}
	blt.tg_x = blt.tg_x + blt.smer.x;
	blt.tg_y = blt.tg_y + blt.smer.y; 
	blt.tg_z = blt.tg_z + blt.smer.z;
	glTranslatef(blt.tg_x, blt.tg_y, blt.tg_z);
	glScalef(0.8f, 0.8f, 0.8f);
	glTranslatef(3.0f, 6.0f, 0.0f);
	glutSolidSphere(3, 6, 6);
	return blt;
};
int bitmapLoad(int texture, const char *filename)
{
    FILE          *fin;
    int           width, height, bpp=0;
    int           size;
    unsigned char *bitmap;
    unsigned char bmpHeader[54]={0x42, 0x4d,        // magicke cislo souboru BMP
                        0x00, 0x00, 0x00, 0x00,     // velikost souboru
                        0x00, 0x00, 0x00, 0x00,     // rezervovano, vzdy nastaveno na nula
                        0x36, 0x04, 0x00, 0x00,     // data offset=54
                        0x28, 0x00, 0x00, 0x00,     // velikost hlavicky=40
                        0x00, 0x00, 0x00, 0x00,     // sirka obrazku v pixelech=?
                        0x00, 0x00, 0x00, 0x00,     // vyska obrazku v pixelech=?
                        0x01, 0x00,                 // pocet bitovych rovin=1
                        0x08, 0x00,                 // pocet bitu na pixel=24
                        0x00, 0x00, 0x00, 0x00,     // metoda komprimace=nic
                        0x00, 0x00, 0x00, 0x00,     // velikost bitmapy
                        0x00, 0x00, 0x00, 0x00,     // pocet pixelu na metr v horizontalnim smeru
                        0x00, 0x00, 0x00, 0x00,     // pocet pixelu na metr ve vertikalnim smeru
                        0x00, 0x00, 0x00, 0x00,     // pocet pouzitych barev
                        0x00, 0x00, 0x00, 0x00,     // pocet dulezitych barev
    };
    if (!filename) return -1;
    fin=fopen(filename, "rb");
    if (!fin) return -1;                            // otevreni souboru se nezdarilo
    if (fread(bmpHeader, 54, 1, fin)!=1) return -1; // nacist hlavicku BMP souboru
    memcpy(&width, bmpHeader+18, 4);                // sirka obrazku v pixelech
    memcpy(&height, bmpHeader+22, 4);               // vyska obrazku v pixelech
    memcpy(&bpp, bmpHeader+28, 2);                  // pocet bitu na pixel
    if (bpp!=24) return -1;
    size=width*height*3;
    bitmap=(unsigned char *)malloc(size*sizeof(unsigned char));
    if (fread(bitmap, size, sizeof(unsigned char), fin)!=1) return -1;// nacteni rastrovych dat
    fclose(fin);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height,// nacteni textury do GPU
                 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap);
    free(bitmap);
    return 0;
}
int loadTextures(void)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, textures);
    if(bitmapLoad(textures[TEXTURE], "texture32.bmp"))exit(0);
	else return 0;
}
void kresli_teren( void )
{   
	int X = 0, Y = 0;
	int x, y, z;

    for ( X = 0; X < ( vyskova_mapa.cols - STEP_SIZE ); X += STEP_SIZE ){
		for ( Y = 0; Y < ( vyskova_mapa.rows - STEP_SIZE ); Y += STEP_SIZE ){
			//Bottom Left Vertex
			x = X;							
			y = vyskova_mapa.at<uchar>(X,Y);	
			z = Y;
			
			if(y <= 64) glTexCoord2f(0.0, 0.0);
			else if(y > 64 && y <= 128)glTexCoord2f(0.5, 0.5);
			else if(y > 128 && y <= 150)glTexCoord2f(0.0, 0.5);
			else glTexCoord2f(0.5, 0.0);
			glVertex3i(x, y, z);

			//Top Left Vertex
			x = X;										
			y = vyskova_mapa.at<uchar>(X, Y + STEP_SIZE );  
			z = Y + STEP_SIZE ;							
			
			if(y <= 64) glTexCoord2f(0.0, 0.5);
			else if(y <= 128 && y > 64)glTexCoord2f(0.5, 1.0);
			else if(y <= 150 && y > 128)glTexCoord2f(0.0, 1.0);
			else glTexCoord2f(0.5, 0.5);
			glVertex3i(x, y, z);

			//Top Right Vertex
			x = X + STEP_SIZE; 
			y = vyskova_mapa.at<uchar>( X + STEP_SIZE, Y + STEP_SIZE ); 
			z = Y + STEP_SIZE ;

			if(y <= 64) glTexCoord2f(0.5, 0.5);
			else if(y <= 128 && y > 64)glTexCoord2f(1.0, 1.0);
			else if(y <= 150 && y > 128)glTexCoord2f(0.5, 1.0);
			else glTexCoord2f(1.0, 0.5);
			glVertex3i(x, y, z);

			//Bottom Right Vertex
			x = X + STEP_SIZE; 
			y = vyskova_mapa.at<uchar>( X + STEP_SIZE, Y ); 
			z = Y;

			if(y <= 64) glTexCoord2f(0.5, 0.0);
			else if(y <= 128 && y > 64)glTexCoord2f(1.0, 0.5);
			else if(y <= 150 && y > 128)glTexCoord2f(0.5, 0.5);
			else glTexCoord2f(1.0, 0.0);
			glVertex3i(x, y, z);
		}
    }
}
void onTimer(int value)
{
    static int old_framecnt = 0;

    glutTimerFunc(value, onTimer, value); // znovu pustit casovac po value milisekundach
    //printf("%d FPS\n", new_frame_count - old_framecnt );
    framesPs = new_frame_count - old_framecnt;
	old_framecnt = new_frame_count;
}
void kresli_triangly(int num){
	glBegin(GL_TRIANGLES);              
        glColor3f(1.0, 0.0, 0.0); 
		glVertex3i(-15*num, -25*num, -500);            
        
		glColor3f(0.0, 1.0, 0.0); 
		glVertex3i(-30*num, 0, -500);          
        
		glColor3f(1.0, 1.0, 1.0); 
		glVertex3i(0, 0, -500); 

		glColor3f(1.0, 0.0, 0.0); 
		glVertex3i(15*num, -25*num, -500);            
        
		glColor3f(1.0, 1.0, 1.0); 
		glVertex3i(0, 0, -500);          
        
		glColor3f(0.0, 0.0, 1.0); 
		glVertex3i(30*num, 0,-500);

		glColor3f(1.0, 1.0, 1.0); 
		glVertex3i(0, 0, -500);            
        
		glColor3f(0.0, 1.0, 0.0); 
		glVertex3i(-15*num, 25*num, -500);          
        
		glColor3f(0.0, 0.0, 1.0); 
		glVertex3i(15*num, 25*num,-500);
	glEnd();
}
void kresli_terc(Mat & image, int t_x, int t_y){
	//levý horní roh
	line(image, Point2d(t_x - 40, t_y - 25), Point2d(t_x - 30, t_y - 25), Scalar(255,255,0), 2, 8, 0);
	line(image, Point2d(t_x - 40, t_y - 25), Point2d(t_x - 40, t_y - 15), Scalar(255,255,0), 2, 8, 0);
	//pravý horní roh
	line(image, Point2d(t_x + 40, t_y - 25), Point2d(t_x + 30, t_y - 25), Scalar(255,255,0), 2, 8, 0);
	line(image, Point2d(t_x + 40, t_y - 25), Point2d(t_x + 40, t_y - 15), Scalar(255,255,0), 2, 8, 0);
	//levý dolní roh
	line(image, Point2d(t_x - 40, t_y + 25), Point2d(t_x - 30, t_y + 25), Scalar(255,255,0), 2, 8, 0);
	line(image, Point2d(t_x - 40, t_y + 25), Point2d(t_x - 40, t_y + 15), Scalar(255,255,0), 2, 8, 0);
	//pravý dolní roh
	line(image, Point2d(t_x + 40, t_y + 25), Point2d(t_x + 30, t_y + 25), Scalar(255,255,0), 2, 1, 0);
	line(image, Point2d(t_x + 40, t_y + 25), Point2d(t_x + 40, t_y + 15), Scalar(255,255,0), 2, 1, 0);
}
void track_obj(){
	int x = 0, y = 0, soucet_x = 0, soucet_y = 0, pocet = 0;
	Mat image, image_two;
	IplImage* frame = NULL;
	vector<cv::Mat> komponenty;
	frame = cvQueryFrame(cvCaptureFromCAM(0));
	image = frame;
	cvtColor(image, image_two, CV_BGR2HSV);
	inRange(image_two, Scalar(145, 0, 100), Scalar(175, 255, 255), image_two);   // tøetí hodnoty 100 a 175

	split(image_two, komponenty);	
	for ( y = 0; y < image_two.rows; y++ ){
		for ( x = 0; x < image_two.cols; x++ ){
			if(komponenty[0].at<uchar>(y, x) != 0){
				soucet_x += x;
				soucet_y += y;
				pocet++;
			}
		}
	}
	
	if(pocet != 0)
	{
		teziste_x = soucet_x/pocet;
		teziste_y = soucet_y/pocet;
		kresli_terc(image, teziste_x,teziste_y);
	}

	teziste_x = (teziste_old_x + teziste_x)/2;
	teziste_y = (teziste_old_y + teziste_y)/2;

	teziste_old_x = teziste_x;
	teziste_old_y = teziste_y;

	/*namedWindow("Originál", 0);
	imshow("Originál", image);
	namedWindow("Scéna TWO", 0);
	imshow("Scéna TWO", image_two);*/
	cvReleaseImage(&frame);
}
void nastav2D()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	GLfloat aspect = width / (GLfloat)height;
	GLdouble ortho_width = 1000 * aspect;
	GLdouble ortho_height = 1000;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-width/2, width/2, -height/2, height/2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void nastav3D()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)width / height, 0.1, 2500.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void onResize(int w, int h)
{
    if(h == 0)h = 1;
	float ratio = 1.0 * w / h;
	glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);        
    glLoadIdentity();
	gluPerspective(45.0, ratio, 0.1, 2500);
    width=w; height=h;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void onDisplay(void)
{
	nastav3D();

	//Frame counter one when 720
	if(frame_count == 720){
		frame_count = 1;
	}

	//Shooting of enemy
	if((frame_count > 0) && (frame_count % 72 == 0) && (enemy == true)){	
		if(bul_num2 == 21){
			bul_num2 = 0;
		}else{
			if(bul_pack2[bul_num2].bis == false){
				bul_pack2[bul_num2].bis = true;
				bul_pack2[bul_num2].smer = smerOdEnemy(camera.mPos);
				bul_pack2[bul_num2].b_x = 512.0 + teziste_x;
				bul_pack2[bul_num2].b_y = 148;
				bul_pack2[bul_num2].b_z = 512.0 + teziste_y;
				bul_num2++;
			}else{
				bul_num2++;
			}
		}
	}
	frame_count++;
	
	//Tracking of move by webcam
	track_obj();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();
	glutSetCursor(GLUT_CURSOR_NONE);
	
	//Camera
	gluLookAt(camera.mPos.x,  camera.mPos.y + c,  camera.mPos.z,	
			  camera.mView.x, camera.mView.y + c, camera.mView.z,	
			  camera.mUp.x,   camera.mUp.y,   camera.mUp.z);
	camera.Mouse_Move(640, 480);

	//Player1
	glPushMatrix();
		int a = camera.mView.x;
		int b = camera.mView.z;

		if((a < 1024 && a > 0)&&(b < 1024 && b > 0)){
			if(((player1.AvHeight - vyskova_mapa.at<uchar>(a,b)) > 0) && fall == true){
				c -= player1.AvWeight;
				player1.AvHeight = c;
			}else if(((player1.AvHeight - vyskova_mapa.at<uchar>(a,b)) == 0) && fall == true){
				fall = false;
			}else if(((player1.AvHeight - vyskova_mapa.at<uchar>(a,b)) < 0) && fall == true){
				c -= player1.AvWeight;
				player1.AvHeight = c;
			}
			else{
				c = 1 + (	vyskova_mapa.at<uchar>(a,b)		 +
							vyskova_mapa.at<uchar>(a,b+1)	 + 
							vyskova_mapa.at<uchar>(a,b-1)	 +
							vyskova_mapa.at<uchar>(a+1,b)	 +
							vyskova_mapa.at<uchar>(a+1,b+1)  +
							vyskova_mapa.at<uchar>(a+1,b-1)  +
							vyskova_mapa.at<uchar>(a-1,b)	 +
							vyskova_mapa.at<uchar>(a-1,b+1)	 +
							vyskova_mapa.at<uchar>(a-1,b-1)		)/9;
				player1.AvHeight = c;
				float g = vyskova_mapa.at<uchar>(a+1,b) - vyskova_mapa.at<uchar>(a-1,b);
				float h = sqrt( pow(g,2) + pow((float)STEP_SIZE,2));
				naklon = (sinf(g/h) * 180 / PI);

				float m = vyskova_mapa.at<uchar>(a,b+1) - vyskova_mapa.at<uchar>(a,b-1);
				float n = sqrt( pow(m,2) + pow((float)STEP_SIZE,2));
				naklon2 = (sinf(m/n) * 180 / PI);
			}
		}else{
			c -= player1.AvWeight;
			player1.AvHeight = c;
			fall = true;
		}
		glTranslatef(camera.mView.x, c, camera.mView.z);
		glRotatef(rot_angle,0,2,0);
		glRotatef(naklon,0,0,2);
		glRotatef(naklon2,-2,0,0);
		player1.kresli_avatar();
	glPopMatrix();
	
	//Bullet of player collison with enemy
	for(int i = 0; i < 21; i++){
		if(bul_pack[i].bis == true){
			glPushMatrix();
			glTranslatef(bul_pack[i].b_x, bul_pack[i].b_y, bul_pack[i].b_z);
			bul_pack[i] = kresli_bullet(bul_pack[i], 1);
			glPopMatrix();

			if(collision(bul_pack[i]) == true){
				if(enemy_life > 0){
					enemy_life -= 1;
					bul_pack[i].bis = false;
				}
			}
			if( ((bul_pack[i].b_x + bul_pack[i].tg_x) < 1024) && ((bul_pack[i].b_z + bul_pack[i].tg_z) < 1024) && ((bul_pack[i].b_y + bul_pack[i].tg_y) < 512)){
			}else{
				bul_pack[i].bis = false;
				bul_pack[i] = bulInit(bul_pack[i], i);
			}

		}else{
			continue;
		}
	}
	
	//Light
	glMaterialfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, color_white);
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	
	//Textured ground with sky
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE]);
	glBegin(GL_QUADS);
    kresli_teren();
    glEnd();
	glTranslatef(512.0,0.0,512.0);
	glCullFace(GL_CCW);
	glutSolidSphere(512, 10, 10);
	glCullFace(GL_CW);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//Enemy Quadrosaur not textured
	if(enemy == true){
		glPushMatrix();
		glTranslatef(512.0 + teziste_x, 148.0, 512.0 + teziste_y);
		glScalef(0.5f, 0.5f, 0.5f);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_INT, 0, vertices);	
		if(frame_count <= 240) glRotatef( frame_count*3,0,1,1);
		else if(frame_count <= 480 ) glRotatef( frame_count*3,1,1,0);
		else glRotatef(frame_count*3,1,0,1);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_B);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_BYTE, triangles);
		glPopMatrix();
	}

	//Bullet of enemy collision with player
	for(int j = 0; j < 21; j++){
		if(bul_pack2[j].bis == true){
			glPushMatrix();
			glTranslatef(bul_pack2[j].b_x, bul_pack2[j].b_y, bul_pack2[j].b_z);
			bul_pack2[j] = kresli_bullet(bul_pack2[j], 2);
			glPopMatrix();

			if(collision2(bul_pack2[j]) == true){
				if(player1.AvLife > 0){
					player1.AvLife -= 1;
					bul_pack2[j].bis = false;
				}
			}
			if( ((bul_pack2[j].b_x + bul_pack2[j].tg_x) < 1024) && ((bul_pack2[j].b_z + bul_pack2[j].tg_z) < 1024) && ((bul_pack2[j].b_y + bul_pack2[j].tg_y) < 512)){
			}else{
				bul_pack2[j].bis = false;
				bul_pack2[j] = bulInit(bul_pack2[j], j);
			}

		}else{
			continue;
		}
	}

	//Bullet with bullet collision
	for(int i = 0; i < 21; i++){
		for(int j = 0; j < 21; j++){
			if((bul_pack[i].bis == true) && (bul_pack2[j].bis == true)){
				if(bulletCollision(bul_pack[i],bul_pack2[j]) == true){
					bul_pack[i].bis = false;
					bul_pack2[j].bis = false;
				}
			}
		}
	}

	//Bullets collision with ground
	for(int i = 0; i < 21; i++){
		if(bul_pack[i].bis == true){
			if(mapCollision(bul_pack[i], vyskova_mapa) == true){
				bul_pack[i].bis = false;
				int x = (int)bul_pack[i].b_x + (int)bul_pack[i].tg_x;
				int z = (int)bul_pack[i].b_z + (int)bul_pack[i].tg_z;
				
				for(int k = -2*STEP_SIZE; k < 3*STEP_SIZE; k++){
					for(int l = -2*STEP_SIZE; l < 3*STEP_SIZE; l++){
						if(vyskova_mapa.at<uchar>(x+k,z+l) != 0 || vyskova_mapa.at<uchar>(x+k,z+l) != NULL){
							vyskova_mapa.at<uchar>(x+k,z+l) -= 6;
						}
					}
				}
			}
		}
		if(bul_pack2[i].bis == true){
			if(mapCollision(bul_pack2[i], vyskova_mapa) == true){
				bul_pack2[i].bis = false;
				int x = (int)bul_pack2[i].b_x + (int)bul_pack2[i].tg_x;
				int z = (int)bul_pack2[i].b_z + (int)bul_pack2[i].tg_z;
				
				for(int k = -2*STEP_SIZE; k < 3*STEP_SIZE; k++){
					for(int l = -2*STEP_SIZE; l < 3*STEP_SIZE; l++){
						if(vyskova_mapa.at<uchar>(x+k,z+l) != 0 || vyskova_mapa.at<uchar>(x+k,z+l) != NULL){
							vyskova_mapa.at<uchar>(x+k,z+l) -= 6;
						}
					}
				}
			}	
		}
	}

	//Rendering 2D API, life of enemy and player, FPS
	nastav2D();
	glColor3f(0.0f, 0.0f, 1.0f);
	string s,t,u;
	stringstream out,outPlayer, outEnemy, outFps;
	outPlayer << "Bulbasaur: ";
	if(player1.AvLife <= 0){
		outPlayer << "Bulbasaur has fainted";
		gameover = true;
	}else{
		outPlayer << player1.AvLife;
	}
	s = outPlayer.str();
	void * fontb = GLUT_BITMAP_HELVETICA_18;
	void * fonta = GLUT_BITMAP_TIMES_ROMAN_24;
	glRasterPos2d((-width/2)+10,(height/2) -30);
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glutBitmapCharacter(fontb, c);
	}
	
	outEnemy << "Quadrosaur: ";
	if(enemy_life <= 0){
		outEnemy << "Pokemon has fainted";
		enemy = false;
	}else{
		outEnemy << enemy_life;
	}
	t = outEnemy.str();
	glRasterPos2d((-width/2)+10,(height/2) -50);
	for (string::iterator i = t.begin(); i != t.end(); ++i)
	{
		char c = *i;
		glutBitmapCharacter(fontb, c);
	}

	outFps << "Frames per second: ";
	outFps << framesPs;
	u = outFps.str();
	glRasterPos2d((-width/2)+10,(height/2) -70);
	for (string::iterator i = u.begin(); i != u.end(); ++i)
	{
		char c = *i;
		glutBitmapCharacter(fontb, c);
	}
	if(gameover == true){
		out << "GAME OVER (press ESC)";
	}else if(enemy_life == 0){
		out << "BULBASAUR WIN (press ESC)";
	}
	u = out.str();
	glRasterPos2d(-50,0);
	for (string::iterator i = u.begin(); i != u.end(); ++i)
	{
		char c = *i;
		glutBitmapCharacter(fonta, c);
	}
	
	glFlush();	
	glutSwapBuffers();
	new_frame_count++;
}
void onMouseClick(int button, int state, int x, int y)
{
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state==GLUT_DOWN) 
            {
                status = LeftButton;
                mouseX = x;
				mouseY = y;
            }
            else
            {
                status = None;
                oldX = newX;
				oldY = newY;
            }
            break;                    
        default:
            break;
    }
    glutPostRedisplay();
}
void onKeyboard(unsigned char key, int x, int y)
{
    key=(key>'A' && key<='Z') ? key+'a'-'A':key;
    switch (key) {
        case 27:
			glutDestroyWindow(1);
			exit(0);
            break;
        case 'f':
            glutFullScreen();
            break;
        case 'r':
            glutReshapeWindow(640, 480);
            glutPositionWindow(50, 50);
            break;
		case 'w':
			camera.Move_Camera( CAMERASPEED);
			break;
		case 's':
			camera.Move_Camera(-CAMERASPEED);
			break;
		case 'a':
			rot_angle += 6.0;
			break;
		case 'd':
			rot_angle -= 6.0;
			break;
		case 'e':
			camera.Strafe_Camera( CAMERASPEED);
			break;
		case 'q':
			camera.Strafe_Camera(-CAMERASPEED);
			break;
		case 32:
			if(bul_num == 21){
				bul_num = 0;
			}else{
				if(bul_pack[bul_num].bis == false){
					bul_pack[bul_num].bis = true;
					bul_pack[bul_num].smer = smerOdHrace(camera.mPos);
					bul_pack[bul_num].b_x = camera.mPos.x+7;
					bul_pack[bul_num].b_y = c+2;
					bul_pack[bul_num].b_z = camera.mPos.z+7;
					bul_num++;
				}else{
					bul_num++;
				}
			}
			break;
        default:
            break;
    }
}
void inicializace( void )
{
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_POLYGON_SMOOTH );
	glEnable( GL_LINE_SMOOTH );
	glLineWidth( 2.0 );
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, color_white );
	glShadeModel(GL_SMOOTH);
	loadTextures();
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float loc_a = 512.0f;
	camera.Position_Camera(
		loc_a - 7.0f, 8.0f, loc_a,
		loc_a + 7.0f, 8.0f, loc_a,
		0.0f, 1.0f, 0.0f
	);
	player1.AvLife = 5;
	player1.AvWeight = 1;
	player1.AvHeight = c;
	for(int i = 0; i < 21; i++){
		bul_pack[i] = bulInit(bul_pack[i], i);
		bul_pack2[i] = bulInit(bul_pack2[i], i);
	}
	glutFullScreen();
}
void onIdle(void)
{  
	glutPostRedisplay();
}
int main(int argc, char * argv[] )
{
	vyskova_mapa = imread("vysky.png");
	split( vyskova_mapa, komponenty_vyskove_mapy );
	
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(50, 200);
	glutCreateWindow("GLUT");
    if(glewInit() != GLEW_OK){
        printf("GLEW failed\n");
        return EXIT_FAILURE;
    }
    else{printf("GLEW ok\n");}
	inicializace();
	MujObjekt = new Model("C:\\Users\\Jaroslav\\Documents\\Visual Studio 2010\\Projects\\MujProjekt\\MujProjekt\\obj\\bulba.obj");
	glutDisplayFunc(onDisplay);
	glutReshapeFunc(onResize);
	glutKeyboardFunc(onKeyboard);
	glutMouseFunc(onMouseClick);
	glutIdleFunc(onIdle);
	glutTimerFunc(1000, onTimer, 1000);
	glutMainLoop();
	return 1;
}