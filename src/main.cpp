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

/*********************************************************************
 TYPES
 *********************************************************************/

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

    bool bound;
};

struct Texture {
    int id;
    GLuint glid;
};

struct Frame {
    int id;
    GLuint glid;
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

/*********************************************************************
 GLOBALS
 *********************************************************************/

static Window *window = nullptr;
static std::map<std::string, Shader *> SHADERS;

static int SCREEN_WIDTH = 1200;
static int SCREEN_HEIGHT = 800;

/*********************************************************************
 FUNCTION DEFINITIONS
 *********************************************************************/

std::string read_file(std::string filename);
void init_scene(Scene *scene, std::string name);
void init_frame(Frame *frame);
void init_texture(Texture *texture);
void init_shader(Shader *shader, std::string name, std::string vertex_filename, std::string fragment_filename);
void use_shader(Shader *shader);
void create_window(Window *win, std::string &title, int width, int height);

/*********************************************************************
 PROGRAM
 *********************************************************************/
int main(int argc, char * argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);

    window = (Window*)malloc(sizeof(Window));
    std::string window_title = "ludum dare 40";
    create_window(window, window_title, SCREEN_WIDTH, SCREEN_HEIGHT);

    Shader *shader = (Shader*)malloc(sizeof(Shader));

    GLuint vao = 0;
    glCreateVertexArrays(1, &vao);

    std::cout << "VAO: " << vao << std::endl;

    glBindVertexArray(vao);

    init_shader(shader, "default_shader", "media\\shaders\\simple_shader.vs.glsl", "media\\shaders\\simple_shader.fs.glsl");
    use_shader(shader);

    bool running = true;
    SDL_Event event;
    while(running) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                {
                    running = false;
                    break;
                }
            }
        }

        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        SDL_GL_SwapWindow(window->sdl_window);
    }

    return 0;
}

/*********************************************************************
 FUNCTIONS
 *********************************************************************/

std::string read_file(std::string filename)
{
    std::fstream file;
    file.open(filename.c_str(), std::ios::in | std::ios::binary);

    file.seekg(0, std::ios::end);
    int size = file.tellg();

    std::cout << "Reading file " << filename << " @ " << size << std::endl;

    file.seekg(0, std::ios::beg);

    char *med = (char*)malloc(sizeof(char)*size);
    file.read(med, size);

    std::string result = med;
    std::cout << result << std::endl;

    return result;
}


void init_scene(Scene *scene, std::string name)
{
    if (scene == nullptr) {
        std::cout << "scene is null" << std::endl;
        exit(1);
    }

    static int scene_ids = 0;

    memset(scene, 0, sizeof(Scene));
    scene->id = ++scene_ids;
    glGenVertexArrays(1, &scene->vao);
    glBindVertexArray(scene->vao);
    init_frame(&scene->frame);
}

void init_frame(Frame *frame)
{
    if (frame == nullptr) {
        std::cout << "frame is null" << std::endl;
        exit(1);
    }

    static int frame_ids = 0;

    memset(frame, 0, sizeof(Frame));
    frame->id = ++frame_ids;
    glGenFramebuffers(1, &frame->glid);
    glBindFramebuffer(GL_FRAMEBUFFER, frame->glid);

    // glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // GLuint depthrenderbuffer;
    // glGenRenderbuffers(1, &depthrenderbuffer);
    // glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    // glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    // GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    // glDrawBuffers(1, DrawBuffers);

    // if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    //     std::cout << "Could not create framebuffer" << std::endl;
    //     exit(1);
    // }

}

void init_texture(Texture *texture)
{
    if (texture == nullptr) {
        std::cout << "texture is null" << std::endl;
        exit(1);
    }

    static int texture_ids = 0;

    memset(texture, 0, sizeof(Texture));

    texture->id = ++texture_ids;

    glGenTextures(1, &texture->glid);
    glBindTexture(GL_TEXTURE_2D, texture->glid);
}

void init_shader(Shader *shader, std::string name, std::string vertex_filename, std::string fragment_filename)
{
    if ( shader == nullptr ) {
        std::cout << "ERROR: shader object is null" << std::endl;
        exit(1);
    }

    static int shader_ids = 0;

    memset((void*)shader, 0, sizeof(Shader));

    shader->id = ++shader_ids;
    shader->vertex_shader_filename = vertex_filename;
    shader->fragment_shader_filename = fragment_filename;
    shader->name = name;
    shader->bound = false;

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertex_shader_code = read_file(shader->vertex_shader_filename);
    std::string fragment_shader_code = read_file(shader->fragment_shader_filename);

    const char *vertex_shader_code_cstr = vertex_shader_code.c_str();
    const char *fragment_shader_code_cstr = fragment_shader_code.c_str();

    const int vertex_shader_code_size = vertex_shader_code.size();
    const int fragment_shader_code_size = fragment_shader_code.size();

    glShaderSource(vertex_shader, 1, &vertex_shader_code_cstr, &vertex_shader_code_size);
    glShaderSource(fragment_shader, 1, &fragment_shader_code_cstr, &fragment_shader_code_size);

    glCompileShader(vertex_shader);
    glCompileShader(fragment_shader);

    int vertex_compilation_result = 0;

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_compilation_result);

    if(!vertex_compilation_result) {
        int log_length = 0;
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);
        char *infolog = (char*)malloc(sizeof(char)*log_length);
        glGetShaderInfoLog(vertex_shader, log_length, NULL, &infolog[0]);
        std::cout << "VERTEX SHADER COMPILATION LOG\n\n" << infolog << std::endl;
    }

    int fragment_compilation_result = 0;

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_compilation_result);

    if(!fragment_compilation_result) {
        int log_length = 0;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);
        char *infolog = (char*)malloc(sizeof(char)*log_length);
        glGetShaderInfoLog(fragment_shader, log_length, NULL, &infolog[0]);
        std::cout << "FRAGMENT SHADER COMPILATION LOG\n\n" << infolog << std::endl;
        free(infolog);
    }


    shader->glid = glCreateProgram();
    glUseProgram(shader->glid);
    glAttachShader(shader->glid, vertex_shader);
    glAttachShader(shader->glid, fragment_shader);

    glLinkProgram(shader->glid);

    int compilation_result = 0;
    glGetProgramiv(shader->glid, GL_LINK_STATUS, &compilation_result);

	if ( compilation_result == GL_FALSE )
	{
		int log_length = 0;
		glGetProgramiv(shader->glid, GL_INFO_LOG_LENGTH, &log_length);
		char *infolog = (char*)malloc(sizeof(char)*log_length);
		glGetProgramInfoLog(shader->glid, log_length, NULL, &infolog[0]);
		std::cout << "SHADER COMPILATION ERROR: \n\n" <<  infolog << std::endl;
        free(infolog);
        return;
    }
    
    std::cout << "Compiled shader: " << name << std::endl;

    SHADERS[name] = shader;
}

void use_shader(Shader *shader) 
{
    if (shader == nullptr) {
        glUseProgram(0);
        return;
    }

    static GLuint bound_shader_id = 0;

    if (bound_shader_id != shader->glid ) {
        bound_shader_id = shader->glid;
        glUseProgram(bound_shader_id);
    }
}

void create_window(Window *win, std::string &title, int width, int height) 
{
    if (window == nullptr) {
        std::cout << "NO WINDOW!!!!" << std::endl;
        exit(1);
    }

    win->width = width;
    win->height = height;

    win->sdl_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win->width, win->height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    win->context = SDL_GL_CreateContext(win->sdl_window);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    glewExperimental = GL_TRUE;
    glewInit();
}