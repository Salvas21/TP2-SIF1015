//
// Created by Piérik Landry on 2021-11-24.
//

#ifndef TP2_SIF1015_CLIENT_H
#define TP2_SIF1015_CLIENT_H

#include <ncurses.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>


char *appendChar(char *szString, size_t strsize, char c);
void initScreen();
void initColors();
WINDOW *createWindow(int height, int width, int position_y, int position_x, const char * text_window);
void clearWindow(WINDOW *window, const char * text_window);
void *serverWindowThread(WINDOW * window);

#endif //TP2_SIF1015_CLIENT_H
