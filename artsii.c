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
    int cursInitX = 1;
    int cursInitY = 1;
    int memx;
    int memy;
    int height = 0;
    int width = 0;
    char help[] = "'D' to draw, 'W' to write text, 'C' to clear last rectangle, 'Z' to delete last text, 'Q' to quit";
    char title[] = "ARTSII";
    char text[100];
    
    Rectangle rects[100];
    Text texts[100];
    int rect_count = 0;
    int text_count = 0;
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



        //BUTTON TRIGGER
        int ch = wgetch(win);
        if (ch == 'q' || ch == 'Q')
        {
            exit=true;
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
                memx = cursInitX;
                memy = cursInitY;
                char widthStr[50];
                char heightStr[50];
                mvwprintw(win, cursInitY, cursInitX, "ENTER WIDTH:");
                wrefresh(win);
                echo();
                mvwgetnstr(win, cursInitY, cursInitX + 15, widthStr, 49);
                
                width = atoi(widthStr);
                width = width * 2;
                noecho();
                mvwprintw(win, cursInitY, cursInitX, "ENTER HEIGHT:");
                wrefresh(win);
                echo();
                mvwgetnstr(win, cursInitY, cursInitX + 15, heightStr, 49);
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
                echo();
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
        }
        
        delwin(win);
        
    }
    
	
    
	endwin();//end cursor mode
	return 0;
}