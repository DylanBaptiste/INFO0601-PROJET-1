#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "file_utils.h"

#define LARGEUR2 60 
#define HAUTEUR2 20 

#define MAXFNAME 2048

/**
 * @brief ouvre un fichier
 * 
 * @param path chemin du fichier
 * @return int file descriptor du fichier ouvert
 */
int openFile(char* path)
{
    int fd = open(path, O_RDWR);
    /*printf("fd : %d", fd);*/
    
    if (fd >= 0){
        /*printf("fichier ouvert\n");*/
    }
    else if (errno == ENOENT){ /*the named file does not exist*/
        fd = creat(path, S_IRWXU); /* Creation avec droit à tlm*/
        /*printf("fd : %d",fd);*/
        
        if (fd >= 0 ){
            /*printf("file initialise\n");*/
            constructeurFile(fd); /*Cree une map 30*15 pleine de 0*/
            /*fprintf(stderr, "le fichier %s n'existe pas\n", path);
            exit(EXIT_FAILURE);*/
        }
        else{
            /*perror("Erreur lors de l'ouverture du fichier\n");*/
            fprintf(stderr, "erreur lors de l'ouverture du fichier %s\n", path);
            exit(EXIT_FAILURE);
        }
    }
    return fd;
}

/**
 * @brief ecrit un buffer dans un fichier
 * 
 * @param buffer message à ecrire
 * @param fd file descriptor du fichier à remplire
 */
void writeMap(unsigned char* buffer, int fd, int nbF)
{
    int i;
    
    for(i = 0; i < (LARGEUR2 - 2)*(HAUTEUR2 - 2); i++){
        if(write(fd, buffer+i, sizeof(unsigned char)) == -1){
            fprintf(stderr, "erreur lors de la copie de la map\n");
            exit(EXIT_FAILURE);
        }    
    }
    /*ecrire le nb de flocon etc...*/
}
    
/**
 * Construit un decor vide
 */
void constructeurFile(int fd)
{
    int i;
    unsigned char caseVide = 0;


    for(i = 0; i < (LARGEUR2 - 2)*(HAUTEUR2 - 2); i++){
        if(write(fd, &caseVide, sizeof(unsigned char)) == -1){
            fprintf(stderr, "erreur lors de la copie de la map\n");
            exit(EXIT_FAILURE);
        }    
    }
} 

/**
 * @brief copie un fichier dans un buffer
 * 
 * @param fileName le chemin du fichier a copier
 * @param buff le buffer qui doit recevoir le contenu du fichier
 * @return int 
 */
void readMap(int fd, unsigned char* buff, int* nbF/* titre et pos(x,y) */)
{
    int taille = 0;
    unsigned char lecture;
    lseek(fd, 0, SEEK_SET);
    if(fd > 0){
        /*lire uniquement la map 0 1 2 dans le buff*/
        while( read(fd, &lecture, sizeof(unsigned char)) != 0)
        {
           buff[taille++] = lecture;
           *nbF += lecture == 1;
        }
        /*ensuite lire les 2 autre info, (+taille pour le tire au debut?)*/
    }
}

/**
 * @brief créé une simulation à partir d'un decor
 * 
 * @param decor 
 */
void createSim(char* decor){
    
    int nbF;
    int decor_d, sim_d;
	unsigned char* buffer = malloc((LARGEUR2 - 2)*(HAUTEUR2 - 2)*sizeof(unsigned char));
    char* path = malloc(sizeof(char) * strlen(decor) + 1);

    strcat(path, getFileBase(decor));
    strcat(path, ".sim");

    /*On créé un fichier de simulation et on ouvre le decor*/
    decor_d = openFile(decor);
    sim_d = creat(path, S_IRWXU);

    /*On lit le decor dans le buffer*/
    readMap(decor_d, buffer, &nbF);
    
    /*On copie le buffer dans la simulation*/
    writeMap(buffer, sim_d, nbF);

    printf("%s\n", buffer);

}

/**
 * @brief get l'extention du fichier
 * 
 * @param path chemin du fichier
 * @return char* l'extention du fichier
 */
char* getFileExt(const char* path){
    char *dot = strrchr(path, '.');
    if(!dot || dot == path) return "";
    return dot + 1;
}

/**
 * @brief get le chemin sans l'extention
 * 
 * @param path le chemin du fichier
 * @return char* le chemin sans l'extention
 */
char* getFileBase (const char* path) {
    
    int i;
    size_t extSize  = strlen(getFileExt(path)) + 1;
    size_t baseSize = strlen(path) - extSize + 1;
    char* base = (char*)malloc(baseSize);
    
    for(i = 0; (i < baseSize - 1) && (i < MAXFNAME); ++i){
        base[i] = path[i];
    }
    base[i++] = '\0';

    return base;
}

/**
 * @brief insert une valeur à une position spécifié
 * 
 * @param x 
 * @param y 
 * @param fd file descriptor du fichier a modifier
 */
void insertElement(int fd, int x, int y, unsigned char element){
    /*20 == largeur de la matrice*/
    lseek(fd, (LARGEUR2 - 2)*x+y, SEEK_SET);
    if(write(fd, &element, sizeof(unsigned char)) == -1){
        fprintf(stderr, "erreur lors de l'update de la simulation");
        exit(EXIT_FAILURE);
    }
}

/*file size avec lseek seek_end*/
