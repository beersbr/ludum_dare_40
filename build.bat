@echo off 

cls
set SOURCE_DIRECTORY=./src
set INCLUDE_DIRECTORY=./thirdparty/include
set LIBRARY_DIRECTORY=./thirdparty/lib

cl /Zi %SOURCE_DIRECTORY%/main.cpp /I %INCLUDE_DIRECTORY% /link /SUBSYSTEM:CONSOLE /LIBPATH:%LIBRARY_DIRECTORY% glew32.lib SDL2main.lib SDL2.lib opengl32.lib