CC = g++

OBJS = main.cpp

INCLUDE_DIRS = -IC:\SDL3\include -IC:\glew\include

LINKER_DIRS = -LC:\SDL3_precompiled\lib -LC:\glew\lib\Release\x64

LIBRARIES = -lmingw32 -lSDL3 -lopengl32 -lglew32

FLAGS = -Wall -Wl,-subsystem,console

OBJ_NAME = 02-Becoming-Interactive.exe

all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_DIRS) $(LINKER_DIRS) $(LIBRARIES) $(FLAGS) -o $(OBJ_NAME)

clean :
	if exist $(OBJ_NAME) del $(OBJ_NAME)
