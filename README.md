# coyomi
Terminal dairy/calendar organiser written in C, with simplicity in mind.
Uses the ncurses library for TUI in the terminal and runs only on Unix based operating systems for now (i.e. GNU/Linux, MacOS etc...)

![a screenshot of coyomi](https://hirasawa.moe/media/coyomi.png)

Build Requirements:
No requirements are needed (other than an applicable system) in order to run just the binary executable.

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
coyomi is very basic at the moment and has limited functionality, however it is still usable.

### Controls
Controls are vi-like, but are simple enough to be used by someone not familiar with them.
For the purposes of this documentation moving "forward one day" means going into the future and moving "back one day" means going into the past.

#### General
|Key|Description|
|-|-|
|`e`|Edit the entry of the selected day|
|`q`|Quit the program|

#### Simple movement
|Key|Description|Analogous to|
|-|-|-|
|`↑`|Move forward one day|`1d`|
|`↓`|Move back one day|`1D`|
|`→`|Move forward one week|`1w` or `7d`|
|`←`|Move back one week|`1W` or `7D`|

#### Vi-like movement
A vi-like command in coyomi consists of a number and a descriptor. (Usually a number and a letter)
_E.g.:_ `12w` would correspond to going forward `12` weeks (`w`)
If no number is given, `1` is implied.

The arrow keys mentioned above are treated as aliases to the letter descriptors they represent below.

|Key|Description|Number of days|
|-|-|-|
|`d`|Forward n days|+1|
|`D`|Backward n days|-1|
|`w`|Forward n weeks|+7|
|`W`|Backward n weeks|-7|
|`f`|Forward n fortnights|+14|
|`F`|Backward n fortnights|-14|
|`m`|Forward n months|+30\*|
|`M`|Forward n months|+30\*|
|`y`|Forward n years|+365\*|
|`Y`|Forward n years|+365\*|

>\*Note that in future, months will change between 28, 29, 30, 31 depending on the month and year selected. The same goes for years, which will eventually take leap years into account.

### Editor

While coyomi does not come with its own editor, it uses external editors such as: vim, nano, gedit etc... to edit date entries. This makes coyomi quite modular, allowing you to chose your favourite editor rather than bake in a subpar one. 
coyomi saves its files in plain Markdown format (`.md`) allowing other programs to use existing date entries (e.g. pandoc, git etc...). At the moment, coyomi does not render Markdown in its UI.

## Alerts

coyomi has an experimental notification/alerts feature.

On the program's startup, just today's page (for now) is read for a special notification comment somewhere within the document.
A notification takes the following form:
```markdown
<!-- (urgency-level) alert-message -->
```

The current urgency levels are as follows:
- low
- normal
- high\*
- critical\*

*\*These act as the same level*

Replace `alert-message` with your own message.

### Example
```markdown
<!-- (critical) Finish Assignment 1 tomorrow! -->
```




<!-- - `1-7` Select between the seven days of the week (1 = Monday etc...) -->

