#!/bin/bash

cd `dirname $0`/

name=$1
inputName=$name".cpp"
outputName=$name".out"

g++ $inputName `Magick++-config --cxxflags --cppflags` -lGLEW -lSDL2 -lassimp -framework OpenGL -o $outputName \
utils/utils.cpp \
utils/texture.cpp \
utils/cubemap_texture.cpp \
utils/pipeline.cpp \
utils/camera.cpp \
utils/mesh.cpp \
utils/skybox.cpp \
utils/shadow_map_fbo.cpp \
utils/shader_program.cpp \
utils/light_program.cpp \
utils/light_shadow_program.cpp \
utils/shadow_program.cpp \
utils/skybox_program.cpp \
utils/gbuffer.cpp \
utils/deferred_light_program.cpp \
utils/deferred_dir_light_program.cpp \
utils/deferred_point_light_program.cpp \
utils/skinned_mesh.cpp \
utils/skinned_only_mesh.cpp \
utils/skinning_program.cpp \
utils/skeletal_animation.cpp \
`Magick++-config --ldflags --libs`