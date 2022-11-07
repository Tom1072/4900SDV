
//#include <stdio.h>
//#include <stdlib.h>
//#include <curses.h>
//#include <unistd.h>
//#include <sys/neutrino.h>


int main1(void) {
	int count = 0;
	while(1) {
//		clearscr();
		printf("Cleared the screen: %d\n", count++ );
//		sleep(1);
	}
	return EXIT_SUCCESS;
}

int main2(void)
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
