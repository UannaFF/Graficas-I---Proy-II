#include "Objeto.h"

using namespace std;

#define DEF_floorGridScale	0.1f
#define DEF_floorGridXSteps	10.0f
#define DEF_floorGridZSteps	10.0f
#define DEF_ovnitime 3000;

float moveShip = 0.0, moveOvni = -0.9f;
int flagThrow = 1;
bool* keySpecialStates = new bool[256];
bool wall_collition = false, color = true, playing = false, ovni = false;
int enemies_direction = 1;
int score = 0;

// Lista de enemigos
list<Objeto> enemies;
// Lista de defensas
list<Objeto> defenses;
//Lista de balas
list<pair<float,float>> balitasPlayer;
list<pair<float,float>> balitasEnemy;

GLvoid *font_style = GLUT_BITMAP_TIMES_ROMAN_24;

void initGame(){
	moveShip = 0;
	score = 0;
	// Constantes con las posiciones iniciales de las naves enemigas
	float enemiesX1[7] = {-0.65f, -0.5f, -0.35f, -0.2f, -0.05f, 0.1f, 0.25f};
	float enemiesX2[6] = {-0.6f, -0.45f, -0.3f, -0.15f, 0.0f, 0.15f};
	float enemiesY[5] = {0.3f, 0.4f, 0.5f, 0.6f, 0.7f};
	int health = 0;
	for (int i = 0; i < 5; i++){
		//Si es linea impar se utilizan las primeras coordenadas. Caso contrario se utilizan
		//las segundas
		int x_elegir = (int) (i % 2);
		for (int j = 0; j < 7; j++){
			if ((rand() % 100) > 20)
				health = 1;
			else
				health = 2;
			if (x_elegir == 1){
				enemies.emplace_front(enemiesX1[j], enemiesY[i], health);
			}else{
				if (j>5)
					break;
				enemies.emplace_front(enemiesX2[j], enemiesY[i], health);
			}
		}
	}

	// Posiciones iniciales de las defensas
	float defensesX1[6] = {-0.06f,0.06f,0.54f,0.66f,-0.66f,-0.54f};
	float defensesX2[6] = {-0.12f,0.12f,0.48f,0.72f,-0.72f,-0.48f};
	float defensesX3[12] = {-0.18f,-0.06f,0.06f,0.18f,0.42f,0.54f,0.66f,0.78f,-0.78f,-0.66f,-0.54,-0.42f};
	float  defensesY[3] = {-0.5f,-0.6f,-0.7f};
	health = 1;
	for (int i = 0; i < 3; i++)
	{
		int x_elegir = (int) (i % 3);
		for(int j = 0; j < 12; j++)
		{
			if (x_elegir == 0){
				defenses.emplace_front(defensesX1[j], defensesY[i], health);
				if (j >5)
					break;
			}
			
			if (x_elegir == 1){
				defenses.emplace_front(defensesX2[j], defensesY[i], health);
				if (j >5)
					break;
			}
			if (x_elegir == 2)
				defenses.emplace_front(defensesX3[j], defensesY[i], health);
		}
	}
}

//Funcion de reshape
void changeViewport(int w, int h) {
	float aspectratio;
	glViewport(0, 0, w, h);
	//float aspectRatio = (float)w / (float)h;
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	aspectratio = (float) w / (float) h;
	if (w <= h){
		glOrtho(-1.0, 1.0, -1.0, 1.0/aspectratio, 1.0, 10.0);
	} else {
		glOrtho(-1.0*aspectratio, 1.0*aspectratio, -1.0, 1.0, 1.0, 10.0);
	}
	glMatrixMode(GL_MODELVIEW);
}

//Funcion para imprimir strings en pantalla
void printw (float x, float y, float z, char* format, ...){
    va_list args;   //  Variable argument list
    int len;        // String length
    int i;          //  Iterator
    char * text;    // Text

    //  Initialize a variable argument list
    va_start(args, format);

    //  Return the number of characters in the string referenced the list of arguments.
    // _vscprintf doesn't count terminating '\0' (that's why +1)
    len = _vscprintf(format, args) + 1;

    //  Allocate memory for a string of the specified size
    text = (char *) malloc(len * sizeof(char));

    //  Write formatted output using a pointer to the list of arguments
    vsprintf_s(text, len, format, args);

    //  End using variable argument list
    va_end(args);

    //  Specify the raster position for pixel operations.
    glRasterPos3f (x, y, z);

    //  Draw the characters one by one
    for (i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(font_style, text[i]);

    //  Free the allocated memory for the string
    free(text);
}

//Funcion para dibujar el eje de coordenadas
 void ejesCoordenada(float limite) {
	glLineWidth(2.5);
	glBegin(GL_LINES);
		glColor3f(1.0,0.0,0.0);
		glVertex2f(0,limite);
		glVertex2f(0,-limite);
		glColor3f(0.0,0.0,1.0);
		glVertex2f(limite,0);
		glVertex2f(-limite,0);
	glEnd();
	glLineWidth(1.5);
	int i;
	glColor3f(0.0,1.0,0.0);
	glBegin(GL_LINES);
		for(i = -limite; i <=limite; i++){
			if (i!=0) {	
				if ((i%2)==0){	
					glVertex2f(i,0.4);
					glVertex2f(i,-0.4);
					glVertex2f(0.4,i);
					glVertex2f(-0.4,i);
				}else{
					glVertex2f(i,0.2);
					glVertex2f(i,-0.2);
					glVertex2f(0.2,i);
					glVertex2f(-0.2,i);
				}
			}
		}
	glEnd();
	glLineWidth(1.0);
}

//Funcion para dibujar un circulo
void displayCircle(float cx, float cy, float r, int segments) {
	//glColor3f( 255.0f, 255.0f, 255.0f );
	glBegin(GL_LINE_LOOP);
	for(int ii = 0; ii < segments; ii++) { 
		float theta = 2.0f * 3.1415926f * float(ii) / float(segments);
		float x = r * cosf(theta);
		float y = r * sinf(theta); 
		glVertex2f(x + cx, y + cy);
	} 
	glEnd(); 
}

//Funcion para dibujar un circulo
void drawHalfCircle(float cx, float cy, float r, int segments) {
	//glColor3f( 255.0f, 255.0f, 255.0f );
	glBegin(GL_TRIANGLE_FAN);
	for(int ii = 0; ii < segments; ii++){ 
		float theta = (0.5)*3.1415926f * float(ii) / float(segments);
		float x = r * cosf(theta);
		float y = r * sinf(theta); 
		glVertex2f(x + cx, y + cy);
	} 
	glEnd(); 
}

//Funcion para dibujar un circulo relleno
void drawFilledCircle(float cx, float cy, float r, int segments) {
	//glColor3f( 255.0f, 255.0f, 255.0f );
	glBegin(GL_TRIANGLE_FAN);
	for(int ii = 0; ii < segments; ii++){ 
		float theta = 2.0f * 3.1415926f * float(ii) / float(segments);
		float x = r * cosf(theta);
		float y = r * sinf(theta); 
		glVertex2f(x + cx, y + cy);
	} 
	glEnd(); 
}

//Funcion para dibujar un circulo con segmentos dibujados
void displayStrapCircle(float cx, float cy, float r, int segments) {
	//glColor3f( 255.0f, 255.0f, 255.0f );
	glBegin(GL_LINE_LOOP);
	for(int ii = 0; ii < segments; ii++){ 
		float theta = 2.0f * 3.1415926f * float(ii) / float(segments);
		float x = r * cosf(theta);
		float y = r * sinf(theta); 
		glVertex2f(x + cx, y + cy);
	} 
	glEnd(); 
	for(int ii = 0; ii < segments; ii++){ 
		float theta = 2.0f * 3.1415926f * float(ii) / float(segments);
		float x = r * cosf(theta);
		float y = r * sinf(theta); 
		glBegin(GL_LINES);
			glVertex2f(x + cx, y + cy);
			glVertex2f(0.0f, 0.0f);
		glEnd();
	}
}

//Funcion que dibuja un cuadrado relleno
void drawFilledSquare(){
	glBegin(GL_QUADS);
		glVertex2f(0.09f, -0.02f);
		glVertex2f(0.09f, -0.085f);
		glVertex2f(-0.09f, -0.085f);		
		glVertex2f(-0.09f, -0.02f);
	glEnd();
}

//Funcion que dibuja un enemigo relleno
void drawFilledEnemy(float x, float y){
	glBegin(GL_QUADS);
		glVertex2f(x-0.07f, y-0.02f);
		glVertex2f(x+0.07f, y-0.02f);
		glVertex2f(x+0.07f, y+0.02f);		
		glVertex2f(x-0.07f, y+0.02f);
	glEnd();
}

//Funcion que dibuja la nave
void drawShip(){
	glPushMatrix();
		glColor3f(0.0f, 0.0f, 255.0f);
		drawFilledSquare();
		glPushMatrix();
			glTranslatef(-0.08f,0.02f,0.0f);
			glScalef(0.5f,0.5f,0.5f);
			glRotatef(45.0f,0.0f,0.0f,1.0f);
			drawFilledSquare();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-0.025f,0.0f,0.0f);
			glScalef(0.5f,0.5f,0.5f);
			glRotatef(90.0f,0.0f,0.0f,1.0f);
			drawFilledSquare();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.08f,0.02f,0.0f);
			glScalef(0.5f,0.5f,0.5f);
			glRotatef(-45.f,0.0f,0.0f,1.0f);
			drawFilledSquare();
		glPopMatrix();
	glPopMatrix();
}

//Funcion que dibuja las defensas
void drawDefense(){
	glPushMatrix();
		glColor3f(255.0f, 115.0f, 0.0f);
		glTranslatef(-0.15f,0.0f,0.0f);
		glScalef(0.4f,0.4f,0.4f);
		drawFilledSquare();
		glTranslatef(0.25f,0.0f,0.0f);
		drawFilledSquare();
		glTranslatef(0.25f,0.0f,0.0f);
		drawFilledSquare();
		glTranslatef(0.25f,0.0f,0.0f);
		drawFilledSquare();
		glPushMatrix();
			glTranslatef(-0.62f,0.15f,0.0f);
			drawFilledSquare();
			glTranslatef(0.5f,0.0f,0.0f);
			drawFilledSquare();
			glPushMatrix();
				glTranslatef(-0.4f,0.15f,0.0f);
				drawFilledSquare();
				glTranslatef(0.3f,0.0f,0.0f);
				drawFilledSquare();
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

//Funcion que dibuja una linea de base para el piso
void drawFloor(float x1, float x2, float y) {
	glLineWidth(1.5);
	glColor3f(255.0f, 255.0f, 255.0f);
	glBegin(GL_LINES);
		glVertex3f(x1, y, 0.0);
		glVertex3f(x2, y, 0.0);
	glEnd();
}

//Funcion que dibuja un triangulo con segmentos dibujados
void drawTriangle() {
	glBegin(GL_LINES);
		glColor3f(255.0f, 0.0f, 255.0f);
		glVertex3f(-0.2f, -0.5f, 0.0f);
		glVertex3f(0.2f, -0.5f, 0.0f);

		for(float i=-0.2; i<0.2; i+=0.05) {
			glColor3f( 255.0f, 255.0f, 255.0f );
			glVertex3f(0.0f, 0.0f, 0.0f);
			glColor3f(255.0f, 0.0f, 255.0f);
			glVertex3f(i, -0.5f, 0.0f);
		}
	glEnd();
}


//Funcion para dibujar un circulo
void drawCircle(float cx, float cy, float r, int segments) {
	//glColor3f( 255.0f, 255.0f, 255.0f );
	glBegin(GL_LINE_LOOP);
	for(int ii = 0; ii < segments; ii++) 
	{ 
		float theta = 2.0f * 3.1415926f * float(ii) / float(segments);
		float x = r * cosf(theta);
		float y = r * sinf(theta); 
		glVertex2f(x + cx, y + cy);
	} 
	glEnd(); 
}

//Funcion para dibujar el enemigo superior
void drawOvni(){
	glPushMatrix();
		drawFilledCircle(0.0f,0.0f,0.075f,10);
		glColor3f(0.0f,0.0f,0.0f);
		drawFilledCircle(0.0f,0.035f,0.08f,10);
		glColor3f(1.0f,1.0f,1.0f);
		drawHalfCircle(0.0f,0.02f,0.05f,10);
		drawCircle(0.0f,0.015f,0.06f,10);
		glPushMatrix();
			glColor3f(1.0f,1.0f,1.0f);
			glTranslatef(0.05f,-0.06f,0.0f);
			glRotatef(-45.0f,0.0f,0.0f,1.0f);
			drawFloor(-0.03f,0.03f,0.0);
		glPopMatrix();
		glPushMatrix();
			glColor3f(1.0f,1.0f,1.0f);
			glTranslatef(-0.05f,-0.06f,0.0f);
			glRotatef(45.0f,0.0f,0.0f,1.0f);
			drawFloor(-0.03f,0.03f,0.0);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-0.07f,-0.06f,0.0f);
			glScalef(0.1f,0.3f,1.0f);
			drawFilledSquare();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.07f,-0.06f,0.0f);
			glScalef(0.1f,0.3f,1.0f);
			drawFilledSquare();
		glPopMatrix();
	glPopMatrix();
}


//Funcion que dibuja un rectangulo
void drawRectangle(float sizey, float sizex) {
	float midsizey = sizey / 2, midsizex = sizex / 2;
	glBegin(GL_LINES);
		glVertex3f(-midsizex, -midsizey, 0.0);
		glVertex3f(midsizex, -midsizey, 0.0);

		glVertex3f(-midsizex, -midsizey, 0.0);
		glVertex3f(-midsizex, midsizey, 0.0);

		glVertex3f(midsizex, -midsizey, 0.0);
		glVertex3f(midsizex, midsizey, 0.0);

		glVertex3f(-midsizex, midsizey, 0.0);
		glVertex3f(midsizex, midsizey, 0.0);
	glEnd();
}

//Funcion para mover el ovni
void moveOvniFunc(int value){
	moveOvni += 0.007;
	//si se tiene que mover mas llama al timer de nuevo.
	if(moveOvni < 0.9) glutTimerFunc(17, moveOvniFunc, 4);
	else {
		moveOvni = -0.9;
		ovni = false;
	}
}

//Funcion para mover los enemigos
void moveEnemies(int f){
	if (playing){
		int floor = f;
		bool temp_collition = wall_collition;
		wall_collition = false;
		for (list<Objeto>::iterator it=enemies.begin(); it != enemies.end(); ++it){
			if(temp_collition) {
				it->setY(0.1f);
				if(it->getY() <= -0.8){
					playing = false;
					enemies.clear();
					balitasEnemy.clear();
					balitasPlayer.clear();
					defenses.clear();
					break;
				}
			} else {
				it->setX(0.05f*enemies_direction);
				if (it->getX() >= 0.85f || it->getX() <= -0.85f)
					wall_collition = true;
			}
			for(list<Objeto>::iterator itEn=defenses.begin(); itEn != defenses.end(); ++itEn) {
				float xEn = itEn->getX();
				float yEn = itEn->getY();
				//Cubre todas las colisiones
				if(yEn-0.035 <= it->getY() && it->getY() <= yEn+0.035) {
					//En vez de tomar los extremos horizontales, se toma el centro y se le suma el radio al enemigo.
					if(xEn-0.085 <= it->getX() && it->getX() <= xEn+0.085) {
						defenses.erase(itEn);
						break;
					}
				}
			}
			if ((rand() % 100) < 2)
				balitasEnemy.emplace_back(it->getX(), it->getY());
		}
		// 
		if (temp_collition)
			enemies_direction = -enemies_direction;
		if (wall_collition)
			floor += 1;
		if(playing){
			int vel = DEF_EnemyBaseVelocity-(floor*DEF_EnemyIncVelocity);
			if (vel < 50)
				vel = 50;
			glutTimerFunc(vel, moveEnemies, floor);
		}
	}
}

//Funcion para disparar
void moverBalitasPlayer() {
	float xEn = 0.0f, yEn = 0.0f;
	if (playing){
		for (list<pair<float,float>>::iterator it=balitasPlayer.begin(); it != balitasPlayer.end(); ){
			int erase = false;
			(it->second) += 0.05;

			//Colisiones con enemigos
			for(list<Objeto>::iterator itEn=enemies.begin(); itEn != enemies.end(); ++itEn) {
				xEn = itEn->getX();
				yEn = itEn->getY();
				//Cubre todas las colisiones
				if(yEn-0.035 <= (it->second) && (it->second) <= yEn+0.035) {
					//En vez de tomar los extremos horizontales, se toma el centro y se le suma el radio al enemigo.
					if(xEn-0.085 <= (it->first) && (it->first) <= xEn+0.085) {
						if(itEn->reduceHealth() == 0)
							enemies.erase(itEn);
							score += 100;
							if (enemies.size() == 0) {
								playing = false;
								break;
							}
						erase = true;
						//balitasPlayer.remove(make_pair(it->first, it->second));
						break;
					}
				}
			}
			if (!playing){
				enemies.clear();
				balitasEnemy.clear();
				balitasPlayer.clear();
				defenses.clear();
				break;
			}
			//Colision con ovni
			/*
			Faltaaaaa!!! Si, si, si. Mambo mambo mambo (8)!
			*/

			//Colisiones con barrera
			for(list<Objeto>::iterator itDef=defenses.begin(); itDef != defenses.end(); ++itDef) {
				xEn = itDef->getX();
				yEn = itDef->getY();
				//Cubre todas las colisiones
				if(yEn-0.033 <= (it->second) && (it->second) <= yEn+0.033) {
					if(xEn-0.055 <= (it->first) && (it->first) <= xEn+0.055) {
						defenses.erase(itDef);
						score -= 30;
						erase = true;
						break;
					}
				}
			}
			
			//Colisiones con balas enemigas
			for(list<pair<float,float>>::iterator itDef=balitasEnemy.begin(); itDef != balitasEnemy.end(); ++itDef) {
				xEn = itDef->first;
				yEn = itDef->second;
				//Cubre todas las colisiones
				if(yEn-0.03 <= (it->second) && (it->second) <= yEn+0.03) {
					if(xEn-0.03 <= (it->first) && (it->first) <= xEn+0.03) {
						balitasEnemy.erase(itDef);
						erase = true;
						break;
					}
				}
			}


			if(erase || it->second >= 1.0f) {
				if(balitasPlayer.size() == 1) {
					balitasPlayer.erase(it);
					break;
				}  else if(it != balitasPlayer.begin()){
					list<pair<float,float>>::iterator itTemp = it;
					--it;
					balitasPlayer.erase(itTemp);
				} else {
					balitasPlayer.erase(it);
					break;
				}
			} else ++it;
		}
	}
}

void moverBalitasEnemies() {
	float xEn = 0.0f, yEn = 0.0f;
	for (list<pair<float,float>>::iterator it=balitasEnemy.begin(); it != balitasEnemy.end(); ){
		//Hacer las comprobaciones de las colisiones
		int erase = false;
		(it->second) -= 0.01;

		// Colision con defensas
		if (moveShip-0.05f <= (it->first) && moveShip+0.05f >= (it->first) && -0.9f <= (it->second) && -0.8 >= (it->second)){
			playing = false;
			enemies.clear();
			balitasEnemy.clear();
			balitasPlayer.clear();
			defenses.clear();
			break;
		} else {
			for(list<Objeto>::iterator itEn=defenses.begin(); itEn != defenses.end(); ++itEn) {
				xEn = itEn->getX();
				yEn = itEn->getY();
				//Cubre todas las colisiones
				if(yEn-0.033 <= (it->second) && (it->second) <= yEn+0.033) {
					//En vez de tomar los extremos horizontales, se toma el centro y se le suma el radio al enemigo.
					if(xEn-0.055 <= (it->first) && (it->first) <= xEn+0.055) {
						defenses.erase(itEn);
						erase = true;
						break;
					}
				}
			}
			if(erase || (it->second) <= -0.9f) {
				if(balitasEnemy.size() == 1) {
					balitasEnemy.erase(it);
					break;
				} else if(it != balitasEnemy.begin()){
					list<pair<float,float>>::iterator itTemp = it;
					--it;
					balitasEnemy.erase(itTemp);
				} else {
					balitasEnemy.erase(it);
					break;
				}
			} else {
				++it;
			}
		}
	}
}

//Funcion que refresca la pantalla constantemente
void refresh(int value) {
	//color = !color;
	glutPostRedisplay();
	glutTimerFunc(17,refresh,0);
}

//Funcion para hacer aparecer el ovni
void refreshOvni(int value) {
	ovni = true;

	//llama a un timer para que mueva al ovni
	glutTimerFunc(15, moveOvniFunc, 4);
	//llama al ovni despues de cierto tiempo
	glutTimerFunc(7000,refreshOvni,3);
}

//Funcion para las balas
void refreshFuncBalitas(int value) {
	if (playing){
		moverBalitasPlayer();
		moverBalitasEnemies();
		glutTimerFunc(17, refreshFuncBalitas, 1);
	}
}

//Funcion para refrescar el color
void refreshColor(int value) {
	if (playing){
		color = !color;
		glutTimerFunc(500,refreshColor,2);
	}
}

//Funcion para dar permiso de disparo a nave del jugador
void setFlagCanThrow(int value) {
	flagThrow = 1;
}

//Funcion que se encarga de las acciones de las teclas especiales, las flechas.
void specialKeyPressed (int key, int x, int y) {
	//cout << moveShip << endl;
	switch ( key ) {
		case GLUT_KEY_LEFT :
			if(moveShip >= -0.85){
				moveShip -= 0.05;
			}
			break;
		case GLUT_KEY_RIGHT :
			if(moveShip <= 0.85){
				moveShip += 0.05;
			}
			break;
		default :
			break;
	}
}

//Funcion que se encarga de la accion de la barra
void normalKeyPressed (unsigned char key, int x, int y) {
	if (playing) {
		switch ( key ) {
			case ' ' : 
				//Lanza balita
				if(flagThrow) {
					balitasPlayer.emplace_back(moveShip, -0.8);
					flagThrow = 0;
					glutTimerFunc(550, setFlagCanThrow, 1);
				}
				break;
			default :
				break;
		}
	} else {
		if (key == 13){
			initGame();
			glutTimerFunc(DEF_EnemyBaseVelocity, moveEnemies, 0);
			glutTimerFunc(17, refreshFuncBalitas, 1);
			glutTimerFunc(3000, refreshColor, 2);
			glutTimerFunc(2000, refreshOvni, 3);
			playing = true;
		}
	}
}

void drawParalele() {
	//float midsizey = sizey / 2, midsizex = sizex / 2;
	glBegin(GL_LINES);
		glVertex3f(-0.04, -0.05, 0.0);
		glVertex3f(0.04, -0.05, 0.0);

		glVertex3f(-0.04, -0.05, 0.0);
		glVertex3f(-0.02, 0.0, 0.0);

		glVertex3f(0.04, -0.05, 0.0);
		glVertex3f(0.02, 0.0, 0.0);

		glVertex3f(-0.02, 0.0, 0.0);
		glVertex3f(0.02, 0.0, 0.0);
	glEnd();
}

//Funcion para dibujar el fuego de la nave
void drawFire() {
	glBegin(GL_QUADS);
		if(color) glColor3f(1.0f, 0.0f, 0.0f);
		else glColor3f(255.0f, 255.0f, 0.0f);
		glVertex3f(-0.04f, 0.0f, 0.0f);
		glVertex3f(0.04f, 0.0f, 0.0f);
		glVertex3f(0.07f, 0.07f, 0.0f);
		if(color) glColor3f(255.0f, 255.0f, 0.0f);
		else glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.035f, 0.035f, 0.0f);
		glVertex3f(0.0f, 0.08f, 0.0f);
		if(color) glColor3f(1.0f, 0.0f, 0.0f);
		else glColor3f(255.0f, 255.0f, 0.0f);
		glVertex3f(-0.035f, 0.035f, 0.0f);
		glVertex3f(-0.07f, 0.07f, 0.0f);
		//glVertex3f(-0.04f, 0.0f, 0.0f);
	glEnd();
	glBegin(GL_QUADS);
		if(color) glColor3f(1.0f, 0.0f, 0.0f);
		else glColor3f(255.0f, 255.0f, 0.0f);
		glVertex3f(0.04f, 0.0f, 0.0f);
		glVertex3f(-0.04f, 0.0f, 0.0f);
		glVertex3f(-0.07f, 0.07f, 0.0f);
		if(color) glColor3f(255.0f, 255.0f, 0.0f);
		else glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-0.035f, 0.035f, 0.0f);
		glVertex3f(-0.0f, 0.08f, 0.0f);
		if(color) glColor3f(1.0f, 0.0f, 0.0f);
		else glColor3f(255.0f, 255.0f, 0.0f);
		glVertex3f(0.035f, 0.035f, 0.0f);
		glVertex3f(0.07f, 0.07f, 0.0f);
		//glVertex3f(-0.04f, 0.0f, 0.0f);
	glEnd();
}

//Funcion de render
void render(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.0,0.0,5.0,0.0,0.0,0.0,0.0,1.0,0.0);

	//Dibuja la base
	if (playing){
		if(ovni){
			glPushMatrix();
			glTranslatef(moveOvni, 0.85f, 0.0f);
				glColor3f(255.0f, 255.0f, 255.0f);
				drawOvni();
			glPopMatrix();
		}
		glPushMatrix();
			glLineWidth(5.5f);
			glColor3f(0.0f, 0.0f, 255.0f);
			drawRectangle(1.9f, 1.9f);
			glColor3f(0.0f, 255.0f, 255.0f);

			//Dibujando la nave espacial
			glPushMatrix();
				glTranslatef(moveShip,-0.83f,0.0f);
				glPushMatrix();
					glScalef(1.0f, 0.5f, 1.0f);
					glPushMatrix();
						glColor3f(0.0f, 0.0f, 255.0f);
						glLineWidth(1.0f);
						glTranslatef(-0.055f,0.0f,0.0f);
						glRotatef(-20.0f,0.0f,0.0f,1.0f);
						drawHalfCircle(0.0f, 0.0f, 0.1, 5);		
					glPopMatrix();
					glPushMatrix();
						glColor3f(0.0f, 0.0f, 255.0f);
						glLineWidth(1.0f);
						glTranslatef(0.055f,-0.005f,0.0f);
						glRotatef(120.0f,0.0f,0.0f,1.0f);
						drawHalfCircle(0.0f, 0.0f, 0.1, 5);		
					glPopMatrix();

					glPushMatrix();
						glColor3f( 255.0f, 255.0f, 255.0f );
						glLineWidth(1.0f);
						glTranslatef(0.0f,0.0f,0.0f);
						//glRotatef(120.0f,0.0f,0.0f,1.0f);
						drawFilledCircle(0.0f, 0.0f, 0.007f, 5);		
					glPopMatrix();
					//ejesCoordenada(1.0f);
					glPushMatrix();
						glTranslatef(0.03f,-0.05f,0.0f);
						glRotatef(70.0f,0.0f,0.0f,1.0f);
						drawFloor(-0.03, 0.03, 0.0);
					glPopMatrix();

					glPushMatrix();
						glTranslatef(-0.03f,-0.05f,0.0f);
						glRotatef(110.0f,0.0f,0.0f,1.0f);
						drawFloor(-0.03, 0.03, 0.0);
					glPopMatrix();
					glPushMatrix();
						glTranslatef(0.0f,-0.07f,0.0f);
						drawParalele();
					glPopMatrix();
				glPopMatrix();
				if(color){
					glPushMatrix();
						glTranslatef(0.0f,-0.06f,0.0f);
						glRotatef(180.0f,0.0f,0.0f,1.0f);
						glScalef(0.5f, 0.5f, 1.0f);
						drawFire();
					glPopMatrix();
				}

			glPopMatrix();

			//Se encarga de dibujar todas las balitasPlayer que hay en el momento
			glPushMatrix();
				for (list<pair<float,float>>::iterator it=balitasPlayer.begin(); it != balitasPlayer.end(); ++it){
					drawFilledCircle(it->first, it->second, 0.015, 10);
				}
			glPopMatrix();

			//Se encarga de dibujar todas las balitasPlayer que hay en el momento
			glPushMatrix();
				for (list<pair<float,float>>::iterator it=balitasEnemy.begin(); it != balitasEnemy.end(); ++it){
					drawFilledCircle(it->first, it->second, 0.015, 10);
				}
			glPopMatrix();

			//Defensas
			glLineWidth(0.5f);
			glPushMatrix();
				for (list<Objeto>::iterator it=defenses.begin(); it != defenses.end(); ++it){
					glColor3f(245.0f, 184.0f, 0.0f);
					it->drawDefense();
				}
			glPopMatrix();

			//Enemigos
			glLineWidth(1.0f);
			glPushMatrix();
				for (list<Objeto>::iterator it=enemies.begin(); it != enemies.end(); ++it){
					if(it->getHealth() == 1) {
						glColor3f(0.0f, 255.0f, 255.0f);
					} else {
						glColor3f(0.0f, 0.0f, 255.0f);
					}
					it->drawEnemy();
				}
			glPopMatrix();
		glPopMatrix();

		//Print del score
		glColor3f(1.0f, 0.0f, 0.0f);
		printw(0.5f, 0.85f, 0.0f, "Score: %d", score, "text");
	} else {
		glPushMatrix();
			glLineWidth(5.5f);
			glColor3f(0.0f, 0.0f, 255.0f);
			drawRectangle(1.9f, 1.9f);
			glColor3f(0.0f, 255.0f, 255.0f);

			glColor3f(1.0f, 0.0f, 0.0f);
			printw(-0.3f, 0.1f, 0.0f, "PRESIONE ENTER");
			printw(-0.3f, -0.1f, 0.0f, "PARA COMENZAR");
			printw(-0.4f, -0.3f, 0.0f, "Score: %d", score, "text");
		glPopMatrix();
	}
	glutSwapBuffers();
}


int main (int argc, char** argv) {
	srand ( time(NULL) );
	// Inicialización GLUT
	glutInit(&argc, argv);

	//Funcion para refrescar cada cierto tiempo
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(600,600);
	glutCreateWindow("Space Invaders");
	glutDisplayFunc(render);

	glutReshapeFunc(changeViewport);
	glutKeyboardFunc(normalKeyPressed);
	glutSpecialFunc(specialKeyPressed);
	//glutSpecialUpFunc(keySpecialUp);  
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW error");
		return 1;
	}
	// Temporizadores
	glutTimerFunc(17, refresh, 0);
	glutMainLoop();
	return 0;
}