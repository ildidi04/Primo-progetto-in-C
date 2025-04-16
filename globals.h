#define numeroFileDisegni 4
#define MAX_LUN 100
static int righe;
static int colonne;
static int numMaxOstacoli, numMinOstacoli;//numero di ostacoli
static int minVitaNemici, maxVitaNemici; //range dei punti vita che il nemico possiede in base al livello scelto
static int minCureMappa, maxCureMappa; //range del  numero di cure
static int numero_cure,numero_armi; //numero cure della partita corrente estratto random dal range
static int minArmiMappa, maxArmiMappa; //range del numero di potenziatori di armi nella mappa
static int PotenzaArma;
static int minDannoSubito,maxDannoSubito; //range del danno che può ricevere il giocatore dopo 1 battaglia con un nemico
static int numeroNemici;
static char nomi_armi[2][20]={"Sword","Knife"}; //nomi armi
static char disegno[numeroFileDisegni][20]={"cura.txt","arma.txt","duello.txt","gameOver.txt"}; //nomi dei file con i disegni
static int puntiVitaGiocatore;
int modalità;//in base alla modalità vengono inizializzate in un certo modo tutte le variabili globali
int numOggettiRaccolti=0; //tutti gli oggetti raccolti in un certo momento dal giocatore
Nemico*nemici;
PotenziatoreArma*potArma;
Cura*cure;
Giocatore *giocatore;
int**mappa;
Warnings warnings;
static float cds; //cds= coefficiente di stampa, nome che ho inventato per sapere dove stampare i warnings in base alla grandezza del campo
FILE* disegniascii; //punta ad un file con un disegno ascii  
FILE* fileCommands; //contiene l'indirizzo del file contenente i comandi da stampare



