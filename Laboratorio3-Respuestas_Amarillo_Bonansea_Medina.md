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

### 3. Ejecutar `./forkprio 10 30 1 | sort -k 4 -h` y describir el resultado de la ejecución. ¿Por qué el total del tiempo ejecutado por los procesos hijos puede ser mayor que el tiempo que espera el proceso padre?

```bash
$ ./forkprio 10 30 1 | sort -k 4 -h
Child 379602 (nice  1):  29
Child 379603 (nice  1):  29
Child 379604 (nice  1):  29
Child 379605 (nice  1):  29
Child 379606 (nice  1):  29
Child 379607 (nice  1):  29
Child 379608 (nice  1):  29
Child 379609 (nice  1):  29
Child 379610 (nice  1):  29
Child 379611 (nice  1):  29
```

En la salida podemos observar que:

1. Se crearon 10 procesos hijos, cada uno con un nice value de 1, lo que indica una prioridad ligeramente reducida.

2. Todos los procesos hijos reportan haber ejecutado durante 29 segundos, aunque el programa padre estaba configurado para esperar 30 segundos.

3. La salida está ordenada por la cuarta columna (el tiempo de ejecución) usando el comando `sort -k 4 -h`, pero como todos los procesos tienen el mismo tiempo de ejecución, no hay diferencia visible en el ordenamiento.

4. La suma total del tiempo de CPU utilizado por todos los procesos hijos es de 290 segundos (10 procesos × 29 segundos cada uno), lo cual es aproximadamente 9,7 veces mayor que el tiempo real transcurrido (30 segundos).

El total del tiempo ejecutado por los procesos hijos puede ser mayor que el tiempo que espera el proceso padre por varias razones:

1. **Paralelismo real en sistemas multiprocesador**: En un sistema con múltiples núcleos o procesadores, varios procesos hijos pueden ejecutarse simultáneamente. Si tenemos 10 procesos ejecutándose en paralelo durante 30 segundos, el tiempo total de CPU utilizado sería 300 segundos (10 × 30), aunque el tiempo de reloj transcurrido sea de solo 30 segundos.

2. **Sobrecarga del planificador**: El kernel debe dedicar tiempo para cambiar de contexto entre los diferentes procesos. Esto añade tiempo adicional de CPU que no es contabilizado en el tiempo de espera del proceso padre.

3. **Contabilidad de tiempo de sistema versus tiempo de usuario**: El tiempo reportado por los procesos hijos incluye tanto el tiempo de usuario como el tiempo de sistema utilizado por cada proceso, mientras que el proceso padre simplemente espera durante un tiempo fijo sin considerar estos detalles.

4. **Retardos en la entrega de señales**: Aunque el padre espera 30 segundos y luego envía una señal SIGTERM a todos sus hijos, puede haber pequeños retardos entre el momento en que se envía la señal y cuando cada proceso hijo la recibe y procesa.

### 4. Si el comando anterior se ejecuta indicando que no se cambien las prioridades de los procesos hijos, ¿Cúal es el resultado? Explicar por qué.

```bash
$ ./forkprio 10 30 0 | sort -k 4 -h
Child 408390 (nice  0):  29
Child 408391 (nice  0):  29
Child 408392 (nice  0):  29
Child 408393 (nice  0):  29
Child 408394 (nice  0):  29
Child 408395 (nice  0):  29
Child 408396 (nice  0):  29
Child 408397 (nice  0):  29
Child 408398 (nice  0):  29
Child 408399 (nice  0):  29
```

Al ejecutar el comando sin modificar las prioridades de los procesos hijos (nice=0), observamos que los resultados son idénticos a los obtenidos cuando ejecutamos los procesos con prioridad reducida (nice=1):

1. En ambos casos, todos los procesos hijos reportan haber ejecutado durante 29 segundos, casi el tiempo completo que el padre estuvo esperando (30 segundos).

2. La suma total del tiempo de CPU utilizado es aproximadamente 290 segundos en ambos casos, que es casi 10 veces el tiempo de espera del proceso padre.

Esta similitud en los resultados puede explicarse por las características del hardware utilizado y el comportamiento del planificador:

- **Hardware de alto rendimiento**: La prueba se ejecutó en un procesador Intel Core i7-10700K, que cuenta con 8 núcleos físicos y 16 hilos lógicos gracias a la tecnología Hyper-Threading. Con esta capacidad de procesamiento paralelo, el sistema puede ejecutar eficientemente los 10 procesos hijos casi simultáneamente, independientemente de las pequeñas diferencias en sus valores de nice.

- **Diferencia mínima entre los valores de nice**: La diferencia entre nice=0 y nice=1 es muy pequeña en términos de prioridad. En un sistema con recursos abundantes como este i7, esta diferencia es prácticamente imperceptible, ya que hay suficientes recursos de CPU disponibles para todos los procesos.

- **Planificador CFS (Completely Fair Scheduler)**: El planificador moderno de Linux está diseñado para ser justo en la asignación de tiempo de CPU, especialmente en sistemas con múltiples núcleos. Cuando hay suficientes recursos disponibles, el planificador puede asignar tiempo de CPU a todos los procesos de manera eficiente sin que se note la diferencia en prioridad.

- **Asignación de procesos a diferentes núcleos**: Con 8 núcleos físicos disponibles, el planificador puede distribuir los 10 procesos entre los diferentes núcleos, minimizando la competencia directa por tiempo de CPU y permitiendo que cada proceso obtenga casi todo el tiempo de CPU que necesita.

En conclusión pequeñas diferencias en la prioridad de los procesos no afectan significativamente su tiempo de ejecución cuando no hay una competencia intensa por recursos.

## Ejercicio 4
### 1. ¿Cual de las dos variantes tuvo menos costo, la creación de hilos o la creación de procesos? Justificar.

### 2. ¿Cuánto afecta el uso de la opción -w?