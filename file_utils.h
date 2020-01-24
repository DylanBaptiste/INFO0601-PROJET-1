#ifndef _FILE_UTILS_
#define _FILE_UTILS_

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

int  createSim(char* decor, int sim_d, unsigned char* buff, unsigned char* x, unsigned char* y, unsigned char* nbF, unsigned char* titre);
void writeMap(unsigned char* buffer, int fd, unsigned char* x, unsigned char* y, unsigned char* nbf, unsigned char* tritre );
int openFile(char* path);
int openFileSim(char* simFile, unsigned char *buff,  unsigned char *x,  unsigned char *y,  unsigned char* nbf, unsigned char* titre);
void readMap(int fd, unsigned char* buff, unsigned char* x, unsigned char* y, unsigned char* nbF, unsigned char* titre);
char* getFileExt(const char* path);
char* getFileBase (const char* path);
void insertElement(int fd, int x, int y, unsigned char element);
void constructeurFile(int fd);
void writeFallPosition(int fd, unsigned char x, unsigned char y);
void writeNbF(int fd, unsigned char nb);
void writeTitle(int fd, unsigned char c);
#endif