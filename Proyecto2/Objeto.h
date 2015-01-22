#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <list>
#include <ctime>

#define DEF_EnemyBaseVelocity 450
#define DEF_EnemyIncVelocity 60


class Objeto
{
private:
	float x, y;
	int health;
public:
	Objeto(void);
	~Objeto(void);
	Objeto(float, float, int);
	void setX(float);
	void setY(float);
	void setHealth(int);
	int reduceHealth();
	float getX();
	float getY();
	int getHealth();
	void drawEnemy();
	void drawDefense();
};

