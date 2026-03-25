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
    int width = 0;
    int height = 0;
	while (!exit)
    {
        int row , col;
        getmaxyx(stdscr,row,col );
        winCor win1 = {row , col , 0 , 0 };
        WINDOW *win = newwin(win1.height , win1.width , win1.start_y , win1.start_x);
        
        box(win, 0, 0);
        wmove(win, cursInitY, cursInitX);
        wrefresh(win);

        keypad(win, TRUE);

        



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
                char widthStr[50];
                char heightStr[50];
                mvwprintw(win, cursInitY, cursInitX, "ENTER WIDTH:");
                wrefresh(win);
                echo();
                mvwgetnstr(win, cursInitY, cursInitX + 15, widthStr, 49);
                width = atoi(widthStr);
                noecho();
                mvwprintw(win, cursInitY, cursInitX, "ENTER HEIGHT:");
                wrefresh(win);
                echo();
                mvwgetnstr(win, cursInitY, cursInitX + 15, heightStr, 49);
                height = atoi(heightStr);
                noecho();
                
                break;
            }
        }
        
        delwin(win);
        
    }
    
	
    
	endwin();//end cursor mode
	return 0;
}