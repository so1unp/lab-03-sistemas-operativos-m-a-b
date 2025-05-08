# Laboratorio 3 - Respuestas Amarillo Bonansea Medina

## Ejercicio 1
### 1. ¿Cuáles son las señales que no es posible atrapar? ¿Por qué?

Las señales que no se pueden atrapar son `SIGKILL` y `SIGSTOP`. Estas señales están diseñadas para controlar el comportamiento de los procesos de manera que no puedan ser ignoradas o manejadas por el proceso receptor. Esta restricción está implementada a nivel del kernel en sistemas Unix/Linux y es una característica de diseño deliberada para asegurar que el sistema operativo siempre mantenga control sobre los procesos garantizado un mecanismo para terminar o detener los mismos. Además, también existen otras dos señales (32 y 33) que no están definidas en el estándar `POSIX`, pero que pueden ser utilizadas por algunos sistemas operativos para propósitos específicos.

```bash
$ ./sigcatch &
[1] 366047
No pude capturar: Killed
No pude capturar: Stopped (signal)
No pude capturar: Unknown signal 32
No pude capturar: Unknown signal 33
$ kill -15 366047
Señal recibida: Terminated
```

## Ejercicio 2
### 1. Ejecutar `./forkprio 1 0 1 >/dev/null &`. Luego ejecutar el comando `ps -fp $(pgrep forkprio)`. ¿En que estados están los procesos? Explicar.

```bash
$ ./forkprio 1 0 1 >/dev/null &
[2] 374649
$ ps -fp $(pgrep forkprio)
UID          PID    PPID  C STIME TTY      STAT   TIME CMD
mariano+  374649  362843  0 15:50 pts/0    S      0:00 ./forkprio 1 0 1
mariano+  374651  374649 99 15:50 pts/0    RN     0:24 ./forkprio 1 0 1
```

En la salida del comando `ps` podemos observar dos procesos:

1. El proceso padre (PID 374649) se encuentra en estado "S" (Sleep), lo que significa que está en un estado de espera interrumpible. Este proceso ha creado un hijo y está esperando a que este termine su ejecución.

2. El proceso hijo (PID 374651) se encuentra en estado "RN", donde:
   - "R" significa que está en estado "Running", es decir, está ejecutándose activamente o en la cola de ejecución listo para ser ejecutado.
   - "N" indica que se está ejecutando con una prioridad baja (nice).

Además, el proceso hijo muestra un uso de CPU del 99%, lo que indica que está consumiendo intensivamente recursos de CPU, mientras que el proceso padre muestra un 0% porque simplemente está esperando.

### 2. Ejecutar `./forkprio 1 10 1` en tu computadora y luego en el servidor de la cátedra ¿Existe alguna diferencia en el resultado? Ejecutar múltiples veces para estar seguro de los resultados.

* **En Local:**
    ```bash
    $ ./forkprio 1 10 1
    Child 396926 (nice  1):   9
    ```

* **En el servidor:**
    ```bash
    $./forkprio 1 10 1
    Child 10141 (nice  1):	  9
    ```

- No se aprecian diferencias significativas en los resultados entre la ejecución local y en el servidor de la cátedra (se ejecuto 5 veces en ambos lugares).
- La consistencia en los resultados entre la máquina local y el servidor sugiere que el comportamiento del planificador en relación con procesos de baja prioridad es similar en ambos sistemas, al menos para esta prueba específica con un solo proceso hijo.
- La principal diferencia entre ambas ejecuciones podria llegar a darse en que el servidor de la catedra tiene una carga de trabajo diferente, con una mayor cantidad de procesos y usuarios activos, lo que podría afectar el rendimiento y la asignación de recursos. Pero en este caso, no se observó una diferencia notable en el tiempo de ejecución.

### 3. Ejecutar `./forkprio 10 30 1 | sort -k 4 -h` y describir el resultado de la ejecución. ¿Por qué el total del tiempo ejecutado por los procesos hijos puede ser mayor que el tiempo que espera el proceso padre?

```bash
$ ./forkprio 10 30 1 | sort -k 4 -h
Child 18239 (nice  1):   23
Child 18240 (nice  1):   22
Child 18241 (nice  1):   23
Child 18242 (nice  1):   24
Child 18243 (nice  1):   22
Child 18244 (nice  1):   23
Child 18245 (nice  1):   24
Child 18246 (nice  1):   23
Child 18247 (nice  1):   23
Child 18248 (nice  1):   24
```

En la salida podemos observar que:

1. Se crearon 10 procesos hijos, cada uno con un nice value de 1, lo que indica una prioridad ligeramente reducida.

2. El tiempo de ejecución de los procesos hijos varía entre 22 y 24 segundos, aunque el programa padre estaba configurado para esperar 30 segundos.

3. La salida está ordenada por la cuarta columna (el tiempo de ejecución) utilizando el comando `sort -k 4 -h`, lo que nos permite ver claramente la distribución de tiempos de CPU entre los procesos hijos.

4. La suma total del tiempo de CPU utilizado por todos los procesos hijos es aproximadamente 231 segundos (sumando todos los tiempos individuales), lo cual es mayor que el tiempo real transcurrido (30 segundos).

El total del tiempo ejecutado por los procesos hijos puede ser mayor que el tiempo que espera el proceso padre debido a los siguientes factores:

1. **Paralelismo real en sistemas multiprocesador**: En un sistema con múltiples núcleos o procesadores, varios procesos hijos pueden ejecutarse simultáneamente en diferentes núcleos. Si hay 10 procesos ejecutándose en paralelo durante aproximadamente 23 segundos cada uno, el tiempo total de CPU utilizado sería cercano a 230 segundos, aunque el tiempo de reloj transcurrido sea de solo 30 segundos.

2. **Tiempo de CPU vs. tiempo de reloj**: El tiempo que reporta cada proceso hijo es su tiempo de CPU efectivo (cuánto tiempo el procesador estuvo ejecutando sus instrucciones), no el tiempo de reloj transcurrido. En un sistema multitarea, varios procesos comparten los recursos de CPU, lo que permite que la suma de sus tiempos de CPU exceda el tiempo real transcurrido.

3. **Planificación del sistema operativo**: El planificador del sistema operativo asigna tiempo de CPU a múltiples procesos de forma concurrente, alternando entre ellos. Esto permite que varios procesos avancen "simultáneamente" desde la perspectiva del usuario, aunque técnicamente están compartiendo tiempo de CPU.

4. **Distribución en múltiples núcleos**: Si cada proceso hijo se ejecuta principalmente en un núcleo fisico distinto, cada uno puede acumular tiempo de CPU independientemente, resultando en una suma total mayor al tiempo real transcurrido.

En este caso específico, observamos que ningún proceso hijo alcanzó los 30 segundos completos de tiempo de CPU (varían entre 22-24 segundos), lo que sugiere que hubo cierta competencia por recursos de CPU incluso con la disponibilidad de múltiples núcleos. Esto puede deberse a que algunos procesos compartieron núcleos o a que el sistema operativo reservó recursos para otros procesos del sistema.

### 4. Si el comando anterior se ejecuta indicando que no se cambien las prioridades de los procesos hijos, ¿Cúal es el resultado? Explicar por qué.

```bash
$ ./forkprio 10 30 0 | sort -k 4 -h
Child 21973 (nice  0):   23
Child 21974 (nice  0):   24
Child 21975 (nice  0):   23
Child 21976 (nice  0):   23
Child 21977 (nice  0):   23
Child 21978 (nice  0):   23
Child 21979 (nice  0):   23
Child 21980 (nice  0):   23
Child 21981 (nice  0):   23
Child 21982 (nice  0):   23
```

Al ejecutar el comando sin modificar las prioridades de los procesos hijos (nice=0), observamos resultados muy similares a los obtenidos con prioridad reducida (nice=1):

1. En ambos casos (nice=0 y nice=1), los procesos hijos reportan tiempos de ejecución entre 22 y 24 segundos, cuando el proceso padre estuvo esperando 30 segundos.

2. La suma total del tiempo de CPU utilizado en este caso es aproximadamente 231 segundos, prácticamente idéntica al caso anterior.

3. La distribución de los tiempos de ejecución es también muy similar, con la mayoría de los procesos ejecutándose durante 23 segundos.

El rendimiento similar entre procesos con nice=0 y nice=1 demuestra que en sistemas modernos con múltiples núcleos y bajo condiciones de baja carga, pequeñas diferencias en la prioridad de los procesos tienen un impacto mínimo en su tiempo de ejecución. La verdadera diferencia en el rendimiento debido a la prioridad se volvería más evidente en sistemas con alta carga o con recursos limitados de CPU, donde los procesos compiten más intensamente por el tiempo de procesador.

## Ejercicio 4

```bash
$ /usr/bin/time -p ./benchmark -p 1000
Probando fork()...
real 0.12
user 0.18
sys 0.15

$ /usr/bin/time -p ./benchmark -t 1000
Probando pthread_create()...
real 0.07
user 0.00
sys 0.06

$ /usr/bin/time -p ./benchmark -p -w 1000
Probando fork()...
real 0.28
user 0.15
sys 0.13

$ /usr/bin/time -p ./benchmark -t -w 1000
Probando pthread_create()...
real 0.06
user 0.00
sys 0.03
```

### 1. ¿Cual de las dos variantes tuvo menos costo, la creación de hilos o la creación de procesos? Justificar.

Según los resultados obtenidos, la creación de hilos tuvo significativamente menos costo que la creación de procesos. Podemos observar esto en las siguientes comparaciones:

**Sin esperar por la terminación (-w no utilizada):**
- Procesos (`-p 1000`): real 0.12s, user 0.18s, sys 0.15s
- Hilos (`-t 1000`): real 0.07s, user 0.00s, sys 0.06s

**Esperando por la terminación (-w utilizada):**
- Procesos (`-p -w 1000`): real 0.28s, user 0.15s, sys 0.13s
- Hilos (`-t -w 1000`): real 0.06s, user 0.00s, sys 0.03s

La creación de hilos es más eficiente por las siguientes razones:

1. **Memoria compartida**: Los hilos comparten el mismo espacio de direcciones, por lo que no es necesario duplicar la memoria del proceso como ocurre en el caso de `fork()`.

2. **Menor sobrecarga del sistema**: La creación de procesos requiere más operaciones del sistema operativo, como duplicar tablas de páginas, descriptores de archivos, y otros recursos del proceso padre, mientras que los hilos solo necesitan crear una nueva pila y estructura de control.

3. **Contexto de ejecución más ligero**: Los hilos tienen un contexto más pequeño que los procesos, lo que hace que su creación y cambio de contexto sea más rápido.

4. **Uso eficiente de recursos**: Los tiempos de sistema (`sys`) para los hilos son considerablemente menores, lo que indica un menor uso de recursos del kernel.

### 2. ¿Cuánto afecta el uso de la opción -w?

La opción `-w` hace que el proceso/hilo padre espere a que cada proceso/hilo hijo termine antes de crear el siguiente. Esta opción tiene un impacto diferente en procesos y en hilos:

**Para procesos:**
- Sin `-w`: real 0.12s, user 0.18s, sys 0.15s 
- Con `-w`: real 0.28s, user 0.15s, sys 0.13s
- Impacto: El tiempo real aumenta un 133% (de 0.12s a 0.28s), pero el tiempo total de CPU (user + sys) disminuye ligeramente de 0.33s a 0.28s.

**Para hilos:**
- Sin `-w`: real 0.07s, user 0.00s, sys 0.06s
- Con `-w`: real 0.06s, user 0.00s, sys 0.03s
- Impacto: El tiempo real disminuye ligeramente, y el tiempo de sistema se reduce a la mitad (de 0.06s a 0.03s).

**Análisis del impacto:**

1. **En procesos**: La opción `-w` aumenta significativamente el tiempo real de ejecución porque cada proceso debe completarse antes de crear el siguiente, eliminando el paralelismo. Sin embargo, reduce la contención por recursos del sistema, lo que explica la ligera disminución en el tiempo total de CPU.

2. **En hilos**: La opción `-w` tiene un impacto menor o incluso positivo en el tiempo real, posiblemente debido a:
   - Menor sobrecarga de gestión al tener menos hilos activos simultáneamente
   - Mejor utilización de la caché del procesador al tener menos hilos compitiendo por ella
   - Eliminación de la sobrecarga de sincronización entre múltiples hilos

3. **Diferencias entre procesos e hilos**:
   - En procesos, esperar por la terminación elimina la posibilidad de paralelismo real entre procesos, lo que aumenta significativamente el tiempo de ejecución.
   - En hilos, la creación secuencial reduce la sobrecarga del sistema de gestión de hilos, compensando cualquier pérdida de paralelismo.