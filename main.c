// librerie standard di main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// inclusione dell'header gamelib.h
#include "gamelib.h"

// menu principale del source code di Tragic
int main(void){
    char *selezione_menu = malloc(LEN);
    int temp_menu = 0;
    clear_BufferANSI();
    // ciclo do_while fondamentale in caso l'utente selezioni una scelta sbagliata
    // per puro caso, oppure volontariamente per testare la fallacità del programma
    // dato che è molto probabile, il loop chiederà all'infinito all'utente
    // di scegliere, ovvero di mettere in input, il numero 1, 2 o 3, finché non ci saranno
    // uno e solo uno di questi numeri
    
    // chiamata con un parametro statico, per prevenire fgets e i suoi eventuali blocchi
    // in input.
    fflush(stdout);
    
    // output di disclaimer prima di stampare il menu princiale.
    printf(ITA"\nQuesto gioco ha bisogno di essere a schermo intero\n\nper poter funzionare correttamente.\n");
    printf("\n\nPremi un tasto qualsiasi per continuare...\n"CLR);
    getchar();
    
    // pulisci la schermata che invita a mettere a schermo intero
    clear_BufferANSI();
    do{
        printf("Salve, straniero della gilda di Ravnika e benvenuto in Tragic!\n");
        printf("1) Imposta gioco\n2) Combatti\n3) Termina gioco\n4) Dati Giocatori\n5) Guida Gioco\n");
        printf("\nSelezionare una di queste opzioni...\n");
        
        // chiede all'utente di selezionare un input
        fgets(selezione_menu, LEN, stdin);
        // pulisce ogni spazio e fa da secondo controllo se l'utente preme solo invio,
        // oppure immette solo spazi come input.
        clear_arrBuffer(selezione_menu, NULL, 1);
        
        // passa il carattere di selezione_menu implicitamente convertito ad int
        temp_menu = atoi(selezione_menu);

        // cosa l'utente ha scelto determineranno le fondamentali chiamate di funzione per:
        // impostare il gioco, giocare, vedere la memoria allocata sotto forma di interfaccia grafica,
        // ed uscire e deallocare se esiste memoria allocata dinamicamente
        switch(temp_menu){
            case 1:
            // va alle impostazioni ed impostera' il gioco se non e' stata gia' allocata
            // la memoria dinamicamente.
            imposta_gioco(&temp_menu, 1);
            temp_menu = 0;
            break;
            case 2:
            // avvia il gioco, sempre se e' stata allocata memoria dinamica prima la quale
            // un integer temp_menu informera' in questo file se e' stata allocata oppure no.

            // se la condizione è falsa ripete il loop assegnando la variabile 0
            // in modo tale da poter rispettare la condizione do_while
            // finché il bool globale "impostato" non sarà 1 ovvero true.
            imposta_gioco(&temp_menu, 2);

            temp_menu = 0;
            break;
            case 3:
            // termina l'esecuzione del programma e determina
            termina_gioco(&temp_menu);
            break;
            case 4:
            // mostra tutta la memoria creata dinamicamente e gli eventuali indirizzi
            // dentro gli array dedicati al mazzo e al campo di ogni giocatore.
            // Anche se non e' stata allocata memoria sara' comunque possibile visualizzare
            // se la memoria e' stata allocata oppure no, ma verra' mostrato in output all'utente
            // il valore NULL.
            // Questo menu ha un interfaccia grafica comprensibile ed intuitiva all'utente.
            imposta_gioco(&temp_menu, 3);
            temp_menu = 0;
            break;
            case 5:
            imposta_gioco(&temp_menu, 4);
            temp_menu = 0;
            break;
            default:
            // input sbagliato, ripeti il ciclo finché l'utente non effettua un input
            // giusto (anche per uscire dal programma).
            clear_BufferANSI();
            printf("\nErrore: scelta non valida, riprovare...\n\n");
            temp_menu = 0;
        }
    }while(temp_menu <= 0);
    return 0; // ritorna un valore che segnala che l'esecuzione e' terminata positivamente.
}