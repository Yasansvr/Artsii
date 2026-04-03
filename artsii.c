#include<ncurses.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<dirent.h>

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

typedef struct corner {
    int x;
    int y;
    chtype ch;
} Corner;

void save_diagram(Rectangle* rects, int rect_count, Text* texts, int text_count, line* lines, int line_count, Corner* corners, int corner_count) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char filename[100];
    sprintf(filename, "diagram_%04d-%02d-%02d_%02d-%02d-%02d.txt", 
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
            tm.tm_hour, tm.tm_min, tm.tm_sec);
    
    FILE* fp = fopen(filename, "w");
    if (!fp) return;
    for (int i=0; i<rect_count; i++) fprintf(fp, "R %d %d %d %d\n", rects[i].memx, rects[i].memy, rects[i].width, rects[i].height);
    for (int i=0; i<text_count; i++) fprintf(fp, "T %d %d %s\n", texts[i].memx, texts[i].memy, texts[i].text);
    for (int i=0; i<line_count; i++) fprintf(fp, "L %d %d %d %d\n", lines[i].memx, lines[i].memy, lines[i].dir, lines[i].length);
    for (int i=0; i<corner_count; i++) fprintf(fp, "C %d %d %d\n", corners[i].x, corners[i].y, (int)corners[i].ch);
    fclose(fp);
}

void load_diagram(const char* filename, Rectangle* rects, int* rect_count, Text* texts, int* text_count, line* lines, int* line_count, Corner* corners, int* corner_count) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return;
    char type[20];
    *rect_count = *text_count = *line_count = *corner_count = 0;
    while (fscanf(fp, "%19s", type) == 1) {
        if (strcmp(type, "R") == 0) {
            fscanf(fp, "%d %d %d %d", &rects[*rect_count].memx, &rects[*rect_count].memy, &rects[*rect_count].width, &rects[*rect_count].height);
            (*rect_count)++;
        } else if (strcmp(type, "T") == 0) {
            fscanf(fp, "%d %d", &texts[*text_count].memx, &texts[*text_count].memy);
            int ch;
            while ((ch = fgetc(fp)) == ' '); // skip spaces
            if (ch != EOF) ungetc(ch, fp);
            fgets(texts[*text_count].text, sizeof(texts[0].text), fp);
            texts[*text_count].text[strcspn(texts[*text_count].text, "\n")] = 0; // remove newline
            (*text_count)++;
        } else if (strcmp(type, "L") == 0) {
            fscanf(fp, "%d %d %d %d", &lines[*line_count].memx, &lines[*line_count].memy, &lines[*line_count].dir, &lines[*line_count].length);
            (*line_count)++;
        } else if (strcmp(type, "C") == 0) {
            int ch;
            fscanf(fp, "%d %d %d", &corners[*corner_count].x, &corners[*corner_count].y, &ch);
            corners[*corner_count].ch = (chtype)ch;
            (*corner_count)++;
        }
    }
    fclose(fp);
}

void load_last_diagram(Rectangle* rects, int* rect_count, Text* texts, int* text_count, line* lines, int* line_count, Corner* corners, int* corner_count) {
    DIR *dir;
    struct dirent *ent;
    char last_file[256] = "";
    if ((dir = opendir(".")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strncmp(ent->d_name, "diagram_", 8) == 0 && strstr(ent->d_name, ".txt") != NULL) {
                if (strcmp(ent->d_name, last_file) > 0) {
                    strcpy(last_file, ent->d_name);
                }
            }
        }
        closedir(dir);
    }
    if (strlen(last_file) > 0) {
        load_diagram(last_file, rects, rect_count, texts, text_count, lines, line_count, corners, corner_count);
    }
}

int main(int argc , char *argv[])
{
	initscr();//initialize cursor mode
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLUE);
        init_pair(2, COLOR_BLACK, COLOR_WHITE);
    }
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
    char help[] = "'H' for Help";
    char title[] = "ARTSII";
    char text[100];
    
    Rectangle rects[100];
    int rect_count = 0;

    Text texts[100];
    int text_count = 0;

    line lines[100];
    int line_count = 0;

    Corner corners[100];
    int corner_count = 0;

    bool helpwin = false;
    int ch2;

    WINDOW* helpwin1 = NULL;
    int ch3;

    if (argc > 1) {
        FILE *test_fp = fopen(argv[1], "r");
        if (test_fp) {
            fclose(test_fp);
            load_diagram(argv[1], rects, &rect_count, texts, &text_count, lines, &line_count, corners, &corner_count);
        } else {
            load_last_diagram(rects, &rect_count, texts, &text_count, lines, &line_count, corners, &corner_count);
        }
    } else {
        load_last_diagram(rects, &rect_count, texts, &text_count, lines, &line_count, corners, &corner_count);
    }

	while (!exit)
    {
        int row , col;
        getmaxyx(stdscr,row,col );
        winCor win1 = {row , col , 0 , 0 };
        WINDOW *win = newwin(win1.height , win1.width , win1.start_y , win1.start_x);
        wattron(win, A_REVERSE);
        mvwprintw(win, row-2, (col-strlen(help))/2, "%s", help);
        wattroff(win, A_REVERSE);
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
            switch (lines[i].dir)
            {
            case KEY_DOWN:
                mvwvline(win, lines[i].memy, lines[i].memx, 0, lines[i].length);
                break;
            case KEY_RIGHT:
                mvwhline(win, lines[i].memy, lines[i].memx, 0, lines[i].length);
                break;
            case KEY_UP:
                mvwvline(win, lines[i].memy - lines[i].length + 1, lines[i].memx, 0, lines[i].length);
                break;
            case KEY_LEFT:
                mvwhline(win, lines[i].memy, lines[i].memx - lines[i].length + 1, 0, lines[i].length);
                break;
            }
        }
        for (int i = 0; i < corner_count; i++)
        {
            mvwaddch(win, corners[i].y, corners[i].x, corners[i].ch);
        }
        wmove(win, cursInitY, cursInitX);
        



        //HELP SESSION
        if (helpwin)
        {
            noecho();
            curs_set(0);
            
            int hw_h = win1.height * 2 / 3;
            int hw_w = win1.width * 2 / 3;
            if (hw_h < 16) hw_h = 16;
            if (hw_w < 40) hw_w = 40;
            int hw_y = (row - hw_h) / 2;
            int hw_x = (col - hw_w) / 2;
            
            helpwin1 = newwin(hw_h, hw_w, hw_y, hw_x);
            if (helpwin1) {
                wbkgd(helpwin1, COLOR_PAIR(1));
                box(helpwin1, 0, 0);
                wrefresh(helpwin1);
                
                int hw2_h = hw_h - 4;
                int hw2_w = hw_w - 4;
                int hw2_y = hw_y + 2;
                int hw2_x = hw_x + 2;
                
                WINDOW *helpwin2 = newwin(hw2_h, hw2_w, hw2_y, hw2_x);
                if (helpwin2) {
                    wbkgd(helpwin2, COLOR_PAIR(2));
                    box(helpwin2, 0, 0);
                    
                    wattron(helpwin2, COLOR_PAIR(2));
                    mvwprintw(helpwin2, 1, 2, "'D' : Draw Mode");
                    mvwprintw(helpwin2, 2, 2, "'W' : Write text");
                    mvwprintw(helpwin2, 3, 2, "'L' : Draw line");
                    mvwprintw(helpwin2, 4, 2, "'C' : Clear last rectangle");
                    mvwprintw(helpwin2, 5, 2, "'Z' : Delete last text");
                    mvwprintw(helpwin2, 6, 2, "'X' : Delete last line");
                    mvwprintw(helpwin2, 7, 2, "Ctrl+S : Save diagram");
                    mvwprintw(helpwin2, 8, 2, "Ctrl+O : Open diagram");
                    mvwprintw(helpwin2, 9, 2, "'Q' : Quit");
                    mvwprintw(helpwin2, hw2_h - 2, 2, "Press ENTER or 'H' to close");
                    wattroff(helpwin2, COLOR_PAIR(2));
                    wrefresh(helpwin2);
                }
                
                while (1) {
                    ch3 = helpwin2 ? wgetch(helpwin2) : wgetch(helpwin1);
                    if (ch3 == 10 || ch3 == 'h' || ch3 == 'H' || ch3 == 27) {
                        break;
                    }
                }
                
                if (helpwin2) delwin(helpwin2);
                delwin(helpwin1);
            }
            curs_set(1);
            helpwin = false;
            
            touchwin(win);
            wrefresh(win);
        }

        //BUTTON TRIGGER
        int ch = wgetch(win);
        if (ch == 'q' || ch == 'Q')
        {
            exit=true;
        }

        if (!done) {
            if (ch == 10) {
                done = true;
            } else if (line_count > 0) {
                int curr = line_count - 1;
                if (ch == lines[curr].dir) {
                    // Grow the line when arrow keys are pressed
                    lines[curr].length++;
                } else if (ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT) {
                    // Add corner and change direction
                    int endx = lines[curr].memx;
                    int endy = lines[curr].memy;
                    
                    if (lines[curr].dir == KEY_RIGHT) endx += lines[curr].length - 1;
                    else if (lines[curr].dir == KEY_LEFT) endx -= lines[curr].length - 1;
                    else if (lines[curr].dir == KEY_DOWN) endy += lines[curr].length - 1;
                    else if (lines[curr].dir == KEY_UP) endy -= lines[curr].length - 1;

                    chtype corner = 0;
                    if (lines[curr].dir == KEY_RIGHT && ch == KEY_DOWN) corner = ACS_URCORNER;
                    else if (lines[curr].dir == KEY_RIGHT && ch == KEY_UP) corner = ACS_LRCORNER;
                    else if (lines[curr].dir == KEY_LEFT && ch == KEY_DOWN) corner = ACS_ULCORNER;
                    else if (lines[curr].dir == KEY_LEFT && ch == KEY_UP) corner = ACS_LLCORNER;
                    else if (lines[curr].dir == KEY_DOWN && ch == KEY_RIGHT) corner = ACS_LLCORNER;
                    else if (lines[curr].dir == KEY_DOWN && ch == KEY_LEFT) corner = ACS_LRCORNER;
                    else if (lines[curr].dir == KEY_UP && ch == KEY_RIGHT) corner = ACS_ULCORNER;
                    else if (lines[curr].dir == KEY_UP && ch == KEY_LEFT) corner = ACS_URCORNER;

                    if (corner != 0 && line_count < 100 && corner_count < 100) {
                        corners[corner_count].x = endx;
                        corners[corner_count].y = endy;
                        corners[corner_count].ch = corner;
                        corner_count++;

                        lines[line_count].memx = endx;
                        lines[line_count].memy = endy;
                        lines[line_count].dir = ch;
                        lines[line_count].length = 2;
                        line_count++;
                    }
                }
            }
        }

        switch (ch)
        {
            case 15: // Ctrl+O
            {
                DIR *dir;
                struct dirent *ent;
                char files[200][100];
                int file_count = 0;
                
                if ((dir = opendir(".")) != NULL) {
                    while ((ent = readdir(dir)) != NULL) {
                        if (strncmp(ent->d_name, "diagram_", 8) == 0 && strstr(ent->d_name, ".txt") != NULL) {
                            if (file_count < 200) {
                                strncpy(files[file_count], ent->d_name, 99);
                                files[file_count][99] = '\0';
                                file_count++;
                            }
                        }
                    }
                    closedir(dir);
                }

                if (file_count == 0) {
                    wattron(win, A_REVERSE);
                    mvwprintw(win, row-2, (col-19)/2, " No saved diagrams ");
                    wattroff(win, A_REVERSE);
                    wrefresh(win);
                    napms(1000);
                    break;
                }

                for (int i = 0; i < file_count - 1; i++) {
                    for (int j = i + 1; j < file_count; j++) {
                        if (strcmp(files[i], files[j]) < 0) {
                            char temp[100];
                            strcpy(temp, files[i]);
                            strcpy(files[i], files[j]);
                            strcpy(files[j], temp);
                        }
                    }
                }

                int hw_h = file_count + 4;
                if (hw_h > row * 2 / 3) hw_h = row * 2 / 3;
                if (hw_h < 10) hw_h = 10;
                int hw_w = 50;
                int hw_y = (row - hw_h) / 2;
                int hw_x = (col - hw_w) / 2;

                WINDOW* fpwin = newwin(hw_h, hw_w, hw_y, hw_x);
                keypad(fpwin, TRUE);
                wbkgd(fpwin, COLOR_PAIR(1));
                box(fpwin, 0, 0);
                mvwprintw(fpwin, 0, (hw_w - 16) / 2, " Select Diagram ");
                
                int selected = 0;
                int offset = 0;
                int max_display = hw_h - 2;
                bool picked = false;

                while (1) {
                    for (int i = 0; i < max_display; i++) {
                        int idx = offset + i;
                        if (idx < file_count) {
                            if (idx == selected) wattron(fpwin, A_REVERSE);
                            mvwprintw(fpwin, i + 1, 2, "%-46s", files[idx]);
                            if (idx == selected) wattroff(fpwin, A_REVERSE);
                        } else {
                            mvwprintw(fpwin, i + 1, 2, "%-46s", " ");
                        }
                    }
                    wrefresh(fpwin);
                    
                    int fch = wgetch(fpwin);
                    if (fch == KEY_UP && selected > 0) {
                        selected--;
                        if (selected < offset) offset--;
                    } else if (fch == KEY_DOWN && selected < file_count - 1) {
                        selected++;
                        if (selected >= offset + max_display) offset++;
                    } else if (fch == 10) { // Enter
                        picked = true;
                        break;
                    } else if (fch == 27 || fch == 'q' || fch == 'Q') { // Esc or q
                        break;
                    }
                }

                if (picked) {
                    load_diagram(files[selected], rects, &rect_count, texts, &text_count, lines, &line_count, corners, &corner_count);
                    wattron(win, A_REVERSE);
                    mvwprintw(win, row-2, (col-9)/2, " Loaded! ");
                    wattroff(win, A_REVERSE);
                    wrefresh(win);
                    napms(500);
                }

                delwin(fpwin);
                touchwin(win);
                wrefresh(win);
                break;
            }

            case 19: // Ctrl+S
                save_diagram(rects, rect_count, texts, text_count, lines, line_count, corners, corner_count);
                wattron(win, A_REVERSE);
                mvwprintw(win, 2, (col-strlen(" Saved! "))/2, " Saved! ");
                wattroff(win, A_REVERSE);
                wrefresh(win);
                napms(500);
                break;

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
                    ch2 = wgetch(win);
                    curs_set(1);
                    if (ch2 == KEY_UP || ch2 == KEY_DOWN || ch2 == KEY_LEFT || ch2 == KEY_RIGHT)
                    {
                        lines[line_count].memx = cursInitX;
                        lines[line_count].memy = cursInitY;
                        lines[line_count].dir = ch2;
                        lines[line_count].length = 1;
                        line_count++;
                        done = false; 
                    }
                }
                break;
            case 'x':
            case 'X':
                if (line_count > 0) {
                    if (corner_count > 0 && lines[line_count - 1].memx == corners[corner_count - 1].x && lines[line_count - 1].memy == corners[corner_count - 1].y) {
                        corner_count--;
                    }
                    line_count--;
                }
                break;
            case 'h':
            case 'H':
                helpwin = true;
    
                
                
        }
        
        delwin(win);
        

        
    }
    
	
    
	endwin();//end cursor mode
	return 0;
}