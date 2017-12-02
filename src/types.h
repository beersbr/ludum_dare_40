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
};

struct Frame {
    int id;
    GLuint glid;

    GLuint gl_texture_id;
    GLuint gl_depth_buffer_id;
};


struct Sprite {
    int id;
};

struct Scene;
struct Entity {
    int id;
    Scene *scene;
    
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale; 

    std::list<Entity *> children;
};

struct Scene {
    int id;

    GLuint vao;
    Frame frame;

    std::list <Entity *> entities;
};
