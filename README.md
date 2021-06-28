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
