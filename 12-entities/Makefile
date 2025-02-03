CC = g++

OBJS = main.cpp shader.cpp texture.cpp model.cpp entity.cpp

INCLUDE_DIRS = -IC:\SDL3\include -IC:\SDL3_image\include -IC:\glm -IC:\glew\include

LINKER_DIRS = -LC:\SDL3_precompiled\lib -LC:\glew\lib\Release\x64

LIBRARIES = -lmingw32 -lSDL3 -lSDL3_image -lopengl32 -lglew32

FLAGS = -Wall -Wl,-subsystem,console

OBJ_NAME = 12-Entities.exe

all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_DIRS) $(LINKER_DIRS) $(LIBRARIES) $(FLAGS) -o $(OBJ_NAME)

clean :
	if exist $(OBJ_NAME) del $(OBJ_NAME)
