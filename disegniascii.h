
void printDisegno(char* nomeFile,int numRigheDaStampare){ //nome file da puntare, numero righe da stampare del file dato
    char buffer[100]; 
    FILE*file;
    file = fopen(nomeFile, "r");
    if (file == NULL) {
        perror("Errore nell'apertura del file");
        return ;
    }
    if(numRigheDaStampare<=0){//se il valore passat è 0 o negativo stampo tutto il disegno
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            printf("%s", buffer); 
        }
    }else if(numRigheDaStampare>0){ //altrimenti scorro solo fino alla riga specificata e stampo solo essa 
        int count = 0; //questa funzione serve per stampare in fila gli oggetti dello zaino
        while (count < numRigheDaStampare && fgets(buffer, sizeof(buffer), file) != NULL) {
           //buffer[strcspn(buffer, "\n")] = 0;
           int lastchar=strlen(buffer);
           buffer[lastchar]='\0';
           printf("%c",buffer[lastchar]);
           printf("%c",buffer[lastchar]);
            if(count==numRigheDaStampare-1){
                printf("%s", buffer); // Stampa la riga senza l'a capo (\n)
            }    
            count++;
        }
    }
    fclose(file);
}

