#define _XOPEN_SOURCE_EXTENDED
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <wchar.h>
#include <ncursesw/ncurses.h>
#include <signal.h>
#include <locale.h>

#define MAP_H	512
#define MAP_W	512
#define MIDDLE_H	((MAP_H >> 1) -1)
#define MIDDLE_W	((MAP_W >> 1) -1)

#define SWIDTH  40
#define SHEIGHT  40
#define DWIDTH  (SWIDTH * 2)
#define DHEIGHT  (SHEIGHT * 2)

unsigned char map[MAP_H*MAP_W];
unsigned char pmap[MAP_H*MAP_W];

void print_food() {
    attrset(A_NORMAL);
    mvaddwstr(SHEIGHT-2,SWIDTH-2,L"▄▓▄");
    mvaddwstr(SHEIGHT-1,SWIDTH-2,L"▒░▒");
    mvaddwstr(SHEIGHT,SWIDTH-2,L"▒▒▒ FOOD");
}

void print_drink() {
    attrset(A_NORMAL);
    mvaddwstr(SHEIGHT-1,SWIDTH-3,L"░░░░");
    mvaddwstr(SHEIGHT,SWIDTH-4,L"░░░░ WATER");
}

void print_potion() {
    attrset(A_NORMAL);
    mvaddwstr(SHEIGHT-3,SWIDTH-2,L" ▄ ");
    mvaddwstr(SHEIGHT-2,SWIDTH-2,L"▄█▄");
    mvaddwstr(SHEIGHT-1,SWIDTH-2,L"█▒█ POTION");
    mvaddwstr(SHEIGHT  ,SWIDTH-2,L"▀▀▀");
}

void print_enemy() {
    attrset(A_NORMAL);
    mvaddwstr(SHEIGHT-19,SWIDTH-3,L" █▄ ▄█");
    mvaddwstr(SHEIGHT-18,SWIDTH-3,L" █░█░█");
    mvaddwstr(SHEIGHT-17,SWIDTH-3,L" ▀█▄█▀");
    mvaddwstr(SHEIGHT-16,SWIDTH-3,L" ▄███▄   ▄");
    mvaddwstr(SHEIGHT-15,SWIDTH-3,L"░░███ ▀▄▀");
    mvaddwstr(SHEIGHT-14,SWIDTH-3,L"░░███ ▀");
    mvaddwstr(SHEIGHT-13,SWIDTH-3,L"  █ █");
    mvaddwstr(SHEIGHT-12 ,SWIDTH-3,L" ▄█ █▄");
}

void print_enemy2() {
    attrset(A_NORMAL);
    mvaddwstr((SHEIGHT>>1)+1,SWIDTH-1,L"█▄█");
    mvaddwstr((SHEIGHT>>1)+2,SWIDTH-1,L"▀▄▀");
    mvaddwstr((SHEIGHT>>1)+3,SWIDTH-1,L"░██▄▀");
    mvaddwstr((SHEIGHT>>1)+4,SWIDTH-1,L"▀ ▀");
}

typedef struct _inventory_item {
  char name[16];
  unsigned int value[16];
  void (*action)(void*);
} inventory_item;

typedef struct _player_stats {
  int health;
  int armor;
  int food;
  int water;
  int steps;
} player_stats;

player_stats player;

typedef struct _enemy {
  int health;
  int attack;
  int armor;
} enemy;

void init_player() {
  player.health=100;
  player.armor=0;
  player.food=50;
  player.water=50;
  player.steps=0;
}

int get_map(int x,int y)
{
 return map[(y*MAP_H)+x];
}
void set_map(int x,int y,int val)
{
 map[(y*MAP_H)+x]=val;
}
void clear_map()
{
 memset(map,1,MAP_H*MAP_W);
}

int get_pmap(int x,int y)
{
 return pmap[(y*MAP_H)+x];
}
void set_pmap(int x,int y,int val)
{
 pmap[(y*MAP_H)+x]=val;
}
void clear_pmap()
{
 memset(pmap,1,MAP_H*MAP_W);
}

int player_action(int x,int y,player_stats *p) {
  int curxy=get_map(x,y);
  switch (curxy) {
   case 4:
    set_map(x,y,0);
    p->food+=15+(rand()&15);
    if (p->food > 100) p->food=100;
    return 1;
   case 5:
    set_map(x,y,0);
    p->water+=15+(rand()&15);
    if (p->water > 100) p->water=100;
    return 2;
   case 6:
    set_map(x,y,0);
    p->health+=10+(rand()&10);
    if (p->health > 100) p->health=100;
    return 3;
  }
  return 0;
}

int check_borders(int x,int y)
{
 if (x<1 || x>(MAP_W-2) || y<1 || y>(MAP_H-2)) return 0;
 if (get_map(x+1,y)!=1 && get_map(x,y+1)!=1 && get_map(x+1,y+1)!=1) return 0;
 if (get_map(x+1,y)!=1 && get_map(x,y-1)!=1 && get_map(x+1,y-1)!=1) return 0;
 if (get_map(x-1,y)!=1 && get_map(x,y-1)!=1 && get_map(x-1,y-1)!=1) return 0;
 if (get_map(x-1,y)!=1 && get_map(x,y+1)!=1 && get_map(x-1,y+1)!=1) return 0;
 return 1;
}

int create_lab(int num)
{
 int n=0,x=MIDDLE_W,y=MIDDLE_H,newx,newy;
 int cstep=0;
 clear_map();
 clear_pmap();
 srand(time(0));
 set_map(x,y,2);
 for(;n<num;n++)
 {
  if (get_map(x,y)==1)
   set_map(x,y,0);
  switch (rand()&3)
  {
   case 0:
    newx=x+1; newy=y;
    break;
   case 1:
    newx=x-1; newy=y;
    break;
   case 2:
    newx=x; newy=y+1;
    break;
   case 3:
    newx=x; newy=y-1;
    break;
  }
  if (check_borders(newx,newy))
  {
   x=newx;
   y=newy;
  }
  if (cstep>3) {
   if ((rand()&15)==0) {
    set_map(x,y,6); //Potion
   }
   if ((rand()%9)==0) {
    set_map(x,y,5); //Water
   }
   if ((rand()%10)==0) {
    set_map(x,y,4); //Food
   }
   if ((rand()&15)==0) {
     set_map(x,y,7); //Enemy
   }
  }
  cstep++;
 }
 set_map(x,y,3);
}

int draw_lab(int xo,int yo,int xd,int yd,int diro)
{
 unsigned char c;
 int xn,yn,xr,yr,xl,yl,xnl,ynl,xnr,ynr;
 int x=xo,y=yo,dir=diro;
 int ccx=SWIDTH/2,ccy=SWIDTH/2,dpos=SWIDTH/2,dposn,xb;
 unsigned char curxy;
 unsigned char denemy=0;
 clear();

  
 
 do {
  xn=x+xd;
  yn=y+yd;
  xnl=xn+yd;
  ynl=yn-xd;
  xnr=xn-yd;
  ynr=yn+xd;
  xl=x+yd;
  yl=y-xd;
  xr=x-yd;
  yr=y+xd;

  dposn=dpos/2;
  dpos--;

  curxy=get_map(xn,yn);

  set_pmap(xl,yl,get_map(xl,yl));
  set_pmap(xr,yr,get_map(xr,yr));
  for(;dpos>dposn;dpos--)
  {
   if (get_map(xl,yl)==1)
   {
    attrset(A_NORMAL);
    mvaddwstr(ccy-dpos,2*(ccx-dpos),L"▀▄");
    mvaddwstr(ccy+dpos,2*(ccx-dpos),L"▄▀");
   }
   else {
     attrset(A_NORMAL);
     mvaddwstr(ccy+dposn,2*(ccx-dpos)-1,L"▄▄▄");
     attrset(A_NORMAL);
     mvaddwstr(ccy-dposn,2*(ccx-dpos)-1,L"▀▀▀");
   }
   if (get_map(xr,yr)==1)
   {
    attrset(A_NORMAL);
    mvaddwstr(ccy-dpos,2*(ccx+dpos),L"▄▀");
    mvaddwstr(ccy+dpos,2*(ccx+dpos),L"▀▄");
   }
   else {
     attrset(A_NORMAL);
     mvaddwstr(ccy+dposn,2*(ccx+dpos),L"▄▄▄");
     attrset(A_NORMAL);
     mvaddwstr(ccy-dposn,2*(ccx+dpos),L"▀▀▀");
   }
  }
  if (get_map(xn,yn)==3) {
   c='E';
  } else { c=' '; }
  for(xb=ccx-dpos;xb<=ccx+dpos;xb++)
  {
   attrset(A_NORMAL);
   mvaddwstr(ccy-dpos,2*xb,L"▀▀");
   mvaddwstr(ccy+dpos,2*xb,L"▄▄");
   attrset(A_REVERSE);
   mvaddch(xb,2*(ccx-dpos),c);
   mvaddch(xb,2*(ccy+dpos)+1,c);
  }
  attrset(A_REVERSE);
  mvaddch(ccx-dpos,2*(ccy+dpos)+1,c);
  if (get_map(xn,yn)!=1) {
   if (get_map(xnl,ynl)==1) {
    mvaddch(ccx-dpos,2*(ccx-dpos)+1,' ');
    mvaddch(ccx+dpos,2*(ccx-dpos)+1,' ');
   }
   if (get_map(xnr,ynr)==1) {
    mvaddch(ccx+dpos,2*(ccx+dpos),' ');
    mvaddch(ccx-dpos,2*(ccx+dpos),' ');
   }
  }
  switch (curxy) {
    case 4:
    case 5:
    case 6:
     if (dposn>4) {
      attrset(A_NORMAL);
      mvaddwstr((SHEIGHT>>1)+dposn-1,SWIDTH,L"░░");
     }
    break;
    case 7:
     switch (dposn) {
      case 10:
       denemy=1;
      break;
      case 5:
       if (denemy==0) denemy=2;
      break;
     }
    break;
  }

  set_pmap(x,y,get_map(x,y));
  x+=xd;
  y+=yd;
 } while (get_map(x,y)!=1 && dpos>1);

 switch (get_map(xo,yo)) {
  case (4) :
    print_food();
   break;;
  case (5) :
    print_drink();
   break;;
  case (6) :
    print_potion();
   break;;
 }

 switch (denemy) {
  case 1:
    print_enemy();
   break;
  case 2:
    print_enemy2();
   break;
 }


 attrset(A_INVIS);
 move(SWIDTH+1,SWIDTH+1);

 //Draw Player MAP
 x=xo,y=yo,dir=diro;
 xr=0;
 for(xn=x-5;xn<=x+5;xn++)
 {
  yr=0;
  for(yn=y-5;yn<=y+5;yn++)
  {
   if (xn>=0 && xn<MAP_W && yn>=0 && yn<MAP_H)
   {
    switch (get_pmap(xn,yn))
    {
     case 1:
      attrset(A_REVERSE);
      mvaddch(yr+1,DWIDTH+4+xr,' ');
      break;
     case 3:
      attrset(A_NORMAL);
      mvaddch(yr+1,DWIDTH+4+xr,'E');
      break;
     case 4:
      attrset(A_NORMAL);
      mvaddch(yr+1,DWIDTH+4+xr,'F');
      break;
     case 5:
      attrset(A_NORMAL);
      mvaddch(yr+1,DWIDTH+4+xr,'D');
      break;
     case 6:
      attrset(A_NORMAL);
      mvaddch(yr+1,DWIDTH+4+xr,'P');
      break;
     case 7:
      attrset(A_NORMAL);
      mvaddch(yr+1,DWIDTH+4+xr,'*');
      break;
    }
   }
   yr++;
  }
  xr++;
 }
 attrset(A_NORMAL);
 switch (dir)
 {
  case 0:
   mvaddch(6,DWIDTH+9,'>');
   break;
  case 1:
   mvaddch(6,DWIDTH+9,'v');
   break;
  case 2:
   mvaddch(6,DWIDTH+9,'<');
   break;
  case 3:
   mvaddch(6,DWIDTH+9,'^');
   break;
 } // End player MAP

 return 0;
}

void finish(int sig)
{
    endwin();
    exit(0);
}

int end_turn(player_stats *p,int x,int y,int xn,int yn) {
  if (get_map(xn,yn)==7 && (((rand()&3)==0) || (rand()&3)==0)) {
    set_map(xn,yn,0);
    set_pmap(xn,yn,0);
  }
  if (get_map(x+1,y)==7 || get_map(x-1,y)==7 || get_map(x,y+1)==7 || get_map(x,y-1)==7) {
   p->health-=3+(rand()&7);
  }
  if (p->food==0) {p->health-=2;}
  if (p->water==0) {p->health-=2;}
  if (p->food > 0) {
    p->food-=2;
    p->water-=3;
  }
  if (p->food<0) p->food=0;
  if (p->water<0) p->water=0;
  if (p->health<0) p->health=0;
  if (p->health==0) return 1;
  p->steps++;
  return 0;
}

int main(int argc, char *argv[])
{
    FILE * fp;
    int num = 0;
    int x=MIDDLE_W,y=MIDDLE_H,xd=1,yd=0,xn,yn,tmp;
    int pen=0;
    int c;
    unsigned int key,dir=0;
    int compl=30;
    int level=1;
    int valxy;


    /* initialize your non-curses data structures here */
    setlocale(LC_ALL, "en_US.UTF-8");
    setlocale(LC_CTYPE, "en_US.UTF-8");

    signal(SIGINT, finish);     


    initscr();      /* initialize the curses library */
    //start_color();
    keypad(stdscr, TRUE);  /* enable keyboard mapping */
    nonl();         /* tell curses not to do NL->CR/NL on output */
    cbreak();       /* take input chars one at a time, no wait for \n */
    curs_set(0);

    init_player();

    while (1)
    {
     create_lab(compl);
     x=MIDDLE_W;y=MIDDLE_H;
     while(get_map(x,y)!=3)
     {
      switch (dir)
      {
       case 0:
        xd=1;
        yd=0;
        break;
       case 1:
        xd=0;
        yd=1;
        break;
       case 2:
        xd=-1;
        yd=0;
        break;
       case 3:
        xd=0;
        yd=-1;
        break;
      }
      draw_lab(x,y,xd,yd,dir);
      attrset(A_NORMAL);
      move(0,SWIDTH*2+4);
      printw("LEVEL %d",level);
      move(12,SWIDTH*2+4);
      printw("HEALTH  %3d",player.health);
      move(13,SWIDTH*2+4);
      printw("FOOD    %3d",player.food);
      move(14,SWIDTH*2+4);
      printw("WATER   %3d",player.water);
      move(20,SWIDTH*2+4);
      printw("ARROW KEYS = MOVE");
      move(21,SWIDTH*2+4);
      printw("SPACE = USE ITEM");
      move(SWIDTH,0);
      xn=x+xd;
      yn=y+yd;
      key=getch();
      switch(key)
      {
       case 259:
         valxy=get_map(xn,yn);
         if (valxy!=1) {
           if (valxy!=7) {x=xn; y=yn;}
           if (end_turn(&player,x,y,xn,yn)==1) {
           attrset(A_NORMAL);
           endwin();
           printf("YOU DIED!!! End of Game\n");
           printf("You lasted %d steps and got to level %d\n\n",player.steps,level);
           exit(0);
          }
         }
        break;
       case 260:
        dir=(dir-1)&3;
        break;
       case 261:
        dir=(dir+1)&3;
        break;
       case ' ':
        player_action(x,y,&player);
        break;
      }
     }
     level++;
     if (compl<20000)
      compl+=(compl/2);
    }

   finish(0);               /* we're done */
}