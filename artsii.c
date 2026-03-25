#include<ncurses.h>

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
    curs_set(0);
	keypad(stdscr,TRUE);//function keys reading enabled
    bool exit = false; 

	while (!exit)
    {
        int row , col;
        getmaxyx(stdscr,row,col );
        winCor win1 = {row , col , 0 , 0 };
        WINDOW *win = newwin(win1.height , win1.width , win1.start_y , win1.start_x);
        refresh();
        box(win , 0 , 0);
        wrefresh(win);
        char c = getch();
        if (c == 'q' || c == 'Q')
        {
            exit=true;
        }
        delwin(win);
        
    }
    
	
    
	endwin();//end cursor mode
	return 0;
}