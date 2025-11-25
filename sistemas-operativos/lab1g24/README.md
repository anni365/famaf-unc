>#Laboratorio 1: Syscalls para accseso a VGA.  

>- ##Parte 1:

>>Al iniciar ***XV6*** desde el emulador (con el comando make qemu) vimos que éste arranca en el modo texto (0x30) de manera predeterminada. 

>>Entonces sabemos que la memoria está dentro del dispositivo VGA en el rango de direcciones 0x000A0000 a 0x000BFFFF y que estas direcciones se mapean con las direcciones físicas de la memoria principal, por lo que para hacer algún cambio en el dispositivo basta acceder a  dichas direcciones.

>>Así si queremos escribir algo en la pantalla debemos posicionarnos en el lugar deseado considerando las dimensiones en el modo texto y cargar la información en el formato correspondiente a cada caracter.

>>Como se pedía en el enunciado, hicimos una modificación en el archivo **console.c**, agregando la función ***void* vgainit()**. 

>>En este caso decidimos colocar la palabra **"SO-2017"** en el extremo inferior izquierdo. Para hacerlo primero calculamos qué dirección se corresponde con dicha ubicación en la pantalla; en el caso del modo texto sabemos que toda la información relativa a los caracteres se encuentra en el rango de direcciones 0x000B8000 a 0x000B8FA0; entonces la ubicación para comenzar a escribir será 0x000B8FA0 menos 0x000009F lo que da 0x000B8F01. 

>>Pasamos esta memoria física con la función **P2V** a virtual, se la asignamos a un puntero y vamos modificando el contenido de la memoria asignando el color y luego el caracter de manera sucesiva hasta terminar la palabra deseada. 

>>Por último para que **vgainit()** se ejecute al iniciar el sistema fue colocada en el archivo **main.c** junto con las demas funciones de **"tipo init"** y tambien lo agregamos en **defs.h**.

>- ##Parte 2:

>>Para poder iniciar con el ejercicio era necesario recopilar un poco de ésta información:  

>>Los dispositivos de *E/S*, entre los que se incluye la VGA, constan de dos partes: el dispositivo controlador y el dispositivo en sí; el primero es el chip o conjunto de chips que controla físicamente el dispositivo. Cada tipo de dispositivo controlador es distinto y se requiere un software diferente para controlar a cada uno de ellos, este software es el denominado *driver*.  

>>A su vez todo dispositivo controlador tiene un número peque?o de registros que sirven para comunicarse con él. Para activar el dispositivo controlador, el driver recibe un comando del **SO** y después lo traduce en valores apropiados para escribirlos en los registros del dispositivo.  
>>La colección de todos los registros forma el espacio de puertos *E/S*.  

>>Hay instrucciones *IN* y *OUT* especiales que están disponibles en modo kernel que permiten a los drivers leer y escribir en los registros.    

>>En conclusión los  registros internos llevan parámetros importantes para la **VGA**,  y estos pueden ser accedidos mediante los puertos *E/S* a los que están conectados, que van desde el 3b0h al 3dfh y nos permiten su modificación y lectura. 

>>Veamos en la siguiente tabla los nombres de los componentes que son manejados mediante cada puerto, dándonos acceso a los registros de la **VGA** mediante los controladores de cada uno de ellos.    

>>***Tabla 1***

>>![](http://www.sromero.org/ext/articulos/modox/figura2-1.gif)

>>*En la **Tabla** 1 los nombres de los puertos no coinciden, pero su identificador nos ayudó en nuestro caso.*
[*Más info acá* ](http://www.sromero.org/ext/articulos/modox/modox2.html)

>>En este punto vimos necesario utilizar la llamada **"súper ayuda"** *(archivo modes.c)* del enunciado dado. De ese archivo pudimos sacar los registros necesarios para trabajar en el **modo texto (80x25)**, en el **modo gráfico (320x200)**, y también la función que nos permite escribir en dichos registros: ***void* write_regs()**. 

>>Entonces nos propusimos utilizar todo esto para extender la funcionalidad de **console.c** agregando los registros para cada modo mencionado, la función **void write_regs()**, los puertos y el número de registros necesarios. Es importante aclarar que dentro de la función ***void* write_regs()** hicimos modificaciones que se ajusten a los procedimientos, para escribir puertos de Xv6 (es el caso de las funciones utilizadas en la super ayuda *"ouportb"* e *"inportb"* que en Xv6 se denominan **out** e **inb** respectivamente). 

>>Luego para ver que el cambio de modo es efectivo en ***void* vgainit()** llamamos a ***void* write_regs()** dándole como argumentos los registros del modo gráfico y viendo que al volver a compilar el modo texto desapareció. 

>- ##Parte 3:

>>La tercera parte del proyecto requería la implementación de dos nuevas llamadas al sistemas para una modularización del punto anterior.

>>Recordemos que los procesadores pueden ejecutar instrucciones en dos modos: modo kernel y modo usuario; en el modo kernel el procesador tiene permitido ejecutar instrucciones privilegiadas, por ejemplo como es nuestro caso escribir en registros específicos. Para que un programa de usuario puede ejecutar una instrucción privilegiada tiene que cambiar al modo kernel para que el software en este modo puede realizar dicha instrucción solicitada.

>>Por eso, los procesadores proveen una instrucción especial que cambia el procesador desde el modo usuario al modo kernel y permite entrar al kernel (software que se ejecuta en el espacio kernel) en un punto de entrada especificado por el mismo kernel. El procesador x86 provee la instrucción int para este propósito.

>>Los mecanismos para implementar la instrucción int estan relacionados las llamadas a sistemas (system calls) , las excepciones y las interrupciones.

>>Una vez realizado el cambio, el kernel puede validar los argumentos de una llamada a sistema, decidir si un programa de usuario o aplicación tiene permitido realizar la operación requerida, y negarla o ejecutarla.

>>Una llamada al sistema permite a un programa de usuario solicitar un servicio del sistema operativo; en nuestro caso queríamos una llamada **int modeswitch(int mode)** que nos cambie de modos de **VGA** y otra **int plotpixel(int x, int y, int color)** que nos pinte un pixel en la pantalla estando el modo gráfico.

>>Por lo que comenzamos agregando al ***fichero* user.h** estas nuevas llamadas a sistema aunque todavía no las hemos implementado. De esta forma estarán disponibles para ser usadas en las aplicaciones.

>>Ahora a continuación también debemos modificar varios archivos, mas uno de ellos es **syscall.h** que define el número con el que se identifica cada *system call*. Este numero es único para cada una y es importante para indexar cada llamada en lo que se va a definir en el archivo llamado **syscall.c**. que modificaremos mas adelante.

>>Luego tenemos que asociar cada llamada a sistema con la instrucción int mencionada. Esto lo encontramos en el archivo usys.S . Allí vemos definida una función SYSCALL(name) que esta en assembler. Esta función asocia el numero único de cada llamada a la variable %eax y también va a generar la interrupcion que se corresponde con T_SYSCALL, que es #64 que esta reservado para las interrupciones hechas por llamadas a sistemas. Entonces agregamos dos nuevas funciones más ***SYSCALL(modeswitch)*** y abajo ***SYCALL(plotpixel)***

>>Ahora resta implementar las llamadas a sistemas y modificar **syscall.c**. La implentacion la realizamos en **sysproc.c**; en **sysfile.c** hay implementacions de otras llamadas pero vimos que estan relacionadas a llamadas relativas a los archivos.

>>Definimos ***int* sys_modeswitch(void)** e ***int* sys_plotpixel(void)**. Debemos notar que ambas llamadas necesitan tomar sus argumentos correspondientes, para ello utilizan la funcion auxilar *argint*: esta usa el registro %esp del espacio de usuario para encontrar el n-ésimo elemento, dado que %esp apunta a la dirección de retorno de la llamada a sistema. Los argumentos estan justo arriba, en %esp+4. Entonces el e-nesimo está en %esp+4+4*n.

>>En cuanto a los procedimientos que realizan estas llamadas es muy similar a lo que se hizo antes: necesitamos escribir los registros correspondientes a cada modo, si el argumento es **1** entonces pasamos al **modo gráfico**, **caso contrario** pasamos al **modo texto**.
>>Para ello creamos el fichero **vga.h** con los puertos; **console.h** que nos da acceso a los registros y los incluimos en **sysproc.c**; y usamos la funcion ***void* write_regs()**, despues de incluirla en **defs.h**; el modo lo recuperamos usando argint().

>>***Int* sys_plotpixel(void)** tiene que buscar tres argumentos considerando el orden que sera dado por el usuario. Para programar el procedimiento usamos la misma idea de la función **vgainit()** y la información del enunciado.

>>Es importante notar que las llamadas *devuelven un valor*. Esto permite ver que se hayan efectuado correctamente o no.

>>Ahora resta modificar **syscall.c**. Esto consiste en agregar las dos nuevas llamadas a sistema como un tipo *extern int*; y luego indexarlas en un arreglo syscall con su correspondiente numero identificador. Este arreglo sera usado luego por el procedimiento syscall definido allí abajo.

>>Ahora si las llamadas a sistema deberían funcionar.

>>Resta solo modificar **vgainit()** para que tome un argumento *int mode* y cambie el modo.
>>Para esto debimos hacer una función **modeswitch(int mode)** similar a *sys_modeswitch* que definimos en **console.c**. No podemos usar **modeswitch(int mode)** definida para el usuario, porque nos encontramos en el kernel.


>- ##Parte 4:

>>Agregamos los archivos **testplot.c** y **testplotcool.c** a nuestro directorio de xv6 y tambien en el Makefile.

>>Una vez que logramos entender el funcionamiento de cada operación y probar los test en el emulador, decidimos modificar el archivo   **tesplotcool.c**, creando nuestras propias imágenes (con ayudas de patrones de internet) a través de una matriz con valores enteros (los cuales determinan el color que tomaría cada píxel) y tambien con ciclos anidados para poder mostrar nuestro dibujo por pantalla, utilizando las llamadas a sistemas creada switchmode (int mode) y plotpixel(int x, int y, int color).
 
>>Para determinar los colores nos guiamos por la sigiente grilla:

>>![](http://jm00092.freehostia.com/cqb/img/paleta64.gif)

>>Del archivo original conservamos el ciclo for que se encarga de colorear el fondo (cambiando solo el valor del color); agregamos otro ciclo para definir el suelo (similar al primero, solo que cambiamos el origen del primer pixel y lo delimitamos) y por último agregamos otros for's que definen los objetos distribuidos en el entorno gráfico, dentro de cada uno definimos la ubicación del primer pixel (primer elemento de la matriz que generamos antes), como si fuera una coordenada, y delimitamos su tama?o. El mismo ciclo se encarga de *"dibujar"*,tras pasar al modo gráfico, la matriz que definimos antes generando asi el dibujo que se muestra.  

