#!/bin/bash

cd `dirname $0`/

fileName="$1.cpp"

g++ $fileName -framework OpenGL -framework GLUT