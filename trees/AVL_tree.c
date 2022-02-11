#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

enum print_order {in_o, pre_o, post_o};

struct avl_node{
	int elem;
	struct avl_node *left;
	struct avl_node *right;
	int height;
};

typedef struct avl_node avl_tree;

int height( avl_tree *n ){
	if(n == NULL)
		return -1;
	else
		return n->height;
}

int max(int a, int b){
    return a>b?a:b;}

avl_tree* rot_LL(avl_tree *k2){
	 //printf("\nTodo cool 20");
	avl_tree *k1;
     //printf("\nTodo cool 21");
	k1 = k2->left;
	 //printf("\nTodo cool 22");
	k2->left = k1->right;
	 //printf("\nTodo cool 23");
	k1->right = k2;
     //printf("\nTodo cool 24");
	//actualizar alturas
	k2->height = max(height(k2->left), height(k2->right))+1;
	k1->height = max(k2->height, height(k1->left))+1;

	return k1;
}

avl_tree* rot_RR(avl_tree *k1){

	 //printf("\nTodo cool 7");
	avl_tree *k2;

	k2 = k1->right;
	k1->right = k2->left;
	k2->left = k1;


	//actualizar alturas
	k1->height = max(height(k1->left), height(k1->right))+1;
	k2->height = max(k1->height, height(k2->right))+1;
     //printf("\nTodo cool 8");
	return k2;
}


avl_tree* rot_LR(avl_tree *k3){
	//avl_tree *k1, *k2;

	k3->left = rot_RR(k3->left);
	return rot_LL(k3);
	//no me tengo que preocupar por alturas
}

avl_tree* rot_RL(avl_tree *k1){
	//avl_tree *k1, *k2;
     //printf("\nTodo cool 11");
	k1->right = rot_LL(k1->right);
	 //printf("\nTodo cool 12");
	return rot_RR(k1);
	//no me tengo que preocupar por alturas
}

avl_tree* find(avl_tree *n, int x){

    if(n==NULL)
        return NULL;

    if(n->elem == x)
        return n;

    if(x < n->elem)
        return find(n->left, x);

    else
        return find(n->right, x);


}


//debería ser void?: nope porque es recursivo

avl_tree *avl_insert(avl_tree *n, int x){

    //nodo disponible - caso base
    if( n == NULL ){
        n = malloc(sizeof(avl_tree));

        if(n==NULL){
            printf("\nNo se pudo insertar %d", x);
            return n;
        }

        n->elem = x;
        n->left = NULL;
        n->right = NULL;
        n->height = 0;//nodo nuevo

        return n;

    }

    //nodo no disponible: menor ->checar izq
    if( x < n->elem ){

        n->left = avl_insert(n->left, x);
                //preguntar si tengo violacion avl
                //si lo hacemos qui ya sabemos que solo es un L, y tenemos que ver si es un LR o LR

        /*creo que si es necesario modificar la altura al ppio*/ //no creo que no importa, ya que solo vamos a revisar la altura de los hijos
        n->height = max(height(n->left), height(n->right))+1;

        if( height(n->left) > (height(n->right)+1)) { // si sí, tenemos un probleam de left

            if( height(n->left->left) >= height(n->left->right)){//LL
                return rot_LL(n);
            }
            else //LR: puede que caiga en el >=
                return rot_LR(n);
        }
        else
            return n;
            //n->height = max(height(n->left), height(n->right))+1;

    }

    //nodo no disponible: menor ->checar der
    else if( x > n->elem ){
        n->right = avl_insert(n->right, x);

        n->height = max(height(n->left), height(n->right))+1;
         //printf("\nTodo cool 1");
        if( height(n->right) > (height(n->left)+1) ){ // si sí, tenemos un probleam de right
         //printf("\nTodo cool 2");
            if( height(n->right->right) >= height(n->right->left)){//RL
                 //printf("\nTodo cool 3");
                return rot_RR(n);
            }
            else{ //RL
                //printf("\nTodo cool 4");
                return rot_RL(n);


            }
        }
        else
            return n;

    }

    else{ // x=n   debe ser igual, ya no tendríamosm,que usar otra
        //printf("\nYa estaba presente %d en %d", x, n->elem);
        return n;
    }

}
//{IMPRIMIR
void avl_in_order(avl_tree *n){
    if(n==NULL){
        return;
    }


    avl_in_order(n->left);
    printf("%d ", n->elem);
    avl_in_order(n->right);

}

void avl_pre_order(avl_tree *n){
    if(n==NULL){
        return;
    }

    printf("%d ", n->elem);
    avl_pre_order(n->left);
    avl_pre_order(n->right);

}

void avl_post_order(avl_tree *n){
    if(n==NULL){
        return;
    }


    avl_post_order(n->left);
    avl_post_order(n->right);
    printf("%d ", n->elem);
}

void avl_print(avl_tree *root, const char print_order){
    printf("\nAVL: [");
    if(print_order == in_o){
        avl_in_order(root);
        //return;
    }

    if(print_order == pre_o){
        avl_pre_order(root);
        //return;
    }

    if(print_order == post_o){
        avl_post_order(root);
        //return;
    }

    printf("]\n");
}

//}end imprimir


avl_tree *avl_del_max(avl_tree *n, avl_tree *del){

    if( n->right == NULL ){//ya es el maximo

        del->elem = n->elem;
        avl_tree *n_left = n->left;
        free(n);
        return n_left;
    }

    //aun no es el maximo
    n->right = avl_del_max(n->right, del);

    //checar violaciones
        //si hay un problema, va a ser L

    n->height = max(height(n->left), height(n->right))+1;
    if( height(n->left)>(height(n->right)+1) ){

        //LL
        if( height(n->left->left)>=height(n->left->right) ){
            return rot_LL(n);
        }
        else //LR
            return rot_LR(n);
    }
    else
        return n;
}


avl_tree *avl_delete(avl_tree *n, int x){
    //printf("\nLlegue aqui 0 con %d", n->elem);

    if( n==NULL ){
        //printf("\nNo se encontro %d", x);
        return n;
    }

    if( x<n->elem ){
        //printf("\nLlegue aqui L");
        n->left = avl_delete(n->left, x);
        //checar propiedades AVL
            //si eliminamos de L, tendremos problemas en R

        n->height = max(height(n->left), height(n->right))+1;

        if( height(n->right) > (height(n->left)+1) ){
            //RR
            if( height(n->right->right)>=height(n->right->left) ){
                return rot_RR(n);
            }
            else //RL
                return rot_RL(n);
        }
        else
            return n;
    }

    if( x>n->elem ){
        //printf("\nLlegue aqui R");
        n->right = avl_delete(n->right, x);

        //checar propiedades AVL
            //si eliminamos de R, habra problema L

        if( height(n->left) > (height(n->right)+1) ){

            //LL
            if( height(n->left->left)>= height(n->left->right) ){
                return rot_LL(n);
            }
            else //LR
                return rot_LR(n);
        }

    }

    //aqui creo que no tengo que checar violaciones
    //tengo que checar

    if( x==n->elem ){//se encontro el elemento
        //printf("\nLlegue aqui =");
        if( n->left != NULL ){//como quité de L, puede ocasionar problema en R

            n->left = avl_del_max(n->left, n);

            n->height = max(height(n->left), height(n->right))+1;

            if( height(n->right) > (height(n->left)+1) ){
                //RR
                if( height(n->right->right)>=height(n->right->left) ){
                    return rot_RR(n);
                }
                else //RL
                    return rot_RL(n);
            }
            else
                return n;

        }
        else{
            //printf("\nLlegue aqui =H");
            avl_tree *n_right = n->right;
            free(n);
            return n_right;
        }

    }


        return n;
}

void avl_free(avl_tree *n){
    if(n==NULL){
        return;
    }

    avl_free(n->left);
    avl_free(n->right);
    free(n);
}



int main(int argc, const char * argv[]) {

    avl_tree *root = NULL;

    int N=100000;

    /*int n=8;
    int arr[8]={50,75,70,100,69,71,90,103};*/

    clock_t tic = clock();

    printf("\nINSERTANDO");
    for(int i=0; i<N; i++){
        root=avl_insert(root, rand());
    }

    printf("\nBORRANDO");
    for(int i=0; i<N; i++){
        root=avl_delete(root, rand());
    }


    clock_t toc = clock();
    double tictoc = (double)(toc - tic) / CLOCKS_PER_SEC;

    printf("\nN: %d- Tiempo: %f",N, tictoc);



    avl_free(root);

    return 0;
}
