//
//  main.c
//  HashTables
//
//  Created by Miguel Lastras on 2/25/21.
//  Copyright © 2021 Miguel Lastras. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "hash.h"
#include <time.h>
#include <windows.h>

int main(int argc, const char * argv[]) {

    //uint8_t no_pruebas = 1;
    size_t comandos_max=1000;
    //double tiempos_arr[no_pruebas];
    size_t no_rec;
    size_t s_final;


    //ClearConsoleToColors(0,15);
    system("color F0");


    /*FILE *ft = fopen("totos.txt", "a");
    if (ft == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }*/


    //FILE *pFile = fopen("C:\\Users\\Rebeca\ Alvarado\\Documents\\Licenciatura\\8Semestre\\Computo\ de\ alto\ rendimiento\\P1\\commands_small_1.txt", "rt");
    FILE *pFile = fopen("./commands.txt", "rt");
    //FILE *pFile = fopen("./commands_small_1.txt", "rt");
    //FILE *pFile = fopen("../commands_yo.txt", "rt");
    //_small_1
    if(pFile == NULL){
        fprintf(stderr, "Cannot open command file!\n");
    }

    puts("Comenzando ... ");
    getchar();

    char command[100];


    size_t ii=0;
    hash_table *ht = ht_new(open_addr);
    ht->probing=double_hash;
    if(ht==NULL)
        return 1;

    while( fgets(command, 100, pFile) != NULL){


        command[strlen(command)-1] = '\0'; // remove new line character
        char operation[20];
        char name[50];
        char lastname[50];
        char fullname[100];
        sscanf(command, "%s %s %s", operation, name, lastname);
        sprintf(fullname, "%s %s", name, lastname);
        //puts(command);
        if( strcmp(operation, "I") == 0)
            ht_insertRecord(fullname, ht);
        if( strcmp(operation, "R") == 0)
            ht_removeRecord(fullname, ht);


        if(ii>=comandos_max)
            break;

        ii++;

    }
    no_rec=ht->num_records;
    s_final=ht->hash_size;

    print_hash_structure(ht, s_final*0.05);


    ht_free(ht);

    puts("Termino ... ");
    getchar();



    //system("cls");
    //print_hash_structure(ht, 500);


    printf("\n\nOperaciones: %zu, Load Factor: %zu / %zu\n", comandos_max, no_rec, s_final);

    //fprintf(ft, "%zu %f %zu %zu\n", comandos_max, average, no_rec, s_final);
    fclose(pFile);
    //fclose(ft);
    //ht_free(ht);

    return 0;
}

