#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

void days_from_today(char *buffer, size_t buf_size, int days_offset, int verbose) {
	time_t rawtime;
	struct tm *newtime;
   	time(&rawtime);
	rawtime += days_offset * 24 * 60 * 60;
	newtime = localtime(&rawtime);
	if (verbose) {
		strftime(buffer, buf_size, "%A %e %B, %Y", newtime);
	} else {
		strftime(buffer, buf_size, "%A %e %b", newtime);
	}
	
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
	days_from_today(now_string, 50, selected-week_offset, true);
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

int read_day(char *buffer, size_t buf_size, int relative){
	FILE *fp;
	char file_name[50];
	time_t rawtime;
	struct tm *newtime;
   	time(&rawtime);
	rawtime += relative * 24 * 60 * 60;
	newtime = localtime(&rawtime);
	strftime(file_name, 50, "entries/%F.md", newtime);
	if (fp = fopen(file_name, "r")) {
		int i = 0;
		char c;
		while (i < buf_size) {
			c = fgetc(fp);
			if (feof(fp)) {
				break;
			}
			buffer[i++] = c;
		}
		fclose(fp);
		return 1;
	}
	return 0;
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
	char text[5000];

	selected = week_offset;
	draw_week(days, week_offset, selected);

	draw_main_window(&mainwin, week_offset, selected);
	read_day(text, 5000, selected-week_offset);
	mvwprintw(mainwin, 1, 2, text);
	wrefresh(mainwin);

	while (1) {
		char c = getch();
		if (c == 'q') {
			break;
		}
		else if (c >= '1' && c <= '7') {
			selected = c - 49;
			draw_week_names(days, week_offset, selected);
			draw_main_window(&mainwin, week_offset, selected);
			if (read_day(text, 5000, selected-week_offset)){
				mvwprintw(mainwin, 1, 2, text);
			} else {
				mvwprintw(mainwin, 1, 2, "Empty.");
			}
			wrefresh(mainwin);
		}
	}

	endwin();
	return 0;
}
