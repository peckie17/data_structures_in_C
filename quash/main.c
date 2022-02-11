

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "quash.h"



int main(int argc, const char * argv[]) {

    system("color F0");
    FILE *pFile = fopen("./commands.txt", "rt");
    if(pFile == NULL){
        fprintf(stderr, "Cannot open command file!\n");
        return 2;
    }

    quash *qh=new_quash(20);
    if(qh==NULL)
        return 1;

    char command[100];

    //printf("lavlis");
    while( fgets(command, 100, pFile) != NULL){
        //printf("dagar");
        command[strlen(command)-1] = '\0'; // remove new line character
        char operation[25];
        int value;
        sscanf(command, "%s %d", operation, &value);

        if(strcmp(operation, "lookup")==0){
            lookup(value, qh);
        }

        else if(strcmp(operation, "insert")==0){
            insert_val(value, qh);
        }

        else if(strcmp(operation, "delete")==0){
            delete_val(value, qh);
        }

        else if(strcmp(operation, "deleteMin")==0){
            deleteMin(qh);
        }

        else if(strcmp(operation, "print")==0){
            print_heap(qh->hp);
        }

        else{
            printf("\nComando invalido.");
        }

        //printf("\nc: %s, x: %d", operation, value);

    }

    printf("\n\n");
    fclose(pFile);
    free_quash(qh);


return 0;
}
