//
// Created by Piérik Landry on 2021-11-24.
//
#include "../include/client.h"

int main()
{
    //init the screen
    initScreen();

    //init color
    initColors();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);

    //init window sizes
    int height, width, start_y, start_x;
    height = 25;
    width = 50;
    start_y = start_x = 1;

    //init client&sever command input
    char command[80];
    char clientInput = 0;
    int commandLine = 0;
    int secret = 0;

    //init thread
    pthread_t tid;

    //creating client window and waiting ENTER to start
    WINDOW * clientWindow = createWindow(height, width, start_y, start_x, "PRESS ENTER TO START");
    while (clientInput != 10)
    {
        clientInput = getch();
    }

    //creating server window thread
    WINDOW * serverWindow = createWindow(height, width, start_y, start_x + width, "Waiting...");
    pthread_create(&tid, NULL, serverWindowThread, serverWindow);

    //resetting command array
    memset(command, 0, sizeof command);

    clearWindow(clientWindow,"");

    //creating input window with custom color
    WINDOW * inputWindow = createWindow(3, width-3, height-3, start_x+1,"Command : ");
    wbkgd(inputWindow, COLOR_PAIR(1));
    wrefresh(inputWindow);

    //move cursor to inputWindow
    move(height-2,start_x+12);

    while (true) {
        clientInput = getch();

        //if input is ENTER, add command to client window
        if (clientInput == 10) {
            commandLine += 1;

            //resetting client window when all lines full
            if (commandLine > height - 5)
            {
                commandLine = 1;
                clearWindow(clientWindow, "");
            }

            //writing command to client window
            mvwprintw(clientWindow, commandLine, 1, "You Entered:");

            //add color to the command word
            if (secret == 1) {
                writeRainbowText(clientWindow,command, commandLine);
            }
            else
            {
                writeCommandOnWindow(clientWindow,command, commandLine);
            }

            wrefresh(clientWindow);

            //end program and thread when input is quit
            if (strcmp(command, "quit") == 0)
            {
                pthread_cancel(tid);
                break;
            }

            if (strcmp(command, "uuddlrlrba") == 0)
            {
                commandLine += 1;
                secret = 1;
                char konami[20] = "KONAMI CODE UNLOCK";
                writeRainbowText(clientWindow,konami, commandLine);
                wrefresh(clientWindow);
            }

            //resetting input window and variable
            memset(command, 0, sizeof command);
            move(height-2,start_x+12);

            clearWindow(inputWindow, "Command : ");
        } else {
            //if input is DELETE, remove last letter
            if (clientInput == 127)
            {
                command[strlen(command)-1] = '\0';
            } else
            {
                appendChar(command, sizeof(command), clientInput);
            }

            clearWindow(inputWindow,"");

            //manually updating inputWindow
            mvwprintw(inputWindow, 1, 1, "Command : %s", command);
            wrefresh(inputWindow);

        }
    }
    //deallocate and end ncurses
    endwin();
    pthread_exit(NULL);
}

void initScreen()
{
    initscr();
    noecho();
    cbreak();
}

void initColors()
{
    if(has_colors() == FALSE)
    {	endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    start_color();
}

void writeCommandOnWindow(WINDOW *window, const char * text_char, int commandLine)
{
    wattron(window,COLOR_PAIR(1));
    mvwprintw(window, commandLine, 14,"%s",text_char);
    wattroff(window,COLOR_PAIR(1));
}

void writeRainbowText(WINDOW *window, const char * text_char, int commandLine)
{
    int color = 1;
    for (int i = 1; i <strlen(text_char)+1; ++i) {
        if (color > 3)
        {
            color = 1;
        }
        wattron(window,COLOR_PAIR(color));
        mvwprintw(window, commandLine, 13+i,"%c",text_char[i-1]);
        wattroff(window,COLOR_PAIR(color));
        color += 1;

    }
}

char *appendChar(char *szString, size_t strsize, char c)
{
    size_t len = strlen(szString);
    if((len+1) < strsize)
    {
        szString[len++] = c;
        szString[len] = '\0';
        return szString;
    }
    return NULL;
}

WINDOW *createWindow(int height, int width, int position_y, int position_x, const char * text_window)
{
    WINDOW * window = newwin(height, width, position_y, position_x);
    refresh();
    box(window,0,0);
    mvwprintw(window,1,1,text_window);
    wrefresh(window);
    return window;
}
void clearWindow(WINDOW *window, const char * text_window)
{
    wclear(window);
    box(window,0,0);
    mvwprintw(window,1,1,text_window);
    wrefresh(window);
}

void *serverWindowThread(WINDOW * window)
{
    wclear(window);
    char serverResponse[100];

    for (int i = 0; i < 10; ++i) {
        sleep(2);
        sprintf(serverResponse,"%d",rand());
        box(window,0,0);
        mvwprintw(window,i+1,1,serverResponse);
        wrefresh(window);
    }

    pthread_exit(NULL);
}

//    char left, right, top, bottom, tlc, trc, blc, brc;
//    left = right = top = bottom = tlc = trc = blc = brc = 42;
//    wborder(serverWindow, left, right, top, bottom, tlc, trc, blc, brc);
//    wrefresh(serverWindow);