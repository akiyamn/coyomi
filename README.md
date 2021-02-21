# coyomi
Terminal dairy/calendar organiser written in C, with simplicity in mind.
Uses the ncurses library for TUI in the terminal and runs only on Unix based operating systems for now (i.e. GNU/Linux, MacOS etc...)

![a screenshot of coyomi](https://hirasawa.moe/media/coyomi.png)

## Requirements:
- `gcc` compiler
- `ncurses` library


## Compiling

Simply run `make` in the root directory and run the executable named `curses`.

```bash
git clone https://github.com/akiyamn/coyomi.git
cd coyomi
make
./curses
```

## Usage
Coyomi is very basic at the moment and has limited functionality. However it is still usable.

### Controls

- `1-7` Select between the seven days of the week (1 = Monday etc...)
- `e` Edit the entry of a given day
- `q` Quit the program
