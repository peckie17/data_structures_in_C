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

int main(int argc, const char * argv[]) {

    uint8_t no_pruebas = 1;
    size_t comandos_max=5000000;
    double tiempos_arr[no_pruebas];
    size_t no_rec;
    size_t s_final;





    FILE *ft = fopen("tiempos_OA-double1.txt", "a");
    if (ft == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }


    //FILE *pFile = fopen("C:\\Users\\Rebeca\ Alvarado\\Documents\\Licenciatura\\8Semestre\\Computo\ de\ alto\ rendimiento\\P1\\commands_small_1.txt", "rt");
    FILE *pFile = fopen("./commands.txt", "rt");
    //FILE *pFile = fopen("./commands_small_1.txt", "rt");
    //FILE *pFile = fopen("../commands_yo.txt", "rt");
    //_small_1
    if(pFile == NULL){
        fprintf(stderr, "Cannot open command file!\n");
    }

    //puts("Archivo abierto correctamente ... ");
    //getchar();

    char command[100];
    //puts("hola");
    //getchar();
    for(uint8_t pr=0; pr<no_pruebas; pr++){
            size_t ii=0;
            hash_table *ht = ht_new(open_addr);
            ht->probing=double_hash;
            if(ht==NULL)
                return 1;

            clock_t tic = clock();
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
            clock_t toc = clock();
            double tictoc = (double)(toc - tic) / CLOCKS_PER_SEC;

            tiempos_arr[pr]=tictoc;
            no_rec=ht->num_records;
            s_final=ht->hash_size;
            ht_free(ht);
            /*if(pr<(no_pruebas-1)) // no liberar la ultima iteracion
                ht_free(ht);
            else{
                ht_free(ht);
                system("cls");
                print_hash_structure(ht, 500);
            }*/
    }



    //system("cls");
    //print_hash_structure(ht, 500);

    double average=0;

    for(uint8_t pr=0; pr<no_pruebas; pr++){
        printf("Tiempos %d: %f\n", pr, tiempos_arr[pr]);
        average+=tiempos_arr[pr];
    }
    average=average/(float)(no_pruebas);
    printf("\n\nOperaciones: %zu, average = %f, Load Factor: %zu / %zu\n", comandos_max, average, no_rec, s_final);

    fprintf(ft, "%zu %f %zu %zu\n", comandos_max, average, no_rec, s_final);
    fclose(pFile);
    fclose(ft);
    //ht_free(ht);

    return 0;
}

