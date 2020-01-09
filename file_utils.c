#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>


void constructeurFile(int fd){
    char caseVide = ' ';
    int nbrflocon = 1;
    int i;

    for(i = 0; i < 1044; i++){
        if(write(fd, &caseVide, sizeof(char)) == -1){
            perror("bug ecriture fichier");
            exit(EXIT_FAILURE);
        }    
    }
    if(write(fd, &nbrflocon, sizeof(int)) == -1){
        perror("bug ecriture fichier int");
        exit(EXIT_FAILURE);
    }  
}

int openFile(char* Adresse)
{
    int fd = open(Adresse, O_RDWR);
    /*printf("fd : %d", fd);*/
    
    if (fd >= 0){
        /*printf("fichier ouvert\n");*/
    }
    else if (errno == ENOENT){ /*the named file does not exist*/
        fd = creat(Adresse, S_IRWXU); /* Creation avec droit à tlm*/
        /*printf("fd : %d",fd);*/
        
        if (fd >= 0 ){
            /*printf("file initialise\n");*/
            constructeurFile(fd); /* Cree une map 30*15 pleine de 0*/
        } else
        {
            perror("Erreur lors de la création\n");
            exit(EXIT_FAILURE);
        }
    }
    return fd;
}

int readMap(char* fileName, char* res)
{
    int fd, taille = 0;
    char buffer[1044];
    fd = openFile(fileName);
    
    if(fd > 0){
        while( read(fd, &buffer, sizeof(char)) != 0)
        {
            res[taille] = *buffer;
            taille++;
            /*printf("%c %c", *buffer, res[taille]);*/
        }
        /*printf("\n");*/
    }
    
    
    /*printf("res: %s", res);*/
    
    return fd;
} 