// librerie standard di main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// inclusione dell'header gamelib.h
#include "gamelib.h"

// menu principale del source code di Tragic
int main(void){
    char *select_menu = malloc(LEN);
    int temp_menu = 0;

    clear_ansi();
    fflush(stdout);
    
    printf(ITA"\nPlease check if your window is at full-screen before moving on.\n");
    printf("\n\nAny key to continue...\n"CLR);
    getchar();
    
    clear_ansi(); // redraws the entire screen if input is undesired
    do{
        printf("Hello and welcome to <ENTER GAME NAME>!\n");
        printf("1) Setup Game\n2) Fight\n3) Quit\n4) Game Data\n5) Guide\n");
        printf("\nPlease select one of these...\n");
        
        // ask user to enter a key
        fgets(select_menu, LEN, stdin);
        // clean space full of undesired keys
        clear_arrBuffer(select_menu, NULL, 1);
        
        temp_menu = atoi(select_menu);

        switch(temp_menu){
            case 1:
            set_game(&temp_menu, 1);
            temp_menu = 0;
            break;
            case 2:
            set_game(&temp_menu, 2); // starts game if there's memory allocated
            temp_menu = 0;
            break;
            case 3:
            quit_game(&temp_menu); // no integer type passed through, it quits the game
            break;
            case 4:
            set_game(&temp_menu, 3); // goes to game data to check if there is content
            temp_menu = 0;
            break;
            case 5:
            set_game(&temp_menu, 4); // guide of the game
            temp_menu = 0;
            break;
            default:
            clear_ansi();
            printf("\nError: not valid input, please try again...\n\n");
            temp_menu = 0;
        }
    }while(temp_menu <= 0);
    return 0;
}