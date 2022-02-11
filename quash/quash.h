//lo voy a intentar a hacer con la implementación más rapida con hash table cuadratica
#ifndef quash_h
#define quash_h

#ifdef _WIN32
#  ifdef _WIN64
#    define PRI_SIZET PRIu64
#  else
#    define PRI_SIZET PRIu32
#  endif
#else
#  define PRI_SIZET "zu"
#endif

#define N_INF INT32_MIN
#define INF INT32_MAX


#define YES 1
#define NO 0

const int num_primes=9;
const float load_max = 0.50;
const size_t prime_size [9]= {53,97 , 2017, 4027, 8053, 16103, 32203, 64403, 128813};
enum status_value {available, used, lazy_deleted};
enum hash_type {open_addr, sep_chain};
enum probing_type {none,linear, quadratic, double_hash};

//{ESTRUCTURAS
typedef struct{  //aunque es redundante con el de record
int value;
int num_copies;
int pos_heap;
int pos_hash;

}element;

typedef struct{
    element *elem;//arreglo de elementos
    int heap_size;
    int num_elem;
}heap;

typedef struct hash_item{
    char *record;
    char valid;
    char status;
    uint32_t key;
    element *elem; //todo lo que tiene que ver con el heap
                    //no necesita puntero porque no es una rreglo
    //size_t num_colisions; //número de colisiones/ intentos para ser insertado
} hash_item;

typedef struct{
    hash_item *item_list; // for Open Addressing
    size_t hash_size;
    char type; // either OA or SC
    size_t num_records;
    char probing;
    uint8_t current_size; //indice de elemento de prime_size
} hash_table;

typedef struct{
    heap *hp;
    hash_table *ht;
}quash;
//}

//{function prototypes
uint32_t compute_key(char *data, size_t data_len_bytes);
size_t hash_function(size_t key, size_t size);
hash_table* ht_new(const char type);

hash_table* ht_new(const char type);
hash_table* ht_new_size_OA(size_t hash_table_size, uint8_t index_size);
hash_item* ht_findRecord(char *record, hash_table *ht);
void ht_insertRecord(hash_table *ht, char *record, element *elem);
void ht_removeRecord(char *record, hash_table *ht);
void ht_find_printR(char *record, hash_table *ht);
void print_hash_structure_OA(hash_table *ht, size_t pasos);
void ht_resize_OA(hash_table *ht, size_t new_size, uint8_t index_size);
void ht_free(hash_table *ht);
void ht_checkSize(hash_table *ht);


void heapify_down(quash *qh, int pos);
int log2floor(int n);
int log2ceil(int n);
heap *new_heap(int size);
void print_heap(heap *h);
void free_heap(heap *h);
int parent(int pos);
int left_child(int pos);
int left_child(int pos);
int heapify_up(quash *qh, int pos);
element *insert_heap(quash *qh, int value);

hash_item* find_val(int x, hash_table *ht, heap *hp);
void insert_val(int x, quash *qh);
void print_elem(element *elem);
quash *new_quash(int heap_size);
void lookup(int x, quash *qh);
void delete_val(int x, quash *qh);
void deleteMin(quash *qh);
void free_quash(quash *qh);
//}

//{HASH TABLE OA-quad

const uint32_t MOD_ADLER = 65521;
uint32_t compute_key(char *data, size_t data_len_bytes){
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

    return ht_new_size_OA(prime_size[0], 0);
}

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
        ht->item_list[i].elem=NULL;
        //ht->item_list[i].num_colisions = 0;
    }
    //ht->array = NULL; // this is not used in OA
    ht->num_records=0;
    ht->probing=quadratic;//valor default, el usuario lo puede cambiar en el main justo despues de declarar tabla
    return ht;
}



hash_item* ht_findRecord(char *record, hash_table *ht){
    uint32_t key = compute_key(record, strlen(record));
    size_t addr = hash_function(key, ht->hash_size);
    size_t j;

    //dentro del ciclo for en j=... se maneja según el tipo de probing seleccionado
    for(size_t i=0; i<ht->hash_size; i++){
        j=( addr + i*i)%ht->hash_size;//empezamos por revisar el elemento addr

        hash_item *item=&(ht->item_list[j]);

        //se deja de buscar hasta encontrar un slot available
        if( item->status == available){
            return NULL;
        }

        //ahora, aún no se han encontrado, ni se ha topado con un empty slot
        //hay que revisar el elemnto
        if( item->status == used && item->key == key){
            //printf("\nEN FIND: %s - llave: key %d\n", record, key);

            return item; //en principio, no debería de encontralo si su status es lazy:deleted

        }
    }

    return NULL;//no se encontró en la tabla
}

void ht_insertRecord(hash_table *ht, char *record, element *elem){
    uint32_t key = compute_key(record, strlen(record));

    //se su´pone que en la funcion ppal ya se busca la llave
    //aunque flata tratar el problema si hay llaves duplicadas

    //check if record is already present in the table
    /*hash_item *item0=ht_findKey_OA(key, ht);
    if(item0 != NULL){//se encontro un regsitro con la misma llave ->reemplazarlo
        //creo que en principio, solo hay qye cambiar el record y reiniciar num de colisiones, el num de records no cambia
        strcpy(item0->record,record);

    return;
    }*/

    //inserting...
    //checar capacidad, si es necesario expandir
    ht_checkSize(ht);


    size_t addr = hash_function(key, ht->hash_size);        //no está, hay que insertar

    size_t j;
    for(size_t i=0; i<ht->hash_size; i++){
        j = ( addr + i*i) % ht->hash_size;//empezamos por revisar el elemento addr
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
            item->elem=elem;
            item->elem->pos_hash=j;
            strcpy(item->record,record);        //strcpy(destino,fuente)
            //item->num_colisions = i;//i corresponde al numero de intentos para insertar
            ht->num_records++;



            return; //salir después de haber encontrado un lugar

        }//fin if disponible

    }//fin for

    //Qué pasa si no encontró lugar?
    printf("\nCannot insert record!\n");
    return;

}

void ht_removeRecord(char *record, hash_table *ht){ //este es lo mismo para cualquier probing, FIND se encarga de hacer el probing respectivo

    //uint32_t key = compute_key(record, strlen(record));
    hash_item *item = ht_findRecord(record, ht); //yo quiero la dirección

    if(item==NULL){//no estaba
        return;
    }

    //si estaba el record, pero hay que ver si sí es el que solicitaron (puede que tengan las mismas llaves)
    if(strcmp(item->record, record)!=0){// si no es el record especificado -> no se remueve
        return;
    }

    //si está el registro, solo hay que cambiarle el status y bajar la cantidad de records
    item->record=NULL;
    item->elem=NULL;
    item->status=lazy_deleted;
    //item->num_colisions=0; //reiniciar numero de colisiones
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
    printf("\nHash table: OA\n");
    int pos_hp=-1;
    //printf("Load factor: %d / %d\n\n", ht->num_records, ht->hash_size);
    for(size_t i=0; i<ht->hash_size; i++){
        if( i % pasos ==0){
            if(ht->item_list[i].elem == NULL)
                pos_hp=-1;
            else
                pos_hp=ht->item_list[i].elem->pos_heap;

            printf("[%2d]: %s - heap: %d\n", i, ht->item_list[i].record, pos_hp);
        }
    }
}

void ht_resize_OA(hash_table *ht, size_t new_size, uint8_t index_size){
    //printf("\n 1 \n");
    hash_table *big_ht = ht_new_size_OA(new_size, index_size);
    big_ht->probing=ht->probing;
    //printf("\nEXPANDIENDO\n");
    //printf("\n 2 \n");
    for(size_t i=0; i<ht->hash_size; i++){
        hash_item item = ht->item_list[i]; //solo quiero copias
        //insertar si está usado
        if(item.status==used)
            ht_insertRecord( big_ht, item.record, item.elem);
    }
    //printf("\n 3 \n");
    free(ht->item_list);
    *ht = *big_ht;
    //printf("\n 4 \n");

}

void ht_free(hash_table *ht){
    for(size_t i=0; i<ht->hash_size; i++){
        if(ht->item_list[i].record != NULL)
            free(ht->item_list[i].record);
    }
    free(ht->item_list);
    free(ht);
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

        printf("\n Tipo invalido. No se puede agrandar");
    }
    //si no no hace nada, tal vez despues hago lo de reducir

}


//}

//{HEAP


void heapify_down(quash *qh, int pos){
    heap *h = qh->hp;
    int last_leave = h->num_elem;
    int child;

    for( ; pos < last_leave; pos=child){
        child = left_child(pos);

        if(child > last_leave)//todo bien, aun no llegamos al final
            break;
            //que pasaa si era >=?

        //queremos intercambiarlo por el menor
        if( h->elem[child].value > h->elem[child+1].value)
           child++;//el right child es el menor

        //el child es menor que el padre?
        if(h->elem[child].value < h->elem[pos].value){

            element elem_aux = h->elem[pos];
            h->elem[pos] = h->elem[child];
            h->elem[child] = elem_aux;

            h->elem[pos].pos_heap=pos;
            h->elem[child].pos_heap=child;

            //actualizar en la ht
            qh->ht->item_list[h->elem[pos].pos_hash].elem = &(h->elem[pos]);
            qh->ht->item_list[h->elem[child].pos_hash].elem = &(h->elem[child]);


        }

        else
            break;
    }

}

int log2floor(int n){
    int c=0;
    while(n){//mientras sea diferente de 0
        n = n>>1;
        c++;
    }
    return c-1;
}

int log2ceil(int n){
    return log2floor(n)+1;
}

heap *new_heap(int size){
    heap *h=malloc(sizeof(heap));
    int h_size=(1 << log2ceil(size))-1;

    h->elem = malloc(sizeof(element)*(h_size+1));
    if(h->elem==NULL){
        fprintf(stderr, "No hay espacio para reservar el heap!\n");
        exit(1);
    }
    h->heap_size = h_size;

    for(int i=1; i<=size; i++){
        h->elem[i].value=INF;
        h->elem[i].num_copies=0;
        h->elem[i].pos_hash=-1;
        h->elem[i].pos_heap=-1;
        //creo que los habiamos puesto como inf
    }
    h->elem[0].value=N_INF;
    h->num_elem=0;

    return h;
}

void print_heap(heap *h){
    printf("\nHEAP: [ ");

    for(int i=1; i<=h->num_elem; i++)
        printf(" %d ", h->elem[i].value);
    printf(" ]");
}

void free_heap(heap *h){
    free(h->elem);
}

int parent(int pos){
    return pos>>1; //dividir /2
}

int left_child(int pos){
    return pos<<1;//*2 (floor)
}

int right_child(int pos){
    return (pos<<1)+1;//*2 (floor)
}

int heapify_up(quash *qh, int pos){
    heap *h=qh->hp;
    int new_pos=pos;
    while(h->elem[pos].value < h->elem[parent(pos)].value){
        //el hijo es más chico
        new_pos=parent(pos);
        element elem_aux=(h->elem[pos]); //intercambiar
        (h->elem[pos])=(h->elem[parent(pos)]);
        (h->elem[parent(pos)])=(elem_aux);//elem auz ya es una direccion

        //actualizar posiciones, recordar que los elementos ya estan actualizados
        h->elem[pos].pos_heap=pos;
        h->elem[parent(pos)].pos_heap=new_pos;

        //también actualizar las direcciones en la ht
        qh->ht->item_list[h->elem[pos].pos_hash].elem=&(h->elem[pos]);//el nuevo
        qh->ht->item_list[h->elem[parent(pos)].pos_hash].elem=&(h->elem[parent(pos)]);//el nuevo

        pos=parent(pos);
    }
    return new_pos;
}

element *insert_heap(quash *qh, int value){
    heap *h=qh->hp;

    int pos =h->num_elem+1; //insertar al final
    if(pos>h->heap_size)
    {
        printf("No hay espacio para insertar %d", value);
        return NULL;
    }

    h->elem[pos].value=value;
    h->elem[pos].pos_heap=pos;
    int curr_pos=heapify_up(qh,pos);
    //h->elem[curr_pos].value=value;
    //h->elem[curr_pos].pos_heap=curr_pos;//actualizar posicopn
    h->num_elem++;

    return &(h->elem[curr_pos]);
}

//}

//{GENERAL

//qué tipo de función para insertar
//supongo que void es suficiente
quash *new_quash(int heap_size){
    quash *qh=malloc(sizeof(quash));
    qh->hp=new_heap(heap_size);
    qh->ht=ht_new(open_addr);

    return qh;
}

void lookup(int x, quash *qh){
    char x_str[10];//eso es el record
    itoa(x, x_str,10);
    //uint32_t key=compute_key(x_str, strlen(x_str));

    hash_item *ht_item=ht_findRecord(x_str, qh->ht);

    if(ht_item==NULL){
        printf("\nNo se encontro %d", x);
        return;
    }

    if(ht_item->elem->value==x){
        printf("\nElemento %d encontrado. #copias: %d", ht_item->elem->value, ht_item->elem->num_copies);
        return;
    }

    else{
        printf("\nNo coincide con %d .Se encontro %d", x,ht_item->elem->value);
        printf("\n Record: %s", ht_item->record);
        print_elem(ht_item->elem);//creo que eso si es una dirección
        return;
    }
    // tal vez aqui pueda hacer el manejo de llaves duplicadas
}

void insert_val(int x,quash *qh){
    //primero hay que buscarlo,
    // para eso necesitamos generarle una llave
    //y para generar la llave, hay que convertirlo a string

    if(qh->hp->num_elem==(qh->hp->heap_size)-1){
        printf("\nEstructura llena");
        return;
    }

    char x_str[10];//eso es el record
    itoa(x, x_str,10);
    //uint32_t key=compute_key(x_str, strlen(x_str));

    hash_item *ht_item=ht_findRecord(x_str, qh->ht);
    //hash_item *ht_item=find_val(x, ht,hp);

    //hay que ver si es nulo
    if(ht_item==NULL){
        printf("\nInsertando %d.", x);

        //la idea era que la estructura de element fuera compartida entre ambas estructuras
        //tal vez primero insertar en heap y regresar la dirección a la struct elem
        //así en hash_table modifico esa dirección, lo tengo que agregar como argumento en insert
        element *elem_x=insert_heap(qh, x);

        ht_insertRecord(qh->ht, x_str, elem_x);
        elem_x->num_copies++;

        printf(" #copias: %d", elem_x->num_copies);

        //print_elem(elem_x);
        return;
    }
    else{
        ht_item->elem->num_copies++;
        printf("\nYa existe %d. #copias: %d", x, ht_item->elem->num_copies);
        return;
    }
}

void print_elem(element *elem){
    printf("\n Val: %d. Num copias: %d. Pos ht: %d. Pos hp: %d.", elem->value, elem->num_copies, elem->pos_hash, elem->pos_heap);

}

//ahora, qué tiene que hacer el de delete
//necesitamos de dónde buscamos en la ht, saber cual es la dirección en la heap
//recordar actualizar la direccion en la ht de elem a NULL
void hp_delete_x(quash *qh, int hp_index){

    heap *hp = qh->hp;

    if(hp_index == qh->hp->num_elem){
        //si es la ultimaa
        hp->elem[hp->num_elem].value=INF;//INF como vacio
        hp->num_elem--;
        return;
    }


    hp->elem[hp_index]=hp->elem[hp->num_elem];//cambiar por la última hoja
    hp->elem[hp_index].pos_heap=hp_index;
    qh->ht->item_list[hp->elem[hp->num_elem].pos_hash].elem=&(hp->elem[hp_index]);//el nuevo


    //cuando es cuando decremento el numero de contador y lo pongo aINF
    hp->elem[hp->num_elem].value=INF;//INF como vacio
    hp->num_elem--;
    heapify_down(qh, hp_index);

}

void delete_val(int x, quash *qh){

    if(qh->hp->num_elem==0){
        printf("\nEstructura vacia.");
        return;
    }

    //primero hay que buscarlo
    char x_str[10];//eso es el record
    itoa(x, x_str,10);
    //uint32_t key=compute_key(x_str, strlen(x_str));

    hash_item *ht_item=ht_findRecord(x_str, qh->ht);
    //hash_item *ht_item=find_val(x, ht,hp);

    //hay que ver si existe
    if(ht_item==NULL){
        printf("\nNo existe el elemento %d.", x);
        return;
    }

    //ahorita no me preocupo por las duplicadas
    //existe, cuántas copias tiene
    if(ht_item->elem->num_copies > 1){
        //si hay más de una copias, solo hay que decrementar el contador
        ht_item->elem->num_copies--;
        printf("\nContador de %d decrementado. #copias: %d", x, ht_item->elem->num_copies);
        return;
    }

    //ahora, si se tiene que eliminar, supongo que haré una función nueva
    //de la ht, necesito saber el lugar en el heap
    //ese lugar, lo intercambio con la

    if(ht_item->elem->num_copies == 1){
        //aqui es donde ya se elimina

        //print_elem(ht_item->elem);
        hp_delete_x(qh, ht_item->elem->pos_heap);
        //ahora falta borrarlo de la ht
        //creo no tengo que disminuir el numero de copias, ya que está eliminado
        ht_removeRecord(x_str, qh->ht); //estoy haciendo una busqueda de mas... pero bueno
        printf("\nElemento %d eliminado.", x);
        return;
    }

    printf("\nCosas raras pasan");
}

void deleteMin(quash *qh){

    if(qh->hp->num_elem==0){
        printf("\nEstructura vacia.");
        return;
    }

    //ahora hay que ver cual es el min
    int min_x = qh->hp->elem[1].value;
    delete_val(min_x,qh);
    return;


}

void free_quash(quash *qh){
    free_heap(qh->hp);
    ht_free(qh->ht);

}






//}
#endif
