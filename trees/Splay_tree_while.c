#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

enum print_order {in_o, pre_o, post_o};

struct splay_node{
	int elem;
	struct splay_node *left;
	struct splay_node *right;
	struct splay_node *parent;
	//int height;
};

typedef struct splay_node splay_tree;

//se llaman sobre la cabeza, no sobre x
splay_tree *L_zig(splay_tree *p){

    splay_tree *x=p->left;

    p->left=x->right;
    if(p->left)
        p->left->parent=p;

    x->right=p;
    x->parent=p->parent; //deberia ser nulo

    if(p->parent!=NULL){
        //ahora hay que ver si p es el hijo derecho o izq

        if(p==p->parent->left)//es el hijo izq
            p->parent->left=x;

        else
            p->parent->right=x;
    }


    p->parent=x;

    //x.left no se cambia
    //regresamos la cabeza
    return x;
}

splay_tree *R_zag(splay_tree *p){
    //printf("\nTodo cool dRzag 0");
    splay_tree *x=p->right;

    p->right=x->left;

    if(p->right)
        p->right->parent=p;
    x->left=p;
    x->parent=p->parent; //deberia ser nulo// no necesariamente

    if(p->parent!=NULL){
        //ahora hay que ver si p es el hijo derecho o izq

        if(p==p->parent->left)//es el hijo izq
            p->parent->left=x;

        else
            p->parent->right=x;
    }

    p->parent=x;

    //x.right no se cambia, regreso la cabeza
    return x;
}


//se llama sobre la cabeza, el abuelo
splay_tree *L_zigzig(splay_tree *g){

    splay_tree *n; //no creo que haya que crear un nuevo nodo, podemos usar g
    //splay_tree *g=x->parent->parent; //no se usa

    //p será la nueva cabeza
    n=L_zig(g);//a quien se iguala? a la nueva cabeza?
                //se supone que x esta a su izq

    return L_zig(n);// se suppone que x ya esta en la cabeza

}

splay_tree *R_zagzag(splay_tree *g){

    splay_tree *n; //no creo que haya que crear un nuevo nodo, podemos usar g
    //splay_tree *g=x->parent->parent; //no se usa

    //p será la nueva cabeza
    n=R_zag(g);//a quien se iguala? a la nueva cabeza?
                //se supone que x esta a su izq

    return R_zag(n);// se suppone que x ya esta en la cabeza

}

splay_tree *LR_zigzag(splay_tree *g){

    splay_tree *n;                            /*  g
                                                 /
                                                p
                                                 \
                                                  x   */

    g->left=R_zag(g->left);//p esta a la izqierda

    return L_zig(g);// se suppone que x ya esta en la cabeza
}

splay_tree *RL_zagzig(splay_tree *g){

    splay_tree *n;                            /*  g
                                                   \
                                                    p
                                                   /
                                                  x   */

    g->right=L_zig(g->right);//p esta a la izqierda

    return R_zag(g);// se suppone que x ya esta en la cabeza

}


splay_tree *splay(splay_tree *x){

    //printf("\nTodo cool 2: %d p- %d", x->elem, x->parent->elem);
    while( x->parent ){//mientras tenga padre
        //printf("\nTodo cool 3");
        if( !(x->parent->parent) ){// si no tiene abuelo, rptacion sencilla
            //printf("\nTodo cool 4");
            //hay que ver si es zig o zag
            if( x->elem < x->parent->elem ){// esta a la izq, es zig
                //printf("\nTodo cool 5 Lzig");
                x=L_zig(x->parent);
            }
            else{//esta a la der, es zag
                //printf("\nTodo cool 6 Rzag");
                x=R_zag(x->parent);
                //printf("\nTodo cool 6f Rzag");
            }
        }
        else{//tiene abuelos
            if( x->elem < x->parent->elem && x->parent->elem < x->parent->parent->elem ){//ambos a la izq
                x=L_zigzig(x->parent->parent);
            }
            else if( x->elem > x->parent->elem && x->parent->elem > x->parent->parent->elem ){//ambos a la der
                x=R_zagzag(x->parent->parent);
            }
            else if( x->elem < x->parent->elem && x->parent->elem > x->parent->parent->elem ){//RL
                x=RL_zagzig(x->parent->parent);
            }
            else if( x->elem > x->parent->elem &&  x->parent->elem < x->parent->parent->elem ){
                x=LR_zigzag(x->parent->parent);
            }
            else{
                printf("\nSplay: Nunca debería de pasar esto");
            }
        }
    }
    //al salir del while x a debería de estar a la cabeza
    return x;
}



splay_tree *splay_insert(splay_tree *root, int x){

    splay_tree *n = root;
    splay_tree *p = NULL;

    char LR=0;

    while(n){//mientras sea diferente de nulo

        if( x<n->elem ){
            p = n;
            n = n->left;
            LR=1;
        }

        else if( x>n->elem ){
            p = n;
            n = n->right;
            LR=2;
        }

        else{//son iguales
            //printf("\n%d ya estaba presente en %d", x, n->elem);
            return splay(n);
        }
    }

    //encontro un nodo vacio
    n = malloc(sizeof(splay_tree));
    n->elem=x;
    n->left = NULL;
    n->right = NULL;
    n->parent = p;

    if(LR!=0){
        if(LR==1){
            p->left = n;
        }
        else
            p->right = n;
    }

    return splay(n);

}

splay_tree *splay_find(splay_tree *root, int x){

    splay_tree *n = root;

    while(n){//mientras sea diferente de nulo

        if( x<n->elem ){
            n = n->left;
        }

        else if( x>n->elem ){
            n = n->right;
        }

        else{//son iguales
            //printf("\nSe encontro %d en %d", x, n->elem);
            return splay(n);
        }
    }

    //si llego hasta aca esporque no lo encontro
    //printf("\nNo se encontro %d", x);
    return root;

}

splay_tree *splay_max(splay_tree *n){
//le mando el izquierdo, ya no debería de tener padre

    while(n->right){//mientras tenga hijos derechos
        n=n->right;
    }
    //ya no tiene hijos derechos, ya es el maximo
    //tendria que splayearle
    //pero hay un peuqeño problema
    //tiene padre

    return splay(n);// se supone que como es el maximo, no debería de tener hijos izquierdos



}



splay_tree *splay_delete(splay_tree *root, int x){
//primero se busca
    splay_tree *n = root;


    while(n){//mientras sea diferente de nulo

        if( x<n->elem ){
            n = n->left;
        }

        else if( x>n->elem ){
            n = n->right;
        }

        else{//son iguales
            //printf("\nSe encontro %d en %d", x, n->elem);
            n=splay(n);// se splayea el nodo y obtenemos dos árboles: L y R

            splay_tree *L = n->left;
            splay_tree *R = n->right;
            free(n);
            //tal vez qui ya le podemos hacer un free


            //hay que buscar el maximo de L... si existe
            if(L){
                L->parent=NULL;
                L=splay_max(L);
                L->right=R;
            }

            if(R){//si existe R
                if(L)
                    R->parent=L;
                else{
                    //si no existe L
                    R->parent=NULL;
                    //hay que splayear L? Nah, solo hay que regresar el árbol R
                    return R;
                }
            }

            return L;// sea nulo o no

        }
    }

    //No se encontró
    //printf("\nNo se encontro %d", x);
    return root;




}


//{IMPRIMIR
void splay_in_order(splay_tree *n){
    if(n==NULL){
        return;
    }

    splay_in_order(n->left);
    printf("%d ", n->elem);
    splay_in_order(n->right);

}

void splay_pre_order(splay_tree *n){
    if(n==NULL){
        return;
    }

    printf("%d ", n->elem);
    splay_pre_order(n->left);
    splay_pre_order(n->right);

}

void splay_post_order(splay_tree *n){
    if(n==NULL){
        return;
    }


    splay_post_order(n->left);
    splay_post_order(n->right);
    printf("%d ", n->elem);
}

void splay_print(splay_tree *root, const char print_order){
    printf("\nSplay: [ ");
    if(print_order == in_o){
        splay_in_order(root);
        //return;
    }

    if(print_order == pre_o){
        splay_pre_order(root);
        //return;
    }

    if(print_order == post_o){
        splay_post_order(root);
        //return;
    }

    printf("]\n");
}

//}end imprimir

void splay_free(splay_tree *n){
    if(n==NULL){
        return;
    }

    splay_free(n->left);
    splay_free(n->right);
    free(n);
}




int main(int argc, const char * argv[]) {

    int N=200000;
    splay_tree *root = NULL;


    clock_t tic = clock();

    printf("\nINSERTANDO");
    for(int i=0; i<N; i++){
        root=splay_insert(root, rand());
    }

    printf("\nBORRANDO");
    for(int i=0; i<N; i++){
        root=splay_delete(root, rand());
    }


    clock_t toc = clock();
    double tictoc = (double)(toc - tic) / CLOCKS_PER_SEC;

    printf("\nN: %d- Tiempo: %f",N, tictoc);

    splay_free(root);

    return 0;
}








