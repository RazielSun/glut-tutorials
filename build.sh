#!/bin/bash

cd `dirname $0`/

name=$1
inputName=$name".cpp"
outputName=$name".out"

g++ $inputName -framework OpenGL -framework GLUT -o $outputName -Wno-deprecated