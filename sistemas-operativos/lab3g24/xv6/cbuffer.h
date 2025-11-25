#pragma once

/*
 * cbuffer: Wrapper a un arreglo cualquiera para darle semántica de buffer
 *          circular. Las operaciones asumen que el llamador sabe lo que está
 *          haciendo y no verifican nada.
 */

/* Información de estado del buffer circular */
struct cbuffer {
	/* Capacidad del buffer */
	unsigned int size;
	/* Tamaño de cada elemento (sizeof de lo que queramos meter) */
	unsigned int elemsize;
	/* Posición del primer elemento */
	unsigned int start;
	/* Cantidad de elementos en el buffer */
	unsigned int count;
	/* Buffer donde se guardan los datos */
	char * data;
};

extern struct cbuffer buffers[3]; 

/*
 * Dado un buffer existente en memoria, inicializar un struct cbuffer
 * que guarde información para darle semántica de buffer circular.
 */
void cbuffer_init(struct cbuffer * cb, unsigned int size,
  unsigned int esize, void * store);

/* Consulta si el buffer circular está lleno */
int cbuffer_is_full(const struct cbuffer * cb);

/* Consulta si el buffer circular está vacío */
int cbuffer_is_empty(const struct cbuffer * cb);

/*
 * Copia el elemento APUNTADO por elem al final del buffer.
 * Si el buffer está lleno, sobreescribe el primer elemento.
 */
void cbuffer_write(struct cbuffer * cb, void * elem);

/*
 * Extrae el primer elemento del buffer y lo copia a lo apuntado por elem.
 * Rompe el buffer si está vacío.
 */
void cbuffer_read(struct cbuffer * cb, void * elem);
