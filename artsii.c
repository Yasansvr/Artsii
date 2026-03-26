#include<ncurses.h>
#include<stdlib.h>
#include<string.h>

typedef struct window
{
    int height;
    int width;
    int start_x;
    int start_y;
}winCor;

typedef struct rectangle
{
    int memx;
    int memy;
    int width;
    int height;
} Rectangle;

typedef struct text
{
    int memx;
    int memy;
    char text[100];
}Text;

typedef struct line
{
    int memx;
    int memy;
    int dir;
    int length;
}line;

int main()
{
	initscr();//initialize cursor mode
	raw();//disable inline buffering
	noecho();//echo swtiched off
    cbreak();
    curs_set(1);
    refresh();
	keypad(stdscr,TRUE);//function keys reading enabled
    bool exit = false; 
    bool done = true;
    int cursInitX = 1;
    int cursInitY = 1;
    int memx;
    int memy;
    int height = 0;
    int width = 0;
    char help[] = "'D' to draw, 'W' to write text, 'L' to draw line |'C' to clear last rectangle, 'Z' to delete last text, 'X' to delete last line | 'Q' to quit";
    char title[] = "ARTSII";
    char text[100];
    
    Rectangle rects[100];
    int rect_count = 0;

    Text texts[100];
    int text_count = 0;

    line lines[100];
    int line_count = 0;

    int sec = 0;

	while (!exit)
    {
        int row , col;
        getmaxyx(stdscr,row,col );
        winCor win1 = {row , col , 0 , 0 };
        WINDOW *win = newwin(win1.height , win1.width , win1.start_y , win1.start_x);
        mvwprintw(win, row-2, (col-strlen(help))/2, "%s", help);
        mvwprintw(win, 1, (col-strlen(title))/2, "%s", title);
        
        box(win, 0, 0);
        wmove(win, cursInitY, cursInitX);
        wrefresh(win);
        keypad(win, TRUE);

        wtimeout(win, -1); // Always block for input

        //DRAW RECTANGLES
        for (int i = 0; i < rect_count; i++)
        {
            int rx = rects[i].memx;
            int ry = rects[i].memy;
            int rw = rects[i].width;
            int rh = rects[i].height;
            if (rw > 0 && rh > 0)
            {
                mvwhline(win, ry, rx, 0, rw);
                mvwhline(win, ry + rh - 1, rx, 0, rw);
                mvwvline(win, ry, rx, 0, rh);
                mvwvline(win, ry, rx + rw - 1, 0, rh);
                mvwaddch(win, ry, rx, ACS_ULCORNER);
                mvwaddch(win, ry, rx + rw - 1, ACS_URCORNER);
                mvwaddch(win, ry + rh - 1, rx, ACS_LLCORNER);
                mvwaddch(win, ry + rh - 1, rx + rw - 1, ACS_LRCORNER);
            }
        }
        wmove(win, cursInitY, cursInitX);

        //DRAW TEXTS
        for (int i = 0; i < text_count; i++)
        {
            mvwprintw(win, texts[i].memy, texts[i].memx, "%s", texts[i].text);
        }
        wmove(win, cursInitY, cursInitX);

        
        //DRAW LINES
        for (int i = 0; i < line_count; i++)
        {
            sec = i;
            if (lines[i].dir == KEY_DOWN)
            {
                mvwvline(win, lines[i].memy, lines[i].memx, 0, lines[i].length);
            }
            else if (lines[i].dir == KEY_UP)
            {
                mvwvline(win, lines[i].memy - lines[i].length + 1, lines[i].memx, 0, lines[i].length);
            }
            else if (lines[i].dir == KEY_RIGHT)
            {
                mvwhline(win, lines[i].memy, lines[i].memx, 0, lines[i].length);
            }
            else if (lines[i].dir == KEY_LEFT)
            {
                mvwhline(win, lines[i].memy, lines[i].memx - lines[i].length + 1, 0, lines[i].length);
            }
            wmove(win, cursInitY, cursInitX);
        }
        



        //BUTTON TRIGGER
        int ch = wgetch(win);
        if (ch == 'q' || ch == 'Q')
        {
            exit=true;
        }

        if (!done) {
            if (ch == 10) {
                sec++;
                done = true;
            } else if (line_count > 0 && ch == lines[sec].dir) {
                // Grow the line when arrow keys are pressed
                lines[sec].length++;
            }
        }

        switch (ch)
        {
            case KEY_RIGHT:
                if (cursInitX < win1.width - 2)
                {
                    cursInitX++;
                }
                break;
                
            case KEY_LEFT:
                if (cursInitX > 1)
                {
                    cursInitX--;
                }
                break;
                
            case KEY_UP:
                if (cursInitY > 1)
                {
                    cursInitY--;
                }
                break;
                
            case KEY_DOWN:
                if (cursInitY < win1.height - 2)
                {
                    cursInitY++;
                }
                break;

            case 'd':
            case 'D':
            {
                mvwprintw(win, 1, 1, " ~ DRAW MODE ~");
                curs_set(0);
                memx = cursInitX;
                memy = cursInitY;
                char widthStr[50];
                char heightStr[50];
                mvwprintw(win, cursInitY, cursInitX, "WIDTH:");
                curs_set(1);
                wrefresh(win);
                echo();
                mvwgetnstr(win, cursInitY, cursInitX + 10, widthStr, 49);
                width = atoi(widthStr);
                width = width * 2;
                noecho();

                for (size_t i = 0; i < 10 + strlen(widthStr); i++) {
                    mvwaddch(win, cursInitY, cursInitX + i, ' ');
                }

                mvwprintw(win, cursInitY, cursInitX, "HEIGHT:");
                wrefresh(win);
                echo();
                mvwgetnstr(win, cursInitY, cursInitX + 10, heightStr, 49);
                height = atoi(heightStr);
                noecho();
                
                if (width > 0 && height > 0 && rect_count < 100) {
                    rects[rect_count].memx = memx;
                    rects[rect_count].memy = memy;
                    rects[rect_count].width = width;
                    rects[rect_count].height = height;
                    rect_count++;
                }
                
                break;
            }
            
            case 'c':
            case 'C':
                rect_count--;
                break;
            case 'z':
            case 'Z':
                text_count--;
                break;
            case 'w':
            case 'W':
            {
                mvwprintw(win, 1, 1, " ~ WRITE MODE ~");
                curs_set(0);
                echo();
                curs_set(1);
                mvwgetnstr(win, cursInitY, cursInitX, text, 99);

                noecho();
                if (strlen(text) > 0 && text_count < 100)
                {
                    texts[text_count].memx = cursInitX;
                    texts[text_count].memy = cursInitY;
                    strcpy(texts[text_count].text, text);
                    text_count++;
                }
                break;
            }
            case 'l':
            case 'L':
                if (line_count < 100)
                {
                    mvwprintw(win, 1, 1, " ~ LINE MODE ~");
                    curs_set(0);
                    wrefresh(win);
                    wtimeout(win, -1); // Block and wait for direction
                    int ch2 = wgetch(win);
                    curs_set(1);
                    if (ch2 == KEY_UP || ch2 == KEY_DOWN || ch2 == KEY_LEFT || ch2 == KEY_RIGHT)
                    {
                        lines[line_count].memx = cursInitX;
                        lines[line_count].memy = cursInitY;
                        lines[line_count].dir = ch2;
                        lines[line_count].length = 1;
                        line_count++;
                        done = false; // Start growing!
                    }
                }
                break;
            case 'x':
            case 'X':
                line_count--;
                break;
                
                
        }
        
        delwin(win);
        
    }
    
	
    
	endwin();//end cursor mode
	return 0;
}