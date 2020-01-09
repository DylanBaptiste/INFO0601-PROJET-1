#include <string.h>
#include <time.h>
#include <stdlib.h>     
#include <ncurses.h> 
#include <unistd.h>

#include "ncurses_utils.h"
#include "file_utils.h"

#define LARGEUR1 82
#define HAUTEUR1 5
#define POSX1    0
#define POSY1    0

#define LARGEUR2 60 
#define HAUTEUR2 20  
#define POSX2    POSX1
#define POSY2    HAUTEUR1 

#define LARGEUR3 ( LARGEUR1 - LARGEUR2 )
#define HAUTEUR3 HAUTEUR2  
#define POSX3    LARGEUR2  
#define POSY3    HAUTEUR1  

#define SPAWN_RATIO	5 		/*1 chance sur SPAWN_RATIO qu'un flocon apparaisse dans chaque case de la prochaine generation*/
#define START_MENU	2

int nbFlocon, mWidth, mHeight, nbGeneration;
char** matrice;
WINDOW *fenetre_log, *fenetre_jeu, *fenetre_etat;

void placer_element(int y, int x, char c){
	/* TODO update la map ici */
	matrice[y][x] = c;
	mvwprintw(fenetre_jeu, y, x, "%c", c);
}

int is_free(int y, int x){
	return matrice[y][x] == ' ';
}

void spawn(){
	
	int i, j, isStuck, nbNew;
	srand(time(0));
	
	i = 0;
	j = 0;
	nbNew = 0;
	nbGeneration++;

	for(i = mHeight - 1; i >=  0; i--){
		for(j = mWidth; j >= 0 ; j--){
			
			isStuck = FALSE;

			if(matrice[i][j] == 'X'){
				
				if(rand() % 2){
					if(j > 0){

						if(is_free(i+1, j-1)){
							placer_element(i+1, j-1, 'X');
						}else{
							if(is_free(i+1, j+1)){
								placer_element(i+1, j+1, 'X');
							}else{
								if(is_free(i+1, j)){
									placer_element(i+1, j, 'X');
								}else{
									/*le flocon est blocké*/
									isStuck = TRUE;
								}
							}
						}
						
					}else{

						if(is_free(i+1, j+1)){
							placer_element(i+1, j+1, 'X');
						}else{
							if(is_free(i+1, j)){
								placer_element(i+1, j, 'X');
							}else{
								/*le flocon est blocké*/
								isStuck = TRUE;
							}
						}
						
					}
					
				}
				else{
					if(j < mWidth){

						if(is_free(i+1, j+1)){
							placer_element(i+1, j+1, 'X');
						}else{

							if(is_free(i+1, j-1)){
								placer_element(i+1, j-1, 'X');
							}else{
								if(is_free(i+1, j)){
									placer_element(i+1, j, 'X');
								}else{
									/*le flocon est blocké*/
									isStuck = TRUE;
								}
							}
						}
					}else{

						if(is_free(i+1, j-1)){
							placer_element(i+1, j-1, 'X');
						}else{
							if(is_free(i+1, j)){
								placer_element(i+1, j, 'X');
							}else{
								/*le flocon est blocké*/
								isStuck = TRUE;
							}
						}

					}
				}
				
				if(!isStuck){
					placer_element(i, j, ' ');
				}
				
				
			}
			
		}
		
		
	}

	/*srand(time(0)); Trouver une meilleur seed ?*/
	for(j = 0; j < mWidth; j++){
		
		if( is_free(0, j) && (rand() % SPAWN_RATIO + 1) == SPAWN_RATIO){
			placer_element(0, j, 'X');
			mvwprintw(fenetre_etat,0,0, "Nb flocons: %d     ", nbFlocon++);
			wrefresh(fenetre_etat);
			nbNew++;
		}
		
	}
	if(nbNew > 0){

		wprintw(fenetre_log, "\nNouveaux flocons: %d", nbNew);
		mvwprintw(fenetre_etat, 0, 0, "Nb flocons: %d     ", nbFlocon);
		
		wrefresh(fenetre_log);
	}

	mvwprintw(fenetre_etat, 1, 0, "Generation: %d     ", nbGeneration);
	wrefresh(fenetre_etat);

	/*refresch au decalage total de chaque ligne et la génération de la premiere*/
	wrefresh(fenetre_jeu);
	
}

void refresh_game(){
	int i,j;
	nbGeneration = 0;

	wprintw(fenetre_log, "\nRafraichissement");
	
	for(i = 0; i < HAUTEUR2; i++){
		for(j = 0; j < LARGEUR2; j++){
			if(matrice[i][j] == 'X'){
				placer_element(i, j, ' ');
				nbFlocon = 0;
			} 
		}
	}

	
	mvwprintw(fenetre_etat, 0, 0, "Nb flocons: %d     ", nbFlocon);
	mvwprintw(fenetre_etat, 1, 0, "Generation: %d     ", nbGeneration);
	
	wrefresh(fenetre_log);
	wrefresh(fenetre_jeu);
	wrefresh(fenetre_etat);
}


int main() {
	
	int i, j, k, startMenu, fd;
	int quitter = FALSE;
	char* mapBuffer = malloc(1044 * sizeof(char));

	nbFlocon = 0;

	fenetre_log  = NULL;
	fenetre_jeu  = NULL;
	fenetre_etat = NULL;

	mWidth  = LARGEUR2 - 2;
	mHeight = HAUTEUR2 - 2;

	matrice = (char**)malloc(mHeight * sizeof(char*));
	for(i = 0; i < mWidth; i++){
		matrice[i] = (char*)malloc(mWidth * sizeof(char));
	}


	ncurses_initialiser();

    create_box(&fenetre_log,  HAUTEUR1, LARGEUR1, POSY1, POSX1);
    create_box(&fenetre_jeu,  HAUTEUR2, LARGEUR2, POSY2, POSX2);
    create_box(&fenetre_etat, HAUTEUR3, LARGEUR3, POSY3, POSX3);
	
	scrollok(fenetre_log, TRUE);

	fd = readMap("map1", mapBuffer);
	
	k = 0;
	for(i = 0; i < mHeight; i++){
		for(j = 0; j < mWidth; j++, k++){
			/* Chargement de la map */
			placer_element(i, j, mapBuffer[k]);
			/*printf("%c,", mapBuffer[k]);*/
			/*printf("%c,", matrice[i][j]);*/
		}
	}
	
	startMenu = START_MENU;
	mvwprintw(fenetre_etat, ++startMenu, 0, "-------");
	mvwprintw(fenetre_etat, ++startMenu, 0, "Quitter:    q");
	mvwprintw(fenetre_etat, ++startMenu, 0, "Rafraichir: r");
	mvwprintw(fenetre_etat, ++startMenu, 0, "Generer:    ESPACE");
	++startMenu;
	mvwprintw(fenetre_etat, ++startMenu, 0, "Neige:      X");
	mvwprintw(fenetre_etat, ++startMenu, 0, "Obstacle:   -");
	mvwprintw(fenetre_etat, ++startMenu, 0, "Ratio:      1/%d", SPAWN_RATIO);
	mvwprintw(fenetre_etat, ++startMenu, 0, "-------");
	wrefresh(fenetre_etat);


	ncurses_souris();


	while(quitter == FALSE) {
		
		
		i = getch();
		spawn();

		if(i == 'q' || i == 'Q')
			quitter = TRUE;
		
		if(i == 'r' || i == 'R'){
			refresh_game();
		}
				
	}

	/*
	for(i = 0; i < mWidth; i++){
		free(matrice[i]);
	}

	delwin(fenetre_log);
	delwin(fenetre_jeu);
	delwin(fenetre_etat);
	*/
	close(fd);
	ncurses_stopper();

	return EXIT_SUCCESS;
}