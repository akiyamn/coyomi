#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

void days_from_today(char *buf, size_t buf_size, int days_offset) {
	time_t rawtime;
	struct tm *newtime;
   	time(&rawtime);
	rawtime += days_offset * 24 * 60 * 60;
	newtime = localtime(&rawtime);
	strftime(buf, buf_size, "%A %e %b", newtime);
}

void draw_week_names(WINDOW ** days, int week_offset, int selected) {
	for (int i = 0; i < 7; i++){
		char day_name[50];
		days_from_today(day_name, 50, i-week_offset);
		if (selected == i) {wattron(days[i], A_REVERSE);}
		mvwprintw(days[i], 0, 1, day_name);
		wattroff(days[i], A_REVERSE);
		wrefresh(days[i]);
	}
}


void draw_week(WINDOW ** days, int week_offset, int selected) {
	int xmax, ymax;
	getmaxyx(stdscr, ymax, xmax);
	for (int i = 0; i < 7; i++){
		// char day_name[50];
		// days_from_today(day_name, 50, i-week_offset);
		days[i] = newwin(5, xmax/4, i*5, 0);
		box(days[i], 0, 0);
		
	}
	refresh();	
	draw_week_names(days, week_offset, selected);
	// for (int i = 0; i < 7; i++){
	// 	wrefresh(days[i]);
	// }
	return;
}

int main() {

	time_t today_raw;
	struct tm *today;
	int xmax, ymax;
	int week_offset;
	int selected;

	time(&today_raw);
	today = localtime(&today_raw);
	week_offset = today->tm_wday-1;

	setlocale(LC_ALL, "");
	initscr();
	cbreak();	
	noecho();
	curs_set(0);

	getmaxyx(stdscr, ymax, xmax);

	WINDOW * days[7];
	WINDOW * mainwin;

	selected = week_offset;
	draw_week(days, week_offset, selected);

	mainwin = newwin(7*5, xmax*(3/4), 0, xmax/4+1);
	refresh();
	box(mainwin, 0, 0);
	char now_string[50];
	// time_t yyy = time(NULL);
	// struct tm *yyyy = localtime(&yyy);
	days_from_today(now_string, 50, 0);
	// strftime(now_string, 50, "%A %e %B, %Y", yyyy);
	mvwprintw(mainwin, 0, 1, now_string);
	wrefresh(mainwin);

	while (1) {
		char c = getch();
		if (c == 'q') {
			break;
		}
		if (c >= '1' && c <= '7') {
			selected = c - 49;
			draw_week(days, week_offset, selected);
			refresh();
		}
	}
	
	// char c = getch();
	// if (c == 'j') {
	// 	printw("Congratulations, you've pressed the letter 'j'.");
	// 	refresh();
	// }
	// keypad(mainwin, TRUE);
	// int ch;
	// while ((ch = getch()) != 'q') { // not Ctrl_C
    //     printw("%s",ch);
	// 	refresh();
    // }

	endwin();
	
	return 0;
}
