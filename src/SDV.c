#include <stdio.h>
#include <stdlib.h>
#include <curses.h>

int main(void)
{
  //puts("Hello World!!!"); /* prints Hello World!!! */
  initscr();
  // cbreak();
  // noecho();
  // keypad(stdscr, TRUE);

  printw("Hello World!!!");
  refresh();
  getch();

  // int height, width;

  // getmaxyx(stdscr, height, width);
  // WINDOW * win = newwin(height, width, 0, 0);
  // wrefresh(win);
  endwin();

  return EXIT_SUCCESS;
}
