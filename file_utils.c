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

#define MAXFNAME 255

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
        /*fd = creat(path, S_IRWXU);  Creation avec droit à tlm*/
        fd = open(path, O_RDWR|O_CREAT|O_TRUNC, S_IRWXU);

        /*printf("fd : %d",fd);*/
        
        if (fd >= 0 ){
            /*printf("file initialise\n");*/
            constructeurFile(fd); /*Cree une map 30*15 pleine de 0*/
            /*fprintf(stderr, "le fichier %s n'existe pas\n", path);
            exit(EXIT_FAILURE);*/
        }
        else{
            perror("Erreur lors de l'ouverture du fichier");
            
            exit(EXIT_FAILURE);
        }
    }
    else {
    perror("Erreur lors de l'ouverture du fichier");
    exit(EXIT_FAILURE);
    }
    return fd;
}

int openFileSim(char* simFile, unsigned char *buff,  unsigned char *x,  unsigned char *y,  unsigned char* nbf, unsigned char* titre)
{
    /*int nbF;*/
    int fd;
    /*unsigned char buffer[(LARGEUR2 - 2)*(HAUTEUR2 - 2)];*/
    char* path = malloc(sizeof(char) * strlen(simFile) + 1);
    if(path == NULL){
        perror("erreur lors du malloc createSim");
        exit(EXIT_FAILURE);
    }

    strcat(path, getFileBase(simFile));
    strcat(path, ".bin");

    fd = open(simFile, O_RDWR);
    /*printf("fd : %d", fd);*/
    
    if (fd >= 0){
        /*On lit le decor dans le buffer*/
        readMap(fd, buff, x, y, nbf, titre);
        
        /*On copie le buffer dans la simulation*/
        /*writeMap(buffer, fd, nbF);*/
    }
    else if (errno == ENOENT){ /*the named file does not exist*/
        /*fd = creat(simFile, S_IRWXU);  Creation avec droit à tlm*/
        fd = open(simFile, O_RDWR|O_CREAT|O_TRUNC, S_IRWXU);
        /*printf("fd : %d",fd);*/
        
        if (fd >= 0 ){
            
            fd = createSim(simFile, fd, buff, x, y, nbf, titre); 
        }
        else{
            perror("Erreur lors de l'ouverture du fichier\n");
            
            exit(EXIT_FAILURE);
        }
    }
    else {
    perror("Erreur lors de l'ouverture du fichier");
    exit(EXIT_FAILURE);
    }
    return fd;
}

/**
 * @brief ecrit un buffer dans un fichier
 * 
 * @param buffer message à ecrire
 * @param fd file descriptor du fichier à remplire
 */
void writeMap(unsigned char* buffer, int fd, unsigned char* x, unsigned char* y, unsigned char* nbf, unsigned char* tritre )
{
    int i;
    
    for(i = 0; i < (LARGEUR2 - 2)*(HAUTEUR2 - 2); i++){
        if(write(fd, buffer+i, sizeof(unsigned char)) == -1 ){
        perror("Erreur Ecriture");
        exit(EXIT_FAILURE);
    }
    }

    writeFallPosition(fd, *x, *y);
    writeNbF(fd, *nbf);

    for(i = 0; i < strlen((char*)tritre); ++i){
        writeTitle(fd, tritre[i]);
    }
    
    
}
    
/**
 * Construit un decor vide
 */
void constructeurFile(int fd)
{
    int i;
    unsigned char caseVide = 0;


    for(i = 0; i < (LARGEUR2 - 2)*(HAUTEUR2 - 2); i++){
        if(write(fd, &caseVide, sizeof(unsigned char)) == -1 ){
        perror("Erreur Ecriture");
        exit(EXIT_FAILURE);
    }
    }

    writeFallPosition(fd, 255, 255);
    writeNbF(fd, 0);
    /*writeTitle(fd, '\0');*/
} 

/**
 * @brief copie un fichier dans un buffer
 * 
 * @param fileName le chemin du fichier a copier
 * @param buff le buffer qui doit recevoir le contenu du fichier
 * @return int 
 */
void readMap(int fd, unsigned char* buff, unsigned char* x, unsigned char* y, unsigned char* nbF, unsigned char* titre)
{
    int taille = 0;
    int state;
    unsigned char lecture;
    
    if (lseek(fd,0, SEEK_SET) == -1 ){
        perror("Erreur Lors du LSEEK readmap");
        exit(EXIT_FAILURE);
    }

    do{
        state = read(fd, &lecture, sizeof(unsigned char));
        if(state == -1){
            perror("Erreur lors de la lecture de la map, case par defaut");
            buff[taille++] = 0;
        }else{
            buff[taille++] = lecture;
        }
        
    }while(taille <  (LARGEUR2 - 2)*(HAUTEUR2 - 2) );


    if ( lseek(fd, (LARGEUR2 - 2)*(HAUTEUR2 - 2), SEEK_SET) == -1 ){
        perror("Erreur Lors du LSEEK readMap");
        exit(EXIT_FAILURE);
    }
    

    if( (state = read(fd, x, sizeof(unsigned char))) != sizeof(unsigned char)){
        perror("Erreur lors de la lecture de x, valeur par defaut utilise");
        *x = 255;
    }
    if( (state = read(fd, y, sizeof(unsigned char))) != sizeof(unsigned char)){
        perror("Erreur lors de la lecture de x, valeur par defaut utilise");
        *y =255;
    }
    if( (state = read(fd, nbF, sizeof(unsigned char))) != sizeof(unsigned char)){
        perror("Erreur lors de la lecture de x, valeur par defaut utilise");
        *nbF = 0;
    }

    taille = 0;
    do{
        state = read(fd, &lecture, sizeof(unsigned char));
        if(state == -1){
            perror("Erreur lors de la lecture du titre, case par defaut");
            titre[taille++] = '\0';
        }else{
            titre[taille++] = lecture;
        }
        
    }while( state == sizeof(unsigned char) && taille < MAXFNAME );
}

/**
 * @brief créé une simulation à partir d'un decor
 * 
 * @param decor 
 */
int createSim(char* simFile, int sim_d, unsigned char* buff, unsigned char* x, unsigned char* y, unsigned char* nbF, unsigned char* titre){
    
    /*int nbF = 0;*/
    int decor_d;
	/*unsigned char buffer[(LARGEUR2 - 2)*(HAUTEUR2 - 2)];*/
    char* path = malloc(sizeof(char) * strlen(simFile) + 1);
    if(path == NULL){
        perror("erreur lors du malloc createSim");
        exit(EXIT_FAILURE);
    }

    strcat(path, getFileBase(simFile));
    strcat(path, ".bin");

    /*On créé un fichier de simulation et on ouvre le decor*/
    /*simFile = decor.sim*/
    decor_d = openFile(path);

    /*On lit le decor dans le buffer*/
    readMap(decor_d, buff, x, y, nbF, titre);
    
    /*On copie le buffer dans la simulation*/
    writeMap(buff, sim_d, x, y, nbF, titre);

    return sim_d;

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
    if(base == NULL){
        perror("erreur lors du malloc createSim");
        exit(EXIT_FAILURE);
    }
    

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
    if ( lseek(fd, (LARGEUR2 - 2)*x+y, SEEK_SET) == -1 ){
        perror("Erreur Lors du LSEEK insertElement");
        exit(EXIT_FAILURE);
    }
   
    if(write(fd, &element, sizeof(unsigned char)) == -1 ){
        perror("Erreur Ecriture");
        exit(EXIT_FAILURE);
    }
}

/*file size avec lseek seek_end*/


void writeFallPosition(int fd, unsigned char x, unsigned char y){
    if ( lseek(fd, (LARGEUR2 - 2)*(HAUTEUR2-2), SEEK_SET) == -1 ){
        perror("Erreur Lors du LSEEK writeFallPosition");
        exit(EXIT_FAILURE);
    }
   
    
    if( write(fd, &x, sizeof(unsigned char)) == -1 ){
        perror("Erreur Ecriture");
        exit(EXIT_FAILURE);
    }

    /*lseek(fd, 1, SEEK_CUR);*/
    if( write(fd, &y, sizeof(unsigned char)) == -1 ){
        perror("Erreur Ecriture");
        exit(EXIT_FAILURE);
    }

}
void writeNbF(int fd, unsigned char nb){
    if ( lseek(fd, (LARGEUR2 - 2)*(HAUTEUR2-2) + 2*sizeof(unsigned char), SEEK_SET) == -1 ){
        perror("Erreur Lors du LSEEK writeFallPosition");
        exit(EXIT_FAILURE);
    }
    
    if(write(fd, &nb, sizeof(unsigned char)) == -1 ){
        perror("Erreur Ecriture");
        exit(EXIT_FAILURE);
    }


}
void writeTitle(int fd, unsigned char c){
    
    if ( lseek(fd, 0, SEEK_END) == -1 ){
        perror("Erreur Lors du LSEEK writeFallPosition");
        exit(EXIT_FAILURE);
    }
    
    if(write(fd, &c, sizeof(unsigned char)) == -1 ){
        perror("Erreur Ecriture");
        exit(EXIT_FAILURE);
    }

    
    
}
