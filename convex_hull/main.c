
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <inttypes.h>
#include "convex_hull.h"



int main(int argc, const char * argv[]) {

   int n =100;
   int m = n/10;

    ptset *point_set = new_fileset(n, "./puntos_mexico_1k.txt");
    find_convex_hull(point_set, jarvis, p, n, m);

   return 0;
}



