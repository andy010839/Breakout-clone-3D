
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

//磚塊
vector<Block> blocks;
//球
vector<Ball> balls;
//板子
Bar bar;
//道具
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

//牆壁色指定
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
//函式宣告
void draw_walls(void);		 //畫牆
void recover_wallColor(void);//復原牆的顏色透明度
void Collision_Ball(void);	 //球的碰撞偵測
void Collision_Bonus(void);  //道具碰撞偵測
void Generate_Bonus(Ball i_ball,Block i_block);//產生道具

//鍵盤控制
bool keyW;
bool keyS;
bool keyA;
bool keyD;
bool keyUp;
bool keyDown;
bool keyLeft;
bool keyRight;
bool keyF1_BallAmount;	//球無限模式
bool keyF2_BonusMode;	//道具模式
bool keyF3_LightMode;	//光源模式

//視角控制
float cameraAtX;
float cameraAtY;
float cameraAtZ;
float lookAtX;
float lookAtY;
float lookAtZ;
float moveX;
float moveY;

//讀檔
void loadMap()
{
	//讀取檔案
	FILE *fp;
	char filename[30] = "map4.txt";
	fp= fopen(filename, "r");
	
	char c;	//作為存放map檔的字元
	float block_location_x = -frame_size_x;		//畫面左上的ｘ座標
	float block_location_y =  frame_size_y;		//畫面左上的ｙ座標
	float block_location_z = -frame_size_z + 2;	//畫面最深的ｚ座標
	
	if(!blocks.empty())								//如果磚塊不是空的
		blocks.erase(blocks.begin(),blocks.end());	//刪掉所有磚塊才重畫

	while( (c = fgetc(fp))!=EOF ) //一次讀取一個字元
	{
		//設定每一個磚塊的資訊
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
		block_location_x += 1;	//無論是否有畫磚塊 均把ｘ座標移至下一個磚塊起始處	

		if(c == '\n')	//每讀了10次字元就要切換到下一行
		{
			block_location_x  = -frame_size_x;	//重設ｘ座標
			block_location_y -=  1; //到下一排磚塊中心點ｙ座標
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
	srand ( time(NULL) );	//打亂隨機

	//鍵盤設定
	keyW = false;
	keyS = false;
	keyA = false;
	keyD = false;
	keyUp = false;
	keyDown = false;
	keyLeft = false;
	keyRight = false;
	keyF1_BallAmount = false;	//預設關閉球無限模式
	keyF2_BonusMode = true;		//預設開啟道具模式
	keyF3_LightMode = true;		//預設為一道光源

	//物件設定
	//磚塊
	loadMap();
	
	//球
	if(!balls.empty())									//如果球不是零個
		balls.erase(balls.begin(),balls.end());			//先設成零個
	Ball _ball;
	_ball.speed = 0.3;	//球的初速
	_ball.dx =  sin(60 * PI/180)*cos(30 * PI/180) * _ball.speed;	//以三角函數方式改變dx dy dz變量
	_ball.dy =  sin(30 * PI/180)				  * _ball.speed;
	_ball.dz = -cos(60 * PI/180)*cos(30 * PI/180) * _ball.speed;
	_ball.setLocation( 0, 0, -5);
//	_ball.setLocation( -4, -4, -19);
	_ball.setSize( 0.5, 0.5, 0.5);			
	_ball.setColor(1.0,0.0,0.0,1.0);
	balls.push_back(_ball);

	//板子
	bar.setLocation(0,0,-2);
	bar.setSize(4.0,4.0,0.75);
	bar.setColor(0.8,0.2,1.0,0.5);

	if(!bonuses.empty())							//如果道具不是空的
	bonuses.erase(bonuses.begin(),bonuses.end());	//刪掉所有道具
	
	//背景顏色
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT); 

	//3D用
	glEnable(GL_DEPTH_TEST);
	
	//正面填滿
	glPolygonMode(GL_FRONT, GL_FILL);

	//背面畫線
	glPolygonMode(GL_BACK, GL_LINE);
	
	//混色函數
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);  //啟用
//	glDisable(GL_BLEND); //停用

	//shading方法
	glShadeModel(GL_SMOOTH);

	//光源 
	glEnable(GL_LIGHTING);

	//透視投影
	float rate = (float)600/(float)800;		//畫面視野變了，但內容不變形   
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, rate, 1.0, 500.0);	//90度角投影　以600/800的比例從1~3000的距離調整

	//攝影機視角
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//攝影機設定
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

void draw_walls(void)	//畫牆
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

	recover_wallColor();	//畫完牆後把顏色設定復原
}

void recover_wallColor()	//從不透明復原
{
	wallColor.mat_diffuse_left[3] = 0.3;
	wallColor.mat_diffuse_right[3] = 0.3;
	wallColor.mat_diffuse_top[3] = 0.3;
	wallColor.mat_diffuse_bottom[3] = 0.3;
	wallColor.mat_diffuse_front[3] = 0.3;
}

void Collision_Ball(void)	//球的碰撞偵測 因為全部寫在一起了 所以物件碰撞的順序很重要~ 雖然算過之後....跟分開碰撞的時間複雜度沒差
{
	for(vector<Ball>::iterator i_ball = balls.begin() ; i_ball != balls.end() ; ) {
		
		i_ball -> move();

//深度變化 球的顏色
		float color_z = i_ball -> z;
		if(color_z > 0.0)
			color_z = 0.0;
		float colorFromDepth = fabs( color_z / frame_size_z);
		float near_color = 0.0, far_color = 0.0;
		far_color = colorFromDepth;
		near_color = 1.0 - colorFromDepth;
		i_ball -> setColor(near_color,far_color,0.0,1.0);
		
//碰撞測試 與板子
		float angle_theta = 0.0, angle_delta = 0.0; 
		if( Object::hitTest(*i_ball,bar) ) 
		{
			i_ball -> z = bar.z - (i_ball -> length + i_ball -> length/2);
			i_ball -> dz *= -1;
			//theta角度計算
			angle_theta = (i_ball -> x - bar.x)/(bar.width/2 + (i_ball -> width))*60;
			if(angle_theta > 60)
				angle_theta = 60;
			if(angle_theta < -60)
				angle_theta = -60;
			//alpha角度計算
			angle_delta = (i_ball -> y - bar.y)/(bar.height/2 + (i_ball -> height))*60;
			if(angle_delta > 60)
				angle_delta = 60;
			if(angle_delta < -60)
				angle_delta = -60;
			i_ball -> dx =  sin(angle_theta * PI/180)*cos(angle_delta * PI/180) * i_ball -> speed;	//以三角函數方式改變dx dy dz變量
			i_ball -> dy =	sin(angle_delta * PI/180)							* i_ball -> speed;
			i_ball -> dz = -cos(angle_theta * PI/180)*cos(angle_delta * PI/180) * i_ball -> speed;
			cout << "angle_theta = " << angle_theta << " angle_delta = " << angle_delta << "\n";
			cout << "i_ball -> dx = " << i_ball -> dx << " i_ball -> dy = " << i_ball -> dy << " i_ball -> dz = " << i_ball -> dz << "\n";

			bar.setColor(0.8,0.2,1.0,1.0);
		}	


//碰撞測試 與磚塊
		for(vector<Block>::iterator i_block = blocks.begin() ; i_block != blocks.end() ; ) {	//對每個磚塊	作判定
			if( Object::hitTest(*i_ball,*i_block) ) {
				//球的方向改變
				if( Object::hitSide_X(*i_ball,*i_block))	//x方向碰撞
					i_ball -> dx *= -1;
				if( Object::hitSide_Y(*i_ball,*i_block))	//y方向碰撞
					i_ball -> dy *= -1;
				if( Object::hitSide_Z(*i_ball,*i_block))	//z方向碰撞
					i_ball -> dz *= -1;

				i_block -> hit();
				if(i_block -> isDead()) {
					if(keyF2_BonusMode) 
						Generate_Bonus(*i_ball,*i_block);			//磚塊消失後產生道具
					i_block = blocks.erase(i_block);
				}
				break;
			}
			else
				i_block++;
		}

//碰撞測試 與牆
		if( (i_ball -> x - i_ball -> width) <= -frame_size_x ) {	//左
			i_ball -> x = -frame_size_x + i_ball -> width;	
			i_ball -> dx *= -1;
			wallColor.mat_diffuse_left[3] = 1.0;	//碰撞時不透明化
		}
		if( (i_ball -> x + i_ball -> width) >=  frame_size_x ) {	//右
			i_ball -> x =  frame_size_x - i_ball -> width;
			i_ball -> dx *= -1;
			wallColor.mat_diffuse_right[3] = 1.0;
		}
		if( (i_ball -> y + i_ball -> height) >=  frame_size_y ) {	//上
			i_ball -> y = frame_size_y - i_ball -> height;
			i_ball -> dy *= -1;
			wallColor.mat_diffuse_top[3] = 1.0;
		}
		if( (i_ball -> y - i_ball -> height) <= -frame_size_y ) {	//下
			i_ball -> y = -frame_size_y + i_ball -> height;
			i_ball -> dy *= -1;
			wallColor.mat_diffuse_bottom[3] = 1.0;
		}
		if( (i_ball -> z - i_ball -> length) <= -frame_size_z ) {	//前
			i_ball -> z = -frame_size_z + i_ball -> length;
			i_ball -> dz *= -1;
			wallColor.mat_diffuse_front[3] = 1.0;
		}
		if( (i_ball -> z) >=  8) {									//後 脫離可視畫面後消除
			i_ball = balls.erase(i_ball);
//			i_ball -> z = -(i_ball -> length);
//			i_ball -> dz *= -1;
		}
		else 
			i_ball++;
	}
}

//道具的碰撞偵測
void Collision_Bonus(void)
{
	for(vector<Bonus>::iterator i_bonus = bonuses.begin() ; i_bonus != bonuses.end(); ) {
		if(i_bonus -> z > 7) {	//出界控制
			i_bonus = bonuses.erase(i_bonus);
		}
		else if( Object::hitTest(*i_bonus,bar) ) {
			for(unsigned int i_ball = 0; i_ball < balls.size(); i_ball++) {
				if(i_bonus -> bonus_type == BALL_SPEED_UP && balls[i_ball].speed < 0.4)	//球加速
					balls[i_ball].speed += 0.05;
				if(i_bonus -> bonus_type == BALL_SPEED_DOWN && balls[i_ball].speed > 0.2)//球減速
					balls[i_ball].speed -= 0.05;
				if(i_bonus -> bonus_type == BALL_SPEED_RECOVER)							//球初速
					balls[i_ball].speed = 0.3;
				if(i_bonus -> bonus_type == BALL_SIZE_UP && balls[i_ball].width < 1.25)	//球變大
					balls[i_ball].setSize( balls[i_ball].width + 0.25, balls[i_ball].height + 0.25, balls[i_ball].length + 0.25);
				if(i_bonus -> bonus_type == BALL_SIZE_DOWN && balls[i_ball].width > 0.25)//球變小		
					balls[i_ball].setSize( balls[i_ball].width - 0.25, balls[i_ball].height - 0.25, balls[i_ball].length - 0.25);
				if(i_bonus -> bonus_type == BALL_SIZE_RECOVER)							//球初始大小		
					balls[i_ball].setSize( 0.5, 0.5, 0.5);
			}
			if(i_bonus -> bonus_type == BAR_SIZE_UP && bar.width < 8 )				//板子加長
				bar.setSize( bar.width + 1, bar.height + 1, bar.length);
			if(i_bonus -> bonus_type == BAR_SIZE_DOWN && bar.width > 3)				//板子變短
				bar.setSize( bar.width - 1, bar.height - 1, bar.length);
			if(i_bonus -> bonus_type == BALL_NUMBER_UP && balls.size() < 5) {			//球數增加 最多5個
				Ball new_ball;
				new_ball.speed = (balls.begin() -> speed);	
				new_ball.dx =  sin(60 * PI/180)*cos(30 * PI/180) * new_ball.speed;	//以三角函數方式改變dx dy dz變量
				new_ball.dy =  sin(30 * PI/180)					 * new_ball.speed;
				new_ball.dz = -cos(60 * PI/180)*cos(30 * PI/180) * new_ball.speed;
				new_ball.setLocation(bar.x,bar.y, bar.z - bar.length/2 - balls.begin() -> length);	//新球位置在板子的正前方
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

//產生道具
void Generate_Bonus(Ball i_ball,Block i_block)
{
	//磚塊消失後產生道具
	int bonus_num = rand() % 20 + 1;	//1 ~ 40	可改變這邊讓道具掉落率改變
	if(bonus_num <= 15)	{	//隨機數 <=16 , 建立道具  
		Bonus bb;	
		bb.setLocation(i_block.x, i_block.y, i_block.z);
		bb.setSize(0.5,0.5,0.5);
		bb.dx = 0;
		bb.dy = 0;
		bb.dz = 0.2;
	
		switch(bonus_num) {
			case 1:	//球加速
			case 2:
				bb.setBonus_Type(BALL_SPEED_UP);
				bb.setColor(1.0,0.2,0.8,0.6);	//粉紅
				break;

			case 3:	//球減速
			case 4:
				bb.setBonus_Type(BALL_SPEED_DOWN);
				bb.setColor(1.0,0.0,0.0,0.6);	//紅
				break;
	
			case 5:	//球初速
				bb.setBonus_Type(BALL_SPEED_RECOVER);
				bb.setColor(0.0,1.0,0.0,0.6);	//淺綠
				break;

			case 6:	//球變大
			case 7:
				bb.setBonus_Type(BALL_SIZE_UP);
				bb.setColor(0.0,0.2,1.0,0.6);	//深藍
				break;
	
			case 8:	//球變小
			case 9:
				bb.setBonus_Type(BALL_SIZE_DOWN);
				bb.setColor(0.2,1.0,1.0,0.6);	//淺藍
				break;

			case 10: //球初始大小
				bb.setBonus_Type(BALL_SIZE_RECOVER);
				bb.setColor(0.0,0.2,0.0,0.6);	//深綠
				break;
	
			case 11: //板子加長
			case 12:
				bb.setBonus_Type(BAR_SIZE_UP);
				bb.setColor(0.0,0.2,1.0,0.6);	//深藍
				break;
					
			case 13: //板子變短
			case 14:
				bb.setBonus_Type(BAR_SIZE_DOWN);
				bb.setColor(0.2,1.0,1.0,0.6);	//淺藍
				break;

			case 15: //球的數量增加 
				bb.setBonus_Type(BALL_NUMBER_UP);
				bb.setColor(1.0,1.0,0.0,0.6);	//黃
				break;
		}
		bonuses.push_back(bb);
	}
}

void display(void)
{
	//清除畫面
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//牆壁
	draw_walls();

	//畫磚塊
	for(unsigned int i = 0; i < blocks.size(); i++) {
		blocks[i].draw();	//畫block
	}

	//畫球
	Collision_Ball();
	for(unsigned int i = 0; i < balls.size(); i++) {
		balls[i].draw();	//畫block
	}

	//畫道具
	Collision_Bonus();
	for(unsigned int i = 0; i < bonuses.size(); i++) {
		bonuses[i].move();	//道具移動
		bonuses[i].draw();	
	}
	
	//板子移動
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

	if(bar.x > frame_size_x - bar.width/2)	//板子出界控制
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

	//內建圖形測試
//	glutSolidTeapot( 1.0 );	//是茶壺唷~*
//	glutSolidIcosahedron();	//大概是12面體
//	glutSolidTetrahedron();	//三角錐
//	glutSolidOctahedron();	//八面體
//	glutSolidDodecahedron();//好多面體 24面吧

	//攝影機設定 隨板子移動
	lookAtX = 0;
	lookAtY = 0;
	lookAtZ = 0;
	cameraAtX = bar.x;
	cameraAtY = bar.y + 5;
	cameraAtZ = 8;

	//攝影機視角
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt( cameraAtX, cameraAtY, cameraAtZ, 
			   lookAtX, lookAtY, lookAtZ, 
			   cameraAtX, cameraAtY+1  , cameraAtZ);
	//固定視角測試
//	gluLookAt(  0.0, 0.0, 5.0,	//眼睛位置	
//				0.0, 0.0, 0.0,	//看的點 
//				0.0, 1.0, 5.0);	//向上方向

	//光源 
	if(keyF3_LightMode) {
		GLfloat light_diffuse[] ={1.0, 1.0, 1.0, 1.0};	//光的顏色 
		GLfloat light0_pos[] ={-3.0 , 3.0 , 3.0 , 0};	//一道左上的光源
		glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glEnable(GL_LIGHT0); 
	}
	else {
		GLfloat light_diffuse[] ={1.0, 1.0, 1.0, 1.0};	//光的顏色
		GLfloat light1_pos[] ={ 7.0 , 10.0 , 3.0 , 0};	//四道光源 來自右上 右下 左下 左上
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

	glFlush();			//清除buffer
	//動畫用
	glutSwapBuffers();
}
//GAME OVER畫面
void display_GameOver(void)	
{
	glClear(GL_COLOR_BUFFER_BIT);  //清除畫面

	glFlush();			//清除buffer
	glutSwapBuffers();	//動畫用
}
//GAME CLEAR 畫面	//暫時還沒找出在3D畫面上顯示文字的方式
/*void display_Clear(void)
{
	char clear_words[15] = "GAME CLEAR"; 
	int shift = 0;
	glClear(GL_COLOR_BUFFER_BIT);  //清除畫面

	glColor3f( 0.0, 0.0, 0.0); 
	glRasterPos2i(275, 400);

	for(int i = 0; i < 15; i++) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, clear_words[i]);
		shift += glutBitmapWidth(GLUT_BITMAP_9_BY_15, clear_words[i]);
		glRasterPos2i(275 + shift, 400);
	}

	glFlush();			//清除buffer
	glutSwapBuffers();	//動畫用
}*/
//動畫用
void idle(void)
{
	if(balls.empty()) {	//場上沒球	 遊戲結束
		glutDisplayFunc(display_GameOver);
		keyF1_BallAmount = false;
	}
//	else if(blocks.empty())	//場上沒磚塊 遊戲獲勝
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
		myinit();//初始化
		glutDisplayFunc(display);//重新註冊影格事件
	}

	if(key == 27)	//ESC的ASCLL碼
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
	
	if((key == 'z' || key == 'Z') && keyF1_BallAmount) {	//按z鍵生球
		Ball new_ball;
		new_ball.speed = (balls.begin() -> speed);	
//		new_ball.dx =  sin(60 * PI/180)*cos(30 * PI/180) * new_ball.speed;	//以三角函數方式改變dx dy dz變量
//		new_ball.dy =  sin(30 * PI/180)					 * new_ball.speed;
//		new_ball.dz = -cos(60 * PI/180)*cos(30 * PI/180) * new_ball.speed;
		new_ball.dx = 0;
		new_ball.dy = 0;
		new_ball.dz = -0.3;
		new_ball.setLocation(bar.x,bar.y, bar.z - bar.length/2 - balls.begin() -> length);	//新球位置在板子的正前方
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
    
	//設定畫圖模式
	//非動畫
	//glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); // default, not needed 
    //動畫
	//glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
	//3D動畫
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	
	glutInitWindowSize(600,800); // 600 x 800 pixel window 
    glutInitWindowPosition(0,0); // place window top left on display 
    glutCreateWindow("homework3"); // window title 
    
	//鍵盤事件
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	
	//鍵盤事件-特殊鍵 
	glutSpecialFunc(keyboardS);
	glutSpecialUpFunc(keyboardSUp);
	
	//滑鼠事件
    glutMouseFunc(mouse);
	glutMotionFunc(motion); 

	//影格事件
	glutDisplayFunc(display);
	//動畫
	glutIdleFunc(idle); 
	
    myinit();
  
    glutMainLoop(); // enter event loop 
}