#ifndef _FILE_UTILS_
#define _FILE_UTILS_

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

/**
 * @brief créé une simulation à partir d'un decor
 * 
 * @param decor 
 */
void createSim(char* decor);

/**
 * @brief ecrit un buffer dans un fichier
 * 
 * @param buffer message à ecrire
 * @param fd file descriptor du fichier à remplire
 */
void writeMap(unsigned char* buffer, int fd);

/**
 * @brief ouvre un fichier
 * 
 * @param path chemin du fichier
 * @return int file descriptor du fichier ouvert
 */
int openFile(char* path);

/**
 * @brief copie un fichier dans un buffer
 * 
 * @param fileName le chemin du fichier a copier
 * @param buff le buffer qui doit recevoir le contenu du fichier
 * @return int 
 */
void readMap(int fd, unsigned char** buff);

/**
 * @brief get l'extention du fichier
 * 
 * @param path chemin du fichier
 * @return char* l'extention du fichier
 */
char* getFileExt(const char* path);

/**
 * @brief get le chemin sans l'extention
 * 
 * @param path le chemin du fichier
 * @return char* le chemin sans l'extention
 */
char* getFileBase (const char* path);

/**
 * @brief insert une valeur à une position spécifié
 * 
 * @param x 
 * @param y 
 * @param fd file descriptor du fichier a modifier
 */
void insertElement(int fd, int x, int y, unsigned char element);


void constructeurFile(int fd);
#endif