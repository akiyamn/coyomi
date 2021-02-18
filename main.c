#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

time_t add_days(time_t input, int num_of_days) {
	return input + (num_of_days * 24 * 60 * 60);
}

time_t get_monday(time_t input) {
	time_t time_copy = input;
	struct tm *newtime;
	unsigned int days_from_monday;
	newtime = localtime(&time_copy);
	days_from_monday = (newtime->tm_wday == 0) ? 6 : newtime->tm_wday-1;
	return add_days(time_copy, -days_from_monday);
}

void date_string(char *buffer, size_t buf_size, time_t time, int verbose) {
	struct tm *newtime;
	newtime = localtime(&time);
	if (verbose) {
		strftime(buffer, buf_size, "%A %e %B, %Y", newtime);
	} else {
		strftime(buffer, buf_size, "%A %e %b", newtime);
	}
}

// void days_from_today(char *buffer, size_t buf_size, int days_offset, int verbose) {
// 	time_t rawtime;
// 	struct tm *newtime;
//    	time(&rawtime);
// 	rawtime += days_offset * 24 * 60 * 60;
// 	newtime = localtime(&rawtime);
// 	if (verbose) {
// 		strftime(buffer, buf_size, "%A %e %B, %Y", newtime);
// 	} else {
// 		strftime(buffer, buf_size, "%A %e %b", newtime);
// 	}
	
// }


// void draw_week_names_r(WINDOW ** days, int week_offset, int selected) {
// 	for (int i = 0; i < 7; i++){
// 		char day_name[50];
// 		days_from_today(day_name, 50, i-week_offset, false);
// 		if (selected == i) {wattron(days[i], A_REVERSE);}
// 		mvwprintw(days[i], 0, 1, day_name);
// 		wattroff(days[i], A_REVERSE);
// 		wrefresh(days[i]);
// 	}
// }

void draw_week_names(WINDOW ** day_windows, time_t selected) {
	for (int i = 0; i < 7; i++){
		char day_name[50];
		time_t monday = get_monday(selected);
		time_t this_day = add_days(monday, i);
		date_string(day_name, 50, this_day, false);
		if (this_day == selected) {wattron(day_windows[i], A_REVERSE);}
		mvwprintw(day_windows[i], 0, 1, day_name);
		wattroff(day_windows[i], A_REVERSE);
		wrefresh(day_windows[i]);
	}
}


// void draw_week_r(WINDOW ** days, int week_offset, int selected) {
// 	int xmax, ymax;
// 	getmaxyx(stdscr, ymax, xmax);
// 	for (int i = 0; i < 7; i++){
// 		days[i] = newwin(5, xmax/4, i*5, 0);
// 		box(days[i], 0, 0);
		
// 	}
// 	refresh();	
// 	draw_week_names(days, week_offset, selected);
// 	return;
// }

void draw_week(WINDOW ** days, time_t selected) {
	int xmax, ymax;
	getmaxyx(stdscr, ymax, xmax);
	for (int i = 0; i < 7; i++){
		days[i] = newwin(5, xmax/4, i*5, 0);
		box(days[i], 0, 0);
		
	}
	refresh();	
	draw_week_names(days, selected);
	return;
}

void draw_main_window_text(WINDOW ** mainwin, time_t selected){
	char now_string[50];
	date_string(now_string, 50, selected, true);
	box(*mainwin, 0, 0);
	mvwprintw(*mainwin, 0, 1, now_string);
	wrefresh(*mainwin);
}

void draw_main_window(WINDOW ** mainwin, time_t selected) {
	int xmax, ymax;
	getmaxyx(stdscr, ymax, xmax);
	*mainwin = newwin(7*5, xmax*(3/4), 0, xmax/4+1);
	refresh();
	box(*mainwin, 0, 0);
	draw_main_window_text(mainwin, selected);
}

int read_day(char *buffer, size_t buf_size, time_t selected){
	FILE *fp;
	char file_name[50];
	struct tm *newtime;
	newtime = localtime(&selected);
	strftime(file_name, 50, "entries/%F.md", newtime);
	if ((fp = fopen(file_name, "r"))) {
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
	// struct tm *today;
	int xmax, ymax;
	// int week_offset;
	time_t selected;

	time(&today_raw);
	// today = localtime(&today_raw);
	// week_offset = today->tm_wday-1;

	setlocale(LC_ALL, "");
	initscr();
	cbreak();	
	noecho();
	curs_set(0);

	getmaxyx(stdscr, ymax, xmax);

	WINDOW * days[7];
	WINDOW * mainwin;
	char text[5000];

	selected = today_raw;
	draw_week(days, selected);

	draw_main_window(&mainwin, selected);
	read_day(text, 5000, selected);
	mvwprintw(mainwin, 1, 2, text);
	wrefresh(mainwin);

	while (1) {
		char c = getch();
		if (c == 'q') {
			break;
		}
		else if (c >= '1' && c <= '7') {
			selected = add_days(get_monday(selected), c-48-1);
			draw_week_names(days, selected);
			draw_main_window(&mainwin, selected);
			if (read_day(text, 5000, selected)){
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
