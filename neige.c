#include <string.h>
#include <time.h>
#include <stdlib.h> 
#include <unistd.h>

#include "ncurses_utils.h"
#include "file_utils.h"

#include "config.h"


int mWidth, mHeight, fd, mode;
unsigned char restartX, restartY, nbFlocon;
unsigned char** matrice;
WINDOW *fenetre_log, *fenetre_jeu, *fenetre_etat;
unsigned char* title;


void placer_element(int y, int x, unsigned char c, bool write);
int is_free(int y, int x);
void refresh_game();
void generation();

int main(int argc, char** argv) {
	
	int i, j, k, startMenu, sourisX, sourisY, bouton;
	int quitter = FALSE;
	WINDOW *box_jeu, *box_etat, *box_log;
	unsigned char* mapBuffer = malloc((LARGEUR2 - 2)*(HAUTEUR2 - 2) + 1 * sizeof(unsigned char));
	if(mapBuffer == NULL){
        perror("erreur lors du malloc ");
        exit(EXIT_FAILURE);
    }
	
	title = malloc(MAXFNAME * sizeof(unsigned char));
	if(title == NULL){
        perror("erreur lors du malloc ");
        exit(EXIT_FAILURE);
    }
	nbFlocon = -1;

	fenetre_log  = NULL;
	fenetre_jeu  = NULL;
	fenetre_etat = NULL;

	mWidth  = LARGEUR2 - 2;
	mHeight = HAUTEUR2 - 2;
	matrice = malloc(mHeight * sizeof( unsigned char*));

	if(matrice == NULL){
        perror("erreur lors du malloc ");
        exit(EXIT_FAILURE);
    }
	for(i = 0; i < mWidth; i++){
		matrice[i] = malloc(mWidth * sizeof(unsigned char));
		if(matrice[i] == NULL){
			perror("erreur lors du malloc ");
			exit(EXIT_FAILURE);
    	}
	}

	
	if(argc != 3){
		printf("mauvaise utilisation: ./neige [<type>] [<path>]\n");
		exit(EXIT_FAILURE);
	}else{
		if(strcmp(argv[1], "-N") != 0 && strcmp(argv[1], "-S") != 0){
			printf("mauvaise utilisation: ./neige [<type>] [<fichier>]\ntype:\n\t-N créer un nouveau decor\n\t-S lancer/reprendre une simulation\n");
			exit(EXIT_FAILURE);
		}
		if( strcmp(argv[1], "-N") == 0 ){
			if(strcmp(getFileExt(argv[2]), "bin") == 0){
				fd = openFile(argv[2]);
				if (lseek(fd, 0, SEEK_SET) == -1 ){
					perror("Erreur Lors du LSEEK main");
					exit(EXIT_FAILURE);
    			}
				
				readMap(fd, mapBuffer, &restartX, &restartY, &nbFlocon, title);
				mode = DEC_MODE;
			}else{
				printf("le decor doit etre un fichier .bin\n");
				exit(EXIT_FAILURE);
			}
			
		}
		if( strcmp(argv[1], "-S") == 0 ){
			
			if(strcmp(getFileExt(argv[2]), "sim") == 0){
				fd = openFileSim(argv[2], mapBuffer, &restartX, &restartY, &nbFlocon, title);
				mode = SIM_MODE;
			}
			else{
				printf("la simulation doit etre un fichier .sim\n");
				exit(EXIT_FAILURE);
			}
		}
	}

	ncurses_initialiser();

	box_log  = create_box(&fenetre_log,  HAUTEUR1, LARGEUR1, POSY1, POSX1);
	box_jeu  = create_box(&fenetre_jeu,  HAUTEUR2, LARGEUR2, POSY2, POSX2);
	box_etat = create_box(&fenetre_etat, HAUTEUR3, LARGEUR3, POSY3, POSX3);
	
	mvwprintw(box_jeu,  0, 3, "%s", (char*)title);
	mvwprintw(box_log,  0, 3, "Logs");
	mvwprintw(box_etat, 0, 3, "Etat");
	wrefresh(box_log );
	wrefresh(box_jeu );
	wrefresh(box_etat);

	scrollok(fenetre_log, TRUE);	
	
	for(i = 0, k = 0; i < mHeight; i++){
		for(j = 0; j < mWidth; j++, k++){
			placer_element(i, j, mapBuffer[k], false);
		}
	}
	

	if(mode == SIM_MODE){
		
		startMenu = START_MENU;
		mvwprintw(fenetre_etat, ++startMenu, 0, "-------");
		mvwprintw(fenetre_etat, ++startMenu, 0, "Quitter:    F2");
		mvwprintw(fenetre_etat, ++startMenu, 0, "Rafraichir: r");
		/*mvwprintw(fenetre_etat, ++startMenu, 0, "Generer:    ESPACE");*/
		++startMenu;
		mvwprintw(fenetre_etat, ++startMenu, 0, "Neige:      X");
		mvwprintw(fenetre_etat, ++startMenu, 0, "Obstacle:   +");
		mvwprintw(fenetre_etat, ++startMenu, 0, "Ratio:      1/%d", SPAWN_RATIO);
		mvwprintw(fenetre_etat, ++startMenu, 0, "-------");

		mvwprintw(fenetre_etat, 0, 0, "Nb flocons: %d     ", nbFlocon);

		wrefresh(fenetre_jeu);
		wrefresh(fenetre_etat);

		timeout(500);
		while(quitter == FALSE) {
		
			switch (getch())
			{
				case KEY_F(2):
					quitter = TRUE;
					break;
				
				case 'r':
				case 'R':
					refresh_game();
					break;

				default:
					generation();
					break;
			}
				
		}
	}
	else{

		startMenu = START_MENU;
		mvwprintw(fenetre_etat, ++startMenu, 0, "-------");
		mvwprintw(fenetre_etat, ++startMenu, 0, "Quitter:    F2");
		++startMenu;
		mvwprintw(fenetre_etat, ++startMenu, 0, "Obstacle:   +");
		mvwprintw(fenetre_etat, ++startMenu, 0, "-------");
		wrefresh(fenetre_etat);

		ncurses_souris();

		wrefresh(fenetre_jeu);
		wrefresh(box_jeu);
		
		while(quitter == FALSE) {
			i = getch();
			if( (int)i == KEY_MOUSE){
				if(souris_getpos(&sourisX, &sourisY, &bouton) == OK){
					sourisX--;
					sourisY--;
					if( (sourisX >= 0 && sourisX < (LARGEUR2 - 2) ) && ( (sourisY - POSY2) < HAUTEUR2 - 2 && (sourisY - POSY2) >= 0) ){
						
						if( is_free(sourisY - POSY2, sourisX) ){
							placer_element(sourisY - POSY2, sourisX, 2, true);
						}else{
							placer_element(sourisY - POSY2, sourisX , 0, true);
						}
						wrefresh(fenetre_jeu);
						
						wrefresh(fenetre_log);
					}
					

				}
				
			}
			else{
				switch (i)
				{
					case KEY_F(2):
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
	}
	
	close(fd);

	free( matrice ); /*impossible de libérer les matrice[i] ?? -> double free or corruption (out) Aborted (core dumped) */
	free( mapBuffer );
	free( title );


	if( ERR == delwin(fenetre_log)){
		perror("Erreur delwin, tentative free...");	
		free(fenetre_log);
	}
	if( ERR == delwin(fenetre_jeu)){
		perror("Erreur delwin, tentative free...");	
		free(fenetre_jeu);
	}
	if( ERR == delwin(fenetre_etat)){
		perror("Erreur delwin, tentative free...");	
		free(fenetre_etat);
	}
	if( ERR == delwin(box_log )){
		perror("Erreur delwin, tentative free...");	
		free(box_log);
	}
	if( ERR == delwin(box_jeu )){
		perror("Erreur delwin, tentative free...");	
		free(box_jeu);
	}
	if( ERR == delwin(box_etat)){
		perror("Erreur delwin, tentative free...");	
		free(box_etat);
	}

	ncurses_stopper();

	exit( EXIT_SUCCESS );

}


/*=== Définitions des fonctions ===*/


int is_free(int y, int x){ return matrice[y][x] == 0; }


void refresh_game(){
	int i,j, nbFdelete = 0;
	
	for(i = 0; i < HAUTEUR2; i++){
		for(j = 0; j < LARGEUR2; j++){
			if(matrice[i][j] == 1){
				nbFdelete++;
				placer_element(i, j, 0, true);
			} 
		}
	}

	nbFlocon = 0;
	writeNbF(fd, nbFlocon);

	wprintw(fenetre_log, "\nRafraichissement (%d flocons supprimé(s))", nbFdelete);
	
	mvwprintw(fenetre_etat, 0, 0, "Nb flocons: %d     ", nbFlocon);
	
	wrefresh(fenetre_log);
	wrefresh(fenetre_jeu);
	wrefresh(fenetre_etat);
}


void generation(){
	
	int i, j, isStuck, nbNew;
	srand(time(0));
	
	i = 0;
	j = 0;
	nbNew = 0;

	for( i = restartX == DEFAULT_POSITION ? mHeight - 2 : restartX; i >=  0; i--){
		for(j = restartY == DEFAULT_POSITION ? mWidth  - 2 : restartY; j > 0 ; j--){
			
			isStuck = FALSE;

			if(matrice[i][j] == 1){
				
				writeFallPosition(fd, i, j);
				restartY = i;
				restartX = j;

				if(rand() % 2){
					if(j > 1){

						if(is_free(i+1, j-1)){
							placer_element(i+1, j-1, 1, true);
						}else{
							if(is_free(i+1, j+1)){
								placer_element(i+1, j+1, 1, true);
							}else{
								if(is_free(i+1, j)){
									placer_element(i+1, j, 1, true);
								}else{
									isStuck = TRUE;
								}
							}
						}
						
					}else{

						if(is_free(i+1, j+1)){
							placer_element(i+1, j+1, 1, true);
						}else{
							if(is_free(i+1, j)){
								placer_element(i+1, j, 1, true);
							}else{
								isStuck = TRUE;
							}
						}
						
					}
					
				}
				else{
					if(j < mWidth){

						if(is_free(i+1, j+1)){
							placer_element(i+1, j+1, 1, true);
						}else{

							if(is_free(i+1, j-1)){
								placer_element(i+1, j-1, 1, true);
							}else{
								if(is_free(i+1, j)){
									placer_element(i+1, j, 1, true);
								}else{
									isStuck = TRUE;
								}
							}
						}
					}else{

						if(is_free(i+1, j-1)){
							placer_element(i+1, j-1, 1, true);
						}else{
							if(is_free(i+1, j)){
								placer_element(i+1, j, 1, true);
							}else{
								isStuck = TRUE;
							}
						}

					}
				}
				
				if(!isStuck){
					placer_element(i, j, 0, true);
					/**à chaque flocon**/
					/* getch(); wrefresh(fenetre_jeu); 
					*/
				}
				
				
				

				writeFallPosition(fd, DEFAULT_POSITION, DEFAULT_POSITION);
				restartY = DEFAULT_POSITION;
				restartX = DEFAULT_POSITION;
			}
		}
		
		
	}

	writeFallPosition(fd, DEFAULT_POSITION, DEFAULT_POSITION);
	restartY = DEFAULT_POSITION;
	restartX = DEFAULT_POSITION;

	for(j = 0; j < mWidth; j++){
		
		if( is_free(0, j) && (rand() % SPAWN_RATIO + 1) == SPAWN_RATIO){
			
			placer_element(0, j, 1, true);
			mvwprintw(fenetre_etat,0,0, "Nb flocons: %d     ", nbFlocon++);
			wrefresh(fenetre_etat);
			
			writeNbF(fd, nbFlocon);
			
			nbNew++;
		}
		
	}
	if(nbNew > 0){

		mvwprintw(fenetre_etat, 0, 0, "Nb flocons: %d     ", nbFlocon);
		
		wrefresh(fenetre_log);
	}

	wrefresh(fenetre_etat);

	wrefresh(fenetre_jeu);
	
}

void placer_element(int y, int x, unsigned char c, bool write){
	
	matrice[y][x] = c;
	
	switch (c)
	{
		case 0:
			mvwprintw(fenetre_jeu, y, x, " ");
			break;
		case 1 :
			mvwprintw(fenetre_jeu, y, x, "X");
			break;
		case 2:
			mvwprintw(fenetre_jeu, y, x, "+");
			break;
	
		default:
			mvwprintw(fenetre_jeu, y, x, "?");
			break;
	}

	if( write == true){
		insertElement(fd, y, x, c);
	}
	
}