
// definizione costante che delimita il range onde evitare memory leaks dopo la 
// chiamata a fgets
#define LEN 1024
// definizione di costanti assegnate per ognuna un codice ANSI 
// per la modifica in RGB dei colori in output
#define RED "\e[0;31m"
#define HRED "\e[0;91m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define ONG "\e[38;5;209m"
#define BLU "\e[0;34m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"
#define DRK_GRY "\e[0;2m"

// altri formati ANSI
#define ITA "\e[3m"
#define BOL "\e[1m"
#define UND "\e[4m"

// pulisce tutti i formati ANSI utilizzati in precedenza
#define CLR "\e[0m"

// librerie standard dell'header gamelib.h
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// -------------
// posto per le dichiarazioni di funzione prototipo void specificate con lo specifier extern
// la loro dichiarazione in questo file, la loro definizione in gamelib.c.
extern void set_game(int *, int);
extern void start_game(int *);
extern void quit_game(int *);
// -------------
// dichiarazioni delle struct e dei suoi elementi, incluse la loro definizione al prossimo
// indirizzo per le liste dinamiche
struct Mago{
    char *nome;
    int mago_punti_vita;

    enum Classe_mago{
        tenebre,
        vita,
        luce
    }classe_mago;
    
    struct Carta* mazzo;
    struct Carta* mano;
    struct Carta* campo;

    struct Mago *next_mago;
};
struct Carta{

    enum Tipo_carta{        // probabilita' di avere questi tipi nel mazzo di ogni mago:
        creatura,           // 40%
        rimuovi_creatura,   // 25%
        infliggi_danno,     // 20%
        guarisci_danno      // 15%
    }tipo_carta;

    int carta_punti_vita;
    struct Carta *next_carta;
};
// -------------
// le definizioni per ogni struct dichiarata sopra
typedef struct Mago Mago;
typedef struct Carta Mazzo;
typedef struct Carta Mano;
typedef struct Carta Campo;
// -------------
static void clear_arrbuffer(char *, int *, int);
static void clear_ansi(void);
// -------------
// questa funzione semplicemente rende l'input leggibile ad ogni chiamata fgets
// esattamente come lo vuole intendere l'utente, ed anche a prova di utente
// che vuole testare, oppure sbaglia a immettere l'input in molteplici modi
// questa funzione tenta di prevenire questo, in 2 strategie diverse
static void clear_arrbuffer(char *Buffer, int *passa_input, int scelta_Buffer)
{
    bool err_conferma = false;

    for (int a = 0; a < strlen(Buffer); a++)
    {
        if (Buffer[a] == 10)
            Buffer[a] = 0; // pulisce carattere ASCII <space> riempito dopo la chiamata fgets
    }

    if (Buffer[0] == 0)
    {
        clear_ansi();
        // se non ci sono caratteri fa ritornare l'esecuzione alla chiamata di questa funzione
        // portandosi dietro di se (oppure no, decide il programmatore) un valore booleano
        // di 1, segnalando che esiste un input errato.
        printf("\nErrore: l'input inserito non ha caratteri, per favore riprovare!\n\n");
        err_conferma = true;
    }

    switch (scelta_Buffer)
    {
    // prima soluzione adottata
    case 1:
        if (Buffer[0] == 32)
        {
            // anche se l'input e' corretto, riconosce che c'è stato uno spazio prima
            printf("\nErrore: l'input inserito inizia con uno spazio, per favore riprovare!\n\n");
            err_conferma = true;
        }
        if (strlen(Buffer) > 15)
        {
            // verifica se il valore della lunghezza della stringa, supera 15,
            // utile per l'assegnazione del nome ad un mago durante il character creation nel gioco.
            
            clear_ansi(); // "pulisce" l'output se l'input e' maggiore della lunghezza che vale 15.
            if (err_conferma == false)
                printf("\nErrore: l'input inserito e' troppo grande (MAX. 15 caratteri), per favore riprovare!\n\n");
            err_conferma = true;
        }
        break;
    // seconda soluzione adottata
    case 2:
        // effettua un ciclo per quanto e' lunga la stringa Buffer, passata per indirizzo.
        for (int f = 0; f < strlen(Buffer); f++)
        {
            // se non ci sono caratteri spazio oppure invio, la condizione e' vera
            // entra nel block scope
            if (Buffer[f] != 0 && Buffer[f] != 32)
            {
                // un ciclo nested for che fa la stessa cosa di quello precedente
                // ma con la differenza che incrementa di 1 in più rispetto
                // al ciclo che ospita quest'ultimo.

                // Il motivo e' semplicemente quello di individuare la presenza di secondi
                // caratteri, per l'inserimento di input che superano un carattere (esempio: 12),
                // perche' durante l'esecuzione del programma, l'utente avvolte dovra' immettere
                // input che superano il valore 9, e' quindi siccome non viene utilizzato scanf,
                // questa e' una soluzione conveniente, che ovviamente, presenta delle limitazioni.
                for (int s = f + 1; s < strlen(Buffer); s++)
                {
                    if (Buffer[s] != 0 && Buffer[s] != 32)
                    {
                        if (err_conferma == false){
                            clear_ansi();
                            printf("\nErrore: l'input inserito non e' stato riconosciuto, per favore riprovare!\n\n");
                            // il controllo ha avuto un esito negativo (e' presente un errore).
                        }
                        err_conferma = true;
                        break;
                    }
                }
                if (err_conferma == false)
                {   // se la seconda verifica ha avuto esito positivo, assegna al primo
                    // carattere della stringa, quello di Buffer[f].
                    *Buffer = Buffer[f];
                }
                else
                {
                    // se la seconda verifica ha avuto un esito negativo, reimposta la stringa
                    // sia in memoria, che il suo valore in caratteri.
                    memset(Buffer, 0, strlen(Buffer));
                }
                break;
            }
        }
        break;
            // se nessun valore e' lo stesso dei case, allora ripeti il ciclo (se l'utente lo decide).
        default:
            clear_ansi(); // pulisce l'output se l'input non e' valido.
            printf("\nErrore: Nessun valore corrisponde ad un input da analizzare!\n");
            err_conferma = false;
            *passa_input = 0;
    }

    if (passa_input != NULL && err_conferma == false)
        *passa_input = 1;
    else if (passa_input != NULL && err_conferma == true)
    {
        // stesso sistema di "cancellazione" della stringa, una volta che non trova
        // altri caratteri nel controllo dentro il ciclo
        memset(Buffer, 0, strlen(Buffer));
        // ripeti il ciclo do while, se *passa_input l'utente gli ha assegnato un valore
        // durante la chiamata di funzione per parametri
        *passa_input = 0;
    }
    return;
}

// questa funzione "pulisce" l'output del terminale
static void clear_ansi(void){
    printf("\e[1;1H\e[2J");
    return;
}