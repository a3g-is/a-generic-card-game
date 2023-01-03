#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "gamelib.h"

static void init_variable(void);
static void init_handfield(int, int);
static void allocate_mem(void);
static void dealloc_mem(void);
static void scan_mage(int);
static void scan_deck(int, Mazzo *[], Mazzo *, int *, int);
static void print_mem(int, int, int);
static void highlight_mem(int, int, int);
static void thereis_deck(int, int *);
static void draw_card(int, int *);
static void get_arraddress(Mago **, Mazzo **, int, int, int);
static void guide_menu(void);
static void print_guidemenu(int *, int);
static void mem_menu(void);
static void game_input(char *, int *[2], int *, int *, int *);
static void choiceof_play(int *[2], int *);
static void targetinput_sel(int *, int, int);
static void print_mage(int, int);
static void print_deck(int, int, int *);
static void sel_handsorted(int, int);
static void arr_handsort(Mano *[], int);
static void clear_arrbuffer(char *, int *, int);
static void sel_targetoutput(int, int *[2], int *, int, int);
static void print_memaddr(int, int, int);
static void print_addr(Mazzo *[], Mazzo *, Mago *, int, int);
static void print_gameout(int);
static void print_playername(int, int, int);
static void print_playerdeck(char **, int, int);
static void check_gamefieldlayer(int *, int *, int);
static void validate_gamefield(Mazzo *[], int *[2], int *, int, int);
static void print_fieldtable(int, int, int);
static void print_validchoices(int, int *[2], int *, int, int);
static void delete_allmem(void);
static void delete_memdata(Mago **, Mazzo **, int, int);
static void clear_ansi(void);
static void set_delayclock(int, int);
static void gamemode_menu(void);
static void magedeck_menu(int, int *);
static void generate_card(int *);
static void generate_hp(int *, int);
static void transfert_card(int, int, int);
static void create_mage(int, int, char *);
static void create_deckand_hand(int, int, int, int);
static void empty_magedeck(int);
static void empty_magehand(int, int, int);
static void add_cardtohand(int, int, int);
static void print_magehand(Mano *[], int, int);
static void print_gamefield(Campo *[], int, int);
// -------------
long double delay;
int select_menu, select_player, target_player, select_handdeck, target_handdeck;
int n_mago[2], n_deck[4], n_mano[4];
bool impostato;
// -------------
Mago *mago[4], *ptrmago, *ptr_selMago, *ptr_tarMago, *ptrmg_inizio, *ptrmg_fine;
Mazzo *ptrmazzo, *ptr_selCarta, *ptr_tarCarta, *ptrmz_inizio[4], *ptrmz_fine[4];
Mano *arr_Mano1[6], *arr_Mano2[6], *arr_ManoAvv[6], *arr_Mano2Avv[6];
Campo *arr_Campo1[4], *arr_Campo2[4], *arr_CampoAvv[4], *arr_Campo2Avv[4];
// -------------
// dichiarazione del set di numeri che verranno
// sorteggiati nella funzione rand
time_t t;
// -------------
static void pesca(int, int *, int *, int *);
static void gioca_carta(Mazzo *, Mazzo *, Mago *, int *[2]);
static void attacca(Mazzo *, Mazzo *, Mago *, int *[2]);
static void passa(int *);
// -------------
// posto per tutte gli i tipi enumeratori e le loro definizioni
// lo scopo principale di questo enumeratore e' quello di identificare ogni singola
// lista dinamica allocata nell'heap, i nomi di questi elementi sono associati ai
// giocatori di cui hanno per ognuno una propria lista, il tutto per facilitare
// una qualsiasi operazione associabile con una lista dinamica.
// Viene utilizzata anche per eventuali dati da stampare in output,
// oppure per eseguire alcune istruzioni per eventuali array sempre associabili
// ai loro nomi (esempio: la Mano1 e' associabile al Mago1), e quindi non solo per 
// le liste dinamiche.
typedef enum Memoria_Mago{
    _none_memory,

    _Mago,
    _ManoCampo,
    _Mazzo,

    _Mago2,
    _ManoCampo2,
    _Mazzo2,

    _MagoAvv,
    _ManoCampoAvv,
    _MazzoAvv,

    _Mago2Avv,
    _ManoCampo2Avv,
    _Mazzo2Avv
}tipo_mago;
tipo_mago giocatore;
tipo_mago avversario;

// definizione che esplica chi sta giocando nel turno corrente come doppia conferma
// per prevenire eventuali problemi di errore nell'assegnazione dei dati al giocatore,
// definizione estremamente cruciale per evitare di andare in segmentation fault.
tipo_mago curr_giocatore;
tipo_mago curr_avversario;

// questo enumeratore serve per associare ad ogni singolo evento legato al gioco,
// ovvero se le definizioni sono uguali a valori di questi elementi, allora, ad esempio,
// esegui una chiamata di funzione per attaccare una creatura sul campo, oppure
// stampa la schermata finale che stabilisce chi e' il vincitore, oppure
// fai passare il turno, e così via, il tutto sempre per essere il più intuitivo possibile
// all'occhio di chi sta revisionando il codice e del programmatore stesso che deve
// gestire tantissime istruzioni legate al gioco, e nel caso di questo progetto,
// persino gli input per eseguire comandi associati alle carte sempre in costante
// variazione di comandi disponibili e non.
typedef enum Evento_Gioco{
    _none_event,

    _skip_turn,
    _first_turn,
    _change_turn,
    _pause_game,
    _completed_action,

    _print_beginning,
    _print_ending,
    
    _draw,

    _dead_mage,
    _dead_by_hand,
    _straight_attack,
    _deal_mage_dmg,
    _deal_creature_dmg,

    _heal_mage,
    _heal_creature,

    _evoke_creature,
    _play_throwaway_card,
}set_event;
// action[3] esprime il numero di azioni rimanenti per ogni singolo giocatore
// a cui spetta una decisione inizialmente forzata, finché non sono tutti e tre
// gli elementi veri, il turno non cambia in automatico, quindi il giocatore
// non puo' scegliere di cambiare il turno di sua spontaneo volere finche' non
// rende true almeno uno di questi elementi.
set_event action[3];
// definizione generale degli eventi piu' importanti per la partita (esempio: pesca una carta)
set_event event;
// definizione che esprime la creatura a chi sta puntando per un attacco (non verra' mai associato
// ad una carta infliggi_danno, per quello se ne occupa event).
set_event type_target;
// il mago e' stato ucciso oppure ha terminato le carte quindi non puo' giocare.
set_event dead_mage;

// in base a quello che l'utente vuole, gioca contro un solo giocatore, oppure
// in squadre due contro due e mode avra' assegnato il valore associato a queste scelte,
// il primo ovvero _none_mode (0), verra' assegnato in automatico solo quando non e'
// stata allocata memoria.
typedef enum GameMode
{
    _none_mode,
    _1versus1,
    _2versus2
} set_mode;
set_mode mode;

// enumeratore di output: stampa nel menu dedicato all'esposizione di tutta la memoria
// allocata dinamicamente, oppure semplicemente identifica chi sta giocando nel campo,
// senza lasciare solamente il nome e creare confusione nel caso ci siano nomi identici
// tra giocatori.
// Inoltre durante la partita alcuni output che identificano spariscono finché non si esce,
// per tornare a visitare il menu e vedere come e' messa la memoria dopo magari,
// aver distrutto una carta creatura, ed eventualmente mostrare a chi appartiene il mazzo,
// cosa che durante la partita per motivi di spazio, non verra' mostrato, eccetto per il nome
// del mago.
typedef enum Titolo{
    _title_visible,
    _title_nonvisible,    
    _title_only_mage_deck_visible,

}vis_titolo;
vis_titolo mem_title;

// per convenienza, la mano e il campo hanno lo stesso posto negli switch dedicati alle
// operazioni di assegnazione ed output, _side_mazzo dice semplicemente, mostra il mazzo
// di quel mago, _side_campo mostra il campo del mago che associato, pero' il valore uguale
// al loro elemento nell'enumeratore (3 mazzo, 4 mano), incide sull'iterazione dell'altro
// (se passo _side_mano tra i parametri, non potra' iterare la condizione else di _side_campo).
typedef enum Tipo_Array{
    _none_side,
    

    _side_mago, // dedicato alla stampa dell'elemento associato al puntatore array dei singoli maghi.
    _side_nodoMago, // dedicato alla deallocazione in memoria dei singoli maghi in memoria

    _side_mazzo,
    _side_nodoMazzo,
    
    _side_mano,
    _side_campo,

}set_side;


// enumeratore che associa i suoi membri ad una selezione nel campo di battaglia
// che il giocatore prende in input, oppure tutte le selezioni numerate possibili
// (incluso vedere l'indirizzo di memoria, anche se e' NULL).
typedef enum Stampa_EvMemFun{
    _none_print_choice,
    
    _print_mem,
    _highlight_mem,
    _self_dmg,

    _show_single_address_mem,
    _show_all_address_mem,

    _show_all_selections,
    
    _show_dmg_selections,
    _show_only_creatures_dmg,

    _show_creature_summonings,

    _show_heal_selections
}print_choice;
// stampa la funzione dedicata sia alla stampa degli indirizzi, sia le scelte dedicate
// agli effetti carta, oppure agli attacchi delle creatura (chi puo' applicare l'effetto/attaccare
// e chi non).
print_choice fun_print;
// stampa tutte le selezioni disponibili, senza fare distinzioni, sul campo di battaglia
// (comandi come attacca, pesca, stampa la memoria, etc.).
print_choice sel_print;
// -------------
// posto per le dichiarazioni di funzione struct...
// funzioni struct che inizializzano i puntatori a typedef struct, tutte inizializzate inizialmente
// a NULL...
// -------------
// inizializza i puntatori globali anche se sono stati gia' inizializzati (extern, oppure
// chiamata a delete_allmem che elimina tutta la memoria e resetta i puntatori a NULL)
// per prevenire eventuali memory leak, e segmentation fault.

// in questa definizione di funzione Mago* inizializza tutti i puntatori associati al mago
Mago *inizializza_mago(void)
{
    ptrmago = NULL;
    ptrmg_inizio = NULL;
    ptrmg_fine = NULL;
    for (int i = 0; i < 4; i++){
        mago[i] = NULL;
    }
    return 0;
}

// in questa definizione di funzione Mazzo* inizializza tutti i puntatori associati al mago
Mazzo *inizializza_mazzo(void)
{
    ptrmazzo = NULL;
    for (int i = 0; i < 4; i++)
    {
        ptrmz_inizio[i] = NULL;
        ptrmz_fine[i] = NULL;
        n_deck[i] = 0;
    }
    return 0;
}
// -------------
// funzioni struct che creano le liste dinamiche a code, e che chiedono in input nome e classe
// del mago da utilizzare nel gioco...
// -------------
// mette un nuovo nodo in coda per una struct Mago.
Mago *crea_Mago(Mago **nuovo_mago, Mago **inizio, Mago **fine, char **mago, int classe)
{
    (*nuovo_mago) = (Mago *)malloc(sizeof(**nuovo_mago));
    (*nuovo_mago)->nome = *mago;
    (*nuovo_mago)->classe_mago = classe;
    (*nuovo_mago)->mago_punti_vita = 20;
    (*nuovo_mago)->next_mago = NULL;
    if ((*inizio) == NULL)
    {
        (*inizio) = (*nuovo_mago);
        (*fine) = (*nuovo_mago);
    }
    else
    {
        (*fine)->next_mago = (*nuovo_mago);
        (*fine) = (*nuovo_mago);
    }
    return (*nuovo_mago); // la nuova coda della lista Mago
}

// mette un nuovo nodo in coda per una struct Mago che punta ad una struct Mazzo.
Mago *crea_Mazzo(Mago **nuovo_mazzo, Mazzo **inizio, Mazzo **fine, int classe_Carta, int PV_Carta)
{
    (*nuovo_mazzo)->mazzo = malloc(sizeof((**nuovo_mazzo)));
    (*nuovo_mazzo)->mazzo->tipo_carta = classe_Carta;
    (*nuovo_mazzo)->mazzo->carta_punti_vita = PV_Carta;
    (*nuovo_mazzo)->mazzo->next_carta = NULL;
    if ((*inizio) == NULL)
    {
        (*inizio) = (*nuovo_mazzo)->mazzo;
        (*fine) = (*nuovo_mazzo)->mazzo;
    }
    else
    {
        (*fine)->next_carta = (*nuovo_mazzo)->mazzo;
        (*fine) = (*nuovo_mazzo)->mazzo;
    }
    return (*nuovo_mazzo); // la nuova coda della lista Mazzo
}
// -------------
// Metodo coerente per mettere in ordine crescente le carte dinamicamente create,
// al posto di fare un quick sort tra le variabili dei nodi, si effettua un quick
// sort tra i puntatori next dei nodi.
Mazzo *sort_Mazzo(Mazzo **inizio_mazzo, int len_Deck)
{
    Mazzo **ptrscan = NULL;

    for (int i = 0; i <= len_Deck; i++)
    {
        ptrscan = inizio_mazzo;

        for (int j = 0; j < len_Deck - i - 1; j++)
        {

            Mazzo *p1 = *ptrscan;
            Mazzo *p2 = p1->next_carta;

            if (p1->carta_punti_vita > p2->carta_punti_vita)
            {
                Mazzo *temp = p2->next_carta;
                p2->next_carta = p1;
                p1->next_carta = temp;

                *ptrscan = p2;
            }
            ptrscan = &(*ptrscan)->next_carta;
        }
    }
    return *inizio_mazzo; // ritorna la testa della lista Mazzo
}

// Dealloca in coda (ultimo in coda) un nodo alla struct Mago (prima verranno eliminati
// le liste Mazzo associate alle liste di ogni struct Mago dinamicamente allocata).
Mago *elimina_nodoMago(Mago **del_Mago, Mago **inizio_mg, Mago **fine_mg){
    if(*inizio_mg == NULL)
        printf("\nNessun nodo nella lista Mazzo!\n"); // lista vuota (errore)
    else{
        Mago* ptrprec = NULL;
        Mago *ptrscan = *inizio_mg;

        if(*inizio_mg == *del_Mago){
            ptrprec = *inizio_mg;
            if((*inizio_mg)->next_mago != NULL)
                *inizio_mg = (*inizio_mg)->next_mago;
            else{
                *inizio_mg = NULL;
                *fine_mg = NULL;
            }
            free(ptrprec);
        }
        else{
            while(ptrscan->next_mago != NULL){
                if(ptrscan->next_mago == *del_Mago){
                    ptrprec = ptrscan->next_mago;
                    break;
                }
                else
                    ptrscan = ptrscan->next_mago;
            }
            if(ptrscan->next_mago->next_mago != NULL)
                ptrscan->next_mago = ptrscan->next_mago->next_mago;
            else
                *fine_mg = ptrscan;

            *del_Mago = NULL;

            free(ptrprec);
        }
    }
    return *inizio_mg; // ritorna la testa della lista Mago
}

// Dealloca un nodo qualunque di una struct Mazzo (non dovrebbe cancellare in coda ma, 
// per parsimonia, e' in grado anche di cancellare anche la coda e la testa di una struct Mazzo).
Mazzo *elimina_nodoMazzo(Mazzo **del_Mazzo, Mago *inizio_mg, Mazzo **inizio_mz, Mazzo **fine_mz){
    if(*inizio_mz == NULL)
        printf("\nNessun nodo nella lista Mazzo!\n"); // lista vuota (errore)
    else{
        Mazzo* ptrprec = NULL;
        Mago* ptrscan = inizio_mg;
        ptrscan->mazzo = *inizio_mz;

        if(*inizio_mz == *del_Mazzo){
            ptrprec = *inizio_mz;
            if((*inizio_mz)->next_carta != NULL)
                *inizio_mz = ptrscan->mazzo->next_carta;
            else{
                *inizio_mz = NULL;
                *fine_mz = NULL;
            }
            free(ptrprec);
        }
        else{
            while(ptrscan->mazzo->next_carta != NULL){
                if(ptrscan->mazzo->next_carta == *del_Mazzo){
                    ptrprec = ptrscan->mazzo->next_carta;
                    break;
                }
                else
                    ptrscan->mazzo = ptrscan->mazzo->next_carta;
            }

            if(ptrscan->mazzo->next_carta->next_carta != NULL)
                ptrscan->mazzo->next_carta = ptrscan->mazzo->next_carta->next_carta;
            else
                *fine_mz = ptrscan->mazzo;

            *del_Mazzo = NULL;

            free(ptrprec);
        }
    }
    return *inizio_mz; // ritorna la testa della lista Mazzo
}
// -------------
// Funzione struct che scannerizza la struct Mago.
Mago *scan_Mago(Mago *inizio_mg, Mago *nodo_Mago, Mago **presta_Mago)
{
    if (inizio_mg == NULL)
        printf("\nNessun nodo nella lista Mago!\n"); // lista vuota (errore)
    else{
        Mago *scan_mg = inizio_mg;
        do{
            if(scan_mg == nodo_Mago && nodo_Mago != NULL)
                *presta_Mago = nodo_Mago;
            scan_mg = scan_mg->next_mago;
        }while(scan_mg != NULL);
    }
    return inizio_mg; // la testa della lista Mago
}

Mago *scan_Mazzo(Mazzo *scan_mz[], Mano *Mano[], Campo *Campo[], Mago *inizio_mg, Mazzo *inizio_mz)
{
    if (inizio_mz == NULL)
        printf("\nNessun nodo nella lista Mazzo!\n");
    else{
        Mago *ptrscan_mz = inizio_mg;
        ptrscan_mz->mazzo = inizio_mz;

        Mazzo *temp = NULL;
        int a = 0;
        do
        { 
            temp = NULL;
            
            for(int i = 0; i < 6; i++){
                if(ptrscan_mz->mazzo == Mano[i] && Mano[i] != NULL)
                    temp = ptrscan_mz->mazzo;
             
                if(ptrscan_mz->mazzo == Campo[i] && i < 4 && Campo[i] != NULL)
                    temp = ptrscan_mz->mazzo;
            }

            if(temp == NULL){
                scan_mz[a] = ptrscan_mz->mazzo;
                a++;
            }
            ptrscan_mz->mazzo = ptrscan_mz->mazzo->next_carta;
        } while (ptrscan_mz->mazzo != NULL);
    }
    return inizio_mg; // la testa della lista Mazzo
}

Mago *scan_nodoMazzo(Mago *inizio_mg, Mazzo *inizio_mz, Mazzo *nodo_Mazzo, Mazzo **presta_Mazzo){
    if (inizio_mz == NULL)
        printf("\nNessun nodo nella lista Mazzo!\n");
    else{
        Mago *ptrscan_mz = inizio_mg;
        ptrscan_mz->mazzo = inizio_mz;
        do{
            if(ptrscan_mz->mazzo == nodo_Mazzo)
                *presta_Mazzo = ptrscan_mz->mazzo;

            ptrscan_mz->mazzo = ptrscan_mz->mazzo->next_carta;
        }while(ptrscan_mz->mazzo != NULL);  
    }
    return inizio_mg;
}

// -------------
// Corpi di funzioni struct che eliminano tutta la memoria di una singola struct,
// oppure un nodo singolarmente.

// Dealloca un elemento in coda alla struct Mago
Mago *elimina_Mago(Mago **inizio_mago, Mago **fine_mago)
{
    if(*inizio_mago == NULL){
        printf("\nNessun nodo nella lista Mago!\n");
    }
    else {
        Mago* ptrprec = NULL;
        Mago* ptrscan = *inizio_mago;
        
        if((*inizio_mago)->next_mago == NULL){
            free(*inizio_mago);
            *inizio_mago = NULL;
            *fine_mago = NULL;
        }
        else{
            do{
                if((ptrscan->next_mago) == *fine_mago){
                    ptrprec = ptrscan;
                    break;
                }
                else
                    ptrscan = ptrscan->next_mago;
            }while((ptrscan->next_mago) != NULL);
            
            free(ptrprec->next_mago->nome);
            ptrprec->next_mago->nome = NULL;

            free(ptrprec->next_mago);
            ptrprec->next_mago = NULL;
            *fine_mago = ptrprec;
        }
    }
    return *inizio_mago;}

Mazzo *elimina_Mazzo(Mago **inizio_mago, Mazzo **inizio_mazzo, Mazzo **fine_mazzo)
{
    if (*inizio_mazzo == NULL)
    {
        printf("\nNessun nodo nella lista Mazzo!\n");
    }
    else
    {
        Mago *ptrprec = *inizio_mago;
        ptrprec->mazzo = NULL;
        Mago *ptrscan = *inizio_mago;
        ptrscan->mazzo = *inizio_mazzo;

        if ((*inizio_mazzo)->next_carta == NULL)
        {
            free(*inizio_mazzo);
            *inizio_mazzo = NULL;
            *fine_mazzo = NULL;
        }
        else
        {
            do
            {
                if (ptrscan->mazzo->next_carta == *fine_mazzo)
                {
                    ptrprec->mazzo = ptrscan->mazzo;
                    break;
                }
                else
                    ptrscan->mazzo = ptrscan->mazzo->next_carta;
            } while ((ptrscan->mazzo->next_carta) != NULL);
            free(ptrprec->mazzo->next_carta);
            ptrprec->mazzo->next_carta = NULL;
            *fine_mazzo = ptrprec->mazzo;
        }
    }
    return *inizio_mazzo; // testa del Mazzo
}
// -------------
// posto per le funzioni non static...

// Funzione che chiama funzioni dedicate sia alla creazione dei maghi (Inizializza Lista Mago e Mazzo
// di ogni giocatore)
void imposta_gioco(int *passa_Menu, int scelta_input)
{
    switch (scelta_input)
    {
    case 1:
        if (impostato == true)
            dealloc_mem(); // in caso la memoria e' stata gia' allocata, altrimenti torna al menu.
        else
            allocate_mem(); // in caso la memoria non sia stata ancora allocata, crea i giocatori, i mazzi e genera le carte casuali (Memoria dinamica).
        *passa_Menu = 0; // ripeti il loop in main.c una volta che raggiunge la fine del blocco di questa funzione.
        break;
    case 2:
        if (impostato == true)
            combatti(&select_menu); // se la memoria e' allocata ed il giocatore decide di giocare, inizia la partita.
        else
        {
            clear_ansi();
            printf("\nImpostare il gioco prima di iniziare la partita!\n"); // ritorna un errore al termine della chiamata di questa funzione.
        }
        break;
    case 3:
        mem_menu(); // Menu interattivo dedicato alla stampa in output di ogni struct e puntatori array.
        break;
    case 4:
        guide_menu(); // Menu interattivo che spiega il funzionamento del gioco.
        break;
    default:
        printf("\nErrore: Nessun input riconosciuto, per favore riprovare.\n");
    }
    return;
}

void combatti(int *passa_Menu)
{
    // funzione per iniziare la fase ludica del gioco
    // tutte le variabili assegnate in imposta_gioco, avranno significato
    // ed effetto nella funzione combatti
    // -----------
    int *prendi_selTar[2], vincitore = 0, sel_cartaMago = -1;
    int tar_cartaMago = -1, fase_input = 1, layer_Trovato = 0;
    int scelta_Selezione = 0;
    char *scelta_gioco = (char *) malloc(LEN);

    prendi_selTar[0] = &sel_cartaMago, prendi_selTar[1] = &tar_cartaMago;

    fun_print = _none_print_choice;
    mem_title = _title_only_mage_deck_visible;

    if(event == _first_turn){
        // stampa in output la "presentazione" dei giocatori e contro i loro avversari
        // di questa partita (verra' mostrata una volta sola dato che all'interno del ciclo
        // do while di questa funzione non verra' mai assegnato event il valore del membro _first_turn).
        event = _print_beginning;
        print_gameout(event); // chiama la funzione che stampa la schermata di presentazione dei maghi in output.
        
        passa(&vincitore); // gestisce il primo turno scegliendo il primo mago ed il primo avversario di questa partita.
        event = _none_event; // assegna il valore del membro _non_event dopo aver stampato la schermata in output.
    }

    // ciclo do while che fara' iniziare la partita e stampare l'interfaccia del campo di battaglia
    // e che chiedera' in input al giocatore di effettuare un azione, chiamando determinate funzioni,
    // che gestiscono numerose istruzioni.
    do{        
        clear_ansi(); // "pulisce" l'output del terminale ad ogni iterazione del ciclo, non ad ogni cambio del turno.

        // stampa dell'intero campo di battaglia del gioco.
        printf("\n");
        // avversario del turno corrente.
        print_fieldtable(1, sel_cartaMago, tar_cartaMago);
        print_validchoices(avversario, prendi_selTar, &layer_Trovato, _side_mago, 6);
        printf("\n\n");
        // mano avversaria.
        print_fieldtable(2, sel_cartaMago, tar_cartaMago);
        print_validchoices(avversario+1, prendi_selTar, &layer_Trovato, _side_mano, 5);
        printf("\n-----------------------------------------------------------------------------------------------------------------------------------\n\t\t\t\t");
        // campo avversario.
        print_fieldtable(3, sel_cartaMago, tar_cartaMago);
        print_validchoices(avversario+1, prendi_selTar, &layer_Trovato, _side_campo, 4);
        printf(BOL"\n\n\t\t\t\t\t\tTurno di "); // output di chi appartiene il turno corrente.
        print_playername(curr_giocatore, _none_side, 0);
        printf("\n\n\t\t\t\t"CLR);
        // campo del mago.
        print_fieldtable(4, sel_cartaMago, tar_cartaMago);
        print_validchoices(giocatore+1, prendi_selTar, &layer_Trovato, _side_campo, 3);
        printf("\n-----------------------------------------------------------------------------------------------------------------------------------\n");
        // mano del mago.
        print_fieldtable(5, sel_cartaMago, tar_cartaMago);
        print_validchoices(giocatore+1, prendi_selTar, &layer_Trovato, _side_mano, 2);
        printf("\n\n");
        // mago che sta giocando nel turno corrente.
        print_fieldtable(6, sel_cartaMago, tar_cartaMago);
        print_validchoices(giocatore, prendi_selTar, &layer_Trovato, _side_mago, 1);
        printf("\n");

        // funzione che andra' a richiedere una serie di input, richiamando questa funzione
        // un determinato numero di volte durante il turno di un giocatore.
        game_input(scelta_gioco, prendi_selTar, &fase_input, &layer_Trovato, &scelta_Selezione);
        // in base all'input che il giocatore ha inserito, se ha attivato l'effetto di una qualsiasi
        // carta nella sua mano, oppure attacca direttamente con una creatura, allore da choiceof_play
        // chiamera' gioca_carta e attacca e gestira' gli indirizzi di memoria opportuni richiesti
        // dal giocatore di gestire e deallocare eventualmente le condizioni in queste funzioni void
        // siano vere.
        choiceof_play(prendi_selTar, &vincitore);
    }while(vincitore <= 0 && *scelta_gioco != 'q' && *scelta_gioco != 'Q');

    free(scelta_gioco);
    scelta_gioco = NULL;

    if(event == _print_ending){
        print_gameout(vincitore);
        event = _none_event;
    }

    clear_ansi(); // pulisce l'output prima di tornare al menu principale.

    mem_title = _title_visible;
    *passa_Menu = 0;
    return;
}

void termina_gioco(int *passa_Menu)
{
    // funzione per "chiudere il gioco", o meglio dire tornare alla funzione main nel file
    // main.c
    // ------------
    char *input_exit = (char *)malloc(LEN);
    int err_exit = 0;

    clear_ansi();
    do
    {
        printf("\nUscire da Tragic?\n");
        printf("\nPremi y per uscire oppure n per tornare indietro.\n");
        fgets(input_exit, LEN, stdin);
        clear_arrbuffer(input_exit, &err_exit, 2);

        if (*input_exit == 'y' || *input_exit == 'Y')
        {
            clear_ansi();
            printf("\nChiusura del gioco in corso...\n");
            fflush(stdout);
            if (impostato == true)
            {
                printf(BOL"\nLa memoria allocata dinamicamente e' stata deallocata con successo.\n"CLR);
                delete_allmem();

                printf("\n\nPremi un tasto qualsiasi per continuare...\n");
                getchar();
                clear_ansi();
            }
            printf("\nGrazie per aver giocato!\n");
            *passa_Menu = 1;
            break;
        }
        else if(*input_exit == 'n' || *input_exit == 'N')
        {
            clear_ansi();
            *passa_Menu = 0;
            continue;
        }
        else if(err_exit == 1){
            clear_ansi();
            printf("\nErrore: input non riconosciuto, per favore riprovare.\n\n");
            err_exit = 0;            
            continue;
        }
    } while (*input_exit != 'n' && *input_exit != 'N');
    return;
}

// -------------
// body delle funzioni static void...
static void init_variable(void){
    // inizializzazione ai primi membri (di valore 0) delle definizioni globali di alcune
    // struct globali, il motivo per cui vengono inizializzate, e' perche' nel caso il giocatore
    // decide di iniziare nuovamente una partita, pertanto queste definizioni avranno un altro
    // valore in quel determinato momento.
    mode = _none_mode;
    giocatore = _none_memory;
    avversario = _none_memory;
    curr_giocatore = _none_memory;
    curr_avversario = _none_memory;
    fun_print = _none_print_choice;
    event = _none_event;
    action[0] = _none_event;
    action[1] = _none_event;
    action[2] = _none_event;
    type_target = _none_event;
    return;
}
static void init_handfield(int sel_Pulisci, int caso_Pulisci){
    int len_arr = 0;

    if(caso_Pulisci == _side_mano)
    len_arr = 6;
    else if(caso_Pulisci == _side_campo)
    len_arr = 4;

    switch(sel_Pulisci){
        case 1: case 2: case 3:
        for(int i = 0; i < len_arr; i++){
            if(caso_Pulisci == _side_mano){
                arr_Mano1[i] = NULL;
            }
            else if(caso_Pulisci == _side_campo)
                arr_Campo1[i] = NULL;
        }
        n_mano[0] = 0;
        break;
        case 4: case 5: case 6:
        for(int i = 0; i < len_arr; i++){
            if(caso_Pulisci == _side_mano){
                arr_ManoAvv[i] = NULL;
            }
            else if(caso_Pulisci == _side_campo)
            arr_CampoAvv[i] = NULL;
        }
        n_mano[1] = 0;
        break;
        case 7: case 8: case 9:
        for(int i = 0; i < len_arr; i++){
            if(caso_Pulisci == _side_mano){
                arr_Mano2[i] = NULL;
            }
            else if(caso_Pulisci == _side_campo)
            arr_Campo2[i] = NULL;
        }
        n_mano[2] = 0;
        break;
        case 10: case 11: case 12:
        for(int i = 0; i < len_arr; i++){
            if(caso_Pulisci == _side_mano){
                arr_Mano2Avv[i] = NULL;
            }
            else if(caso_Pulisci == _side_campo)
            arr_Campo2Avv[i] = NULL;
        }
        n_mano[3] = 0;
        break;
        default:
        printf("\nErrore: nessun valore corrisponde ad un array Mano oppure Campo da pulire!\n");
    }
    return;
}

static void allocate_mem(void)
{
    srand((unsigned)time(&t));
    // funzione per pianificare il tipo di partita tramite menu di selezione
    // fondamentale, altrimenti non è possibile giocare
    // ------------
    // chiamata di funzione a struct che mette a NULL tutti i puntatori dichiarati
    // globalmente in gamelib.c
    init_variable();
    inizializza_mago();
    inizializza_mazzo();
    for(int i = 0; i < 12; i++){
        init_handfield(i+1, _side_mano);
        init_handfield(i+1, _side_campo);
    }

    // chiamata delle funzioni void che chiedono in input
    // il nome, la classe ed il numero di carte da mettere
    // nel mazzo per ogni giocatore.
    gamemode_menu();
    if (mode == _1versus1 || mode == _2versus2)
    {
        giocatore = 1;
        for (int i = 0; i < n_mago[0] + n_mago[1]; i++)
        {
            if(i == 0 || i == 2)
            printf("\nCreiamo il prossimo Mago.\n");
            else if(i == 1 || i == 3)
            printf("\nCreiamo il prossimo Mago Avversario.\n");            
            clear_ansi();
            magedeck_menu(i + 1, &n_deck[i]);
            sort_Mazzo(&ptrmz_inizio[i], n_deck[i] + 5);

            create_deckand_hand(2, i + 1, 0, 0);
            sel_handsorted(giocatore + 1, 2);
            giocatore = giocatore + 3;
        }
        giocatore = 0;
        impostato = true;
        event = _first_turn;
        clear_ansi();
        printf("\nImpostazioni salvate con successo.\n");
        printf("\nRitorno al menu principale.\n");
        clear_ansi();
        set_delayclock(1, 2);
    }
    return;
}

static void dealloc_mem(void)
{
    char *input_conferma = (char *)malloc(LEN); // inizializza stringa
    int torna_errore = 0;

    clear_ansi();
    fflush(stdout);
    do
    {
        printf("\nLa memoria e' stata gia' allocata.\n");
        printf("Eliminare la memoria?\n");
        printf("\nPremi y per confermare, n per declinare e tornare al Menu.\n");
        fflush(stdout);
        fgets(input_conferma, LEN, stdin);
        clear_arrbuffer(input_conferma, &torna_errore, 2);

        if (*input_conferma == 'y')
        {
            clear_ansi();
            delete_allmem(); 
            // se l'input e' y allora dealloca tutta la memoria dinamica,
            // non fa distinzione se una partita e' cominciata, guarda solo la memoria
            // se e' allocata oppure no.

            printf(BOL"\n\nMemoria dinamica deallocata con successo.\n"CLR);
            impostato = false;
            printf("\nPremi un tasto qualsiasi per continuare...\n");
            getchar();

            allocate_mem();
            clear_ansi();
            break; // interrompi il ciclo, perche' l'utente ha eseguito l'operazione di deallocamento di tutta la memoria dinamica.
        }
        else if (*input_conferma == 'n')
        {
            clear_ansi();
            printf("\nRichiesta declinata con successo.\n");
            // memoria non verra' deallocata su richiesta dell'utente, finito lo scope, torna in main.c (funzione main).
        }
        else if(torna_errore == 1){
            clear_ansi();
            printf("\nErrore: input non riconosciuto, per favore riprovare.\n\n");
            torna_errore = 0;
        }

    } while (*input_conferma != 'n'); // finche' l'input non sara' 'n' continua l'iterazione.

    free(input_conferma); // libera la stringa dell'input in questa funzione void.
    return;
}
// ------------
// Scansiona la lista di una qualsiasi struct Mago.
static void scan_mage(int passa_Mago){
    switch(passa_Mago){
        case -1:
        if(ptrmg_inizio == NULL) // se la lista e' vuota chiama la funzione e stampa l'errore.
            scan_Mago(ptrmg_inizio, NULL, NULL);
        break;
        case 0:
        ptrmago = NULL;
        break;
        case 1: case 2: case 3:
        if(ptrmg_inizio != NULL) // se la lista non e' vuota chiama la funzione e scansiona la lista.
            scan_Mago(ptrmg_inizio, mago[0], &ptrmago);
        break;
        case 4: case 5: case 6:
        if(ptrmg_inizio != NULL) // se la lista non e' vuota chiama la funzione e scansiona la lista.
            scan_Mago(ptrmg_inizio, mago[2], &ptrmago);
        break;
        case 7: case 8: case 9:
        if(ptrmg_inizio != NULL) // se la lista non e' vuota chiama la funzione e scansiona la lista.
            scan_Mago(ptrmg_inizio, mago[1], &ptrmago);
        break;
        case 10: case 11: case 12:
        if(ptrmg_inizio != NULL) // se la lista non e' vuota chiama la funzione e scansiona la lista.
            scan_Mago(ptrmg_inizio, mago[3], &ptrmago);
        break;
        default:
        printf("Errore: nessun valore corrisponde ad un Mago da scannerizzare!\n");
    }
    return;
}

// Scansiona la lista di una qualsiasi struct Mazzo.
static void scan_deck(int passa_Mazzo, Mazzo *passa_Arr[], Mazzo *passa_Nodo, int *passa_Deck, int tipo_Mazzo){
    switch(passa_Mazzo){
        case -4:
        if(ptrmz_inizio[3] == NULL && passa_Arr == NULL && tipo_Mazzo == _side_mazzo) // Chiamata di funzione e stampa errore.
            scan_Mazzo(passa_Arr, arr_Mano2Avv, arr_Campo2Avv, mago[3], ptrmz_inizio[3]);
        break;
        case -3:
        if(ptrmz_inizio[1] == NULL && passa_Arr == NULL && tipo_Mazzo == _side_mazzo) // Chiamata di funzione e stampa errore.
            scan_Mazzo(passa_Arr, arr_ManoAvv, arr_CampoAvv, mago[1], ptrmz_inizio[1]);
        break;
        case -2:
        if(ptrmz_inizio[2] == NULL && passa_Arr == NULL && tipo_Mazzo == _side_mazzo) // Chiamata di funzione e stampa errore.
            scan_Mazzo(passa_Arr, arr_Mano2, arr_Campo2, mago[2], ptrmz_inizio[2]);
        break;
        case -1:
        if(ptrmz_inizio[0] == NULL && passa_Arr == NULL && tipo_Mazzo == _side_mazzo) // Chiamata di funzione e stampa errore.
            scan_Mazzo(passa_Arr, arr_Mano1, arr_Campo1, mago[0], ptrmz_inizio[0]);
        break;
        case 0:
        ptrmazzo = NULL; // assegna NULL a ptrmazzo, se l'utente passa il valore 0 (_none_memory).
        break;
        case 1: case 2: case 3:
        if(passa_Arr != NULL && tipo_Mazzo == _side_mazzo) // prendi in prestito tutti gli elementi di questa struct Mazzo.
            scan_Mazzo(passa_Arr, arr_Mano1, arr_Campo1, mago[0], ptrmz_inizio[0]);
        else if(passa_Nodo != NULL && tipo_Mazzo == _side_nodoMazzo) // prendi in prestito un solo elemento di questa struct Mazzo.
            scan_nodoMazzo(ptrmg_inizio, ptrmz_inizio[0], passa_Nodo, &ptrmazzo);

        if(passa_Deck != NULL) // se l'utente lo desidera puo' recuperare il valore associato all'elemento a n_deck.
            *passa_Deck = n_deck[0];
        break;
        case 4: case 5: case 6:
        if(passa_Arr != NULL && tipo_Mazzo == _side_mazzo) // prendi in prestito tutti gli elementi di questa struct Mazzo.
            scan_Mazzo(passa_Arr, arr_Mano2, arr_Campo2, mago[2], ptrmz_inizio[2]);
        else if(passa_Nodo != NULL && tipo_Mazzo == _side_nodoMazzo) // prendi in prestito un solo elemento di questa struct Mazzo.
            scan_nodoMazzo(ptrmg_inizio, ptrmz_inizio[2], passa_Nodo, &ptrmazzo);

        if(passa_Deck != NULL) // se l'utente lo desidera puo' recuperare il valore associato all'elemento a n_deck.
            *passa_Deck = n_deck[2];
        break;
        case 7: case 8: case 9:
        if(passa_Arr != NULL && tipo_Mazzo == _side_mazzo) // prendi in prestito tutti gli elementi di questa struct Mazzo.
            scan_Mazzo(passa_Arr, arr_ManoAvv, arr_CampoAvv, mago[1], ptrmz_inizio[1]);
        else if(passa_Nodo != NULL && tipo_Mazzo == _side_nodoMazzo) // prendi in prestito un solo elemento di questa struct Mazzo.
            scan_nodoMazzo(ptrmg_inizio, ptrmz_inizio[1], passa_Nodo, &ptrmazzo);

        if(passa_Deck != NULL)
            *passa_Deck = n_deck[1]; // se l'utente lo desidera puo' recuperare il valore associato all'elemento a n_deck.
        break;
        case 10: case 11: case 12:
        if(passa_Arr != NULL && tipo_Mazzo == _side_mazzo) // prendi in prestito tutti gli elementi di questa struct Mazzo.
            scan_Mazzo(passa_Arr, arr_Mano2Avv, arr_Campo2Avv, mago[3], ptrmz_inizio[3]);
        else if(passa_Nodo != NULL && tipo_Mazzo == _side_nodoMazzo) // prendi in prestito un solo elemento di questa struct Mazzo.
            scan_nodoMazzo(ptrmg_inizio, ptrmz_inizio[3], passa_Nodo, &ptrmazzo);

        if(passa_Deck != NULL)
            *passa_Deck = n_deck[3]; // se l'utente lo desidera puo' recuperare il valore associato all'elemento a n_deck.
        break;
        default:
        printf("Errore: nessun valore corrisponde ad un Mazzo da scannerizzare!\n");
    }
    return;
}

// Funzione static void dedicata alla stampa della memoria in output, indipendentemente
// al valore del parametro interno tipo_Memoria, stampa una struct Mago oppure Mazzo,
// stampa quindi tutti i dati disponibili per quella determinata lista.
static void print_mem(int tipo_Memoria, int caso_Memoria, int ferma_Output){
    switch (tipo_Memoria){
        case 1:
            if(caso_Memoria == _side_mago)
                print_mage(_Mago, 0); // il parametro 0, significa non evidenziare l'output (cambiare il formato in Bold).
            break;
        case 2:
            if(caso_Memoria == _side_mano)
                print_magehand(arr_Mano1, _ManoCampo, -1); // il parametro -1, significa non evidenziare l'output (cambiare il formato in Bold).
            else if(caso_Memoria == _side_campo)
                print_gamefield(arr_Campo1, _ManoCampo, -1);
            break;
        case 3:
            print_deck(_Mazzo, n_deck[0], &ferma_Output);
            break;
        case 4:
            if(caso_Memoria == _side_mago)
                print_mage(_Mago2, 0);
            break;
        case 5:
            if(caso_Memoria == _side_mano)
                print_magehand(arr_Mano2, _ManoCampo2, -1);
            else if(caso_Memoria == _side_campo)
                print_gamefield(arr_Campo2, _ManoCampo2, -1);
            break;
        case 6:
            print_deck(_Mazzo2, n_deck[2], &ferma_Output);
            break;
        case 7:
            if(caso_Memoria == _side_mago)
                print_mage(_MagoAvv, 0);
            break;
        case 8:
            if(caso_Memoria == _side_mano)
                print_magehand(arr_ManoAvv, _ManoCampoAvv, -1);
            else if(caso_Memoria == _side_campo)
                print_gamefield(arr_CampoAvv, _ManoCampoAvv, -1);
            break;
        case 9:
            print_deck(_MazzoAvv, n_deck[1], &ferma_Output);
            break;
        case 10:
            if(caso_Memoria == _side_mago)
                print_mage(_Mago2Avv, 0);
            break;
        case 11:
            if(caso_Memoria == _side_mano)
                print_magehand(arr_Mano2Avv, _ManoCampo2Avv, -1);
            else if(caso_Memoria == _side_campo)
                print_gamefield(arr_Campo2Avv, _ManoCampo2Avv, -1);
            break;
        case 12:
            print_deck(_Mazzo2Avv, n_deck[3], &ferma_Output);
            break;
        default:
            printf("\nErrore: nessun valore corrisponde ad una Struct da stampare!\n");
            ferma_Output = 0;
    }
    
    scan_mage(_none_memory); // assegnazione a NULL di *ptrmago.
    
    // il valore di ferma_Output e' 1 solamente se la chiamata avviene da mem_menu, oppure
    // indipendentemente se si vuole stampare la struct Mazzo associata a un determinato Mago.
    if(ferma_Output == 1){
        printf("\n\nPremi un tasto qualsiasi per tornare al Menu...\n");
        getchar();
    }
    return;
}
// ------------
static void highlight_mem(int tipo_Arr, int caso_Evidenzia, int ev_CartaMago){
    switch(tipo_Arr){
        case 1:
        if(caso_Evidenzia == _side_mago)
            print_mage(_Mago, ev_CartaMago);
        break;
        case 2: case 3:
        if(caso_Evidenzia == _side_mano)
            print_magehand(arr_Mano1, _ManoCampo, ev_CartaMago);
        else if(caso_Evidenzia == _side_campo)
            print_gamefield(arr_Campo1, _ManoCampo, ev_CartaMago);
        break;
        case 4:
        if(caso_Evidenzia == _side_mago)
            print_mage(_Mago2, ev_CartaMago);
        break;
        case 5: case 6:
        if(caso_Evidenzia == _side_mano)
            print_magehand(arr_Mano2, _ManoCampo2, ev_CartaMago);
        else if(caso_Evidenzia == _side_campo)
            print_gamefield(arr_Campo2, _ManoCampo2, ev_CartaMago);
        break;
        case 7:
        if(caso_Evidenzia == _side_mago)
            print_mage(_MagoAvv, ev_CartaMago);
        break;
        case 8: case 9:
        if(caso_Evidenzia == _side_mano)
            print_magehand(arr_ManoAvv, _ManoCampoAvv, ev_CartaMago);
        else if(caso_Evidenzia == _side_campo)
            print_gamefield(arr_CampoAvv, _ManoCampoAvv, ev_CartaMago);
        break;
        case 10:
        if(caso_Evidenzia == _side_mago)
            print_mage(_Mago2Avv, ev_CartaMago);
        break;
        case 11: case 12:
        if(caso_Evidenzia == _side_mano)
            print_magehand(arr_Mano2Avv, _ManoCampo2Avv, ev_CartaMago);
        else if(caso_Evidenzia == _side_campo)
            print_gamefield(arr_Campo2Avv, _ManoCampo2Avv, ev_CartaMago);
        break;
        default:
        printf("\nErrore: nessun valore corrisponde ad un nodo Struct da evidenziare!\n");
    }
    return;
}
// ------------
// Funzione dedicata ad individuare se il Mazzo contiene ancora carte da giocare oppure no,
// il controllo avviene ad ogni turno in choiceof_play.
static void thereis_deck(int seleziona_Giocatore, int *passa_Vincitore){
    int presta_nMazzo = 1; // inizializzazione di presta_nMazzo
    Mago *presta_Mago = NULL; // inizializzazione di *presta_Mago

    switch(seleziona_Giocatore){
        case 1: case 2: case 3:
        if(mago[0] != NULL)
            presta_nMazzo = n_deck[0]; // presta_nMazzo ha il valore intero del deck associato da controllare.
        break;
        case 4: case 5: case 6:
        if(mago[2] != NULL)
            presta_nMazzo = n_deck[2]; // presta_nMazzo ha il valore intero del deck associato da controllare.
        break;
        case 7: case 8: case 9:
        if(mago[1] != NULL)
            presta_nMazzo = n_deck[1]; // presta_nMazzo ha il valore intero del deck associato da controllare.
        break;
        case 10: case 11: case 12:
        if(mago[3] != NULL)
            presta_nMazzo = n_deck[3]; // presta_nMazzo ha il valore intero del deck associato da controllare.
        break;
        default:
        printf("\nErrore: nessun valore corrisponde ad una Mano, Campo oppure deck da assegnare!\n");
    }

    // il Mago non ha piu' carte da giocare, libera la memoria del Mago in questione e inizializza a NULL
    // gli array associati a questo giocatore.
    if(presta_nMazzo <= 0){
        int temp_Vita = 0; // inizializzazione di una variabile temporanea
        scan_mage(seleziona_Giocatore); // aggiorna *ptrmago.
        presta_Mago = ptrmago;
        temp_Vita = presta_Mago->mago_punti_vita;  // prende in prestito il valore della vita del Mago da deallocare prima di venire deallocato.

        empty_magehand(seleziona_Giocatore+1, _side_mano, -1); // svuota l'array mano del mago corrispondente
        empty_magehand(seleziona_Giocatore+1, _side_campo, -1); // svuota l'array campo del mago corrispondente

        delete_memdata(NULL, NULL, seleziona_Giocatore+1, _side_mazzo); // dealloca tutta la lista Mazzo del mago corrispondente
        empty_magedeck(seleziona_Giocatore+2); // svuota l'array mazzo del mago corrispondente

        delete_memdata(&ptrmago, NULL, seleziona_Giocatore, _side_nodoMago); // dealloca il nodo alla lista Mago corrispondente al mago da eliminare.
        empty_magedeck(seleziona_Giocatore); // svuota l'array di puntatori mago all'elemento che corrisponde al mago deallocato.

        add_cardtohand(seleziona_Giocatore, 1, 1); // decrementa n_mago[0] oppure n_mago[1] di 1 (il terzo parametro determina incremento oppure decremento della variabile globale).
        
        scan_mage(_none_memory); // resetta il puntatore *ptrmago a NULL
        scan_deck(_none_memory, NULL, NULL, NULL, _none_side); // resetta il puntatore *ptrmazzo a NULL

        // la condizione non guarda all'idea della modalita' 2 contro 2 come avviene con la definizione all'enumeratore mode,
        // controlla soltanto se nell'array di puntatori mago, gli elementi associati ad un "mago amico"
        // (mago[0] e mago[2] giocatori amici, mago[1] e mago[3] giocatori amici avversari) equivalgono a NULL.
        if(mago[0] == NULL && mago[2] == NULL){
            if(mago[1] != NULL && mago[3] != NULL){ // ci sono entrambi i giocatori avversari da comparare con il mago appena deallocato.
                if(temp_Vita > mago[1]->mago_punti_vita && temp_Vita > mago[3]->mago_punti_vita)
                    *passa_Vincitore = 1; // il mago deallocato aveva piu' vita dei maghi avversari, vince la squadra principale.
                else if(temp_Vita < mago[1]->mago_punti_vita && temp_Vita < mago[3]->mago_punti_vita)
                    *passa_Vincitore = 2; // il mago deallocato aveva meno vita dei maghi avversari, vince la squadra avversaria.
                else if(temp_Vita == mago[1]->mago_punti_vita && temp_Vita == mago[3]->mago_punti_vita)
                    *passa_Vincitore = 3; // il mago deallocato aveva pari vita dei maghi avversari, finisce in pareggio.
            }
            else if(mago[1] != NULL && mago[3] == NULL){ // esiste solo un giocatore avversario (mago[1]) da comparare con il mago appena deallocato.
                if(temp_Vita > mago[1]->mago_punti_vita) // vince il mago della squadra principale.
                    *passa_Vincitore = 1; 
                else if(temp_Vita < mago[1]->mago_punti_vita) // vince il mago della squadra avversaria.
                    *passa_Vincitore = 2;
                else if(temp_Vita == mago[1]->mago_punti_vita) // finisce in pareggio.
                    *passa_Vincitore = 3;
            }
            else if(mago[1] == NULL && mago[3] != NULL){ // esiste solo un giocatore avversario (mago[2]) da comparare con il mago appena deallocato.
                if(temp_Vita > mago[3]->mago_punti_vita)
                    *passa_Vincitore = 1;
                else if(temp_Vita < mago[3]->mago_punti_vita)
                    *passa_Vincitore = 2;
                else if(temp_Vita == mago[3]->mago_punti_vita)
                    *passa_Vincitore = 3;
            }
        } // altrimenti se il giocatore appena eliminato era un avversario, determina se il mago in questione ha piu' o meno vita degli altri/o maghi/o.
        else if(mago[1] == NULL && mago[3] == NULL){
            if(mago[0] != NULL && mago[2] != NULL){ // tutti i maghi principale esistono in partita, entra in questo blocco e compara i loro valori con quello di riferimento (temp_vita).
                if(temp_Vita > mago[0]->mago_punti_vita && temp_Vita > mago[2]->mago_punti_vita) // vincono gli avversari.
                    *passa_Vincitore = 2;
                else if(temp_Vita < mago[0]->mago_punti_vita && temp_Vita < mago[2]->mago_punti_vita) // vincono i giocatori principali.
                    *passa_Vincitore = 1;
                else if(temp_Vita == mago[0]->mago_punti_vita && temp_Vita == mago[2]->mago_punti_vita) // pareggio.
                    *passa_Vincitore = 3;
            }
            else if(mago[0] != NULL && mago[2] == NULL){ // esiste solo il mago principale mago[0] con cui effettuare il paragone.
                if(temp_Vita > mago[0]->mago_punti_vita)
                    *passa_Vincitore = 2;
                else if(temp_Vita < mago[0]->mago_punti_vita)
                    *passa_Vincitore = 1;
                else if(temp_Vita == mago[0]->mago_punti_vita)
                    *passa_Vincitore = 3;
            }
            else if(mago[0] == NULL && mago[2] != NULL){ // esiste solo il mago[2] con cui effettuare il paragone.
                if(temp_Vita > mago[2]->mago_punti_vita)
                    *passa_Vincitore = 2;
                else if(temp_Vita < mago[2]->mago_punti_vita)
                    *passa_Vincitore = 1;
                else if(temp_Vita == mago[2]->mago_punti_vita)
                    *passa_Vincitore = 3;
            }
        }
         
        // se il giocatore appena deallocato, apparteneva ad una squadra ma non ha determinato la fine della partita,
        // ovvero che in squadra esiste ancora un altro mago che sta giocando, stampa in output che e' stato eliminato.
        if(*passa_Vincitore <= 0)
            event = _dead_by_hand; // aggiorna la definizio event del tipo enumeratore Evento_Gioco.
    }
    return;
}
// ------------
// Funzione static void incentrata a prendere "in prestito" l'indirizzo esadecimale degli eventuali dei maghi, oppure degli
// array del mazzo e della mano, dei maghi corrispondenti agli elementi di Memoria_Mago.
static void get_arraddress(Mago **prendi_Mago, Mazzo **prendi_Carta, int passa_Giocatore, int passa_Lato, int passa_Pos){
    int len_Arr = 0;

    if(passa_Lato == _side_mano)
        len_Arr = 6;
    else if(passa_Lato == _side_campo)
        len_Arr = 4;

    Mazzo **presta_Arr = NULL; // puntatore doppio temporaneo alla struct Mazzo, inizializzato a NULL.

    switch(passa_Giocatore){
        case 1: case 4: case 7: case 10: // prendi l'indirizzo dei maghi associabili (ideologicamente) a questi interi.
        if(passa_Lato == _side_mago){
            scan_mage(passa_Giocatore);
            *prendi_Mago = ptrmago;
            scan_mage(_none_memory);
        }
        break;
        case 2: case 3:
        if(passa_Pos >= 0){
            if(passa_Lato == _side_mano)
                presta_Arr = arr_Mano1; // assegna il doppio puntatore temporaneo, il puntatore array associato a questa mano.
            else if(passa_Lato == _side_campo)
                presta_Arr = arr_Campo1; // assegna il doppio puntatore temporaneo, il puntatore array associato a questo campo.
        }

        break;
        case 5: case 6:
        if(passa_Pos >= 0){
            if(passa_Lato == _side_mano) // assegna il doppio puntatore temporaneo, il puntatore array associato a questa mano.
                presta_Arr = arr_Mano2;
            else if(passa_Lato == _side_campo) // assegna il doppio puntatore temporaneo, il puntatore array associato a questo campo.
                presta_Arr = arr_Campo2;
        }
        break;
        case 8: case 9:
        if(passa_Pos >= 0){
            if(passa_Lato == _side_mano) // assegna il doppio puntatore temporaneo, il puntatore array associato a questa mano.
                presta_Arr = arr_ManoAvv;
            else if(passa_Lato == _side_campo) // assegna il doppio puntatore temporaneo, il puntatore array associato a questa campo.
                presta_Arr = arr_CampoAvv;
        }
        break;
        case 11: case 12:
        if(passa_Pos >= 0){
            if(passa_Lato == _side_mano)
                presta_Arr = arr_Mano2Avv;
            else if(passa_Lato == _side_campo)
                presta_Arr = arr_Campo2Avv;
        }
        break;
        default:
        printf("\nErrore: nessun valore corrisponde ad un indirizzo da prendere!\n");
    }

    if(len_Arr > 0 && event != _draw && passa_Pos >= 0 && (passa_Lato != _none_side && passa_Lato != _side_mago)){
        // una volta ottenuto l'array giusto, della lunghezza giusta, trova l'elemento a cui corrisponde
        // il determinato indirizzo da estrarre in questo array, grazie al parametro che da il valore in cui
        // e' posizionato in questo array temporaneo.
        for(int i = 0; i < len_Arr; i++){
            if(i == passa_Pos)
                *prendi_Carta = presta_Arr[i];
        }
    }
    return; // termina la chiamata di funzione con nessun valore.
}

static void guide_menu(void){
    char *selezione_guida = (char *)malloc(LEN);
    int mostra_input = 0, escape_guida = 0;
    
    do{
        clear_ansi(); // "pulisce" l'output della chiamata di funzione precedente.
        
        // stampa gli input disponibili.
        printf(CLR UND"\nMenu Guida di Tragic"CLR);
        printf("\n1) Storia\n2) Regole del gioco\n3) Maghi e Avversari\n4) Carte\n5) Crediti\n");
        printf("\nSeleziona una delle seguenti opzioni, oppure q per uscire:\n");

        fgets(selezione_guida, LEN, stdin); // inserisci l'input.
        clear_arrbuffer(selezione_guida, &escape_guida, 1); // pulisce gli spazi dentro gli elementi della stringa selezione_guida.

        if(*selezione_guida != 'q' && *selezione_guida != 'Q'){
            clear_ansi();
            print_guidemenu(&mostra_input, atoi(selezione_guida));

            // se mostra_input = 1 e' vera, chiede di immettere un input per
            // poter continuare a iterare questo ciclo do while.
            if(mostra_input == 1){
                printf("\n\nPremi un tasto qualsiasi per continuare...\n");
                getchar();

                // assegna questa variabile intera il valore 0, finche' questa funzione non richiamera'
                // la funzione print_guidemenu, non gli verra' assegnato il valore 1.
                mostra_input = 0;
            }
        }
    }while(*selezione_guida != 'q' && *selezione_guida != 'Q');
    
    clear_ansi();

    free(selezione_guida); // dealloca l'input.
    selezione_guida = NULL; // assegna il valore NULL, per la prossima chiamata di questa funzione void.
    return;
}

// Funzione static void che stampa delle informazioni in output per aiutare l'utente
// ad ambientarsi nel gioco.
static void print_guidemenu(int *passa_Stop, int passa_Guida){
    switch(passa_Guida){
        case 1:
        // stampa la storia del gioco
        printf(CLR ITA BOL"\n\t\t\t\t-- Storia --\n\n");
        printf("In una piana ventosa, non troppo lontana dalla Citta' delle Gilde, dei maghi si sfidano\n");
        printf("contro altri avversari a colpi di incantesimi e creature magiche in un duello di carte.\n");
        printf("\nChi ne uscira' vincitore?\n"CLR);
        
        // questa variabile passata per indirizzo, assegna 1 per fermare l'output con una richiesta in input
        // al termine di questa chiamata di funzione.
        *passa_Stop = 1;
        break;
        case 2:
        // stampa le regole del gioco
        printf(CLR ITA BOL"\n\t\t\t\t-- Regole del gioco --\n\n");
        printf("Il ");
        printf(YEL UND"giocatore"CLR);
        printf(ITA BOL" ha come obbiettivo quello di sconfiggere gli ");
        printf(YEL UND"avversari"CLR);
        printf(ITA BOL" dentro un campo di battaglia.\n\n");
        printf("Ogni mago ha un suo ");
        printf(YEL UND"mazzo"CLR);
        printf(ITA BOL" di carte giocabili (massimo 30 carte) da poter mettere sul "CLR); 
        printf(YEL UND"campo"CLR);
        printf(ITA BOL" oppure\nnella ");
        printf(YEL UND"mano"CLR);
        printf(ITA BOL".\n\n"CLR);
        printf(ITA BOL"\nSe i ");
        printf(YEL UND"punti vita"CLR);
        printf(ITA BOL" di uno dei maghi e' uguale a zero, "CLR);
        printf(YEL UND"la partita termina"CLR);
        printf(ITA BOL".\n\n"CLR);
        printf(ITA BOL"Se il "CLR);
        printf(YEL UND"numero di carte" CLR);
        printf(ITA BOL" nel mazzo di un mago e' uguale a zero"CLR); 
        printf(ITA BOL" e se non ci sono altri giocatori\n"CLR);
        printf(YEL UND"la partita termina"CLR);
        printf(ITA BOL".\n\n");
        
        // spiegazione delle differenti modalita' di gioco
        printf("\nE' possibile giocare in due modalita' di gioco:\n");
        printf(YEL UND"1 contro 1\n"CLR);
        printf(ITA BOL"Un mago e' contro l'avversario sul campo di battaglia.\n"CLR);
        printf(ITA BOL"Il giocatore controlla sia il mago che avversario, ogni turno\n"CLR);
        printf(ITA BOL"passa dal mago all'avversario e viceversa.\n\n");
        
        printf(YEL UND"2 contro 2\n"CLR);
        printf(ITA BOL"Due maghi in una squadra giocano contro altri due avversari sul\n"CLR);
        printf(ITA BOL"campo di battaglia.\n"CLR);
        printf(ITA BOL"Se uno dei maghi nella squadra viene sconfitto, la partita continua\n"CLR);
        printf(ITA BOL"finche' tutti i maghi in una squadra non vengono sconfitti.\n"CLR);
        
        // questa variabile passata per indirizzo, assegna 1 per fermare l'output con una richiesta in input
        // al termine di questa chiamata di funzione.
        *passa_Stop = 1;
        break;
        case 3:
        // alcune informazioni sui maghi e sulle loro classi
        printf(CLR ITA BOL"\n\t\t\t\t-- Maghi e Avversari --\n\n");
        printf("I ");
        printf(YEL ITA UND"maghi"CLR);  
        printf(ITA BOL" sono i personaggi principali che sfidano gli avversari.\n");
        printf("\nGli ");
        printf(YEL ITA UND"avversari"CLR);
        printf(ITA BOL" sono i personaggi che sfidano i maghi e che devono essere sconfitti.\n");
        printf("Se vince almeno uno di questi avversari, il duello e' perso.\n");
        printf("\n\nSia i maghi che gli avversari che giocano in partita, vengono distinti in tre classi:\n");
        printf(YEL UND"\nVita\n"CLR);
        printf(ITA BOL"I maghi e avversari con la classe Vita, influenzano le loro creature nel mazzo, aumentandone\ni punti vita della meta' in piu'.\n");
        printf(YEL UND"\nTenebre\n"CLR);
        printf(ITA BOL"I maghi e avversari con la classe Tenebre, hanno la meta' dei punti vita in piu' delle carte\ninfliggi danno.\n");
        printf(YEL UND"\nLuce\n"CLR);
        printf(ITA BOL"I maghi e avversari con la classe Vita, le loro carte guarisci danno, hanno tre volte e mezzo \ndei punti vita in piu'.\n");        
        
        // questa variabile passata per indirizzo, assegna 1 per fermare l'output con una richiesta in input
        // al termine di questa chiamata di funzione.
        *passa_Stop = 1;
        break;
        case 4:
        // nozioni del funzionamento delle carte e delle loro relative classi
        printf(CLR ITA BOL"\n\t\t\t\t-- Carte --\n\n"CLR);
        printf(YEL UND "Le carte"CLR);
        printf(ITA BOL" che sono presenti nel mazzo di ogni mago, inclusi gli avversari,\n"CLR);
        printf(ITA BOL"si distinguono in:\n");

        // spiegazione di tutte le classi delle carte
        printf(YEL UND"\nRimuovi creatura\n"CLR);
        printf(ITA BOL"Le carte rimuovi creatura distruggono una carta dal campo avversario.\n");
        printf(YEL UND"\nInfliggi danno\n"CLR);
        printf(ITA BOL"Le carte infliggi danno, danneggiano in base ai loro punti vita, creature sul campo\n"CLR);
        printf(ITA BOL"nemico, oppure direttamente il mago avversario.\n"CLR);
        printf(YEL UND"\nGuarisci danno\n"CLR);
        printf(ITA BOL"Le carte di questa classe, aumentano i punti vita di una creatura oppure di un mago\n");
        printf(ITA BOL"con i punti vita della carta guarisci danno utilizzata.\n"CLR);
        printf(YEL UND"\nCreatura\n"CLR);
        printf(ITA BOL"Le creature sono carte evocabili sul campo, una volta evocate possono attaccare\n"CLR);
        printf(ITA BOL"una creatura oppure direttamente il mago avversario. Se una creatura attacca\n"CLR);
        printf(ITA BOL"il mago avversario, e i suoi punti vita sono inferiori, quella creatura\n"CLR);
        printf(ITA BOL"subisce a sua volta un danno.\n\n"CLR);

        printf(YEL UND"\nTutte"CLR);
        printf(ITA BOL" le carte che utilizzano il loro "); 
        printf(YEL UND"effetto"CLR);
        printf(ITA BOL" (ad eccezione delle creature)\n"CLR);
        printf(ITA BOL"vengono immediatamente ");
        printf(YEL UND"distrutte"CLR);
        printf(ITA BOL" dalla mano del mago che le sta utilizzando."CLR);
        
        
        // questa variabile passata per indirizzo, assegna 1 per fermare l'output con una richiesta in input
        // al termine di questa chiamata di funzione.
        *passa_Stop = 1;
        break;
        case 5:
        // mostra alcune informazioni sullo sviluppatore
        printf(CLR ITA BOL"\n\t\t\t\t-- Crediti --\n\n");
        printf("Codice: Alessandro Pagano\n");
        printf("\nUI: Alessandro Pagano\n");
        printf("\nArt: Alessandro Pagano\n\n");
        printf("\nLa leggenda narra che sia ancora vivo e che sia riuscito a passare l'esame.\n\n");

        // alcune informazioni sul progetto
        printf("\nProgetto assegnato per l'esame di Programmazione Procedurale con Laboratorio.\n");
        printf("Universita' degli Studi di Perugia, Anno Accademico 2022.\n");
        *passa_Stop = 1;
        break;
        default:
        printf("\nErrore: nessun valore corrisponde ad un output da stampare!\n");
        *passa_Stop = 0; // l'output non verra' fermato con una richiesta in input.
    }
    return;
}

// Funzione static void per la stampa della memoria in output mediante interfaccia grafica.
static void mem_menu(void)
{
    char *selezione_memoria = (char *)malloc(LEN); // puntatore di caratteri inizializzato con malloc.
    int set_escape = 0, imposta_lato = 0; // definizioni di variabili intere.

    fflush(stdout);

    fun_print = _show_all_address_mem;
    do{
        clear_ansi(); // lascia spazio al menu che verra' stampato nelle prossime istruzioni.
        printf("\n");
        printf(CLR UND"Menu Dati Giocatori"CLR);
        printf(ITA"\nGiocatori:\n"CLR);
        // (vuoto) significa che mettendo in input un determinato valore, 
        // stampera' in grigio, che questo indirizzo vale NULL, senza andare in segmentation fault,
        // perche' gli e' stato direttamente assegnato il valore NULL.
        // Rimarra' tale quel valore, finche', non vi sara' allocata memoria.

        // Caso contrario, see una determinata lista Mago, con un determinato array Mazzo, vi e' allocata memoria dinamica,
        // allora, colorato in bianco, stampera' il loro indirizzo esadecimale, sia del mago che dei suoi eventuali
        // array associati.
        if(mode == _1versus1 || mode == _2versus2)
            printf("1) Mago\n2) Mano Mago\n3) Mazzo Mago\n");
        else
            printf(DRK_GRY"1) Mago (vuoto)\n2) Mano Mago (vuoto)\n3) Mazzo Mago (vuoto)\n"CLR);
        if(mode ==_2versus2)
            printf("\n4) Mago 2\n5) Mano Mago 2\n6) Mazzo Mago 2\n");
        else
            printf(DRK_GRY"\n4) Mago 2 (vuoto)\n5) Mano Mago 2 (vuoto)\n6) Mazzo Mago 2 (vuoto)\n"CLR);

        printf(ITA"\nAvversari:\n"CLR);
        if(mode == _1versus1 || mode == _2versus2)
            printf("7) Mago Avv.\n8) Mano Mago Avv.\n9) Mazzo Mago Avv.\n");
        else
            printf(DRK_GRY"7) Mago Avv. (vuoto)\n8) Mano Mago Avv. (vuoto)\n9) Mazzo Mago Avv. (vuoto)\n"CLR);
        if(mode == _2versus2)
            printf("\n10) Mago 2 Avv.\n11) Mano Mago 2 Avv.\n12) Mazzo Mago 2 Avv.\n");
        else
            printf(DRK_GRY"\n10) Mago 2 Avv. (vuoto)\n11) Mano Mago 2 Avv. (vuoto)\n12) Mazzo Mago 2 Avv. (vuoto)\n"CLR);

        printf("\nSeleziona una delle seguenti liste da stampare, oppure q per uscire:\n");
        fgets(selezione_memoria, LEN, stdin); // chiede un input all'utente di stampare un numero tra 1 a 12.
        clear_arrbuffer(selezione_memoria, &set_escape, 1); // pulisci gli spazi dopo la chiamata fgets

        if (set_escape == 1 && (*selezione_memoria != 'q' && *selezione_memoria != 'Q')){
            // se selezione_memoria il suo valore implicitamente convertito in decimale corrisponde ai valori di questa
            // condizione, stampa un determinato array associato ad un mago.
            if(atoi(selezione_memoria) == 1 ||  atoi(selezione_memoria) == 4 || atoi(selezione_memoria) == 7
            || atoi(selezione_memoria) == 10)
                imposta_lato = _side_mago;
            else
                imposta_lato = _side_mano;
            clear_ansi();
            print_mem(atoi(selezione_memoria), imposta_lato, 1);
        }
    } while (*selezione_memoria != 'q' && *selezione_memoria != 'Q');
    clear_ansi();

    free(selezione_memoria); // dealloca l'input.
    selezione_memoria = NULL; // assegna il valore NULL, per la prossima chiamata di questa funzione void.
    return;
}
// ------------
// Mostra gli input disponibili e le azioni da compiere con una carta scelta (e' possibile stampare
// individualmente gli indirizzi dei maghi/carte presenti nel determinato turno, precisamente il titolo
// identificativo del mago che sta giocando contro il mago avversario).
static void game_input(char *passa_Gioco, int *presta_selTar[2], int *caso_Menu, int *reset_Pos, int *tipo_selScelta){
    // Stampa la prima sezione del menu, si puo' chiedere di far vedere le scelte sul campo di battaglia, e quindi
    // proseguire alla sezione successiva (*caso_Menu = 2), oppure far fare al mago delle determinate
    // azioni.
    if(*caso_Menu == 1){
        printf("\nE' il tuo turno, seleziona una scelta:\n");
        printf("\n1) Seleziona");
        // Finche' il mago che sta giocando non compie un azione qualsiasi, sara' disabilitata la possibilita' 
        // di passare il turno, senza completare tutte e tre le azioni che un mago puo' effettuare 
        // (escluse le evocazioni delle creature sul campo), la quale, se abilitato, il cambiare turno stesso.
        if(action[0] == _completed_action || action[1] == _completed_action || action[2] == _completed_action)
            printf("\t\t3) Passa il turno");
        if(action[0] != _completed_action)
            printf("\n2) Pesca carta");
        printf("\n\n\t\t\tq - Esci dalla partita\n"); // si puo' uscire dalla partita in qualsiasi momento del gioco.

        fgets(passa_Gioco, LEN, stdin);
        clear_arrbuffer(passa_Gioco, NULL, 1);
        
        if(*passa_Gioco != 'q' && *passa_Gioco != 'Q'){
            switch(atoi(passa_Gioco)){
                case 1:
                // mostra tutti gli input disponibili, alla prossima iterazione del ciclo do while in combatti, 
                // cambiera' l'output di cosa chiedere in input.
                fun_print = _show_all_selections;
                // il primo elemento dell'array di caratteri, ha il valore decimale ASCII associato allo space key,
                // pertanto input valido, ma che fa ripetere il ciclo e ristampare il menu di questa sezione.
                *passa_Gioco = ' ';
                *reset_Pos = 0;
                *caso_Menu = *caso_Menu + 1;
                break;
                case 2:
                event = _none_event; // Resetta eventuali eventi non voluti dal turno precedente.
                if(action[0] != _completed_action)
                    event = _draw;
                *passa_Gioco = ' ';
                *reset_Pos = 0;
                *caso_Menu = 1;
                break;
                case 3:
                event = _none_event;
                if(action[0] == _completed_action || action[1] == _completed_action || action[2] == _completed_action){
                    event = _skip_turn;
                    if(ptr_selMago != NULL)
                        ptr_selMago = NULL;
                    if(ptr_selCarta != NULL)
                        ptr_selCarta = NULL;
                    
                    *tipo_selScelta = 0;
                    *presta_selTar[0] = -1; // resetta il valore associato alla posizione array campo/mano da prendere in considerazione.
                    
                    // resetta il valore associato alla conferma della selezione della carta, 
                    // se e' maggiore di 1 sara' il puntatore tar_carta/tar_mago, a prendere un indirizzo.
                    // altrimenti se *reset_Pos = 0, sara' sel_carta/sel_mago a prendere un indirizzo.
                    *reset_Pos = 0; 
                }
                *passa_Gioco = ' ';
                *caso_Menu = 1;
                break;
                default:
                printf("\nErrore: nessun valore corrisponde ad un input valido!\n");
            }
        }
        else{
            *passa_Gioco = ' ';
            do{
                // ciclo do while che stampa una richiesta di input di uscire dalla partita, finche' l'utente non mette un input
                // valido il ciclo non sara' vero, quindi reitera e ristampa la richiesta.
                clear_ansi();
                printf("\nVuoi uscire dal gioco? (la partita verra' salvata)\n\nPremi y per confermare, n per declinare.\n");
                fgets(passa_Gioco, LEN, stdin);
                clear_arrbuffer(passa_Gioco, NULL, 1);
                
                if(*passa_Gioco == 'y' || *passa_Gioco == 'Y'){
                    // se la condizione e' vera, assegna a NULL, se non sono equivalenti a NULL
                    // queste variabili globalmente dichiarate.
                    if(ptr_selMago != NULL)
                        ptr_selMago = NULL;
                    if(ptr_selCarta != NULL)
                        ptr_selCarta = NULL;

                    *tipo_selScelta = 0;
                    *presta_selTar[0] = -1;
                    *reset_Pos = 0;
                    *passa_Gioco = 'q';
                    // se questa definizione diventa uguale a _pause_game, membro di enum Evento_Gioco, non chiamera'
                    // la funzione print_gameout, mostrando gli eventuali output che non hanno senso stampare
                    // per il momento (non incide sulla stampa delle liste dinamicamente create solo sulla chiamata di 
                    // print_gameout).
                    event = _pause_game;
                    break; // interruzione diretta di questo ciclo dato che *passa_Gioco non sara' mai uguale a 'n' oppure 'N' se si vuole uscire dal gioco in questo caso.
                }
                else if(*passa_Gioco == 'n' || *passa_Gioco == 'N'){
                    *caso_Menu = 1; // termina la chiamata di questa funzione, reitera il ciclo in combatti, ristampa la prima sezione di input.
                    event = _none_event; // per sicurezza, se vi sono associati eventi non desiderati, resetta questa definizione di tipo enum.
                }
            }while(*passa_Gioco != 'n' && *passa_Gioco != 'N');
        }
    }
    else if(*caso_Menu == 2){
        // seconda sezione dell'input dedicato al giocatore.
        if(*reset_Pos <= 0){
            printf("\nSeleziona un elemento sul campo di battaglia.\n\n\t\t\tq - Torna indietro\n");
            // richiesta in input di selezionare qualsiasi cosa che vede senza vincoli (perche' vi saranno degli input non validi,
            // ma che non daranno errore in output, semplicemente faranno reiterare il ciclo per non dare output fastidiosi al giocatore
            // mentre gioca).
            
            *reset_Pos = 0; // resetta per sicurezza come per prevenire un ciclo infinito nella funzione combatti.

            fgets(passa_Gioco, LEN, stdin);
            clear_arrbuffer(passa_Gioco, NULL, 1);

            if(*passa_Gioco != 'q' && *passa_Gioco != 'Q')
                *presta_selTar[0] = atoi(passa_Gioco);
            else{
                *caso_Menu = 1; // torna alla prima fase del menu perche' il giocatore richiede di scegliere qualcos'altro.
                *presta_selTar[0] = -1; // resetta per sicurezza questa variabile, anche se gli appositi puntatore sono ancora assegnati a NULL (ptr_selMago, ptrselCarta).
                *passa_Gioco = ' ';
                fun_print = _none_print_choice; // non mostrare in output i numeri delle selezioni disponibili al giocatore.
            }
        }
        else if(*reset_Pos >= 1){
            // chiamata di funzione check_gamefieldlayer, che eventualmente assegnera' in un valore intero in base al valore di implicitamente passato per intero di passa_Gioco,
            // quello che il giocatore ha chiesto di selezionare, che sia un mago oppure una carta.
            check_gamefieldlayer(&select_player, &select_handdeck, atoi(passa_Gioco));
            
            // qualunque cosa il giocatore ha chiesto di selezionare, i puntatore sel_Carta, oppure sel_Mago (dipende da quello che il giocatore ha scelto),
            // gli verra' assegnato a uno solo dei due un indirizzo di memoria
            get_arraddress(&ptr_selMago, &ptr_selCarta, select_player, select_handdeck, *presta_selTar[0]);
            
            *caso_Menu = *caso_Menu + 1; // incrementa questa variabile, reitera il ciclo, richiama questa funzione da combatti, passa alla sezione successiva.

            *passa_Gioco = ' ';

            sel_print = _highlight_mem;
            fun_print = _none_print_choice; // non chiama la funzione che stampa i numeri degli input disponibili sul campo di battaglia.
        }
    }
    else if(*caso_Menu == 3){
        // terza sezione dedicata alla stampa delle scelte, e alla richiesta in input al giocatore
        // di cosa e' possibile scegliere in basa a quello che ha scelto prima.
        printf("\nSeleziona un opzione:\n");
        
        if(ptr_selMago != NULL){
            if(select_handdeck == _side_mago){
               printf("1) Stampa indirizzo");
               *tipo_selScelta = 1;
            }
        }
        else if(ptr_selCarta != NULL && select_player != avversario+1){
            // se questa condizione e' vera allora e' stata scelta sul campo di battaglia
            // una carta creatura.
            if(ptr_selCarta->tipo_carta == 0 && select_handdeck == _side_mano){
                // se questa condizione e' vera allora la creatura scelta e' nella mano
                // del giocatore.
                printf("1) Evoca\n");
                printf("2) Mostra indirizzo");
                *tipo_selScelta = 2;
            }
            else if(ptr_selCarta->tipo_carta == 0 && select_handdeck == _side_campo){
                // se questa condizione e' vera allora la creatura scelta e' nel campo 
                // di battaglia.
                if(action[2] == _none_event){
                    printf("1) Attacca\n");
                    printf("2) Mostra indirizzo");
                }
                else if(action[2] == _completed_action)
                   printf("1) Stampa indirizzo");              
                *tipo_selScelta = 3;
            }
            else if(ptr_selCarta->tipo_carta == 1){
                // se questa condizione e' stata scelta una carta rimuovi_creatura.
                if(action[1] == _none_event){
                    printf("1) Rimuovi creatura\n");
                    printf("2) Mostra indirizzo");
                }
                else if(action[1] == _completed_action)
                   printf("1) Stampa indirizzo");
                *tipo_selScelta = 4;
            }
            else if(ptr_selCarta->tipo_carta == 2){
                // se questa condizione e' stata scelta una carta infliggi_danno.
                if(action[1] == _none_event){
                    printf("1) Infliggi danno\n");
                    printf("2) Mostra indirizzo");
                }
                else if(action[1] == _completed_action)
                   printf("1) Stampa indirizzo");
                *tipo_selScelta = 5;
            }
            else if(ptr_selCarta->tipo_carta == 3){
                // se questa condizione e' stata scelta una carta guarisci_danno.
                if(action[1] == _none_event){
                    printf("1) Cura\n");
                    printf("2) Mostra indirizzo");
                }
                else if(action[1] == _completed_action)
                   printf("1) Stampa indirizzo");
                *tipo_selScelta = 6;
            }
        }
        else if(ptr_selCarta != NULL && select_player == avversario+1 && (select_handdeck == _side_mano || select_handdeck == _side_campo)){
            // e' stata scelta una carta sul campo di battaglia, ma non e' possibile eseguire alcuna
            // azione.
            printf("1) Stampa indirizzo");
            *tipo_selScelta = 7;
        }
        else if(ptr_selCarta == NULL){
            // e' stato scelto un mago sul campo di battaglia, ma non e' possibile eseguire alcuna
            // azione.
            printf("1) Stampa indirizzo");
            *tipo_selScelta = 7;
        }

        if(*tipo_selScelta == 4 || *tipo_selScelta == 5 || *tipo_selScelta == 6){
            // una carta rimuovi_creatura, oppure infliggi_danno, oppure guarisci_danno
            // e' stata selezionata, pero' e' stato gia' attivato un effetto carta.
            if(action[1] == _completed_action)
                *tipo_selScelta = 7;
        }
        if(*tipo_selScelta == 3){
            // una carta creatura e' stata selezionata, pero' questa, oppure un'altra carta
            // creatura ha gia' attaccato.
            if(action[2] == _completed_action)
                *tipo_selScelta = 7;
        }

        printf("\n\n\t\t\tq - Torna indietro\n");

        fgets(passa_Gioco, LEN, stdin); // richiedi l'input al giocatore.
        clear_arrbuffer(passa_Gioco, NULL, 1);

        if(*passa_Gioco != 'q' && *passa_Gioco != 'Q')
            targetinput_sel(*&caso_Menu, atoi(passa_Gioco), *tipo_selScelta);
        else{
            // allora se il giocatore vuole tornare indietro, resetta per sicurezza
            // variabili importanti locali oppure passate per indirizzo.
            if(ptr_selMago != NULL)
                ptr_selMago = NULL;
            else if(ptr_selCarta != NULL)
                ptr_selCarta = NULL;

            if(ptr_selMago != NULL)
                ptr_selMago = NULL;
            if(ptr_selCarta != NULL)
                ptr_selCarta = NULL;

            *tipo_selScelta = 0;
            *passa_Gioco = ' ';
            *presta_selTar[0] = -1;
            *reset_Pos = 0;

            select_player = _none_memory;
            select_handdeck = _none_memory;
            fun_print = _none_print_choice;
            sel_print = _none_print_choice;

            *caso_Menu = 1;
        }
    }
    else if(*caso_Menu == 4){
        // quarta sezione della stampa degli input.
        if(*reset_Pos  <= 1){
            // la creatura attacca, mostra in input chi puo' e chi non puo' attaccare.
            if(fun_print == _show_dmg_selections){
                if(ptr_selCarta->tipo_carta == 0)
                    printf("\nSeleziona chi attaccare con la creatura.");
                else if(ptr_selCarta->tipo_carta == 2)
                    printf("\nSeleziona a chi infliggere danno.");
            }
            else if(fun_print == _show_only_creatures_dmg){
                // la carta rimuovi_creatura attiva il suo effetto.
                if(ptr_selCarta->tipo_carta == 1)
                    printf("\nSeleziona una creatura da distruggere.");
            }
            else if(fun_print == _show_heal_selections){
                // la carta guarisci_danno attiva il suo effetto.
                if(ptr_selCarta->tipo_carta == 3)
                    printf("\nSeleziona chi curare.");
            }
            else if(fun_print == _show_creature_summonings){
                // la carta creatura sta per essere evocata.
                if(ptr_selCarta->tipo_carta == 0)
                    printf("\nSeleziona dove evocare la creatura.");
            }
            printf("\n\n\t\t\tq - Torna indietro\n");

            fgets(passa_Gioco, LEN, stdin);
            clear_arrbuffer(passa_Gioco, NULL, 1);
            
            if(*passa_Gioco != 'q' && *passa_Gioco != 'Q')
                *presta_selTar[1] = atoi(passa_Gioco); // assegnazione di un valore intero convertito implicitamente.
            else{       
                // reset di puntatori ed eventuali variabili se il giocatore vuole tornare
                // indietro.
                if(*tipo_selScelta != 7 && *tipo_selScelta != 1){
                    // questa volta sara' il puntatore ptr_tarMago oppure ptr_tarCarta
                    // a venire assegnato NULL, se non e' uguale a NULL.
                    if(ptr_tarMago != NULL)
                        ptr_tarMago = NULL;
                    if(ptr_tarCarta != NULL)
                        ptr_tarCarta = NULL;

                    *tipo_selScelta = 0;
                    *passa_Gioco = ' ';
                    *reset_Pos = 1;
                    *presta_selTar[1] = -1;

                    fun_print = _none_print_choice;
                    event = _none_event;

                    *caso_Menu = 3;
                }
                else{
                    if(ptr_selMago != NULL)
                        ptr_selMago = NULL;
                    if(ptr_selCarta != NULL)
                        ptr_selCarta = NULL;

                    *tipo_selScelta = 0;
                    *passa_Gioco = ' ';
                    *reset_Pos = 0;
                    *presta_selTar[0] = -1;
                    select_player = _none_memory;
                    select_handdeck = _none_memory;
                    fun_print = _none_print_choice;
                    sel_print = _none_print_choice;

                    *caso_Menu = 1;
                }
            }
        }
        else if(*reset_Pos >= 2){
            // Stesse chiamate che avvengono in condizione vera di *caso_Menu = 2.
            check_gamefieldlayer(&target_player, &target_handdeck, atoi(passa_Gioco));
            
            // estrae l'indirizzo con i valori convertiti ha disposizione passandolo tra
            // questi parametri.
            get_arraddress(&ptr_tarMago, &ptr_tarCarta, target_player, target_handdeck, *presta_selTar[1]);

            // le assegnazioni fatte in condizione vera di *caso_Menu = 3, hanno determinato
            // il valore passato per indirizzo di *tipo_selScelta.
            targetinput_sel(*&caso_Menu, 1, *tipo_selScelta);
        }
    }
    else if(*caso_Menu == 5){
        // Resetta tutto
        if(ptr_tarMago != NULL)
            ptr_tarMago = NULL;
        if(ptr_tarCarta != NULL)
            ptr_tarCarta = NULL;
        
        if(ptr_selMago != NULL)
            ptr_selMago = NULL;
        if(ptr_selCarta != NULL)
            ptr_selCarta = NULL;

        *tipo_selScelta = 0;
        *passa_Gioco = ' ';
        *reset_Pos = 0;
        *presta_selTar[0] = -1;
        *presta_selTar[1] = -1;
        select_player = _none_memory;
        target_player = _none_memory;
        select_handdeck = _none_memory;
        target_handdeck = _none_memory;
        fun_print = _none_print_choice;
        sel_print = _none_print_choice;
        event = _none_event;
        type_target = _none_event;

        // assegna a *caso_Menu il valore 1, ricomincia la logica
        // dello scopo iterativo di questa funzione fin dall'inizio.
        *caso_Menu = 1;
    }
    return;
}
// Funzione static void dedicata alla gestione di due definizioni globali
// al tipo enumeratore Evento_Gioco.
static void choiceof_play(int *passa_selTar[2], int *passa_Vincitore){
    if(event != _change_turn && event != _skip_turn && event != _print_ending){
        if(event == _straight_attack || event == _play_throwaway_card || event == _evoke_creature){
            // chiama la funzione gioca_carta.
            gioca_carta(ptr_selCarta, ptr_tarCarta, ptr_tarMago, passa_selTar);
            type_target = _none_event; // il target non ha piu' eventi asasociati, perche' quello evento e' terminato dopo la chiamata a gioca_Carta.
            fun_print = _none_print_choice; // deseleziona, ovvero istruisce alla stampa di nessun formato bold che "illumina" il giocatore selezionato.
        }
        if(event == _draw){
            // chiamata di funzione che fa pescare una carta.
            draw_card(giocatore, &*passa_Vincitore);
            action[0] = _completed_action; // azione associata al pescare una carta completata.
        }
        if(event == _dead_mage){
            printf(ITA "\nIl mago avversario e' stato sconfitto!\n"CLR);
            set_delayclock(1, 4);
            event = _change_turn; // cambia il turno, perche' la lista e' deallocata, e gli elementi in array mano/campo sono tutti NULL.
        }

        if(event == _dead_by_hand){
            printf(ITA "\nIl mago non ha piu' carte nel mazzo, il mago e' eliminato!\n"CLR);
            set_delayclock(1, 4);
            event = _change_turn; // alloca cambia il turno, perche' questo giocatore non puo' piu' giocare dato che la sua lista e' stata deallocata.
        }

        // se e' maggiore *passa_Vincitore, variabile passata per indirizzo e dichiarata in combatti, di zero,
        // a priori verra' terminato il ciclo do while in combatti e stampata la schermata di vittoria, sconfitta
        // oppure pareggio.
        if(*passa_Vincitore > 0)
            event = _print_ending;

        if(action[0] == _completed_action && action[1] == _completed_action && action[2] == _completed_action){
            action[0] = _none_event;
            action[1] = _none_event;
            action[2] = _none_event;

            event = _change_turn;
        }
    }

    // questa condizione si occupa di cambiare il turno e di assegnare tutti gli elementi
    // dell'array action al valore del membro _none_event.
    if(event == _change_turn || event == _skip_turn){
        action[0] = _none_event, action[1] = _none_event, action[2] = _none_event;
        
        passa(&*passa_Vincitore); // questa chiamata di funzione cambia il turno e determina il/i vincitore/i.
        event = _none_event;
        
        // se dopo che e' stato sconfitto un avversario da un altro mago, nella funzione passa,
        // viene trovato il vincitore allora questa condizione e' falsa.
        if(*passa_Vincitore <= 0){
            printf(ITA "\nIl turno e' concluso...\n"CLR); // non stampa che il turno e' concluso se non ci sono altri maghi con cui combattere.
            set_delayclock(1, 3); // delay di 3 secondi in output, per permettere di leggere all'utente che il turno e' cambiato.
        }
    }
    
    if(*passa_Vincitore > 0){
        event = _print_ending;
        set_delayclock(1, 4); // delay di 4 secondi in output, per far intuire all'utente che la partita sta per terminare e sta per caricare l'esito finale in output.
    }
    if(*passa_Vincitore <= 0)
        event = _none_event; // ulteriore controllo, per resettare qualsiasi evento associato a questa definizione.
    return;
}

// Funzione static void fondamentale perche' questa funzione e' il nucleo in cui
// viene calcolato e stampato l'output della cosiddetta numerazione in stile "battaglia navale",
// la quale mostra dei numeri che suggeriscono cosa e' possibile fare o meno sul campo di battaglia.
static void targetinput_sel(int *gioco_Menu, int passa_inputGioco, int caso_selTar){
    if(*gioco_Menu == 3){
        if(passa_inputGioco == 1){
            if(caso_selTar == 1){
                fun_print = _show_single_address_mem; // mostra l'indirizzo di cio' che e' stato scelto dal giocatore.
                
                // passa al menu di input successivo in game_input, perche' e' l'input e' valido, la richiesta di input
                // dell'utente e' stata completata.
                *gioco_Menu = *gioco_Menu + 1;
            }
            else if(caso_selTar == 2){
                // mostra le posizioni valide per evocare la creatura sul campo
                // (se non e' gia' occupata da un'altra creatura).
                fun_print = _show_creature_summonings;
                *gioco_Menu = *gioco_Menu + 1; // passa al menu di input successivo.
            }
            else if(caso_selTar == 3 || caso_selTar == 5){
                fun_print = _show_dmg_selections;
                *gioco_Menu = *gioco_Menu + 1;
            }
            else if(caso_selTar == 4){
                fun_print = _show_only_creatures_dmg;
                *gioco_Menu = *gioco_Menu + 1; // passa al menu di input successivo.
            }
            else if(caso_selTar == 6){
                fun_print = _show_heal_selections; // mostra la carta ed il mago protagonista del turno la possibilita' di aumentare i PV.
                *gioco_Menu = *gioco_Menu + 1;
            }
            else if(caso_selTar == 7){
                fun_print = _show_single_address_mem;
                *gioco_Menu = *gioco_Menu + 1;
            }
        }
        else if(passa_inputGioco == 2){
            if(caso_selTar == 2 || caso_selTar == 3 || caso_selTar == 4 || caso_selTar == 5 || caso_selTar == 6){
                fun_print = _show_single_address_mem;
                *gioco_Menu = *gioco_Menu + 1;
            }
        }
    }
    else if(*gioco_Menu == 4){
        if(passa_inputGioco == 1){
            // se caso_selTar = 1, il giocatore ha deciso che verra' stampato un indirizzo di memoria
            // in output.
            if(caso_selTar == 1)
                event = _none_event;
            else if(caso_selTar == 2)
                event = _evoke_creature; // evento associato all'azione di spostare da una posizione dell'array mano ad una posizione dell'array campo del mago che sta giocando nel turno corrente.
            else if(caso_selTar == 3){
                if(ptr_tarCarta != NULL && ptr_tarMago == NULL){
                    event = _straight_attack; // evento associato ad un attacco nei confronti di una carta/mago avversario.
                    type_target = _deal_creature_dmg; // evento associato al target di riferimento, decrementa la vita di una carta/mago avversario perche' sta subendo un attacco.
                }
                else{
                     // evento associato ad una creatura che sta attaccando un'altra creatura sul campo
                     // di battaglia, oppure direttamente il mago (si autodanneggia mentre attacca nel
                     // secondo caso).
                    event = _straight_attack;
                     // allora punta alla variabile intera e ne cambiera' il suo valore, dell'indirizzo
                     // ptr_tarCarta/ptr_tarMago relativa ai punti vita.
                    type_target = _deal_mage_dmg;
                }
            }
            else if(caso_selTar == 4){
                // evento associato all'utilizzo di una carta rimuovi_creatura che verra' deallocata subito dopo.
                event = _play_throwaway_card;
                type_target = _deal_creature_dmg; // allora attacca la creatura.
            }
            else if(caso_selTar == 5){
                if(ptr_tarCarta != NULL && ptr_tarMago == NULL){
                    // evento associato all'utilizzo di una carta che e' in grado di fare del danno al mago avversario.
                    event = _play_throwaway_card;
                    type_target = _deal_creature_dmg;
                }
                else{
                    event = _play_throwaway_card;
                    type_target = _deal_mage_dmg;
                }
            }
            else if(caso_selTar == 6){
                if(ptr_tarCarta != NULL && ptr_tarMago == NULL){
                    // evento associato all'utilizzo di una carta che e' in grado di aumentare i pv alla creatura del giocatore
                    // del turno corrente.
                    event = _play_throwaway_card;
                    type_target = _heal_creature;
                }
                else{
                    // evento associato all'utilizzo di una carta che e' in grado di aumentare i pv al mago associato al giocatore
                    // del turno corrente.
                    event = _play_throwaway_card;
                    type_target = _heal_mage;
                }
            }
        }
        *gioco_Menu = *gioco_Menu + 1;
    }
    return;
}
// ------------
static void print_mage(int memoria_Mago, int evidenzia_Mago)
{
    char *temp_nome = "";
    int temp_deck = 0; // prende in "prestito" il valore del numero di carte presenti nel mazzo di ogni mago.

    // mostra in output di che tipo di mago si stratta (se e' avversario oppure un giocatore principale)
    if(mem_title == _title_visible || mem_title == _title_only_mage_deck_visible)
        print_playername(memoria_Mago, _none_side, 1);
    scan_mage(memoria_Mago);
    scan_deck(memoria_Mago, NULL, NULL, &temp_deck, _side_mazzo);

    if (ptrmago != NULL){
        print_playerdeck(&temp_nome, ptrmago->classe_mago, 1);

        if(evidenzia_Mago == 0){
            printf(CYN ITA "%s\n"CLR, ptrmago->nome);
            printf(HRED "PV: %d\t\t"CLR, ptrmago->mago_punti_vita);
            printf(YEL "Classe: %s\t\t\t\t\t\t\t\t\t\t\t\t"CLR, temp_nome);
        }
        else{
                printf(CYN BOL UND ITA "%s\n"CLR, ptrmago->nome);
                printf(HRED BOL UND "PV:");
                printf(" ");
                printf(RED BOL UND"%d          "CLR, ptrmago->mago_punti_vita);
                printf(YEL BOL UND "Classe: %s\t\t\t\t\t\t\t\t\t\t\t\t"CLR, temp_nome);
            }
        printf(ITA"Deck: %d"CLR, temp_deck);
    }
    else
        printf(DRK_GRY"Mago vuoto"CLR);
    if(fun_print == _show_all_address_mem)
        print_addr(NULL, NULL, ptrmago, _side_mago, -1);
    return;
}

static void print_deck(int memoria_Mazzo, int len_Mazzo, int *passa_Blocco)
{
    // puntatore array temporaneo che ha il numero di elementi lo stesso valore del numero delle
    // carte del mazzo del mago associato ad esso.
    Mazzo *passa_mazzo[len_Mazzo];

    char *loop_mazzo = (char *)malloc(LEN);
    char *temp_tipomz = "";
    int avanti_lista = 5, input_check = 0;

    for (int i = -1; i < len_Mazzo; i++)
    {
        if (i == avanti_lista - 5 || i < 0)
        {
            if(mem_title == _title_visible)
                print_playername(memoria_Mazzo, _none_side, 1);
            scan_deck(memoria_Mazzo, passa_mazzo, NULL, NULL, _side_mazzo);
        }

        // se il mazzo passato per parametri e' uguale a 0, sappiamo che la lista e'
        // vuota, pertanto stampiamo che non ci sono elementi in coda alla struct Mazzo,
        // stampa l'informazione che la struct Mazzo e' vuota, interrompi ed esci subito
        // dal ciclo.
        if (len_Mazzo == 0){
            printf(DRK_GRY "\nMazzo vuoto\n"CLR);
            break;
        }
        
        if (i < 0){
            // se il valore del mazzo e' maggiore di 0, pertanto il ciclo non e' stato interrotto,
            // si puo' iniziare a scorrere i nodi della struct Mazzo richiesta di vedere.
            *passa_Blocco = 0;
            
            // incrementa solo questa volta di 1 il valore intero dell'indice di questo ciclo for,
            // perche' inizializzato a -1 ed andrebbe fuori memoria la lista se lasciato a -1.
            i++;
        }

        if (passa_mazzo[i] != NULL)
        {
            // stampa le informazioni del mazzo in output.
            printf(GRN "\nCarta n.%d" CLR, i + 1);
            print_addr(NULL, passa_mazzo[i], NULL, _none_side, -1);
            printf(HRED "\tPV: %d\n", passa_mazzo[i]->carta_punti_vita);
            print_playerdeck(&temp_tipomz, passa_mazzo[i]->tipo_carta, 2);
            printf(YEL "Tipo: %s\n\n" CLR, temp_tipomz);
        }
        else
        {
            printf(CLR DRK_GRY"Slot mazzo vuoto\t\n\n"CLR);
        }

        // se si trova in cima alla lista oppure alla fine della lista
        // mostra i comandi disponibili relativi al caso in cui il giocatore
        // sta "sfogliando" la lista.
        if (i >= avanti_lista - 1 || i >= len_Mazzo - 1)
        {
            if (avanti_lista <= 5 && i < len_Mazzo - 1)
                printf("\n\t\t\td >\n");
            else if (avanti_lista > 5 && avanti_lista < len_Mazzo)
                printf("\n<< a\t\t\td >\n");
            else if (avanti_lista >= len_Mazzo && i >= 5)
                printf("\n<< a\t\n");

            printf("\npremi q per uscire dal Menu.\n\n");

            fgets(loop_mazzo, LEN, stdin);
            clear_arrbuffer(loop_mazzo, &input_check, 2);
            clear_ansi();

            if (*loop_mazzo == 'd' && avanti_lista < len_Mazzo && input_check == 1)
            {
                avanti_lista = avanti_lista + 5;
                continue;
            }
            else if (*loop_mazzo == 'a' && i > 4 && input_check == 1)
            {
                i = -1;
                avanti_lista = 5;
                continue;
            }
            else if (*loop_mazzo == 'q' && input_check == 1){
                break;
            }

            if (i >= 4 && avanti_lista <= len_Mazzo)
                i = i - 5;
            if (i >= 4 && avanti_lista > len_Mazzo)
                i = (avanti_lista - 1) - 5;
            if (i < 4 && avanti_lista > len_Mazzo)
                i = -1;
        }
    }
    return;
}

static void sel_handsorted(int passa_Mano, int scegli_Caso)
{
    switch(passa_Mano){
        case 1: case 2: case 3:
        arr_handsort(arr_Mano1, scegli_Caso);
        break;
        case 4: case 5: case 6:
        arr_handsort(arr_Mano2, scegli_Caso);
        break;
        case 7: case 8: case 9:
        arr_handsort(arr_ManoAvv, scegli_Caso);
        break;
        case 10: case 11: case 12:
        arr_handsort(arr_Mano2Avv, scegli_Caso);
        break;
        default:
        printf("\nErrore: nessun valore corrisponde ad un array Mano!\n");
    }
}

static void arr_handsort(Mano *ListaMano[], int caso_Mano)
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = i + 1; j < 6; ++j)
        {
            // ordina in base ai punti vita (caso_Mano deve anche essere equivalente a 2),
            // oppure se c'e' l'indice j e' in una posizione in cui l'elemento e' uguale a NULL
            // e non si trova nelle ultime posizioni dell'array, ordinale nelle ultime posizioni
            // allora (caso_Mano deve anche essere equivalente a 1).
            if((ListaMano[i] == NULL && caso_Mano == 1) || (ListaMano[j] != NULL && 
            ListaMano[i]->carta_punti_vita < ListaMano[j]->carta_punti_vita && caso_Mano == 2))
            {
                Mano *temp = ListaMano[i];
                ListaMano[i] = ListaMano[j];
                ListaMano[j] = temp;
            }
        }
    }
    return;
}

static void print_memaddr(int add_Giocatore, int tipo_Add, int pos_Add){
    switch(add_Giocatore){
        case 1: case 4: case 7: case 10:
        if(tipo_Add == _side_mago){
            scan_mage(add_Giocatore);
            print_addr(NULL, NULL, ptrmago, tipo_Add, pos_Add);
            scan_mage(_none_memory);
        }
        break;
        case 2: case 3:
        if(tipo_Add == _side_mano)
            print_addr(arr_Mano1, ptrmazzo, NULL, tipo_Add, pos_Add);
        else if(tipo_Add == _side_campo)
            print_addr(arr_Campo1, ptrmazzo, NULL, tipo_Add, pos_Add);
        break;
        case 5: case 6:
        if(tipo_Add == _side_mano)
            print_addr(arr_Mano2, ptrmazzo, NULL, tipo_Add, pos_Add);
        else if(tipo_Add == _side_campo)
            print_addr(arr_Campo2, ptrmazzo, NULL, tipo_Add, pos_Add);
        break;
        case 8: case 9:
        if(tipo_Add == _side_mano)
            print_addr(arr_ManoAvv, ptrmazzo, NULL, tipo_Add, pos_Add);
        else if(tipo_Add == _side_campo)
            print_addr(arr_CampoAvv, ptrmazzo, NULL, tipo_Add, pos_Add);
        break;
        case 11: case 12:
        if(tipo_Add == _side_mano)
            print_addr(arr_Mano2Avv, ptrmazzo, NULL, tipo_Add, pos_Add);
        else if(tipo_Add == _side_campo)
            print_addr(arr_Campo2Avv, ptrmazzo, NULL, tipo_Add, pos_Add);
        break;
        default:
        printf("\nErrore: nessun valore corrisponde ad un indirizzo da stampare!\n");
    }
}

static void print_addr(Mazzo *add_ManoCampo[], Mazzo *add_Mazzo, Mago *add_Mago, int tipo_Arr, int sel_Arr){
    int len_Arr = -1;

    if(tipo_Arr == _side_mano)
        len_Arr = 6;
    else if(tipo_Arr == _side_campo)
        len_Arr = 4;

    if(tipo_Arr == _side_mago && add_Mazzo == NULL){
        if(add_Mago == NULL)
            printf(CLR DRK_GRY"\n");
        else
            printf(CLR"\n");

        printf(ITA "add: %p"CLR, add_Mago);
    }
    else if(tipo_Arr == _side_mago && add_Mago == NULL){
        if(add_Mazzo == NULL)
            printf(CLR DRK_GRY"\t");
        else
            printf(CLR"\t");
        
        printf(ITA "add: %p\n"CLR, add_Mazzo);
    }

    if(len_Arr > -1 && tipo_Arr != _none_side){
        printf("\n");
        for(int i = 0; i < len_Arr; i++){
            if(i == sel_Arr || sel_Arr < 0){
                if(add_ManoCampo[i] == NULL)
                printf(CLR DRK_GRY"\n\t\t\t\t\t\t\t");
                else
                printf(CLR "\n\t\t\t\t\t\t\t");

                if(tipo_Arr == _side_mano)
                    printf(ITA"Mano[%d]: ", i);
                else if(tipo_Arr == _side_campo)
                    printf(ITA"Campo[%d]: ", i);

                printf("%p"CLR, add_ManoCampo[i]);
            }
        }
    }
    return;
}

static void print_gameout(int passa_Vincitore){
    clear_ansi();
    if(event == _print_beginning){
        scan_mage(_Mago);
        printf(BOL CYN ITA "\t   %s\n" CLR, ptrmago->nome);
        if(mode != _1versus1 && mago[2] != NULL){
            scan_mage(_Mago2);
            printf(BOL CYN ITA "\n\t%s\n" CLR, ptrmago->nome);
        }
        printf(YEL"                                                    \n");
        printf("\t     @@@@@            @@@     @@  @@@@@@   @@     \n");
        printf("\t       @@@@            @&   @@&      @@@@@@       \n");
        printf("\t        @@@@          @@   @@@         @@@        \n");
        printf("\t         @@@@        @@    @@@&       *@@@@@/     \n");
        printf("\t          @@@@      @@     @@@@@@@@@@@@@@@@@@@@   \n");
        printf("\t           @@@@    @@        @@@@&         #@@@   \n");
        printf("\t            @@@@  @@                        @@@   \n");
        printf("\t             @@@@@@                        *@@    \n");
        printf("\t              @@@@          @@@@@         ,@@     \n");
        printf("\t               @@          @@@@@@@@@@@@@@@        \n");
        printf("\t                          @@                      \n");
        printf("\t                                                  \n"CLR);
        printf(ITA"\t                      Contro...                   \n"CLR);
        scan_mage(_MagoAvv);
        printf("\t\t\t\t\t\t");
        if(mode != _1versus1)
            printf("   ");
        printf(BOL CYN ITA "%s\n" CLR, ptrmago->nome);
        if(mode != _1versus1 && mago[3] != NULL){
            scan_mage(_Mago2Avv);
            printf(BOL CYN ITA "\n\t\t\t\t\t\t%s\n" CLR, ptrmago->nome);
        }
        scan_mage(_none_memory);
        printf(CLR ITA "");
        set_delayclock(1, 4);
    }
    else if(event == _print_ending){
        if(passa_Vincitore == 1){
            printf(GRN"\n   /@@@      @@...@@@@  @@@@@@@@@@@@(@@@@@@@@@@@@  &@@...@@@@@@@@@@@@@@@@&  ...@@@@     @/. @@      \n");
            printf("     @@@    .@    &@ @  ,    @@@    ..    @@      @@       &@@  @@@    @@@     @@@@    /@   @@&          \n");
            printf("      @@(  (@     &@ @       @@@          @@      @@        @@@ @@@@//@@@      @@@@   .@@@@@@@@          \n");
            printf("       &@@@@      &@ @       @@@          @@      @@@       @@  @@@  @@@@      @@@@   @/     @@&         \n");
            printf("        @@@       &@ @       @@@          @@       @@@@.   @@   @@@    @@@     @@@@  ,@       @@         \n"CLR);
            if(mode == _1versus1)
                printf(ITA"\n\t\t\t\t\tHai vinto la battaglia!\n"CLR);
            else if(mode == _2versus2)
                printf(ITA"\n\t\t\t\t\tHa vinto la tua quadra!\n"CLR);
        }
        else if(passa_Vincitore == 2){                                                                                                    
            printf(RED"\n    &@*(@@#@.  @@**@@@## ,@&***@@@  /@*       @ *#@@@*&@#/@@@  @@@@@@@@@@&&@@@@@@@@@@*   @* /@@     \n");
            printf("   @@.   &@  @@@        @@       @@  @@@@     @   &@, ,@  @@@      #@@         @@       @@   @@       \n");
            printf("   /@@@@@@@@@@@@        @@       @@@ @  *@@@  @   @@   @  @@@      #@@         @@       @@@@@@@(      \n");
            printf("           /@ @@@       @@@,     @@  @     &@@@   @@      @@@      #@@         @@      @      @@      \n");
            printf("   *@@@@  @@    @@@@@@@@  &@@@@@@  @@@@@&     @  /@@@@@@@@@@@@@  @@@@@@@    &@@@@@@@  @@@*   @@@@@    \n"CLR);
            if(mode == _1versus1)
                printf(ITA"\n\t\t\t\t\tHai perso la battaglia.\n"CLR);
            else if(mode == _2versus2)
                printf(ITA"\n\t\t\t\tHa vinto la squadra avversaria.\n"CLR);
        }
        else if(passa_Vincitore == 3){
            printf(YEL"\n  @@(  @//@@@#     @/ @@   @@@(#@@@@@@      @@@@@@  @   @@//@@@      @@//@@@&  ,/@@@@@    (@@//@@@&     \n");
            printf("     @@     .@@   @    @@     @@    *@@    @     /@.  @@           @@             *@*@  ,@(       @@      \n");
            printf("     @@      @@  @@@@@@@@     @@&  &@@    @@@@@@@@@@@@@        @@  @@       @@     @*@  @@        (@@     \n");
            printf("     @@@    @&   @      @@    @@   @@     @@&       @ @@        @@ @@        &@/   @*@  @@@       (@@     \n");
            printf("     @@         @       @@    @@    @@@    @@@@     @  @@@@    &@   @@@@    &@,    @*@   &@@&    &@.      \n");
            printf("                                                    @                                                     \n"CLR);
            if(mode == _1versus1)
                printf(ITA"\n\tEntrambi i giocatori hanno la stessa vita... la partita rimane in pareggio!\n"CLR);
            else if(mode == _2versus2)
                printf(ITA"\nL'ultimo giocatore rimasto nella tua squadra ha la stessa vita della squadra opposta... la partita rimane in pareggio!\n"CLR);
        }
        
        set_delayclock(1, 5);
        
        clear_ansi();
        delete_allmem();
        printf(BOL"\nLa memoria allocata dinamicamente e' stata deallocata con successo.\n"CLR);
        printf("\n\nPremi un tasto qualsiasi per continuare...\n");
        getchar();
    }
    scan_mage(_none_memory); // assegna *ptrmazzo a NULL.
    return;
}

static void print_playername(int passa_Titolo, int sel_ManoMazzo, int stile_titolo){
    if(stile_titolo != 0)
        printf(ONG ITA "");
    switch (passa_Titolo){
        case 1:
        printf("Mago"CLR); 
        break;
        case 2:
        if(sel_ManoMazzo == _side_mano)
            printf("Mano del Mago"CLR);
        else if(sel_ManoMazzo == _side_campo)
            printf("Campo del Mago"CLR);
        break;
        case 3:
        printf("Mazzo del Mago"CLR);
        break;
        case 4:
        printf("Mago 2"CLR);
        break;
        case 5:
        if(sel_ManoMazzo == _side_mano)
            printf("Mano del Mago 2"CLR);
        else if(sel_ManoMazzo == _side_campo)
            printf("Campo del Mago 2"CLR);
        break;
        case 6:
        printf("Mazzo del Mago 2"CLR);
        break;
        case 7:
        printf("Mago Avversario"CLR);            
        break;
        case 8:
        if(sel_ManoMazzo == _side_mano)
            printf("Mano del Mago Avversario"CLR);
        else if(sel_ManoMazzo == _side_campo)
            printf("Campo del Mago Avversario"CLR);
        break;
        case 9:
        printf("Mazzo del Mago Avversario"CLR);
        break;
        case 10:
        printf("Mago 2 Avversario"CLR);
        break;
        case 11:
        if(sel_ManoMazzo == _side_mano)
            printf("Mano del Mago 2 Avversario"CLR);
        else if(sel_ManoMazzo == _side_campo)
            printf("Campo del Mago 2 Avversario"CLR);
        break;
        case 12:
        printf("Mazzo del Mago 2 Avversario"CLR);
        break;
        default:
        printf("\nErrore: nessun output Mazzo da stampare!");
    }
    if(stile_titolo != 0)
        printf("\n"); // vai a capo, se viene stampato come "blocco", ovvero se viene chiamata questa funzione dalla funzione combatti, e non dalla funzione mem_menu.
    return;
}

// Funzione static void che mostra la classe di un determinato Mago allocato in memoria.
static void print_playerdeck(char **tipo_Classe, int passa_Stampa, int scegli_Output)
{
    if (scegli_Output == 1)
    { // mago
        switch (passa_Stampa)
        {
        case -1:
            *tipo_Classe = "";
        case tenebre:
            *tipo_Classe = "Tenebre";
            break;
        case vita:
            *tipo_Classe = "Vita";
            break;
        case luce:
            *tipo_Classe = "Luce";
            break;
        default:
            printf("\nErrore: il valore passato non corrisponde a nessun tipo di Mago!\n");
        }
    }
    else if (scegli_Output == 2)
    { // mazzo
        switch (passa_Stampa)
        {
        case -1:
            *tipo_Classe = "";
        case creatura:
            *tipo_Classe = "Creatura";
            break;
        case rimuovi_creatura:
            *tipo_Classe = "Rim. Creatura";
            break;
        case infliggi_danno:
            *tipo_Classe = "Infliggi Danno";
            break;
        case guarisci_danno:
            *tipo_Classe = "Guarisci Danno";
            break;
        default:
            printf("\nErrore: il valore passato non corrisponde a nessun tipo di Carta!\n");
        }
    }
}
// ------------
// Funzione static void che individua in quale punto e' stata richiesta l'assegnazione di un indirizzo
// di memoria associato alla selezione in input del giocatore, ai puntatori globali ptr_selMago/ptr_selCarta,
// oppure ptr_tarMago/ptr_tarCarta.
static void check_gamefieldlayer(int *passa_tipoGiocatore, int *passa_Lato, int input_SelTar){
    if(input_SelTar == 22){
        // il giocatore ha inserito l'input dal valore 22, e' il mago avversario.
        *passa_tipoGiocatore = avversario; 
        *passa_Lato = _side_mago; // assegnazione che serve a far capire in quale layer esattamente assegnare il valore da "tradurre".
    }
    else if(input_SelTar >= 16 && input_SelTar <= 21){
        // il giocatore ha inserito un input dal valore 21 al valore 16, e' una carta nella mano avversaria.
        *passa_tipoGiocatore = avversario+1;
        *passa_Lato = _side_mano; //
    }
    else if(input_SelTar >= 12 && input_SelTar <= 15){
        *passa_tipoGiocatore = avversario+1;
        *passa_Lato = _side_campo;
    }
    else if(input_SelTar >= 8 && input_SelTar <= 11){
        // nel turno corrente.
        *passa_tipoGiocatore = giocatore+1;
        *passa_Lato = _side_campo;
    }
    else if(input_SelTar >= 2 && input_SelTar <= 7){
        *passa_tipoGiocatore = giocatore+1;
        *passa_Lato = _side_mano;
    }
    else if(input_SelTar == 1){
        *passa_tipoGiocatore = giocatore;
        *passa_Lato = _side_mago;
    }
    return;
}

// Funzione static void che mostra i cosiddetti numeri in stile "battaglia navale", ogni strato,
// ovvero layer dei cosiddetti output del campo di battaglia, mostrera' dei numeri da 1 a 22,
// intuitivamente associabili al mago/carta da selezionare.
static void sel_targetoutput(int passa_Giocatore, int *passa_selTar[2], int *passa_Check, int passa_Layer, int passa_Tipo){
    switch(passa_Giocatore){
        case 1: case 4: case 7: case 10:
        // passa_Tipo e fun_print devono essere assegnate a membri i quali soddisfano alcune di queste numerose condizioni per poter
        // far vedere gli input al giocatore, serve proprio per evitare che il giocatore riceva in output un qualcosa che non voleva vedere.
        if(passa_Tipo == _side_mago && (fun_print == _show_all_selections || fun_print == _show_dmg_selections || fun_print == _show_only_creatures_dmg 
        || fun_print == _show_heal_selections || fun_print == _show_creature_summonings))
            validate_gamefield(NULL, passa_selTar, &*passa_Check, passa_Layer, passa_Tipo); // passa l'input e il layer per parametri in questa funzione.
        break;
        case 2: case 3:
        if(fun_print == _show_all_selections || fun_print == _show_dmg_selections || fun_print == _show_only_creatures_dmg 
        || fun_print == _show_heal_selections || fun_print == _show_creature_summonings){
            if(passa_Tipo == _side_mano)
                validate_gamefield(arr_Mano1, passa_selTar, &*passa_Check, passa_Layer, passa_Tipo); // passa l'input e il layer per parametri in questa funzione.
            else if(passa_Tipo == _side_campo)
                validate_gamefield(arr_Campo1, passa_selTar, &*passa_Check, passa_Layer, passa_Tipo);
        }
        break;
        case 5: case 6:
        if(fun_print == _show_all_selections || fun_print == _show_dmg_selections || fun_print == _show_only_creatures_dmg 
        || fun_print == _show_heal_selections || fun_print == _show_creature_summonings){
            if(passa_Tipo == _side_mano)
                validate_gamefield(arr_Mano2, passa_selTar, &*passa_Check, passa_Layer, passa_Tipo);
            else if(passa_Tipo == _side_campo)
                validate_gamefield(arr_Campo2, passa_selTar, &*passa_Check, passa_Layer, passa_Tipo);
        }
        break;
        case 8: case 9:
        if(fun_print == _show_all_selections || fun_print == _show_dmg_selections || fun_print == _show_only_creatures_dmg 
        || fun_print == _show_heal_selections || fun_print == _show_creature_summonings){
            if(passa_Tipo == _side_mano)
                validate_gamefield(arr_ManoAvv, passa_selTar, &*passa_Check, passa_Layer, passa_Tipo);
            else if(passa_Tipo == _side_campo)
                validate_gamefield(arr_CampoAvv, passa_selTar, &*passa_Check, passa_Layer, passa_Tipo);
        }
        break;
        case 11: case 12:
        if(fun_print == _show_all_selections || fun_print == _show_dmg_selections || fun_print == _show_only_creatures_dmg 
        || fun_print == _show_heal_selections || fun_print == _show_creature_summonings){
            if(passa_Tipo == _side_mano)
                validate_gamefield(arr_Mano2Avv, passa_selTar, &*passa_Check, passa_Layer, passa_Tipo);
            else if(passa_Tipo == _side_campo)
                validate_gamefield(arr_Campo2Avv, passa_selTar, &*passa_Check, passa_Layer, passa_Tipo);
        }
        break;
        default:
        printf("\nErrore: nessun valore corrisponde ad un valore da mostrare come input valido!\n");
    }
    return;
}

// Funzione static void fondamentale (e' stata anticipata piu' volte tra i commenti di questo file sorgente), senza le istruzioni
// di questa funzione void, non sarebbe possibile per il giocatore eseguire alcun tipo di azione sul campo di battaglia.
// Si occupa di far vedere in output cosa puo' e non puo' selezionare il giocatore sul campo di battaglia.
static void validate_gamefield(Mazzo *passa_Arr[], int *prendi_selTar[2], int *check_Layer, int sel_Layer, int tipo_Layer){
    int len_inizio = 0, len_arr = 0, input_disponibile = -1;

    if(tipo_Layer == _side_mano && tipo_Layer != _side_mago)
        len_arr = 6; // valore assegnato associato alla lunghezza di un array mano.
    else if(tipo_Layer == _side_campo && tipo_Layer != _side_mago)
        len_arr = 4; // valore assegnato associato alla lunghezza di un array campo.

    switch(sel_Layer){
        case 1:
        if(tipo_Layer == _side_mago){
            if((fun_print == _show_all_selections || fun_print == _show_single_address_mem || fun_print == _show_heal_selections)){
                printf(BOL "\n      1  " CLR); // posizione selezionabile relativa al mago che sta giocando nel turno corrente.
                
                // *check_Layer, variabile passata per indirizzo, se il suo valore e' 0, vuol
                // dire che, deve trovare la prima posizione sul campo di battaglia dell'input
                // digitato dal giocatore. Se e' 1, vuol dire che una carta/mago e' stato gia' selezionata/o.
                if(*prendi_selTar[0] == 1 && *check_Layer <= 0){
                    *prendi_selTar[0] = 1; // la prima selezione del mago e' avvenuta con successo perche' e' un input valido.
                    *check_Layer = 1; // prima selezione effettuata, la prossima volta che questa funzione verra' chiamata si occupera' di trovare un'altra posizione.
                }
                else if(*prendi_selTar[1] == 1 && *check_Layer == 1){
                    *prendi_selTar[1] = 1;
                    *check_Layer = 2;
                }
            }
            else if(ptr_selMago != NULL || fun_print == _show_dmg_selections || fun_print == _show_only_creatures_dmg || fun_print == _show_creature_summonings)
                printf(DRK_GRY ITA"\n      x  " CLR); // selezione non disponibile, quindi l'input non sara' valido se e' uguale a 1.
        }
        break;
        case 2:
        len_inizio = 2;
        break;
        case 3:
        len_inizio = 8;
        break;
        case 4:
        len_inizio = 12;
        break;
        case 5:
        len_inizio = 16;
        break;
        case 6:
        if(tipo_Layer == _side_mago){
            if(fun_print == _show_all_selections || fun_print == _show_single_address_mem || fun_print == _show_dmg_selections){
                    printf(BOL "\n      22 " CLR); // selezione disponibile relativa al mago avversario di quel determinato turno.
                if(*prendi_selTar[0] == 22 && *check_Layer != 1){
                    *prendi_selTar[0] = 22;
                    *check_Layer = 1;
                }
                else if(*prendi_selTar[1] == 22 && *check_Layer == 1){
                    *prendi_selTar[1] = 22;
                    *check_Layer = 2;
                }
            }
            else if(ptr_selMago != NULL || fun_print == _show_heal_selections || fun_print == _show_only_creatures_dmg || fun_print == _show_creature_summonings)
                printf(DRK_GRY ITA"\n      x  " CLR); // selezione non disponibile.
        }
        break;
    }

    if(sel_Layer == 2 || sel_Layer == 5)
            printf("\n     ");
    else if(sel_Layer == 3 || sel_Layer == 4)
            printf("\n\t\t\t\t    ");

    if(len_inizio != 0){
        for(int i = 0; i < len_arr; i++){            
            // questi if a cascata si occupano di gestire cosa far vedere di input disponibili
            // sul campo di battaglia e cosa non far vedere.
            if(fun_print == _show_all_selections)
                input_disponibile = 1;
            else if((fun_print == _show_dmg_selections) && ptr_selCarta != NULL 
            && (ptr_selCarta->tipo_carta == 0 || ptr_selCarta->tipo_carta == 2)){
                if(sel_Layer == 2 || sel_Layer == 3 || sel_Layer == 5)
                    input_disponibile = 0;
                else if(sel_Layer == 4){
                    if(passa_Arr[i] == NULL)
                        input_disponibile = 0;
                    else if(ptr_selCarta != NULL && ptr_selCarta != passa_Arr[i])
                        input_disponibile = 1;
                }
            }
            else if(fun_print == _show_heal_selections && ptr_selCarta != NULL && ptr_selCarta->tipo_carta == 3){
                if(sel_Layer == 2 || sel_Layer == 4 || sel_Layer == 5)
                    input_disponibile = 0;
                else if(sel_Layer == 3 && passa_Arr[i] != NULL && passa_Arr[i]->tipo_carta == 0)
                    input_disponibile = 1;
                else if(sel_Layer == 3 && passa_Arr[i] != NULL && (passa_Arr[i]->tipo_carta == 1 || passa_Arr[i]->tipo_carta == 2 
                || passa_Arr[i]->tipo_carta == 3))
                    input_disponibile = 0;
                else if(sel_Layer == 3 && passa_Arr[i] == NULL)
                    input_disponibile = 0;
            }
            else if(fun_print == _show_only_creatures_dmg && ptr_selCarta != NULL && ptr_selCarta->tipo_carta == 1){
                if(sel_Layer == 1 || sel_Layer == 2 || sel_Layer == 3 || sel_Layer == 5 || sel_Layer == 6)
                    input_disponibile = 0;
                else if(sel_Layer == 4){
                    if(passa_Arr[i] != NULL && passa_Arr[i]->tipo_carta == 0)
                        input_disponibile = 1;
                    else if(passa_Arr[i] == NULL || passa_Arr[i]->tipo_carta != 0)
                        input_disponibile = 0;
                }
            }
            else if(fun_print == _show_creature_summonings && ptr_selCarta != NULL && ptr_selCarta->tipo_carta == 0){
                if(sel_Layer == 1 || sel_Layer == 2 || sel_Layer == 4 || sel_Layer == 5)
                    input_disponibile = 0;
                else if(sel_Layer == 3){
                    if(passa_Arr[i] == NULL)
                        input_disponibile = 1;
                    else
                        input_disponibile = 0;
                }
            }

            if(ptr_selCarta != NULL && ptr_selCarta == passa_Arr[i])
                input_disponibile = 0;

            if(input_disponibile == 1){
                if(len_inizio >= 10)
                    printf(BOL" %d " CLR, len_inizio);
                else if(len_inizio < 10)
                    printf(BOL" %d  " CLR, len_inizio);
            }
            else if(input_disponibile == 0)
                printf(DRK_GRY ITA" x  " CLR);


            if(len_inizio == *prendi_selTar[0] && *check_Layer == 0 && input_disponibile == 1){
                *prendi_selTar[0] = i;
                *check_Layer = 1;
                input_disponibile = 0;
            }
            else if(len_inizio == *prendi_selTar[1] && *check_Layer == 1 && input_disponibile == 1){
                *prendi_selTar[1] = i;
                *check_Layer = 2;
                input_disponibile = 0;
            }
   
            if(sel_Layer == 2 || sel_Layer == 5)
                printf("\t\t     ");
            else if(sel_Layer == 3 || sel_Layer == 4){
                printf("\t    ");
            }            
            len_inizio++;
        }
    }
    // se viene richiamata questa funzione al momento che sel_Layer, ovvero il blocco associato
    // all'output del primo mago, vale 1, ma sono state gia' trovate entrambe le posizioni da selezionare
    // sul campo di battaglia, allora sfrutta questo ultimo controllo, per resettare queste variabili
    // passate per indirizzo.
    if(sel_Layer == 1){
        if(*check_Layer == 0)
            *prendi_selTar[0] = -1;
        else if(*check_Layer == 1)
            *prendi_selTar[1] = -1;
    }
    return;
}
// Funzione static void che viene chiamata in combatti, fa cambiare l'output dei blocchi
// in cui sono rappresentati in output sia i maghi che gli array mano/campo, fa cambiare
// il loro modo di vederli.
static void print_fieldtable(int Layer, int passa_posSel, int passa_Target){
    switch(Layer){
        case 1:
        if(sel_print == _highlight_mem && select_player == avversario && select_handdeck == _side_mago)
            highlight_mem(select_player, select_handdeck, passa_posSel);
        else
            print_mem(avversario, _side_mago, 0);
        break;
        case 2:
        if(sel_print == _highlight_mem && select_player == avversario+1 && select_handdeck == _side_mano)
            highlight_mem(select_player, select_handdeck, passa_posSel);
        else
            print_mem(avversario+1, _side_mano, 0);
        break;
        case 3:
        if(sel_print == _highlight_mem && select_player == avversario+1 && select_handdeck == _side_campo)
            highlight_mem(select_player, select_handdeck, passa_posSel);
        else
            print_mem(avversario+1, _side_campo, 0);
        break;
        case 4:
        if(sel_print == _highlight_mem && select_player == giocatore+1 && select_handdeck == _side_campo)
            highlight_mem(select_player, select_handdeck, passa_posSel);
        else
            print_mem(giocatore+1, _side_campo, 0);
        break;
        case 5:
        if(sel_print == _highlight_mem && select_player == giocatore+1 && select_handdeck == _side_mano)
            highlight_mem(select_player, select_handdeck, passa_posSel);
        else
            print_mem(giocatore+1, _side_mano, 0);
        break;
        case 6:
        if(sel_print == _highlight_mem && select_player == giocatore && select_handdeck == _side_mago)
            highlight_mem(select_player, select_handdeck, passa_posSel);
        else       
            print_mem(giocatore, _side_mago, 0);
        break;
        default:
        printf("\nErrore: nessun valore corrisponde ad un blocco del campo di battaglia da stampare!\n");
    }
    return;
}

// Funzione static void chiamata da combatti che si occupa di far passare per indirizzo tutte le variabili
// che servono per trovare dove la posizione sul campo di battaglia di una determinata carta/mago,
// e in game_input chiamato da combatti.
static void print_validchoices(int passa_giocatore, int *passa_SelTar[2], int *passa_checkLayer, int passa_Lato, int passa_Layer){
    if(fun_print == _show_all_selections || fun_print == _show_dmg_selections || fun_print == _show_only_creatures_dmg 
    || fun_print == _show_heal_selections || fun_print == _show_creature_summonings)
        sel_targetoutput(passa_giocatore, passa_SelTar, &*passa_checkLayer, passa_Layer, passa_Lato);
    else if(fun_print == _show_single_address_mem && passa_giocatore == select_player && passa_Lato == select_handdeck)
        print_memaddr(select_player, select_handdeck, *passa_SelTar[0]);
    return;
}
// -------------

static void delete_allmem(void)
{
    // dealloca la memoria della prima struct Mago dinamicamente allocata.
    if(ptrmg_inizio != NULL)
        delete_memdata(NULL, NULL, _Mago, _side_mazzo);
    
    // dealloca la lista struct Mazzo che punta alla struct del primo Mago
    if(ptrmz_inizio[0] != NULL){
        delete_memdata( NULL, NULL, _Mazzo, _side_mazzo);
        scan_mage(_Mago); // aggiorna il puntatore globale *ptrmago.
        scan_deck(-1, NULL, NULL, NULL, _side_mazzo);
        init_handfield(_ManoCampo, _side_mano); // assegna NULL l'array mano associato al primo mago.
        init_handfield(_ManoCampo, _side_campo); // assegna NULL l'array campo associato al primo mago.
    }

    // dealloca la lista struct Mazzo che punta alla struct del secondo Mago
    if(ptrmz_inizio[1] != NULL){
        delete_memdata(NULL, NULL, _MazzoAvv, _side_mazzo);
        scan_mage(_MagoAvv); // aggiorna il puntatore globale *ptrmazzo.
        scan_deck(-3, NULL, NULL, NULL, _side_mazzo);
        init_handfield(_ManoCampoAvv, _side_mano); // assegna NULL l'array mano associato al primo mago.
        init_handfield(_ManoCampoAvv, _side_campo); // assegna NULL l'array campo associato al secondo mago.
    }

    if(mode == _1versus1)
        delete_memdata(NULL, NULL, _Mago, _side_mazzo);
    else if(mode == _2versus2){
        if(ptrmz_inizio[2] != NULL){
            delete_memdata(NULL, NULL, _Mazzo2, _side_mazzo); // dealloca la lista dinamica della struct del secondo Mago della squadra amica.
            scan_mage(_Mago2); // aggiorna *ptrmago all'indirizzo del secondo mago avversario.
            scan_deck(-2, NULL, NULL, NULL, _side_mazzo);
            init_handfield(_ManoCampo2, _side_mano); // assegna a NULL l'array della mano associato al primo mago avversario.
            init_handfield(_ManoCampo2, _side_campo); // assegna a NULL l'array della mano associato al secondo mago avversario.
        }

        if(ptrmz_inizio[3] != NULL){
            delete_memdata(NULL, NULL, _Mazzo2Avv, _side_mazzo); // dealloca la lista dinamica della struct del secondo Mago avversario.
            scan_mage(_Mago2Avv);
            scan_deck(-4, NULL, NULL, NULL, _side_mazzo);
            init_handfield(_ManoCampo2Avv, _side_mano);
            init_handfield(_ManoCampo2Avv, _side_campo);
        }

        delete_memdata(NULL, NULL, _Mago, _side_mazzo); // dealloca il mazzo del secondo mago nella squadra avversaria.
    }
    
    inizializza_mago(); // resetta alcune variabili globali.
    
    scan_mage(-1); // inizializza il puntatore *ptrmago a NULL.

    // resetta al membro _none_memory i tipi enum della struct Memoria_Mago,
    giocatore = _none_memory;
    avversario = _none_memory;
    select_player = _none_memory;

    mode = _none_mode; // resetta al membro _none_mode i tipi enum della struct Evento_Gioco,
    mem_title = _title_visible; // resetta al membro _title_visible i tipi enum della struct Stampa_EvMemFun.
    
    select_handdeck = _none_side; // resetta al membro _none_side questa variabile intera globale.
    fun_print = _none_print_choice; // resetta al membro _none_print_choice i tipi enum della struct Memoria_Mago.

    impostato = false;
    return;
}

static void delete_memdata(Mago **nodo_Mago, Mazzo **nodo_Mazzo, int scelta_Elimina, int tipo_del)
{
    switch (scelta_Elimina)
    {
    case 1:
        // struct Mago di tutti i maghi sia avversari che non, allocati dinamicamente in memoria.
        if(tipo_del == _side_mago){
            while (ptrmg_inizio != NULL)
                elimina_Mago(&ptrmg_inizio, &ptrmg_fine);
        }
        else if(tipo_del == _side_nodoMago && nodo_Mago != NULL)
            elimina_nodoMago(&*nodo_Mago, &ptrmg_inizio, &ptrmg_fine);
    break;
    case 2: case 3:
        // struct Mazzo del primo Mago "amico".
        if(tipo_del == _side_mazzo){
            while (ptrmz_inizio[0] != NULL)
                elimina_Mazzo(&mago[0], &ptrmz_inizio[0], &ptrmz_fine[0]);
        }
        else if(tipo_del == _side_nodoMazzo && nodo_Mazzo != NULL)
            elimina_nodoMazzo(&*nodo_Mazzo, ptrmg_inizio, &ptrmz_inizio[0], &ptrmz_fine[0]);
    break;
    case 4: case 5: case 6:
        // struct Mazzo del secondo Mago "amico".
        if(tipo_del == _side_mazzo){
            while (ptrmz_inizio[2] != NULL)
                elimina_Mazzo(&mago[2], &ptrmz_inizio[2], &ptrmz_fine[2]);
        }
        else if(tipo_del == _side_nodoMazzo && nodo_Mazzo != NULL)
            elimina_nodoMazzo(&*nodo_Mazzo, ptrmg_inizio, &ptrmz_inizio[2], &ptrmz_fine[2]);
    break;
    case 7: case 8: case 9:
        // struct Mazzo del primo Mago avversario.
        if(tipo_del == _side_mazzo){
            while (ptrmz_inizio[1] != NULL)
                elimina_Mazzo(&mago[1], &ptrmz_inizio[1], &ptrmz_fine[1]);
        }
        else if(tipo_del == _side_nodoMazzo && nodo_Mazzo != NULL)
            elimina_nodoMazzo(&*nodo_Mazzo, ptrmg_inizio, &ptrmz_inizio[1], &ptrmz_fine[1]);
    break;
    case 10: case 11: case 12:
        // struct Mazzo del secondo Mago avversario.
        if(tipo_del == _side_mazzo){
            while (ptrmz_inizio[3] != NULL)
                elimina_Mazzo(&mago[3], &ptrmz_inizio[3], &ptrmz_fine[3]);
        }
        else if(tipo_del == _side_nodoMazzo && nodo_Mazzo != NULL)
            elimina_nodoMazzo(&*nodo_Mazzo, ptrmg_inizio, &ptrmz_inizio[3], &ptrmz_fine[3]);
    break;
    default:
        printf("\nErrore: nessun valore corrisponde ad una lista da deallocare!\n");
    }
}

// Funzione static void che da un delay in output, facendo aspettare l'utente del tempo
// prima di chiedere un input dall'output.
static void set_delayclock(int passa_Delay, int len_Hold)
{
    // il valore intero passato per parametri passa_Delay e' un valore che rappresenta la durata di un secondo,
    // piu' il valore e' alto, maggiore sara' il tempo per passare da un secondo a un altro e viceversa.
    // la costante CLOCKS_PER_SEC e' necessaria per individuare il valore di clock per secondo.
    passa_Delay = passa_Delay * CLOCKS_PER_SEC;
    for (int i = 1; i < len_Hold; i++)
    {
        clock_t time_set = passa_Delay + clock();
        while (time_set > clock()) // finche' non raggiunge il valore della funzione clock, la condizione del ciclo e' vera.
            continue;
    }
    return;
}

static void gamemode_menu(void)
{
    char *input_mode = (char *)malloc(LEN); // inizializza l'input
    int ripeti_loop = 0;
    
    clear_ansi();
    do
    {
        printf("\nSeleziona una modalita' di gioco.\n");
        printf("\n1) 1 contro 1\n2) 2 contro 2\n");
        printf("\nPremi q per uscire dalle impostazioni.\n");
        fgets(input_mode, LEN, stdin);
         
        // restituisce un errore in output in caso non sia un input valido, oppure non ci sia alcun tipo di input
        // (esempio: viene premuto enter senza immettere nessun carattere).
        clear_arrbuffer(input_mode, &ripeti_loop, 2);
        
        if (atoi(input_mode) == 1)
        {
            mode = _1versus1; // l'utente ha scelto la modalita' 1 contro 1, allora aggiorna la definizione del tipo enum mode.
            n_mago[0] = 1; // viene assegnato il numero di giocatori "buoni" in partita, nel caso della modalita' 1 contro 1, appunto, e' uno solo.
            n_mago[1] = 1; // viene assegnato il numero di giocatori "cattivi" in partita, 1 in questo caso.
            ripeti_loop = 1; // il valore assegnato e' 1, esce dal ciclo do while.
        }
        else if (atoi(input_mode) == 2)
        {
            mode = _2versus2;
            n_mago[0] = 2;
            n_mago[1] = 2;
            ripeti_loop = 1;
        }
        else if (ripeti_loop != 0 && *input_mode != 'q')
        {
            clear_ansi();
            printf("\nErrore: l'input inserito non e' stato riconosciuto, per favore riprovare!\n");
            ripeti_loop = 0; // ripete il ciclo dopo l'output di errore.
        }
    } while (ripeti_loop != 1 && *input_mode != 'q');
    
    clear_ansi(); // prima di tornare nel menu principale pulisce l'output.
    return;
}
// -------------
static void magedeck_menu(int scegli_Giocatore, int *scegli_Deck)
{
    // dichiarazione ed inizializzazione delle variabili locali...
    int selezione_classe = 0, selezione_deck = 0, input_char = 0, classe_Carta = 0;
    int punti_Vita = 0, temp_classe = 0;
    // stampiamo in output la richiesta di immettere un nome, e la tipologia di classe
    // da utilizzare nel gioco, nel farlo si dichiarerà un array char di 20 elementi...
    char *input_Classe = (char *)malloc(LEN);
    char *nome_Mago = (char *)malloc(LEN);
    char *prendi_Deck = (char *)malloc(LEN);
    // ------------
    // "puliamo" l'output per non creare problemi tra le scanf precedentemente
    // dichiarate e la funzione fgets prossima ad essere chiamata
    clear_ansi();
    do
    {
        // richiesta in input per la creazione del PRIMO GIOCATORE
        // e chiamata di funzione per la creazione della lista dinamica...
        // nome del primo mago
        printf(ITA"\nCome ti chiami mago?\n"CLR);
        printf("\nInserisci il nome da assegnare al mago...\n");
        // si utilizza la funzione fgets per evitare fallace di sicurezza
        // che potrebbero avvenire utilizzando la funzione scanf in questo caso...
        fgets(nome_Mago, LEN, stdin);
        clear_arrbuffer(nome_Mago, &input_char, 1);

        // Verifica se la stringa nome_Mago equivale ad altre stringhe di altri maghi.
        // se entrambi i valori delle condizioni sono 1.
        if(mago[0] != NULL && !(strcmp(mago[0]->nome, nome_Mago)))
            input_char = -1; // se si allora stampa un errore.
        else if(mago[1] != NULL && !(strcmp(mago[1]->nome, nome_Mago)))
            input_char = -1;
        else if(mago[2] != NULL && !(strcmp(mago[2]->nome, nome_Mago)))
            input_char = -1;
        else if(mago[3] != NULL && !(strcmp(mago[3]->nome, nome_Mago)))
            input_char = -1;

        if(input_char == -1){
            clear_ansi();
            printf("\nErrore: nome gia' utilizzato, per favore riprovare.\n");
            input_char = 0; // reseetta il loop allora.
        }
    } while (input_char <= 0);

    
    // chiediamo la classe del primo mago.
    clear_ansi(); // "pulisce" l'output.
    do
    {
        print_playerdeck(NULL, scegli_Giocatore, 3);
        printf(ITA"\nE quale sarebbe la tua classe?\n"CLR);
        printf("1) Tenebre\n2) Vita\n3) Luce\n");
        printf("\nInserisci la classe da assegnare mago...\n");
        fgets(input_Classe, LEN, stdin);
        clear_arrbuffer(input_Classe, NULL, 1);

        temp_classe = atoi(input_Classe);

        if (temp_classe < 1 || temp_classe > 3)
        {
            clear_ansi();
            printf("\nErrore: Classe non valida!\n\n");
            selezione_classe = 0;
        }
        else
        {
            switch (temp_classe)
            {
            case 1:
                temp_classe = tenebre;
                break;
            case 2:
                temp_classe = vita;
                break;
            case 3:
                temp_classe = luce;
                break;
            default:
                printf("\nErrore: nessun input inserito e' associato ad una classe Mago, per favore riprovare!\n");
                break;
            }
            selezione_classe = 1;
        }
    } while (selezione_classe <= 0);
    // chiediamo per il primo mago il numero di carte da inserire dentro la struct Mazzo
    // e riempiremo la mano con 5 carte nella struct Mano
    
    clear_ansi(); // "pulisce" l'output.
    do
    {
        printf(ITA"\nCon quante carte vuoi combattere?\n"CLR);
        printf("\nInserisci il numero di carte che vuoi avere nel deck (il massimo che puoi scegliere e' di 60 carte)\n");
        fgets(prendi_Deck, LEN, stdin);
        
        clear_arrbuffer(prendi_Deck, NULL, 1);
        *scegli_Deck = atoi(prendi_Deck);

        if (*scegli_Deck > 60)
        {
            clear_ansi();
            printf("\nErrore: numero di carte troppo alto!\n\n");
            selezione_deck = 0;
        }
        else if (*scegli_Deck <= 0)
        {
            clear_ansi();
            printf("\nErrore: valore di carte non valido!\n\n");
            selezione_deck = 0;
        }
        else
            selezione_deck = 1;
    } while (selezione_deck <= 0);

    // chiamata di funzione struct, da qui si crea il primo nodo alla struct Mago
    nome_Mago = realloc(nome_Mago, *nome_Mago);
    create_mage(scegli_Giocatore, temp_classe, nome_Mago);

    for (int i = 0; i < *scegli_Deck + 5; i++)
    {
        generate_card(&classe_Carta);
        generate_hp(&punti_Vita, temp_classe);
        create_deckand_hand(1, scegli_Giocatore, classe_Carta, punti_Vita);

        classe_Carta = 0;
        punti_Vita = 0;
    }

    //  ------------
    //  riassegnazione a 0 delle variabili utilizzate nella condizione do_while
    //  di questa funzione
    //  in modo tale da poterla riutilizzare senza dichiarare tante variabili che
    //  pesano in memoria
    
    selezione_classe = 0, selezione_deck = 0;
    return;
}

// Funzione static void che genera casualmente i PV delle carte.
static void generate_card(int *Tipo)
{
    // dichiarazione ed inizializzazione delle variabili locali...
    int prob_tipo = rand() % (100 + 1);
    
    // generazione del numero intero che fara' da probabilita' per sorteggiare
    // il tipo di carta dall'enum Tipo_carta in struct Carta
    // la variabile rand utilizza un range di numeri che esclude sempre
    // la probabilita' che venghi sorteggiato il numero 0.
    if (prob_tipo <= 100 && prob_tipo > 85)
        *Tipo = creatura; // la nuova carta da mettere nella lista a coda ha una sua classe.
    // creatura
    if (prob_tipo <= 85 && prob_tipo > 65)
        *Tipo = rimuovi_creatura;
    // rimuovi_creatura
    if (prob_tipo <= 65 && prob_tipo > 40)
        *Tipo = infliggi_danno;
    // infliggi_danno
    if (prob_tipo <= 40 && prob_tipo > 0)
        *Tipo = guarisci_danno;
    // guarisci_danno
    return;
}

static void generate_hp(int *PV, int Mago)
{
    // generazione del numero intero che determinerà i punti vita
    // di ciascuna singola carta, questa variabile sarà utilizzata
    // per l'assegnazione della variabile carta_punti_vita nella struct Carta.
    *PV = (rand() % 8) + 1;

    switch (Mago)
    {
    case tenebre:
        if (Mago == infliggi_danno)
        {
            *PV = (*PV) * 2;
        }
        break;

    case vita:
        if (Mago == creatura)
        {
            int temp_Creatura = *PV / 2;
            PV = PV + temp_Creatura;
        }
        break;

    case luce:
        if (Mago == guarisci_danno)
        {
            float temp_guarDanno = (*PV) * 3 / 5;

            *PV = (int)temp_guarDanno;
        }
        break;
    default:
        clear_ansi();
        printf("\nErrore: input non riconosciuto, per favore riprovare.\n\n");
        return;
    }
    return;
}

// Funzione static void che mette in coda gli indirizzi di memoria dinamicamente allocati
// creando la lista dinamica relativa ai Maghi sia "buoni" che "cattivi".
static void create_mage(int assegna_Mago, int assegna_Classe, char *assegna_Nome)
{
    switch (assegna_Mago)
    {
    case 1:
        crea_Mago(&mago[0], &ptrmg_inizio, &ptrmg_fine, &assegna_Nome, assegna_Classe); // alloca il primo mago "buono".
        break;
    case 2:
        crea_Mago(&mago[1], &ptrmg_inizio, &ptrmg_fine, &assegna_Nome, assegna_Classe); // alloca il secondo mago "buono".
        break;
    case 3:
        crea_Mago(&mago[2], &ptrmg_inizio, &ptrmg_fine, &assegna_Nome, assegna_Classe); // alloca il primo mago "cattivo".
        break;
    case 4:
        crea_Mago(&mago[3], &ptrmg_inizio, &ptrmg_fine, &assegna_Nome, assegna_Classe); // alloca il secondo mago "cattivo".
        break;
    default:
        printf("\nErrore: non e' stato possibile identificare quale struct Mago inizializzare!\n");
    }
    return;
}

// Funzione static void che mette in coda gli indirizzi di memoria dinamicamente allocati delle
// struct Mazzo, considerate carte che verranno giocate in partita.
static void create_deckand_hand(int posizione_Carta, int assegna_Giocatore, int assegna_Carta, int assegna_PV)
{
    // se questo parametro intero passato per valore vale 1, allora chiama in questo scope
    // delle funzioni static void che allocheranno in una lista struct Mazzo in coda memoria dinamica.
    if (posizione_Carta == 1)
    {
        switch (assegna_Giocatore)
        {
        case 1:
            crea_Mazzo(&mago[0], &ptrmz_inizio[0], &ptrmz_fine[0], assegna_Carta, assegna_PV); // alloca memoria per un nuovo nodo in coda alla struct Mazzo associata al primo mago "buono".
            break;
        case 2:
            crea_Mazzo(&mago[1], &ptrmz_inizio[1], &ptrmz_fine[1], assegna_Carta, assegna_PV); // alloca memoria per un nuovo nodo in coda  alla struct Mazzo associata al secondo mago "buono".
            break;
        case 3:
            crea_Mazzo(&mago[2], &ptrmz_inizio[2], &ptrmz_fine[2], assegna_Carta, assegna_PV); // alloca memoria per un nuovo nodo in coda  alla struct Mazzo associata al primo mago "cattivo".
            break;
        case 4:
            crea_Mazzo(&mago[3], &ptrmz_inizio[3], &ptrmz_fine[3], assegna_Carta, assegna_PV); // alloca memoria per un nuovo nodo in coda  alla struct Mazzo associata al secondo mago "cattivo".
            break;
        default:
            printf("\nErrore: non e' stato possibile identificare in quale struct inizializzare il Mazzo!\n");
        }
    }
    else if (posizione_Carta == 2)
    {
        // puntatore temporaneo alla struct Mazzo, il suo numero di elementi rappresenta il numero totale
        //delle Carte che un giocatore puo' avere nel suo mazzo, piu' le prime cinque carte che sono nella
        // sua mano a inizio partita.
        Mazzo *temp[60 + 5];

        switch (assegna_Giocatore)
        {
        case 1:
            scan_mage(_Mago); // aggiorna *ptrmago.
            scan_deck(_Mazzo, temp, NULL, NULL, _side_mazzo); // aggiorna *ptrmazzo.
            for(int i = 0; i < 5; i++){
                arr_Mano1[i] = temp[i]; // assegna il membro di questo array con l'indirizzo di *ptrmazzo.
                n_mano[0] = n_mano[0] + 1; // incrementa di 1 il numero di carte disponibili nella mano del primo mago "buono".
            }
            break;
        case 2:
            scan_mage(_MagoAvv);
            scan_deck(_MazzoAvv, temp, NULL, NULL, _side_mazzo);
            for(int i = 0; i < 5; i++){
                arr_ManoAvv[i] = temp[i];
                n_mano[1] = n_mano[1] + 1;
            }
            break;
        case 3:
            scan_mage(_Mago2);
            scan_deck(_Mazzo2, temp, NULL, NULL, _side_mazzo);
            for(int i = 0; i < 5; i++){
                arr_Mano2[i] = temp[i];
                n_mano[2] = n_mano[2] + 1;
            }
            break;
        case 4:
            scan_mage(_Mago2Avv);
            scan_deck(_Mazzo2Avv, temp, NULL, NULL, _side_mazzo);
            for(int i = 0; i < 5; i++){
                arr_Mano2Avv[i] = temp[i];
                n_mano[3] = n_mano[3] + 1;
            }
            break;
        default:
            printf("\nErrore: non e' stato possibile identificare in quale struct inizializzare la Mano!\n");
        }
        
        scan_mage(_none_memory);
    }
    return;
}

static void empty_magedeck(int passa_MagoMazzo){
    switch(passa_MagoMazzo){
        case 1: case 2:
        mago[0] = NULL;
        break;
        case 3:
        ptrmz_inizio[0] = NULL;
        break;
        case 4: case 5:
        mago[2] = NULL;
        break;
        case 6:
        ptrmz_inizio[2] = NULL;
        break;
        case 7: case 8:
        mago[1] = NULL;
        break;
        case 9:
        ptrmz_inizio[1] = NULL;
        break;
        case 10: case 11:
        mago[3] = NULL;
        break;
        case 12:
        ptrmz_inizio[3] = NULL;
        break;
        default:
        printf("\nErrore: nessun valore corrisponde ad un Mago oppure Mazzo da svuotare!\n");
    }
    return;
}

static void transfert_card(int partenza_Carta, int pos_Partenza, int pos_Arrivo){
    switch(partenza_Carta){
        case 1: case 2:
        if(pos_Partenza > -1 && pos_Arrivo > -1){
            for(int i = 0; i < 4; i++){
                if(arr_Campo1[i] == NULL && i == pos_Arrivo)
                    arr_Campo1[i] = arr_Mano1[pos_Partenza]; // avviene l'assegnazione di un array campo che prende l'indirizzo dell'array mago associato al primo mago.
            }
            arr_Mano1[pos_Partenza] = NULL; // allora assegna NULL per non vedere informazioni di indirizzi in due array diversi.
        }
        break;
        case 3:
        if(ptrmazzo != NULL && pos_Partenza < 0 && pos_Arrivo < 0){
            for(int i = 0; i < 6; i++){
                if(arr_Mano1[i] == NULL){
                    arr_Mano1[i] = ptrmazzo; // avviene l'assegnazione di un array campo che prende l'indirizzo dell'array mago associato al secondo mago.
                    break;
                }
            }
        }
        break;
        case 4: case 5:
        if(pos_Partenza > -1 && pos_Arrivo > -1){
            for(int i = 0; i < 4; i++){
                if(arr_Campo2[i] == NULL && i == pos_Arrivo)
                    arr_Campo2[i] = arr_Mano2[pos_Partenza]; // avviene l'assegnazione di un array campo che prende l'indirizzo dell'array mago associato al primo mago avversario.
            }
            arr_Mano2[pos_Partenza] = NULL; // allora assegna NULL per non vedere informazioni di indirizzi in due array diversi.
        }
        break;
        case 6:
        if(ptrmazzo != NULL && pos_Partenza < 0 && pos_Arrivo < 0){
            for(int i = 0; i < 6; i++){
                if(arr_Mano2[i] == NULL){
                    arr_Mano2[i] = ptrmazzo;  // avviene l'assegnazione di un array campo che prende l'indirizzo dell'array mago associato al secondo mago avversario.
                    break;
                }
            }
        }
        break;
        case 7: case 8:
        if(pos_Partenza > -1 && pos_Arrivo > -1){
            for(int i = 0; i < 4; i++){
                if(arr_CampoAvv[i] == NULL && i == pos_Arrivo)
                    arr_CampoAvv[i] = arr_ManoAvv[pos_Partenza];
            }
            arr_ManoAvv[pos_Partenza] = NULL;
        }
        break;
        case 9:
        if(ptrmazzo != NULL && pos_Partenza < 0 && pos_Arrivo < 0){
            for(int i = 0; i < 6; i++){
                if(arr_ManoAvv[i] == NULL){
                    arr_ManoAvv[i] = ptrmazzo;
                    break;
                }
            }
        }
        break;
        case 10: case 11:
        if(pos_Partenza > -1 && pos_Arrivo > -1){
            for(int i = 0; i < 4; i++){
                if(arr_Campo2Avv[i] == NULL && i == pos_Arrivo)
                    arr_Campo2Avv[i] = arr_Mano2Avv[pos_Partenza];
            }
            arr_Mano2Avv[pos_Partenza] = NULL;
        }
        break;
        case 12:
        if(ptrmazzo != NULL && pos_Partenza < 0 && pos_Arrivo < 0){
            for(int i = 0; i < 6; i++){
                if(arr_Mano2Avv[i] == NULL){
                    arr_Mano2Avv[i] = ptrmazzo;
                    break;
                }
            }
        }
        break;
        default:
        printf("\nErrore: nessun valore corrisponde ad una carta da trasferire!\n");
    }
    return;
}

static void empty_magehand(int passa_Memoria, int passa_ManoMazzo, int passa_Posizione){
    int len_arr = 0;
    if(passa_ManoMazzo == _side_mano)
        len_arr = 6;
    else if(passa_ManoMazzo == _side_campo)
        len_arr = 4;

    switch(passa_Memoria){
        case 1: case 2: case 3:
        for(int i = 0; i < len_arr; i++){
            if(i == passa_Posizione || passa_Posizione == -1){
                if(passa_ManoMazzo == _side_mano)
                    arr_Mano1[i] = NULL; // le posizioni sono comunque accessibili, ma sono volutamente assegnati NULL, quindi non andra' fuori memoria se viene tentato di accedere ad uno di questi elementi.
                else if(passa_ManoMazzo == _side_campo)
                    arr_Campo1[i] = NULL; // stessa cosa anche per l'array del campo associato al primo mago.
            }
        }
        break;
        case 4: case 5: case 6:
        for(int i = 0; i < len_arr; i++){
            if(i == passa_Posizione || passa_Posizione == -1){
                if(passa_ManoMazzo == _side_mano)
                    arr_Mano2[i] = NULL; // assegnazione a NULL anche per le posizioni di questo array associato al secondo mago.
                else if(passa_ManoMazzo == _side_campo)
                    arr_Campo2[i] = NULL; // stessa cosa anche per l'array del campo associato al secondo mago.
            }
        }
        break;
        case 7: case 8: case 9:
        for(int i = 0; i < len_arr; i++){
            if(i == passa_Posizione || passa_Posizione == -1){
                if(passa_ManoMazzo == _side_mano)
                    arr_ManoAvv[i] = NULL;
                else if(passa_ManoMazzo == _side_campo)
                    arr_CampoAvv[i] = NULL;
            }
        }
        break;
        case 10: case 11: case 12:
        for(int i = 0; i < len_arr; i++){
            if(i == passa_Posizione || passa_Posizione == -1){
                if(passa_ManoMazzo == _side_mano)
                    arr_Mano2Avv[i] = NULL;
                else if(passa_ManoMazzo == _side_campo)
                    arr_Campo2Avv[i] = NULL;
            }
        }
        break;
        default:
        printf("\nErrore: nessun valore corrisponde ad un array Mano oppure Campo da pulire!\n");
    }
    return;
}

// Funzione static void che chiama la funzione che si occupa di pescare una carta, ovvero di copiare
// un nodo di una determinata struct Mazzo, nella mano di un determinato mago.
static void draw_card(int passa_Pesca, int *passa_Vittoria){
    switch(passa_Pesca){
        case 1: case 2: case 3:
        pesca(passa_Pesca, &n_mano[0], &n_deck[0], &*passa_Vittoria);
        break;
        case 4: case 5: case 6:
        pesca(passa_Pesca, &n_mano[2], &n_deck[2], &*passa_Vittoria);
        break;
        case 7: case 8: case 9:
        pesca(passa_Pesca, &n_mano[1], &n_deck[1], &*passa_Vittoria);
        break;
        case 10: case 11: case 12:
        pesca(passa_Pesca, &n_mano[3], &n_deck[3], &*passa_Vittoria);
        break;
        default:
        printf("\nErrore: nessun valore corrisponde ad un carta da pescare!");
    }
    return;
}

static void pesca(int pesca_Mazzo, int *passa_Mano, int *passa_Deck, int *passa_Vincita){
    Mazzo *presta_Mazzo[*passa_Deck];

    switch(pesca_Mazzo){
        case 1: case 2: case 3:
        if(mago[0] != NULL){
            scan_mage(_Mago);
            if(*passa_Mano < 6 && *passa_Deck >= 1){
                scan_deck(_Mazzo, presta_Mazzo, NULL, NULL, _side_mazzo); // aggiorna *ptrmazzo.
                ptrmazzo = presta_Mazzo[0]; // assegnazione dell'indirizzo della testa della struct Mazzo del primo mago.
                sel_handsorted(_ManoCampo, 1);
                transfert_card(_Mazzo, -1, -1);
                *passa_Mano = *passa_Mano + 1; // incrementa il numero di carte presente nella mano.
                *passa_Deck = *passa_Deck - 1; // decrementa il numero di carte presente nel mazzo.
            }
            else if(*passa_Mano >= 6 && *passa_Deck >= 1){
                scan_deck(_Mazzo, presta_Mazzo, NULL, NULL, _side_mazzo);
                ptrmazzo = presta_Mazzo[0];

                elimina_nodoMazzo(&presta_Mazzo[0], ptrmg_inizio, &ptrmz_inizio[0], &ptrmz_fine[0]);
                *passa_Deck = *passa_Deck - 1;
            }
        }
        break;
        case 4: case 5: case 6:
        if(mago[2] != NULL){
            scan_mage(_Mago2);
            if(*passa_Mano < 6 && *passa_Deck >= 1){
                scan_deck(_Mazzo2, presta_Mazzo, NULL, NULL, _side_mazzo);
                ptrmazzo = presta_Mazzo[0];
                sel_handsorted(_ManoCampo2, 1);
                transfert_card(_Mazzo2, -1, -1);
                *passa_Mano = *passa_Mano + 1;
                *passa_Deck = *passa_Deck - 1;
            }
            else if(*passa_Mano >= 6 && *passa_Deck >= 1){
                scan_deck(_Mazzo2, presta_Mazzo, NULL, NULL, _side_mazzo);
                ptrmazzo = presta_Mazzo[0];
                elimina_nodoMazzo(&presta_Mazzo[0], ptrmg_inizio, &ptrmz_inizio[2], &ptrmz_fine[2]);
                *passa_Deck = *passa_Deck - 1;
            }
        }
        break;
        case 7: case 8: case 9:
        if(mago[1] != NULL){
            scan_mage(_MagoAvv);
            if(*passa_Mano < 6 && *passa_Deck >= 1){
                scan_deck(_MazzoAvv, presta_Mazzo, NULL, NULL, _side_mazzo);
                ptrmazzo = presta_Mazzo[0];
                sel_handsorted(_ManoCampoAvv, 1);
                transfert_card(_MazzoAvv, -1, -1);
                *passa_Mano = *passa_Mano + 1;
                *passa_Deck = *passa_Deck - 1;
            }
            else if(*passa_Mano >= 6 && *passa_Deck >= 1){
                scan_deck(_MazzoAvv, presta_Mazzo, NULL, NULL, _side_mazzo);
                ptrmazzo = presta_Mazzo[0];
                elimina_nodoMazzo(&presta_Mazzo[0], ptrmg_inizio, &ptrmz_inizio[1], &ptrmz_fine[1]);
                *passa_Deck = *passa_Deck - 1;
            }
        }
        break;
        case 10: case 11: case 12:
        if(mago[3] != NULL){
            scan_mage(_Mago2Avv);
            if(*passa_Mano < 6 && *passa_Deck >= 1){
                scan_deck(_Mazzo2Avv, presta_Mazzo, NULL, NULL, _side_mazzo);
                ptrmazzo = presta_Mazzo[0];
                sel_handsorted(_ManoCampo2Avv, 1);
                transfert_card(_Mazzo2Avv, -1, -1);
                *passa_Mano = *passa_Mano + 1;
                *passa_Deck = *passa_Deck - 1;
            }
            else if(*passa_Mano >= 6 && *passa_Deck >= 1){
                scan_deck(_Mazzo2Avv, presta_Mazzo, NULL, NULL, _side_mazzo);
                ptrmazzo = presta_Mazzo[0];
                elimina_nodoMazzo(&presta_Mazzo[0], ptrmg_inizio, &ptrmz_inizio[3], &ptrmz_fine[3]);
                *passa_Deck = *passa_Deck - 1;
            }
        }
        break;
        default:
        printf("\nErrore: nessun valore corrisponde ad una carta da pescare!\n");
    }
    scan_deck(_none_memory, NULL, 0, 0, _none_side);

    // chiamata di funzione che rileva il numero carte presenti nel mazzo.
    thereis_deck(pesca_Mazzo, *&passa_Vincita);
    return;
}

static void add_cardtohand(int passa_manoDeck, int tipo_Operazione, int val_Operazione){
    switch(passa_manoDeck){
        case 1: case 4:
        if(tipo_Operazione == 0)
            n_mago[0] = n_mago[0] + val_Operazione;
        else if(tipo_Operazione == 1)
            n_mago[0] = n_mago[0] - val_Operazione;
        break;
        case 7: case 10:
        if(tipo_Operazione == 0)
            n_mago[1] = n_mago[1] + val_Operazione;
        else if(tipo_Operazione == 1)
            n_mago[1] = n_mago[1] - val_Operazione;
        break;
        case 2:
        if(tipo_Operazione == 0)
            n_mano[0] = n_mano[0] + val_Operazione;
        else if(tipo_Operazione == 1)
            n_mano[0] = n_mano[0] - val_Operazione;
        break;
        case 3:
        if(tipo_Operazione == 0)
            n_deck[0] = n_deck[0] + val_Operazione;
        else if(tipo_Operazione == 1)
            n_deck[0] = n_deck[0] - val_Operazione;
        break;
        case 5:
        if(tipo_Operazione == 0)
            n_mano[2] = n_mano[2] + val_Operazione;
        else if(tipo_Operazione == 1)
            n_mano[2] = n_mano[2] - val_Operazione;
        break;
        case 6:
        if(tipo_Operazione == 0)
            n_deck[2] = n_deck[2] + val_Operazione;
        else if(tipo_Operazione == 1)
            n_deck[2] = n_deck[2] - val_Operazione;
        break;
        case 8:
        if(tipo_Operazione == 0)
            n_mano[1] = n_mano[1] + val_Operazione;
        else if(tipo_Operazione == 1)
            n_mano[1] = n_mano[1] - val_Operazione;
        break;
        case 9:
        if(tipo_Operazione == 0)
            n_deck[1] = n_deck[1] + val_Operazione;
        else if(tipo_Operazione == 1)
            n_deck[1] = n_deck[1] - val_Operazione;
        break;
        case 11:
        if(tipo_Operazione == 0)
            n_mano[3] = n_mano[3] + val_Operazione;
        else if(tipo_Operazione == 1)
            n_mano[3] = n_mano[3] - val_Operazione;
        break;
        case 12:
        if(tipo_Operazione == 0)
            n_deck[3] = n_deck[3] + val_Operazione;
        else if(tipo_Operazione == 1)
            n_deck[3] = n_deck[3] - val_Operazione;
        break;
        default:
        printf("\nErrore: nessun valore corrisponde ad una Mano o Deck a cui effettuare un operazione!\n");
    }
    return;
}
// ------------
static void print_magehand(Mano *passa_arr[], int memoria_Mano, int evidenzia_Mano)
{
    int ctr = 0, pass_check = 0;
    char *prendi_tipo;

    if(mem_title == _title_visible)
        print_playername(memoria_Mano, _side_mano, 1);
    // pass_check e' una variabile passata per valore che rappresenta il numero
    // di righe in cui va a capo, per stampare graficamente la carta nell'array mano,
    while (pass_check < 2)
    {
        if (passa_arr[ctr] != NULL && (memoria_Mano-1 != avversario))
        {
            switch (pass_check)
            {
            case 0:
                // Se la condizione ctr = evidenzia_Mano e' vera, stampa lo stesso output con gli stessi valori
                // delle stesse variabili ma in un formato bold, ovvero dando l'idea di "luminosita'",
                // in caso il giocatore decidesse di selezionare un indirizzo di una determinata posizione
                // nell'array mano associato ad un mago.
                if (ctr == evidenzia_Mano)
                {
                    printf(HRED UND BOL"PV: %d"CLR, passa_arr[ctr]->carta_punti_vita);
                    printf("\t\t        ");
                }
                else
                {
                    printf(HRED "PV: %d"CLR, passa_arr[ctr]->carta_punti_vita);
                    printf("\t\t        ");
                }
                break;
            case 1:
                print_playerdeck(&prendi_tipo, passa_arr[ctr]->tipo_carta, 2);
                if (ctr == evidenzia_Mano)
                    printf(YEL UND BOL "%s"CLR, prendi_tipo);
                else
                    printf(YEL "%s"CLR, prendi_tipo);

                if (strlen(prendi_tipo) < 15)
                {
                    printf("\t\t");
                }
                else
                {
                    printf("         ");
                }
                break;
            default:
                printf("\nErrore: nessun valore corrisponde ad una Mano da stampare!\n");
            }
        }
        else if(passa_arr[ctr] != NULL && (memoria_Mano-1 == avversario)){
            // in caso debbano essere stampate le informazioni relative agli indirizzi di un array
            // mano di un mago che in un determinato turno e' un avversario del mago con cui si sta
            // giocando, al posto di mostrare tutti i valori, li nasconde.
            if (pass_check == 0){
                if(ctr == evidenzia_Mano)
                    printf(CLR WHT UND BOL"Mano\t\t\t"CLR);
                else
                    printf(CLR WHT"Mano\t\t\t"CLR);
            }
            if (pass_check == 1){
                if(ctr == evidenzia_Mano)
                    printf(CLR WHT UND BOL"nascosta\t\t"CLR);
                else
                    printf(CLR WHT"nascosta\t\t"CLR);
            }
        }
        else if(passa_arr[ctr] == NULL)
        {
            // se nel caso la posizione dell'array mano ha il valore NULL, allora mostra in output
            // che la slot di quella mano e' vuota.
            if (pass_check == 0){
                if(ctr == evidenzia_Mano)
                    printf(CLR WHT UND"Mano vuota\t\t" CLR);
                else
                    printf(CLR DRK_GRY"Mano vuota\t\t" CLR);
            }
            if (pass_check == 1)
                printf("\t\t\t"CLR);
        }

        if (ctr < 5)
            ctr++;
        else
        {
            ctr = 0;
            pass_check++;
            if(pass_check < 2)
                printf("\n");
        }
    }
    if(fun_print == _show_all_address_mem)
        print_memaddr(memoria_Mano, _side_mano, -1);
    return;
}

static void print_gamefield(Campo *passa_arr[], int mem_Campo, int evidenzia_Campo)
{
    int ctr = 0, pass_check = 0;
    char *prendi_tipo;

    if(mem_title == _title_visible)
        print_playername(mem_Campo, _side_campo, 1);
    while (pass_check < 2)
    {
        if (passa_arr[ctr] != NULL)
        {
            switch (pass_check)
            {
            case 0:
                // mostra tutte le informazioni relativa ai valori degli indirizzi
                // dentro un array campo di un determinato mago.
                if (ctr == evidenzia_Campo)
                    printf(HRED BOL UND "PV: %d\t\t", passa_arr[ctr]->carta_punti_vita);
                else
                {
                    printf(HRED "PV: ");
                    printf("%d\t\t", passa_arr[ctr]->carta_punti_vita);
                }
                break;
            case 1:
                print_playerdeck(&prendi_tipo, passa_arr[ctr]->tipo_carta, 2);
                if (ctr == evidenzia_Campo)
                    printf(YEL UND BOL "%s"CLR, prendi_tipo);
                else
                    printf(YEL "%s"CLR, prendi_tipo);

                printf("\t");
                break;
            default:
                printf("\nErrore: nessun valore corrisponde ad un Campo da stampare!\n");
            }
        }
        else
        {
            if (pass_check == 0){
                if(ctr == evidenzia_Campo)
                    printf(CLR WHT UND"Campo vuoto\t"CLR);
                else
                    printf(CLR DRK_GRY"Campo vuoto\t"CLR);
            }
            if (pass_check == 1)
                printf("\t\t");
        }

        if (ctr < 3)
            ctr++;
        else
        {
            ctr = 0;
            pass_check++;
            if(pass_check < 2)
                printf("\n\t\t\t\t");
        }
    }
    if(fun_print == _show_all_address_mem)
        print_addr(passa_arr, NULL, NULL, _side_campo, -1);
    return;
}

static void gioca_carta(Mazzo *carta_Scelta, Mazzo *carta_Puntata, Mago *mg_Puntato, int *pos_selPuntata[2]){
    switch(carta_Scelta->tipo_carta){
        case 0:
        if(event == _straight_attack){
            if(carta_Puntata != NULL && carta_Puntata->tipo_carta == 0 && type_target == _deal_creature_dmg)
                attacca(carta_Scelta, carta_Puntata, NULL, pos_selPuntata); // attacca la creatura nemica.
            else if(mg_Puntato != NULL && type_target == _deal_mage_dmg)
                attacca(carta_Scelta, NULL, mg_Puntato, pos_selPuntata); // attacca il mago nemico.
            
            // azione associata all'attacco completata, action[2] gli viene assegnato il valore del membro del tipo
            // enum Evento_Gioco, ovvero _completed_action
            action[2] = _completed_action;
        }
        else if(event == _evoke_creature){
            transfert_card(giocatore+1, *pos_selPuntata[0], *pos_selPuntata[1]);
            add_cardtohand(giocatore+1, 1, 1); // decrementa il valore che conta il numero delle carta sul mazzo di 1.
        }
        break;
        case 1:
        if(event == _play_throwaway_card && type_target == _deal_creature_dmg){
            if(carta_Puntata != NULL && carta_Puntata->tipo_carta == 0)
                carta_Puntata->carta_punti_vita = 0;

            if(carta_Puntata->carta_punti_vita == 0){
                empty_magehand(avversario, _side_campo, *pos_selPuntata[1]); // assegnazione a NULL della posizione della carta distrutta sull'array del campo avversario.
                delete_memdata(NULL, &carta_Puntata, avversario+1, _side_nodoMazzo);
            }
            // azione associata all'utilizzo di un effetto carta e' stata completata, pertanto la seconda posizione
            // dell'array action gli viene assegnato il valore _completed_action.
            action[1] = _completed_action;
        }
        break;
        case 2:
        if(event == _play_throwaway_card){
            if(carta_Puntata != NULL && carta_Puntata->tipo_carta == 0 && type_target == _deal_creature_dmg)
                attacca(carta_Scelta, carta_Puntata, NULL, pos_selPuntata); // effetto della carta infliggi_danno attivata contro una creatura.
            else if(mg_Puntato != NULL && type_target == _deal_mage_dmg)
                attacca(carta_Scelta, NULL, mg_Puntato, pos_selPuntata); // effetto della carta infliggi_danno attivata contro un mago.

            action[1] = _completed_action; // azione completata.
        }
        break;
        case 3:
        if(event == _play_throwaway_card){
            if(carta_Puntata != NULL && carta_Puntata->tipo_carta == 0 && type_target == _heal_creature){
                carta_Puntata->carta_punti_vita = carta_Puntata->carta_punti_vita + carta_Scelta->carta_punti_vita; // effetto della carta guarisci_danno attivata contro una creatura.
            }
            else if(mg_Puntato != NULL && type_target == _heal_mage)
                mg_Puntato->mago_punti_vita = mg_Puntato->mago_punti_vita + carta_Scelta->carta_punti_vita; // effetto della carta guarisci_danno attivata contro una creatura.
            
            action[1] = _completed_action; // azione completata.
        }
        break;
        default:
        printf("\nErrore: nessun valore corrisponde ad un tipo carta!\n");
    }
    
    if(event == _play_throwaway_card && carta_Scelta != NULL){                
        empty_magehand(select_player, _side_mano, *pos_selPuntata[0]);
        delete_memdata(NULL, &carta_Scelta, select_player, _side_nodoMazzo); // posizione dell'array mano del giocatore protagonista del turno corrente, aggiornata con l'assegnazione di NULL.

        add_cardtohand(giocatore+1, 1, 1); // decremento del valore che rappresenta il numero di carte presente nella mano del mago che sta giocando in un determinato turno.
    }
    return;
 }

static void attacca(Mazzo *sel_Carta, Mazzo *tar_Carta, Mago *tar_Mago, int *pos_selTar[2]){
    int temp_Vita = 0;
    if(sel_Carta != NULL){
        if(tar_Carta != NULL && tar_Mago == NULL && type_target == _deal_creature_dmg){
            temp_Vita = sel_Carta->carta_punti_vita; // attacca la creatura.

            if(sel_Carta->tipo_carta == 0)
                sel_Carta->carta_punti_vita = sel_Carta->carta_punti_vita - tar_Carta->carta_punti_vita;

            tar_Carta->carta_punti_vita = tar_Carta->carta_punti_vita - temp_Vita;
        }
        else if(tar_Mago != NULL && tar_Carta == NULL && type_target == _deal_mage_dmg){
            temp_Vita = sel_Carta->carta_punti_vita; // attacca il mago.
            
            // se e' una creatura che attacca il mago, allora anche i suoi punti vita decrementato in base a quanto
            // vale il valore intero dei punti vita del mago a cui eseguendo un attacco.
            if(sel_Carta->tipo_carta == 0)
                sel_Carta->carta_punti_vita = sel_Carta->carta_punti_vita - tar_Mago->mago_punti_vita; // assegnazione di un nuovo valore dei punti vita della creatura.

            tar_Mago->mago_punti_vita = tar_Mago->mago_punti_vita - temp_Vita;
        }
    }

    // condizioni if a cascata che deallocano dalla memoria in caso la creatura/mago cui una carta
    // scelta dal giocatore creatura oppure di un altro tipo qualsiasi sul campo di battaglia,
    // il mago/creatura a cui sta effettuando un attacco.
    if(tar_Carta != NULL && tar_Carta->carta_punti_vita <= 0){
        empty_magehand(avversario+1, _side_campo, *pos_selTar[1]); // dealloca la carta nemica dalla lista in coda.
        delete_memdata(NULL, &tar_Carta, target_player, _side_nodoMazzo); // assegnazione a NULL della posizione su un array campo/mano.
    }
    if(sel_Carta != NULL && sel_Carta->tipo_carta == 0 && sel_Carta->carta_punti_vita <= 0){
        empty_magehand(giocatore+1, _side_campo, *pos_selTar[0]);  // dealloca la carta nemica dalla lista in coda.
        delete_memdata(NULL, &sel_Carta, select_player, _side_nodoMazzo); // assegnazione a NULL della posizione su un array campo/mano.
    }
    if(tar_Mago != NULL && tar_Mago->mago_punti_vita <= 0){
        // dealloca il mago da una nodo di una struct Mago.
        empty_magehand(avversario+1, _side_mano, -1);
        empty_magehand(avversario+1, _side_campo, -1);

        scan_mage(avversario);
        delete_memdata(NULL, NULL, avversario+1, _side_mazzo);
        delete_memdata(&ptrmago, NULL, avversario, _side_nodoMago);

        empty_magedeck(avversario+2);
        empty_magedeck(avversario);

        add_cardtohand(avversario, 1, 1);
        
        scan_mage(_none_memory);
        scan_deck(_none_memory, NULL, NULL, NULL, _none_side);
        
        // quando event vale _dead_mage nella funzione combatti verra' stampato in output che il mago
        // avversario in quel determinato turno e' stato sconfitto.
        event = _dead_mage;
    }
    return;
}

static void passa(int *rileva_Vincitore){
    int rand_Turno = 0, pass_Check = 0;

    // in questa funzione ci sono due tipi di assegnazione del nuono turno del prossimo giocatore
    // e del prossimo avversario:
    // uno dentro ad una condizione while, che seleziona casualmente il prossimo mago in base
    // ad un criterio, ovvero che il mago scelto non sia lo stesso giocato nel turno precedente.
    while(pass_Check < 1){
        if(mode == _1versus1)
            rand_Turno = rand() % 2;
        else if(mode == _2versus2)
            rand_Turno = rand() % 4;

        if(rand_Turno == 0){
            if(mago[0] != NULL){
                giocatore = _Mago;
                // giocatore che deve giocare il prossimo turno trovato, alla prossima
                // iterazione se vale 1 si occupa di pescare il prossimo avversario.
                pass_Check++;
            }
        }
        else if(rand_Turno == 1){
            if(mago[1] != NULL){
                giocatore = _MagoAvv; // avversario che deve giocare il prossimo turno trovato.
                pass_Check++; // pass_Check incrementa il suo valore a 1.
            }
        }
        else if(rand_Turno == 2){
            if(mago[2] != NULL){
                giocatore = _Mago2; // giocatore che gioca nel prossimo turno trovato.
                pass_Check++; // alla prossima iterazione del ciclo, trovera' l'avversario.
            }
        }
        else if(rand_Turno == 3){
            if(mago[3] != NULL){
                giocatore = _Mago2Avv;
                pass_Check++;
            }
        }
        
        if(n_mago[0] <= 0){
            *rileva_Vincitore = 2; // i "buoni" sono stati sconfitti, la partita e' persa.
            break;
        }
        else if(n_mago[1] <= 0){
            *rileva_Vincitore = 1; // gli avversari sono stati sconfitti, la partita e' vinta.
            break;
        }

        if(curr_giocatore == giocatore){
            // se il giocatore preso casualmente e' uguale a quello del turno corrente,
            // ripete il ciclo finche' non ne trova uno differente.
            pass_Check = 0;
        }        
    }
    // per evitare di scegliere lo stesso giocatore che ha giocato nel turno precedente,
    // assegna il valore di giocatore a curr_giocatore.
    curr_giocatore = giocatore;

    // sceglie l'avversario in base a chi e' stato scelto casualmente chi e' il mago
    // del prossimo turno.
    if(giocatore == _Mago){
        avversario = _MagoAvv;
        if(mago[1] == NULL)
            avversario = _Mago2Avv;
    }
    else if(giocatore == _Mago2){
        avversario = _Mago2Avv;
        if(mago[3] == NULL)
            avversario = _MagoAvv;
    }
    else if(giocatore == _MagoAvv){
        avversario = _Mago;
        if(mago[0] == NULL)
            avversario = _Mago2;
    }
    else if(giocatore == _Mago2Avv){
        avversario = _Mago2;
        if(mago[2] == NULL)
            avversario = _Mago;
    }

    return;
}