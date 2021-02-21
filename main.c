#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h>
#include <time.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <ncurses.h>

#define DAYS_IN_WEEK 7
#define DAY_NAME_SIZE 50
#define MAIN_TEXT_SIZE 5000
#define ASCII_DIGIT_START 48
#define SECONDS_IN_DAY 86400
#define LAYOUT_X_RATIO 0.25
#define MAIN_PADDING 2

// Will hard code these for now...
#define TERMINAL "st"
#define EDITOR "vim"

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
	WINDOW *mainwin_border;
	getmaxyx(stdscr, ymax, xmax);
	ysize = (ymax/DAYS_IN_WEEK)*DAYS_IN_WEEK;
	xsize = xmax*(1-LAYOUT_X_RATIO);
	ypos = 0;
	xpos = xmax*LAYOUT_X_RATIO+1;
	mainwin_border = newwin(ysize, xsize, ypos, xpos);
	*mainwin = newwin(ysize-MAIN_PADDING*2, xsize-MAIN_PADDING*2, ypos+MAIN_PADDING, xpos+MAIN_PADDING);
	refresh();
	box(mainwin_border, 0, 0);
	draw_main_window_text(&mainwin_border, selected);
	wrefresh(mainwin_border);
}

void draw_command_window(WINDOW ** comwin) {
	int ymax, xmax;
	getmaxyx(stdscr, ymax, xmax);
	*comwin = newwin(1, xmax-1, (ymax/DAYS_IN_WEEK)*DAYS_IN_WEEK, 0);
	refresh();
}

/*
	Creates the file path and name for a given time and stores it in the filename buffer
*/
void date_filename(char *filename, time_t time) {
	struct tm *time_obj;
	time_obj = localtime(&time);
	strftime(filename, DAY_NAME_SIZE, "entries/%F.md", time_obj);
}

/*
	Reads a given day's entry data from file and puts it into the given buffer (given the buffer's size).
	Returns true on success and false on failure.
*/
int read_day(char *buffer, size_t buf_size, time_t selected){
	FILE *fp;
	char file_name[DAY_NAME_SIZE];
	date_filename(file_name, selected);
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

/*
	Draw the entry text on a given panel for a given day, provided a text buffer (that will be zeroed).
*/
void draw_date_entry(WINDOW ** mainwin, time_t selected, char *text_buffer) {
	memset(text_buffer, '\0', MAIN_TEXT_SIZE);
	if (read_day(text_buffer, MAIN_TEXT_SIZE, selected)){
		mvwprintw(*mainwin, 0, 0, text_buffer);
	} else {
		mvwprintw(*mainwin, 0, 0, "Empty.");
	}
	wrefresh(*mainwin);
}

/*
	Updates the entire calendar part of the UI, to be used when loading a new date.
	Requires a pointer to a location for an array of day windows, pointer to main window, 
		the selected date and a buffer for the entry text
*/
void update_ui(WINDOW ** days, WINDOW ** mainwin, time_t selected, char *text_buffer) {
	draw_week(days, selected);
	draw_main_window(mainwin, selected);
	draw_date_entry(mainwin, selected, text_buffer);
}


/*
	Open a program (via fork & exec) in order to edit a given day's entry, 
	flush the text buffer, then redraw appropriate UI elements.
*/
void edit_date(WINDOW ** mainwin, time_t selected, char *text_buffer) {
	char filename[DAY_NAME_SIZE];
	date_filename(filename, selected);
	int pid = fork();
	refresh();
	if (pid == 0) {
		execlp(TERMINAL, TERMINAL, EDITOR, filename, NULL);
	} else if (pid == -1) {
		printw("An error occurred opening the process.");
		refresh();
	} else {
		int status;
		waitpid(pid, &status, 0);
		draw_main_window(mainwin, selected);
		draw_date_entry(mainwin, selected, text_buffer);
	}
}

int is_digit(int c) {
	return c >= '0' && c <= '9';
}

time_t parse_vi_command(int *command, size_t size, time_t selected) {
	int num_times = 0;
	int i;
	int digit_value;
	int letter_command = command[size-1];
	if (size > 1) {
		for (i = 0; i<size-1; i++) {
			if (is_digit(command[i])) {
				digit_value = command[i]-48;
				num_times += digit_value * pow(10, i);
			}
		}
	} else {
		num_times = 1;
	}
	for (i = 0; i<num_times; i++) {
		switch (letter_command) {
			case KEY_DOWN:
			case 'd': 
				selected = add_days(selected, 1); 
				break;
			case KEY_UP:
			case 'D': selected = add_days(selected, -1); break;
			case KEY_RIGHT:
			case 'w': selected = add_days(selected, 7); break;
			case KEY_LEFT:
			case 'W': selected = add_days(selected, -7); break;
			case 'f': selected = add_days(selected, 14); break;
			case 'F': selected = add_days(selected, -14); break;
			case 'm': selected = add_days(selected, 30); break;
			case 'M': selected = add_days(selected, -30); break;
			case 'y': selected = add_days(selected, 365); break;
			case 'Y': selected = add_days(selected, -365); break;
		}
	} 
	return selected;

}

int input_vi_command(WINDOW *comwin, int* command_buffer, int first_char) {
	size_t command_size = 1;
	wmove(comwin, 0, 0);
	wclear(comwin);
	wrefresh(comwin);
	command_buffer[0] = first_char;
	while (is_digit(command_buffer[command_size-1])) {
		wprintw(comwin, "%c", command_buffer[command_size-1]);
		wrefresh(comwin);
		command_buffer[command_size++] = getch();
	}
	wclear(comwin);
	wrefresh(comwin);
	return command_size;
}

/*
	The main UI loop which renders the ncurses UI and handles character input
*/
void ui_loop() {
	time_t selected, today_raw;
	char text_buffer[MAIN_TEXT_SIZE];
	int command_buffer[50];
	size_t command_size;
	WINDOW * days[DAYS_IN_WEEK];
	WINDOW * mainwin;
	WINDOW * comwin;

	time(&today_raw);
	selected = today_raw;

	update_ui(days, &mainwin, selected, text_buffer); // Update once before loop starts
	draw_command_window(&comwin);
	while (true) {
		int c = getch();
		wrefresh(comwin);
		switch(c) {
			case 'q': return; // Quit the program on 'q'
			case 'e': edit_date(&mainwin, selected, text_buffer); break;
			case ' ': 
				time(&selected);
				update_ui(days, &mainwin, selected, text_buffer);
				break;
		default:

			memset(command_buffer, '\0', 50*sizeof(int));
			command_size = input_vi_command(comwin, command_buffer, c);
			selected = parse_vi_command(command_buffer, command_size, selected);
			update_ui(days, &mainwin, selected, text_buffer);
			break;
		}

	}
}



/* The main body function */
int main() {
	setlocale(LC_ALL, "");
	initscr();
	// cbreak();	
	noecho();
	curs_set(0);
	keypad(stdscr, true);

	ui_loop();

	endwin();
	return 0;
}
