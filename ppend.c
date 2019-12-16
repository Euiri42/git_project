#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h> 	
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#define LEFTEDGE 1
#define RIGHTEDGE 50
#define TOP 1
#define BOTTOM 46
#define MAX 35
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 

struct flight
{
int x,y;
char x_dir, y_dir;
char life; 
char bomb; 
char p; //투사체 딜레이를 줄임
char enemy; // enemy가 1이면 적, 0이면 죽음, 2이면 boss
}; // 날아다닐 적 및 본인

struct shooting
{
	int x,y;
	char y_dir;
	char val;//피격판정할때 사용
}; // 발사체

struct po
{
	int x,y;
	char x_dir,y_dir;
	char val;
};
struct po po;
struct flight flight;
struct shooting shooting[MAX];
struct flight enemy[MAX];

struct shooting e_shooting[MAX];

char difficult = 1;
char stage = 1;

int score;
int timer;

void startgame();
void printhelp();
void drawingmap(); 
void draw(); 
void drawplusthing();
void drawflight();
void drawenemy();
void drawenemy();
void drawshooting();
void e_drawshooting();
void shoot();
void e_shoot();
void scorep();

void clearmap();

void bomb();
void boss();

void enemyspone();
void test(); 
int hp=3;
char buf[100];
void finish_game();

int main()
{
	char i;
	int par;
	pthread_t t1;
	pthread_t t2,t3,t4,t5;
	startgame();

	srand(time(NULL));
	flight.life = 3;
	flight.x = 25;
	flight.y = 45;
	flight.bomb =3;

		pthread_create(&t1,NULL,enemyspone,NULL);
		pthread_create(&t2,NULL,draw,NULL);
		pthread_create(&t3,NULL,test,NULL);
		pthread_create(&t4,NULL,shoot,NULL);
		pthread_create(&t5,NULL,e_shoot,NULL);
		while(i = getch())
		{
			flight.y_dir = 0;
			flight.x_dir = 0;
			if(flight.life == 0||hp==0){
				break;
			}
			if(i == 'w'){
				if((flight.y - 2)== TOP){
					flight.y_dir = 0;}
					else
					flight.y_dir = -1;
			}
			if(i == 's'){
				if((flight.y + 1)==BOTTOM){
					flight.y_dir = 0;}
				else
					flight.y_dir = 1;
			}
			if(i == 'a'){
				if((flight.x - 1)==LEFTEDGE){
					flight.x_dir = 0;}
				else	
					flight.x_dir = -1;
			}
			if(i == 'd'){
				if((flight.x + 1) == RIGHTEDGE){
					flight.x_dir = 0;}
				else
					flight.x_dir = 1;
			}
			if(i == 'b')
			{
				bomb();
			}
		}
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	pthread_join(t3,NULL);
	pthread_join(t4,NULL);
	pthread_join(t5,NULL);
	finish_game();
	endwin();
}

void finish_game()
{
	int sco1[1000],sco2=0;
	FILE *f;
	char tmp[100][100];
	char buf[1000];
	int i,j;
	int temp = 0;
	int cnt=0;
	f = fopen("score.txt","a+");
	fprintf(f,"%d\n",score);
	fclose(f);

	f=fopen("score.txt","r");
	clear();
	fprintf(f,"%d\n",score);
	while(1){
		if(feof(f))break;
		fscanf(f,"%s",tmp[cnt]);
		sco1[cnt] = atoi(tmp[cnt]);
		cnt++;
	}
	fclose(f);
	for(i=0;i<cnt;i++){
		for(j=0;j<cnt;j++){
			if(sco1[j]<sco1[j+1]){
				temp = sco1[j];
				sco1[j]=sco1[j+1];
				sco1[j+1]=temp;
			}
		}
	}
	for(i=0;i<cnt-1;i++){
		sprintf(buf,"%d",i+1);
		move(10+(i*2),10);
		addstr(buf);
		move(10+(i*2),13);
		addch(':');
		sprintf(buf,"%d",sco1[i]);
		move(10+(i*2),15);
		addstr(buf);
	}
	move(5,10);
	addstr("Wait 10 second....");
	refresh();
	sleep(10);

}
void boss()
{
	int i,j;
	int count=0;
	int rndtmp;
	pthread_mutex_lock(&lock);
	attron(COLOR_PAIR(1));
	move(10,20);
	addstr("warning!!");
	pthread_mutex_unlock(&lock);
	usleep(200000);
	pthread_mutex_lock(&lock);
	move(10,20);
	addstr("         ");
	pthread_mutex_unlock(&lock);
	usleep(200000);
	pthread_mutex_lock(&lock);
	move(10,20);
	addstr("warning!!");
	pthread_mutex_unlock(&lock);
	usleep(200000);
	move(10,20);
	addstr("         ");

	pthread_mutex_unlock(&lock);
	for(i=0;i<MAX;i++)
		enemy[i].enemy = 0;
	enemy[0].life = 10*stage;
	enemy[0].enemy = 2;
	enemy[0].x=6;
	enemy[0].y=23;
	enemy[0].y_dir=1;
	while(enemy[0].life != 0){
		if(flight.life==0||hp==0)return;
		timer = 0;
		for(i=0;i<MAX;i++)
		{
			if(shooting[i].val == 1 && shooting[i].y <= enemy[0].x+2 && shooting[i].y >= enemy[0].x-2 && shooting[i].x <= enemy[0].y+2 && shooting[i].x >= enemy[0].y-2)
			{
				shooting[i].val = 0;
				enemy[0].life--;
			}
		}
	}
}

void scorep()
{
	score += 100*difficult*stage;
}

void bomb()
{
	int i;
	int tmp;
	if(flight.bomb > 0){
	for(i = 0;i<MAX;i++)
	{
		if(enemy[0].enemy == 2)
			e_shooting[i].val = 0;
		else{
		if(enemy[i].enemy == 1){
			tmp=rand()%10;
			if(tmp<=2 && po.val != 1){
				po.x=enemy[i].y;
				po.y=enemy[i].x;
				po.x_dir=1;
				po.y_dir=1;
				po.val=1;
			}
			scorep();
		}
		enemy[i].enemy = 0;
		e_shooting[i].val = 0;
		shooting[i].val = 0;
		}
	}
	flight.bomb--;
	}
}

void shoot()
{
	char count = 0;
	char i = 0;
	while(1){
		if(stage==4||hp==0)return;
	count++;
	if(count >= 55 - flight.p * 15)//생성 delay
	{
	count = 0;
	if(i == MAX)
		i = 0;
	shooting[i].x = flight.x;
	shooting[i].y = flight.y-1;
	shooting[i].val = 1;
	shooting[i++].y_dir = -1;
	}
	if(flight.life == 0)
		return;
	usleep(5000);
	}
}
void e_shoot()
{
	char count = 0;
	char i = 0;
	int k;
	int j;
	while(1){
		if(stage==4||hp==0)return;
	count++;
	if(count >= 120/difficult)
	{
	count = 0;
	if(enemy[0].enemy == 2)
	{
	k = rand()%5 + 1;
	if(i<MAX-2&& e_shooting[i].val != 1 && e_shooting[i+1].val != 1 && e_shooting[i+2].val != 2)
		{
			if(k > 2){
			e_shooting[i].x=enemy[0].y;
			e_shooting[i].y=enemy[0].x+3;
			e_shooting[i].val=1;
			e_shooting[i++].y_dir=1;
			e_shooting[i].x=enemy[0].y-2;
			e_shooting[i].y=enemy[0].x+2;
			e_shooting[i].val=1;
			e_shooting[i++].y_dir=1;
			e_shooting[i].x=enemy[0].y+2;
			e_shooting[i].y=enemy[0].x+2;
			e_shooting[i].val=1;
			e_shooting[i++].y_dir=1;
			if(i>=MAX-2)
				i = 0;
			}
		}
	}
	else{
	for(j=0;j<MAX;j++)
	{
		if(i == MAX)
			i = 0;
		k = rand()%5 + 1;
		if(enemy[j].enemy == 1)
		{
		if(k == 1)
		{
			e_shooting[i].x = enemy[j].y;
			e_shooting[i].y = enemy[j].x+1;
			e_shooting[i].val = 1;
			e_shooting[i++].y_dir = 1;
		}
		}
	}
	}
	}
	if(flight.life == 0)
		return;
	usleep(5000);
	}
}


void test()//피격 판정
{
	int i,j;
	int k;
	while(1){
		if(stage==4||hp==0)return;
		if(po.x == flight.x && po.y == flight.y && flight.p != 3&&po.val == 1)
		{
			po.val = 0;
			flight.p++;
			move(po.y,po.x);
			addch(' ');
		}
	for(i = 0; i<MAX; i++){
		for(j=0;j<MAX;j++){
			if(enemy[i].x == shooting[j].y && enemy[i].y == shooting[j].x && enemy[i].enemy == 1&& shooting[j].val == 1)
			{
				k = rand()%40 + 1;
				if(k == 1 && po.val == 0){
					po.x = enemy[i].y;
					po.y = enemy[i].x;
					po.y_dir = 1;
					po.x_dir = 1;
					po.val = 1;
				}
				move(shooting[j].y,shooting[j].x);
				addch(' ');
				enemy[i].enemy = 0;
				shooting[j].x = 0;
				shooting[j].y = 0;
				shooting[j].val = 0;
				move(LINES-1,0);
				scorep();
				break;
			}
		}
	}
	for(i = 0;i<MAX; i++)
	{
		if(flight.x == e_shooting[i].x && flight.y == e_shooting[i].y && e_shooting[i].val == 1)
		{
			move(e_shooting[i].y,e_shooting[i].x);
			addch(' ');
			e_shooting[i].val = 0;
			flight.life--;
			if(flight.life == 0){
				clearmap();
			}
			flight.bomb = 3;
			flight.p = 0;
			move(LINES-1,0);
			break;
		}
	}
for(i =0; i<MAX; i++)
	{
		if(flight.x == enemy[i].y && flight.y == enemy[i].x && enemy[i].enemy == 1)
		{
		move(enemy[i].x,enemy[i].y);
		addch(' ');
		enemy[i].enemy = 0;
		flight.life--;
		if(flight.life == 0)
		{
			clearmap();
		}
		flight.bomb = 3;
		flight.p = 0;
		break;
		}
	}
	if(flight.life == 0)
		return;
	usleep(5000);
	}
}

void drawpo()
{
	static char count = 0;
	count++;
	if(count >= 10){
	count = 0;
	move(po.y,po.x);
	addch(' ');
	po.y += po.y_dir;
	po.x += po.x_dir;
	move(po.y,po.x);
	addch('P');	

	if(po.x + po.x_dir == RIGHTEDGE || po.x + po.x_dir == LEFTEDGE)
		po.x_dir = -po.x_dir;
	if(po.y + po.y_dir == TOP || po.y + po.y_dir == BOTTOM)
		po.y_dir = -po.y_dir;
	}
}

void drawplusthing()
{
	static char count = 0;
	char output[5];
	int i;
	count ++;
	if(count >= 20)
	{
	attron(COLOR_PAIR(1));
	move(10,55);
	addstr("Life: ");
	move(10, 60+(flight.life+1)*2);
	addch(' ');
	attron(COLOR_PAIR(2));
	for(i = 1; i<= flight.life; i++)
	{
	move(10,60+i*2);
	addch('1');
	}

	attron(COLOR_PAIR(1));
	move(15,55);
	addstr("Bomb: ");
	move(15,60+(flight.bomb+1)*2);
	addch(' ');
	attron(COLOR_PAIR(2));
	for(i= 1; i<=flight.bomb; i++)
	{
	move(15,60+i*2);
	addch('2');
	}
	move(LINES-1,0);
	attron(COLOR_PAIR(1));

	move(20,55);
	addstr("Score: ");
	move(20,65);
	sprintf(output,"%d",score);
	addstr(output);

	move(5,55);
	addstr("Stage: ");
	sprintf(output,"%d",stage);
	move(5,65);
	addstr(output);
	move(LINES-1,0);
	count = 0;
	}
}

void drawenemy()
{
	int j;
	static char count;
	char output[10];
	count++;
	if(count == 20){
		if(enemy[0].enemy != 2){
	for(j=0;j<MAX;j++)
	{
		if(enemy[j].x == BOTTOM-2)
			enemy[j].enemy = 0;

		if(enemy[j].enemy==1)
		{	
			attron(COLOR_PAIR(1));
			move(enemy[j].x,enemy[j].y);
			addch(' ');
			enemy[j].x+=enemy[j].x_dir;
			attron(COLOR_PAIR(4));
			move(enemy[j].x,enemy[j].y);
			addch('E');
			attron(COLOR_PAIR(1));
		 }
		 else if(enemy[j].enemy == 0)
		 {
			attron(COLOR_PAIR(1));
			move(enemy[j].x,enemy[j].y);
			addch(' ');
		 }
	}
	}
else if(enemy[0].enemy == 2)
{
		attron(COLOR_PAIR(1));	

		move(enemy[0].x-2,enemy[0].y-1);
		addstr("   ");
		move(enemy[0].x-1,enemy[0].y);
		addch(' ');
		move(enemy[0].x,enemy[0].y-2);
		addstr("     ");

		move(enemy[0].x+1,enemy[0].y-1);
		addstr("   ");
		move(enemy[0].x+2,enemy[0].y);
		addch(' ');
		enemy[0].y+=enemy[0].y_dir;
		
		if(enemy[0].y+4 > RIGHTEDGE){
			enemy[0].y=RIGHTEDGE-4;
			enemy[0].y_dir=-1;
		}
		if(enemy[0].y-4<LEFTEDGE){
			enemy[0].y=LEFTEDGE+4;
			enemy[0].y_dir=1;
		}
		attron(COLOR_PAIR(4));
		move(enemy[0].x-2,enemy[0].y-1);
		addstr("   ");
		move(enemy[0].x-1,enemy[0].y);
		addch(' ');
		move(enemy[0].x,enemy[0].y-2);
		addstr("     ");
		move(enemy[0].x+1,enemy[0].y-1);
		addstr("   ");
		move(enemy[0].x+2,enemy[0].y);
		addch(' ');
		attron(COLOR_PAIR(1));
}
	count = 0;
	}
}

void drawflight()
{
	static char count = 0;
	count++;
	if(count == 4){
	attron(COLOR_PAIR(1));
	move(flight.y,flight.x);
	addch(' ');
	flight.y += flight.y_dir;
	flight.x += flight.x_dir;
	attron(COLOR_PAIR(2));
	move(flight.y,flight.x);
	addch(' ');
	attron(COLOR_PAIR(1));
	flight.x_dir=0;
	flight.y_dir=0;
	count =0;
	}
}

void drawshooting()
{
	static char delay = 0;
	char i;
	delay++;
	if(delay == 6) //발사체 속도
	{
	for(i=0; i<MAX; i++)
	{
		move(shooting[i].y,shooting[i].x);
		if(shooting[i].val == 1){
		if(shooting[i].y == TOP+1 || shooting[i].y == BOTTOM-1)
		{
			shooting[i].val = 0;
		}
		else
		{
			addch(' ');
			shooting[i].y += shooting[i].y_dir;
			move(shooting[i].y,shooting[i].x);
			addch('^');
		}
		}
		else
			addch(' ');
	}
	delay = 0;
	}
}

void e_drawshooting()
{
	static char delay = 0;
	char i;
	delay++;
	if(delay == 9)
	{
	for(i=0;i<MAX;i++)
	{
		move(e_shooting[i].y,e_shooting[i].x);
		if(e_shooting[i].val == 1)
		{
			attron(COLOR_PAIR(1));
			if(e_shooting[i].y == BOTTOM -1 || e_shooting[i].y == TOP+1)
			{
				e_shooting[i].val = 0;
			}
			else
			{
				addch(' ');
				e_shooting[i].y += e_shooting[i].y_dir;
				move(e_shooting[i].y,e_shooting[i].x);
				addch('|');
			}
		}
		else
			addch(' ');
	}
	delay = 0;
	}
}

void draw()
{
	char num;
	while(1){
		if(stage==4){
			hp=0;
			flight.life=0;
			return;
		}
	pthread_mutex_lock(&lock);

	drawplusthing();
	drawflight();
	drawenemy();	

	e_drawshooting();
	drawshooting();
	if(po.val == 1)
		drawpo();
	if(timer == 4000)
		difficult++;
	timer++;

	move(LINES-1,0);
	refresh();

	if(flight.life == 0){
		clearmap();
		hp=0;
		move(10,20);
		addstr("You dead!");
		move(12,16);
		addstr("please input any key!");
		refresh();
		pthread_mutex_unlock(&lock);
		return;
	}
	usleep(8000);
	pthread_mutex_unlock(&lock);
	}
}


void startgame() 
{
	char i;
	char flag = 1;
	initscr();
	crmode();
	start_color();
	noecho();
	clear();
	for(i=0;i<MAX;i++)
	{
		enemy[i].x = 0;
		enemy[i].y = 0;
		enemy[i].enemy = 0;
	}
	init_pair(1,COLOR_WHITE,COLOR_BLACK); // normal
	init_pair(2,COLOR_BLUE,COLOR_BLUE); // flight
	init_pair(3,COLOR_YELLOW,COLOR_YELLOW); // map
	init_pair(4,COLOR_RED,COLOR_RED); // enemy
	init_pair(5,COLOR_BLUE,COLOR_BLACK);
	attron(COLOR_PAIR(5));
	move(15,19);

	move(16,3);
        addstr("___________________________________________________");
	move(17,3);
	addstr("  /     |   |  _____|  |   __     )   |    __    |");
	move(18,3);
        addstr(" /  /|   |  |  _____|  |   [_)  /     |   [__]   |");
	move(19,3);
        addstr("/__/ |____| |_______|  |__^________:  |__________|");
	move(20,3);
        addstr("___________________________________________________");


	attron(COLOR_PAIR(1));

	move(35,20);
	addstr("start game");
	move(38,20);
	addstr("print help");
	move(35,19);
	addstr(">");
	move(LINES-1,0);
	while(1)
	{
		i = getch();
		if( i == 'w'){
			flag = 1;
			move(38,19);
		        addstr(" ");	
			move(35,19);
			addstr(">");
		}
		if(i == 's'){
			flag = 2;
			 move(35,19);
			 addstr(" ");
			 move(38,19);
			 addstr(">");
		}
		if(i == '\n')
		       	break;
		move(LINES-1,0);
			refresh();
	}
	if(flag == 1)
	drawingmap();
	else if(flag == 2)
	printhelp();
}
void drawingmap()
{
	int i,j;
	clear();
	attron(COLOR_PAIR(3));
	for(i=LEFTEDGE; i<= RIGHTEDGE; i++)
		for(j = TOP; j<= BOTTOM; j++)
		{
			move(j,i);
			if(i == LEFTEDGE)
			{	
				if(j == TOP)
				addch('1');
				else if(j == BOTTOM)
				addch('3');
				else
				addstr("0");
			}
			else if(i == RIGHTEDGE)
			{
				if(j == TOP)
				addstr("2");
				else if(j == BOTTOM)
				addstr("4");
				else
				addstr("0");
			}
			else
			{
				if(j == TOP)
				addstr("0");
				else if(j == BOTTOM)
				addstr("0");
			}
		}
	attron(COLOR_PAIR(1));
	move(LINES-1,0);
	refresh();
}

void printhelp()
{
	clear();
	move(5,11);
	addstr("If you press button 'wasd', you can move your flight");
	move(7,11);
	addstr("If you press button 'b', you can use bomb");
	move(9,11);
	addstr("If you touch 'p', your shooing delay will reduced");
	move(11,11);
	addstr("You have only three lives");
	move(13,11);
	addstr("If you hit by enemy shooing or touch enemy, your life will reduced");
	move(15,11);
	addstr("We can clear three stages, each stage have boss");
	move(17,11);
	addstr("If you kill boss, you can get next stage");
	move(25,11);
	addstr("Press any key");
	move(LINES-1,0);
	refresh();
	getch();
	drawingmap();
}

void clearmap()
{
	int i,j;

	for(i=0; i<MAX;i++)
	{
	enemy[i].enemy = 0;
	e_shooting[i].val = 0;
	shooting[i].val = 0;
	}
	for(i=TOP+1;i<BOTTOM;i++){
		for(j=LEFTEDGE+1;j<RIGHTEDGE;j++)
		{
			move(i,j);
			addch(' ');
		}
	}
	refresh();

}

void enemyspone(){
	int i,j,k=0;
	int x;
	int count = 0;
	if(enemy[0].enemy != 2)
	while(1){
		for(i=0;i<MAX;i++)
		{
			if(timer>=8000){
				for(j=0;j<MAX;j++)
				{
					enemy[j].enemy=0;
					e_shooting[j].val = 0;
				}
				clearmap();
				difficult = 2;
				boss();
				clearmap();
				score += stage*10000;
				timer = 0;
				enemy[0].enemy = 0;
				difficult = 1;
				stage++;
				
				}
			if(i == MAX-1)
				i = 0;
			x = rand()%40;
			if(x <= 2*difficult)
			{
				if(enemy[i].enemy == 0)
				{
					enemy[i].y=rand()%39+2;
					enemy[i].x=TOP+1;
					enemy[i].y_dir=0;
					enemy[i].x_dir=1;
					enemy[i++].enemy=1;	
				}
				else if(enemy[i].enemy == 1)
					continue;
			}
			if(flight.life == 0)
				return;
			usleep(100000);
		}
	}
}
