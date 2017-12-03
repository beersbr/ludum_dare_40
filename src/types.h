#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_Image.h>

#include <glm/glm.hpp>

#define MALLOC(T) ((T*)malloc(sizeof(T)))

#define KILOBYTES(n) ((n)*1024)
#define MEGABYTES(n) (KILOBYTES(n)*1024)
#define GIGABYTES(n) (MEGABYTES(n)*1024)
#define TERABYTES(n) (GIGABYTES(n)*1024)


struct Window {
    SDL_Window *sdl_window;
    SDL_GLContext context;

    std::string window_title;
    int width;
    int height;
};

struct Shader {
    int id;
    GLuint glid;

    std::string vertex_shader_filename;
    std::string fragment_shader_filename;
    std::string name;

    std::map<std::string, GLint> *uniform_locations;

    bool bound;
};

struct Texture {
    int id;
    GLuint glid;

    std::string name;
    std::string image_path;
    glm::vec2 image_size;
};

struct Frame {
    int id;
    GLuint glid;

    GLuint gl_texture_id;
    GLuint gl_depth_buffer_id;
};


struct Entity;
struct Sprite {
    int id;

    Entity *parent;
    Texture *texture;

    glm::vec2 texture_frame_offset;
    glm::vec2 texture_frame_size;
};


struct Scene;
struct Entity {
    int id;
    std::string tag;
    Scene *scene;
    Sprite *sprite;
    Entity *parent;

    std::list<Entity *> *children;

    glm::vec3 acceleration;
    glm::vec3 velocity;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

struct GameController {

};

int DEFAULT_MEMORY_ARENA_SIZE_MB = MEGABYTES(1024);
struct MemoryArena {
    unsigned char *memory;
    int memory_size;

    unsigned char *current_memory_pointer;
};

typedef void (*SceneStartupFunc)(Scene*);
typedef void (*SceneUpdateFunc)(Scene*, float elapsed_time_s);
typedef void (*SceneShutdownFunc)(Scene*);

struct Scene {
    int id;

    MemoryArena memory_arena;

    GLuint vao;
    Frame frame;

    bool initialized;
    bool should_end;

    std::list <Entity *> entities;

    SceneStartupFunc startup;
    SceneUpdateFunc update;
    SceneShutdownFunc shutdown;

    Entity *player;

};


enum PROJECTILE_TYPE {
    NONE,
    LASER,
    BULLET,
    MISSILE,
    ORB
};

struct Projectile {
    Entity *entity;
    
    Entity *from;

    float Projectile_velocity;
    float Projectile_damage;
    PROJECTILE_TYPE projectile_type;
};

struct Player {
    Entity *entity;

    std::list<Entity *> options;

    float health;
    float experience;

    float primary_fire_interval;
    float secondary_fire_interval;

    float velocity_per_second;

    PROJECTILE_TYPE projectile_type;
};


struct GamePadController {
    bool btn_up;
    bool btn_down;
    bool btn_left;
    bool btn_right;

    bool btn_a;
    bool btn_b;
    bool btn_x;
    bool btn_y;
};
