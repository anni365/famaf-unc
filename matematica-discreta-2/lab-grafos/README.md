# Implementación de Proyecto sobre Grafos

Este repositorio contiene la entrega del proyecto para la materia Matemática Discreta II. El objetivo es implementar una estructura de datos de grafo utilizando el tipo de dato **`u32`** (entero de 32 bits sin signo) para la representación de todos los índices, grados y colores del grafo.

## Estructura de Archivos

- `Rii.h`: Contiene la librería, la estructura `GrafoSt` y las llamadas de las funciones pedidas en el proyecto.

- `641355.c`: Tiene la implementación completa de la lógica de coloreo y ordenamiento del grafo.

- `grafo_ej.c`: Grafo pequeño de ejemplo en formato DIMACS (en la práctica, probamos grafos de hasta *casi 1 millón de vértices y lados*)

---

## Funcionalidades Principales Implementadas

* **`ConstruccionDelGrafo()`:** Lee el grafo en formato DIMACS, gestiona la memoria y crea la representación interna.
* **`DestruccionDelGrafo()`:** Libera toda la memoria dinámica alocada por el grafo.
* **`Greedy()`:** Aplica el algoritmo de coloreo voraz sobre el orden actual de los vértices.
* **`Bipartito()`:** Determina si el grafo es 2-coloreable (bipartito) y lo colorea con 2 colores si lo es.
* **Ordenamientos:**
  * `OrdenNatural()`
  * `OrdenWelshPowell()`
  * `RMBCnormal()`, `RMBCrevierte()`, `RMBCchicogrande()`

Hay más funciones.

---

## Protocolo de Prueba y Compilación (Para la Cátedra)

**Nota:** Esta entrega incluye únicamente los archivos de la librería solicitada. No se incorpora ningún archivo con la función `main()` (como `test.c`), ya que es material provisto por la cátedra y no está permitido compartirlo.

### Cómo compilar

Compilar el proyecto utilizando el archivo de prueba entregado por la cátedra:

```bash
gcc -Wall -Wextra -O3 -std=c99 641355.c test.c -o prueba
```

### Cómo ejecutar

Ejecutar el programa indicando el grafo por entrada estándar:

```bash
./prueba < grafo_ej.c 
```

---

Este proyecto fue desarrollado en equipo. Esta es una copia del repositorio original (privado en GitLab) para mostrarlo en mi portfolio.

Los archivos de librería (`Rii.h` y `641355.c`) fueron desarrollados por:

- Dias Veronica
- Salgado Romina
- Godoy Anahí Rocío


