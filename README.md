# INFO0601-PROJET-1

Création de carte et simulation de neige qui tombe avec ncurses

## Utilisation

- Générer les sources: 
    ```
    $> make
    ```
- Créé une carte: 
    ```
    $> ./neige.out -N decor.bin
    ```
    - Cliquer sur la fenêtre de jeu pour ajouter un obstacle
    - Cliquer sur un obstacle pour le retirer
    - Taper au clavier le titre du decor
    - F2 pour quitter

- Lancer/Reprendre une simulation: 
    ```
    $> ./neige.out -S decor.sim
    ```
    - Nouvelle génération toutes les 500ms
    - Taper R pour supprimer tous les flocons
    - F2 pour qitter
    
    (La ligne 377 de neige.c peut etre décommenté pour voir une actualisation flocon par flocon et donc s'assurer de la bonne reprise de la simulation apres avoir quitté)
- Netoyer le dossier: 
    ```
    $> make clean
    ```

## Auteurs
|||
|----------------|---------------------------------------|
| Dylan BAPTISTE | dylan.baptiste@etudiant.univ-reims.fr |
| Thomas POGU    | thomas.pogu@etudiant.univ-reims.fr    |
