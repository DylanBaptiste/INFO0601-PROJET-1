#include <string.h>
#include <time.h>
#include <stdlib.h> 
#include <unistd.h>

#include "ncurses_utils.h"
#include "file_utils.h"

#define LARGEUR1 80
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

#define SPAWN_RATIO	5  		/*1 chance sur SPAWN_RATIO qu'un flocon apparaisse dans chaque case de la prochaine generation*/
#define START_MENU	2

#define SIM_MODE 0
#define DEC_MODE 1

int mWidth, mHeight, nbGeneration, fd, mode;
unsigned char restartX, restartY, nbFlocon;
unsigned char** matrice;
WINDOW *fenetre_log, *fenetre_jeu, *fenetre_etat;
unsigned char* title;
void placer_element(int y, int x, unsigned char c){
	
	matrice[y][x] = c;
	
	switch (c)
	{
		case 0:
			mvwprintw(fenetre_jeu, y, x, " ");
			/*attron(COLOR_PAIR(0));
			mvaddch(y + HAUTEUR1 + 1, x+1, ' ');
			attroff(COLOR_PAIR(0));*/
			break;
		case 1 :
		mvwprintw(fenetre_jeu, y, x, "X");
		/*
			attron(COLOR_PAIR(1));
			mvaddch(y+HAUTEUR1 + 1, x+1, 'X');
			attroff(COLOR_PAIR(1));
			*/
			break;
		case 2:
		mvwprintw(fenetre_jeu, y, x, "+");
		/*
			attron(COLOR_PAIR(2));
			mvaddch(y+HAUTEUR1 + 1, x+1, '+');
			attroff(COLOR_PAIR(2));*/
			break;
	
		default:
		mvwprintw(fenetre_jeu, y, x, "?");
			/*mvaddch(y, x, '?');*/
			break;
	}

	
	
	insertElement(fd, y, x, c);
	

	/*wprintw(fenetre_log, "\n(%d, %d) %d", y, x, c);
	wrefresh(fenetre_log);*/
	
}

int is_free(int y, int x){
	return matrice[y][x] == 0;
}

void spawn(){
	
	int i, j, isStuck, nbNew;
	srand(time(0));
	
	i = 0;
	j = 0;
	nbNew = 0;
	nbGeneration++;

	for(i = mHeight - 2; i >=  0; i--){
		for(j = mWidth -2 ; j > 0 ; j--){
			
			isStuck = FALSE;

			if(matrice[i][j] == 1){
				
				if(rand() % 2){
					if(j > 1){

						if(is_free(i+1, j-1)){
							placer_element(i+1, j-1, 1);
						}else{
							if(is_free(i+1, j+1)){
								placer_element(i+1, j+1, 1);
							}else{
								if(is_free(i+1, j)){
									placer_element(i+1, j, 1);
								}else{
									
									/*le flocon est blocké*/
									isStuck = TRUE;
								}
							}
						}
						
					}else{

						if(is_free(i+1, j+1)){
							placer_element(i+1, j+1, 1);
						}else{
							if(is_free(i+1, j)){
								placer_element(i+1, j, 1);
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
							placer_element(i+1, j+1, 1);
						}else{

							if(is_free(i+1, j-1)){
								placer_element(i+1, j-1, 1);
							}else{
								if(is_free(i+1, j)){
									placer_element(i+1, j, 1);
								}else{
									/*le flocon est blocké*/
									isStuck = TRUE;
								}
							}
						}
					}else{

						if(is_free(i+1, j-1)){
							placer_element(i+1, j-1, 1);
						}else{
							if(is_free(i+1, j)){
								placer_element(i+1, j, 1);
							}else{
								/*le flocon est blocké*/
								isStuck = TRUE;
							}
						}

					}
				}
				
				if(!isStuck){
					placer_element(i, j, 0);
				}
				
				/**chaque f**/
				getch();
				wrefresh(fenetre_jeu);
			}
			
		}
		
		
	}

	/*srand(time(0)); Trouver une meilleur seed ?*/
	for(j = 0; j < mWidth; j++){
		
		if( is_free(0, j) && (rand() % SPAWN_RATIO + 1) == SPAWN_RATIO){
			
			/*getch();
			wrefresh(fenetre_jeu);*/

			placer_element(0, j, 1);
			mvwprintw(fenetre_etat,0,0, "Nb flocons: %d     ", nbFlocon++);
			wrefresh(fenetre_etat);
			nbNew++;
		}
		
	}
	if(nbNew > 0){

		/*wprintw(fenetre_log, "\nNouveaux flocons: %d", nbNew);*/
		mvwprintw(fenetre_etat, 0, 0, "Nb flocons: %d     ", nbFlocon);
		
		wrefresh(fenetre_log);
	}

	mvwprintw(fenetre_etat, 1, 0, "Generation: %d     ", nbGeneration);
	wrefresh(fenetre_etat);

	/*refresch au decalage total de chaque ligne et la génération de la premiere*/
	wrefresh(fenetre_jeu);
	
}

void refresh_game(){
	int i,j, nbFdelete = 0;
	nbGeneration = 0;

	
	
	for(i = 0; i < HAUTEUR2; i++){
		for(j = 0; j < LARGEUR2; j++){
			if(matrice[i][j] == 1){
				nbFdelete++;
				placer_element(i, j, 0);
				nbFlocon = 0;
			} 
		}
	}

	wprintw(fenetre_log, "\nRafraichissement (%d flocons supprimé(s))", nbFdelete);
	
	mvwprintw(fenetre_etat, 0, 0, "Nb flocons: %d     ", nbFlocon);
	mvwprintw(fenetre_etat, 1, 0, "Generation: %d     ", nbGeneration);
	
	wrefresh(fenetre_log);
	wrefresh(fenetre_jeu);
	wrefresh(fenetre_etat);
}



int main(int argc, char** argv) {
	
	int i, j, k, startMenu, sourisX, sourisY, bouton;
	int quitter = FALSE;
	unsigned char* mapBuffer/*[(LARGEUR2 - 2)*(HAUTEUR2 - 2) +1];*/ = malloc((LARGEUR2 - 2)*(HAUTEUR2 - 2) + 1 * sizeof(unsigned char));
	title = malloc(255 * sizeof(unsigned char));
	nbFlocon = -1;

	fenetre_log  = NULL;
	fenetre_jeu  = NULL;
	fenetre_etat = NULL;

	mWidth  = LARGEUR2 - 2;
	mHeight = HAUTEUR2 - 2;
	matrice = (unsigned char**)malloc(mHeight * sizeof( unsigned char*));
	for(i = 0; i < mWidth; i++){
		matrice[i] = (unsigned char*)malloc(mWidth * sizeof(unsigned char));
	}
	
	if(argc != 3){
		fprintf(stderr, "mauvaise utilisation: ./neige [<type>] [<path>]\n");
		exit(EXIT_FAILURE);
	}else{
		if(strcmp(argv[1], "-N") != 0 && strcmp(argv[1], "-S") != 0){
			fprintf(stderr, "mauvaise utilisation: ./neige [<type>] [<fichier>]\ntype:\n\t-N créer un nouveau decor\n\t-S lancer/reprendre une simulation\n");
			exit(EXIT_FAILURE);
		}
		if( strcmp(argv[1], "-N") == 0 ){
			if(strcmp(getFileExt(argv[2]), "bin") == 0){
				fd = openFile(argv[2]);
				lseek(fd, 0, SEEK_SET);
				readMap(fd, mapBuffer, &restartX, &restartY, &nbFlocon, title);
				printf("\nTitre: %s\nX: %d\nY:%d\nNbF:%d\n", title, restartX, restartY, nbFlocon);
				mode = DEC_MODE;
			}else{
				fprintf(stderr, "le decor doit etre un fichier .bin\n");
				exit(EXIT_FAILURE);
			}
			
		}
		if( strcmp(argv[1], "-S") == 0 ){
			
			if(strcmp(getFileExt(argv[2]), "sim") == 0){
				fd = openFileSim(argv[2], mapBuffer, &restartX, &restartY, &nbFlocon, title);
				mode = SIM_MODE;
			}
			else{
				fprintf(stderr, "la simulation doit etre un fichier .sim\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	 

	
	
	/*k = 0;
	for(i = 0; i < mHeight; i++){
		for(j = 0; j < mWidth; j++, k++){
			printf("%d", mapBuffer[k]);
			if((( (k % (LARGEUR2-2)) - (LARGEUR2-3) ))==0){ printf("\n");}
		}
	}
	printf("\n\n");*/
	


	ncurses_initialiser();

	create_box(&fenetre_log,  HAUTEUR1, LARGEUR1, POSY1, POSX1);
	create_box(&fenetre_jeu,  HAUTEUR2, LARGEUR2, POSY2, POSX2);
	create_box(&fenetre_etat, HAUTEUR3, LARGEUR3, POSY3, POSX3);
	
	scrollok(fenetre_log, TRUE);

	/*ncurses_couleurs();*/
	
	
	k = 0;
	for(i = 0; i < mHeight; i++){
		for(j = 0; j < mWidth; j++, k++){
			/* Chargement de la simulation */
			placer_element(i, j, mapBuffer[k]);
			
			/*printf("%d", matrice[i][j]);*/
		}
	}
	
	

	if(mode == SIM_MODE){
		
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


		mvwprintw(fenetre_etat, 0, 0, "Nb flocons: %d     ", nbFlocon);

		wrefresh(fenetre_jeu);
		wrefresh(fenetre_etat);

		while(quitter == FALSE) {
		
			
			
			switch (getch())
			{
				case 'q':
				case 'Q':
					quitter = TRUE;
					break;
				
				case 'r':
				case 'R':
					refresh_game();
					break;

				default:
					spawn();
					break;
			}
				
		}
		close(fd);

	}
	else{

		startMenu = START_MENU;
		mvwprintw(fenetre_etat, ++startMenu, 0, "-------");
		mvwprintw(fenetre_etat, ++startMenu, 0, "Quitter:    q");
		++startMenu;
		mvwprintw(fenetre_etat, ++startMenu, 0, "Obstacle:   +");
		mvwprintw(fenetre_etat, ++startMenu, 0, "-------");
		wrefresh(fenetre_etat);

		ncurses_souris();
		/*ncurses_couleurs();*/
		wrefresh(fenetre_jeu);

		

		/*writeFallPosition(fd, 255, 254);
		writeNbF(fd, 10);*/
		/*writeTitle(fd, 'a');*/

		wprintw(fenetre_log, "Titre: %s", title);
		wrefresh(fenetre_log);


		while(quitter == FALSE) {
			i = getch();
			if( (int)i == KEY_MOUSE){
				if(souris_getpos(&sourisX, &sourisY, &bouton) == OK){
					sourisX--;
					sourisY--;
					if( (sourisX >= 0 && sourisX < (LARGEUR2 - 2) ) && ( (sourisY - POSY2) < HAUTEUR2 - 2 && (sourisY - POSY2) >= 0) ){
						/*wprintw(fenetre_log, "\n(%d, %d)", sourisX, sourisY - POSY2);*/
						
						if( is_free(sourisY - POSY2, sourisX) ){
							placer_element(sourisY - POSY2, sourisX, 2);
						}else{
							placer_element(sourisY - POSY2, sourisX , 0);
						}
						wrefresh(fenetre_jeu);
						
						wrefresh(fenetre_log);
					}
					

				}
				
			}
			else{
				switch (i)
				{
					case 'q':
					case 'Q':
						quitter = TRUE;
						break;
					
					case '\n' : case '\t': case '\r': case '\0': break;
					case ' ':
						wprintw(fenetre_log, "_");
						wrefresh(fenetre_log);
						writeTitle(fd, '_');
						break;
					default:
						wprintw(fenetre_log, "%c", i);
						wrefresh(fenetre_log);
						writeTitle(fd, i);
						break;
				}
			}

			
			
		}
		
		close(fd);
	}
	
	/*
	for(i = 0; i < mWidth; i++){
		free(matrice[i]);
	}

	delwin(fenetre_log);
	delwin(fenetre_jeu);
	delwin(fenetre_etat);
	*/
	
	ncurses_stopper();

	return EXIT_SUCCESS;
}