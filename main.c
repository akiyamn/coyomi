#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

void days_from_today(char *buf, size_t buf_size, int days_offset, int verbose) {
	time_t rawtime;
	struct tm *newtime;
   	time(&rawtime);
	rawtime += days_offset * 24 * 60 * 60;
	newtime = localtime(&rawtime);
	if (verbose) {
		strftime(buf, buf_size, "%A %e %B, %Y", newtime);
	}
	strftime(buf, buf_size, "%A %e %b", newtime);
	
}

void draw_week_names(WINDOW ** days, int week_offset, int selected) {
	for (int i = 0; i < 7; i++){
		char day_name[50];
		days_from_today(day_name, 50, i-week_offset, false);
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
		days[i] = newwin(5, xmax/4, i*5, 0);
		box(days[i], 0, 0);
		
	}
	refresh();	
	draw_week_names(days, week_offset, selected);
	return;
}

void draw_main_window_text(WINDOW ** mainwin, int week_offset, int selected){
	char now_string[50];
	// time_t yyy = time(NULL);
	// struct tm *yyyy = localtime(&yyy);
	days_from_today(now_string, 50, selected-week_offset, true);
	// strftime(now_string, 50, "%A %e %B, %Y", yyyy);
	box(*mainwin, 0, 0);
	mvwprintw(*mainwin, 0, 1, now_string);
	wrefresh(*mainwin);
}

void draw_main_window(WINDOW ** mainwin, int week_offset, int selected) {
	int xmax, ymax;
	getmaxyx(stdscr, ymax, xmax);
	*mainwin = newwin(7*5, xmax*(3/4), 0, xmax/4+1);
	refresh();
	box(*mainwin, 0, 0);
	draw_main_window_text(mainwin, week_offset, selected);
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

	draw_main_window(&mainwin, week_offset, selected);

	while (1) {
		char c = getch();
		if (c == 'q') {
			break;
		}
		else if (c >= '1' && c <= '7') {
			selected = c - 49;
			draw_week_names(days, week_offset, selected);
			draw_main_window_text(&mainwin, week_offset, selected);
		}
	}

	endwin();
	return 0;
}
