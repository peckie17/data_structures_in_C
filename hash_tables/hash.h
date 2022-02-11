//
//  hash.h
//  HashTables
//
//  Created by Miguel Lastras on 3/3/21.
//  Copyright © 2021 Miguel Lastras. All rights reserved.
//



#ifndef hash_h

#define hash_h
#define prime 499
#define YES 1
#define NO 0
//#define INITIAL_HASH_SIZE 19
#define INITIAL_HASH_ROOT_SIZE 4

int num_primes=9;
const size_t prime_size [9]= {503, 1009, 2017, 4027, 8053, 16103, 32203, 64403, 128813};
//size_t min_size=503;

float load_max = 0.50;


enum status_value {available, used, lazy_deleted};
enum hash_type {open_addr, sep_chain};
enum probing_type {none,linear, quadratic, double_hash};

typedef struct hash_item{
    char *record;
    char valid;
    char status;
    uint32_t key;
    size_t num_colisions; //número de colisiones/ intentos para ser insertado
} hash_item;

typedef struct{
    hash_item *item_list;
    size_t len; // number of items in the list
} hash_item_root;

typedef struct{
    hash_item *item_list; // for Open Addressing
    hash_item_root *array; // for Separate Chaining
    size_t hash_size;
    char type; // either OA or SC
    size_t num_records;
    char probing;
    uint8_t current_size; //indice de elemento de prime_size
} hash_table;

// function prototypes
int fun_prueba();

uint32_t compute_key(char *data, size_t data_len_bytes);
size_t hash_function(size_t key, size_t size);
hash_table* ht_new(const char type);
hash_item* ht_findRecord(char *record, hash_table *ht);
void ht_insertRecord(char *record, hash_table *ht);
void ht_removeRecord(char *record, hash_table *ht);
void ht_free(hash_table *ht);
void ht_find_printR(char *record, hash_table *ht);
void print_hash_structure(hash_table *ht, size_t pasos);

// specific function for open addressing (OA) type
hash_table* ht_new_size_OA(size_t hash_table_size, uint8_t index_size);
hash_item* ht_findKey_OA(uint32_t key, hash_table *ht);
//hash_item* ht_findKey_LinearOA(uint32_t key, hash_table *ht);
void ht_free_OA(hash_table *ht);
void ht_insertRecord_OA(char *record, hash_table *ht);
//void ht_insertRecord_LinearOA(char *record, hash_table *ht);
void ht_removeRecord_OA(char *record, hash_table *ht);
void print_hash_structure_OA(hash_table *ht, size_t pasos);
void ht_resize_OA(hash_table *ht, size_t new_size, uint8_t index_size);
size_t probingFunction_OA(char ht_probing, size_t key, size_t current_i);
size_t hash_function_2(size_t key);
void ht_checkSize(hash_table *ht);

// specific function for separate chaining (SC) type
hash_table* ht_new_size_SC(size_t hash_table_size, uint8_t index_size);
void ht_free_SC(hash_table *ht);
hash_item* ht_findKey_SC(uint32_t key, hash_table *ht);
void ht_insertRecord_SC(char *record, hash_table *ht);
void ht_removeKey_SC(uint32_t key, hash_table *ht);
void ht_removeRecord_SC(char *record, hash_table *ht);
void print_hash_structure_SC(hash_table *ht, size_t pasos);
void ht_resize_SC(hash_table *ht, size_t new_size, uint8_t index_size);


//{Funciones Generales
// simple checksum algorithm (Adler-32)
// to compute hash keys.
// source: https://en.wikipedia.org/wiki/Adler-32
const uint32_t MOD_ADLER = 65521;
uint32_t compute_key(char *data, size_t data_len_bytes)
{
    uint32_t a = 1, b = 0;
    size_t index;

    for (index = 0; index < data_len_bytes; ++index)
    {
        a = (a + data[index]) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
    }

    return (b << 16) | a;
}

size_t hash_function(size_t key, size_t size){
    return key % size;
}


hash_table* ht_new(const char type){
    if(type==sep_chain){
        return ht_new_size_SC(prime_size[0], 0);
    }
    if(type==open_addr){
        return ht_new_size_OA(prime_size[0], 0);
    }
    fprintf(stderr, "Not a valid type of hash table.\n");
    return NULL;
}

hash_item* ht_findRecord(char *record, hash_table *ht){
    uint32_t key = compute_key(record, strlen(record));

    if(ht->type == sep_chain)                         //recordar que lo cambié a regresar dirección                                                        //no solo la cadena de record
        return ht_findKey_SC(key, ht);

    if(ht->type == open_addr)
        return ht_findKey_OA(key, ht);

    return NULL;
}

void ht_insertRecord(char *record, hash_table *ht){
    if(ht->type == sep_chain){
        ht_insertRecord_SC(record, ht);
        return;
    }

    if(ht->type == open_addr){
        ht_insertRecord_OA(record, ht);
        return;
    }
}

void ht_free(hash_table *ht){
    if(ht->type == sep_chain)
        ht_free_SC(ht);

    if(ht->type == open_addr)
        ht_free_OA(ht);
}


void ht_removeRecord(char *record, hash_table *ht){
    if(ht->type==sep_chain){
        ht_removeRecord_SC(record, ht);
        return;
    }

    if(ht->type==open_addr){
        ht_removeRecord_OA(record, ht);
        return;
    }
}

void print_hash_structure(hash_table *ht, size_t pasos){
    if(ht->type==sep_chain){
        print_hash_structure_SC(ht, pasos);
        return;
    }

    if(ht->type==open_addr){
        print_hash_structure_OA(ht, pasos);
        return;
    }
}

void ht_checkSize(hash_table *ht){

    //si esta a mas de load_max de capacidad, resize
    float cap = (ht->hash_size)*load_max;
    if(ht->num_records >= cap && ht->hash_size<prime_size[num_primes-1]){
        uint8_t index_size = ht->current_size+1;
        //printf("\n%zu/%zu - Expandiendo a: %zu\n", ht->num_records, (uint32_t)cap, prime_size[index_size]);

        if(ht->type==open_addr){
            ht_resize_OA(ht, prime_size[index_size], index_size);
            return;
        }

        if(ht->type==sep_chain){
            ht_resize_SC(ht, prime_size[index_size], index_size);
            return;
        }

        printf("\n Tipo invalido. No se puede agrandar");
    }
    //si no no hace nada, tal vez despues hago lo de reducir

}

//}Funciones generales

//{Funciones SC
hash_table* ht_new_size_SC(size_t hash_table_size, uint8_t index_size){
    hash_table *ht = malloc(sizeof(hash_table));
    if(ht == NULL){
        fprintf(stderr, "Cannot allocate hash_table!\n");
        exit(1);
    }
    ht->array = malloc(sizeof(hash_item_root)*hash_table_size);
    if(ht->array == NULL){
        fprintf(stderr, "Cannot allocate hash_item_root lists!\n");
        exit(1);
    }
    for(size_t i=0; i<hash_table_size; i++){
        ht->array[i].item_list = malloc(sizeof(hash_item)*INITIAL_HASH_ROOT_SIZE);
        if(ht->array[i].item_list == NULL){
            fprintf(stderr, "Cannot allocate hash_item lists!\n");
            exit(1);
        }
    }

    ht->hash_size = hash_table_size;
    ht->current_size = index_size;

    // init all hash entries
    for(size_t i=0; i<ht->hash_size; i++){
        ht->array[i].len = INITIAL_HASH_ROOT_SIZE;
        for(size_t j=0; j<ht->array[i].len; j++){
            ht->array[i].item_list[j].record = NULL;
            ht->array[i].item_list[j].valid = NO;
            ht->array[i].item_list[j].key = 0;
            ht->array[i].item_list[j].num_colisions = 0;
        }
    }
    ht->type = sep_chain;
    ht->num_records=0;
    ht->probing=none;
    ht->item_list = NULL; // this is not used in SC
    return ht;
}

hash_item* ht_findKey_SC(uint32_t key, hash_table *ht){
    size_t addr = hash_function(key, ht->hash_size);
    for(size_t i=0; i<ht->array[addr].len; i++){
        hash_item *item = &(ht->array[addr].item_list[i]);
        if( item->valid == YES && item->key == key )
            return item; //dirección al alemento
    }
    return NULL;
}


void ht_insertRecord_SC(char *record, hash_table *ht){

    // check if record is already present in the table
    uint32_t key = compute_key(record, strlen(record));

    //check if record is already present in the table
    hash_item *item0 = ht_findKey_SC(key, ht);
    if(item0 != NULL){//se encontro un regsitro con la misma llave ->reemplazarlo
        //creo que en principio, solo hay qye cambiar el record y reiniciar num de colisiones, el num de records no cambia
        strcpy(item0->record,record);
        item0->num_colisions=0;
        return;
    }

    ht_checkSize(ht);

    //printf("inserting '%s'\n", record);
    size_t addr = hash_function(key, ht->hash_size);

    // try to insert record in the existing list
    for( size_t i=0; i<ht->array[addr].len; i++ ){
        hash_item *item = &(ht->array[addr].item_list[i]);          //y aquí porqué si?
        if(item->valid == NO){
            if(item->record != NULL) //ya existía
                item->record = realloc(item->record, strlen(record)+1);
            else
                item->record = malloc(strlen(record)+1); //no existía, reservar nueva memoria
            if( item->record == NULL ){
                fprintf(stderr, "Cannot allocate memory for record!\n");
                exit(1);
            }
            ht->num_records++;
            item->key = key;
            item->valid = YES;
            strcpy(item->record, record);
            item->num_colisions=i;
            return;
        }
    }

    // no space to insert record in the selected list!
    // we need to increase the size of the list
    hash_item_root new_root;
    new_root.item_list = malloc(sizeof(hash_item)*ht->array[addr].len*2);
    if(new_root.item_list == NULL){
        fprintf(stderr, "Cannot allocate memory for new root!\n");
        exit(1);
    }
    new_root.len = ht->array[addr].len*2;
    // copy the entries from the old to the new root
    for( size_t i=0; i<ht->array[addr].len; i++ ){
        new_root.item_list[i].key = ht->array[addr].item_list[i].key;
        new_root.item_list[i].valid = ht->array[addr].item_list[i].valid;
        new_root.item_list[i].record = ht->array[addr].item_list[i].record;
    }

    // insert the new entry in the next available position
    ht->num_records++;
    new_root.item_list[ht->array[addr].len].key = key;
    new_root.item_list[ht->array[addr].len].valid = YES;
    new_root.item_list[ht->array[addr].len].record = malloc(strlen(record)+1);
    new_root.item_list[ht->array[addr].len].num_colisions = ht->array[addr].len; //numero de intentos para poder insertarlo por fin
    strcpy(new_root.item_list[ht->array[addr].len].record, record);

    // initialize the rest of the entries (mark them as available)
    for( size_t i=ht->array[addr].len+1; i<new_root.len; i++ ){
        new_root.item_list[i].key = 0;
        new_root.item_list[i].valid = NO;
        new_root.item_list[i].record = NULL;
    }
    // free old root list array
    free(ht->array[addr].item_list);
    // replace old root with new root
    ht->array[addr] = new_root;

    return;
}

void ht_removeRecord_SC(char *record, hash_table *ht){
    //printf("removing '%s'\n", record);
    uint32_t key = compute_key(record, strlen(record));

    hash_item *item = ht_findKey_SC(key, ht); //en principio no necesitaría buscarlo en el array ya que ya tengo su dirección
    if(item==NULL){//no estaba
        return;
    }

    //si estaba el record, pero hay que ver si sí es el que solicitaron (puede que tengan las mismas llaves)
    if(strcmp(item->record, record)!=0){// si no es el record especificado -> no se remueve
        return;
    }

    //si estaba y si es el que solicitaron
    ht->num_records--;
    //item->record=NULL;
    item->num_colisions=0;
    item->valid=NO;
    return;

    /*size_t addr = hash_function(key, ht->hash_size);
    for( size_t i=0; i<ht->array[addr].len; i++ ){
        hash_item item = ht->array[addr].item_list[i];
        if( item.valid == YES && item.key == key ){
            ht->array[addr].item_list[i].valid = NO;
            ht->num_records--;
            return;
        }
    }
    fprintf(stderr, "Something is fishy! The record was found by ht_findKey but was not marked as not valid.\n");*/
}

void print_hash_structure_SC(hash_table *ht, size_t pasos){
    printf("\nHash table: SC - %d\n", ht->probing);
    printf("Load factor: %zu / %zu\n\n", ht->num_records, ht->hash_size);
    for(size_t i=0; i<ht->hash_size; i++){

        if(i%pasos==0){
            printf("[%2zu]: %zu-", i, ht->array[i].len);
            for(size_t j=0; j<ht->array[i].len; j++){
                if(ht->array[i].item_list[j].valid==YES)
                    printf("%s, ", ht->array[i].item_list[j].record);
            }

            printf("\n");
        }
    }
}

void ht_free_SC(hash_table *ht){
    for(size_t i=0; i<ht->hash_size; i++){
        for(size_t j=0; j<ht->array[i].len; j++){
            if(ht->array[i].item_list[j].record != NULL)
                free(ht->array[i].item_list[j].record);
        }
        free(ht->array[i].item_list);
    }
    free(ht->array);
    free(ht);
}

void ht_resize_SC(hash_table *ht, size_t new_size, uint8_t index_size){
    //printf("\n 1 \n");
    hash_table *big_ht = ht_new_size_SC(new_size, index_size);
    big_ht->probing=none;
    //printf("\n 2 \n");
    for(size_t i=0; i<ht->hash_size; i++){

        for(size_t j=0; j<ht->array[i].len; j++){
            //ht->array[addr].item_list[i]
            hash_item item = ht->array[i].item_list[j]; //solo quiero copias

            //insertar si está usado
            if(item.valid==YES)
                ht_insertRecord(item.record, big_ht);

            free(ht->array[i].item_list[j].record);
        }
        //al final, libera ese item_list
        free(ht->array[i].item_list);
    }
    //printf("\n 3 \n");
    free(ht->array);
    *ht = *big_ht;
    //printf("\n 4 \n");

}




//} fin Funciones SC

//{Funciones de OA
hash_table* ht_new_size_OA(size_t hash_table_size, uint8_t index_size){
    hash_table *ht = malloc(sizeof(hash_table));
    if(ht == NULL){
        fprintf(stderr, "Cannot allocate hash_table!\n");
        exit(1);
    }
    ht->item_list = malloc(sizeof(hash_item)*hash_table_size);
    if(ht->item_list== NULL){
        fprintf(stderr, "Cannot allocate hash entries!\n");
        exit(1);
    }
    ht->type = open_addr;
    ht->hash_size = hash_table_size;
    ht->current_size = index_size;

    //inicializar slots
    for(size_t i=0; i<ht->hash_size; i++){
        ht->item_list[i].key = 0;
        ht->item_list[i].valid = NO;
        ht->item_list[i].record = NULL;
        ht->item_list[i].status = available;
        ht->item_list[i].num_colisions = 0;
    }
    ht->array = NULL; // this is not used in OA
    ht->num_records=0;
    ht->probing=linear;//valor default, el usuario lo puede cambiar en el main justo despues de declarar tabla
    return ht;
}



hash_item* ht_findKey_OA(uint32_t key, hash_table *ht){//esto será usado por ht_findRecord

    size_t addr = hash_function(key, ht->hash_size);
    size_t j;

    //dentro del ciclo for en j=... se maneja según el tipo de probing seleccionado
    for(size_t i=0; i<ht->hash_size; i++){
        j=( addr + probingFunction_OA(ht->probing, key, i) )%ht->hash_size;//empezamos por revisar el elemento addr

        hash_item *item=&(ht->item_list[j]);

        //se deja de buscar hasta encontrar un slot available
        if( item->status == available){
            return NULL;
        }

        //ahora, aún no se han encontrado, ni se ha topado con un empty slot
        //hay que revisar el elemnto
        if( item->status == used && item->key == key){
            return item; //en principio, no debería de encontralo si su status es lazy:deleted
        }
    }

    return NULL;//no se encontró en la tabla
}

void ht_insertRecord_OA(char *record, hash_table *ht){
    uint32_t key = compute_key(record, strlen(record));

    //check if record is already present in the table
    hash_item *item0=ht_findKey_OA(key, ht);
    if(item0 != NULL){//se encontro un regsitro con la misma llave ->reemplazarlo
        //creo que en principio, solo hay qye cambiar el record y reiniciar num de colisiones, el num de records no cambia
        strcpy(item0->record,record);
        item0->num_colisions=0;
    return;
    }

    //inserting...
    //checar capacidad, si es necesario expandir
    ht_checkSize(ht);


    size_t addr = hash_function(key, ht->hash_size);        //no está, hay que insertar

    size_t j;
    for(size_t i=0; i<ht->hash_size; i++){
        j = ( addr + probingFunction_OA(ht->probing, key, i) ) % ht->hash_size;//empezamos por revisar el elemento addr
        hash_item *item=&(ht->item_list[j]);// nos referimos al original


        //está disponible:nuevo o borrado
        if(item->status!=used){

            if(item->status==available)// se supone que está nuevo, por lo que habría que alocar ( y no realocar)
                item->record=malloc(strlen(record)+1); //+1 para el caracter nulo

            if(item->status==lazy_deleted)//si ya está usado, pero se eliminó
                item->record=realloc(item->record,strlen(record)+1);

            //error al reservar memoria?
            if( item->record == NULL ){
                fprintf(stderr, "Cannot allocate memory for record!\n");
                exit(1);
            }

            //no hubo error,asignar valores->insertar
            item->status = used; //cambiar estado
            item->key = key;
            strcpy(item->record,record);        //strcpy(destino,fuente)
            item->num_colisions = i;//i corresponde al numero de intentos para insertar
            ht->num_records++;


            return; //salir después de haber encontrado un lugar

        }//fin if disponible

    }//fin for

    //Qué pasa si no encontró lugar?
    printf("\nCannot insert record!\n");
    return;

}

/*las funciones son basicamente iguales, solo cambian cómo se computa el elemento del arreglo j, dentro del ciclo (for i):
    linear:     j=addr + i
    quadratic:  j=addr + i*i
    doubleHash: j=addr + i*h2(key)
                %notese que addr = hash_function(key) = h1(key)
                h2(key)=prime-(key%prime) y prime < hash_size
*/

//Al usarla probing function voy a hacer el modulo, para no tener que pasar addr como argumento
size_t probingFunction_OA(char ht_probing, size_t key, size_t current_i){

    if(ht_probing == linear)//linear es el default
        return current_i;

    if(ht_probing == quadratic)
        return current_i*current_i;

    if(ht_probing == double_hash)
        return current_i * hash_function_2(key);

    fprintf(stderr, "Not a valid type of probing.\n");
    return 1;
}


//depende de prime que es una macro

//no estoy segura si prime deba tener una relación con el hash_size
size_t hash_function_2(size_t key){
    return prime - key%prime;
}

void ht_removeRecord_OA(char *record, hash_table *ht){ //este es lo mismo para cualquier probing, FIND se encarga de hacer el probing respectivo

    uint32_t key = compute_key(record, strlen(record));
    hash_item *item = ht_findKey_OA(key, ht); //yo quiero la dirección

    if(item==NULL){//no estaba
        return;
    }

    //si estaba el record, pero hay que ver si sí es el que solicitaron (puede que tengan las mismas llaves)
    if(strcmp(item->record, record)!=0){// si no es el record especificado -> no se remueve
        return;
    }

    //si está el registro, solo hay que cambiarle el status y bajar la cantidad de records
    item->record=NULL;
    item->status=lazy_deleted;
    item->num_colisions=0; //reiniciar numero de colisiones
    ht->num_records--;
    return;
}

//solo para que imprima si existe o no
void ht_find_printR(char *record, hash_table *ht){

    hash_item *item=ht_findRecord(record, ht);
    if(item==NULL){
        printf("\nNO EXISTE\n");
        //return;
    }
    else{
        printf("\nrecord: %s\n", item->record);
        //printf("llave: %zu, addr: ")
    }

    return;

}

void print_hash_structure_OA(hash_table *ht, size_t pasos){
    printf("\nHash table: OA - %d\n", ht->probing);
    printf("Load factor: %zu / %zu\n\n", ht->num_records, ht->hash_size);
    for(size_t i=0; i<ht->hash_size; i++){
        if( i % pasos ==0)
            printf("[%2zu]: %s - %zu\n", i, ht->item_list[i].record, ht->item_list[i].num_colisions);
    }
}


//una funcion que me reduzca o aumente el tamaño, supongo que en una función externa voy a calcular lo que se necesite
//eg. si está al

void ht_resize_OA(hash_table *ht, size_t new_size, uint8_t index_size){
    //printf("\n 1 \n");
    hash_table *big_ht = ht_new_size_OA(new_size, index_size);
    big_ht->probing=ht->probing;
    //printf("\n 2 \n");
    for(size_t i=0; i<ht->hash_size; i++){
        hash_item item = ht->item_list[i]; //solo quiero copias
        //insertar si está usado
        if(item.status==used)
            ht_insertRecord(item.record, big_ht);
    }
    //printf("\n 3 \n");
    free(ht->item_list);
    *ht = *big_ht;
    //printf("\n 4 \n");

}




void ht_free_OA(hash_table *ht){
    for(size_t i=0; i<ht->hash_size; i++){
        if(ht->item_list[i].record != NULL)
            free(ht->item_list[i].record);
    }
    free(ht->item_list);
    free(ht);
}

//}//fin OA

#endif /* hash_h */





