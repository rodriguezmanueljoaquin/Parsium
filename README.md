# Lenguaje Parsium
## Autómatas, Teoría de Lenguajes y Compiladores - 1º cuatrimestre 2021
### Trabajo Práctico Especial
#### Instituto Tecnológico de Buenos Aires
---------------------
## Sobre el proyecto
*Parsium* es un lenguaje de programación diseñado para declarar y ejecutar máquinas de estados finitos (o autómatas finitos determinísticos) bajo un paradigma declarativo. Permitiendo al usuario parsear cadenas con el formato que él decida, y tomar decisiones a partir de si son o no aceptadas. El formato preferido para los archivos del lenguaje es `.pars`. 

------------------------------------------------------------
## Autores

* [**Rodríguez, Manuel Joaquín**](https://github.com/RodriguezManuel) - Legajo 60258
* [**Arca, Gonzalo**](https://github.com/gonzaloarca) - Legajo 60303
* [**Parma, Manuel Félix**](https://github.com/mparmaITBA) - Legajo 60425

------------------------------------------------------------

## Ubicación de material

El código fuente se encuentra en la carpeta raíz del proyecto. Los códigos de ejemplo se encuentran dentro de `examples`

------------------------------------------------------------

## Dependencias

* make
* Compilador de C
* flex
* bison

------------------------------------------------------------

## Cómo compilar el proyecto

Situado sobre la carpeta raíz del proyecto, ejecutar

    $ make all

De esta forma, se genera el ejecutable `pc`, que sería el compilador del lenguaje (Parsium Compiler)

------------------------------------------------------------

## Cómo compilar código Parsium

Habiendo ya compilado el proyecto, y situado sobre la carpeta raíz del proyecto, ejecutar

    $ cat entrada.pars | ./pc > salida.c
    $ cc salida.c
    $ ./a.out

De esta manera se genera la traducción en una salida intermedia dentro de un archivo C, y finalmente se obtiene el ejecutable del programa de Parsium utilizando el compilador de C (se genera el archivo `a.out`, pero puede utilizar otro nombre si modifica el uso del compilador de C).

------------------------------------------------------------

## Cómo utilizar Parsium en otro proyecto C
Generamos el archivo .pars y lo compilamos
	
	$ cat entrada.pars | ./pc > salida.c

El archivo salida.c contiene toda la traduccion en C del codigo generado en el lenguaje Parsium. Para utilizarlo dentro de otro archivo C es suficiente con incorporarlo en la linkedicion y definir la función que corre la máquina en un .h que el otro archivo incluya. Luego, se podra llamar a esta función para realizar la ejecución de la máquina con la cadena indicada por parámetro.

La función que ejecuta una máquina se debe declarar de la forma 

	bool run_machine_[machine_id](char *parse);

 donde `[machine_id]` denota el nombre asignado a la máquina.

------------------------------------------------------------

## Grámatica de Parsium
### Declaración y asignación de variables
	def TYPE varId = expression;

TYPE puede ser: `char`, `bool`, `machine`, `predicate`, `int` o `string`. 

Notamos que se puede realizar la declaracion sin la asignacion y viceversa.

### Bloque condicional
	if ( condition ) {
		[block]
	} else {
		[block]
	}

Notamos que el bloque `else` es opcional.

### Declaración de predicados
	def predicate predicateId = ( param_name ) {
		[block]
	};

Predicados default: 

	| isNumber	  | devuelve true si el carácter actual es un número.	  	|
	| isUpperCase | devuelve true si el carácter actual es una mayúscula. 	|
	| isLowerCase | devuelve true si el carácter actual es una minúscula. 	|

### Bloques de repetición hasta incumplimiento de condición
	while( condition ) {
		[block]
	}

### Declaración y asignación de máquina
	def machine machineId = <<
		transitions = [ transitionsArray ],
		initialState = initialStateId,
		finalStates = [ finalStatesIdArray ]
	>>;

Todas las maquinas cuentan con un estado `ERROR` por default que puede ser utilizado para llevar a la no aceptacion de una cadena.
### Transición
	originSymbol -> destinationSymbol when condition


Donde condition puede ser: 

	|     ANY     | sin importar el carácter actual se tomará esta transición.									|
	|  ‘carácter’ | se tomará la transición sólo si el carácter actual es el que se indica en la condición.		|
	| predicateId | se tomará la transición sólo si la ejecución del predicate devuelve true.					|

### Ejecución de una máquina
	parse string with machineId;
Esta ejecucion devuelve un valor booleano indicando si `string` es aceptado o no por la máquina.

