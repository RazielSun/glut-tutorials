#!/bin/bash

cd `dirname $0`/

name=$1
inputName=$name".cpp"
outputName=$name".out"

# g++ $inputName -lGLEW -lSDL2 -framework OpenGL -o $outputName # for tutorial less 11
g++ $inputName -lGLEW -lSDL2 -framework OpenGL -o $outputName util_3d.cpp

# #!/bin/bash

# cd `dirname $0`/

# name=$1
# inputName=$name".cpp"
# outputName=$name".out"

# g++ $inputName -lGLEW -framework OpenGL -framework GLUT -o $outputName -Wno-deprecated