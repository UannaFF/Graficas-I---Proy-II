#include "Objeto.h"

	
Objeto::Objeto(void)
{
}


Objeto::~Objeto(void)
{
}

Objeto::Objeto(float coord_x, float coord_y, int h): x(coord_x), y(coord_y), health(h) {}

void Objeto::setX(float coord_x){
	x = x + coord_x;
}

void Objeto::setY(float coord_y){
	y = y - coord_y;
}

void Objeto::setHealth(int h){
	health = h;
}

float Objeto::getX(){
	return x;
}

float Objeto::getY(){
	return y;
}

int Objeto::getHealth(){
	return health;
}

int Objeto::reduceHealth() {
	--health;
	return health;
}

void Objeto::drawEnemy(){
	glBegin(GL_QUADS);
		glVertex2f(x-0.05f, y-0.02f);
		glVertex2f(x+0.05f, y-0.02f);
		glVertex2f(x+0.05f, y+0.02f);		
		glVertex2f(x-0.05f, y+0.02f);
	glEnd();
}

void Objeto::drawDefense(){
	glBegin(GL_QUADS);
		glVertex2f(x-0.04f, y-0.018f);
		glVertex2f(x+0.04f, y-0.018f);
		glVertex2f(x+0.04f, y+0.018f);		
		glVertex2f(x-0.04f, y+0.018f);
	glEnd();
}