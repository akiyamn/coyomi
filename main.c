#include "main.h"

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
	Creates the file path and name for a given time and stores it in the filename buffer
*/
void date_filename(char *filename, time_t time) {
	struct tm *time_obj;
	char date_buffer[DAY_NAME_SIZE];
	time_obj = localtime(&time);
	char prefix[50];
	strftime(date_buffer, DAY_NAME_SIZE, "%F.md", time_obj);
	if (ENTRY_PATH[0] == '~') { // Expand ~ to the home directory
		strncpy(prefix, ENTRY_PATH, 50);
		// Put the home directory, default folder and date filename into the buffer.
		// Remove the ~ and / by starting the prefix string one character over
		snprintf(filename,  PATH_MAX, "%s%s/%s", getenv("HOME"), prefix+(sizeof(char)), date_buffer);
	} else { // No home directory, just treat as normal
		snprintf(filename, PATH_MAX, "%s/%s", ENTRY_PATH, date_buffer);
	}
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
	Return a win_dims_t object describing the dimensions of a given day window in a week
*/
win_dims_t dims_day_window(int offset) {
	int ymax, xmax;
	getmaxyx(stdscr, ymax, xmax);
	win_dims_t dims;
	dims.ysize = ymax/DAYS_IN_WEEK;
	dims.xsize = xmax*LAYOUT_X_RATIO;
	dims.ypos = offset*(ymax/DAYS_IN_WEEK);
	dims.xpos = 0;
	return dims;
}

/*
	Draws the string date labels for the week panel, given a list of windows for each day and the selected day
*/
void draw_week_windows(WINDOW ** day_windows, time_t selected) {
	for (int i = 0; i < DAYS_IN_WEEK; i++){
		wclear(day_windows[i]);
		char day_name[DAY_NAME_SIZE];
		time_t monday, this_day;
		monday = get_monday(selected);
		this_day = add_days(monday, i);
		date_string(day_name, DAY_NAME_SIZE, this_day, false); // Get the date string
		box(day_windows[i], 0, 0);
		if (this_day == selected) {wattron(day_windows[i], A_REVERSE);} // Highlight the selected day
		mvwprintw(day_windows[i], 0, 1, day_name); // Place the date string onto the screen
		wattroff(day_windows[i], A_REVERSE);
		wrefresh(day_windows[i]);
	}
}

/*
	Draws the entire 7 week panel including labels and boxes, given a memory location for the window representing each day
*/
void create_week_window(WINDOW ** days, time_t selected) {
	win_dims_t dims;
	for (int i = 0; i < DAYS_IN_WEEK; i++){
		dims = dims_day_window(i);
		days[i] = newwin(dims.ysize, dims.xsize, dims.ypos, dims.xpos);
		box(days[i], 0, 0);
		
	}
	refresh();	
	draw_week_windows(days, selected);
	return;
}

/*
	Return a win_dims_t object describing the dimensions of the main window
*/
win_dims_t dims_main_window() {
	int ymax, xmax;
	getmaxyx(stdscr, ymax, xmax);
	win_dims_t dims;
	dims.ysize = (ymax/DAYS_IN_WEEK)*DAYS_IN_WEEK;
	dims.xsize = xmax*(1-LAYOUT_X_RATIO);
	dims.ypos = 0;
	dims.xpos = xmax*LAYOUT_X_RATIO+1;
	return dims;
}

/*
	Return a win_dims_t object describing the dimensions of the main window
*/
win_dims_t dims_text_window() {
	win_dims_t dims = dims_main_window();
	dims.ysize += -MAIN_PADDING*2;
	dims.xsize += -MAIN_PADDING*2;
	dims.ypos += MAIN_PADDING;
	dims.xpos += MAIN_PADDING;
	return dims;
}

/*
	Draws the date label and border for the main window, given a pointer to the main window and the selected day
*/
void draw_main_window(WINDOW ** mainwin, WINDOW ** textwin, time_t selected){
	char now_string[DAY_NAME_SIZE];
	wclear(*mainwin);
	wclear(*textwin);
	date_string(now_string, DAY_NAME_SIZE, selected, true);
	box(*mainwin, 0, 0);
	mvwprintw(*mainwin, 0, 1, now_string);
	wrefresh(*mainwin);
}

/*
	Creates and draws the entire main window (not including read data), given a pointer to the main window and the selected day
*/
void create_main_window(WINDOW ** mainwin, WINDOW ** textwin, time_t selected) {
	win_dims_t dims = dims_main_window();
	*mainwin = newwin(dims.ysize, dims.xsize, dims.ypos, dims.xpos);
	*textwin = newwin(dims.ysize-MAIN_PADDING*2, dims.xsize-MAIN_PADDING*2, dims.ypos+MAIN_PADDING, dims.xpos+MAIN_PADDING);
	refresh();
	draw_main_window(mainwin, textwin, selected);
}


/*
	Return a win_dims_t object describing the dimensions of the command window
*/
win_dims_t dims_com_window() {
	int ymax, xmax;
	getmaxyx(stdscr, ymax, xmax);
	win_dims_t dims;
	dims.ysize = 1;
	dims.xsize = xmax-1;
	dims.ypos = (ymax/DAYS_IN_WEEK)*DAYS_IN_WEEK;
	dims.xpos = 0;
	return dims;
}

/*
	Creates the command window, given a location to store it
*/
void create_com_window(WINDOW ** comwin) {
	win_dims_t dims = dims_com_window();
	*comwin = newwin(dims.ysize, dims.xsize, dims.ypos, dims.xpos);
	refresh();
}

/*
	Recalculates and adjusts the size and position of the week, main, text and com windows
*/
void resize_windows(WINDOW ** days, WINDOW ** mainwin, WINDOW ** textwin, WINDOW ** comwin, time_t selected) {
	win_dims_t dims;
	for (int i = 0; i < DAYS_IN_WEEK; i++){
		dims = dims_day_window(i);
		wresize(days[i], dims.ysize, dims.xsize);
		mvwin(days[i], dims.ypos, dims.xpos);
	}

	dims = dims_main_window(); // Resize main window
	wresize(*mainwin, dims.ysize, dims.xsize);
	mvwin(*mainwin, dims.ypos, dims.xpos);

	dims = dims_text_window(); // Resize text box inside main window
	wresize(*textwin, dims.ysize, dims.xsize);
	mvwin(*textwin, dims.ypos, dims.xpos);

	dims = dims_com_window(); // Resize text box inside main window
	wresize(*comwin, dims.ysize, dims.xsize);
	mvwin(*comwin, dims.ypos, dims.xpos);

	refresh();
}



/*
	Reads a given day's entry data from file and puts it into the given buffer (given the buffer's size).
	Returns true on success and false on failure.
*/
int read_day(char *buffer, size_t buf_size, time_t selected){
	FILE *fp;
	char filename[PATH_MAX];
	date_filename(filename, selected);
	if ((fp = fopen(filename, "r"))) {
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
void draw_date_entry(WINDOW ** textwin, time_t selected, char *text_buffer) {
	wclear(*textwin);
	memset(text_buffer, '\0', MAIN_TEXT_SIZE);
	if (read_day(text_buffer, MAIN_TEXT_SIZE, selected)){
		mvwprintw(*textwin, 0, 0, text_buffer);
	} else {
		mvwprintw(*textwin, 0, 0, "Empty.");
	}
	wrefresh(*textwin);
}

/*
	Updates the entire calendar part of the UI, to be used when loading a new date.
	Requires a pointer to a location for an array of day windows, pointer to main window, 
		the selected date and a buffer for the entry text
*/
void update_ui(WINDOW ** days, WINDOW ** mainwin, WINDOW ** textwin, time_t selected, char *text_buffer) {
	draw_main_window(mainwin, textwin, selected);
	draw_week_windows(days, selected);
	draw_date_entry(textwin, selected, text_buffer);
}


/*
	Open a program (via fork & exec) in order to edit a given day's entry, 
	flush the text buffer, then redraw appropriate UI elements.
*/
void edit_date(WINDOW ** textwin, time_t selected, char *text_buffer) {
	char filename[PATH_MAX];
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
		draw_date_entry(textwin, selected, text_buffer);
	}
}

/*
	Returns if a given character (expressed as an int) is a digit from 0 to 9
*/
int is_digit(int c) {
	return c >= '0' && c <= '9';
}

/*
	Given a vi command, its length and the selected time, return a new time which is the result of the command
*/
time_t parse_vi_command(int *command, size_t size, time_t selected) {
	int num_times = 0;
	int i;
	int digit_value;
	int letter_command = command[size-1];
	if (size > 1) {
		for (i = 0; i<size-1; i++) {
			if (is_digit(command[i])) {
				digit_value = command[i]-ASCII_DIGIT_START;
				num_times += digit_value * pow(10, size-i-2);
			}
		}
	} else {
		num_times = 1;
	}
	for (i = 0; i<num_times; i++) { // Breakdown of keys to representation
		switch (letter_command) {
			case KEY_DOWN:
			case 'd': selected = add_days(selected, 1); break;
			case KEY_UP:
			case 'D': selected = add_days(selected, -1); break;
			case KEY_RIGHT:
			case 'w': selected = add_days(selected, DAYS_IN_WEEK); break;
			case KEY_LEFT:
			case 'W': selected = add_days(selected, -DAYS_IN_WEEK); break;
			case 'f': selected = add_days(selected, DAYS_IN_WEEK*2); break;
			case 'F': selected = add_days(selected, -DAYS_IN_WEEK*2); break;
			case 'm': selected = add_days(selected, 30); break; // TODO: Dynamic month and year number of days 
			case 'M': selected = add_days(selected, -30); break;
			case 'y': selected = add_days(selected, 365); break;
			case 'Y': selected = add_days(selected, -365); break;
		}
	} 
	return selected;

}

/*
	Prompts the user to enter a vi command and puts the result in the command_buffer (array of ints)
	Requires the first character as a separate argument (to be appended to the array)
	Shows text output similar to vi, vim, less etc... to comwin window.
	Returns the length of the command (i.e. the number of ints used in the buffer)

*/
int input_vi_command(WINDOW *comwin, int* command_buffer, int first_char) {
	size_t command_size = 1;
	wmove(comwin, 0, 0);
	wclear(comwin);
	wrefresh(comwin);
	command_buffer[0] = first_char;
	while (is_digit(command_buffer[command_size-1])) { // Keep adding characters until a non-digit is found
		wprintw(comwin, "%c", command_buffer[command_size-1]);
		wrefresh(comwin);
		command_buffer[command_size++] = getch();
	}
	wclear(comwin);
	wrefresh(comwin);
	return command_size;
}

/*
	Takes a string of most size n and makes it lower case
*/
void strlower(char* string, size_t n) {
	for (int i = 0; i < n; i++){
		string[i] = tolower(string[i]);
	}
}


/*
	Sends a notify-send notification with given text provided an urgency level.
	0 = low, 1 = normal, 2 = critical
*/
void notify(int level, char* text) {
	const char* urgency_strings[] = {"low", "normal", "critical"};
	int pid = fork();
	if (pid == 0) {
		execlp("notify-send", "notify-send", "-u", urgency_strings[level], text, NULL);
	} else if (pid == -1) {
		printw("An error occurred opening the process.");
		refresh();
	} else {
		int status;
		waitpid(pid, &status, 0);
	}
}

/*
	Check and alert any notifications on a given day. This alert is scanned from the day's text in a special kind of alert comment.
	Activates the alert via notify-send only at the moment
*/
void check_notifs(time_t day) {
	char text_buffer[MAIN_TEXT_SIZE];
	if (read_day(text_buffer, MAIN_TEXT_SIZE, day)) {
		int line_matches = 0;
		char notify_string[NOTIFY_TEXT_SIZE];
		char level_string[NOTIFY_URGENCY_SIZE];

		// Match: "<!-- (urgency-level) alert-text -->" and provide both strings
		line_matches = sscanf(text_buffer, "%*[^<]<!-- (%[^)]) %[^-] -->%*[\n]", level_string, notify_string);

		// Compare the level provided in the comment and convert it to a number
		strlower(level_string, 20);
		if (line_matches == 2) {
			int level = 60;
			printf("low %d", strcmp(level_string, "low"));
			if (!strcmp(level_string, "low")) level = 0;
			else if (!strcmp(level_string, "normal")) level = 1;
			else if (!strcmp(level_string, "high")) level = 2;
			else if (!strcmp(level_string, "critical")) level = 2;
			notify(level, notify_string);
		}
		
	}
	
}


/*
	The main UI loop which renders the ncurses UI and handles character input
*/
void ui_loop() {
	time_t selected, today_raw;
	char text_buffer[MAIN_TEXT_SIZE];
	int command_buffer[COMMAND_SIZE];
	size_t command_size;
	WINDOW * days[DAYS_IN_WEEK];
	WINDOW * mainwin;
	WINDOW * textwin;
	WINDOW * comwin;

	// Get the time
	time(&today_raw);
	selected = today_raw;	

	// Initialise UI
	create_week_window(days, selected);
	create_main_window(&mainwin, &textwin, selected);
	draw_date_entry(&textwin, selected, text_buffer);
	create_com_window(&comwin);
	check_notifs(today_raw);
	
	while (true) { // Main loop
		int c = getch();
		switch(c) {
			case 'q': return; // Quit the program on 'q'
			case KEY_RESIZE: // Triggered when the window resizes
				clear();
				printf("8");
				resize_windows(days, &mainwin, &textwin, &comwin, selected);
				break;
			case 'e': edit_date(&textwin, selected, text_buffer); break; // Edit entry
			case ' ': time(&selected); break; // Return to the current day
		default:
			// Input and execute vi-like commands if pattern not matched
			memset(command_buffer, '\0', COMMAND_SIZE*sizeof(int)); // Clear cmd buffer
			command_size = input_vi_command(comwin, command_buffer, c);
			selected = parse_vi_command(command_buffer, command_size, selected);
			break;
		}
		update_ui(days, &mainwin, &textwin, selected, text_buffer);

	}
}



/* The main body function */
int main() {
	setlocale(LC_ALL, "");
	initscr();
	noecho();
	curs_set(0);
	keypad(stdscr, true);

	ui_loop();

	endwin();
	return 0;
}
