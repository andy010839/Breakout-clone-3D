
#pragma once
#include <gl/glut.h>
#include <math.h>
#define PI 3.1415926

class Object
{
	public:
	float x;		//����y�Ц�m
	float y;
	float z;

	float dx;	//�V�q�첾
	float dy;
	float dz;

	float colorR;	//RGB��m
	float colorG;
	float colorB;
	float colorA;	//�z����
	float width;	//�ex ��y ��z
	float height;
	float length;


	Object()		//�غc��
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

	void setMaterial()	//����K��
	{
		GLfloat mat_diffuse[] ={colorR, colorG, colorB, colorA};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	}

	static bool hitTest(Object o1,Object o2)	//�⪫��I������
	{
		//�� ( ��Ӫ��󪺥b�|�M > ��Ӫ��󪺤��I�Z�� ) �N�|�o�ͽĬ�     
		if( (o1.width + o2.width)/2 > fabs(o1.x - o2.x) && (o1.height  + o2.height)/2 > fabs(o1.y - o2.y) && (o1.length  + o2.length)/2 > fabs(o1.z - o2.z) )
			return true;
		else
			return false;
 	}

	static bool hitSide_X(Object o1,Object o2)	//�A�I����V�P�_
	{
		if( fabs(o1.x - o2.x) / (o1.width + o2.width) >= fabs(o1.y - o2.y) / (o1.height  + o2.height) ||
			fabs(o1.x - o2.x) / (o1.width + o2.width) >= fabs(o1.z - o2.z) / (o1.length  + o2.length))
			return true;	
		else
			return false;
	}
	static bool hitSide_Y(Object o1,Object o2)	//�B�I����V�P�_
	{
		if( fabs(o1.y - o2.y) / (o1.height  + o2.height) >= fabs(o1.x - o2.x) / (o1.width + o2.width) ||
			fabs(o1.y - o2.y) / (o1.height  + o2.height) >= fabs(o1.z - o2.z) / (o1.length  + o2.length))
			return true;
		else
			return false;
	}
	static bool hitSide_Z(Object o1,Object o2)	//�C�I����V�P�_
	{
		if( fabs(o1.z - o2.z) / (o1.length  + o2.length) >= fabs(o1.y - o2.y) / (o1.height  + o2.height) ||
			fabs(o1.z - o2.z) / (o1.length  + o2.length) >= fabs(o1.x - o2.x) / (o1.width + o2.width))
			return true;	
		else
			return false;
	}
};

//�O�l
class Bar:public Object
{
	public:

	void draw()
	{
		setMaterial();						//����W��
		glPushMatrix();
			glTranslatef(x,y,z);			//�����ܮy��(x,y,z)
			glScalef(width,height,length);	//���Y���ƭȤj�p
			glutSolidCube(1);				//�e�����1���ߤ���
		glPopMatrix();
	}
};

//�j��
class Block:public Object
{
	public:
	int hp;

	void setColorFromHP()
	{
		float alpha = 0.4;
		if(hp == 1)
			setColor(0.0,0.0,0.0, alpha);	//��
		if(hp == 2)
			setColor(0.5,0.5,1.0, alpha);	//��
		if(hp == 3)
			setColor(0.4,0.8,1.0, alpha);	//�L��
		if(hp == 4)
			setColor(1.0,0.9,0.5, alpha);	//�ֽ�
		if(hp == 5)
			setColor(0.9,0.0,0.0, alpha);	//�� 
		if(hp == 6)
			setColor(1.0,1.0,1.0, alpha);	//��
	}

	void hit()	//�I���o�ͪ��ʧ@
	{
		hp--;
		setColorFromHP();
	}

	bool isDead()	//�P�_HP�O�_>0
	{
		if(hp > 0) 
			return false;
		else
			return true;
	}

	void draw()
	{
		setMaterial();						//����W��
		glPushMatrix();
			glTranslatef(x,y,z);			//�����ܮy��(x,y,z)
			glScalef(width,height,length);	//���Y���ƭȤj�p
			glutSolidCube(1);				//�e�����1���ߤ���
		glPopMatrix();
	}
};

//�y
class Ball:public Object
{
	public:
	float speed;

	void draw()
	{
		setMaterial();						//����W��
		glPushMatrix();
			glTranslatef(x,y,z);			//�����ܮy��(x,y,z)
			glScalef(width,height,length);	//���Y���ƭȤj�p
			glutSolidSphere(1,20,20);			//�e�b�|��1���y�� �g�n���Ϋ׬�20
		glPopMatrix();
	}
};

//�D��
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

		setMaterial();						//����W��
		glPushMatrix();
			glTranslatef(x,y,z);			//�����ܮy��(x,y,z)
			glScalef(width,height,length);	//���Y���ƭȤj�p
			
			switch(bonus_type) {			
				case 0:	//�D��������y���t�� �T���@		
				case 1:
				case 2:
					glutSolidTetrahedron();//�e����j���O1���T���@ =w=
					break;
				
				case 3:	//�D��������y���j�p �K����
				case 4:
				case 5:
					glutSolidOctahedron();	//�e�T���κc�����K����
					break;

				case 6:	//�D��������O�l�ܪ�		<-->
					glBegin(GL_TRIANGLE_FAN);//�k���T���@
						glVertex3f( 1.0,  0.0, 0.0);	//�k���T�����@�����I
							
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f( 0.5, cos(c*theta)/2, sin(c*theta)/2);	
						}
					glEnd();
					glBegin(GL_TRIANGLE_FAN);
						glVertex3f( 0.5,  0.0, 0.0);	//�k���T�����@�����ꪺ����
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f( 0.5, sin(c*theta)/2, cos(c*theta)/2);	
						}
					glEnd();
					glBegin(GL_POLYGON);//��������W
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f(-0.5, cos(c*theta)/4, sin(c*theta)/4);
							glVertex3f( 0.5, cos(c*theta)/4, sin(c*theta)/4);
						}
					glEnd();
					glBegin(GL_TRIANGLE_FAN);//�����T���@
						glVertex3f(-1.0,  0.0, 0.0);	//�����T�����@�����I
					
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f(-0.5, sin(c*theta)/2, cos(c*theta)/2);	
						}
					glEnd();
					glBegin(GL_TRIANGLE_FAN);
						glVertex3f(-0.5,  0.0, 0.0);	//�����T�����@�����ꪺ����
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f(-0.5, cos(c*theta)/2, sin(c*theta)/2);
					}
					glEnd();
					break;

				case 7: //�D��������O�l�ܵu		-><-
					glBegin(GL_TRIANGLE_FAN);//�������T���@
						glVertex3f( 0.0, 0.0, 0.0);	//�������T�����@�����I
					
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f(-0.5, cos(c*theta)/2, sin(c*theta)/2);	//��b�B�@���b�C
						}
					glEnd();
					glBegin(GL_TRIANGLE_FAN);
						glVertex3f(-0.5, 0.0, 0.0);	//�������T�����@�����ꪺ����
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f(-0.5, sin(c*theta)/2, cos(c*theta)/2);	//��b�C�@���b�B	
						}
					glEnd();
					glBegin(GL_TRIANGLE_FAN);//���k���T���@
						glVertex3f( 0.0, 0.0, 0.0);	//���k���T�����@�����I
					
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f( 0.5, sin(c*theta)/2, cos(c*theta)/2);	//��b�C�@���b�B
						}
					glEnd();
					glBegin(GL_TRIANGLE_FAN);
						glVertex3f( 0.5, 0.0, 0.0);	//���k���T�����@�����ꪺ����
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f( 0.5, cos(c*theta)/2, sin(c*theta)/2);	//��b�B�@���b�C
						}
					glEnd();
					glBegin(GL_POLYGON);//��������W
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f(-0.5, cos(c*theta)/4, sin(c*theta)/4);
							glVertex3f(-1.0, cos(c*theta)/4, sin(c*theta)/4);
						}
					glEnd();
					glBegin(GL_TRIANGLE_FAN);
						glVertex3f(-1.0, 0.0, 0.0);	//��������W�����ꪺ����
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f(-1.0, sin(c*theta)/4, cos(c*theta)/4);	//��b�C�@���b�B
						}
					glEnd();
					glBegin(GL_POLYGON);//�k������W
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f( 0.5, cos(c*theta)/4, sin(c*theta)/4);
							glVertex3f( 1.0, cos(c*theta)/4, sin(c*theta)/4);
						}
					glEnd();
					glBegin(GL_TRIANGLE_FAN);
						glVertex3f( 1.0, 0.0, 0.0);	//�k������W�����ꪺ����
						for(float theta = -180.0; theta <= 180.0; theta+=5) {
							glVertex3f( 1.0, cos(c*theta)/4, sin(c*theta)/4);	//��b�B�@���b�C
						}
					glEnd();
					break;

				case 8:	//�D��������y���ƶq�W�[ ����=w="
					glutSolidIcosahedron();
					break;

			}
		glPopMatrix();
	}
};
