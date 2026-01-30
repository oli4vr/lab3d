#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>
#include <signal.h>

#define MAP_H	128
#define MAP_W	128

#define SWIDTH  40
#define SHEIGHT  40

unsigned char map[MAP_H*MAP_W];

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
 int n=0,x=63,y=63,newx,newy;
 clear_map();
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
 }
 set_map(x,y,3);
}

int draw_lab(int x,int y,int xd,int yd,int dir)
{
 unsigned char c;
 int xn,yn,xr,yr,xl,yl,xnl,ynl,xnr,ynr;
 int ccx=SWIDTH/2,ccy=SWIDTH/2,dpos=SWIDTH/2,dposn,xb;
 clear();
 xr=0;
 for(xn=x-5;xn<=x+5;xn++)
 {
  yr=0;
  for(yn=y-5;yn<=y+5;yn++)
  {
   if (xn>=0 && xn<MAP_W && yn>=0 && yn<MAP_H)
   {
    switch (get_map(xn,yn))
    {
     case 1:
      attrset(A_REVERSE);
      mvaddch(yr+1,SWIDTH+4+xr,' ');
      break;
     case 3:
      attrset(A_NORMAL);
      mvaddch(yr+1,SWIDTH+4+xr,'E');
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
   mvaddch(6,SWIDTH+9,'>');
   break;
  case 1:
   mvaddch(6,SWIDTH+9,'v');
   break;
  case 2:
   mvaddch(6,SWIDTH+9,'<');
   break;
  case 3:
   mvaddch(6,SWIDTH+9,'^');
   break;
 }
 
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
  for(;dpos>dposn;dpos--)
  {
   if (get_map(xl,yl)==1)
   {
    attrset(A_NORMAL);
    mvaddch(ccy-dpos,ccx-dpos,'\\');
    mvaddch(ccy+dpos,ccx-dpos,'/');
   }
   else {
     attrset(A_REVERSE);
     mvaddch(ccy-dposn,ccx-dpos,' ');
     attrset(A_REVERSE);
     mvaddch(ccy+dposn,ccx-dpos,' ');
   }
   if (get_map(xr,yr)==1)
   {
    attrset(A_NORMAL);
    mvaddch(ccy-dpos,ccx+dpos,'/');
    mvaddch(ccy+dpos,ccx+dpos,'\\');
   }
   else {
     attrset(A_REVERSE);
     mvaddch(ccy-dposn,ccx+dpos,' ');
     attrset(A_REVERSE);
     mvaddch(ccy+dposn,ccx+dpos,' ');
   }
  }
  if (get_map(xn,yn)==3)
   c='E';
  else
   c=' ';
  for(xb=ccx-dpos;xb<=ccx+dpos;xb++)
  {
   attrset(A_REVERSE);
   mvaddch(ccy-dpos,xb,c);
   mvaddch(ccy+dpos,xb,c);
   mvaddch(xb,ccx-dpos,c);
   mvaddch(xb,ccy+dpos,c);
  }
  x+=xd;
  y+=yd;
 } while (get_map(x,y)!=1 && dpos>1);
 attrset(A_INVIS);
 move(SWIDTH+1,SWIDTH+1);

 return 0;
}

static void finish(int sig);

int main(int argc, char *argv[])
{
    FILE * fp;
    int num = 0;
    int x=63,y=63,xd=1,yd=0,xn,yn,tmp;
    int pen=0;
    int c;
    unsigned int key,dir=0;
    int compl=30;
    int level=1;


    /* initialize your non-curses data structures here */

    signal(SIGINT, finish);     

    initscr();      /* initialize the curses library */
    keypad(stdscr, TRUE);  /* enable keyboard mapping */
    nonl();         /* tell curses not to do NL->CR/NL on output */
    cbreak();       /* take input chars one at a time, no wait for \n */
    curs_set(0);

    while (1)
    {
     create_lab(compl);
     x=63;y=63;
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
      move(0,SWIDTH+4);
      printw("LEVEL %d",level);
      move(SWIDTH,0);
      xn=x+xd;
      yn=y+yd;
      key=getch();
      switch(key)
      {
       case 259:
        if (get_map(xn,yn)!=1)
         { x=xn; y=yn; }
        break;
       case 260:
        dir=(dir-1)&3;
        break;
       case 261:
        dir=(dir+1)&3;
        break;
      }
     }
     level++;
     if (compl<20000)
      compl+=(compl/2);
    }

   finish(0);               /* we're done */
}

static void finish(int sig)
{
    endwin();

    /* do your non-curses wrapup here */

    exit(0);
}

