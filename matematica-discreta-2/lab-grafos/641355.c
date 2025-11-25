#include "Rii.h"

//Estructura usada en OrdenWelshPowell, RMBCrevierte y RMBCnormal.
struct aux {
    u32 comp;
    u32 ind;
};

//Estructura para la función RMBCchicogrande.
struct aux2 {
    u32 comp;
    u32 ind;
    u32 cant_col;
};

//calcula el maximo entre dos numeros
u32 maximo(u32 num1, u32 num2){return (num1 > num2 ) ? num1 : num2;}

//calcula el minimo entre dos numeros
u32 minimo(u32 num1, u32 num2){return (num1 < num2 ) ? num1 : num2;}

//usada en qsort para ordenar un arreglo de menor a mayor
int cmpfunc (const void * a, const void * b) {
   return ( *(u32*)a > *(u32*)b );
}

//usada en qsort para ordenar un arreglo usando un campo de su estructura de mayor a menor
int comp_dec_aux(const void *v1, const void *v2) {
  const struct aux2 *p1 = v1;
  const struct aux2 *p2 = v2;
  if (p1->comp < p2->comp)
    return(+1);
  else if (p1->comp > p2->comp)
    return(-1);
  else
    return(0);
}

//usada en qsort para ordenar un arreglo usando un campo de su estructura de menor a mayor
int comp_cre_aux(const void *v1, const void *v2) {
  const struct aux2 *p1 = v1;
  const struct aux2 *p2 = v2;
  if (p1->comp > p2->comp)
    return(+1);
  else if (p1->comp < p2->comp)
    return(-1);
  else
    return(0);
}

//usada en qsort para ordenar un arreglo usando dos campos de su estructura de menor a mayor
int col_cre_aux(const void *v1, const void *v2) {
  const struct aux2 *p1 = v1;
  const struct aux2 *p2 = v2;
  if (p1->cant_col > p2->cant_col)
    return(+1);
  else if (p1->cant_col < p2->cant_col)
    return(-1);
  else if (p1->comp > p2->comp)
    return(+1);
  else if (p1->comp < p2->comp)
    return(-1);
  else
    return(0);
}

//Es para verificar que el formato sea el correcto, osea que luego de 
//"p edge n m" o "e v w", haya espacios en blanco o un salto de linea.
//Devuelve true si el formato esta bien y false si no.
bool format_correcto() {
  bool corr = false;
  int s_tmp13;
  s_tmp13 = getchar();
  //si hay \n o \r
  if(s_tmp13 == 10 || s_tmp13 == 13) {
    corr = true; //esta correcto
  } else if(s_tmp13 == 32) { //si hay espacios en blanco
    while (s_tmp13 == 32) { //mientras aya espacios en blanco, veo el siguiente
      s_tmp13 = getchar();
    } if (s_tmp13 != 10 && s_tmp13 != 13) { //si el siguiente no es \r o \n
      corr = false; //el formato es incorrecto
    } else {
      corr = true; //si lo es el formato es correcto
    }
  } else if (s_tmp13 == EOF) { //si ni \r \n o espacio en blanco
    corr = true; //el formato esncorrecto
  } else { //si ni \r \n o espacio en blanco
    corr = false; //el formato es incorrecto
  }
  return corr;
}

//Libera memoria y enviar mensaje si hay error en la lectura del algun lado del grafo
void error_lect_lado(u32 * a_v1, u32 *a_v2, u32 i, u32 r) {
  free(a_v1);
  a_v1 = NULL;
  free(a_v2);
  a_v2 = NULL;
  if(r == 0 ) {
    printf("error de lectura en lado L: %u\n", i);
   } else {
    printf("error de lectura en lado l+1: %u\n", i);
  }
}
//Busca el indice i en el arreglo arr de tamaño num_ar
u32 buscar_ind (u32 i, u32 *arr, u32 num_ar) {
  bool t = false;
  u32 indice = num_ar +1 ;
  int centro=0,inf=0,sup=num_ar-1;
  while(inf<=sup && t == false){
    centro=((sup-inf)/2)+inf;
    if(arr[centro]==i) {
      indice = centro;
      t = true;
    } else if(i < arr[centro]) {
      sup=centro-1;
    } else {
      inf=centro+1;
    }
  }
  return indice;
}


//Obtiene los nombres reales de los vertices, puestos en un arreglo aux, de tamaño num_aristas, y los guarda en un arreglo vert
u32 * nombre_verts(u32 *aux, u32 num_aristas, u32 *vert) {
  u32 i = 0;
  u32 max;
  max = num_aristas*2;
  for (u32 j=0; j < max; j++) {
    vert[i] = aux[j];
    if (j < max - 1 && aux[j] != aux[j+1]) {
      vert[i+1] = aux[j];
      i++;
    }
  }
  return vert;
}

//Cuenta los vertices del arreglo.
u32 largo_vert(u32 *aux, u32 num_aristas) {
  u32 i = 0;
  for (u32 j=0; j < num_aristas; j++) {
    if (j < num_aristas - 1 && aux[j] != aux[j+1]) {
      i++;
    }
  }
  i = i+1;
  return i;
}

//Calcula los grados de los vertices, a partir de un arreglo vert de tamaño tam y los guarda en un arreglo c_vec
u32 * grado_vert(u32 *vert, u32 tam, u32 *c_vec) {
    u32 j = 0, r = 1;
    for (u32 i=0; i<tam; i++) {
      if (vert[i] == vert[i+1]) {
        c_vec[j] = r + 1;
        r++;
      } else if (i > 0 && vert[i] != vert[i-1]) {
        c_vec[j] = 1;
        j++;
      } else if (i == 0 && vert[i] != vert[i+1]){
        c_vec[j] = 1;
        j++;
      } else if (vert[i] != vert[i+1]){
        r = 1;
        j++;
      }
    }
  return c_vec;
}

// Devuelve el indice de un vertice
u32 *ind(u32 *pv, u32 tam, u32 *a_v, u32 *ver, u32 tamver) {
  int j = 0;
  for(u32 i = 0; i < tam; i++) {
    pv[j] = buscar_ind(a_v[i], ver, tamver);
    j++;
  }
  return pv;
}

// Alloca memoria para los vecinos de los vertices
u32 **array_vec(u32 **gra, u32 *vec, u32 num_vertices){
  for(u32 i = 0; i < num_vertices; i++) {
    gra[i] = calloc(vec[i], sizeof(int));
    if (gra[i] == NULL) {
      printf("error al alocar memoria\n");
      return NULL;
    }
  }
  return gra;
}

// Devuelve una matriz con los vertices y sus vecinos
u32 **llen_vec(u32 **gra, u32 *pv1, u32 *pv2, u32 num_aristas, u32 num_vertices, u32 *vec){
  u32 i, j, k;
  i = 0;
  j = 0;
  k = 0;
  bool flag = true;
  // Arreglo auxiliar para los vecinos, se inicializa en 0
  u32 *v_ll = calloc(num_vertices, sizeof(int));
  if (v_ll == NULL) {
    printf("error al alocar memoria\n");
    return NULL;
  }
  for(k=0; k<num_vertices; k++){
    v_ll[k] = 0;
  }
  // Si faltan aristas por revisar se agrega en los vertices sus vecinos
  // correspondientes, 2 a la vez, por ejemplo si la arista es e 1 2,
  // al vertice 1 se le agrega el vecino 2, y viceversa
  while(flag){
    j = v_ll[pv1[i]];
    gra[pv1[i]][j] = pv2[i];
    if (j <= vec[pv1[i]]) {
      v_ll[pv1[i]] = j+1;
    }
    j = v_ll[pv2[i]];
    gra[pv2[i]][j] = pv1[i];
    if (j <= vec[pv2[i]]){
      v_ll[pv2[i]] = j+1;
    }
    if (i == num_aristas-1){
      flag = false;
    }
    i++;
  }
  free(v_ll);
  return gra;
}

//para liberar memoria y dar mensajes de error
u32 *error_1l_sin_comentario(char* str2, u32 *lectura) {
  free(str2);
  str2=NULL;
  lectura[0] = 0;
  printf("error en primera linea sin comentario\n");
  return lectura;
}

// Retorna el primer p edge bien escrito, que no se encuentre en un comentario
u32 * leer_p_edge(u32 * lectura) {
  char *str2, *edge;
  edge = "edge";
  bool oks, comp;
  oks = false;
  comp = false;
  u32 num_vertices, num_aristas;
  str2 = NULL;
  str2 = (char*) malloc(sizeof(char)+4);
  // Si se pudo allocar memoria, se prosigue
  if (str2 != NULL) {
    while(!oks) {
      // Si lo que que sigue al p NO es un edge
      if(scanf("%4s", str2) && (strcmp(str2,edge)!=0)) {
        return (error_1l_sin_comentario(str2, lectura));
      }
      char esp;
      esp = getchar();
      //si lo que sigue es edge y luego hay un espacio en blanco
      if(esp == 32) {
       // Si la cantidad de vertices o lados es 0
        if(scanf("%u %u", &num_vertices, &num_aristas) || ((num_vertices==0) || (num_aristas == 0))) {
          if((num_vertices==0) || (num_aristas == 0)) {
            return (error_1l_sin_comentario( str2, lectura));
          }
        }
      } else { //si lo que sigue es edge pèro luego no hay un espacio en blanco
        return (error_1l_sin_comentario(str2, lectura));
      }
      // Si se encuentra en un formato correcto
      comp = format_correcto();
      if(comp) {
        oks = true;
      } else {
      // Si ocurre un error en la primera linea sin comentario
        return (error_1l_sin_comentario(str2, lectura));
      }
      free(str2);
      str2=NULL;
    }
  } else { printf("error al alocar memoria\n"); return NULL;}
  // Si todo fue bien, se guarda en el arreglo lectura en los indices 1 y 2 
  // la cantidad de vertices y lados dados por el grafo
  lectura[0]=1;
  lectura[1]=num_vertices;
  lectura[2]=num_aristas;
  return lectura;
}

// Salta todas las lineas con comentarios
void lectura_gra_p (u32 *lectura) {
  char salto;
  bool flag = false;
  char esp;
  while(!flag) {
    //Si el primer caracter es igual a c
    esp = getchar();
    if(esp == 99) {
      esp = getchar();
      //si le sigue un espacio en blanco
      if (esp == 32) {
        while((salto = getchar()) != '\n'){} //Busco un salto de linea
       } else if (esp == 10) {//si le sigue un salto de linea
       } else { //si no le sigue ni un espacio en blanco ni salto de linea, es que no es un comentario
       printf("error en primera linea sin comentario\n");
       lectura[0] = 0;
       flag = true;
      }
     } else if(esp == 112) { //Si el primer caracter es igual a p
       esp = getchar();
       if (esp == 32) { //si le sigue un espacio en blanco
         flag = true; //Vamos a dejar de leer comentarios
         lectura = leer_p_edge(lectura);
         if(lectura[0]!=0) { //está mal escrita la linea de p edge
           flag = true;
        }
       } else { //si no le sigue un espacio en blanco, entonces no sera ni comentario ni p edge
         printf("error en primera linea sin comentario\n");
         lectura[0] = 0;
         flag = true;
       }
     } else { //empieza con cualquier caracter != de p o c
       printf("error en primera linea sin comentario\n");
       lectura[0] = 0;
       flag = true;
     }
  }
}

// Carga los vertices de las aristas
u32 * lectura_gra_e(u32 *auxb, u32 *a_v1, u32 *a_v2, u32 num_aristas, u32 num_vertices) {
  u32 i = 0;
  u32 v1, v2;
  bool cto = false;
  u32 r = 0;
  // Si no he llegado al numero de aristas
  while (i < num_aristas) {
    char esp;
    esp = getchar();
    if(esp != 101) {// Si la linea no comienza con e
      if(esp == EOF || esp == 10 || esp == 13) { //si la linea era \n \r o no habia nada
        r = 1;
      }
      error_lect_lado(a_v1, a_v2, i + 1, r); //imprimo error
      return NULL; //retorno null
    }
    //si la linea empieza con e y le sigue un espacio en blanco
    if((esp = getchar() == 32) && scanf("%u %u", &v1, &v2)) {
      //Si la linea esta en el formato correcto
      cto = format_correcto();
      if(cto) {
        a_v1[i] = v1;
        a_v2[i] = v2;
        i++;
      } else {
        // Si no, da error de lectura en el lado correspondiente
        error_lect_lado(a_v1, a_v2, i + 1, r);
        return NULL;
      }
    } else {
      //si la linea empieza con e y le sigue no es un espacio en blanco
      error_lect_lado( a_v1, a_v2, i + 1, r); //da error de lectura en el lado correspondiente
      return NULL;
    }
  }
  
  auxb = calloc((num_aristas*2)+1, sizeof(int));
  // Si se pudo allocar memoria
  if(auxb != NULL) {
  // Se copia en auxb los vertices en a_v1 y a_v2 y se ordena
    memcpy(auxb, a_v1, num_aristas * sizeof(int));
    memcpy(auxb + num_aristas , a_v2, num_aristas * sizeof(int));
    qsort(auxb, num_aristas*2, sizeof(int), cmpfunc);
    // Si ocurre un problema con la cantidad de vertices
    u32 largo = largo_vert(auxb, num_aristas*2);
    if (largo != num_vertices) {
      printf("cantidad de vertices leidos no es la declarada\n");
      free(a_v1);
      a_v1=NULL;
      free(a_v2);
      a_v2=NULL;
      free(auxb);
      auxb = NULL;
      return NULL;
    }
  } else { printf("error al alocar memoria\n"); return NULL;}
  return auxb;
}

// Construccion del Grafo
Grafo ConstruccionDelGrafo() {
  // Declaramos los strings que deben leerse obligatoriamente en el archivo
  u32 num_vertices, num_aristas, lectura[3];
  lectura_gra_p(lectura);
  // Si hay datos en lectura se agrega en num_aristas y num_vertices lo 
  //correspondiente
  if(lectura[0] != 0) {
    num_vertices = lectura[1];
    num_aristas = lectura[2];
    u32 *a_v1, *a_v2;
    a_v1 = calloc(num_aristas, sizeof(int));
    a_v2 = calloc(num_aristas, sizeof(int));
    if(a_v1 != NULL && a_v2 != NULL) {
      u32  *auxb;
      auxb = NULL;
      // Se carga las aristas
      auxb = lectura_gra_e (auxb, a_v1, a_v2, num_aristas, num_vertices);
      if(auxb != NULL) {
        u32 *vert, *ord, *col, **gra, *vec;
        Grafo g;
        g = NULL;
        g = calloc(1,sizeof(GrafoSt));//añadimos memoria al grafo
        if( g != NULL) {
          // Se alloca a los miembros del grafo
          g->nombre_vertices = calloc(num_vertices, sizeof(int));
          g->grado_vertices = calloc(num_vertices, sizeof(int));
          g->colores = calloc(num_vertices, sizeof(int));
          g->orden = calloc(num_vertices+1, sizeof(int));
          g->gra = calloc(num_vertices, sizeof(int*));
          if(g->nombre_vertices != NULL && g->grado_vertices != NULL && g->colores != NULL && g->orden != NULL && g->gra != NULL) {
          // Se llena el arreglo vert con los nombres de los distintos vertices
            vert = nombre_verts(auxb, num_aristas, g->nombre_vertices);
            vec = g->grado_vertices, col = g->colores, ord = g->orden, gra = g->gra;
            // Se inicializa ord con i y gra[i] con NULL
            for (u32 i = 0; i <num_vertices; i++) {
              ord[i] = i;
            }
            for (u32 i = 0; i < num_vertices; i++) {
              gra[i] = NULL;
            }
            // Se llena vec con los vecinos de cada vertice
            vec = grado_vert(auxb, num_aristas*2, vec);
            u32 *pv1, *pv2;
            // Se alloca memoria para los auxiliares pv1 y pv2
            pv1 = calloc(num_aristas, sizeof(int));
            pv2 = calloc(num_aristas, sizeof(int));
            if(pv1 != NULL && pv2 != NULL) {
            // Se llena pv1 y pv2 con los vertices de a_v1 y a_v2
              pv1 = ind(pv1, num_aristas, a_v1, vert, num_vertices);
              pv2 = ind(pv2, num_aristas, a_v2, vert, num_vertices);
              gra = array_vec(gra, vec, num_vertices);
              if (gra != NULL) {
                // Se llena gra con los vecinos
                gra = llen_vec(gra, pv1, pv2, num_aristas, num_vertices, vec);
                // Se agrega a cada miembro del grafo con su correspondiente valor
                g->nombre_vertices = vert;
                g->grado_vertices = vec;
                g->colores = col;
                g->gra = gra;
                g->num_vertices = num_vertices;
                g->num_aristas = num_aristas;
                g->orden = ord;
                u32 gr = Greedy(g);
                g->num_colores = gr;
                free(a_v1);
                a_v1=NULL;
                free(a_v2);
                a_v2=NULL;
                free(pv1);
                pv1=NULL;
                free(pv2);
                pv2=NULL;
                free(auxb);
                auxb = NULL;
                return g;
               //si se da algun problema de alocar memoria
              } else { return NULL;}
            } else { printf("error al alocar memoria\n"); return NULL;}
          } else { printf("error al alocar memoria\n"); return NULL;}
        } else { printf("error al alocar memoria\n"); return NULL;}
      } else { return NULL;}
    } else { printf("error al alocar memoria\n"); return NULL;}
  } else { return NULL;}
}

// Copia el grafo procesado
Grafo CopiarGrafo(Grafo G) {
  u32 num_vertices = G->num_vertices;
  Grafo Gcpy;
  Gcpy = calloc(1,sizeof(GrafoSt));
  if (Gcpy == NULL) {
    return NULL;
  }
  //Pido memoria para cada arreglo de mi estructura
  Gcpy->nombre_vertices = calloc(num_vertices, sizeof(int));
  if (Gcpy->nombre_vertices == NULL) {
    free(Gcpy);
    Gcpy = NULL;
    return NULL;
  }
  Gcpy->grado_vertices = calloc(num_vertices, sizeof(int));
  if (Gcpy->grado_vertices == NULL) {
    free(Gcpy->nombre_vertices);
    Gcpy->nombre_vertices = NULL;
    free(Gcpy);
    Gcpy = NULL;
    return NULL;
  }
  Gcpy->colores = calloc(num_vertices, sizeof(int));
  if (Gcpy->colores == NULL) {
    free(Gcpy->nombre_vertices);
    Gcpy->nombre_vertices = NULL;
    free(Gcpy->grado_vertices);
    Gcpy->grado_vertices = NULL;
    free(Gcpy);
    Gcpy = NULL;
    return NULL;
  }
  Gcpy->orden = calloc(num_vertices, sizeof(int));
  if (Gcpy->orden == NULL) {
    free(Gcpy->nombre_vertices);
    Gcpy->nombre_vertices = NULL;
    free(Gcpy->grado_vertices);
    Gcpy->grado_vertices = NULL;
    free(Gcpy->colores);
    Gcpy->colores = NULL;
    free(Gcpy);
    Gcpy = NULL;
    return NULL;
  }
  for(u32 i = 0; i < num_vertices; i++){
    Gcpy->orden[i] = G->orden[i];
    Gcpy->nombre_vertices[i] = G->nombre_vertices[i];
    Gcpy->grado_vertices[i] = G->grado_vertices[i];
    Gcpy->colores[i] = G->colores[i];
  }
  // Copio las cantidades de mi estructura
  Gcpy->num_vertices = G->num_vertices;
  Gcpy->num_aristas = G->num_aristas;
  Gcpy->num_colores = G->num_colores;
  // Alloca memoria para los vecinos
  Gcpy->gra=calloc(num_vertices, sizeof(int*));
  if (Gcpy->gra == NULL) {
    free(Gcpy->nombre_vertices);
    Gcpy->nombre_vertices = NULL;
    free(Gcpy->grado_vertices);
    Gcpy->grado_vertices = NULL;
    free(Gcpy->colores);
    Gcpy->colores = NULL;
    free(Gcpy->orden);
    Gcpy->orden = NULL;
    free(Gcpy);
    Gcpy = NULL;
    return NULL;
  }
  for (u32 i = 0; i < num_vertices; i++) {
    Gcpy->gra[i] = NULL;
  }
  for(u32 i = 0; i < num_vertices; i++){
    u32 tam = G->grado_vertices[i];
    Gcpy->gra[i] = calloc(tam, sizeof(u32));
    if (Gcpy->gra[i] == NULL) {
      free(Gcpy->nombre_vertices);
      Gcpy->nombre_vertices = NULL;
      free(Gcpy->grado_vertices);
      Gcpy->grado_vertices = NULL;
      free(Gcpy->colores);
      Gcpy->colores = NULL;
      free(Gcpy->orden);
      Gcpy->orden = NULL;
      free(Gcpy);
      Gcpy = NULL;
      return NULL;
    }
  //Copio la matriz de vecinos de cada vértice
    for(u32 j = 0; j < tam; j++){
      Gcpy->gra[i][j] = G->gra[i][j];
    }
  }
  return Gcpy;
}

u32 colorear(Grafo G) {
  u32 *coloreado = G->colores;
  u32 *vec = G->grado_vertices;
  u32 **gra = G->gra;
  u32 *ord = G->orden;
  u32 num_vert = G->num_vertices;
  //Se crea un arreglo auxiliar para decir que colores aun no estan usados, los indices seran los colores.
  bool *coloreado_aux;
  u32 ind_vec, max_vec, max_aux, min_aux, c, ci, init;
  init = num_vert + 1;
  c = 0;
  ci = 0;
  max_vec = 0; //cota minima para el arreglo aux (minimo color del vecino)
  max_aux = 0; //cota maxima para el arreglo aux (maxima color del vecino)
  min_aux = init;
  coloreado_aux = calloc(num_vert, sizeof(u32));
  if (coloreado_aux == NULL) {
    return -1;
  }
  //Se pinta con color init que es un color q nunca exisitira, y el arreglo auxiliar en falso.
  for(u32 i = 0; i < num_vert; i++) {
    coloreado[ord[i]] = init;
    coloreado_aux[i] = false;
  }
  //se recorre el arreglo de orden
  for(u32 i = 0; i < num_vert; i++) {
    if (i == 0) {
      coloreado[ord[i]] = 0; //como recien empiezo, pinto al 1mer elemento en 0.
      c++;
    } else {
      if(min_aux < max_aux) { //si mi cota minima es menor q mi cota maxima
        for(u32 j = min_aux; j < max_aux +1; j++) { //recorro esa parte del arreglo
         coloreado_aux[j] = false; //seteando en falso.
        }
      }
      u32 count = 0;//inicializo un contador para ver cuantos vecinos tengo coloreados.
      for(u32 j = 0; j < vec[ord[i]]; j++) { //recorro el arreglo de mis vecinos
        ind_vec = gra[ord[i]][j]; //obtengo el indice de mi vecino
        if (init != coloreado[ind_vec]) { //si mi vecino esta coloreado
          coloreado_aux[coloreado[ind_vec]] = true; //pongo que ese color esta siendo usado
          max_aux = maximo(coloreado[ind_vec], max_aux);//calculo mi cota maxima
          min_aux = minimo(coloreado[ind_vec], min_aux);//calculo mi cota minima
          count++; //sumo vecino coloreado
        }
      }
      if (count == 0) { //si no tengo ningun vecino coloreado
        coloreado[ord[i]] = 0; //me pinto en 0
      } else if (min_aux != 0) { //si el minimo color de mis vecinos no es 0
        coloreado[ord[i]] = 0; // me pinto en 0
        coloreado_aux[min_aux] = false; //seteo ese color a falso
      } else if (min_aux == max_aux) { //si mis vecinos tienen color 0
        coloreado[ord[i]] = 1; //me pinto en 1.
        c = maximo(coloreado[ord[i]], max_vec); //calculo el maximo color final
        coloreado_aux[min_aux] = false; // seteo ese color a falso
      } else { //si tengo vecinos pintados, el minimo no es 0 y el maximo tampoco
        for(u32 k = min_aux; k < max_aux+2; k ++) { //recoro mi arreglo auxiliar, desde mi cota minima, hasta mi cota maxima +2 
                                                    //x si todos mis vecinos estan pintados
          if (coloreado_aux[k] == false) { //recorro hasta encontrar el minimo que no esta pintado
            coloreado[ord[i]] = k; //me pinto de ese color
            c = maximo(coloreado[ord[i]], max_vec); //calculo el maximo color final
            break; //corto el coloreado
          } else {
            coloreado_aux[k] = false; //mientras recorro seteo mi arreglo auxiliar con los colores ya visto en falso
            min_aux++; //aumento mi cota minima
          }
        }
      }
    }
    ci = maximo(c, ci); //calculo el color final
  }
  ci++; //aumento mi color final en 1 (ya que empiezan en 0)
  G->colores = coloreado; //guardo mi coloreado en el grafo
  G->num_colores = ci; //guardo la cantidad de colores en el grafo
  free(coloreado_aux); //libero memoria
  return ci; //devuelvo la cantidad de colores obtenida
}

// Retorna Cantidad de Vertices guardado en la estructura
u32 NumeroDeVertices(Grafo G) {
  return G->num_vertices;
}

// Retorna Cantidad de Lados guardado en la estructura
u32 NumeroDeLados(Grafo G) {
  return G->num_aristas;
}

// Retorna Cantidad de Colores guardado en la estructura
u32 NumeroDeColores(Grafo G) {
  return G->num_colores;
}

// Retorna Nombre del Vertice guardado en la estructura
u32 NombreDelVertice(Grafo G, u32 i) {
  u32 *ord = G->orden;
  u32 vert = G->nombre_vertices[ord[i]];
  return vert;
}

// Retorna Color del Vertice
u32 ColorDelVertice(Grafo G, u32 i) {
  u32 *ord = G->orden;
  // Si el indice se encuentra en el rango adecuado
  if(i < G->num_vertices) {
    // Se obtiene el color del vertice en el arreglo orden
    u32 col = G->colores[ord[i]];
    return col;
  } else {
    // Si no se encuentra en el rango adecuado
    u32 col = pow(2, 32) - 1;
    return col;
  }
}

// Retorna Grado del Vertice
u32 GradoDelVertice(Grafo G, u32 i) {
  u32 *ord = G->orden;
  // Si el indice se encuentra en el rango adecuado
  if(i < G->num_vertices) {
    // Se obtiene el grado del vertice en el arreglo orden
    u32 gra = G->grado_vertices[ord[i]];
    return gra;
  } else {
    // Si no se encuentra en el rango adecuado
    u32 gra = pow(2, 32) - 1;
    return gra;
  }
}

// Retorna Color del J-esimo Vecino
u32 ColorJotaesimoVecino(Grafo G, u32 i, u32 j) {
  u32 *ord = G->orden;
  u32 color_j = 0;
  u32 **gra = G->gra;
  u32 ind_vec = 0;
  // Si el color j-esimo se encuentra en el rango adecuado
  if( j < G->grado_vertices[ord[i]] && i < G->num_vertices) {
    // Indice del j-esimo vecino en el arreglo orden
    ind_vec = gra[ord[i]][j];
    // Color del j-esimo vecino
    color_j = G->colores[ind_vec];
    return color_j;
  } else {
    // Si no se encuentra en el rango adecuado
    u32 ret = pow(2, 32) - 1;
    return ret;
  }
}

// Retorna el nombre del j-esimo vecino
u32 NombreJotaesimoVecino(Grafo G, u32 i,u32 j) {
  u32 *ord = G->orden;
  u32 nombre_j_vec = 0;
  u32 **gra = G->gra;
  u32 ind_vec = 0;
  // Indice del j-esimo vecino en el arreglo orden
  ind_vec = gra[ord[i]][j];
  // Nombre del j-esimo vecino
  nombre_j_vec = G->nombre_vertices[ind_vec];
  return nombre_j_vec;
}

// Intercambia dos vertices
char SwitchVertices(Grafo G, u32 i, u32 j) {
  char ord_switch;
  u32 num_vert = G->num_vertices;
  u32 temp;
  u32 *ord = G->orden;
  //se intercambian los vertices i y j
  if(i < num_vert && j < num_vert){
    temp = ord[i];
    ord[i] = ord[j];
    ord[j] = temp;
    ord_switch = 0;
  } else {
    ord_switch = 1;
  }
  //se reemplaza el orden por el generado
  G->orden = ord;
  return ord_switch;
}

// Intercambia dos colores
char SwitchColores(Grafo G,u32 i,u32 j) {
  u32 num_vert = G->num_vertices;
  u32 *col = G->colores;
  u32 num_col = G->num_colores;
  //se verifica que i,j sean menores al numero de colores de G en este momento
  if(i < num_col && j < num_col) {
    for(u32 k = 0; k < num_vert; k++) {
      if(col[k] == j) { //cuando el color es igual a j
        col[k] = i; //se cambia por i
      } else if (col[k] == i) { //cuando el color es igual a i
        col[k] = j; //se cambia por j
      }
    }
  } else {
    return 1;
  }
  //se reemplaza el coloreo por el generado
  G->colores = col;
  return 0;
}

//Ordena por grado de vertices de mayor a menor del arreglo orden
char OrdenWelshPowell(Grafo G) {
  u32 *grado = G->grado_vertices;
  u32 num_vert = G->num_vertices;
  u32 *ord = G->orden;
  char ord_w_p;
  bool band;
  band = false;
  struct aux * gr_ind;
  //Estructura auxiliar para guardar grado e indices de vertices
  gr_ind = calloc(num_vert, sizeof(struct aux));
  if (gr_ind == NULL) {
    return 1;
  }
  for(u32 j = 0; j < num_vert; j++) {
    gr_ind[j].comp = grado[j];
    gr_ind[j].ind = j;
  }
  //Se ordena el arreglo auxiliar de mayor a menor por grado del vertice
  qsort(gr_ind, num_vert, sizeof(struct aux), comp_dec_aux);
  //Se asigna el resultado de la ordenacion al arreglo orden del grafo
  for(u32 j = 0; j < num_vert; j++) {
    ord[j] = gr_ind[j].ind;
  }
  G->orden = ord;
  //Se verifica que el resultado sea correcto
  for(u32 i= 0; i < num_vert; i++) {
    if(i+1 != num_vert && grado[ord[i]] >= grado[ord[i+1]]) {
      band = true;
    } else if ( i+1 == num_vert) {
      ord_w_p = 0;
      free(gr_ind);
      return ord_w_p;
    } else {//Si no es correcto devuelve 1
      free(gr_ind);
      ord_w_p = 1;
      return ord_w_p;
    }
  }
  if(band) { //Si esta ordenado correctamente, se retorna 0
    ord_w_p = 0;
  }
  free(gr_ind);
  return ord_w_p;
}

// Ordena por nombre de vertices de menor a mayor del arreglo orden
char OrdenNatural(Grafo G) {
  u32 *ord = G->orden;
  bool band;
  band = false;
  char ord_nat;
  u32 num_vert = G->num_vertices;
  // Se ordena los vertices de menor a mayor
  qsort(ord, num_vert, sizeof(u32), cmpfunc);
  // Se asigna el resultado de la ordenacion al arreglo orden del grafo
  G->orden = ord;
  // Se verifica que el resultado sea correcto
  for(u32 i= 0; i < num_vert; i++) {
    if(ord[i] <= ord[i+1]){
      band = true;
    } else {
      // Si no esta bien ordenado, se retorna 1
      ord_nat = 1;
    }
  }
  // Si esta ordenado correctamente, se retorna 0
  if(band) {
    ord_nat = 0;
  }
  return ord_nat;
}

// Ordena por color de menor a mayor del arreglo orden
char RMBCnormal(Grafo G) {
    u32 *colores = G->colores;
    u32 num_vert = G->num_vertices;
    u32 *ord = G->orden;
    char ord_r_n;
    bool band = false;
    // Estructura auxiliar para guardar colores e indices de vertices
    struct aux * col_ind;
    col_ind = calloc(num_vert, sizeof(struct aux));
    if (col_ind == NULL) {
      return 1;
    }
    for(u32 j = 0; j < num_vert; j++) {
        col_ind[j].comp = colores[j];
        col_ind[j].ind = j;
    }
    // Se ordena los colores de menor a mayor
    qsort(col_ind, num_vert, sizeof(struct aux), comp_cre_aux);
    // Se asigna el resultado de la ordenacion al arreglo orden del grafo
    for(u32 i = 0; i < num_vert; i++) {
        ord[i] = col_ind[i].ind;
    }
    G->orden = ord;
    // Se verifica que el resultado sea correcto
    for(u32 i= 0; i < num_vert; i++) {
        if(i+1 != num_vert && colores[ord[i]] <= colores[ord[i+1]]) {
            band = true;
        } else if (i+1 == num_vert) {
            ord_r_n = 0;
            free(col_ind);
            return ord_r_n;
        } else {
            // Si no esta bien ordenado, se retorna 1
            ord_r_n = 1;
            free(col_ind);
            return ord_r_n;
        }
    }
    // Si esta ordenado correctamente, se retorna 0
    if(band) {
        ord_r_n = 0;
    }
    free(col_ind);
    return ord_r_n;
}

//Ordena por color de mayor a menor del arreglo orden
char RMBCrevierte(Grafo G) {
  u32 *colores = G->colores;
  u32 num_vert = G->num_vertices;
  u32 *ord = G->orden;
  char ord_r_r;
  bool band = false;
  // Estructura auxiliar para guardar colores e indices de vertices
  struct aux * col_ind;
  col_ind = calloc(num_vert, sizeof(struct aux));
  if (col_ind == NULL) {
    return 1;
  }
  for(u32 j = 0; j < num_vert; j++) {
    col_ind[j].comp = colores[j];
    col_ind[j].ind = j;
  }
  qsort(col_ind, num_vert, sizeof(struct aux), comp_dec_aux);
  // Se asigna el resultado de la ordenacion al arreglo orden del grafo
  for(u32 i = 0; i < num_vert; i++) {
    ord[i] = col_ind[i].ind;
  }
  G->orden = ord;
  // Se verifica que el resultado sea correcto
  for(u32 i= 0; i < num_vert; i++) {
    if(i+1 != num_vert && colores[ord[i]] >= colores[ord[i+1]]) {
      band = true;
    } else if (i+1 == num_vert) {
      ord_r_r = 0;
      free(col_ind);
      return ord_r_r;
    } else {
      // Si no esta bien ordenado, se retorna 1
      ord_r_r = 1;
      free(col_ind);
      return ord_r_r;
    }
  }
  // Si esta ordenado correctamente, se retorna 0
  if(band) {
    ord_r_r = 0;
  }
  free(col_ind);
  return ord_r_r;
}

//Ordena por cantidad de colores de menor a mayor del arreglo orden
char RMBCchicogrande(Grafo G) {
  u32 *colores = G->colores;
  u32 num_vert = G->num_vertices;
  u32 *ord = G->orden;
  char ord_r_cg;
  bool band = true;
  // Estructura auxiliar para guardar colores, indices de vertices
  struct aux2 * col_ind;
  u32 *ccol = calloc(G->num_colores, sizeof(u32));
  if (ccol == NULL) {
    return 1;
  }
  col_ind = calloc(num_vert, sizeof(struct aux2));
  if (col_ind == NULL) {
    free(ccol);
    return 1;
  }
  for(u32 j = 0; j < num_vert; j++) {
    col_ind[j].comp = colores[j];
    col_ind[j].ind = j;
    ccol[colores[j]]++;
    col_ind[j].cant_col = 0;
  }
  // Se asigna a la estructura auxiliar la cantidad de vertices con cada color
  for(u32 j = 0; j < num_vert; j++) {
    col_ind[j].cant_col = ccol[col_ind[j].comp];
  }
  // Se ordena la cantidad de colores de menor a mayor
  qsort(col_ind, num_vert, sizeof(struct aux2), col_cre_aux);
  // Se verifica que el resultado sea correcto
  for(u32 i = 0; i < num_vert; i++) {
    ord[i] = col_ind[i].ind;
    if(band == true && i+1 != num_vert && col_ind[i].cant_col <= col_ind[i+1].cant_col) {
      band = true;
    } else if (i+1 == num_vert) {
      band = true;
    } else {
      // Si no esta bien ordenado, se retorna 1
      ord_r_cg = 1;
      band = false;
    }
  }
  if(band) {
    ord_r_cg = 0;
  }
  // Se asigna el resultado de la ordenacion al arreglo orden del grafo
  G->orden = ord;
  free(col_ind);
  free(ccol);
  return ord_r_cg;
}

u32 Greedy(Grafo G) {
  u32 greedy = colorear(G);
  return greedy;
}

//Para ver si el grafo es bipartito
int Bipartito (Grafo G) {
  u32 num_vert = G->num_vertices;
  u32 **gra = G->gra;
  u32 *vec = G->grado_vertices;
  u32 tam_niv_act = 0, cvert_n = 1, ind_vec = 0, cvert_us = 1;
  u32 init = num_vert + 1;
  u32 *niv_act, *niv_nue;
  niv_act = NULL, niv_nue = NULL;
  niv_act = calloc(vec[0], sizeof(u32));
  if (niv_act == NULL) {
    return -1;
  }
  u32 *color = G->colores;
  //Se pinta con color init que es un color q nunca exisitira.
  for(u32 j = 0; j < num_vert; j++) {
    color[j] = init;
  }
  color[0] = 0;
  tam_niv_act++;
  //mientras no haya visto todos mis vertices
  while(cvert_us != num_vert+1) {
    //pido memoria para armar un nuevo nivel
    niv_nue = calloc(1, sizeof(u32));
    if (niv_nue == NULL) {
      free(niv_act);
      return -1;
    }
    //recorro mi actual nivel
    for(u32 i = 0; i < tam_niv_act; i++) {
      //recorro los vecinos de los vertices de mi actual nivel
      for(u32 j = 0; j < vec[niv_act[i]]; j++) {
        ind_vec = gra[niv_act[i]][j];
        //si mi vecino tiene mi mismo color, no sera bipartito
        if (color[ind_vec] == color[niv_act[i]]) {
          free(niv_nue);
          free(niv_act);
          Greedy(G);
          return 0;
        //si no y mi vecino aun no esta pintado
        } else if(color[ind_vec] == init) {
          //si mi color es 0
          if (color[niv_act[i]] == 0) {
            niv_nue = (u32 *)realloc(niv_nue, cvert_n*sizeof(u32)); //armo el nuevo nivel
            niv_nue[cvert_n-1] = ind_vec; //digo que este nivel tendra a este vecino
            color[ind_vec] = 1; //pinto a mi vecino en 1
            cvert_n++;
          //si mi color es 1
          } else if (color[niv_act[i]] == 1) {
            niv_nue = (u32 *)realloc(niv_nue, cvert_n*sizeof(u32));
            niv_nue[cvert_n-1] = ind_vec;
            color[ind_vec] = 0; //pinto a mi vecino en 0
            cvert_n++;
          }
        }
      }
      /*si termine de recorrer mi nivel actual pero no cree un nuevo nive
       y aun tengo vertices que recorrer (para grafos no conexos) */
      if (i+1 == tam_niv_act && niv_nue[0] == 0 ) {
        for(u32 k = 0; k < num_vert; k++) { // busco algun vertice que aun no
          if(color[k] == init) {            // haya sido pintado y armo ni
            niv_nue[0] = k;                 // nuevo nivel con el.
            color[k] = 0;
            cvert_n++;
            break;
          }
        }
      }
    }
    free(niv_act); //libero memoria de mi viejo nivel
    niv_act = calloc(cvert_n, sizeof(u32)); //pido memoria para mi nuevo nivel
    if (niv_act == NULL) {
      free(niv_nue);
      return -1;
    }
    memcpy(niv_act, niv_nue, (cvert_n -1)* sizeof(u32)); //mi actual nivel sera el ultimo que arme
    free(niv_nue); //libero memoria para poeder rearmarlo
    tam_niv_act = cvert_n -1; //doy el tamaño de mi nuevo nivel
    cvert_us = cvert_us + tam_niv_act; //calculo cuantos vertices recorri
    cvert_n = 1; //reinicio el conteo para mi nuevo nivel
    if (tam_niv_act  == 0 && cvert_us == num_vert) { //si ya recorri todos mis vertices
      cvert_us++;  //aumento mi cantidad para salir del while.
    }
  }
  G->colores = color;
  G->num_colores = 2;
  free(niv_act);
  return 1;
}

// Destruye el grafo dado, liberando la memoria asignada
void DestruccionDelGrafo(Grafo G) {
  u32 num_vert = G->num_vertices;
  for (u32 i = 0; i < num_vert; i++) {
    free(G->gra[i]);
  }
  free(G->nombre_vertices);
  free(G->grado_vertices);
  free(G->colores);
  free(G->orden);
  free(G->gra);
  free(G);
  G = NULL;
}

