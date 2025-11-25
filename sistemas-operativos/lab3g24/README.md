>#Laboratorio 3: MLFQ, modificación del planificador de Xv6.
  

>**Breve explicación de los diferentes puntos:**

>**Parte 1: **Luego de agregar los archivos verduiops.c y frutaflops.c, efectivamente comprobamos lo sugerido, vimos que el proceso verduiops se degrada (basándonos en la medición de performance propuesta) a medida que ejecutamos más procesos frutaflops.

>**Parte 2:** En este punto se nos pedía implementar 3 prioridades distintas para cada proceso; para ellos agregamos un nuevo campo en struct proc llamado priority, que tiene un tipo de datos nuevo llamado procpriority definido como un tipo enum, en donde a las diferentes prioridades se las relaciona con un número de 0 a 2; de esta forma **MINIMUM = 0, INTERMEDIATE = 1** y **MAXIMUM = 2**.  
>Luego se nos pedía que al iniciarse un proceso tenga máxima prioridad, y que también variara segun su comportamiento.  
>Para asignarle máxima prioridad a un proceso modificamos la función allocproc() que es la encargada de “crear” un nuevo proceso. También decidimos modificar la función fork() que crea un hijo a partir del padre, como este hijo se consideraría un nuevo proceso también le asignamos la máxima prioridad.  
>Como dijimos la prioridad debe variar según el comportamiento de la siguiente forma:
>>- Ascender cada vez que un proceso bloquea antes de terminar su quántum.
>>- Descender cada vez que un proceso pasa todo el quántum realizando cómputo.

> Para eso el siguiente diagrama de estados de proceso puede ser útil, y en concreto ver como xv6 realiza estas transiciones:

>>![](http://img.fenixzone.net/i/LlhpBQy.png)

>Los procesos RUNNABLE (listos) son los que pueden ejecutarse en cualquier momento, lo que equivaldría a pasar tener el estado RUNNING; el encargado de realizar esta transición es el planificador (scheduler) del que se hablará más adelante. Ahora un estado que está en estado RUNNING o ejecutándose actualmente, puede tener dos tipos de comportamientos:  
>>- CPU-bound: ser un proceso que invierte la mayor parte de su tiempo realizando cálculos.
>>- I/O-bound: ser un proceso que invierte la mayor parte de su tiempo esperando I/O.

> Si se trata de un proceso CPU-bound, el mismo nunca realizará las llamadas a las funciones sleep y wake-up que eran primitivas de comunicación entre procesos que permitían bloquearlos y luego reactivarlos cuando sea necesario para evitar desperdiciar tiempo de CPU, mientras los mismos esperan alguna tarea solicitada.  
>De esta manera cuando un proceso es CPU-bound en xv6 este tendrá una detención involuntaria cuando sea tiempo de pasar a ejecutar otro proceso; esto ocurre cuando trap llame a la función yield() que cambiará el estado de proceso a RUNNABLE y también devolverá el control al planificador para que seleccione otro procesos.  
>Si un proceso es I/O-bound entonces este llamará a sleep, lo que lo obligará a detenerse,  hasta que en algún momento se llame wakeup para reactivarlo.  
>Por eso en yiel() descenderemos la prioridad si proceso tiene una prioridad mayor a la mínima, y por otra parte en sleep aumentaremos la prioridad si es menor a máxima.  

>**Parte 3:** Esta parte consistía en reemplazar el planificador de xv6 para que ejecute los procesos por orden de llegada; para ello usamos cbuffer.c y cbuffer.h en donde se implementa una cola con arreglos circulares. Por el diagrama anterior sabemos que los procesos que toma el planificador deben estar en estado RUNNABLE, por eso vamos a agregar a la cola todos los procesos que tengan tal estado.   
>Esto ocurre en las siguientes funciones:  

>- userinit() que carga el primer proceso que se ejecuta;  
>- fork() debemos cargar el nuevo proceso creado;  
>- yield()  y wakeup() como dijimos anteriormente;  
>- kill() tambien establecen los procesos a RUNNABLE por lo que estos también deben agregarse a la cola.  

> El nuevo planificador consistirá en un ciclo que verifica si la cola no está vacía y si esto es asi lee el primer elemento de la cola, y asi sucesivamente.

>##PLANIFICADOR DE XV6  
>Xv6 utiliza como algoritmo de planificación uno llamado Round-Robin (turno circular). A cada proceso se le asigna un intervalo de tiempo, conocido como quántum, durante el cual se le permite ejecutarse. Si el proceso se sigue ejecutando al final del quántum, la CPU es apropiada para dársela a otro proceso. Si el proceso se bloquea o termina antes que haya transcurrido el quántum, la conmutación de la CPU se realiza cuando el proceso se bloquea, desde luego.  
> La única cuestión interesante con este algoritmo es la longitud del quántum. Para conmutar de un proceso a otro se requiere cierta cantidad de tiempo: guardar y cargar tanto registros como mapas de memoria, actualizar varias tablas y listas, vaciar y recargar la memoria caché y así sucesivamente. Esto es lo que se denomina conmutación de proceso o conmutación de contexto.  
>Es importante tratar de que el tiempo de conmutación de contexto no sea demasiado grande porque con esto perderemos tiempo de CPU. Otro factor es que si al quántum se le asigna un tiempo más largo que la ráfaga promedio CPU, la apropiación no ocurrirá con mucha frecuencia.  
>Bien ahora vemos como xv6 implementa este famoso algoritmo; esto lo encontramos en proc.c en la función scheduler.  
> En esta función se implementa un ciclo simple que encuentra un proceso para ejecutarlo, lo ejecuta hasta que se detenga y repite esto indefinidamente.  
>Vemos que scheduler mantiene ptable.lock para la mayoria de sus acciones pero libera el bloqueo (y habilita las interrupciones) una vez en cada interaccion. Luego el planificador hace un bucle sobre la tabla de procesos buscando alguno que tenga p->state == RUNNABLE. Una vez que encuentra un proceso, establece en la variable c->proc el proceso seleccionado, cambia a la tabla de páginas del proceso con switchuvm, marca el proceso como RUNNING y luego llama a swtch para comenzar a ejecutarlo.  
> La función switch es la encargada de realizar el cambio de contexto; basicamente switch solo guarda y restaura conjuntos de registros, llamados justamente contextos; cada contexto está representado por struct context*, un puntero a una estructura almacenada en la pila del kernel involucrada. Entonces luego de ejcutar swtch ya no estaríamos mas en el planificador, sino que nos habriamos desplazadado a otra parte del código, concretamente al proceso que deséabamos ejecutar.  
> Eventualmente se volverá al scheduler, ya sea involuntariamente cuando un proceso ha consumido todo su quantum (via yield()) o voluntariamente en caso de bloquearse (via sleep()); entonces luego de esto la variable c->proc vuelve a establecerse en cero y la tabla de procesos es liberada.  

>##PLANIFICADOR MLFQ (Colas multinivel realimentadas).  
>Antes de ver **MLFQ** veamos **MLQ** para comprender mejor:  

>- MLQ (Multi Level Queues) es un algoritmo de planificación. Esto quiere decir que clasifican los procesos en diferentes grupos a los que se le asignan distintas estrategias de planificación. Una característica del algoritmo es que es apropiativa, es decir, si llega un proceso con mayor prioridad que el que se está ejecutando podrá expulsarse y apropiarse del procesador.

> **MLFQ (Multi Level Feedback Queues)** se basa en algoritmos de colas multinivel, pero permiten el movimiento de los procesos de unas colas a otras.  
>Mediante la planificación con colas multinivel realimentadas, un proceso se puede mover de una cola a otra dependiendo de su comportamiento en tiempo de ejecución.  
>El funcionamiento de este algoritmo consiste en ejecutar los procesos de la cola de prioridad más alta, a continuación se pasan a ejecutar los procesos de la siguiente cola y así sucesivamente.

>- Hacer que un proceso que duerme sea planificable:  
Para que un proceso que duerme sea planificable debe cambiar su estado de SLEEPING a RUNNABLE esto sucede solo cuando el proceso es despertado y luego agregado a la cola correspondiente.   

>- Ejecutar un proceso planificable:  
Para ejecutar un proceso planificable el scheduler solo selecciona un proceso de la cola de prioridad máxima en ese momento.  
>- Replanificar un proceso que ha agotado su tiempo de procesador disponible:                   
Luego de que un proceso ha sido seleccionado en el scheduler, de alguna cola no vacia y de maxima prioridad en su momento, cambia su estado a RUNNING, se realiza un cambio de contexto y se va al código del mencionado proceso. Cuando éste consume su quantum se produce una interrupción, se llama a yield donde se le disminuye la prioridad, se lo encola en su nueva cola y se llama a sched que realiza otro cambio de contexto al código del scheduler. 
>- Bloquear un proceso que está esperando entrada/salida:
Cuando un proceso se bloquea se lo manda a dormir, es decir que cambia su estado a SLEEPING, aumentamos su prioridad, se llama a sched la cual realiza un cambio de contexto y se vuelve al scheduler.  

>**Gráfico de comparación:**  

>>![](http://img.fenixzone.net/i/OjGPM1B.png)