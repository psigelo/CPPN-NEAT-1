# CPPN-NEAT 

## Compiling

Just execute the *makefile* in order to compile the program

```
$ cd CPPN-NEAT
$ make clean 
$ make
```

## Usage

We've incorporated both of the minimal actions related to evolving a network, i.e. training a certain network & using it.

### Training a network

In order to rain a network ( by default *genetic_encodings/default_00.genetic_encoding* ) you must use the flag *-t* followed by the file to train

```
$ cd CPPN-NEAT 
$ ./executables/CPPN-NEAT -t genetic_encodings/default_00.genetic_encoding
```

This will print in the console the 

This will create a file in the same folder with the same name ( with extension **.out.genetic_encoding** ) which contain the network structure.

\* We use JSON as a network structure in order to make a visual panel in the future using *html* and *jvascript*.

### Use a trained network

```
$ cd CPPN-NEAT 
$
$ # In order to train a network file (extension .genetic_encoding)
$ ./executables/CPPN-NEAT -t genetic_encodings/default_00.genetic_encoding
$ 
$ # In order to execute the trained network 
$ ./executables/CPPN-NEAT -x genetic_encodings/default_00.out.genetic_encoding -v 0 0
```