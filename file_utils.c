#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

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
            /*constructeurFile(fd); Cree une map 30*15 pleine de 0*/
            fprintf(stderr, "le fichier %s n'existe pas", path);
            exit(EXIT_FAILURE);
        }
        else{
            /*perror("Erreur lors de l'ouverture du fichier\n");*/
            fprintf(stderr, "erreur lors de l'ouverture du fichier %s", path);
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
void writeMap(char* buffer, int fd)
{
    int i;

    for(i = 0; i < 1044; i++){
        if(write(fd, &buffer, sizeof(char)) == -1){
            fprintf(stderr, "erreur lors de la copie de la map");
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
void readMap(int fd, char* buff)
{
    int taille = 0;
    char lecture[1044];
    
    if(fd > 0){
        while( read(fd, &lecture, sizeof(char)) != 0)
        {
            buff[++taille] = *lecture;
        }
    }
}

/**
 * @brief créé une simulation à partir d'un decor
 * 
 * @param decor 
 */
void createSim(char* decor){
    
    char buffer[1044];
    
    /*On créé un fichier de simulation et on ouvre le decor*/
    int decor_d = openFile(decor);
    int sim_d = creat("sim.sim", S_IRWXU);

    /*On lit le decor dans le buffer*/
    readMap(decor_d, buffer);
    
    /*On copie le buffer dans la simulation*/
    writeMap(buffer, sim_d);

}