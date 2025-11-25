>#Laboratorio 2: Semáforos.
>##Parte 1: 
>>La parte uno consistía en la implementación de 4 nuevas llamadas a sistema.

>>Antes de esto decidimos crear una estructura de datos que implemente los semáforos, que es lo que hicimos en semaphore.h donde determinamos un máximo de 14 semaforos (el nombre de cada semaforo será un número entre 0 y 13), un número para cada uno de los flags y creamos la estructura semaphore; dentro de la misma se encuentra un ***spinlock***, un ***value***, y un ***counter***.

>>Pusimos todos nuestros semáforos en un arreglo (semaphore sv[]). También creamos el archivo *semafhore.c* donde implementamos la función **svinit** para inicializar el arreglo de semáforos inicializando su contador en 0 y cada spinlok con el string *“semaphore”*. Esta función será luego llamada en el archivo *main.c* para que al iniciar el sistema el arreglo de semáforos pueda ser usado. 

>>Una vez teniendo la estructura de nuestros semáforos pudimos pasar a hacer las syscalls solicitadas, para ello modificamos una serie de archivos:

>>- En ***user.h*** agregamos las 4 llamadas a sistema aunque aun no las hayamos implementado.
>>- ***En syscall.h*** agregamos nuestras nuevas syscalls con un número que las va identificar, este numero es unico ya que con el mismo indexamos todas las syscalls.
>>- En ***usys.S*** alli se encuentra la funcion *syscall(name)* que está en codigo asembler. Esta funcion se encarga de asociar el numero que identifica a cada llamada a sistema a la varianle *%eax*.
>>- En ***sysproc.c*** implementamos las 4 llamadas a sistema con la funcionalidad pedida:
>>>- ***sys_sem_open(semaphore, flags, value)***
>>>- ***sys_sem_close(semaphore)***
>>>- ***sys_sem_up(semaphore)***
>>>- ***sys_sem_down(semaphore)***

>>- En ***syscall.c*** acá agregamos las 4 nuevas llamadas a sistema con el tipo extern int y despues las indexamos en un arreglo de syscalls con su número identificador correspondiente.

>> ###Funciones especiales usadas en  las syscalls

>>Las funciones usadas en las syscalls de los semáforos fueron: acquire, release, wakeup, sleep y argint. Las dos primeras están relacionadas al mecanismo que ofrece xv6 para poder implementar la exclusión mutua, lo cual es de interés en este caso, pues los semáforos solo deben ser modificados por un proceso que los use a la vez. 

>>Por otro parte wakeup y sleep fueron usados para implementar las llamadas sys_up y sys_down respectivamente. A su vez, argint la habíamos usado anteriormente y es la función que nos permite extraer de la pila del kernel, los argumentos con los que las llamadas al sistema fueron ejecutadas por parte de usuario.

>>Hablaremos sobre cada una de ellas, para explicar lo que hacen y cómo las implementa xv6.

>>###Locks: Acquire y Release

>>Xv6 corre en multiprocesadores, las computadoras con múltiples CPUs ejecutan código independientemente. Estas múltiples CPUs operan con una sola memoria física y comparten las estructuras de datos; para ello xv6 debe introducir un mecanismo de coordinación que evita que interfieran entre ellos. Incluso en un uniprocesador, xv6 debe usar algún mecanismo para evitar que los manejadores de interrupciones interfieran con el código sin interrupciones.

>>Xv6 usa el mismo concepto de bajo nivel para ambos casos: el lock. El lock implementa la exclusión mutua, asegurando que un CPU a la vez pueda manejar el lock. De esta manera evitamos la condiciones de carrera. 

>>###Código

>>xv6 representa el lock como una struct spinlock. El campo crítico en la estructura es locked, una palabra que es cero cuando el lock está abierto (no bloqueado) y no nulo cuando está cerrado (bloqueo).

>>Las operaciones sobre el lock van a ser dos:

>>- acquire: espera hasta que el lock no esté bloqueado, es decir sea igual a 0, y luego cambia su estado a bloqueado y regresa.
>>- release: marca al lock como no bloqueado, es decir, lo pone en 0 nuevamente.  

>>Una forma de pensar la implementación de acquire es:

>>while (1) {  
>>>if (lock->locked == 0) {  
>>>>lock-> locked = 1;  
>>>>break;  
>>}

>>>}

>>Sin embargo, si quisiéramos implementarlo de esta forma tendríamos problemas pues podrían aparecer condiciones de carrera.

>>Para evitar esto se usa una instrucción especial xchg que ofrecen los procesadores 386, con la idea de que sean atómicas.  
>>En una operación atómica, xchg intercambia una palabra en memoria con el contenido del registro.  
>>La función acquire repite esta instrucción en el ciclo; en cada iteración lee lk->locked y atómicamente lo pone a 1. Si el lock está cerrado, lk→locked va ser 1, así que xchg retorna 1 y el ciclo continúa. Si xchg retorna 0, acquire ha adquirido el lock y el ciclo puede terminar. Una vez que el lock es adquirido, acquire guarda, para debuggear, el CPU y la pila que llamo al lock.   
>>La función release es lo opuesto: borra los campos de debuggear y luego libera el lock, es decir pone lk->locked en 0.  
>>Estas funciones se encuentran implementadas en: spinlock.c. También es importante notar que esta función utiliza las funciones: pushcli and popcli para evitar un interbloqueo. Para evitar esta situación nunca modifica un lock con las interrupciones habilitadas. Para ello usa las dos funciones mencionadas para manejar la pila de operaciones de interruptores deshabilitados (cli es un intruccion de x86 que deshabilita las interrupciones) . Acquires llama pushcli antes de adquirir el lock y release llama a popcli después de liberar el lock.  


>>###Dormir y Despertar - Sleep y Wakeup  
>>El lock es un herramienta útil y correcta, pero  tiene el efecto de requerir la espera ocupada. En esencia, un lock comprueba si se permite la entrada cuando un proceso desea entrar a la región crítica. Si no se permite, el proceso solo espera un ciclo estrecho hasta que se permita la entrada.

>>Este método no solo desperdicia tiempo de la CPU, sino que también puede tener efectos inesperados.  
>>Para esto existen estas primitivas de comunicación entre procesos que los bloquean en vez de desperdiciar tiempo de la CPU cuando no pueden entrar a sus regiones críticas.  
>>Basicamente sleep y wakeup trabajan de la siguiente manera: sleep(chanel) pone el proceso que lo llamó a “dormir” en el valor arbitrario chanel, llamado el canal de espera, liberando la CPU para otras tareas; por otro lado,  wakeup(chanel) despierta todos los procesos que “duermen” en chanel (si los hay), haciendo que los procesos que llamaron a sleep vuelvan. Si ningún proceso está a la espera en chanel, despertar no hace nada.  
>>Sin embargo hace falta que las funciones sleep y wakeup tomen más argumentos, para que no se pierdan señales de despertar.  
>>A sleep debemos pasarle un lock para que pueda liberarlo después de que el proceso que lo llamó sea marcado como dormido y que está como tal en el canal.  
>>Esto hace que cualquier otro proceso tenga que esperar hasta que el que llamó a sleep esté realmente dormido; y de esta manera si alguno de esos procesos tiene un wakeup entonces va a encontrar a alguno dormido.  Una vez que el proceso que llamó a sleep sea despertado sleep adquiere el lock antes de volver. 

>>###Código  
>>El código de estas funciones puede encontrarse en proc.c  
>>La idea básica para implementarlos, es que sleep marque el proceso concurrente como SLEEPING y luego llama sched para liberar el procesador; por otra parte, wakeup busca por todos los procesos durmientes en el canal dormido y lo marca como RUNNABLE.  
>>Sleep chequea que exista un proceso concurrente y que le hayan pasado un lock del proceso llamador. Luego sleep adquiere el ptable.lock (lock de la tabla de proceso). Mantener el lock del llamador fue necesario: para asegurarse que ningún otro proceso pueda empezar una llamada wakeup. Pero ahora que el sleep tiene ptable.lock es seguro liberar el lock del llamador: entonces otro proceso puede realizar una llamada a wakeup, pero wakeup no correrá hasta que  pueda adquirir el ptable.lock, así que debe esperar hasta que sleep haya terminado de poner el proceso a dormir, haciendo que el wakeup y sleep estén en concordancia.  
>>También se verifica que el lock llamador no sea igual ptable.lock para evitar un interbloqueo.  
>>Asi sleep pone el proceso a dormir guardando en el canal correspondiente (es su otro parámetro), cambia el estado del proceso y luego llama sched().  
>>En algún otro momento de la vida, un proceso llama a wakeup (con su canal como parámetro); wakeu adquiere el ptable.lock y luego llama a wakeup1, que hace el trabajo real. wakeup1 cicla en la tabla de procesos. Cuando encuentra un proceso con estado SLEEPING que machea con el canal correspondiente, lo cambia a RUNNABLE. Asi la proxima vez que el planificador arranque este proceso estará listo para correr.  
>>Wakeup siempre debe ser llamado mientras mantiene el lock que chequea la condición del wakeup.  Esto también contribuye a que no se pierdan las señales de despertar.  
>>Algunas veces múltiples procesos están durmiendo en el mismo canal. Una sola llamada a wakeup, los despierta a todos. Uno de ellos correra mas rapido y adquiere el lock con el que sleep fue llamado, entonces los otros procesos deberían volver a dormir; esta es la razón por la que sleep siempre es llamado dentro de un ciclo que chequea la condición.
