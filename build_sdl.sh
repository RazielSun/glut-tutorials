#!/bin/bash

cd `dirname $0`/

name=$1
inputName=$name".cpp"
outputName=$name".out"

g++ $inputName -lSDL2 -framework OpenGL -o $outputName