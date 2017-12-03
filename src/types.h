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
    Scene *scene;
    Sprite *sprite;
    Entity *parent;
    // std::sprite_hook = 

    std::list<Entity *> *children;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

struct Scene {
    int id;

    GLuint vao;
    Frame frame;

    std::list <Entity *> entities;
};


struct Controller {
    bool btn_up;
    bool btn_down;
    bool btn_left;
    bool btn_right;

    bool btn_a;
    bool btn_b;
    bool btn_x;
    bool btn_y;
};