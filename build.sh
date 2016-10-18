#!/bin/bash

cd `dirname $0`/

name=$1
inputName=$name".cpp"
outputName=$name".out"

g++ $inputName `Magick++-config --cxxflags --cppflags` -lGLEW -lSDL2 -framework OpenGL -o $outputName \
utils/util_3d.cpp \
utils/util_texture.cpp \
utils/util_pipeline.cpp \
utils/util_camera.cpp \
utils/util_shader_program.cpp \
utils/util_light.cpp \
`Magick++-config --ldflags --libs`