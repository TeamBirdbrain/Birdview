# Birdview
A plotting program written in C++ with Qt, meant to be used with the Birdsense
app.

## Requirements
* A C++14 compatible compiler
* Qt 5

## Compiling

### Linux
```bash
qmake -makefile
make
```

### Windows
Birdview can be compiled either with Visual Studio or QtCreator. With QtCreator
it's as simple as opening the project and hitting `Ctrl + B`. With Visual
Studio, you'll need to first install the Qt Visual Studio Tools, open the 
project, and then compile.
