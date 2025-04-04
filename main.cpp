#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f
#define MAX_RGB 255

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

//#include <SDL_mixer.h>
#include "SDL_mixer.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "Menu.h"
#include "Effects.h"

// ����� CONSTANTS ����� //
constexpr int WINDOW_WIDTH = 1280,
WINDOW_HEIGHT = 960;

constexpr float SCALE_VIEW_MATRIX = 0.5;

constexpr float BG_RED = 27.0f / MAX_RGB,
BG_BLUE = 43.0f / MAX_RGB,
BG_GREEN = 52.0f / MAX_RGB,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ����� GLOBAL VARIABLES ����� //
Scene* g_curr_scene = nullptr;
Menu* g_menu = nullptr;
LevelA* g_levelA = nullptr;
LevelB* g_levelB = nullptr;
LevelC* g_levelC = nullptr;

Effects* g_effects = nullptr;
Scene* g_levels[5];

SDL_Window* g_display_window = nullptr;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

bool g_is_colliding_bottom = false;
int curr_scene_index = 0;

AppStatus g_app_status = RUNNING;

void switch_to_scene(Scene* scene);
void initialise();
void process_input();
void update();
void render();
void shutdown();

void switch_to_scene(Scene* scene)
{
    g_curr_scene = scene;
    g_curr_scene->initialise();
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Hello, Special Effects!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-3.75f, 3.75f, -2.8125f, 2.8125f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    g_menu = new Menu();
    g_levels[0] = g_menu;

    g_levelA = new LevelA();
    g_levels[1] = g_levelA;

    g_levelB = new LevelB();
    g_levels[2] = g_levelB;

    g_levelC = new LevelC();
    g_levels[3] = g_levelC;


    switch_to_scene(g_levels[0]);

    g_effects = new Effects(g_projection_matrix, g_view_matrix);
}

void process_input()
{
    SDL_Event event;
    if (curr_scene_index == 0) {
        while (SDL_PollEvent(&event))
        {
            switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_q: g_app_status = TERMINATED;     break;
                case SDLK_RETURN:
                    curr_scene_index += 1;
                    switch_to_scene(g_levels[curr_scene_index]);
                    break;

                default:
                    break;
                }

            default:
                break;
            }
        }
        return;
    }
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    g_curr_scene->get_state().player->set_movement(glm::vec3(0.0f));
    // clear effects at start
    g_effects->start(NONE);



    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q: g_app_status = TERMINATED;     break;
            case SDLK_RETURN:
                curr_scene_index += 1;
                switch_to_scene(g_levels[curr_scene_index]);
                break;
            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(nullptr);

    if (key_state[SDL_SCANCODE_D]) {
        g_curr_scene->get_state().player->move_right();
    }
    else if (key_state[SDL_SCANCODE_A]){
        g_curr_scene->get_state().player->move_left();
    }

    if (key_state[SDL_SCANCODE_SPACE] || key_state[SDL_SCANCODE_W])
    {
        if (g_curr_scene->get_state().player->get_collided_bottom()) {
            g_curr_scene->get_state().player->jump();
        }
    }

    if (key_state[SDL_SCANCODE_C])
    {
        g_curr_scene->get_state().player->set_player_state(CHARGING);
        // start effect
        g_effects->start(TREMBLE);
    }
    else
    {
        g_curr_scene->get_state().player->set_player_state(REST);
    }


    if (glm::length(g_curr_scene->get_state().player->get_movement()) > 1.0f)
        g_curr_scene->get_state().player->normalise_movement();
}

void update()
{
    if (curr_scene_index == 0) { return; }
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        g_curr_scene->update(FIXED_TIMESTEP);
        g_effects->update(FIXED_TIMESTEP);

        g_is_colliding_bottom = g_curr_scene->get_state().player->get_collided_bottom();

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    g_view_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::translate(g_view_matrix,
        glm::vec3(SCALE_VIEW_MATRIX*(-g_curr_scene->get_state().player->get_position().x),
            SCALE_VIEW_MATRIX * (-g_curr_scene->get_state().player->get_position().y),
            0)
    );

    // translate matrix
    g_view_matrix = glm::translate(g_view_matrix, g_effects->get_tremble_offset());
    g_view_matrix = glm::scale(g_view_matrix, glm::vec3(SCALE_VIEW_MATRIX, SCALE_VIEW_MATRIX, 0.0f));
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);

    // ����� RENDERING THE SCENE (i.e. map, character, enemies...) ����� //
    g_curr_scene->render(&g_shader_program);

    if (g_curr_scene != 0) {
        g_effects->render();
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    delete g_levelA;
    delete g_effects;
}

// ����� DRIVER GAME LOOP ����� //
int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();

        //        if (g_curr_scene->get_state().next_scene_id >= 0)
        //            switch_to_scene(g_levels[g_curr_scene->get_state().next_scene_id]);

        render();
    }

    shutdown();
    return 0;
}