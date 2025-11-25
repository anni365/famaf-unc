#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

typedef unsigned int u32;

typedef struct {
    u32 *nombre_vertices;
    u32 *grado_vertices;
    u32 *colores;
    u32 ** gra;
    u32 num_vertices;
    u32 num_aristas;
    u32 num_colores;
    u32 * orden;
} GrafoSt, *Grafo;

//Funciones pedidas en el Proyecto

Grafo ConstruccionDelGrafo();

void DestruccionDelGrafo(Grafo G);

Grafo CopiarGrafo(Grafo G);

u32 Greedy(Grafo G);

int Bipartito(Grafo G);

u32 NumeroDeVertices(Grafo G);

u32 NumeroDeLados(Grafo G);

u32 NumeroDeColores(Grafo G);

u32 NombreDelVertice(Grafo G, u32 i);

u32 ColorDelVertice(Grafo G, u32 i);

u32 GradoDelVertice(Grafo G, u32 i);

u32 ColorJotaesimoVecino(Grafo G, u32 i,u32 j);

u32 NombreJotaesimoVecino(Grafo G, u32 i,u32 j);

char OrdenNatural(Grafo G);

char OrdenWelshPowell(Grafo G);

char SwitchVertices(Grafo G,u32 i,u32 j);

char RMBCnormal(Grafo G);

char RMBCrevierte(Grafo G);

char RMBCchicogrande(Grafo G);

char SwitchColores(Grafo G,u32 i,u32 j);

//Integrantes

// Dias Veronica Alejandra. Correo electrónico: vad41284@gmail.com
// Salgado Romina. Correo electrónico: romisalgado28@gmail.com
// Godoy Anahí Rocío. Correo electrónico: anahi.godoy20@gmail.com
