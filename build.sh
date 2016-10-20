#!/bin/bash

cd `dirname $0`/

name=$1
inputName=$name".cpp"
outputName=$name".out"

g++ $inputName `Magick++-config --cxxflags --cppflags` -lGLEW -lSDL2 -lassimp -framework OpenGL -o $outputName \
utils/utils.cpp \
utils/texture.cpp \
utils/pipeline.cpp \
utils/camera.cpp \
utils/shader_program.cpp \
utils/light.cpp \
utils/mesh.cpp \
`Magick++-config --ldflags --libs`