/*========================================
 *    sl.c: SL version 5.03
 *        Copyright 1993,1998,2014-2015
 *                  Toyoda Masashi
 *                  (mtoyoda@acm.org)
 *        Last Modified: 2014/06/03
 *========================================
 */
/* sl version 5.03 : Fix some more compiler warnings.                        */
/*                                              by Ryan Jacobs    2015/01/19 */
/* sl version 5.02 : Fix compiler warnings.                                  */
/*                                              by Jeff Schwab    2014/06/03 */
/* sl version 5.01 : removed cursor and handling of IO                       */
/*                                              by Chris Seymour  2014/01/03 */
/* sl version 5.00 : add -c option                                           */
/*                                              by Toyoda Masashi 2013/05/05 */
/* sl version 4.00 : add C51, usleep(40000)                                  */
/*                                              by Toyoda Masashi 2002/12/31 */
/* sl version 3.03 : add usleep(20000)                                       */
/*                                              by Toyoda Masashi 1998/07/22 */
/* sl version 3.02 : D51 flies! Change options.                              */
/*                                              by Toyoda Masashi 1993/01/19 */
/* sl version 3.01 : Wheel turns smoother                                    */
/*                                              by Toyoda Masashi 1992/12/25 */
/* sl version 3.00 : Add d(D51) option                                       */
/*                                              by Toyoda Masashi 1992/12/24 */
/* sl version 2.02 : Bug fixed.(dust remains in screen)                      */
/*                                              by Toyoda Masashi 1992/12/17 */
/* sl version 2.01 : Smoke run and disappear.                                */
/*                   Change '-a' to accident option.                         */
/*                                              by Toyoda Masashi 1992/12/16 */
/* sl version 2.00 : Add a(all),l(long),F(Fly!) options.                     */
/*                                              by Toyoda Masashi 1992/12/15 */
/* sl version 1.02 : Add turning wheel.                                      */
/*                                              by Toyoda Masashi 1992/12/14 */
/* sl version 1.01 : Add more complex smoke.                                 */
/*                                              by Toyoda Masashi 1992/12/14 */
/* sl version 1.00 : SL runs vomiting out smoke.                             */
/*                                              by Toyoda Masashi 1992/12/11 */

#include <curses.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "sl.h"

void add_smoke(int y, int x);
int add_smoke_r(int y, int x);
void add_man(int y, int x);
void add_fdancer(int y, int x);
void add_mdancer(int y, int x);
void add_fdancer_r(int y, int x);
void add_mdancer_r(int y, int x);
int add_C51(int x);
int add_D51(int x);
int add_TGV(int x);
int add_C51_r(int x);
int add_D51_r(int x);
int add_TGV_r(int x);
int add_sl(int x, int cars);
int add_sl_r(int x, int cars);
int add_train(int (*loc_func)(int), int locs, int cars, int loc_len, int car_len, int loc_h, int x);
int add_train_r(int (*loc_func)(int), int locs, int cars, int loc_len, int car_len, int loc_h, int x);
int add_locomotive(int (*func)(int), int len, int no, int x);
int add_locomotive_r(int (*func)(int), int len, int no, int x);
int add_cars(int start, int len, int no, int h, int x, int err);
int add_car(int x, int i, int i_diff);
int add_last_car(int x, int i, int i_diff);
int add_vagoon(int x, int i_diff);
int add_cars_r(int start, int len, int no, int h, int x, int err);
int add_car_r(int x, int i, int i_diff);
int add_last_car_r(int x, int i, int i_diff);
int add_vagoon_r(int x, int i_diff);
void option(char *str);
int my_mvaddstr(int y, int x, char *str);
int my_mvaddstr_r(int y, int x, char *str);
int begin_gate(int, int);
int add_cross(int, int);
int x_gate(int, int);
int end_gate(int, int);

int ACCIDENT  = 0;
int LOGO      = 0;
int FLY       = 0;
int C51       = 0;
int DANCE     = 0;
int LOCOMOTIVES = 1;
int CARS      = -1;
unsigned int WAIT_TIME = 0;
int CROSS     = 0;
int ONEDIREC  = 1;
int EVIL      = 0;
int DISCO     = 0;
int TGV       = 0;
int RAINBOW   = 0;

int my_mvaddstr(int y, int x, char *str)
{
    int color, err = 0;
    for ( ; x < 0; ++x, ++str)
        if (str == NULL || *str == '\0')  return ERR;
    for ( ; str != NULL && *str != '\0'; ++str, ++x) {
        if (RAINBOW == 1) {
	    color = ((x+y)/5) % 7 + 1;
            attron(COLOR_PAIR(color));
        }
        err = mvaddch(y, x, *str);
        if (RAINBOW == 1)
            attroff(COLOR_PAIR(color));
        if (err == ERR)
            return ERR;
    }
    return OK;
}

int my_mvaddstr_r(int y, int x, char *str)
{
  int i = 0, color, err = 0;
  char c;

  for ( ; x >= COLS; --x, ++i) {
    if (str[i] == '\0') {
      return ERR;
    }
  }
  for ( ; str[i] != '\0'; ++i, --x) {
    c = str[i];
    switch (c) {
      case '\\':
      c = '/';
      break;
      case '/':
      c = '\\';
      break;
      case '(':
        c = ')';
break;
case ')':
c = '(';
  break;
  case '[':
    c = ']';
break;
case ']':
c = '[';
  break;
}
if (RAINBOW == 1) {
    color = ((x+y)/5) % 7 + 1;
    attron(COLOR_PAIR(color));
}
err = mvaddch(y, x, c);
if (RAINBOW == 1)
    attroff(COLOR_PAIR(color));
if (err == ERR)
    return ERR;
}

return OK;
}

void option(char *str)
{
    extern int ACCIDENT, LOGO, FLY, C51;

    while (*str != '\0') {
        switch (*str++) {
            case 'a': ACCIDENT = 1; break;
            case 'F': FLY      = 1; break;
            case 'l': LOGO     = 1; break;
            case 'c': C51      = 1; break;
            case 'd': DANCE    = 1; break;
            case 'N': CARS     = atoi(str); break;
            case 'L': LOCOMOTIVES = atoi(str); break;
            case 'W': WAIT_TIME = atoi(str)*1000; break;
            case 'C': CROSS    = 1; break;
            case 'R': ONEDIREC = 0; break;
            case 'e': EVIL     = 1; break;
            case 'D': DISCO    = 1; break;
            case 'G': TGV      = 1; break;
            case 'r': RAINBOW  = 1; break;
            default:                break;
        }
    }
    if (CARS==-1) {
        CARS = (LOGO || TGV);
    }
}

int main(int argc, char *argv[])
{
    int x, i;

    WAIT_TIME = 0;
    srand(time(NULL));
    for (i = 1; i < argc; ++i) {
        if (*argv[i] == '-') {
            option(argv[i] + 1);
        }
    }
    initscr();
    if (DISCO == 1 || RAINBOW == 1) {
        start_color();
	if (can_change_color() || RAINBOW == 1) {
            init_color(COLOR_RED, 1000, 0, 0);
            init_color(COLOR_WHITE, 1000, 500, 0);
            init_color(COLOR_YELLOW, 1000, 1000, 0);
            init_color(COLOR_GREEN, 0, 1000, 0);
            init_color(COLOR_CYAN, 300, 300, 1000);
            init_color(COLOR_BLUE, 0, 0, 1000);
            init_color(COLOR_MAGENTA, 1000, 0, 1000);
        }
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_GREEN, COLOR_BLACK);
        init_pair(5, COLOR_CYAN, COLOR_BLACK);
        init_pair(6, COLOR_BLUE, COLOR_BLACK);
        init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
    }
    if (EVIL==1) {
        signal(SIGINT, SIG_IGN);
    }
    signal(SIGTSTP, SIG_IGN);
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    leaveok(stdscr, TRUE);
    scrollok(stdscr, FALSE);

    if (WAIT_TIME==0) {
      WAIT_TIME = (unsigned)((rand()%7 + 1) * 5000);
    }
    if (FLY != 1 && CROSS==1) {
      begin_gate(3 * COLS / 10, 0);
    }
    for (x = COLS - 1; ; --x) {
        if (!RAINBOW && DISCO && (x + INT_MAX/2) % 7 == 2)
            attron(COLOR_PAIR((x + INT_MAX/2) / 16 % 7 + 1));
        if (LOGO == 1) {
            if (add_sl(x, CARS) == ERR) break;
        }
        else if (C51 == 1) {
            if (add_train(add_C51, LOCOMOTIVES, CARS, C51LENGTH-2, PASSLENGTH, C51HEIGHT, x) == ERR) break;
        }
        else if (TGV == 1) {
            if (add_train(add_TGV, LOCOMOTIVES, CARS, TGVLENGTH, TGVLENGTH, TGVHEIGHT, x) == ERR) break;
        }
        else {
            if (add_train(add_D51, LOCOMOTIVES, CARS, D51LENGTH-2, PASSLENGTH, D51HEIGHT, x) == ERR) break;
        }
        if (FLY!=1 && CROSS==1) {
          add_cross(3*COLS/10, 0);
        }
        getch();
        refresh();
        usleep(WAIT_TIME);
    }
    if(ONEDIREC!=1) {
    if (FLY!=1 && CROSS==1) {
      x_gate(3*COLS/10, 0);
    }
    for (x = 0; ; ++x) {
        if (!RAINBOW && DISCO && (x + INT_MAX/2) % 7 == 2)
            attron(COLOR_PAIR((x + INT_MAX/2) / 16 % 7 + 1));
        if (LOGO == 1) {
            if (add_sl_r(x, CARS) == ERR) break;
        }
        else if (C51 == 1) {
            if (add_train_r(add_C51_r, LOCOMOTIVES, CARS, C51LENGTH-2, PASSLENGTH, C51HEIGHT, x) == ERR) break;
        }
        else if (TGV == 1) {
            if (add_train_r(add_TGV_r, LOCOMOTIVES, CARS, TGVLENGTH, TGVLENGTH, TGVHEIGHT, x) == ERR) break;
        }
        else {
            if (add_train_r(add_D51_r, LOCOMOTIVES, CARS, D51LENGTH-2, PASSLENGTH, D51HEIGHT, x) == ERR) break;
        }
        if (FLY!=1 && CROSS==1) {
          add_cross(3*COLS/10, 1);
        }
        getch();
        refresh();
        usleep(WAIT_TIME);
    }
    }
    if(FLY!=1 && CROSS==1)
      end_gate(3*COLS/10, (ONEDIREC==1 ? 0 : 1) );
    mvcur(0, COLS - 1, LINES - 1, 0);
    endwin();

    return 0;
}

int add_train(int (*loc_func)(int), int locs, int cars, int loc_len, int car_len, int loc_h, int x) {
  int err = add_locomotive(loc_func, loc_len, locs, x);
  return add_cars(loc_len*locs, car_len, cars, loc_h, x, err);
}

int add_locomotive(int (*func)(int), int len, int no, int x) {
  int i, err = 0;
  for (i=0; i<no; ++i) {
    err = func(x + i*len);
  }
  return err;
}

int add_cars(int start, int len, int no, int h, int x, int err) {
  int i;
  for (i=0; i<no; ++i) {
    if (TGV==1) {
      err = add_vagoon(x + i*len + start, h-TGVHEIGHT);
    } else if (i!=no-1) {
      add_car(x + i*len + start, i+1, h-D51HEIGHT);
    } else {
      err = add_last_car(x + i*len + start, i+1, h-D51HEIGHT);
    }
  }
  return err;
}

int add_train_r(int (*loc_func)(int), int locs, int cars, int loc_len, int car_len, int loc_h, int x) {
  int err = add_locomotive_r(loc_func, loc_len, locs, x);
  return add_cars_r(-loc_len*locs, car_len, cars, loc_h, x, err);
}

int add_locomotive_r(int (*func)(int), int len, int no, int x) {
  int i, err = 0;
  for (i=0; i<no; ++i) {
    err = func(x - i*len);
  }
  return err;
}

int add_cars_r(int start, int len, int no, int h, int x, int err) {
  int i;
  for (i=0; i<no; ++i) {
    if (TGV==1) {
      err = add_vagoon_r(x - i*len + start, h-TGVHEIGHT);
    } else if (i!=no-1) {
      add_car_r(x - i*len + start, i+1, h-D51HEIGHT);
    } else {
      err = add_last_car_r(x - i*len + start, i+1, h-D51HEIGHT);
    }
  }
  return err;
}

int get_y() {
  if (LOGO == 1) {
    return LINES / 2 - 7;
  } else if (C51 == 1) {
    return LINES / 2 - 4;
  } else {
    return LINES / 2 - 5;
  }
}


int add_car(int x, int no, int i_diff) {
  static char *coach[D51HEIGHT + 1]
    = {PASS01, PASS02, PASS03, PASS04, PASS05,
       PASS06, PASS07, PASS08, PASS09, PASS10, PASSDEL};

    int y, i;

    char num[10];
    sprintf(num, "%2d", no);

    if (x < - PASSLENGTH)  return ERR;
    y = LINES / 2 - 5;

    if (FLY == 1) {
        y = (x / 7) + LINES - (COLS / 7) - D51HEIGHT;
    }
    for (i = 0; i <= D51HEIGHT; ++i) {
        my_mvaddstr(y + i + i_diff, x, coach[i]);
        if (i == 3) {
          my_mvaddstr(y + i + i_diff, x+8, num);
        }
    }
    if (ACCIDENT == 1) {
        for (i=0; i<10; ++i) {
            add_man(y + 2+i_diff, x + i*7 +9);
            add_man(y + 2+i_diff, x + i*7 +13);
        }
    }
    if (DANCE == 1 && ACCIDENT == 0 && FLY == 0) {
        for (i=1; i<10; i+=2) {
            add_mdancer(y - 2 +i_diff, x + i*7 + 6); add_fdancer(y - 2 + i_diff, x + i*7+ 10);
        }
    }
    return OK;
}

int add_last_car(int x, int no, int i_diff) {
  static char *lcoach[D51HEIGHT + 1]
    = {LPASS01, LPASS02, LPASS03, LPASS04, LPASS05,
       LPASS06, LPASS07, LPASS08, LPASS09, LPASS10, LPASSDEL};

    int y, i;

    char num[10];
    sprintf(num, "%2d", no);

    if (x < - PASSLENGTH)  return ERR;
    y = LINES / 2 - 5;

    if (FLY == 1) {
        y = (x / 7) + LINES - (COLS / 7) - D51HEIGHT;
    }
    for (i = 0; i <= D51HEIGHT; ++i) {
        my_mvaddstr(y + i + i_diff, x, lcoach[i]);
        if (i == 3) {
          my_mvaddstr(y + i + i_diff, x+8, num);
        }
    }
    if (ACCIDENT == 1) {
        for (i=0; i<9; ++i) {
            add_man(y + 2+i_diff, x + i*7 +9);
            add_man(y + 2+i_diff, x + i*7 +13);
        }
    }
    if (DANCE == 1 && ACCIDENT == 0 && FLY == 0) {
        for (i=1; i<10; i+=2) {
            add_mdancer(y - 2 +i_diff, x + i*7 + 6); add_fdancer(y - 2 + i_diff, x + i*7+ 10);
        }
    }

    return OK;
}

////////////

int add_car_r(int x, int no, int i_diff) {
  static char *coach[D51HEIGHT + 1]
    = {PASS01, PASS02, PASS03, PASS04, PASS05,
       PASS06, PASS07, PASS08, PASS09, PASS10, PASSDEL};

    int y, i;

    char num[10];
    sprintf(num, "%2d", no);

    if (x > PASSLENGTH + COLS)  return ERR;
    if (x < - PASSLENGTH) return OK;
    y = LINES / 2 - 5;

    if (FLY == 1) {
        y = -(x / 7) + LINES - (COLS / 7) - D51HEIGHT;
    }
    for (i = 0; i <= D51HEIGHT; ++i) {
        my_mvaddstr_r(y + i + i_diff, x, coach[i]);
        if (i == 3) {
          my_mvaddstr_r(y + i + i_diff, x-8, num);
        }
    }
    if (ACCIDENT == 1) {
        for (i=0; i<10; ++i) {
            add_man(y + 2+i_diff, x - i*7 -11);
            add_man(y + 2+i_diff, x - i*7 -15);
        }
    }
    if (DANCE == 1 && ACCIDENT == 0 && FLY == 0) {
        for (i=1; i<10; i+=2) {
          add_mdancer_r(y - 2 +i_diff, x - i*7 - 7); add_fdancer_r(y - 2 + i_diff, x - i*7 - 11);
        }
    }
    return OK;
}

int add_last_car_r(int x, int no, int i_diff) {
  static char *lcoach[D51HEIGHT + 1]
    = {LPASS01, LPASS02, LPASS03, LPASS04, LPASS05,
       LPASS06, LPASS07, LPASS08, LPASS09, LPASS10, LPASSDEL};

    int y, i;

    char num[10];
    sprintf(num, "%2d", no);

    if (x > PASSLENGTH + COLS)  return ERR;
    if (x < - PASSLENGTH) return OK;
    y = LINES / 2 - 5;

    if (FLY == 1) {
        y = -(x / 7) + LINES - (COLS / 7) - D51HEIGHT;
    }
    for (i = 0; i <= D51HEIGHT; ++i) {
        my_mvaddstr_r(y + i + i_diff, x, lcoach[i]);
        if (i == 3) {
          my_mvaddstr_r(y + i + i_diff, x-8, num);
        }
    }
    if (ACCIDENT == 1) {
        for (i=0; i<9; ++i) {
            add_man(y + 2+i_diff, x - i*7 -11);
            add_man(y + 2+i_diff, x - i*7 -15);
        }
    }
    if (DANCE == 1 && ACCIDENT == 0 && FLY == 0) {
        for (i=1; i<10; i+=2) {
            add_mdancer_r(y - 2 +i_diff, x - i*7 - 7); add_fdancer_r(y - 2 + i_diff, x - i*7 - 11);
        }
    }
    return OK;
}

int add_sl(int x, int cars)
{
    static char *sl[LOGOPATTERNS][LOGOHEIGHT + 1]
        = {{LOGO1, LOGO2, LOGO3, LOGO4, LWHL11, LWHL12, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL21, LWHL22, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL31, LWHL32, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL41, LWHL42, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL51, LWHL52, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL61, LWHL62, DELLN}};

    static char *coal[LOGOHEIGHT + 1]
        = {LCOAL1, LCOAL2, LCOAL3, LCOAL4, LCOAL5, LCOAL6, DELLN};

    static char *car[LOGOHEIGHT + 1]
        = {LCAR1, LCAR2, LCAR3, LCAR4, LCAR5, LCAR6, DELLN};

    int i, j, y, py1 = 0;
    int logolength = 44 + cars*42 - 1;

    if (x < - logolength)  return ERR;
    y = LINES / 2 - 3;

    if (FLY == 1) {
        y = (x / 6) + LINES - (COLS / 6) - LOGOHEIGHT;
        py1 = 2;
    }
    for (i = 0; i <= LOGOHEIGHT; ++i) {
        my_mvaddstr(y + i, x, sl[(LOGOLENGTH + x) / 3 % LOGOPATTERNS][i]);
        my_mvaddstr(y + i + py1, x + 21, coal[i]);
        for (j = 0; j < 2*cars; j+=2) {
          my_mvaddstr(y + i + (j+2)*py1, x + j*21 + 42, car[i]);
          my_mvaddstr(y + i + (j+3)*py1, x + j*21 + 63, car[i]);
        }
    }
    if (ACCIDENT == 1) {
        add_man(y + 1, x + 14);
        for (j = 0; j < 2*cars; j+=2) {
          add_man(y + 1 + (j+2)*py1, x + j*21 + 45);  add_man(y + 1 + (j+2)*py1, x + j*21 + 53);
          add_man(y + 1 + (j+3)*py1, x + j*21 + 66);  add_man(y + 1 + (j+3)*py1, x + j*21 + 74);
        }
    }
    if (DANCE == 1 && ACCIDENT == 0 && FLY == 0) {
        add_mdancer(y - 2, x + 21);
        for (j = 0; j < 2*cars; j+=2) {
          add_fdancer(y + (j+2)*py1 - 2, x + j*21 + 45);
          add_fdancer(y + (j+2)*py1 - 2, x + j*21 + 50); add_fdancer(y + (j+2)*py1 - 2, x + j*21 + 55);
          add_mdancer(y + (j+3)*py1 - 2, x + j*21 + 66);
          add_mdancer(y + (j+3)*py1 - 2, x + j*21 + 71); add_mdancer(y + (j+3)*py1 - 2, x + j*21 + 76);
        }
    }
    add_smoke(y - 1, x + LOGOFUNNEL);
    return OK;
}


int add_sl_r(int x, int cars)
{
    static char *sl[LOGOPATTERNS][LOGOHEIGHT + 1]
        = {{LOGO1, LOGO2, LOGO3, LOGO4, LWHL11, LWHL12, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL21, LWHL22, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL31, LWHL32, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL41, LWHL42, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL51, LWHL52, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL61, LWHL62, DELLN}};

    static char *coal[LOGOHEIGHT + 1]
        = {LCOAL1, LCOAL2, LCOAL3, LCOAL4, LCOAL5, LCOAL6, DELLN};

    static char *car[LOGOHEIGHT + 1]
        = {LCAR1, LCAR2, LCAR3, LCAR4, LCAR5, LCAR6, DELLN};

    int i, j, y, py1 = 0;
    int logolength = 44 + cars*42 - 1;

    if (x > logolength + COLS)  return ERR;
    y = LINES / 2 - 3;

    if (FLY == 1) {
        y = -(x / 6) + LINES - (COLS / 6) - LOGOHEIGHT;
        py1 = 2;
    }
    for (i = 0; i <= LOGOHEIGHT; ++i) {
        my_mvaddstr_r(y + i, x, sl[(LOGOLENGTH + x) / 3 % LOGOPATTERNS][i]);
        my_mvaddstr_r(y + i + py1, x - 21, coal[i]);
        for (j = 0; j < 2*cars; j+=2) {
          my_mvaddstr_r(y + i + (j+2)*py1, x - j*21 - 42, car[i]);
          my_mvaddstr_r(y + i + (j+3)*py1, x - j*21 - 63, car[i]);
        }
    }
    if (ACCIDENT == 1) {
        add_man(y + 1, x - 16);
        for (j = 0; j < 2*cars; j+=2) {
          add_man(y + 1 + (j+2)*py1, x - j*21 - 47);  add_man(y + 1 + (j+2)*py1, x - j*21 - 55);
          add_man(y + 1 + (j+3)*py1, x - j*21 - 68);  add_man(y + 1 + (j+3)*py1, x - j*21 - 76);
        }
    }
    if (DANCE == 1 && ACCIDENT == 0 && FLY == 0) {
        add_mdancer_r(y - 2, x - 21);
        for (j = 0; j < 2*cars; j+=2) {
          add_fdancer_r(y + (j+2)*py1 - 2, x - j*21 - 47);
          add_fdancer_r(y + (j+2)*py1 - 2, x - j*21 - 52); add_fdancer_r(y + (j+2)*py1 - 2, x - j*21 - 57);
          add_mdancer_r(y + (j+3)*py1 - 2, x - j*21 - 68);
          add_mdancer_r(y + (j+3)*py1 - 2, x - j*21 - 73); add_mdancer_r(y + (j+3)*py1 - 2, x - j*21 - 78);
        }
    }
    add_smoke(y - 1, x - LOGOFUNNEL - 2);
    return OK;
}


int add_D51(int x)
{
    static char *d51[D51PATTERNS][D51HEIGHT + 1]
        = {{D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL11, D51WHL12, D51WHL13, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL21, D51WHL22, D51WHL23, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL31, D51WHL32, D51WHL33, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL41, D51WHL42, D51WHL43, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL51, D51WHL52, D51WHL53, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL61, D51WHL62, D51WHL63, D51DEL}};
    static char *coal[D51HEIGHT + 1]
        = {COAL01, COAL02, COAL03, COAL04, COAL05,
           COAL06, COAL07, COAL08, COAL09, COAL10, COALDEL};

    int y, i, dy = 0;

    if (x < - D51LENGTH)  return ERR;
    y = LINES / 2 - 5;

    if (FLY == 1) {
        y = (x / 7) + LINES - (COLS / 7) - D51HEIGHT;
        dy = 1;
    }
    for (i = 0; i <= D51HEIGHT; ++i) {
        my_mvaddstr(y + i, x, d51[(D51LENGTH + x) % D51PATTERNS][i]);
        my_mvaddstr(y + i + dy, x + 53, coal[i]);
    }
    if (ACCIDENT == 1) {
        add_man(y + 2, x + 43);
        add_man(y + 2, x + 47);
    }
    if (DANCE == 1 && ACCIDENT ==0 && FLY == 0) {
       add_mdancer(y - 2, x + 43); add_fdancer(y - 2, x + 48);
    }
    add_smoke(y - 1, x + D51FUNNEL);
    return OK;
}

int add_C51(int x)
{
    static char *c51[C51PATTERNS][C51HEIGHT + 1]
        = {{C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH11, C51WH12, C51WH13, C51WH14, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH21, C51WH22, C51WH23, C51WH24, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH31, C51WH32, C51WH33, C51WH34, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH41, C51WH42, C51WH43, C51WH44, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH51, C51WH52, C51WH53, C51WH54, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH61, C51WH62, C51WH63, C51WH64, C51DEL}};
    static char *coal[C51HEIGHT + 1]
        = {COALDEL, COAL01, COAL02, COAL03, COAL04, COAL05,
           COAL06, COAL07, COAL08, COAL09, COAL10, COALDEL};

    int y, i, dy = 0;

    if (x < - C51LENGTH)  return ERR;
    y = LINES / 2 - 5;

    if (FLY == 1) {
        y = (x / 7) + LINES - (COLS / 7) - C51HEIGHT;
        dy = 1;
    }
    for (i = 0; i <= C51HEIGHT; ++i) {
        my_mvaddstr(y + i, x, c51[(C51LENGTH + x) % C51PATTERNS][i]);
        my_mvaddstr(y + i + dy, x + 55, coal[i]);
    }
    if (ACCIDENT == 1) {
        add_man(y + 3, x + 45);
        add_man(y + 3, x + 49);
    }
    if (DANCE == 1 && ACCIDENT ==0 && FLY == 0) {
        add_mdancer(y - 1, x + 45); add_fdancer(y - 1, x + 50);
    }
    add_smoke(y - 1, x + C51FUNNEL);
    return OK;
}
////////

int add_D51_r(int x)
{
    static char *d51[D51PATTERNS][D51HEIGHT + 1]
        = {{D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL11, D51WHL12, D51WHL13, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL21, D51WHL22, D51WHL23, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL31, D51WHL32, D51WHL33, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL41, D51WHL42, D51WHL43, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL51, D51WHL52, D51WHL53, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL61, D51WHL62, D51WHL63, D51DEL}};
    static char *coal[D51HEIGHT + 1]
        = {COAL01, COAL02, COAL03, COAL04, COAL05,
           COAL06, COAL07, COAL08, COAL09, COAL10, COALDEL};

    int y, i, dy = 0;

    if (x > D51LENGTH + COLS)  return ERR;
    if (x < - D51LENGTH) return OK;
    y = LINES / 2 - 5;

    if (FLY == 1) {
        y = -(x / 7) + LINES - (COLS / 7) - D51HEIGHT;
        dy = 1;
    }
    for (i = 0; i <= D51HEIGHT; ++i) {
        my_mvaddstr_r(y + i, x, d51[(D51LENGTH + x) % D51PATTERNS][i]);
        my_mvaddstr_r(y + i + dy, x - 53, coal[i]);
    }
    if (ACCIDENT == 1) {
        add_man(y + 2, x - 45);
        add_man(y + 2, x - 49);
    }
    if (DANCE == 1 && ACCIDENT ==0 && FLY == 0) {
        add_fdancer_r(y - 2, x - 45); add_mdancer_r(y - 2, x - 50);
    }
    add_smoke_r(y - 1, x - D51FUNNEL - 2);
    return OK;
}

int add_C51_r(int x)
{
    static char *c51[C51PATTERNS][C51HEIGHT + 1]
        = {{C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH11, C51WH12, C51WH13, C51WH14, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH21, C51WH22, C51WH23, C51WH24, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH31, C51WH32, C51WH33, C51WH34, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH41, C51WH42, C51WH43, C51WH44, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH51, C51WH52, C51WH53, C51WH54, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH61, C51WH62, C51WH63, C51WH64, C51DEL}};
    static char *coal[C51HEIGHT + 1]
        = {COALDEL, COAL01, COAL02, COAL03, COAL04, COAL05,
           COAL06, COAL07, COAL08, COAL09, COAL10, COALDEL};

    int y, i, dy = 0;

    if (x > C51LENGTH + COLS)  return ERR;
    if (x < - C51LENGTH) return OK;
    y = LINES / 2 - 5;

    if (FLY == 1) {
        y = -(x / 7) + LINES - (COLS / 7) - C51HEIGHT;
        dy = 1;
    }
    for (i = 0; i <= C51HEIGHT; ++i) {
        my_mvaddstr_r(y + i, x, c51[(C51LENGTH + x) % C51PATTERNS][i]);
        my_mvaddstr_r(y + i + dy, x - 55, coal[i]);
    }
    if (ACCIDENT == 1) {
        add_man(y + 3, x - 47);
        add_man(y + 3, x - 51);
    }
    if (DANCE == 1 && ACCIDENT ==0 && FLY == 0) {
        add_fdancer_r(y - 1, x - 47); add_mdancer_r(y - 1, x - 52);
    }
    add_smoke_r(y - 1, x - C51FUNNEL - 2);
    return OK;
}

int add_TGV(int x)
{
    static char *tgv[TGVPATTERNS][TGVHEIGHT + 1]
      = {{TGVSTR0, TGVSTR1, TGVSTR2, TGVSTR3, TGVSTR4, TGVSTR5, TGVSTR6,
            TGVWHL1, TGVDEL},
           {TGVSTR0, TGVSTR1, TGVSTR2, TGVSTR3, TGVSTR4, TGVSTR5, TGVSTR6,
            TGVWHL2, TGVDEL}};
    int y, i;

    if (x < - TGVLENGTH)  return ERR;
    y = LINES / 2 - 3;

    if (FLY == 1) {
        y = (x / 7) + LINES - (COLS / 7) - TGVHEIGHT;
    }
    for (i = 0; i <= TGVHEIGHT; ++i) {
        my_mvaddstr(y + i, x, tgv[((TGVLENGTH + x) / 2) % TGVPATTERNS][i]);
    }

    if (ACCIDENT == 1) {
        add_man(y + 2, x + 14);
    }
    if (DANCE == 1 && ACCIDENT ==0 && FLY == 0) {
        add_fdancer(y - 1, x + 45); add_mdancer(y - 1, x + 50);
    }
    return OK;
}

int add_vagoon(int x, int i_diff)
{
    static char *vagoon[TGVHEIGHT + 1]
        = {TGVVAG0, TGVVAG1, TGVVAG2, TGVVAG3, TGVVAG4, TGVVAG5, TGVVAG6, TGVVAG7, TGVDEL};

    int y, i;

    if (x < - TGVLENGTH)  return ERR;
    y = LINES / 2 - 3;

    if (FLY == 1) {
        y = (x / 7) + LINES - (COLS / 7) - TGVHEIGHT;
    }
    for (i = 0; i <= TGVHEIGHT; ++i) {
        my_mvaddstr(y + i_diff + i, x, vagoon[i]);
    }
    if (ACCIDENT == 1) {
        add_man(y + i_diff + 3, x + 30);
        add_man(y + i_diff + 3, x + 35);
        add_man(y + i_diff + 3, x + 40);
        add_man(y + i_diff + 3, x + 45);
    }
    if (DANCE == 1 && ACCIDENT ==0 && FLY == 0) {
        add_fdancer(y - 1, x + 18); add_mdancer(y - 1, x + 23);
        add_fdancer(y - 1, x + 31); add_mdancer(y - 1, x + 36);
        add_fdancer(y - 1, x + 45); add_mdancer(y - 1, x + 50);
    }
    return OK;
}

int add_TGV_r(int x)
{
    static char *tgv[TGVPATTERNS][TGVHEIGHT + 1]
      = {{TGVSTR0, TGVSTR1, TGVSTR2, TGVSTR3, TGVSTR4, TGVSTR5, TGVSTR6,
            TGVWHL1, TGVDEL},
           {TGVSTR0, TGVSTR1, TGVSTR2, TGVSTR3, TGVSTR4, TGVSTR5, TGVSTR6,
            TGVWHL2, TGVDEL}};
    int y, i;

    if (x > TGVLENGTH + COLS)  return ERR;
    if (x < -TGVLENGTH) return OK;
    y = LINES / 2 - 3;

    if (FLY == 1) {
        y = -(x / 7) + LINES - (COLS / 7) - TGVHEIGHT;
    }
    for (i = 0; i <= TGVHEIGHT; ++i) {
        my_mvaddstr_r(y + i, x, tgv[((TGVLENGTH + x) / 2) % TGVPATTERNS][i]);
    }

    if (ACCIDENT == 1) {
        add_man(y + 2, x - 12);
    }
    if (DANCE == 1 && ACCIDENT ==0 && FLY == 0) {
        add_fdancer_r(y - 1, x - 43); add_mdancer_r(y - 1, x - 48);
    }
    return OK;
}

int add_vagoon_r(int x, int i_diff)
{
    static char *vagoon[TGVHEIGHT + 1]
        = {TGVVAG0, TGVVAG1, TGVVAG2, TGVVAG3, TGVVAG4, TGVVAG5, TGVVAG6, TGVVAG7, TGVDEL};

    int y, i;

    if (x > TGVLENGTH + COLS)  return ERR;
    if (x < -TGVLENGTH) return OK;
    y = LINES / 2 - 3;

    if (FLY == 1) {
        y = -(x / 7) + LINES - (COLS / 7) - TGVHEIGHT;
    }
    for (i = 0; i <= TGVHEIGHT; ++i) {
        my_mvaddstr_r(y + i_diff + i, x, vagoon[i]);
    }
    if (ACCIDENT == 1) {
        add_man(y + i_diff + 3, x - 28);
        add_man(y + i_diff + 3, x - 33);
        add_man(y + i_diff + 3, x - 38);
        add_man(y + i_diff + 3, x - 43);
    }
    if (DANCE == 1 && ACCIDENT ==0 && FLY == 0) {
        add_fdancer_r(y - 1, x - 16); add_mdancer_r(y - 1, x - 21);
        add_fdancer_r(y - 1, x - 29); add_mdancer_r(y - 1, x - 34);
        add_fdancer_r(y - 1, x - 43); add_mdancer_r(y - 1, x - 48);
    }
    return OK;
}


void add_man(int y, int x)
{
    static char *man[2][2] = {{"", "(O)"}, {"Help!", "\\O/"}};
    int i;

    for (i = 0; i < 2; ++i) {
        my_mvaddstr(y + i, x, man[(LOGOLENGTH + x) / 12 % 2][i]);
    }
}


void add_fdancer(int y, int x)
{
   static char *fdancer[2][3] = {{"\\\\0", "/\\", "|\\"}, {"0//", "/\\", "/|"}};
   static char *Efdancer[2][3] = {{"   ", "  ", "  "}, {"   ", "  ", "  "}};
   int i, j=1;

   for (i = 0; i<3; ++i) {
        my_mvaddstr(y+i, x + j, Efdancer[(LOGOLENGTH + x) / 12 %2][i]);
        my_mvaddstr(y+i, x, fdancer[(LOGOLENGTH + x) / 12 %2][i]);
   }
}

void add_mdancer(int y, int x)
{
   static char *mdancer[3][3] = {{"_O_", " #", "/\\"}, {"(0)", " #", "/\\"}, {"(O_", " #", "/\\"}};
   static char *Emdancer[3][3] = {{"   ", "  ", "  "}, {"   ", "  ", "  "}, {"   ", "  ", "  "}};
   int i, j=1;

   for (i = 0; i<3; ++i) {
         my_mvaddstr(y+i, x + j, Emdancer[(LOGOLENGTH + x) / 12 %3][i]);
         my_mvaddstr(y+i, x, mdancer[(LOGOLENGTH + x) / 12 %3][i]);
   }
}

void add_fdancer_r(int y, int x)
{
   static char *fdancer[2][3] = {{"\\\\0", "/\\", "|\\"}, {"0//", "/\\", "/|"}};
   static char *Efdancer[2][3] = {{"   ", "  ", "  "}, {"   ", "  ", "  "}};
   int i, j=-1;

   for (i = 0; i<3; ++i) {
        my_mvaddstr(y+i, x + j, Efdancer[(LOGOLENGTH + x) / 12 %2][i]);
        my_mvaddstr(y+i, x, fdancer[(LOGOLENGTH + x) / 12 %2][i]);
   }
}

void add_mdancer_r(int y, int x)
{
   static char *mdancer[3][3] = {{"_O_", " #", "/\\"}, {"(0)", " #", "/\\"}, {"(O_", " #", "/\\"}};
   static char *Emdancer[3][3] = {{"   ", "  ", "  "}, {"   ", "  ", "  "}, {"   ", "  ", "  "}};
   int i, j=-1;

   for (i = 0; i<3; ++i) {
         my_mvaddstr(y+i, x + j, Emdancer[(LOGOLENGTH + x) / 12 %3][i]);
         my_mvaddstr(y+i, x, mdancer[(LOGOLENGTH + x) / 12 %3][i]);
   }
}


void add_smoke(int y, int x)
#define SMOKEPTNS        16
{
    static struct smokes {
        int y, x;
        int ptrn, kind;
    } S[1000];
    static int sum = 0;
    static char *Smoke[2][SMOKEPTNS]
        = {{"(   )", "(    )", "(    )", "(   )", "(  )",
            "(  )" , "( )"   , "( )"   , "()"   , "()"  ,
            "O"    , "O"     , "O"     , "O"    , "O"   ,
            " "                                          },
           {"(@@@)", "(@@@@)", "(@@@@)", "(@@@)", "(@@)",
            "(@@)" , "(@)"   , "(@)"   , "@@"   , "@@"  ,
            "@"    , "@"     , "@"     , "@"    , "@"   ,
            " "                                          }};
    static char *Eraser[SMOKEPTNS]
        =  {"     ", "      ", "      ", "     ", "    ",
            "    " , "   "   , "   "   , "  "   , "  "  ,
            " "    , " "     , " "     , " "    , " "   ,
            " "                                          };
    static int dy[SMOKEPTNS] = { 2,  1, 1, 1, 0, 0, 0, 0, 0, 0,
                                 0,  0, 0, 0, 0, 0             };
    static int dx[SMOKEPTNS] = {-2, -1, 0, 1, 1, 1, 1, 1, 2, 2,
                                 2,  2, 2, 3, 3, 3             };
    int i;

    if (x % 4 == 0) {
        for (i = 0; i < sum; ++i) {
            my_mvaddstr(S[i].y, S[i].x, Eraser[S[i].ptrn]);
            S[i].y    -= dy[S[i].ptrn];
            S[i].x    += dx[S[i].ptrn];
            S[i].ptrn += (S[i].ptrn < SMOKEPTNS - 1) ? 1 : 0;
            my_mvaddstr(S[i].y, S[i].x, Smoke[S[i].kind][S[i].ptrn]);
        }
        my_mvaddstr(y, x, Smoke[sum % 2][0]);
        S[sum].y = y;    S[sum].x = x;
        S[sum].ptrn = 0; S[sum].kind = sum % 2;
        sum ++;
    }
}

int add_smoke_r(int y, int x)
#define SMOKEPTNS	16
 {
  static struct smokes {
   int y, x;
   int ptrn, kind;
 } S[1000];
 static int sum = 0;
 static char *Smoke[2][SMOKEPTNS]
 = {{"(   )", "(    )", "(    )", "(   )", "(  )",
 "(  )" , "( )"   , "( )"   , "()"   , "()"  ,
 "O"    , "O"     , "O"     , "O"    , "O"   ,
 " "                                          },
 {"(@@@)", "(@@@@)", "(@@@@)", "(@@@)", "(@@)",
 "(@@)" , "(@)"   , "(@)"   , "@@"   , "@@"  ,
 "@"    , "@"     , "@"     , "@"    , "@"   ,
 " "                                          }};
 static char *Eraser[SMOKEPTNS]
 =  {"     ", "      ", "      ", "     ", "    ",
 "    " , "   "   , "   "   , "  "   , "  "  ,
 " "    , " "     , " "     , " "    , " "   ,
 " "                                          };
 static int dy[SMOKEPTNS] = { 2,  1, 1, 1, 0, 0, 0, 0, 0, 0,
   0,  0, 0, 0, 0, 0             };
   static int dx[SMOKEPTNS] = {-2, -1, 0, 1, 1, 1, 1, 1, 2, 2,
     2,  2, 2, 3, 3, 3             };
     int i;

    if (x > 2 * COLS) {
      return 0;
    }
    if (x % 4 == 0) {
     for (i = 0; i < sum; ++i) {
       my_mvaddstr_r(S[i].y, S[i].x, Eraser[S[i].ptrn]);
       S[i].y    -= dy[S[i].ptrn];
       S[i].x    -= dx[S[i].ptrn];
       S[i].ptrn += (S[i].ptrn < SMOKEPTNS - 1) ? 1 : 0;
       my_mvaddstr_r(S[i].y, S[i].x, Smoke[S[i].kind][S[i].ptrn]);
     }
     my_mvaddstr(y, x, Smoke[sum % 2][0]);
     S[sum].y = y;    S[sum].x = x;
     S[sum].ptrn = 0; S[sum].kind = sum % 2;
     sum ++;
   }
    return 0;
 }


int dirc(int y, int x, int dir)
{
  if (dir==0) {
    my_mvaddstr(y + 5, cros0l[5] + x - 1, "<-- ");
  } else {
    my_mvaddstr(y + 5, cros0l[5] + x - 1, " -->");
  }
  return 0;
}

int add_cross(int p, int dir)
{
  int i, y, n = 20, hn;
  static int tt;
#ifdef DEBUG
  char buf[100];
#endif

  hn = n / 2;
  y=get_y();
  for (i = 2; i < D51HEIGHT; ++i) {
    my_mvaddstr(y + i, cros0l[i] + p, cros0[i]);
  }
  for (i = 8; i < D51HEIGHT; ++i) {
    my_mvaddstr(y + i, p + 5, cros3[i]);
  }
#ifdef DEBUG
  sprintf(buf, "%d", tt);
  my_mvaddstr(0, 0, buf);
#endif
  if ( tt % n >= 0 && tt % n < hn) {
    my_mvaddstr(y + 4, cros0l[5] + p - 1, "O");
    my_mvaddstr(y + 4, cros0l[5] + p + 2, "X");
    my_mvaddstr(y + 5, cros0l[5] + p - 1, " || ");
  } else if ( tt % n >= hn && tt % n < n) {
    my_mvaddstr(y + 4, cros0l[5] + p - 1, "X");
    my_mvaddstr(y + 4, cros0l[5] + p + 2, "O");
    dirc(y, p, dir);
  }
  ++tt;

  return 0;
}


int begin_gate(int p, int dir)
{
  int i, y, n = 20, hn;
  int tt;

  hn = n / 2;
  y=get_y();

  for (tt = 0; tt < 80; ++tt) {
    for (i = 0; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, p + 5, cros1[i]);
    }
    for (i = 2; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, cros0l[i] + p, cros0[i]);
    }
    if ( tt % n >= 0 && tt % n < hn) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "O");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "X");
      my_mvaddstr(y + 5, cros0l[5] + p - 1, " || ");
    } else if ( tt % n >= hn && tt % n < n) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "X");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "O");
      dirc(y, p, dir);
    }
    my_mvaddstr(LINES + 1, COLS + 1, "");
    refresh();
    usleep(WAIT_TIME);
  }
  for (tt = 0; tt <= 15; ++tt) {
    for (i = 0; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, p + 5, cros2[i]);
    }
    for (i = 2; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, cros0l[i] + p, cros0[i]);
    }
    if ( tt % n >= 0 && tt % n < hn) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "O");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "X");
      my_mvaddstr(y + 5, cros0l[5] + p - 1, " || ");
    } else if ( tt % n >= hn && tt % n < n) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "X");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "O");
      dirc(y, p, dir);
    }
    my_mvaddstr(LINES + 1, COLS + 1, "");
    refresh();
    usleep(WAIT_TIME);
  }
  for (tt = 0; tt <= 20; ++tt) {
    for (i = 0; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, p + 5, cros3[i]);
    }
    for (i = 2; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, cros0l[i] + p, cros0[i]);
    }
    if ( tt % n >= 0 && tt % n < hn) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "O");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "X");
      my_mvaddstr(y + 5, cros0l[5] + p - 1, " || ");
    } else if ( tt % n >= hn && tt % n < n) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "X");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "O");
      dirc(y, p, dir);
    }
    my_mvaddstr(LINES + 1, COLS + 1, "");
    refresh();
    usleep(WAIT_TIME);
  }

  return 0;
}


int end_gate(int p, int dir)
{
  int i, y, n = 20, hn;
  int tt;

  hn = n / 2;
  y=get_y();

  for (tt = 0; tt <= 20; ++tt) {
    for (i = 0; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, p + 5, cros3[i]);
    }
    for (i = 2; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, cros0l[i] + p, cros0[i]);
    }
    if ( tt % n >= 0 && tt % n < hn) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "O");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "X");
      my_mvaddstr(y + 5, cros0l[5] + p - 1, " || ");
    } else if ( tt % n >= hn && tt % n < n) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "X");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "O");
      dirc(y, p, dir);
    }
    my_mvaddstr(LINES + 1, COLS + 1, "");
    refresh();
    usleep(WAIT_TIME);
  }
  for (tt = 0; tt <= 15; ++tt) {
    for (i = 0; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, p + 5, cros2[i]);
    }
    for (i = 2; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, cros0l[i] + p, cros0[i]);
    }
    if ( tt % n >= 0 && tt % n < hn) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "O");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "X");
      my_mvaddstr(y + 5, cros0l[5] + p - 1, " || ");
    } else if ( tt % n >= hn && tt % n < n) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "X");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "O");
      dirc(y, p, dir);
    }
    my_mvaddstr(LINES + 1, COLS + 1, "");
    refresh();
    usleep(WAIT_TIME);
  }
  for (tt = 0; tt < 80; ++tt) {
    for (i = 0; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, p + 5, cros1[i]);
    }
    for (i = 2; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, cros0l[i] + p, cros0[i]);
    }
    my_mvaddstr(y + 4, cros0l[5] + p - 1, "X");
    my_mvaddstr(y + 4, cros0l[5] + p + 2, "X");
    my_mvaddstr(LINES + 1, COLS + 1, "");
    my_mvaddstr(y + 5, cros0l[5] + p - 1, " || ");
    refresh();
    usleep(WAIT_TIME);
  }

  return 0;
}


int x_gate(int p, int dir)
{
  int i, y, n = 20, hn;
  int tt;

  hn = n / 2;
  y = get_y();

  for (tt = 0; tt <= 20; ++tt) {
    for (i = 0; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, p + 5, cros3[i]);
    }
    for (i = 2; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, cros0l[i] + p, cros0[i]);
    }
    if ( tt % n >= 0 && tt % n < hn) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "O");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "X");
      my_mvaddstr(y + 5, cros0l[5] + p - 1, " || ");
    } else if ( tt % n >= hn && tt % n < n) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "X");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "O");
      dirc(y, p, dir);
    }
    my_mvaddstr(LINES + 1, COLS + 1, "");
    refresh();
    usleep(WAIT_TIME);
  }
  for (tt = 0; tt <= 10; ++tt) {
    for (i = 0; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, p + 5, cros2[i]);
    }
    for (i = 2; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, cros0l[i] + p, cros0[i]);
    }
    if ( tt % n >= 0 && tt % n < hn) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "O");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "X");
      my_mvaddstr(y + 5, cros0l[5] + p - 1, " || ");
    } else if ( tt % n >= hn && tt % n < n) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "X");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "O");
      dirc(y, p, dir);
    }
    my_mvaddstr(LINES + 1, COLS + 1, "");
    refresh();
    usleep(WAIT_TIME);
  }
  dir = ( dir + 1 ) % 2;
  for (tt = 0; tt <= 10; ++tt) {
    for (i = 0; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, p + 5, cros2[i]);
    }
    for (i = 2; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, cros0l[i] + p, cros0[i]);
    }
    if ( tt % n >= 0 && tt % n < hn) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "O");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "X");
      my_mvaddstr(y + 5, cros0l[5] + p - 1, " || ");
    } else if ( tt % n >= hn && tt % n < n) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "X");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "O");
      dirc(y, p, dir);
    }
    my_mvaddstr(LINES + 1, COLS + 1, "");
    refresh();
    usleep(WAIT_TIME);
  }
  for (tt = 0; tt <= 20; ++tt) {
    for (i = 0; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, p + 5, cros3[i]);
    }
    for (i = 2; i < D51HEIGHT; ++i) {
      my_mvaddstr(y + i, cros0l[i] + p, cros0[i]);
    }
    if ( tt % n >= 0 && tt % n < hn) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "O");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "X");
      my_mvaddstr(y + 5, cros0l[5] + p - 1, " || ");
    } else if ( tt % n >= hn && tt % n < n) {
      my_mvaddstr(y + 4, cros0l[5] + p - 1, "X");
      my_mvaddstr(y + 4, cros0l[5] + p + 2, "O");
      dirc(y, p, dir);
    }
    my_mvaddstr(LINES + 1, COLS + 1, "");
    refresh();
    usleep(WAIT_TIME);
  }

  return 0;
}
