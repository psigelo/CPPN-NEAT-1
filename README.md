# CPPN-NEAT 

## Compiling

As simple as a *makefile*
```
$ cd CPPN-NEAT
$ make clean 
$ make
```
## Usage

We've incorporated both of the minimal actions related to evolving a network, i.e. training a certain network & using it.

### Network Training

In order to train a certain network ( in this example we use **genetic_encodings/default_00.genetic_encoding** ) the *-t* flag must be used followed by the file name in which the network is stored (in *JSON* format).
```
$ cd CPPN-NEAT 
$ ./executables/CPPN-NEAT -t genetic_encodings/default_00.genetic_encoding
```
This will create a file in the same folder with the same name ( with extension **.out.genetic_encoding** ) which contain the final network structure, i.e. the champion.

\* *We use JSON as a network structure in order to make a visual dashboard in the future using 'html' and 'jvascript'.*

You may also use the *-p* flag in order to print in the console the champion's fitness and the champion's network structure.
```
$ cd CPPN-NEAT 
$ ./executables/CPPN-NEAT -t genetic_encodings/default_00.genetic_encoding -p
```
### Network Usage

Once the network has been trained we can use it by specifying the *-x* flag followed by the file name ( in this example we use **genetic_encodings/default_00.out.genetic_encoding** ) in which the network is stored (in *JSON* format).

The last flag **must** be the value's specification flag *-v* followed by the number of inputs needed by the network  (an error will arrise if the number of inputs differ from the number of given values)
```
$ cd CPPN-NEAT 
$ ./executables/CPPN-NEAT -x genetic_encodings/default_00.out.genetic_encoding -v 0 0
```
This will return in the console the respective outputs of the network.

## Network Structure

The network structure is extremately simple. Again we will be using the **genetic_encodings/default_00.genetic_encoding** file.

To see more information about JSON go to the following [link](http://www.json.com/)

### Nodes

For the node definition we have:
```
"nodes":
		[
			{"exist": 1,"node": 0,"type": 0, "row": 0, "function": "IDENTITY"},
			{"exist": 1,"node": 1,"type": 0, "row": 0, "function": "IDENTITY"},
			{"exist": 1,"node": 2,"type": 2, "row": 1, "function": "SIGMOID"}
		]
```
Where:
- **exist** indicates if the node exists *(1)* or if it doesn't *(0)*. This is used for the algoithm in order to make the mating esear. For a starting network all the nodes are considered to exist.
- **node** represent the node's *id*. They must be correlative but not necesary ordered.
- **type** indicates the node type, i.e. *0: INPUT*, *1: HIDDEN* & *2: OUTUT* 
- **row** indicated the row in which the node is positioned. This is used for the algorithm in order to avoid recurrent connections. If two nodes are connected they must be in different rows.
- **function** indicates the node's function. There are (so far) a set of functions, i.e. *IDENTITY*, *SIGMOID*, *GAUSSIAN*, *ABS*, *SIN* & *COS*. The users can create as many functions as they want.

### Connections

For the connection definition we have:
```
"connection_genes":
		[
			{"exist": 1,"innovation": 0,"in": 0,"out": 2,"weight": 1.,"enable": 1},
			{"exist": 1,"innovation": 1,"in": 1,"out": 2,"weight": 1.,"enable": 1}
		]
```

