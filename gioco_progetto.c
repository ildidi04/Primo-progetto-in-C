#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include  <string.h>
#include <time.h>
#include "strutture.h"
#include "globals.h"
#include "disegniascii.h"
#ifdef _WIN32
    #include <windows.h>
    #define CLEAR_SCREEN "cls"
    #define SLEEP(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define CLEAR_SCREEN "clear"
    #define SLEEP(ms) usleep((ms) * 1000) // Converti millisecondi in microsecondi
#endif



int scegliModalità();
void stampaIstruzioni(FILE*file);//stampa da file le istruzioni
int** creaMappa();
void generaOstacoli(int**mappa);
Nemico* generaNemici(int**mappa); //ritorna l'indirizzo di dove si trova l'array di tipo Nemico
void stampaMappa();
void inizializzaVarGlobali(int mode);//in base alla modalità scelta avrò determinati valori
Cura* generaCure(int**mappa); //fanno aumentare la vita SOLO del personaggio principale e NON dei nemici
PotenziatoreArma* generaPotArma(int **mappa); //fanno aumentare la potenza delle armi SOLO del personaggio principale e NON dei nemici
int raccogliOggetto (Item**firstItem,OggettoUnion*newOggetto,TipoOggetto tipo);
bool printOggettiZaino(Item*head);
int rimuoviItem(Item**primoElementoLista,int value,TipoOggetto tipoOggetto_da_rimuovere);
Giocatore* generaGiocatore(int**mappa);
int contaNemici(Nemico*nemici);
int spostati(char simboloDiDirezione);//se lo spostamento avviene ritorna vero, falso nel caso la cella si occupata da nemico o un ostacolo
char* inserisciPercorso(char*percorso);
bool salvaMossa(char*percorso); //salva uno o più caratteri ogni volta che viene chiamata al fine di avere una cronologia di tutti gli spostamenti
int oggettoPresente();
void* returnOggetto(TipoOggetto tipo);
int* controllaNemiciVicini();//conta i nemici vivi attorno al giocatore
bool uccidiNemico(int nemico); //se uccido il nemico nel duello ritorna true altrimenti false
void stampaLegenda(); //stampa per l'utente la spiegazione dei comandi per giocare
void printGiocatore(); //stampa tutte le statistiche del giocatore per ogni mossa che fa, allineate con la mappa
bool checkSubstituteWeapon(); //controlla se nell'attuale posizione del giocatore c'è un'arma da sostituire
int executeCommand(char*commands);//controlla se l'input contiene uno dei comandi riconosciuti dal gioco che non siano di spostamento del giocatore (<,v,^,>)
bool chooseItem(TipoOggetto type,int value);//controlla che l'oggetto che vuole consumare il giocatore sia presente nella lista
bool consumeItem(Item*obj); //consuma the item if the player doesn't have already full health or maxed weapon
TipoOggetto getInputItemType(char*command); //understand if the user asked for a Medication or Weapon Enhancer
int getInputItemValue(char*command); //get the value of the item the user asked for
void printWarnings();//this warnings are printed if the health is low and/or the weapon is weak
void checkWeapons();// if the weapons of the player and enemies are under 0 of power they are going to be reset to 0
void freeAll(); //deallocatte all the dynamic memory allocated of this match
int main(){
    char answernewGame[20]; //ask the player if wants to play another match
    bool askAgain=true; // in case the player types bad his decision to play the new match or not
    bool newGame=true;
    while(newGame==true){
        Giocatore *player;
        char*percorso;//contiene l'indirizzo di dove si trova l'array allocato dinamicamente nella funzione
        system(CLEAR_SCREEN);
        srand(time(NULL));
        modalità=scegliModalità(); 
        int**mappa=NULL;
        mappa=creaMappa();
        generaOstacoli(mappa);
        cure=generaCure(mappa);
        nemici=generaNemici(mappa);
        potArma=generaPotArma(mappa);
        giocatore=generaGiocatore(mappa);
        warnings.nemiciRimasti=contaNemici(nemici);
        inserisciPercorso(percorso);
        bool sopravvisuto=true;
        OggettoUnion obj[numero_armi+numero_cure];//tutti gli oggetti da raccogliere
        int conta=0;
        stampaMappa();
        while(sopravvisuto==true && warnings.nemiciRimasti>0 ){
            printf("[:]>   ");
            percorso=inserisciPercorso(percorso);
            int contSegno=0;
            int move=0;
            int lunPercorso=strlen(percorso);
            int*nemiciVicini;
            bool itemRaccolto; //variabile che segnale se è stato raccolto un item per ogni spostamento
            warnings.close_enemies=false;
            if(executeCommand(percorso)==1){}//se è stato eseguito un comando
            else if(executeCommand(percorso)==0){ //se è stato richiesto uno spostamento
                while(contSegno<lunPercorso && move==0 && sopravvisuto==true){ //esco dal ciclo se ho finito il percorso, incontro un ostacolo o muoio
                    warnings.numNemiciVicini=0;
                    move=spostati(percorso[contSegno]);
                    nemiciVicini=controllaNemiciVicini(&warnings.numNemiciVicini);
                    if(warnings.numNemiciVicini>0){
                        warnings.close_enemies=true;
                    }
                    if(move==4){//se c'è un nemico nella celle dove si vuole spostare il giocatore (4
                        int nemiciAffrontati=0;
                        if(warnings.numNemiciVicini>1){
                            printf("If you want to proceed in that direction you have to face all the %d enemies close !\n",warnings.numNemiciVicini);
                        }else if(warnings.numNemiciVicini==1){
                            printf("If you want to proceed in that direction you have to face the enemy in front of you!\n");
                        }
                        while(sopravvisuto==true && nemiciAffrontati<warnings.numNemiciVicini){
                            checkWeapons(); 
                            printDisegno(disegno[DUELLO],0);
                            printf("\nFight in progress...\n");
                            SLEEP(4000);
                            if(uccidiNemico(nemiciVicini[nemiciAffrontati])==false){
                                sopravvisuto=false;
                                printf("The enemy killed you :-c \n");
                            }else{
                                printf("Enemy killed!.\n");
                                warnings.nemiciRimasti--;
                                nemiciAffrontati++;
                            }
                            SLEEP(2000);
                        }
                        free(nemiciVicini); //dealloco array 
                        if(sopravvisuto==true){//se sono sopravvissuto vuol dire che ho ucciso tutti i nemici vicini
                            warnings.close_enemies=false; //quindi nemici vicini diventa false
                        }
                    }
                    if(move==0){ //se non ci sono impedenze  controllo se ci sono oggetti da raccogliere
                        itemRaccolto=false;
                        if(oggettoPresente()==2){ // se l'oggetto è una cura
                            obj[numOggettiRaccolti].cura=(Cura*)returnOggetto(TIPO_CURA); //ritorna dove si trova l'oggetto cura in memoria(nell'array di cura)
                            raccogliOggetto(&giocatore->zaino,&obj[numOggettiRaccolti],TIPO_CURA);
                            mappa[obj[numOggettiRaccolti].cura->pos.r][obj[numOggettiRaccolti].cura->pos.c]=0; //tolgo
                            itemRaccolto=true;
                        }else if(oggettoPresente()==3){ //se l'oggetto è un potenziatore d'arma
                            obj[numOggettiRaccolti].potArma=(PotenziatoreArma*)returnOggetto(TIPO_ARMA);
                            raccogliOggetto(&giocatore->zaino,&obj[numOggettiRaccolti],TIPO_ARMA);
                            mappa[obj[numOggettiRaccolti].potArma->pos.r][obj[numOggettiRaccolti].potArma->pos.c]=0; //tolgo l'arma nella posizione attuale
                            itemRaccolto=true;
                        }
                        if(itemRaccolto==true){
                            numOggettiRaccolti++;
                        }
                    }
                    else{ //se ci sono impedenze
                        if(move==1){ //controllo se l'impedenza sia un ostacolo
                            printf("You have an obstacle in front of you.\n");
                        }
                    }
                    contSegno++;
                    if(move!=0 || itemRaccolto==true){ //se  ci sono ostacoli/nemici oppure ho raccolto qualche oggetto
                        SLEEP(2000); // prima di pulire lo schermo faccio una pausa di n secondi di modo da dare il tempo all'utente di leggere cosa è avvenuto durante lo spostamento
                    }
                    if(move!=-2){
                        giocatore->vita--; //for each movement the player decrease 1 life's point
                        if(giocatore->vita<=0 && sopravvisuto==true){//the second condition to avoid this message in case the player lose life due to a fight and not for too many steps
                            system(CLEAR_SCREEN); 
                            printf("You did too many steps, you just died, the next time be more careful when you choose the paths!\n");
                            Sleep(6000);
                            sopravvisuto=false;
                        }
                        system(CLEAR_SCREEN);
                        stampaMappa();
                        if(lunPercorso>1){
                            Sleep(400);
                        }
                    }
                }
            }else if(executeCommand(percorso)==-1){ //if the user wants to quit the game
                printf("Quitting the match...\n");
                sopravvisuto=false;
                Sleep(1000);
            }
            system(CLEAR_SCREEN); // Pulisce lo schermo 
            stampaMappa();
            conta++;
        }
        
        if(sopravvisuto==true){
            printf("Congratulations! You have passed the ");
            if(modalità==1){
                printf("easy");
            }else if(modalità==2){
                printf("medium");    
            }else{
                printf("hard");
            }
            printf(" mode!\n");
        }
        Sleep(2000);
        system(CLEAR_SCREEN);
        answernewGame[20];
        askAgain=true; // in case the player types bad his decision
        printf("Do you want to start a new game?");
        while(askAgain==true){
            fgets(answernewGame, 20, stdin);
            answernewGame[strcspn(answernewGame, "\n")] = '\0'; // remove newline
            if(strcasecmp(answernewGame,"yes")==0 || strcasecmp(answernewGame,"y")==0 || strcasecmp(answernewGame,"yep")==0 || strcasecmp(answernewGame,"yeah")==0 ){
                newGame=true;
                askAgain=false;
            }else if(strcasecmp(answernewGame,"no")==0 || strcasecmp(answernewGame,"not")==0 || strcasecmp(answernewGame,"nah")==0 || strcasecmp(answernewGame,"nope")==0){
                newGame=false;
                askAgain=false;
            }else{
                printf("Type 'yes','y','yep' or 'yeah' to play a new game or 'no','n', 'nah' or 'nope' to quit>");
                askAgain=true;
            }
        }
        if(newGame==false){
            printDisegno(disegno[GAMEOVER],0);//stampa game over in ascii art, 0 per dire stampa il disegno intero
            Sleep(5000);
        }else{ //if there is a new match we need to deallocate all the memory allocated dynamically
            freeAll(); 
        }
    }
    return 0;
}

void freeAll(){
    for(int i=0;i<righe;i++){
        free(*(mappa+i));//deallocate each row of integers
    }
    free(mappa);//deallocate the array of pointers to each row of integers
    Item* head=giocatore->zaino;
    Item*aus;
    while(head!=NULL){
        aus=head->next;
        free(head); //deallocate each Item in the backpack
        head=aus;
    }
}

void checkWeapons(){
    if(giocatore->arma.potenza<0){
        giocatore->arma.potenza=0;
    }
    for(int n=0;n<numeroNemici;n++){
        if(nemici[n].arma.potenza<0){
            nemici[n].arma.potenza=0;
        }
    }
}

void printWarnings(){
    if(giocatore->vita<50){
        printf("Your health is low, look for medication or consume those you have in the backpack.\n");
    }
    if(giocatore->arma.potenza<50){
        printf("Your weapon is weak, upgrade it with power-ups.\n");
    }
    if(strcmp(giocatore->arma.nome_arma,"Knife")==0){
        printf("Look for a sword killing an enemy, it's stronger than a knife!\n");
    }
    if(giocatore->arma.potenza<=0){
        printf("Your weapon is broken, repair it with some power-ups or take that of a dead enemy.\n");
    }
}
TipoOggetto getInputItemType(char*command){
    char type[15];
    sscanf(command, "%s", type); //copia la prima parola della stringa command in type
    int c=0;
    const char* tipi_cura[] = {"med", "medication", "hb", "health", "health booster","cure"};
    const char* tipi_arma[] = {"weapon", "we", "weapon booster", "weapon enhancer", "wb","wp"};
    for (int i = 0; i < 6; i++) {
        if (strcasecmp(type, tipi_cura[i]) == 0){ // confronto ignorando maiuscole/minuscole
            return TIPO_CURA;
        }
        else if (strcasecmp(type, tipi_arma[i]) == 0){ // confronto ignorando maiuscole/minuscole
            return TIPO_ARMA;
        }
    }
    return -1 ;
}

int getInputItemValue(char*command){
    int numeroCifra=0;
    int value=0;
    bool continua=true;
    char digit[2];//le cifre non possono essere superiori a 99
    int len=strlen(command);
    int s=0;
    bool cifraPresente=false;
    while(continua==true && s<len ){
        if(numeroCifra<2){
            if(isdigit(*command)){
                digit[numeroCifra]=*command;
                numeroCifra++;
                cifraPresente=true;
            } 
        }else{
            continua=false;
        }
        *command++;
        s++;
    }
    int newDigit;
    if(cifraPresente==true){
        for(int i=0;i<2;i++) {
            newDigit = digit[i] - '0';   // converte da quel punto in avanti
            value=value*10 +(newDigit);     
        }
        return value;
    }else{
        return -1;
    }  
}

int executeCommand(char*percorso){ //returns 1 if the move is command, -1 if the player wants to quit, 0 if the move is  movement
    char backToGame;
    bool checkbackpack=false;
    int executedCommand=0;
    for(int t=0;t<strlen(percorso);t++){
        percorso[t]=tolower(percorso[t]); //converto in minuscolo
    }
    if(strcmp(percorso,"l")==0 || strcmp(percorso,"legend")==0 || strcmp(percorso,"commands")==0 || strcmp(percorso,"command")==0){
        system(CLEAR_SCREEN);
        stampaLegenda(fileCommands);
        executedCommand=1;
    }else if(strcmp(percorso, "bp")==0 || strcmp(percorso, "backpack")==0 || strcmp(percorso, "back pack")==0){
        system(CLEAR_SCREEN); 
        executedCommand=1;
        checkbackpack=printOggettiZaino(giocatore->zaino); //se lo zaino è vuoto ritorna false e non si chiede all'utente di selezionare alcun item per ovvie ragioni
    }else if(strcmp(percorso, "sub") == 0 || strcmp(percorso, "switch") == 0 || strcmp(percorso, "swap") == 0 || strcmp(percorso, "sw") == 0){
        system(CLEAR_SCREEN);
        bool sub=checkSubstituteWeapon();
        if(sub==false){
            printf("Cannot swap weapons in a position where there are no dead enemies.\n");
        }
        executedCommand=1;
    }else if(strcmp(percorso,"hint")==0 || strcmp(percorso,"h")==0 || strcmp(percorso,"advice")==0){
        system(CLEAR_SCREEN);
        printWarnings();
        executedCommand=1;
    }else if(strcmp(percorso,"quit")==0 || strcmp(percorso,"exit")==0 || strcmp(percorso,"q")==0){
        system(CLEAR_SCREEN);
        executedCommand=-1;
    }
    if(executedCommand==true){
        if(checkbackpack==true){
            bool selectNewItem=true;
            do{
            char command[40];
            printf("Select an Item or press any key to go back to the game> ");
            fgets(command, sizeof(command), stdin);
            command[strcspn(command, "\n")] = '\0';
            TipoOggetto type=getInputItemType(command);
            int value=getInputItemValue(command);
            if(type==TIPO_ARMA){
                int returnedValue=rimuoviItem(&giocatore->zaino,value,TIPO_ARMA);
                if(returnedValue==0){
                    printf("You can't use this weapon enhancer becasue your Weapon is already Maxed Out.\n");
                }else if(returnedValue==-1){
                    printf("There is no weapon enhancer of %d points\n",value);
                }else{
                    printf("Weapon Enhancer of %d points removed\n",value);
                }
            }else if(type==TIPO_CURA){
                int returnedvalue=rimuoviItem(&giocatore->zaino,value,TIPO_CURA);
                if(returnedvalue==0){
                    printf("You can't use this medication becasue your health is already full.\n");
                }else if(returnedvalue==-1){
                    printf("There is no medication of %d points\n",value);
                }else{
                    printf("Medication of %d points removed.\n",value);
                }
            }else if(strcmp(command,"l")==0||strcmp(command,"L")==0){
                stampaLegenda(fileCommands);
                printf("\npress any key to check your backpack:");
                getchar();
            }
            else{
                selectNewItem=false;
            }
            if(selectNewItem==true){
                Sleep(2000);
                system(CLEAR_SCREEN);
                checkbackpack=printOggettiZaino(giocatore->zaino);
            }
            }while(selectNewItem==true && checkbackpack==true);
        }else{
            printf("\nPress any key to go back to the game:");
            getchar();
        }
    }
    return executedCommand;
}

bool consumeItem(Item*obj){
    if(obj->tipo==TIPO_ARMA){
        if(giocatore->arma.potenza==100){
            return false;
        }else{
            giocatore->arma.potenza+=obj->object->potArma->valorePotenziamento;
            if(giocatore->arma.potenza>100){
                giocatore->arma.potenza=100;
            }
            return true;
        }   
    }else if(obj->tipo==TIPO_CURA){
        if(giocatore->vita==100){
            return false;
        }else{
            giocatore->vita+=obj->object->cura->punti_vita; //potenzio arma
            if( giocatore->vita>100){
                giocatore->vita=100;
            }
            return true;
        }
    }
}
bool checkSubstituteWeapon(){
    bool nemicoMorto=false;
    bool armaRotta=false;
    for(int n=0;n<numeroNemici;n++){
        if(giocatore->pos.c==nemici[n].pos.c && giocatore->pos.r==nemici[n].pos.r){
            if(nemici[n].vita<=0){ //se ho un nemici morto nelle cordinate del giocatore 
                if(nemici[n].arma.potenza>0){ //se l'arma è più di 0 ovvero che non è rotta
                    int ausid=nemici[n].arma.id;
                    char*ausnome=nemici[n].arma.nome_arma;
                    int auspot=nemici[n].arma.potenza;
                    nemici[n].arma.id=giocatore->arma.id;
                    nemici[n].arma.nome_arma=giocatore->arma.nome_arma;
                    nemici[n].arma.potenza=giocatore->arma.potenza;
                    giocatore->arma.id=ausid;
                    giocatore->arma.nome_arma=ausnome;
                    giocatore->arma.potenza=auspot;
                    nemicoMorto=true;    
                    printf("You swapped power %d %s for power %d %s\n",nemici[n].arma.potenza,nemici[n].arma.nome_arma,giocatore->arma.potenza,giocatore->arma.nome_arma);
                }else{
                    printf("The enemy's weapon is broken not reccomended to take it unless you have strong weapons boosters.\n");
                    armaRotta=true;
                }
            }
        }
    }
    if(nemicoMorto==true || armaRotta==true){
        return true; //l'arma è stata scambiata
    }else{
        return false;//l'arma non è stata scambiata
    }
   
}
void printGiocatore(int nrsg){
    char riga='A';
    riga+=(char)giocatore->pos.r;
    if(modalità==1){
        printf("\t\t\t");
    }else if(modalità==2){
        printf("\t\t");
    }
    if(nrsg==1){
        printf("|%s's statistics",giocatore->nome);
        printf("\t\t\t\t ");
    }else if(nrsg==2){
        printf("|You are at the coordinates %c %d (*)",riga,giocatore->pos.c);
        printf("  \t\t ");
    }else if(nrsg==3){
        printf("|life's points:%d/100",giocatore->vita);
        printf("  Number of Items collected:%d",numOggettiRaccolti);
    }else if(nrsg==4){
        printf("|Type of weapon:%s     ",giocatore->arma.nome_arma);
        printf("Weapon's Power:%d/100   ",giocatore->arma.potenza);
    }


}
bool uccidiNemico(int nemico){
    int enemyDamage=1;
    if(strcmp(giocatore->arma.nome_arma,"Coltello")==0){
        enemyDamage=2; //enemydamage multiplier
    }else if(strcmp(giocatore->arma.nome_arma,"Sword")==0){
        enemyDamage=1; //enemydamage multiplier
    } 
    int lifes_gap=abs(giocatore->vita - nemici[nemico].vita);
    int weapons_gap=abs(nemici[nemico].arma.potenza - giocatore->arma.potenza);
    if(nemici[nemico].vita>0){
        if(nemici[nemico].arma.potenza > giocatore->arma.potenza){
            if(weapons_gap<=15){ //se il gap è poco e l'arma del nemico è un coltello
                if(strcmp(nemici[nemico].arma.nome_arma,"Coltello")==0){
                    giocatore->vita-=30; //the player loses a lot of health
                    giocatore->arma.potenza-=10; //and also his weapon's power
                    nemici[nemico].arma.potenza-=20; //the enemy's weapon loses even more power since is weaker
                    nemici[nemico].vita=0; //the enemy dies
                    return true;
                }else{
                    nemici[nemico].arma.potenza-=5;
                    giocatore->vita=0;
                    return false;
                }
            }else if(weapons_gap>15){
                nemici[nemico].vita-=15*enemyDamage; //the enemy take damage
                nemici[nemico].arma.potenza-=5;  //the enemy's weapon loses a bit of power
                giocatore->vita=0; //but still the player dies
                return false;
            }
        }else if(nemici[nemico].arma.potenza==giocatore->arma.potenza){
            if(giocatore->vita < nemici[nemico].vita){
                if(lifes_gap<=10){ //if the life's gap is low the player has some chances to survive
                    giocatore->vita-=20*enemyDamage; //but he has to have an high health 
                    giocatore->arma.potenza-=30; //the player's weapon loses a lot of power 
                    nemici[nemico].arma.potenza-=30; //the enemy's waepon loses a lot of power too
                    if(giocatore->vita>0){
                        nemici[nemico].vita=0;
                        return true;
                    }else{
                        return false;
                    }
                }else if(lifes_gap>10){ //if the life's gap is high the player dies
                    nemici[nemico].arma.potenza-=30; //but still the enemy's power loses a lot
                    giocatore->vita=0;
                    return false;
                }
            }else{//a parità di potenza di arma e vita vince il giocatore se
                if(strcmp(giocatore->arma.nome_arma,"Sword")==0){ //l'arma è una spada
                    nemici[nemico].vita=0; //uccido nemico
                    giocatore->vita-=30*enemyDamage; //perdo però tanta vita
                    giocatore->arma.potenza-=35; //I lose a lot of weapon's power too
                    return true;
                }else{ //altrimenti se l'arma è un coltello muoio
                    giocatore->vita=0;
                    nemici[nemico].arma.potenza-=35; //but still the enemy loses a lot of weapon's power
                    return false;
                }
            }
        }else{
            if(weapons_gap<15){
                giocatore->vita-=10*enemyDamage;
            }
            nemici[nemico].vita=0; //uccido nemico
            return true;
        }
    }
}
int*  controllaNemiciVicini(int*numNemiciVicini){ //conta i nemici vicini vivi
    int gr=giocatore->pos.r;
    int gc=giocatore->pos.c;
    int* nemiciVicini;
    *numNemiciVicini=0;
   for(int i=-1;i<2;i++){
        for(int j=-1;j<2;j++){
            for(int n=0;n<numeroNemici;n++){//controllo ogni elemento del mio array di nemici
                if(nemici[n].vita>0){//se il nemico che sto controllando ha vita >0 ovvero è vivo
                    if(nemici[n].pos.r==gr+i && nemici[n].pos.c==gc+j){ //controllo che sia vicino al mio giocatore                           
                        (*numNemiciVicini)++; //se è vicino al mio giocatore incremento il contatore
                    }
                }
            }
        }
   }
   nemiciVicini=(int*)malloc(sizeof(int)*(*numNemiciVicini));
   int conta=0;
    for(int i=-1;i<2;i++){
        for(int j=-1;j<2;j++){
            for(int n=0;n<numeroNemici;n++){//controllo ogni elemento del mio array di nemici
                if(nemici[n].pos.r==gr+i && nemici[n].pos.c==gc+j){ //controllo che sia vicino al mio giocatore
                    nemiciVicini[conta]=n; //salvo in quale posizione dell'array di nemici si trova il nemico da affrontare successivamente
                    conta++;
        
                }
            }
        }
    }
   return nemiciVicini;
}
void* returnOggetto(TipoOggetto tipo){
    if(tipo==TIPO_CURA){
        for(int c=0;c<numero_cure;c++){
            if(giocatore->pos.r==cure[c].pos.r && giocatore->pos.c==cure[c].pos.c ){
                printf("You just found an health booster of %d points!\n",cure[c].punti_vita);
                return &cure[c];
            }
        }
    }else if(tipo==TIPO_ARMA){
        for(int a=0;a<numero_armi;a++){
            if(giocatore->pos.r==potArma[a].pos.r && giocatore->pos.c==potArma[a].pos.c ){
                printf("You just found a weapon'sbooster of %d points!\n",potArma[a].valorePotenziamento);
                return &potArma[a];
            }
           
        }
    }
}
int oggettoPresente(){ 
    int gr=giocatore->pos.r;
    int gc=giocatore->pos.c;
    if(mappa[gr][gc]==2){
        return 2;
    }else if(mappa[gr][gc]==3){
        return 3;
    }
}
bool salvaMossa(char*mossa){ 
    FILE*file=fopen("cronologiaMosse.txt","a");
    if(file==NULL){
        printf("Errore di lettura da file\n");
        return false;
    }
    fprintf(file, "%c ", *mossa);
    fclose(file);
}
char* inserisciPercorso(char*input){
    char percorso[MAX_LUN];
    
    // Svuota il buffer prima di leggere
    fflush(stdin); // Su alcuni sistemi non funziona sempre correttamente

    if (fgets(percorso, MAX_LUN, stdin) == NULL) {
        printf("Error in reading\n");
        return NULL;
    }

    percorso[strcspn(percorso, "\n")] = '\0';  // Rimuove il '\n'

    input = (char*)malloc(strlen(percorso) + 1); //alloca un array della lunghezza esatta del percorso
    if (input != NULL) {
        strcpy(input, percorso);
    }

    return input;
    
}
int spostati(char direction){
    bool cellaOccupata=false;
    int gr=giocatore->pos.r; //gr= riga giocatore
    int gc=giocatore->pos.c; //gc=colonna giocatore
    int spostaColonna=0;
    int spostaRiga=0;
    bool nemicoPresente=false;
    if(direction=='>'){
        spostaRiga=0;
        spostaColonna=1;
    }else if(direction=='<'){
        spostaRiga=0;
        spostaColonna=-1;
    }else if(direction=='^'){
        spostaRiga=-1;
        spostaColonna=0;
    }else if(direction=='v'){
        spostaRiga=1;
        spostaColonna=0;
    }else{
        return -2; //non è un comando valido per spostarsi
    }
    if(gr+spostaRiga<0 || gc+spostaColonna>=colonne+1 || gr+spostaRiga>righe-1||gc+spostaColonna<1){ //se esco dal campo
        return -1;
    }else{ //se non esco
        for(int i=0;i<numeroNemici;i++){ //controllo se ci sono presenti dei nemici
            if(nemici[i].vita>0){//se il nemico è vivo
                if(nemici[i].pos.r==gr+spostaRiga && nemici[i].pos.c==gc+spostaColonna){ //allora controllo che sia nelle cordinate di dove il giocatore è diretto
                    nemicoPresente=true; //se corrispondono il nemico è presente nel percorso del giocatore
                    i=numeroNemici; //esco dal ciclo
                }
            }
        }
        if(mappa[gr+spostaRiga][gc+spostaColonna]==1){ //se nel percorso del giocatore c'è un ostacolo 
            return 1; //ritorno 1 per dire che non devo più spostarmi e ignorare le direzioni successive
        }else if(nemicoPresente==true){ //se c'è un nemico
            return 4; //ritorno 4 
        }
        else{ //altrimenti faccio spostare il giocatore aggiornando le cordinate della sua posizione 
            giocatore->pos.r=gr+spostaRiga;
            giocatore->pos.c=gc+spostaColonna;
            return 0; //ritorno vero per dire che lo spostamento è avvenuto con successo
        }
    }



}
int contaNemici(Nemico*nemici){
    int numeroNemiciVivi=0;
    for(int i=0;i<numeroNemici;i++){
        if(nemici[i].vita>0){
            numeroNemiciVivi++;
        }
    }
    return numeroNemiciVivi;
}
Giocatore* generaGiocatore(int**mappa){
    bool cellaOccupata;
    int randomRiga= 0 + rand() % (righe-1);
    int randomColonna= 1 + rand() % (colonne);
    int conta=0;
    Giocatore*giocatore=(Giocatore*)calloc(1,sizeof(Giocatore));
    int c;
    while ((c = getchar()) != '\n' && c != EOF);  // svuota stdin
    giocatore->nome = malloc(20 * sizeof(char));
    printf("What's your name?");
    scanf("%s", giocatore->nome);
    giocatore->arma.id=1;
    int randomArma =rand() % 2; // Valore tra 0 e 1
    giocatore->arma.nome_arma=nomi_armi[randomArma]; //assegno il nome dell'arma
    if(randomArma==0){//se l'arma è un coltello
        giocatore->arma.potenza=25; //le assegno una potenza fissa di 25
    }else if(randomArma==1){//se l'arma è una spada
        giocatore->arma.potenza=25;
    }
    do{
        cellaOccupata=false;
        if(mappa[randomRiga][randomColonna]==1){
            cellaOccupata=true; //cella occupata già da un ostacolo
        }
        else if(mappa[randomRiga][randomColonna]==2){
            cellaOccupata=true; //cella occupata già da una cura
        }else if(mappa[randomRiga][randomColonna]==3){
            cellaOccupata=true; //cella occupata da un'arma
        }
        else{
            for(int nemico=0;nemico<numeroNemici;nemico++){
                if(nemici[nemico].pos.r==randomRiga && nemici[nemico].pos.c==randomColonna){
                    cellaOccupata=true;
                }
            }
        }
        if(cellaOccupata==true){
            randomColonna= 1 + rand() % (colonne); //estraggo una nuova colonna
        }
    }while(cellaOccupata==true);
    giocatore->pos.c=randomColonna;
    giocatore->pos.r=randomRiga;
    giocatore->vita=puntiVitaGiocatore;
    char riga='A';
    riga+=(char)randomRiga;
    return giocatore;
}
int rimuoviItem(Item**inizio,int value,TipoOggetto tipo){
    bool oggettoTrovato=false;
    Item*prec=NULL; //il precedente del primo elemento non esiste
    int posizioneOggetto=0;
    int debugconta=1;
    Item*head=*inizio; //copio l'indirizzo del primo oggetto(contenuto in zaino nel main ovvero *inizio) in una variabile head ausiliaria
    while(oggettoTrovato==false && head!=NULL){
        if(head->tipo==tipo){
            if(head->object->potArma->valorePotenziamento==value ){
                oggettoTrovato=true;
            }

        }else if(head->tipo==tipo){
            if(head->object->cura->punti_vita==value ){
                oggettoTrovato=true;
            }
        }
       if(oggettoTrovato==false){ //se non trovo l'oggetto vado avanti, mi serve questo per non modificare l'indirizzo di head per quando trovo l'oggetto
            prec=head; //l'attuale oggetto diventa precedente
            head=head->next; //il prossimo oggetto diventa il corrente
            posizioneOggetto++; 
       }
       debugconta++;
    }
    if(oggettoTrovato==true){
        Item*prox=head->next; //salvo l'indirizzo del nodo/oggetto successivo in una variabile ausiliare
        if(consumeItem(head)==true){
            free(head); //elimino il nodo corrente/oggetto
            if(posizioneOggetto==0){ //se il primo elemento viene rimosso devo aggiornare l'indirizzo di inizio lista nel main
                *inizio=prox;  //aggiorno il contenuto della variabile nel main
            }else{
                prec->next=prox; //il nodo precedente viene collegato al successivo (il nodo dopo di quello deallocato)
            }
            return 1;//l'item viene rimosso
        }else{
            return 0; //l'item  non viene rimosso perchè non si può consumarlo in questo momento
        }

    }else{
        return -1; //se l'oggetto non è presente ritorno -1
    }


}
bool printOggettiZaino(Item*zaino){
    int c=1;
    Item*aus=zaino; //salvo l'indirizzo del primo Item
    if(zaino==NULL){
        printf("Backpack empty.\n");
        Sleep(1500);
        return false;
    }
    else{//codice sotto da metterci dentro


    /*   if(head->tipo==TIPO_CURA){
            printDisegno(disegni[CURA],);
            printf(" \n(%d)VALORE:%d\n",c,giocatore->zaino->object->cura->punti_vita);    
            
        }else if(head->tipo==TIPO_ARMA){
                printArma(disegniascii);
                printf(" \n(%d)VALORE:%d\n",c,giocatore->zaino->object->potArma->valorePotenziamento);  

        }
        head=head->next;
        c++;
        for(int  i=1;i<=6;i++){ //4 è il numero righe di ogni disegno
            head=aus;
            while(head!=NULL){ 
                if(head->tipo==TIPO_CURA){
                    printDisegno(disegno[CURA],i);
                }else if(head->tipo==TIPO_ARMA){
                    printDisegno(disegno[ARMA],i);
                }
                printf("  ");//distanzio ogni disegno
                head=head->next; 
            }
            printf("\n"); //una volta stampate tutte le prime righe, mando a capo
        
        }
        */
        while(zaino!=NULL){
            if(zaino->tipo==TIPO_CURA){
                printf("Health booster of %d\n",zaino->object->cura->punti_vita);
            }else if(zaino->tipo==TIPO_ARMA){
                printf("Weapon enhancer of %d\n",zaino->object->potArma->valorePotenziamento);
            }
            zaino=zaino->next;
        }
        return true;
    }



}
int raccogliOggetto(Item** head, OggettoUnion * itemtype,TipoOggetto tipo){
    Item* newOggetto=(Item*)malloc(sizeof(Item)); //alloco un nuovo elemento dello zaino
    if(newOggetto==NULL){
        return 0;
    }
    newOggetto->object=itemtype;
    if(tipo==TIPO_ARMA){
        newOggetto->object->potArma=itemtype->potArma;
    }else if(tipo==TIPO_CURA){
        newOggetto->object->cura=itemtype->cura;
    }
    newOggetto->next=NULL;
    newOggetto->tipo=tipo;
    if(*head==NULL){ //se lo zaino è vuoto(primo elemento della lista di Oggetti Item)
        *head=newOggetto;
    }else{
        Item*aus=*head; //così l'indirizzo del primo nodo non cambia
        while(aus->next!=NULL){
            aus=aus->next;
        }
        aus->next=newOggetto;
    
    }
    return 1;

}
PotenziatoreArma* generaPotArma(int**mappa){
    numero_armi= minArmiMappa + rand() % (maxArmiMappa - minArmiMappa + 1);
    int potArmiX[numero_armi]; //salvo le cordinate X (righe) di dove posizionerò il pot arma
    int potArmiY[numero_armi];  //salvo le cordinate y (colonne) di dove posizionerò il pot arma
    int conta;
    int randomColonna, randomRiga;
    bool rigaGiaOccupata;
    PotenziatoreArma*potArmi=(PotenziatoreArma*)malloc(sizeof(PotenziatoreArma)*numero_armi);
    for(int potArma=0;potArma<numero_armi;potArma++){
        conta=0;
        randomRiga= 0 + rand() % (righe-1);
        potArmiX[potArma]=randomRiga;
        rigaGiaOccupata=false;
        while(conta<potArma && rigaGiaOccupata==false ){
            if(potArmiX[conta]==randomRiga){ //se è presente già un'altra arma
                rigaGiaOccupata=true;
            }
            conta++;
        }
        conta=1;
        while(conta<colonne && rigaGiaOccupata==false ){ //controllo ogni elemento della riga
            if(mappa[randomRiga][conta]==1){ //se è presente un ostacolo nella riga random estratta
                rigaGiaOccupata=true; //metto vera la flag rigaContieneOstacolo
            }
            conta++;
        }
        conta=1;
        while(conta<colonne && rigaGiaOccupata==false ){ //controllo ogni elemento della riga
            if(mappa[randomRiga][conta]==2){ //se è presente una cura
                rigaGiaOccupata=true; //metto vera la flag rigaContieneOstacolo
            }
            conta++;
        }
        randomColonna= 1 + rand() % (colonne);
        if(rigaGiaOccupata==true){//se la riga estratta contiene già una cura, almeno 1 ostacolo o un'altro potenziatore d'arma controllo che non estragga anche la medesima colonna
            bool armaPresente=false;
            for(int c=0;c<potArma;c++){
                if(potArmiX[c]==randomRiga && potArmiY[c]==randomColonna){
                    armaPresente=true;
                    c=colonne; //esco dal ciclo
                }
            }
            if(armaPresente==true || mappa[randomRiga][randomColonna]==1 || mappa[randomRiga][randomColonna==2]){//se nelle cordinate attuali è presente un'altra arma o un ostacolo o una cura
                
                randomColonna= 1 + rand() % (colonne);
            }else{
                rigaGiaOccupata=false;
            }
        }
        potArmiY[potArma]=randomColonna; //assegno la colonna        
    }
    for(int potArma=0;potArma<numero_armi;potArma++){
        mappa[potArmiX[potArma]][potArmiY[potArma]]=3; //assegno 3 come identificativo di un'arma per evitare dopo nel main controlli su tutto l'array di armi
        potArmi[potArma].pos.r=potArmiX[potArma]; //assegno la pos della riga alla struttura potenziatoreArma
        potArmi[potArma].pos.c=potArmiY[potArma]; //assegno la pos della colonna alla struttura potenziatoreArma
        potArmi[potArma].valorePotenziamento=10+rand()%30;
    }
    return potArmi;
}
Cura* generaCure(int**mappa){
    numero_cure= minCureMappa + rand() % (maxCureMappa - minCureMappa + 1);
    int cureX[numero_cure]; //salvo le cordinate X (righe) di dove posizionerò la cura
    int cureY[numero_cure];  //salvo le cordinate y (colonne) di dove posizionerò la cura
    int conta;
    int randomColonna, randomRiga;
    bool rigaGiaOccupata;
    Cura*cure=(Cura*)malloc(sizeof(Cura)*numero_cure);
    for(int cura=0;cura<numero_cure;cura++){
        conta=0;
        randomRiga= 0 + rand() % (righe-1);
        cureX[cura]=randomRiga;
        rigaGiaOccupata=false;
        while(conta<cura && rigaGiaOccupata==false ){
            if(cureX[conta]==randomRiga){
                rigaGiaOccupata=true;
            }
            conta++;
        }
        conta=1;
        while(conta<colonne && rigaGiaOccupata==false ){ //controllo ogni elemento della riga
            if(mappa[randomRiga][conta]==1){ //se è presente un ostacolo nella riga random estratta
                rigaGiaOccupata=true; //metto vera la flag rigaContieneOstacolo
            }
            conta++;
        }
        randomColonna= 1 + rand() % (colonne);
        if(rigaGiaOccupata==true){//se la riga estratta contiene già una cura o almeno 1 ostacolo controllo che non estragga anche la medesima colonna
            bool curaPresente=false;
            for(int c=0;c<cura;c++){
                if(cureX[c]==randomRiga && cureY[c]==randomColonna){
                    curaPresente=true;
                    c=colonne; //esco dal ciclo
                }
            }
            if(curaPresente==true || mappa[randomRiga][randomColonna]==1){
                randomColonna= 1 + rand() % (colonne);
            }else{
                rigaGiaOccupata=false;
            }
        }
        cureY[cura]=randomColonna; //assegno la colonna        
    }
    for(int cura=0;cura<numero_cure;cura++){
        mappa[cureX[cura]][cureY[cura]]=2; //assegno 2 come identificativo di una cura per evitare dopo nel main controlli su tutto l'array di cure
        cure[cura].pos.r=cureX[cura]; //assegno la pos della riga alla struttura cura
        cure[cura].pos.c=cureY[cura]; //assegno la pos della colonna alla struttura cura
        cure[cura].punti_vita=10+rand()%50;
    }
    return cure;
}
Nemico* generaNemici(int ** mappa){
    Nemico*nemici=(Nemico*)malloc(sizeof(Nemico)*numeroNemici);
    int contatoreID=2;
    int randomArma;//per scegliere se assegnare al nemico un coltello o una spada
    int randomRiga, randomColonna;
    int posColNemici[numeroNemici],posRigaNemici[numeroNemici];//per controllare di non mettere più nemici nella stessa posizione
    int posRigaOstacolo[numeroNemici]; //tiene traccia di tutte le righe con almeno un ostacolo al loro interno
    int conta;
    bool rigaGiaOccupata; //variabile flag che diventa vera quando c'è almeno un ostacolo o un nemico nella riga appena estratta

    for(int n=0;n<numeroNemici;n++){
        nemici[n].vita=minVitaNemici + rand() % (maxVitaNemici - minVitaNemici + 1);
        nemici[n].arma.id=contatoreID+n;
        randomArma =rand() % 2; // Valore random tra 0 e 1
        nemici[n].arma.nome_arma=nomi_armi[randomArma]; //assegno il nome dell'arma
        if(randomArma==0){//se l'arma è un coltello
            nemici[n].arma.potenza=25; //le assegno una potenza fissa di 25
        }else if(randomArma==1){//se l'arma è una spada
            nemici[n].arma.potenza=minPotenzaArma + rand() % (maxPotenzaArma - minPotenzaArma + 1);
        }
        randomRiga=0+rand()%(righe-1); //genero una riga a caso
        //la riga la posso assegnare subito senza controlli perchè il numero di nemici in ogni riga sarà sempre minore della sua lunghezza(il numero di colonne)
        posRigaNemici[n]=randomRiga; //salvo tutte le cordinate di righe dove si trova ogni nemico
        nemici[n].pos.r=randomRiga; //assegno la riga al nemico ennesimo
        conta=1;
        int ostacolo=0;
        rigaGiaOccupata=false;
        while(conta<colonne && rigaGiaOccupata==false ){ //controllo ogni elemento della riga
            if(mappa[randomRiga][conta]==1){ //se è presente un ostacolo nella riga random estratta
                rigaGiaOccupata=true; //metto vera la flag rigaGiaOccupata
            }
            conta++;
        }
        conta=0; //riazzero la variabile contatore
        while(rigaGiaOccupata==false && conta<n){ //controllo fino al nemico dove attualmente sono arrivato, quindi n
            if(posRigaNemici[conta]==randomRiga){ //se nella riga attualmente estratta è presente già almeno un nemico
                rigaGiaOccupata=true; //allora flag rigaGiaOccupata è vera
            }
            conta++;
        }
        conta=0;
        randomColonna=1+rand()%(colonne);//genero una colonna a caso
        bool nemicoPresente=false;
        while(rigaGiaOccupata==true){ //se la riga è occupata da almeno un ostacolo o un nemico, faccio un controllo per evitare che combaci anche la colonna
            nemicoPresente=false; //per controllare se il nemico è presente nella cella corrente
            for(int num_nemico=0;num_nemico<n;num_nemico++){
                if(randomRiga==posRigaNemici[num_nemico] && randomColonna==posColNemici[num_nemico]){
                    nemicoPresente=true;
                    num_nemico=n; //esco dal ciclo
                }
            }
            if(mappa[randomRiga][randomColonna]==1 || nemicoPresente==true){ //se la cella  nelle cordinate estratte è un ostacolo o un altro nemico è presente 
                randomColonna=1+rand()%(colonne); //estraggo una nuova colonna
            }else{ //altrimenti se passo tutti i controlli allora la riga non è occupata 
                rigaGiaOccupata=false; //esco dal ciclo
            }
            conta++;
        }
        posColNemici[n]=randomColonna; //salvo la colonna di dove si trova il nemico ennesimo
        nemici[n].pos.c=randomColonna; //una volta passato il controllo so per certo che l'ultima  colonna estratta la posso assegnare
    }
    return nemici;
}
void generaOstacoli(int**mappa){
    int numero_ostacoli,x,y;
    bool ostacolo_trovato;
    int contatore_ostacoli;
    for(int i=0;i<righe-1;i++){
        numero_ostacoli= numMinOstacoli + rand() % (numMaxOstacoli - numMinOstacoli + 1); //ostacoli della riga corrente
        int pos_ostacolo[numero_ostacoli-1]; //array che contiene i valori delle colonne di dove si trovano gli ostacoli nella riga corrente
        for(int o=0;o<numero_ostacoli;o++){
            pos_ostacolo[o]=0+rand()%(colonne); //assegno le cordinate scelte in modo random tra 0 e il numero delle colonne

        } 
        
        for(int j=1;j<colonne;j++){ 
            ostacolo_trovato=false; //flag che fa interrompere il ciclo se ho trovato l'ostacolo
            contatore_ostacoli=0;     
            while(ostacolo_trovato==false && contatore_ostacoli<numero_ostacoli){
                if(pos_ostacolo[contatore_ostacoli]==j){
                    mappa[i][j]=1;
                    ostacolo_trovato=true;
                }
                contatore_ostacoli++;
            }
        }
    }
}
void stampaMappa(){
    bool personaggio;
    int n;
    char car='A';
    int num=0;
    int nrsg;//numero riga statistiche giocatore
    for(int i=0;i<righe+1;i++){
        for(int j=0;j<colonne+1;j++){
            if(j==0 && i<righe){
                printf("  %c| ",car);
                car++;
            }else if(i==righe){
                if(j>0){
                    if(num>9){
                        printf("  %d ",num);
                    }else{
                        printf("  %d  ",num);
                    }
                }else if(j==0){ //se sono alla colonna 0 dell'ultima riga
                    printf("     ");
                }
                num++;
            }else{
                personaggio=false; //presuppongo che non ci sia un personaggio da stampare nelle cordinate attuali
                n=0;
                while(n<numeroNemici+1 && personaggio==false){ //ciclo che controlla che un personaggio(nemico o giocatore)si trovi nelle cordinate i j esime
                    if(i==nemici[n].pos.r && j==nemici[n].pos.c && nemici[n].vita>0){ //se un nemico vivo si trova in entrambe le cordinate attuali
                        printf("  E  ");//allora lo stampo con N
                        personaggio=true;// e uso la variabile flag personaggio per dire di non stampare altri caratteri per la stessa cordinata
                    }else if(giocatore->pos.r==i && giocatore->pos.c==j){
                        printf("  *  ");
                        personaggio=true;
                    }else if(i==nemici[n].pos.r && j==nemici[n].pos.c && nemici[n].vita<=0){
                        printf("  /  ");//stampo simbolo arma 
                        personaggio=true;
                    }
                    n++;
                }
                if(personaggio==false){ //se non c'è nessun personaggio nella cordinata attuale allora posso stampare quello che c'è nel campo
                        switch(mappa[i][j]){
                            case 0:
                                printf("  .  ");
                                break;
                            case 1:
                                printf("  X  ");
                                break;
                            default:
                                printf("  .  ");
                            }
                }
            }

        }
        if(i==1*cds ){ //se sono alla riga 1 del livello facile e medio o 2 del livello difficile 
            if(warnings.close_enemies==true){
                if(warnings.numNemiciVicini==1){
                    printf("        Be careful, you are close to an enemy!     ");
                }else if(warnings.numNemiciVicini>1){
                    printf("      Be careful, there are %d enemies close to you  ",warnings.numNemiciVicini);
                }
            }
        }
        if(i==2*cds || i==(2*cds)+5){
            if(modalità==1){
                printf("\t\t\t");
            }else if(modalità==2){
                printf("\t\t");
            }
            printf("--------------------------------------");
        }
        else if(i>=(2*cds)+1 && i<(2*cds)+6){
            if(i==2*cds+1){
                nrsg=1; //devo stampare la prima riga delle statistiche del giocatore
            }
            printGiocatore(nrsg);
            nrsg++; //al prossimo giro stamperò la seconda
        }
        else if(i==8*cds || i==13 && modalità==2){
            if(modalità==1){
                printf("\t\t\t");
            }else if(modalità==2){
                printf("\t\t");
            }
            printf("Enemies lefted:%d",warnings.nemiciRimasti);
        }else if(i==9*cds || i==14 && modalità==2){
            if(modalità==1){
                printf("\t\t\t");
            }else if(modalità==2){
                printf("\t\t");
            }
            if(giocatore->vita<=15){
                printf("ATTENTION YOUR HEALTH IS VERY LOW!");
            }
        }
        printf("\n\n");
    }
}
int** creaMappa(){
    mappa=(int**)calloc(righe+1,sizeof(int*));
    for(int i=0;i<righe;i++){
        mappa[i]=(int*)calloc(colonne+1,sizeof(int));
    }
    return mappa;
}
void inizializzaVarGlobali(int mode){
    srand(time(NULL));
    if(mode==1){//modalità facile
        righe=10;
        colonne=10;  // Inizializza il generatore di numeri casuali
        numMinOstacoli=0,numMaxOstacoli=2;
        numeroNemici=4;
        minVitaNemici=20,maxVitaNemici=40;
        minPotenzaArma=20,maxPotenzaArma=60;
        minCureMappa=2,maxCureMappa=5;
        minArmiMappa=2, maxArmiMappa=5; //potenziatore armi
        puntiVitaGiocatore=60;
        minDannoSubito=10,maxDannoSubito=20;
        cds=1;
    }else if(mode==2){//modalità media
        righe=15;
        colonne=15;
        numeroNemici=6;
        numMinOstacoli=1,numMaxOstacoli=4;
        minVitaNemici=40,maxVitaNemici=60; 
        minPotenzaArma=25,maxPotenzaArma=100;
        minCureMappa=5,maxCureMappa=7;
        minArmiMappa=3, maxArmiMappa=5; 
        puntiVitaGiocatore=40;
        minDannoSubito=15,maxDannoSubito=30;
        cds=2;
    }else if(mode==3){//modalità difficile
        righe=20;
        colonne=20;
        numeroNemici=10;
        numMinOstacoli=3,numMaxOstacoli=6; //range of obstacles number for each row
        minVitaNemici=60,maxVitaNemici=80; 
        minPotenzaArma=25, maxPotenzaArma=100;
        minCureMappa=6,maxCureMappa=8;
        minArmiMappa=4, maxArmiMappa=6;
        puntiVitaGiocatore=30;
        minDannoSubito=20,maxDannoSubito=40;
        cds=2;
    }

    
}
int scegliModalità(){
    int input;
    FILE *istruzioni; // file dove leggere istruzioni
    stampaIstruzioni(istruzioni);
    bool inputNonValido;

    do {
        inputNonValido = false;
        printf("\nNumber: ");
        
        if (scanf("%d", &input) != 1) {  // Controlliamo che scanf abbia letto un numero
            inputNonValido = true;
            printf("Error: you have to type a number(1,2 o 3)!\n");

            // Puliamo il buffer dell'input
            while (getchar() != '\n');  
        } else {
            if(input<4 && input>=1){
                printf("You chose ");
            }

            switch (input) {
                case 1: // easy
                    printf("easy");
                    break;
                case 2: // intermediate
                    printf("medium");
                    break;
                case 3: // hard
                    printf("hard");
                    break;
                default:
                    printf("Type 1 for easy mode, 2 medium e 3 hard!\n");
                    inputNonValido = true;
            }
            printf(" mode");
        }
    } while (inputNonValido);
    printf("!\n");
    inizializzaVarGlobali(input);
    return input;
}
void stampaIstruzioni(FILE*file){
    char buffer[100]; 
    file = fopen("istruzioni.txt", "r");
    if (file == NULL) {
        perror("Errore nell'apertura del file");
        return ;
    }
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer); 
    }
    fclose(file);
    printf("\n");
    stampaLegenda(file);
}
void stampaLegenda(FILE*file){
    char buffer[100]; 
    file = fopen("legenda.txt", "r");
    if (file == NULL) {
        perror("Errore nell'apertura del file");
        return ;
    }
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer); 
    }
    fclose(file);
}