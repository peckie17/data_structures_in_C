#ifndef convex_hull_h
#define convex_hull_h

#define LB 0; //lower boundary
#define UB 500009; //upper bpundary
#define SS 10; //set size, número de puntos

enum chull_method{jarvis, graham, chan};
enum print_option{p, np};

typedef struct {
    int x,y; //par de puntos x,y
} point;

typedef struct {
    point *ptarray;
    size_t ssize;
} ptset;

typedef struct {
    point *ptarray;
    size_t ssize;
    int last_i; //indice de ultimo
} stack;

typedef struct {
    point start; //puntero al elemento de start... aunque ambién podría ser la posición del arreglo
    size_t start_index;
    ptset *point_set; // el de puntos
    stack *ch_stk; //stack
    int h;
} convex_hull;

typedef struct{
    ptset *point_set;
    convex_hull **chull_array;
    int k; //k grupos
    int m; //con m puntos max
    ptset *points_minis;
    convex_hull *ch_minis; //para guardar los resultantes de los minis
} chan_chull;

point *s0;//variable global con le punto mínimos

//{ FUNCTION PROTOTYPES
ptset *new_randset(size_t set_size);
ptset *new_fileset(size_t set_size, char *file_name);
stack *new_stack(int n);
void stack_push(stack *stk, point pt);
void stack_pop(stack *stk);
point stack_next(stack *stk);
point stack_top(stack *stk);
int cross_product(point a, point b, point c);
int compare_angle(const void *gq, const void *gr);
convex_hull *new_convexhull(ptset *point_set);
convex_hull *new_convexhull_from_array(point *parr, int n_slice);
void chull_minx(convex_hull *chull);
void chull_minyx(ptset *point_set);
long long int dist_sq(point p, point q);
int compare_dist(point p, point q, point r);
void J_find_convexhull(convex_hull *chull);
void G_find_convexhull(convex_hull *chull);
void free_stack(stack *stk);
void free_ptset(ptset *pts);
void free_chull(convex_hull *chull);
void printFile_original(convex_hull *chull, char *fname);
void printFile_chull(convex_hull *chull, char *fname);
int compute_groups(int n, int m);
chan_chull *new_chan_chull(int k, int m, ptset *pts);
void C_find_convexhull(chan_chull *chan, char print);
void find_convex_hull(ptset *pts, char method, char print, int n, int m);

//}

//{ GENERAR SETS
ptset *new_randset(size_t set_size){

    ptset *point_set = malloc(sizeof(ptset));
    point_set->ssize = set_size;
    //hay que hacer un arreglo de structuras de set_size puntos
    //inicializarlos aleatoriamente
    if(point_set == NULL){
            fprintf(stderr, "\nError al alocar memoria de set\n");
            exit(2);
    }
    point_set->ptarray = malloc(sizeof(point)*set_size);

    if(point_set->ptarray == NULL){
            fprintf(stderr, "\nError al alocar memoria de arreglo\n");
            exit(2);
    }

    for(int i=0; i<set_size; i++){ //inicializar x,y
        point_set->ptarray[i].x = rand()%UB;
        point_set->ptarray[i].y = rand()%UB;
    }

    return point_set;
}

ptset *new_fileset(size_t set_size, char *file_name){

    ptset *point_set = malloc(sizeof(ptset));
    point_set->ssize = set_size;
    //hay que hacer un arreglo de structuras de set_size puntos
    //inicializarlos aleatoriamente
    if(point_set == NULL){
            fprintf(stderr, "\nError al alocar memoria de set\n");
            exit(2);
    }
    point_set->ptarray = malloc(sizeof(point)*set_size);

    if(point_set->ptarray == NULL){
            fprintf(stderr, "\nError al alocar memoria de arreglo\n");
            exit(2);
    }

    FILE *pFile = fopen(file_name, "rt");

    if(pFile == NULL){
        fprintf(stderr, "Cannot open command file!\n");
        exit(3);
    }

    size_t h = 0;

    char command[100];
    while( fgets(command, 100, pFile) != NULL){


        command[strlen(command)-1] = '\0'; // remove new line character

        int x, y;
        sscanf(command, "%d %d", &x, &y);
        point_set->ptarray[h].x = x;
        point_set->ptarray[h].y = y;


        if(h >= set_size)
            break;

        h++;

    }
    if(h<set_size){
        fprintf(stderr,"\nSolo se encontraron %d puntos, se pidieron %d.\n", h, set_size);
        exit(10);
        point_set->ssize = h;
    }

    fclose(pFile);

    return point_set;
}

//}

//{ STACK
stack *new_stack(int n){

    stack *stk = malloc(sizeof(stack));

    if(stk == NULL){
        fprintf(stderr, "\nError para reservar memoria para stack.\n");
        exit(3);
    }

    stk->ssize = n;
    stk->last_i = -1;//aun no tiene nada

    stk->ptarray = malloc(sizeof(point)*n);

    if(stk->ptarray == NULL){
        fprintf(stderr, "\nError para reservar memoria para arrat de stack.\n");
        exit(3);
    }

    //no creo que lo tenga que inicializar

    return stk;
}

void stack_push(stack *stk, point pt){//insertar elemento

    stk->last_i++;

    stk->ptarray[stk->last_i] = pt;
}

void stack_pop(stack *stk){//eliminar último

    if(stk->last_i < 0)
        return; //no hay nada que sacar

    stk->last_i--;//no hay necesidad de cambiarlo, solo lo voy a ignorar

}

//regreso un puntero o una estructura?
point stack_next(stack *stk){//el penúltimo

    return (stk->ptarray[stk->last_i-1]);
}

point stack_top(stack *stk){//el penúltimo

    return (stk->ptarray[stk->last_i]);
}

//}

//{ ANGULOS

int cross_product(point a, point b, point c){
    long long int cp = (b.x-a.x)*(c.y-a.y) - (c.x-a.x)*(b.y-a.y);

    if(cp<0)
        return -1;

    if(cp>0)
        return 1;

    else
        return 0;

    //return (b.x-a.x)*(c.y-a.y) - (c.x-a.x)*(b.y-a.y);

        //si es >0 es CCW (a la izq) y si es <0 es CW (a la der)
        //si es 0 es colineal
}

////funcion para qsort, neceista const void, dentro hago el casteo
//oridenar según sean ccw (descendente)
int compare_angle(const void *gq, const void *gr){

    point *q = (point *)gq;  //casteo a tipo punto
    point *r = (point *)gr;

    return cross_product(*s0, *q, *r)*(-1);//los contenidos

}

//}

//{ FUNCIONES CONVEX HULL
convex_hull *new_convexhull(ptset *point_set){

    int n = point_set->ssize;
    convex_hull *chull = malloc(sizeof(convex_hull));
    if(chull == NULL){
        fprintf(stderr, "\nError al alocar memoria de chull\n");
        exit(2);
    }

    chull->point_set = point_set;
    chull->h = 0;

    //crear un stack?0
    chull->ch_stk = new_stack(n);

    return chull;

}

convex_hull *new_convexhull_from_array(point *parr, int n_slice){

    convex_hull *chull = malloc(sizeof(convex_hull));
    if(chull == NULL){
        fprintf(stderr, "\nError al alocar memoria de chull\n");
        exit(2);
    }

    ptset *point_set = malloc(sizeof(ptset));
    if(chull == NULL){
        fprintf(stderr, "\nError al alocar memoria de chull pset\n");
        exit(2);
    }

    point_set->ssize = n_slice;
    point_set->ptarray = parr;
    chull->point_set = point_set;

    chull->h = 0;

    //crear un stack?0
    chull->ch_stk = new_stack(n_slice);

    return chull;

}

//para encontrar el leftmost
void chull_minx(convex_hull *chull){

    size_t min = 0; //el indice minimo
    for(int i=0; i<chull->point_set->ssize; i++){
        if(chull->point_set->ptarray[i].x < chull->point_set->ptarray[min].x){
            min = i;
        }
        //si no... pues no hago nada

    }

    s0 = &(chull->point_set->ptarray[min]);
    chull->start = (chull->point_set->ptarray[min]);
    chull->start_index = min;

}

//encontrar minimo y (y leftmost)
void chull_minyx(ptset *point_set){

    size_t n = point_set->ssize;
    point *parr = point_set->ptarray;
    size_t min = 0; //indice del minimo

    for(size_t i=0; i<n; i++){

        if(parr[i].y < parr[min].y){
            min = i;
        }

        if(parr[i].y == parr[min].y){//escoger la x más pequeña
            if(parr[i].x < parr[min].x)
                min = i;
        }

    }

    //ahora hay que swapear el 0 con el índice min

    point aux = parr[min];
    parr[min] = parr[0];
    parr[0] = aux;

    s0 = &(parr[0]); //la dirección del mínimo
}

//distancia al cuadrado
long long int dist_sq(point p, point q){
    return (q.x-p.x)*(q.x-p.x)+(q.y-p.y)*(q.y-p.y);
}

int compare_dist(point p, point q, point r){

    long long int d = dist_sq(p,q) - dist_sq(p,r);

    if(d>0)
        return 1;

    if(d<0)
        return -1;

    else
        return 0;
    //>0 si pq es mayor
    //<0 si pr es mayor
}

//}

//{ FIND CONVEX_HULL

void J_find_convexhull(convex_hull *chull){

    point *points = chull->point_set->ptarray;
    size_t n = chull->point_set->ssize;
    chull_minx(chull);

    size_t p0, p, q;    //vpy a usar los indices como comparacion
    int h=0;

    p0 = chull->start_index;//empiezo por el left most
    p  = p0;

        //ahora que ya tengo mi origen, tengo que ver por dónde empiezo, ...por cero, nomás hay que cuidar que no sea el start
    while(1){// no sé cuantas iteraciones voy a hacer
        q = (p+1)%n; //el siguiente después del start o de p?//debería ser de p
                        //porqur cuando es q=s0, se debe de detener


        for(int r=0; r < n; r++){

            if(r == p || r == q){//si son iguales no lo pelo //va a salir colineales
                continue;
            }

            //asumiendo que ya son diferentes
            int ccw = cross_product(points[p], points[q], points[r]); //hayar la orientación
            if(ccw>0){// significa que está a la izquierda -> es el nuevo current?
                q = r;
            }

            if(ccw==0){//son colineales, que el current sea el más lejano a origin
                if( compare_dist(points[p], points[q], points[r])<0 ){
                    //significa que r es mayor, es el que queremos tomar
                    q = r;
                }

            }

            if(ccw<0){//significa que está a la derecha, entonces no sirve
                //continua la iteración? no hago nada
                continue;
            }

        }
        //printf("\nFin %d\n",h);
        stack_push(chull->ch_stk, points[p]);
        //pero si q es s0, es que ya terminé
        if( q == p0)
            break;

        //ya terminamos el ciclo, significa que ya tenemos nuestra nueva p
        //que es q

        p = q;
        h++;
    }
}

void G_find_convexhull(convex_hull *chull){
    //primero encontrar el mínimo y ordenar

    size_t n = chull->point_set->ssize;
    point *parr = chull->point_set->ptarray;


    chull_minyx(chull->point_set);
    qsort(&(chull->point_set->ptarray[1]), n-1, sizeof(point), compare_angle);

    stack_push(chull->ch_stk, parr[0]);
    stack_push(chull->ch_stk, parr[1]);
    stack_push(chull->ch_stk, parr[2]);

    int ccw; //tal vez debería ponerla de argumento en el while
    for(int t=3; t<n; t++){

        while(1){

            ccw = cross_product(stack_next(chull->ch_stk), stack_top(chull->ch_stk), parr[t]);

            if(ccw > 0)
                break;

            if(ccw == 0){
                if( compare_dist(*s0, stack_top(chull->ch_stk), parr[t]) > 0 )
                    break;
                else
                    stack_pop(chull->ch_stk);
            }

            if(ccw < 0)
                stack_pop(chull->ch_stk);

        }


        stack_push(chull->ch_stk, parr[t]);//el que está más a la izquierda

    }

    //se supone que ya está
}

//}

//{ FREE
//no sé porqué no funciona
void free_stack(stack *stk){

    free(stk->ptarray);
    free(stk);
}

void free_ptset(ptset *pts){
    free(pts->ptarray);
    free(pts);
}

void free_chull(convex_hull *chull){
    printf("\nIntento liberar");

    free_stack(chull->ch_stk);
    printf("\nPude con stack");

    free_ptset(chull->point_set);
    printf("\nPude con point set");

    free(chull);
    printf("\nPude con chull");
}

//}

//{ IMPRIMIR A FILE

void printFile_original(convex_hull *chull, char *fname){

    FILE *ft = fopen(fname, "w");
    if(ft == NULL){
        fprintf(stderr, "Cannot open command file!\n");
        return;
    }

    printf("\nSET DE PUNTOS\n");
    for(size_t i=0; i<chull->point_set->ssize; i++){
        fprintf(ft, "%d\t%d\n", chull->point_set->ptarray[i].x, chull->point_set->ptarray[i].y);
    }

    fclose(ft);
}

void printFile_chull(convex_hull *chull, char *fname){

    FILE *fs = fopen(fname, "w");
    if(fs == NULL){
        fprintf(stderr, "Cannot open command file!\n");
        return;
    }

    printf("\nSET DE CH\n");
    for(int i=0; i<=chull->ch_stk->last_i; i++){
        fprintf(fs, "%d\t%d\n", chull->ch_stk->ptarray[i].x, chull->ch_stk->ptarray[i].y);
    }

    fclose(fs);
}

//}

//{ CHAN

int compute_groups(int n, int m){

    int k=n/m;
   int r=n%m;

    if(r!=0){
        while(r<3){
            m--;
            k=n/m;
            r=n%m;
        }
        if(r!=0)
            k++;
    }

    printf("\nCon %d puntos en %d grupos de %d<=m<=%d", n, k, r, m);


    return k;

}


chan_chull *new_chan_chull(int k, int m, ptset *pts){

    chan_chull *chan = malloc(sizeof(chan_chull));

    if(chan == NULL){
        fprintf(stderr,"\nNo se pudo reservar memoria para Chan");
        exit(4);
    }

    size_t n = pts->ssize;
    chan->point_set = pts;
    chan->k = k;
    chan->m = m;

    chan->chull_array = malloc(sizeof(convex_hull)*k);

    if(chan->chull_array == NULL){

        fprintf(stderr,"\nNo se pudo reservar memoria para Chan array");
        exit(4);
    }
    int s, pr;
    for(size_t i=0; i<k; i++){
        s=m;//tamaño del slice del array
        pr=n-i*m;//puntos restantes
        if(pr<m)
            s=pr;
        chan->chull_array[i] = new_convexhull_from_array(&(pts->ptarray[i*m]), s); //eso me regresa un puntero, y quiero el contenido //pero no es mucha memoria? tendria que liberarla
                                                                //tal vez un arreglo de punteros
    }

    return chan;
}

void C_find_convexhull(chan_chull *chan, char print){

    char fname_0k[100];
    char fname_fk[100];

    int k=chan->k;
    int n_minis = 0; //puntos resultantes de los nmnis
    for(int i=0; i<k; i++){
        //printf("\n\n Grupo %d\n", i);

        if(print==p){
            sprintf(fname_0k, "puntos_originales_%d.txt", i);
            printFile_original(chan->chull_array[i], fname_0k);
        }

        G_find_convexhull(chan->chull_array[i]);
        if(print==p){
            sprintf(fname_fk, "puntos_chull_%d.txt", i);
            printFile_chull(chan->chull_array[i], fname_fk);
        }
        n_minis += chan->chull_array[i]->ch_stk->last_i +1 ;
    }

    //ya tengo la longitud de los nmins
    //crear un ptset de esa longitud y copiar los puntos a ese set

    chan->points_minis = malloc(sizeof(ptset));
    chan->points_minis->ssize = n_minis;
    if(chan->points_minis == NULL){
        fprintf(stderr, "\nError al alocar memoria de set de minis\n");
        exit(6);
    }

    chan->points_minis->ptarray = malloc(sizeof(point)*n_minis);
    if(chan->points_minis->ptarray == NULL){
        fprintf(stderr, "\nError al alocar memoria de arreglo para minis\n");
        exit(6);
    }

    //ahora a copiar todos del stack de cada grupo
    int t=0;
    for(int i=0; i<chan->k; i++){

        for(int j=0; j<=chan->chull_array[i]->ch_stk->last_i;j++){
            chan->points_minis->ptarray[t] = chan->chull_array[i]->ch_stk->ptarray[j];
            t++;
        }
    }

    chan->ch_minis = new_convexhull(chan->points_minis);
    J_find_convexhull(chan->ch_minis);

    if(print==p)
        printFile_chull(chan->ch_minis, "puntos_chull_chan.txt");

}

//}

//{ Escoger método

void find_convex_hull(ptset *pts, char method, char print, int n, int m){

    if(method==jarvis){
        convex_hull *chull = new_convexhull(pts);
        J_find_convexhull(chull);

        if(print==p)
            printFile_chull(chull, "puntos_chull_jarvis.txt");

        return;
    }

    if(method==graham){
        convex_hull *chull = new_convexhull(pts);
        G_find_convexhull(chull);

        if(print==p)
            printFile_chull(chull, "puntos_chull_graham.txt");

        return;
    }

    if(method==chan){
        int k = compute_groups(n,m);
        chan_chull *chan = new_chan_chull(k, m, pts);
        C_find_convexhull(chan, print);
        return;
    }

    else{
        fprintf(stderr,"\nNo es un metodo valido: jarvis, graham, chan\n");
        exit(0);
    }




}


#endif
