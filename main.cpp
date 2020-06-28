
#include <windows.h>
#include <gl/glut.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <time.h>
#include "object.h"
#define PI 3.1415926

using namespace std;

float frame_size_x = 6;
float frame_size_y = 8;
float frame_size_z = 20;

//�j��
vector<Block> blocks;
//�y
vector<Ball> balls;
//�O�l
Bar bar;
//�D��
vector<Bonus> bonuses;
enum BONUS_TYPE
{
    BALL_SPEED_UP,		//0
    BALL_SPEED_DOWN,	//1
	BALL_SPEED_RECOVER,	//2
    BALL_SIZE_UP,		//3
	BALL_SIZE_DOWN,		//4
    BALL_SIZE_RECOVER,	//5
	BAR_SIZE_UP,		//6
	BAR_SIZE_DOWN,		//7
	BALL_NUMBER_UP,		//8
};

//�������w
struct WALLCOLOR {
	GLfloat mat_diffuse_left[4];
	GLfloat mat_diffuse_right[4];
	GLfloat mat_diffuse_top[4];
	GLfloat mat_diffuse_bottom[4];
	GLfloat mat_diffuse_front[4];
} wallColor = {{0.0, 0.0, 1.0, 0.3},
			   {1.0, 0.0, 1.0, 0.3},
			   {1.0, 0.0, 0.0, 0.3},
			   {0.0, 1.0, 0.0, 0.3},
			   {0.5, 0.5, 0.5, 0.3}};
//�禡�ŧi
void draw_walls(void);		 //�e��
void recover_wallColor(void);//�_�����C��z����
void Collision_Ball(void);	 //�y���I������
void Collision_Bonus(void);  //�D��I������
void Generate_Bonus(Ball i_ball,Block i_block);//���͹D��

//��L����
bool keyW;
bool keyS;
bool keyA;
bool keyD;
bool keyUp;
bool keyDown;
bool keyLeft;
bool keyRight;
bool keyF1_BallAmount;	//�y�L���Ҧ�
bool keyF2_BonusMode;	//�D��Ҧ�
bool keyF3_LightMode;	//�����Ҧ�

//��������
float cameraAtX;
float cameraAtY;
float cameraAtZ;
float lookAtX;
float lookAtY;
float lookAtZ;
float moveX;
float moveY;

//Ū��
void loadMap()
{
	//Ū���ɮ�
	FILE *fp;
	char filename[30] = "map4.txt";
	fp= fopen(filename, "r");
	
	char c;	//�@���s��map�ɪ��r��
	float block_location_x = -frame_size_x;		//�e�����W���A�y��
	float block_location_y =  frame_size_y;		//�e�����W���B�y��
	float block_location_z = -frame_size_z + 2;	//�e���̲`���C�y��
	
	if(!blocks.empty())								//�p�G�j�����O�Ū�
		blocks.erase(blocks.begin(),blocks.end());	//�R���Ҧ��j���~���e

	while( (c = fgetc(fp))!=EOF ) //�@��Ū���@�Ӧr��
	{
		//�]�w�C�@�ӿj������T
		if(c >= '1' && c <= '9')
		{
			Block b;
			b.setLocation(block_location_x + 0.5,
						  block_location_y - 0.5,
						  block_location_z + 0.5);
			b.setSize(1,1,1);
			b.hp = c - '0';
			b.setColorFromHP();

			blocks.push_back(b);
		}
		block_location_x += 1;	//�L�׬O�_���e�j�� ����A�y�в��ܤU�@�ӿj���_�l�B	

		if(c == '\n')	//�CŪ�F10���r���N�n������U�@��
		{
			block_location_x  = -frame_size_x;	//���]�A�y��
			block_location_y -=  1; //��U�@�ƿj�������I�B�y��
		}

		if(c == '#')
		{
			block_location_x  = -frame_size_x;
			block_location_y  =  frame_size_y;
			block_location_z +=  1;
		}
	}
	fclose(fp);
}

void myinit()
{
	srand ( time(NULL) );	//�����H��

	//��L�]�w
	keyW = false;
	keyS = false;
	keyA = false;
	keyD = false;
	keyUp = false;
	keyDown = false;
	keyLeft = false;
	keyRight = false;
	keyF1_BallAmount = false;	//�w�]�����y�L���Ҧ�
	keyF2_BonusMode = true;		//�w�]�}�ҹD��Ҧ�
	keyF3_LightMode = true;		//�w�]���@�D����

	//����]�w
	//�j��
	loadMap();
	
	//�y
	if(!balls.empty())									//�p�G�y���O�s��
		balls.erase(balls.begin(),balls.end());			//���]���s��
	Ball _ball;
	_ball.speed = 0.3;	//�y����t
	_ball.dx =  sin(60 * PI/180)*cos(30 * PI/180) * _ball.speed;	//�H�T����Ƥ覡����dx dy dz�ܶq
	_ball.dy =  sin(30 * PI/180)				  * _ball.speed;
	_ball.dz = -cos(60 * PI/180)*cos(30 * PI/180) * _ball.speed;
	_ball.setLocation( 0, 0, -5);
//	_ball.setLocation( -4, -4, -19);
	_ball.setSize( 0.5, 0.5, 0.5);			
	_ball.setColor(1.0,0.0,0.0,1.0);
	balls.push_back(_ball);

	//�O�l
	bar.setLocation(0,0,-2);
	bar.setSize(4.0,4.0,0.75);
	bar.setColor(0.8,0.2,1.0,0.5);

	if(!bonuses.empty())							//�p�G�D�㤣�O�Ū�
	bonuses.erase(bonuses.begin(),bonuses.end());	//�R���Ҧ��D��
	
	//�I���C��
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT); 

	//3D��
	glEnable(GL_DEPTH_TEST);
	
	//������
	glPolygonMode(GL_FRONT, GL_FILL);

	//�I���e�u
	glPolygonMode(GL_BACK, GL_LINE);
	
	//�V����
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);  //�ҥ�
//	glDisable(GL_BLEND); //����

	//shading��k
	glShadeModel(GL_SMOOTH);

	//���� 
	glEnable(GL_LIGHTING);

	//�z����v
	float rate = (float)600/(float)800;		//�e�������ܤF�A�����e���ܧ�   
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, rate, 1.0, 500.0);	//90�ר���v�@�H600/800����ұq1~3000���Z���վ�

	//��v������
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//��v���]�w
	lookAtX = 0;
	lookAtY = 0;
	lookAtZ = 0;
	cameraAtX = bar.x;
	cameraAtY = bar.y + 5;
	cameraAtZ = 8;
	gluLookAt( cameraAtX, cameraAtY, cameraAtZ, 
			   lookAtX, lookAtY, lookAtZ, 
			   cameraAtX, cameraAtY+1  , cameraAtZ);
}

void draw_walls(void)	//�e��
{
	glPushMatrix();
	//left
		glMaterialfv(GL_FRONT, GL_DIFFUSE, wallColor.mat_diffuse_left);
//		glBegin(GL_LINE_LOOP);	
		glBegin(GL_QUADS);
			glVertex3f( -frame_size_x,  frame_size_y,  0);
			glVertex3f( -frame_size_x, -frame_size_y,  0);	
			glVertex3f( -frame_size_x, -frame_size_y, -frame_size_z);
			glVertex3f( -frame_size_x,  frame_size_y, -frame_size_z);
		glEnd();				
	//right
		glMaterialfv(GL_FRONT, GL_DIFFUSE, wallColor.mat_diffuse_right);
//		glBegin(GL_LINE_LOOP);	
		glBegin(GL_QUADS);
			glVertex3f(  frame_size_x,  frame_size_y,  0);
			glVertex3f(  frame_size_x,  frame_size_y, -frame_size_z);
			glVertex3f(  frame_size_x, -frame_size_y, -frame_size_z);
			glVertex3f(  frame_size_x, -frame_size_y,  0);			
		glEnd();
	//top
		glMaterialfv(GL_FRONT, GL_DIFFUSE, wallColor.mat_diffuse_top);
//		glBegin(GL_LINE_LOOP);	
		glBegin(GL_QUADS);
			glVertex3f(  frame_size_x,  frame_size_y,  0);
			glVertex3f( -frame_size_x,  frame_size_y,  0);	
			glVertex3f( -frame_size_x,  frame_size_y, -frame_size_z);
			glVertex3f(  frame_size_x,  frame_size_y, -frame_size_z);
		glEnd();
	//bottom
		glMaterialfv(GL_FRONT, GL_DIFFUSE, wallColor.mat_diffuse_bottom);
//		glBegin(GL_LINE_LOOP);	
		glBegin(GL_QUADS);
			glVertex3f(  frame_size_x, -frame_size_y,  0);
			glVertex3f(  frame_size_x, -frame_size_y, -frame_size_z);
			glVertex3f( -frame_size_x, -frame_size_y, -frame_size_z);
			glVertex3f( -frame_size_x, -frame_size_y,  0);			
		glEnd();
	//back
		glMaterialfv(GL_FRONT, GL_DIFFUSE, wallColor.mat_diffuse_front);
//		glBegin(GL_LINE_LOOP);	
		glBegin(GL_QUADS);	
			glVertex3f(  frame_size_x,  frame_size_y, -frame_size_z);
			glVertex3f( -frame_size_x,  frame_size_y, -frame_size_z);
			glVertex3f( -frame_size_x, -frame_size_y, -frame_size_z);
			glVertex3f(  frame_size_x, -frame_size_y, -frame_size_z);			
		glEnd();
	glPopMatrix();

	recover_wallColor();	//�e�������C��]�w�_��
}

void recover_wallColor()	//�q���z���_��
{
	wallColor.mat_diffuse_left[3] = 0.3;
	wallColor.mat_diffuse_right[3] = 0.3;
	wallColor.mat_diffuse_top[3] = 0.3;
	wallColor.mat_diffuse_bottom[3] = 0.3;
	wallColor.mat_diffuse_front[3] = 0.3;
}

void Collision_Ball(void)	//�y���I������ �]�������g�b�@�_�F �ҥH����I�������ǫܭ��n~ ���M��L����....����}�I�����ɶ������רS�t
{
	for(vector<Ball>::iterator i_ball = balls.begin() ; i_ball != balls.end() ; ) {
		
		i_ball -> move();

//�`���ܤ� �y���C��
		float color_z = i_ball -> z;
		if(color_z > 0.0)
			color_z = 0.0;
		float colorFromDepth = fabs( color_z / frame_size_z);
		float near_color = 0.0, far_color = 0.0;
		far_color = colorFromDepth;
		near_color = 1.0 - colorFromDepth;
		i_ball -> setColor(near_color,far_color,0.0,1.0);
		
//�I������ �P�O�l
		float angle_theta = 0.0, angle_delta = 0.0; 
		if( Object::hitTest(*i_ball,bar) ) 
		{
			i_ball -> z = bar.z - (i_ball -> length + i_ball -> length/2);
			i_ball -> dz *= -1;
			//theta���׭p��
			angle_theta = (i_ball -> x - bar.x)/(bar.width/2 + (i_ball -> width))*60;
			if(angle_theta > 60)
				angle_theta = 60;
			if(angle_theta < -60)
				angle_theta = -60;
			//alpha���׭p��
			angle_delta = (i_ball -> y - bar.y)/(bar.height/2 + (i_ball -> height))*60;
			if(angle_delta > 60)
				angle_delta = 60;
			if(angle_delta < -60)
				angle_delta = -60;
			i_ball -> dx =  sin(angle_theta * PI/180)*cos(angle_delta * PI/180) * i_ball -> speed;	//�H�T����Ƥ覡����dx dy dz�ܶq
			i_ball -> dy =	sin(angle_delta * PI/180)							* i_ball -> speed;
			i_ball -> dz = -cos(angle_theta * PI/180)*cos(angle_delta * PI/180) * i_ball -> speed;
			cout << "angle_theta = " << angle_theta << " angle_delta = " << angle_delta << "\n";
			cout << "i_ball -> dx = " << i_ball -> dx << " i_ball -> dy = " << i_ball -> dy << " i_ball -> dz = " << i_ball -> dz << "\n";

			bar.setColor(0.8,0.2,1.0,1.0);
		}	


//�I������ �P�j��
		for(vector<Block>::iterator i_block = blocks.begin() ; i_block != blocks.end() ; ) {	//��C�ӿj��	�@�P�w
			if( Object::hitTest(*i_ball,*i_block) ) {
				//�y����V����
				if( Object::hitSide_X(*i_ball,*i_block))	//x��V�I��
					i_ball -> dx *= -1;
				if( Object::hitSide_Y(*i_ball,*i_block))	//y��V�I��
					i_ball -> dy *= -1;
				if( Object::hitSide_Z(*i_ball,*i_block))	//z��V�I��
					i_ball -> dz *= -1;

				i_block -> hit();
				if(i_block -> isDead()) {
					if(keyF2_BonusMode) 
						Generate_Bonus(*i_ball,*i_block);			//�j�������Უ�͹D��
					i_block = blocks.erase(i_block);
				}
				break;
			}
			else
				i_block++;
		}

//�I������ �P��
		if( (i_ball -> x - i_ball -> width) <= -frame_size_x ) {	//��
			i_ball -> x = -frame_size_x + i_ball -> width;	
			i_ball -> dx *= -1;
			wallColor.mat_diffuse_left[3] = 1.0;	//�I���ɤ��z����
		}
		if( (i_ball -> x + i_ball -> width) >=  frame_size_x ) {	//�k
			i_ball -> x =  frame_size_x - i_ball -> width;
			i_ball -> dx *= -1;
			wallColor.mat_diffuse_right[3] = 1.0;
		}
		if( (i_ball -> y + i_ball -> height) >=  frame_size_y ) {	//�W
			i_ball -> y = frame_size_y - i_ball -> height;
			i_ball -> dy *= -1;
			wallColor.mat_diffuse_top[3] = 1.0;
		}
		if( (i_ball -> y - i_ball -> height) <= -frame_size_y ) {	//�U
			i_ball -> y = -frame_size_y + i_ball -> height;
			i_ball -> dy *= -1;
			wallColor.mat_diffuse_bottom[3] = 1.0;
		}
		if( (i_ball -> z - i_ball -> length) <= -frame_size_z ) {	//�e
			i_ball -> z = -frame_size_z + i_ball -> length;
			i_ball -> dz *= -1;
			wallColor.mat_diffuse_front[3] = 1.0;
		}
		if( (i_ball -> z) >=  8) {									//�� �����i���e�������
			i_ball = balls.erase(i_ball);
//			i_ball -> z = -(i_ball -> length);
//			i_ball -> dz *= -1;
		}
		else 
			i_ball++;
	}
}

//�D�㪺�I������
void Collision_Bonus(void)
{
	for(vector<Bonus>::iterator i_bonus = bonuses.begin() ; i_bonus != bonuses.end(); ) {
		if(i_bonus -> z > 7) {	//�X�ɱ���
			i_bonus = bonuses.erase(i_bonus);
		}
		else if( Object::hitTest(*i_bonus,bar) ) {
			for(unsigned int i_ball = 0; i_ball < balls.size(); i_ball++) {
				if(i_bonus -> bonus_type == BALL_SPEED_UP && balls[i_ball].speed < 0.4)	//�y�[�t
					balls[i_ball].speed += 0.05;
				if(i_bonus -> bonus_type == BALL_SPEED_DOWN && balls[i_ball].speed > 0.2)//�y��t
					balls[i_ball].speed -= 0.05;
				if(i_bonus -> bonus_type == BALL_SPEED_RECOVER)							//�y��t
					balls[i_ball].speed = 0.3;
				if(i_bonus -> bonus_type == BALL_SIZE_UP && balls[i_ball].width < 1.25)	//�y�ܤj
					balls[i_ball].setSize( balls[i_ball].width + 0.25, balls[i_ball].height + 0.25, balls[i_ball].length + 0.25);
				if(i_bonus -> bonus_type == BALL_SIZE_DOWN && balls[i_ball].width > 0.25)//�y�ܤp		
					balls[i_ball].setSize( balls[i_ball].width - 0.25, balls[i_ball].height - 0.25, balls[i_ball].length - 0.25);
				if(i_bonus -> bonus_type == BALL_SIZE_RECOVER)							//�y��l�j�p		
					balls[i_ball].setSize( 0.5, 0.5, 0.5);
			}
			if(i_bonus -> bonus_type == BAR_SIZE_UP && bar.width < 8 )				//�O�l�[��
				bar.setSize( bar.width + 1, bar.height + 1, bar.length);
			if(i_bonus -> bonus_type == BAR_SIZE_DOWN && bar.width > 3)				//�O�l�ܵu
				bar.setSize( bar.width - 1, bar.height - 1, bar.length);
			if(i_bonus -> bonus_type == BALL_NUMBER_UP && balls.size() < 5) {			//�y�ƼW�[ �̦h5��
				Ball new_ball;
				new_ball.speed = (balls.begin() -> speed);	
				new_ball.dx =  sin(60 * PI/180)*cos(30 * PI/180) * new_ball.speed;	//�H�T����Ƥ覡����dx dy dz�ܶq
				new_ball.dy =  sin(30 * PI/180)					 * new_ball.speed;
				new_ball.dz = -cos(60 * PI/180)*cos(30 * PI/180) * new_ball.speed;
				new_ball.setLocation(bar.x,bar.y, bar.z - bar.length/2 - balls.begin() -> length);	//�s�y��m�b�O�l�����e��
				new_ball.setSize( (balls.begin() -> width) , (balls.begin() -> height) ,balls.begin() -> length);			
				new_ball.setColor(1.0,0.0,0.0,1.0);
				balls.push_back(new_ball);
			}
			i_bonus = bonuses.erase(i_bonus);
		}
		else
			i_bonus++;
	}
}

//���͹D��
void Generate_Bonus(Ball i_ball,Block i_block)
{
	//�j�������Უ�͹D��
	int bonus_num = rand() % 20 + 1;	//1 ~ 40	�i���ܳo�����D�㱼���v����
	if(bonus_num <= 15)	{	//�H���� <=16 , �إ߹D��  
		Bonus bb;	
		bb.setLocation(i_block.x, i_block.y, i_block.z);
		bb.setSize(0.5,0.5,0.5);
		bb.dx = 0;
		bb.dy = 0;
		bb.dz = 0.2;
	
		switch(bonus_num) {
			case 1:	//�y�[�t
			case 2:
				bb.setBonus_Type(BALL_SPEED_UP);
				bb.setColor(1.0,0.2,0.8,0.6);	//����
				break;

			case 3:	//�y��t
			case 4:
				bb.setBonus_Type(BALL_SPEED_DOWN);
				bb.setColor(1.0,0.0,0.0,0.6);	//��
				break;
	
			case 5:	//�y��t
				bb.setBonus_Type(BALL_SPEED_RECOVER);
				bb.setColor(0.0,1.0,0.0,0.6);	//�L��
				break;

			case 6:	//�y�ܤj
			case 7:
				bb.setBonus_Type(BALL_SIZE_UP);
				bb.setColor(0.0,0.2,1.0,0.6);	//�`��
				break;
	
			case 8:	//�y�ܤp
			case 9:
				bb.setBonus_Type(BALL_SIZE_DOWN);
				bb.setColor(0.2,1.0,1.0,0.6);	//�L��
				break;

			case 10: //�y��l�j�p
				bb.setBonus_Type(BALL_SIZE_RECOVER);
				bb.setColor(0.0,0.2,0.0,0.6);	//�`��
				break;
	
			case 11: //�O�l�[��
			case 12:
				bb.setBonus_Type(BAR_SIZE_UP);
				bb.setColor(0.0,0.2,1.0,0.6);	//�`��
				break;
					
			case 13: //�O�l�ܵu
			case 14:
				bb.setBonus_Type(BAR_SIZE_DOWN);
				bb.setColor(0.2,1.0,1.0,0.6);	//�L��
				break;

			case 15: //�y���ƶq�W�[ 
				bb.setBonus_Type(BALL_NUMBER_UP);
				bb.setColor(1.0,1.0,0.0,0.6);	//��
				break;
		}
		bonuses.push_back(bb);
	}
}

void display(void)
{
	//�M���e��
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//���
	draw_walls();

	//�e�j��
	for(unsigned int i = 0; i < blocks.size(); i++) {
		blocks[i].draw();	//�eblock
	}

	//�e�y
	Collision_Ball();
	for(unsigned int i = 0; i < balls.size(); i++) {
		balls[i].draw();	//�eblock
	}

	//�e�D��
	Collision_Bonus();
	for(unsigned int i = 0; i < bonuses.size(); i++) {
		bonuses[i].move();	//�D�㲾��
		bonuses[i].draw();	
	}
	
	//�O�l����
	if(bar.x + bar.width/2 >=  6) 
		keyRight = false;
	if(bar.x - bar.width/2 <= -6)
		keyLeft = false;
	if(bar.y + bar.height/2 >=  8) 
		keyUp = false;
	if(bar.y - bar.height/2 <= -8)
		keyDown = false;

	bar.dx = 0;
	bar.dy = 0;
	bar.dz = 0;

	float movement = 0.4;
	if(keyLeft)
		bar.dx = -movement;
	if(keyRight)
		bar.dx = movement;
	if(keyUp)
		bar.dy = movement;
	if(keyDown)
		bar.dy = -movement;

	if(bar.x > frame_size_x - bar.width/2)	//�O�l�X�ɱ���
		bar.x = frame_size_x - bar.width/2;
	if(bar.x < -frame_size_x + bar.width/2)
		bar.x = -frame_size_x + bar.width/2;
	if(bar.y > frame_size_y - bar.height/2)
		bar.y = frame_size_y - bar.height/2;
	if(bar.y < -frame_size_y + bar.height/2)
		bar.y = -frame_size_y + bar.height/2;

	bar.move();
	bar.draw();

	bar.setColor(0.8,0.2,1.0,0.5);

	//���عϧδ���
//	glutSolidTeapot( 1.0 );	//�O������~*
//	glutSolidIcosahedron();	//�j���O12����
//	glutSolidTetrahedron();	//�T���@
//	glutSolidOctahedron();	//�K����
//	glutSolidDodecahedron();//�n�h���� 24���a

	//��v���]�w �H�O�l����
	lookAtX = 0;
	lookAtY = 0;
	lookAtZ = 0;
	cameraAtX = bar.x;
	cameraAtY = bar.y + 5;
	cameraAtZ = 8;

	//��v������
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt( cameraAtX, cameraAtY, cameraAtZ, 
			   lookAtX, lookAtY, lookAtZ, 
			   cameraAtX, cameraAtY+1  , cameraAtZ);
	//�T�w��������
//	gluLookAt(  0.0, 0.0, 5.0,	//������m	
//				0.0, 0.0, 0.0,	//�ݪ��I 
//				0.0, 1.0, 5.0);	//�V�W��V

	//���� 
	if(keyF3_LightMode) {
		GLfloat light_diffuse[] ={1.0, 1.0, 1.0, 1.0};	//�����C�� 
		GLfloat light0_pos[] ={-3.0 , 3.0 , 3.0 , 0};	//�@�D���W������
		glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glEnable(GL_LIGHT0); 
	}
	else {
		GLfloat light_diffuse[] ={1.0, 1.0, 1.0, 1.0};	//�����C��
		GLfloat light1_pos[] ={ 7.0 , 10.0 , 3.0 , 0};	//�|�D���� �Ӧۥk�W �k�U ���U ���W
		glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
		glEnable(GL_LIGHT1); 
		GLfloat light2_pos[] ={ 7.0 ,-10.0 , 3.0 , 0}; 
		glLightfv(GL_LIGHT2, GL_POSITION, light2_pos);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
		glEnable(GL_LIGHT2); 
		GLfloat light3_pos[] ={-7.0 ,-10.0 , 3.0 , 0}; 
		glLightfv(GL_LIGHT3, GL_POSITION, light3_pos);
		glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse);
		glEnable(GL_LIGHT3); 
		GLfloat light4_pos[] ={-7.0 , 10.0 , 3.0 , 0}; 
		glLightfv(GL_LIGHT4, GL_POSITION, light4_pos);
		glLightfv(GL_LIGHT4, GL_DIFFUSE, light_diffuse);
		glEnable(GL_LIGHT4); 
	}

	glFlush();			//�M��buffer
	//�ʵe��
	glutSwapBuffers();
}
//GAME OVER�e��
void display_GameOver(void)	
{
	glClear(GL_COLOR_BUFFER_BIT);  //�M���e��

	glFlush();			//�M��buffer
	glutSwapBuffers();	//�ʵe��
}
//GAME CLEAR �e��	//�Ȯ��٨S��X�b3D�e���W��ܤ�r���覡
/*void display_Clear(void)
{
	char clear_words[15] = "GAME CLEAR"; 
	int shift = 0;
	glClear(GL_COLOR_BUFFER_BIT);  //�M���e��

	glColor3f( 0.0, 0.0, 0.0); 
	glRasterPos2i(275, 400);

	for(int i = 0; i < 15; i++) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, clear_words[i]);
		shift += glutBitmapWidth(GLUT_BITMAP_9_BY_15, clear_words[i]);
		glRasterPos2i(275 + shift, 400);
	}

	glFlush();			//�M��buffer
	glutSwapBuffers();	//�ʵe��
}*/
//�ʵe��
void idle(void)
{
	if(balls.empty()) {	//���W�S�y	 �C������
		glutDisplayFunc(display_GameOver);
		keyF1_BallAmount = false;
	}
//	else if(blocks.empty())	//���W�S�j�� �C�����
//		glutDisplayFunc(display_Clear);
	//printf("\n  idle");
	Sleep(10);
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	//printf("\nkeyboard key=%c, x=%d, y=%d",key,x,y);
	if(key == 'd' || key == 'D')
		keyD = true;
	if(key == 'w' || key == 'W')
		keyW = true;
	if(key == 'a' || key == 'A')
		keyA = true;
	if(key == 's' || key == 'S')
		keyS = true;

	if(key == ' ') {
		myinit();//��l��
		glutDisplayFunc(display);//���s���U�v��ƥ�
	}

	if(key == 27)	//ESC��ASCLL�X
		exit(true);
}

void keyboardUp(unsigned char key, int x, int y)
{
	//printf("\nkeyboardUp key=%c, x=%d, y=%d",key,x,y);
	if(key == 'd' || key == 'D')
		keyD = false;
	if(key == 'w' || key == 'W')
		keyW = false;
	if(key == 'a' || key == 'A')
		keyA = false;
	if(key == 's' || key == 'S')
		keyS = false;
	
	if((key == 'z' || key == 'Z') && keyF1_BallAmount) {	//��z��Ͳy
		Ball new_ball;
		new_ball.speed = (balls.begin() -> speed);	
//		new_ball.dx =  sin(60 * PI/180)*cos(30 * PI/180) * new_ball.speed;	//�H�T����Ƥ覡����dx dy dz�ܶq
//		new_ball.dy =  sin(30 * PI/180)					 * new_ball.speed;
//		new_ball.dz = -cos(60 * PI/180)*cos(30 * PI/180) * new_ball.speed;
		new_ball.dx = 0;
		new_ball.dy = 0;
		new_ball.dz = -0.3;
		new_ball.setLocation(bar.x,bar.y, bar.z - bar.length/2 - balls.begin() -> length);	//�s�y��m�b�O�l�����e��
		new_ball.setSize( (balls.begin() -> width) , (balls.begin() -> height) ,balls.begin() -> length);			
		new_ball.setColor(1.0,0.0,0.0,1.0);
		balls.push_back(new_ball);
	}
}

void keyboardS(int key, int x, int y)
{
	//printf("\nkeyboardS key=%d, x=%d, y=%d",key,x,y);
	if(key == GLUT_KEY_RIGHT)
		keyRight = true;
	if(key == GLUT_KEY_UP)
		keyUp = true;
	if(key == GLUT_KEY_LEFT)
		keyLeft = true;
	if(key == GLUT_KEY_DOWN)
		keyDown = true;
}
void keyboardSUp(int key, int x, int y)
{
	//printf("\nkeyboardSUp key=%d, x=%d, y=%d",key,x,y);
	if(key == GLUT_KEY_RIGHT)
		keyRight = false;
	if(key == GLUT_KEY_UP)
		keyUp = false;
	if(key == GLUT_KEY_LEFT)
		keyLeft = false;
	if(key == GLUT_KEY_DOWN)
		keyDown = false;

	if(key == GLUT_KEY_F1) {
		if(keyF1_BallAmount)
			keyF1_BallAmount = false;
		else
			keyF1_BallAmount = true;
	}

	if(key == GLUT_KEY_F2) {
		if(keyF2_BonusMode)
			keyF2_BonusMode = false;
		else
			keyF2_BonusMode = true;
	}

	if(key == GLUT_KEY_F3) {
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1); 
		glDisable(GL_LIGHT2);
		glDisable(GL_LIGHT3);
		glDisable(GL_LIGHT4);

		if(keyF3_LightMode)
			keyF3_LightMode = false;
		else
			keyF3_LightMode = true;
	}
}

void mouse(int button, int state, int x, int y)
{
	//printf("mouse state = %d\n",state);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{	
		//printf("mouse down state = %d \n",state);
	}
}

void motion (int x, int y) 
{
	//printf("mouse %d, %d\n",x,y);
}

int main(int argc, char** argv)
{

	// standard GLUT initialization 
    glutInit(&argc,argv);
    
	//�]�w�e�ϼҦ�
	//�D�ʵe
	//glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); // default, not needed 
    //�ʵe
	//glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
	//3D�ʵe
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	
	glutInitWindowSize(600,800); // 600 x 800 pixel window 
    glutInitWindowPosition(0,0); // place window top left on display 
    glutCreateWindow("homework3"); // window title 
    
	//��L�ƥ�
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	
	//��L�ƥ�-�S���� 
	glutSpecialFunc(keyboardS);
	glutSpecialUpFunc(keyboardSUp);
	
	//�ƹ��ƥ�
    glutMouseFunc(mouse);
	glutMotionFunc(motion); 

	//�v��ƥ�
	glutDisplayFunc(display);
	//�ʵe
	glutIdleFunc(idle); 
	
    myinit();
  
    glutMainLoop(); // enter event loop 
}