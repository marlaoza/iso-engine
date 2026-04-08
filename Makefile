MANAGERS = src/engine/managers/framemanager/framemanager.cpp src/engine/managers/cameramanager/cameramanager.cpp src/engine/managers/inputManager/inputManager.cpp src/engine/managers/dialogManager/dialogManager.cpp
ENTITIES = src/engine/entities/entity/entity.cpp src/engine/entities/unit/unit.cpp src/engine/entities/projectile/projectile.cpp src/engine/entities/skill/skill.cpp
EFFECTS = src/engine/effects/highlight/highlight.cpp src/engine/effects/particle/particle.cpp 
ENGINE = src/engine/map/map.cpp src/engine/geometry/geometry.cpp src/engine/render/render.cpp src/engine/ui/ui.cpp src/engine/sprite/sprite.cpp 

INCLUDES = -I src -I src/engine -I src/include/ -I src/include/sdl3

SHADERS = entity highlight particle tile ui

all: shaders
	g++ $(INCLUDES) -L src/lib -o main main.cpp $(MANAGERS) $(ENTITIES) $(EFFECTS) $(ENGINE) -lmingw32 -lSDL3 -lSDL3_ttf -static-libgcc -static-libstdc++

shaders:
	@for dir in $(SHADERS); do \
		glslangValidator -D -V -e main -o src/assets/shaders/$$dir/vertShader src/assets/shaders/$$dir/shader.vert; \
		glslangValidator -D -V -e main -o src/assets/shaders/$$dir/fragShader src/assets/shaders/$$dir/shader.frag; \
	done