#include <windows.h>
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
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "types.h"

/*********************************************************************
 GLOBALS
 *********************************************************************/

static Window *window = nullptr;
static std::map<std::string, Shader *> SHADERS;

static int SCREEN_WIDTH = 1200;
static int SCREEN_HEIGHT = 800;

static std::list<Scene *> SCENE_STACK;

/*********************************************************************
 FUNCTION DEFINITIONS
 *********************************************************************/

std::string read_file(std::string filename);
void init_scene(Scene *scene, std::string name);
void push_scene(Scene *scene);
void use_scene(Scene *scene);
Scene *pop_scene();
void init_frame(Frame *frame);
void use_frame(Frame *frame);
void init_shader(Shader *shader, std::string name, std::string vertex_filename, std::string fragment_filename);
void set_shader_uniform_1i(Shader *shader, std::string uniform_name, int value);
void set_shader_uniform_1f(Shader *shader, std::string uniform_name, float value);
void set_shader_uniform_matrix4fv(Shader *shader, std::string uniform_name, int count, bool transpose, glm::mat4 *matrices);

GLint get_shader_uniform_location(Shader *shader, std::string uniform_name);
void use_shader(Shader *shader);
void create_window(Window *win, std::string &title, int width, int height);

void init_texture(Texture *texture, std::string name, std::string image_path);
void init_entity(Entity *entity, Scene *scene, glm::vec3 position, glm::vec3 scale = glm::vec3(1.f, 1.f, 1.f), glm::vec3 rotation = glm::vec3(0.f, 0.f, 0.f));
void init_sprite(Sprite *sprite, Entity *parent, Texture *texture, glm::vec2 offset = glm::vec2(0.f, 0.f), glm::vec2 frame_size = glm::vec2(0.f, 0.f));
void add_entity(Entity *entity, Entity *parent);

void draw_entity(Entity *entity);

/*********************************************************************
 PROGRAM
 *********************************************************************/
int main(int argc, char * argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF);

    window = MALLOC(Window);

    std::string window_title = "ludum dare 40";
    create_window(window, window_title, SCREEN_WIDTH, SCREEN_HEIGHT);

    Controller player_controller = {};

    //TODO(Brett): add memory manager...
    Shader *default_shader = MALLOC(Shader);
    Shader *frame_shader = MALLOC(Shader);
    Shader *sprite_shader = MALLOC(Shader);

    init_shader(default_shader, "default", "media\\shaders\\simple.vs.glsl", "media\\shaders\\simple.fs.glsl");
    init_shader(frame_shader, "frame", "media\\shaders\\frame.vs.glsl", "media\\shaders\\frame.fs.glsl");
    init_shader(sprite_shader, "sprite", "media\\shaders\\sprite.vs.glsl", "media\\shaders\\sprite.fs.glsl");

    Texture *ship_texture = MALLOC(Texture);
    init_texture(ship_texture, "blue_ship", "media\\images\\PNG\\playerShip2_blue.png");

    Texture *option_texture = MALLOC(Texture);
    init_texture(option_texture, "blue_option", "media\\images\\PNG\\ufoBlue.png");

    Scene *scene = MALLOC(Scene);
    init_scene(scene, "main_scene");


    Entity *player = MALLOC(Entity);
    init_entity(player, scene,
                glm::vec3(SCREEN_WIDTH/2.f, SCREEN_HEIGHT/2.f, 0.f),
                glm::vec3(1.f, 1.f, 1.f),
                glm::vec3(0.f, 0.f, 0.f));

    player->sprite = MALLOC(Sprite);
    init_sprite(player->sprite, player, ship_texture);
    
    glm::vec2 image_size = player->sprite->texture->image_size;
    player->scale = glm::vec3(image_size.x, image_size.y, 0.f);
    player->rotation = glm::vec3(180.f, 0.f, 0.f);

    
    Entity *option = MALLOC(Entity);
    init_entity(option, nullptr,
                glm::vec3(100.f, 0.f, -1.f),
                glm::vec3(30.f, 30.f, 30.f),
                glm::vec3(0.f, 0.f, 0.f));
    
    option->sprite = MALLOC(Sprite);
    init_sprite(option->sprite, option, option_texture);

    add_entity(option, player);

    bool running = true;

    float start_time = SDL_GetTicks();
    float total_time_s = 0.0f;
    float current_time = start_time;

    float angle = 0.f;
    SDL_Event event;

    float target_fps = 60.f;
    float target_frame_time_s = 1.f/target_fps;
    float frame_interval_s = 0.f;
    float frames = 0.f;

    while(running) {
        int last_time = current_time;
        current_time = SDL_GetTicks();
        float elapsed_time_s = (current_time - last_time)/ 1000.f;

        total_time_s += elapsed_time_s;

        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                {
                    running = false;
                    break;
                }
                case SDL_KEYDOWN:
                {
                    switch(event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                        {
                            running = false;
                            break;
                        }

                        case SDLK_UP:
                        {
                            player_controller.btn_up = true;
                            break;
                        }
                        case SDLK_DOWN:
                        {
                            player_controller.btn_down = true;
                            break;
                        }
                        case SDLK_LEFT:
                        {
                            player_controller.btn_left = true;
                            break;
                        }
                        case SDLK_RIGHT:
                        {
                            player_controller.btn_right = true;
                            break;
                        }
                    }

                    break;
                }
                case SDL_KEYUP:
                {
                    switch(event.key.keysym.sym) {
                        case SDLK_UP:
                        {
                            player_controller.btn_up = false;
                            break;
                        }
                        case SDLK_DOWN:
                        {
                            player_controller.btn_down = false;
                            break;
                        }
                        case SDLK_LEFT:
                        {
                            player_controller.btn_left = false;
                            break;
                        }
                        case SDLK_RIGHT:
                        {
                            player_controller.btn_right = false;
                            break;
                        }
                        break;
                    }
                }
            }
        }
        
        angle += 90.f * elapsed_time_s;
        option->position = glm::vec3(100.f * cosf(glm::radians(angle)), 100.f * sinf(glm::radians(angle)), -1.f);

        float player_velocity_per_second = 100.f;

        player->acceleration = glm::vec3(0.f);

        if (player_controller.btn_up) {
            player->acceleration.y += 1.0f;
        }
        else if (player_controller.btn_down) {
            player->acceleration.y -= 1.0f;
        }

        if (player_controller.btn_left) {
            player->acceleration.x -= 1.f;
        }
        else if (player_controller.btn_right) {
            player->acceleration.x += 1.f;
        }

        if (glm::length(player->acceleration) > 0.f) {
            player->acceleration = glm::normalize(player->acceleration);
            player->velocity += player->acceleration * (player_velocity_per_second*elapsed_time_s);
        }

        player->velocity += player->velocity * -1.f * 0.1f;
        player->position += player->velocity;

        if (frame_interval_s > 0.f ) {
            frame_interval_s -= elapsed_time_s;
            continue;
        }

        frames += 1;
        // std::cout << "FPS: " << frames/total_time_s << std::endl;
        frame_interval_s = target_frame_time_s + frame_interval_s;

        //NOTE(Brett): Draw scene
        use_shader(default_shader);
        use_frame(&scene->frame);

        glClearColor(0.f, 0.f, 0.5f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw_entity(player);

        use_frame(nullptr);
        use_shader(frame_shader);
        set_shader_uniform_1i(frame_shader, "frame_texture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, scene->frame.gl_texture_id);
        // glClearColor(1.f, 0.f, 1.f, 1.f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    memset(med, 0, size);
    file.read(med, size);

    std::string result = std::string(med);
    result[size] = '\0';
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

    glGenTextures(1, &frame->gl_texture_id);
    glBindTexture(GL_TEXTURE_2D, frame->gl_texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    frame->gl_depth_buffer_id;
    glGenRenderbuffers(1, &frame->gl_depth_buffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, frame->gl_depth_buffer_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frame->gl_depth_buffer_id);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, frame->gl_texture_id, 0);
    GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error creating framebuffer" << std::endl;
        exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void use_frame(Frame *frame)
{
    GLuint id = 0;
    if (frame == nullptr){
        id = 0;
    }
    else {
        id = frame->glid;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, id);
}


void push_scene(Scene *scene)
{
    if (scene==nullptr) {
        std::cout << "Cannot push null scene" << std::endl;
        exit(1);
    }

    SCENE_STACK.push_back(scene);
}


void use_scene(Scene *scene)
{
    if (scene==nullptr) {
        std::cout << "Cannot set null scene" << std::endl;
        exit(1);
    }

    // Scene *scene = SCENE_STACK.back();
    SCENE_STACK.pop_back();
    SCENE_STACK.push_back(scene);
}


Scene *pop_scene()
{
    Scene *scene = SCENE_STACK.back();
    SCENE_STACK.pop_back();
    return scene;
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
    shader->uniform_locations = new std::map<std::string, GLint>();

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


GLint get_shader_uniform_location(Shader *shader, std::string uniform_name) 
{
    if (shader == nullptr) {
        std::cout << "Attempt to read nullptr instead of shader" << std::endl;
        exit(1);
    }

    if (shader->uniform_locations->find(uniform_name) == shader->uniform_locations->end()) {
        GLint location = glGetUniformLocation(shader->glid, uniform_name.c_str());

        if ( location < 0 ) {
            std::cout << "Uniform location " << uniform_name << " not found!" << std::endl;
            // exit(1);
        }
        
        (*shader->uniform_locations)[uniform_name] = location;
        return (*shader->uniform_locations)[uniform_name];
    }
    else {
        return (*shader->uniform_locations)[uniform_name];
    }

    //NOTE(Brett): SHould never be here
    return -1;
}


void set_shader_uniform_1i(Shader *shader, std::string uniform_name, int value) 
{
    if ( shader == nullptr ) {
        std::cout << "Attempt to read nullptr instead of shader" << std::endl;
        exit(1);
    }

    GLint location = get_shader_uniform_location(shader, uniform_name);
    glUniform1i(location, value);
}


void set_shader_uniform_1f(Shader *shader, std::string uniform_name, float value) 
{
    if ( shader == nullptr ) {
        std::cout << "Attempt to read nullptr instead of shader" << std::endl;
        exit(1);
    }

    GLint location = get_shader_uniform_location(shader, uniform_name);
    glUniform1f(location, value);
}


void set_shader_uniform_matrix4fv(Shader *shader, std::string uniform_name, int count, bool transpose, glm::mat4 *matrices)
{
    if ( shader == nullptr ) {
        std::cout << "Attempt to read nullptr instead of shader" << std::endl;
        exit(1);
    }

    GLint location = get_shader_uniform_location(shader, uniform_name);
    glUniformMatrix4fv(location, count, transpose, (GLfloat*)matrices);
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glewExperimental = GL_TRUE;
    glewInit();
}


void init_texture(Texture *texture, std::string image_name, std::string image_path)
{
    if (texture == nullptr) {
        std::cout << "cannot initialize texture when it is null" << std::endl;
        exit(1);
    }

    static int texture_ids = 0;

    memset(texture, 0, sizeof(Texture));

    texture->id = ++texture_ids;

    SDL_Surface *med_surface = IMG_Load(image_path.c_str());

    if ( !med_surface ) {
        std::cout << "Could not load image named: " << image_path << std::endl;
        exit(1);
    }

    texture->image_size = glm::vec2(med_surface->w, med_surface->h);

    glGenTextures(1, &texture->glid);
    glBindTexture(GL_TEXTURE_2D, texture->glid);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, med_surface->w, med_surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)med_surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_FreeSurface(med_surface);
}


void init_entity(Entity *entity, Scene *scene, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    if (entity == nullptr) {
        std::cout << "cannot initialize entity when it is null" << std::endl;
        exit(1);
    }

    if (scene == nullptr) {
        std::cout << "initializing entity with no scene. Probably a child." << std::endl;
        // exit(1);
    }

    static int entity_ids = 0;

    memset(entity, 0, sizeof(Entity));

    entity->id = ++entity_ids;
    entity->scene = scene;
    entity->sprite = nullptr;
    entity->children = new std::list<Entity *>();

    entity->acceleration = glm::vec3(0.f);
    entity->velocity = glm::vec3(0.f);
    entity->position = position;
    entity->scale = scale;
    entity->rotation = rotation;
}


void init_sprite(Sprite *sprite, Entity *parent, Texture *texture, glm::vec2 frame_offset, glm::vec2 frame_size) 
{
    if (sprite == nullptr) { 
        std::cout << "cannot initialize sprite when it is null" << std::endl;
        exit(1);
    }

    if (parent == nullptr) {
        std::cout << "cannot set sprite parent to null entity" << std::endl;
        exit(1);
    }

    if (texture == nullptr) {
        std::cout << "sprite cannot use null texture" << std::endl;
        exit(1);
    }

    static int sprite_ids = 0;

    memset(sprite, 0, sizeof(Sprite));

    sprite->id = ++sprite_ids;
    sprite->parent = parent;
    sprite->texture = texture;
    sprite->texture_frame_offset = frame_offset;

    if (frame_size == glm::vec2(0.f, 0.f)) {
        sprite->texture_frame_size = sprite->texture->image_size;
    }
    else {
        sprite->texture_frame_size = frame_size;
    }
}


void add_entity(Entity *entity, Entity *parent)
{
    if (entity == nullptr) { 
        std::cout << "cannot set entity property when its null" << std::endl;
        exit(1);
    }

    if (parent == nullptr) {
        std::cout << "cannot set parent to null entity" << std::endl;
        exit(1);
    }

    entity->parent = parent;
    parent->children->push_back(entity);
}


void draw_entity(Entity *entity)
{
    if (entity == nullptr) {
        std::cout << "Cannot draw null entity" << std::endl;
        exit(1);
    }

    Shader *sprite_shader = SHADERS["sprite"];

    if (!sprite_shader){
        std::cout << "Coudl not locate sprite shader... it might not be initialized" << std::endl;
        exit(1);
    }

    use_shader(sprite_shader);
    set_shader_uniform_1i(sprite_shader, "sprite_texture", 0);

    // draw children first
    for(auto iter = entity->children->begin(); iter != entity->children->end(); iter++) {
        draw_entity(*iter);
    }

    glBindTexture(GL_TEXTURE_2D, entity->sprite->texture->glid);
    glActiveTexture(GL_TEXTURE0);

    glm::mat4 projection = glm::ortho(0.f, (float)SCREEN_WIDTH, 0.f, (float)SCREEN_HEIGHT, 0.0f, 100.f);
    glm::mat4 view = glm::mat4(1.f);
    glm::mat4 model = glm::mat4(1.f);


    glm::vec3 translate_offset = glm::vec3(0.f);
    if (entity->parent) {
        translate_offset = entity->parent->position;
    }

    model = glm::translate(model, translate_offset + entity->position);
    model = glm::scale(model, entity->scale);

    model = glm::rotate(model, glm::radians(entity->rotation.x), glm::vec3(1.f, 0.f, 0.f));
    model = glm::rotate(model, glm::radians(entity->rotation.y), glm::vec3(0.f, 1.f, 0.f));
    model = glm::rotate(model, glm::radians(entity->rotation.z), glm::vec3(0.f, 0.f, 1.f));

    set_shader_uniform_matrix4fv(sprite_shader, "projection", 1, false, &projection);
    set_shader_uniform_matrix4fv(sprite_shader, "view", 1, false, &view);
    set_shader_uniform_matrix4fv(sprite_shader, "model", 1, false, &model);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);

}