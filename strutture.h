typedef struct Posizione{
    int r,c;//righe e colonne di dove si trova 
}Posizione;
typedef struct Cura{
    int punti_vita; //quanti punti da
    Posizione pos; //cordinate di dove si trova la cura
}Cura;
typedef struct PotenziatoreArma{
    int valorePotenziamento; //quanto incrementa la potenza dell'arma
    Posizione pos; //cordinate di dove si trova il potenziatore
}PotenziatoreArma;
typedef struct Arma{
    int id; //serve per selezionare l'arma all'utente senza dover scrivere il nome
    char*nome_arma; 
    int potenza; //per stabilire chi vince la battaglia
}Arma;
typedef struct Nemico{
    Posizione pos; //dove si trova il nemico nella mappa
    float vita; //quanta vita ha da 1 a 100
    Arma arma; //
}Nemico;
typedef enum {
    TIPO_CURA=2, 
    TIPO_ARMA
} TipoOggetto;
typedef union{
    Cura*cura;
    PotenziatoreArma*potArma;
}OggettoUnion;
typedef struct Item {
    TipoOggetto tipo;
    OggettoUnion*object;
    struct Item* next;
}Item;
typedef struct Giocatore{
    char*nome;
    Posizione pos;
    int vita;
    Item*zaino;
    Arma arma;
}Giocatore;
typedef struct printWarnings{ //stampa tutte le statistiche e gli avvisi al giocatore
    bool close_enemies; //nemici vicini
    int numNemiciVicini; //quanti nemici ci sono nelle vicinanze
    int nemiciRimasti;
}Warnings;
typedef enum{
    CURA=0,
    ARMA,
    DUELLO,
    GAMEOVER
}Disegni;