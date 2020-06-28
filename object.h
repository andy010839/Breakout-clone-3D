
#pragma once
#include <gl/glut.h>
#include <math.h>
#define PI 3.1415926

class Object
{
	public:
	float x;		//物件座標位置
	float y;
	float z;

	float dx;	//向量位移
	float dy;
	float dz;

	float colorR;	//RGB色彩
	float colorG;
	float colorB;
	float colorA;	//透明度
	float width;	//寬x 高y 長z
	float height;
	float length;


	Object()		//建構式
	{
		setLocation(0,0,0);
		setColor(0,0,0,1);
		setSize(1,1,1);
	}

	void draw();

	void move()
	{
		x += dx;
		y += dy;
		z += dz;
	}

	void setLocation(float xx,float yy, float zz)
	{
		x = xx;
		y = yy;
		z = zz;
	}
	void setSize(float w,float h, float l)
	{
		width = w;
		height = h;
		length = l;
	}
	void setColor(float r,float g, float b)
	{
		colorR = r;
		colorG = g;
		colorB = b;
	}
	void setColor(float r,float g, float b, float a)
	{
		colorR = r;
		colorG = g;
		colorB = b;
		colorA = a;
	}

	void setMaterial()	//材質貼圖
	{
		GLfloat mat_diffuse[] ={colorR, colorG, colorB, colorA};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	}

	static bool hitTest(Object o1,Object o2)	//兩物件碰撞偵測
	{
		//當 ( 兩個物件的半徑和 > 兩個物件的中點距離 ) 就會發生衝突     
		if( (o1.width + o2.width)/2 > fabs(o1.x - o2.x) && (o1.height  + o2.height)/2 > fabs(o1.y - o2.y) && (o1.length  + o2.length)/2 > fabs(o1.z - o2.z) )
			return true;
		else
			return false;
 	}

	static bool hitSide_X(Object o1,Object o2)	//ｘ碰撞方向判斷
	{
		if( fabs(o1.x - o2.x) / (o1.width + o2.width) >= fabs(o1.y - o2.y) / (o1.height  + o2.height) ||
			fabs(o1.x - o2.x) / (o1.width + o2.width) >= fabs(o1.z - o2.z) / (o1.length  + o2.length))
			return true;	
		else
			return false;
	}
	static bool hitSide_Y(Object o1,Object o2)	//ｙ碰撞方向判斷
	{
		if( fabs(o1.y - o2.y) / (o1.height  + o2.height) >= fabs(o1.x - o2.x) / (o1.width + o2.width) ||
			fabs(o1.y - o2.y) / (o1.height  + o2.height) >= fabs(o1.z - o2.z) / (o1.length  + o2.length))
			return true;
		else
			return false;
	}
	static bool hitSide_Z(Object o1,Object o2)	//ｚ碰撞方向判斷
	{
		if( fabs(o1.z - o2.z) / (o1.length  + o2.length) >= fabs(o1.y - o2.y) / (o1.height  + o2.height) ||
			fabs(o1.z - o2.z) / (o1.length  + o2.length) >= fabs(o1.x - o2.x) / (o1.width + o2.width))
			return true;	
		else
			return false;
	}
};

//板子
class Bar:public Object
{
	public:

	void draw()
	{
		setMaterial();						//材質上色
		glPushMatrix();
			glTranslatef(x,y,z);			//平移至座標(x,y,z)
			glScalef(width,height,length);	//伸縮成數值大小
			glutSolidCube(1);				//畫邊長為1的立方體
		glPopMatrix();
	}
};

//磚塊
class Block:public Object
{
	public:
	int hp;

	void setColorFromHP()
	{
		float alpha = 0.4;
		if(hp == 1)
			setColor(0.0,0.0,0.0, alpha);	//黑
		if(hp == 2)
			setColor(0.5,0.5,1.0, alpha);	//藍
		if(hp == 3)
			setColor(0.4,0.8,1.0, alpha);	//淺藍
		if(hp == 4)
			setColor(1.0,0.9,0.5, alpha);	//皮膚
		if(hp == 5)
			setColor(0.9,0.0,0.0, alpha);	//紅 
		if(hp == 6)
			setColor(1.0,1.0,1.0, alpha);	//白
	}

	void hit()	//碰撞發生的動作
	{
		hp--;
		setColorFromHP();
	}

	bool isDead()	//判斷HP是否>0
	{
		if(hp > 0) 
			return false;
		else
			return true;
	}

	void draw()
	{
		setMaterial();						//材質上色
		glPushMatrix();
			glTranslatef(x,y,z);			//平移至座標(x,y,z)
			glScalef(width,height,length);	//伸縮成數值大小
			glutSolidCube(1);				//畫邊長為1的立方體
		glPopMatrix();
	}
};

//球
class Ball:public Object
{
	public:
	float speed;

	void draw()
	{
		setMaterial();						//材質上色
		glPushMatrix();
			glTranslatef(x,y,z);			//平移至座標(x,y,z)
			glScalef(width,height,length);	//伸縮成數值大小
			glutSolidSphere(1,20,20);			//畫半徑為1的球體 經緯切割度為20
		glPopMatrix();
	}
};

//道具
class Bonus:public Object
{
	public:
	int bonus_type;
	
	void setBonus_Type(int bt) 
	{
		bonus_type = bt;
	}

	void draw()
	{
		float c = 3.14159/180.0;

		setMaterial();						//材質上色
		glPushMatrix();
			glTranslatef(x,y,z);			//平移至座標(x,y,z)
			glScalef(width,height,length);	//伸縮成數值大小
			
			switch(bonus_type) {			
				case 0:	//道具種類為球的速度 三角錐		
				case 1:
				case 2:
					glutSolidTetrahedron();//畫邊長大概是1的三角錐 =w=
					break;
				
				case 3:	//道具種類為球的大小 八面體
				case 4:
				case 5:
					glutSolidOctahedron();	//畫三角形構成的八面體
					break;

				case 6:	//道具種類為板子變長		<-->
					glBegin(GL_TRIANGLE_FAN);//右側三角錐
						glVertex3f( 1.0,  0.0, 0.0);	//右側三角圓錐的頂點
							
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f( 0.5, cos(c*theta)/2, sin(c*theta)/2);	
						}
					glEnd();
					glBegin(GL_TRIANGLE_FAN);
						glVertex3f( 0.5,  0.0, 0.0);	//右側三角圓錐底部圓的中心
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f( 0.5, sin(c*theta)/2, cos(c*theta)/2);	
						}
					glEnd();
					glBegin(GL_POLYGON);//中間的圓柱
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f(-0.5, cos(c*theta)/4, sin(c*theta)/4);
							glVertex3f( 0.5, cos(c*theta)/4, sin(c*theta)/4);
						}
					glEnd();
					glBegin(GL_TRIANGLE_FAN);//左側三角錐
						glVertex3f(-1.0,  0.0, 0.0);	//左側三角圓錐的頂點
					
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f(-0.5, sin(c*theta)/2, cos(c*theta)/2);	
						}
					glEnd();
					glBegin(GL_TRIANGLE_FAN);
						glVertex3f(-0.5,  0.0, 0.0);	//左側三角圓錐底部圓的中心
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f(-0.5, cos(c*theta)/2, sin(c*theta)/2);
					}
					glEnd();
					break;

				case 7: //道具種類為板子變短		-><-
					glBegin(GL_TRIANGLE_FAN);//中左側三角錐
						glVertex3f( 0.0, 0.0, 0.0);	//中左側三角圓錐的頂點
					
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f(-0.5, cos(c*theta)/2, sin(c*theta)/2);	//橫軸ｙ　直軸ｚ
						}
					glEnd();
					glBegin(GL_TRIANGLE_FAN);
						glVertex3f(-0.5, 0.0, 0.0);	//中左側三角圓錐底部圓的中心
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f(-0.5, sin(c*theta)/2, cos(c*theta)/2);	//橫軸ｚ　直軸ｙ	
						}
					glEnd();
					glBegin(GL_TRIANGLE_FAN);//中右側三角錐
						glVertex3f( 0.0, 0.0, 0.0);	//中右側三角圓錐的頂點
					
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f( 0.5, sin(c*theta)/2, cos(c*theta)/2);	//橫軸ｚ　直軸ｙ
						}
					glEnd();
					glBegin(GL_TRIANGLE_FAN);
						glVertex3f( 0.5, 0.0, 0.0);	//中右側三角圓錐底部圓的中心
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f( 0.5, cos(c*theta)/2, sin(c*theta)/2);	//橫軸ｙ　直軸ｚ
						}
					glEnd();
					glBegin(GL_POLYGON);//左側的圓柱
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f(-0.5, cos(c*theta)/4, sin(c*theta)/4);
							glVertex3f(-1.0, cos(c*theta)/4, sin(c*theta)/4);
						}
					glEnd();
					glBegin(GL_TRIANGLE_FAN);
						glVertex3f(-1.0, 0.0, 0.0);	//左側的圓柱底部圓的中心
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f(-1.0, sin(c*theta)/4, cos(c*theta)/4);	//橫軸ｚ　直軸ｙ
						}
					glEnd();
					glBegin(GL_POLYGON);//右側的圓柱
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f( 0.5, cos(c*theta)/4, sin(c*theta)/4);
							glVertex3f( 1.0, cos(c*theta)/4, sin(c*theta)/4);
						}
					glEnd();
					glBegin(GL_TRIANGLE_FAN);
						glVertex3f( 1.0, 0.0, 0.0);	//右側的圓柱底部圓的中心
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f( 1.0, cos(c*theta)/4, sin(c*theta)/4);	//橫軸ｙ　直軸ｚ
						}
					glEnd();
					break;

				case 8:	//道具種類為球的數量增加 茶壺=w="
					glutSolidIcosahedron();
					break;

			}
		glPopMatrix();
	}
};
