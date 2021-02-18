#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <ncurses.h>

#define DAYS_IN_WEEK 7
#define DAY_NAME_SIZE 50
#define MAIN_TEXT_SIZE 5000
#define ASCII_DIGIT_START 48
#define SECONDS_IN_DAY 86400
#define LAYOUT_X_RATIO 1/4

/*
	Returns num_of_days to the time_t input given.
*/
time_t add_days(time_t input, int num_of_days) {
	return input + (num_of_days * SECONDS_IN_DAY);
}


/*
	Returns the time_t of the Monday on the week of the input time_t
*/
time_t get_monday(time_t input) {
	time_t time_copy = input;
	struct tm *time_obj;
	unsigned int days_from_monday;
	time_obj = localtime(&time_copy);
	days_from_monday = (time_obj->tm_wday == 0) ? DAYS_IN_WEEK-1 : time_obj->tm_wday-1;
	return add_days(time_copy, -days_from_monday);
}


/*
	Returns a formatted string date given a time_t time, buffer and appropriate size
	Verbose, if true: returns a longer date string including the year.
*/
void date_string(char *buffer, size_t buf_size, time_t time, int verbose) {
	struct tm *time_obj;
	time_obj = localtime(&time);
	if (verbose) {
		strftime(buffer, buf_size, "%A %e %B, %Y", time_obj);
	} else {
		strftime(buffer, buf_size, "%A %e %b", time_obj);
	}
}

/*
	Draws the string date labels for the week panel, given a list of windows for each day and the selected day
*/
void draw_week_names(WINDOW ** day_windows, time_t selected) {
	for (int i = 0; i < DAYS_IN_WEEK; i++){
		char day_name[DAY_NAME_SIZE];
		time_t monday, this_day;
		monday = get_monday(selected);
		this_day = add_days(monday, i);
		date_string(day_name, DAY_NAME_SIZE, this_day, false); // Get the date string
		if (this_day == selected) {wattron(day_windows[i], A_REVERSE);} // Highlight the selected day
		mvwprintw(day_windows[i], 0, 1, day_name); // Place the date string onto the screen
		wattroff(day_windows[i], A_REVERSE);
		wrefresh(day_windows[i]);
	}
}


/*
	Draws the entire 7 week panel including labels and boxes, given a memory location for the window representing each day
*/
void draw_week(WINDOW ** days, time_t selected) {
	int ymax, xmax, ysize, xsize, ypos, xpos;
	getmaxyx(stdscr, ymax, xmax);
	for (int i = 0; i < DAYS_IN_WEEK; i++){
		ysize = ymax/DAYS_IN_WEEK;
		xsize = xmax*LAYOUT_X_RATIO;
		ypos = i*(ymax/DAYS_IN_WEEK);
		xpos = 0;
		days[i] = newwin(ysize, xsize, ypos, xpos);
		box(days[i], 0, 0);
		
	}
	refresh();	
	draw_week_names(days, selected);
	return;
}

/*
	Draws the date label for the main window, given a pointer to the main window and the selected day
*/
void draw_main_window_text(WINDOW ** mainwin, time_t selected){
	char now_string[DAY_NAME_SIZE];
	date_string(now_string, DAY_NAME_SIZE, selected, true);
	box(*mainwin, 0, 0);
	mvwprintw(*mainwin, 0, 1, now_string);
	wrefresh(*mainwin);
}

/*
	Draws the entire main window (not including read data), given a pointer to the main window and the selected day
*/
void draw_main_window(WINDOW ** mainwin, time_t selected) {
	int ymax, xmax, ysize, xsize, ypos, xpos;
	getmaxyx(stdscr, ymax, xmax);
	ysize = (ymax/DAYS_IN_WEEK)*DAYS_IN_WEEK;
	xsize = xmax*(1-LAYOUT_X_RATIO);
	ypos = 0;
	xpos = xmax*LAYOUT_X_RATIO+1;
	*mainwin = newwin(ysize, xsize, ypos, xpos);
	refresh();
	box(*mainwin, 0, 0);
	draw_main_window_text(mainwin, selected);
}

/*
	Reads a given day's entry data from file and puts it into the given buffer (given the buffer's size).
	Returns true on success and false on failure.
*/
int read_day(char *buffer, size_t buf_size, time_t selected){
	FILE *fp;
	char file_name[DAY_NAME_SIZE];
	struct tm *time_obj;
	time_obj = localtime(&selected);
	strftime(file_name, DAY_NAME_SIZE, "entries/%F.md", time_obj);
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
		return true;
	}
	return false;
}

/* The main body function */
int main() {

	time_t today_raw;
	time_t selected;

	time(&today_raw);

	setlocale(LC_ALL, "");
	initscr();
	cbreak();	
	noecho();
	curs_set(0);
	keypad(stdscr, true);

	WINDOW * days[DAYS_IN_WEEK];
	WINDOW * mainwin;
	char text[MAIN_TEXT_SIZE];

	selected = today_raw;
	draw_week(days, selected);

	draw_main_window(&mainwin, selected);
	read_day(text, MAIN_TEXT_SIZE, selected);
	mvwprintw(mainwin, 1, 2, text);
	wrefresh(mainwin);

	while (1) {
		char c = getch();
		if (c == 'q') {
			break;
		}
		else if (c >= '1' && c <= '7') {
			selected = add_days(get_monday(selected), c-ASCII_DIGIT_START-1);
			draw_week(days, selected);
			draw_main_window(&mainwin, selected);
			if (read_day(text, MAIN_TEXT_SIZE, selected)){
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
