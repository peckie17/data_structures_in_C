#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

enum print_order {in_o, pre_o, post_o};

struct bst_node{
	int elem;
	struct bst_node *left;
	struct bst_node *right;
	//int height;
};

typedef struct bst_node bst_tree;

bst_tree *bst_find(bst_tree *n, int x){

    if(n==NULL){
        printf("\nNo se encontro %d", x);
        return n;
    }

    if(x < n->elem){
        return bst_find(n->left, x);
    }

    if(x > n->elem){
        return bst_find(n->right, x);
    }

    else{//deberia ser igual
        printf("Se encontro %d en %d", x, n->elem);
        return n;
    }

}

bst_tree *bst_insert(bst_tree *n, int x){

    if(n==NULL){//si es nulo se inserta
        n = malloc(sizeof(bst_tree));
        if(n==NULL){
            fprintf(stderr, "\nNo se pudo insertar");
            return n;
        }

        //si se pudo alocar memoria
        n->elem = x;
        n->right = NULL;
        n->left = NULL;


        return n;
    }

    //no es nulo, emepzar la recursion
    if( x < n->elem ){//insertar a la izq
        n->left = bst_insert(n->left, x);
        return n;
    }

    else if( x > n->elem ){//insertar a la der
        n->right = bst_insert(n->right, x);
        return n;
    }

    else{
        //fprintf(stderr, "\nYa se encontraba el elemento");
        return n;
    }

}

void bst_in_order(bst_tree *n){
    if(n==NULL){
        return;
    }


    bst_in_order(n->left);
    printf("%d ", n->elem);
    bst_in_order(n->right);

}

void bst_pre_order(bst_tree *n){
    if(n==NULL){
        return;
    }

    printf("%d ", n->elem);
    bst_pre_order(n->left);
    bst_pre_order(n->right);

}

void bst_post_order(bst_tree *n){
    if(n==NULL){
        return;
    }


    bst_post_order(n->left);
    bst_post_order(n->right);
    printf("%d ", n->elem);
}

void bst_print(bst_tree *root, const char print_order){
    printf("\nBST: [");
    if(print_order == in_o){
        bst_in_order(root);
        //return;
    }

    if(print_order == pre_o){
        bst_pre_order(root);
        //return;
    }

    if(print_order == post_o){
        bst_post_order(root);
        //return;
    }

    printf("]\n");
}

//ya asumimos que sí existe ese sub árbol izq
bst_tree *bst_del_max(bst_tree *n, bst_tree *del){//del es el que quiero eliminar

    if(n->right == NULL){
            //ya no tiene hijos a la derecha, significa que es el max  //con este es el que voy a reemplazar


        del->elem = n->elem; //solo copio el valor
            //tengo que liberar el nodo n... creo
            //pero tengo que regresar su hijo izq
        bst_tree *n_left=n->left; //sea nulo o no, regreso su direccion
        free(n);
        return n_left;
    }

        //aun no es el maximo, entonces, seguir buscando en el derecho
    n->right = bst_del_max(n->right, del);
        //pero a que se iguala? al mismo right?
    return n;//creo que si es un return n
}

bst_tree *bst_del_min(bst_tree *n, bst_tree *del){

    if(n->left == NULL){// ya es el menor
        del->elem = n->elem; //solo copio el valor
        bst_tree *n_right=n->right; //sea nulo o no, regreso su direccion
        free(n);
        return n_right;
    }

    //si no, seguir buscando a la izq
    n->left = bst_del_min(n->left, del);
    return n;
}


bst_tree *bst_delete(bst_tree *n, int x){
/*La idea general es:
    lo busco, ya que lo encuentro*/

    if(n==NULL){
        //fprintf(stderr, "\nNo se encontro el elemento");
        return n;
    }

    if(x < n->elem){
        n->left=bst_delete(n->left, x);
        return n;
    }

    if(x > n->elem){
        n->right=bst_delete(n->right, x);
        return n;
    }

    if(x == n->elem){// lo encontré... ahora hay que susitituir el valor
        //printf("\nEncontre %d en %d", x, n->elem);

        if(n->left != NULL){//existe arbol izq, sustituir por el max
            n->left = bst_del_max(n->left, n);
            return n;
        }

        else{
            bst_tree *n_right=n->right;
            free(n);
            return n_right; //sea nulo o no
        }

        //es una hoja->solo hay que hacerle un free y regresar null

    }

    fprintf(stderr,"\nNunca deberia de llegar aqui");
}

void bst_free(bst_tree *n){
    if(n==NULL){
        return;
    }

    bst_free(n->left);
    bst_free(n->right);
    free(n);
}




int main(int argc, const char * argv[]) {

    bst_tree *root = NULL;
    int N=100000;

    int n=8;
    int arr[8]={50,75,70,100,69,71,90,103};

    clock_t tic = clock();

    printf("\nINSERTANDO");
    for(int i=0; i<N; i++){
        root=bst_insert(root, rand());
    }

    printf("\nBORRANDO");
    for(int i=0; i<N; i++){
        root=bst_delete(root, rand());
    }


    clock_t toc = clock();
    double tictoc = (double)(toc - tic) / CLOCKS_PER_SEC;

    printf("\nN: %d- Tiempo: %f",N, tictoc);


    bst_free(root);

    return 0;
}
