#include <ncurses.h>
#include <ctime>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

// Taille du labyrinthe
const int HEIGHT = 25;
const int WIDTH = 53;

// Fonction qui génère une disposition aléatoire pour l'intérieur du labyrinthe sur le principe du retour sur trace
void generateMaze(std::vector<std::vector<char>>& maze, int row, int col) {
    // Definition des directions (up, down, left, right)
    const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    // Mélange au hasard des directions
    std::vector<int> indices = {0, 1, 2, 3};
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(indices.begin(), indices.end(), std::default_random_engine(seed));

    // Vérification des déplacements
    for (const auto& index : indices) {
        int newRow = row + directions[index][0] * 2;
        int newCol = col + directions[index][1] * 2;
        if (newRow > 0 && newRow < HEIGHT && newCol > 0 && newCol < WIDTH && maze[newRow][newCol] == '#') {
            maze[newRow][newCol] = ' ';
            maze[row + directions[index][0]][col + directions[index][1]] = ' ';
            generateMaze(maze, newRow, newCol);
        }
    }
}

// Fonction qui déssine le labyrinthe
void drawMaze() {
    // Initialisation des murs du labyrinthe
    std::vector<std::vector<char>> maze(HEIGHT, std::vector<char>(WIDTH, '#'));

    // Définition des positions d'entrée et de sortie
    const int entryRow = 1;
    const int entryCol = 1;
    const int exitRow = HEIGHT - 1;
    const int exitCol = WIDTH - 2;

    // Positionnement de l'entrée et de la sortie
    maze[entryRow][entryCol] = 'S';
    maze[exitRow][exitCol] = 'F';

    // Appel de la fonction de génération aléatoire du labyrinthe
    generateMaze(maze, entryRow, entryCol);

    // Dessin du labyrinthe
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            mvprintw(i, j, "%c", maze[i][j]);
        }
    }
}

// Fonction principale
int main() {
    // Initialisation de ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0); // Invisibilité du curseur

    // RNG
    srand(time(NULL));

    // Impression du labyrinthe à l'écran
    drawMaze();

    // Position de départ du joueur
    int playerY = 1;
    int playerX = 1;

    // Boucle principale du jeu
    while (true) {
        // Prise en compte de l'entrée utilisateur
        int input = getch();

        // Calcul de la postion suivante en fonction de entrée utilisateur
        int nextPlayerY = playerY;
        int nextPlayerX = playerX;
        switch (input) {
            case KEY_UP:
                nextPlayerY = playerY - 1;
                break;
            case KEY_DOWN:
                nextPlayerY = playerY + 1;
                break;
            case KEY_LEFT:
                nextPlayerX = playerX - 1;
                break;
            case KEY_RIGHT:
                nextPlayerX = playerX + 1;
                break;
            case 'q':
                endwin();
                return 0;
        }

        // Vérification si la prochaine position est un mur
        if (mvinch(nextPlayerY, nextPlayerX) == '#') {
            // Si cést un mur, le joueur ne bouge pas
            continue;
        }
         
        // Effacement de la position précédente du joueur
        mvprintw(playerY, playerX, " ");

        // Mise à jour de la postion du joueur
        playerY = nextPlayerY;
        playerX = nextPlayerX;
        mvprintw(playerY, playerX, "@");

        // Raffraichissement de l'écran
        refresh();

        // Vérification de la condition de victoire
        if (playerY == HEIGHT - 1 && playerX == WIDTH - 2) {
            mvprintw(HEIGHT / 1, WIDTH / 2 - 5, "You won!");
            refresh();
            getch();
            break;
        }
    }
    // Fermeture de ncurses
    endwin();

    //fin de la boucle
    return 0;
}
