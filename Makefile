SRCS = main.cpp src/enginedata.cpp src/map.cpp src/geometry.cpp src/unit.cpp src/render.cpp src/camera.cpp src/ui.cpp src/highlight.cpp src/skill.cpp src/managers/inputManager.cpp src/managers/dialogManager.cpp
INCLUDES = -I src -I src/include/ -I src/include/sdl3

all:
	g++ $(INCLUDES) -L src/lib -o main $(SRCS) -lmingw32 -lSDL3 -lSDL3_ttf -static-libgcc -static-libstdc++
