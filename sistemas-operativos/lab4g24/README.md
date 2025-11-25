>#Laboratorio 4: Sistema de Archivos.
>>Antes de realizar el proyecto de laboratorio solicitado decidimos tratar de comprender brevemente como es el sistema de archivos en Xv6, y para ello utilizamos la informacion que esta a continuación.
>>###Sistema de archivos en Xv6:
>>La parte del sistema operativo que trata con los archivos se conoce como **sistema de archivos**.  
>>Los archivos son unidades lógicas de información creada por los procesos. Los procesos pueden leer los archivos existentes y crear otros si es necesario. La información que se almacena en los archivos debe ser **persistente**, no debe ser afectada por la creación y terminación de los procesos.
>>En este sentido sistema de archivos de xv6 brinda las siguientes características:  
>>>- Brinda estructuras de datos para representar el árbol de directorios y archivos, y almacenar las identidades de los bloques que guardan el contenido de cada archivo y recordar cuáles áreas del disco están libres.
>>>- El sistema de archivos soporta recuperación ante un crash (falla de electricidad o demás); esto es si ocurre la falla entonces el sistema tiene que funcionar correctamente después de reiniciarse.
>>>- Permite que diferentes procesos puedan operar en el sistema de archivos al mismo tiempo, por lo que hay coordinación en este aspecto.    
>>>- Cómo acceder al disco es más lento que acceder a la memoria, el sistema de archivos permite mantener en la memoria cache los bloques de disco más populares. 
>>###Implementacion del sistema de archivos en Xv6:
>>La implementación del sistema de archivos xv6 está organizada en 6 capas, como se muestra en la Figura 1.
>>La capa más baja lee y escribe bloques en el disco IDE a través del **buffercache**, que sincroniza el acceso a los bloques del disco, asegurándose de que solo un proceso del kernel a la vez pueda editar los datos del sistema de archivos almacenados en cualquier bloque en particular.  
>>La segunda capa **(Logging)** permite que las capas superiores envuelvan actualizaciones en varios bloques en una transacción, para garantizar que todos los bloques estén actualizados o ninguno.  
>>La tercera capa proporciona archivos sin nombre, cada uno representado mediante un **inodo** y una secuencia de bloques que contienen los datos del archivo.
>>La cuarta capa implementa directorios como un tipo especial de inodo cuyo contenido es una secuencia de entradas de directorio, cada una de las cuales contiene un nombre y una referencia al inodo del archivo nombrado.  
>>La quinta capa proporciona nombres de rutas jerárquicas como /usr/rtm/xv6/fs.c, utilizando la búsqueda recursiva.  
>>La capa final de los file descriptor abstrae muchos recursos por ejemplo tuberías, dispositivos, archivos, etc.  
>> ![](http://img.fenixzone.net/i/4hRCPOr.png) 
>> Figura 1.

>>El sistema de archivos debe tener un plan para almacenar inodos y bloques de contenido en el disco. Para hacerlo, xv6 divide el disco en varias secciones, como se muestra en la Figura 2. El sistema de archivos no usa el bloque 0 (contiene el sector de arranque). El primer bloque se llama **superblock**; contiene metadatos sobre el sistema de archivos (el tamaño del sistema de archivos en bloques, el número de bloques de datos, el número de inodos y el número de bloques en el registro). El segundo bloque contiene **inodos** de los cuales hablaremos específicamente más adelante. Después de eso, los **bloques de mapas de bits (bitmap)** rastrean qué bloques de datos están en uso. La mayoría de los bloques restantes son los bloques de datos; cada uno está marcado como libre en el bitmap o  sino almacenan contenido de archivos y directorios. Los bloques al final del disco contienen un registro que es parte de la capa de transacción (logging). 
>> ![](http://img.fenixzone.net/i/MuqY0C3.png)  
>> Figura 2.

>>###Implementacion de los archivos en Xv6:
>>Existen muchas formas para llevar un registro de qué bloques pertenecen a cual archivo. Una de estas y la utilizada en xv6 es una estructura de datos conocida como **inodo**, la cual lista los atributos y las direcciones de disco de los bloques de archivo. Entonces dado un inodo, entonces es posible encontrar todos los bloques de archivo. El inodo necesita estar en memoria solo cuando está abierto el archivo correspondiente.  
>>En xv6, el término inodo puede tener dos significados relacionados. Podría referirse a la estructura de datos en disco (dinode) que contiene el tamaño de un archivo y la lista de números de bloques de datos. O inode que se refiere a un inodo en la memoria, que contiene una copia del d-inodo en disco así como también información adicional que se necesita dentro del kernel.  
>>Todos los d-inodos en disco están empaquetados en un área contigua de disco llamada bloques de inodo. Cada inodo tiene el mismo tamaño, por lo que es fácil, dado un número n, encontrar el n° de i-nodo en el disco. De hecho, este número n, llamado número de inodo, muestra cómo se identifican los inodos en la implementación.  
>>El kernel mantiene el conjunto de inodos activos en la memoria; struct inode es la copia en memoria de un struct dinode en el disco. El kernel almacena un inodo en la memoria solo si hay punteros que se refieren a ese inodo. El campo ref cuenta el número de punteros que se refieren al inodo en memoria, y el kernel descarta el inodo de memoria si el recuento de referencia cae a cero.  
>>La memoria caché de inodo solo almacena en caché los inodos a los que el código del núcleo o las estructuras de datos tienen punteros. Su trabajo principal es realmente sincronizar el acceso por múltiples procesos. Si se utiliza un inodo con frecuencia, el caché del búfer probablemente lo mantendrá en la memoria si no lo guarda el caché de inodos.  
>>El inodo en disco se define mediante un struct dinode. El campo de tipo distingue entre archivos, directorios y archivos especiales (dispositivos). Un tipo de cero indica que un inodo en disco es gratis. El campo nlink cuenta el número de entradas de directorio que se refieren a este inodo, para reconocer cuándo se debe liberar el inodo. El campo Tamaño registra el número de bytes de contenido en el archivo tambien una matriz de números de bloque. Los datos de inodo se encuentran en los bloques listados en la matriz de addr del dinodo. Los primeros bloques de datos NDIRECT se enumeran en las primeras entradas NDIRECT en la matriz; estos bloques se llaman bloques directos. Los siguientes bloques de datos NINDIRECT no se enumeran en el inodo sino en un bloque de datos denominado bloque indirecto. La última entrada en el conjunto de direcciones da la dirección del bloque indirecto.  
>>Las funciones que provee xv6 para manejar inodos son:  
>>>- **ilock()**: Esto bloquea el inodo (para que ningún otro proceso pueda bloquearlo) y lee el inodo del disco, si es que aún no se ha leído.  
>>>- **ialloc()**: asigna un nuevo inodo. Pasa por encima de las estructuras del inodo en el disco, de a un bloque a la vez, buscando uno que esté marcado como libre. Cuando encuentra uno, lo reclama al escribir el nuevo tipo en el disco y luego devuelve una entrada del caché de inodos con la llamada final a iget.  
>>>- **iget()**:Obtiene un inodo, leído del disco a través del buffer caché.  
>>>- **iput()**: Libera un puntero C a un inodo al decrementar el recuento dereferencia. Si esta es la última referencia, la ranura del inodo en la memoria caché de inodos ahora está libre y puede reutilizarse para un inodo diferente. Si no hay referencias, entonces el inodo y sus bloques de datos deben estar libres.  
>>>- **iunlock()**: libera el bloqueo en el inodo.  

>>###Primera parte:  
>>En este parte debíamos implementar permisos de escritura y lectura para los archivos. Estos permisos serán parte de los atributos del archivo y podrán ser vistos como información adicional por parte de los usuarios.  
>>Añadimos los archivos complementarios al xv6. Para poder añadir permisos a inodos, modificamos el archivo fs.h, en struct dinode, añadiendo el campo ushort permissions “asignando” 2 bits  y “robando” 2 bits al campo minor asignándole 14 bits. Éste que es de tipo short (de 16bits), lo dejamos en 14.  
>>Una vez hecho esto, modificamos una serie de archivos:  
>>En user.h agregamos la nueva llamada a sistema chmod, aunque aun no las hayamos terminado de implementar.  
>>En syscall.h agregamos nuestra syscall con un número que la va identificar, este numero es unico ya que con el mismo indexaremos todas las syscalls.  
>>En usys.S se encuentra la funcion syscall(name) que está en codigo asembler. Esta funcion se encarga de asociar el numero que identifica a cada llamada a sistema a la varianle %eax.  
>>En syscall.c agregamos la llamada a sistema con el tipo extern int y despues las indexamos en un arreglo de syscalls con su número identificador correspondiente.  
>>Luego terminamos de implementar la syscall chmod (que nos dió la cátedra), en sysfile.c.  
>>Este syscall tendrá dos argumentos el nombre de ruta del archivo path y por otra parte el permiso ingresado. Recuperando estos argumentos con argint, procedemos a validarlos y  en este caso los permisos tiene que ser números comprendidos entre 0 y 3, caso contrario devolvemos error.  
>>Una vez que tenemos el permiso a asignar, con la función namei buscamos el inodo que está asociado a esa ruta, y le asignamos el permiso al campo correspondiente.  
>>Para cuidar que permisos están correctamente sincronizados entre la cache en memoria y el disco, modificamos las funciones iupdate() y tambien la función ilock();  
>>También es importante considerar modificar el mkfs.c; este archivo escrito en c se ejecuta durante el make y crea la imagen del sistema (fs.img) que se correrá en el QEMU. Tenemos que pensar que los archivos que se cargaran en el sistema como ls, mkdir y demás tengan asociados los máximos permisos para poder ser usados al arrancar el sistema; por ello modificamos la función ialloc en mkfs.c dotando a todos los archivos con permisos.  
>>Consideramos apropiado añadir los bits de permisos (S_IWRITE y S_IREAD) en el archivo file.h, definiendolas con 0x001 y 0x002 respectivamente.  
>>La tabla de permisos en nuestro caso quedaria asi:  

>>>>R   W  
>>>>0   0      //  no tiene ningún permiso (0)  
>>>>0   1      //  solo permiso de lectura (1)  
>>>>1   0      //   solo permiso de escritura (2)  
>>>>1   1      //  tiene ambos permisos (3)  

>> Para implementar los permisos en sys_open (ubicado en sysfile.c).   
>>Para entender esto hace falta considerar cómo se opera con archivos a nivel de los usuarios, o sea a nivel de los procesos.  
>>Para leer o escribir un archivo, este debe abrirse primero mediante open. Esta llamda especifica el nombre de archivo ue se va abrir ya sea como un nombre de ruta absoluto o relativo al directorio de trabajo y un código de O_RONLY, O_WRONLY o O_RDWR, que significa abrir para escritura , abrir para lectura o ambos. Para crear un archivo se utiliza el parámetro O_CREATE.  
>>Está llamada devuelve un file descriptor (un numero entero) que puede ser usado para leer y escribir.  
>>Al terminar el archivo se puede cerrar mediante close, que hace que el descriptor de archivo esté disponible para reutilizarlo en una llamada a open posterior.  
>>Dicho esto tenemos que acomodar las cosas para que haya concordancia entre los banderas de open y los permisos del archivo a abrir.  
>>Pusimos varias guardas del para cubrir los casos concretos donde hay una contradicción evidente.  
>>Por otro lado cuando el inodo tiene todos los permisos no debemos preocuparnos pq puede usar todas las banderas del open posible.  
>>Para los demás casos (si tiene permisos de solo lectura o sólo escritura) tenemos que acomodar las cosas que solo se habilite a hacer las cosas que corresponde. Para ellos usamos la función auxiliar set_file_rw().  
>>En el mismo archivo hicimos una modificación más a la función create que es la que se llama en el caso de la bandera O_CREATE. Está función devuelve un nuevo puntero a un inodo. Cuando lo creamos debemos asignarle los máximos permisos. Pero en el caso que queramos volver a “crear” el mismo archivo de alguna forma lo estaríamos habilitando a ser reescrito y esto no puede ocurrir si no tiene permisos de escritura de antemano; entonces consideramos este caso cuando el archivo ya existe.  
>>Concluimos con la parte 1 del proyecto modificando el archivo ls.c para terminar de implementar la impresión de permisos. Para lo cual modificamos la estructura stat, agregando el campo permissions que nos será útil para mostrarlo cuando se necesario.

>>##Segunda Parte:  
>>En la segunda parte del proyecto nos solicitaron que modificaramos las funciones **bmap()** e **itrunc()** del archivo fs.c. La primera se encarga de retornar el n-ésimo bloque de datos de un inode, allocandolo si hiciera falta. Esta función se llama tanto en la lectura como en la escritura. La segunda se encarga de eliminar completamente un inode cuando el número de links llega a 0.  
>>Para ello primero necesitamos revisar varias definiciones:
>>>- NDIRECT= 12 (Cantidad de bloques directos de un Inodo).  
>>>- NINDIRECT= (BSIZE/sizeof(uint))= (512/4)= 128.  
>>>- MAXFILE= NDIRECT + NINDIRECT.  
>>>- y el campo addrs del struct dinode= (NDIRECT+1)=13.  

>>Para poder escribir/leer la cantidad de bloque que se nos solicitaba decidimos modificar algunos de estos valores antes de modificar las funciones indicadas, los valores actualizados quedaron de la siguiente manera:  
>>>- NDIRECT= 12 (Cantidad de bloques directos de un Inodo).  
>>>- NINDIRECT= (BSIZE/sizeof(uint))= (512/4)= 128.  
>>>- MAXFILE= (NDIRECT + (7*NINDIRECT)+ 128).  
>>>- y el campo addrs del struct dinode= (NDIRECT+1)=13.  

>>Con estas modificaciones nos dispusimos a modificar bmap() e itrunc():  
>>La funcion bmap toma como parámetros un puntero ip a una estructura inode (struct inode ip) y un entero sin signo (uint) bn. Dentro de la función se declaran addr, a, i, k del tipo uint y también struct buf bp.La primera parte de la función quedó de la manera original, es decir corrobora que bn sea menor a la cantidad de bloques directos, si esto ocurre a addr se le asigna lo que que hay en ip->addrs[bn] y se compara esto con 0, es decir nos fijamos si ip->addrs[bn] existe, si no existe alocamos y en cualquier caso devolvemos la dirección del bloque de disco que necesitamos.  

>> ![](http://img.fenixzone.net/i/2BfKe0I.png)
>>Figura 3