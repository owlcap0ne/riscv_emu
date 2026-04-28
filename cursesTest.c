#include <stdlib.h>
#include <ncurses.h>
#include <signal.h>

static void finish(int sig);

int main(int argc, char *argv[])
{
    int num = 0;

    (void) signal(SIGINT, finish);

    (void) initscr();
    keypad(stdscr, TRUE);
    (void) nonl();
    (void) cbreak();
    (void) echo();

    if (has_colors())
    {
        start_color();

        /*
         * Simple color assignment, often all we need.  Color pair 0 cannot
         * be redefined.  This example uses the same value for the color
         * pair as for the foreground color, though of course that is not
         * necessary:
         */
        init_pair(1, COLOR_RED,     COLOR_BLACK);
        init_pair(2, COLOR_GREEN,   COLOR_BLACK);
        init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
        init_pair(4, COLOR_BLUE,    COLOR_BLACK);
        init_pair(5, COLOR_CYAN,    COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_WHITE,   COLOR_BLACK);
    }

    for(;;)
    {
        int c = getch();
        attrset(COLOR_PAIR(num % 8));
        num++;
    }

    finish(0);
}

static void finish(int sig)
{
    endwin();

    exit(0);
}
