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
constexpr int CD_QUAL_FREQ = 44100,  // compact disk (CD) quality frequency
AUDIO_CHAN_AMT = 2,
AUDIO_BUFF_SIZE = 4096;

constexpr int PLAY_ONCE = 0,
NEXT_CHNL = -1,  // next available channel
MUTE_VOL = 0,
MILS_IN_SEC = 1000,
ALL_SFX_CHN = -1;

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
GLuint g_font_texture_id;
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

int g_lives = 3;

bool g_is_colliding_bottom = false;
int curr_scene_index = 0;

AppStatus g_app_status = RUNNING;

constexpr char BGM_FILEPATH[] = "assets/music/bgm.mp3";
constexpr char BOUNCE_FILEPATH[] = "assets/music/bounce_back.wav";
constexpr char PAIN_FILEPATH[] = "assets/music/pain.wav";
constexpr int    LOOP_FOREVER = -1;  // -1 means loop forever in Mix_PlayMusic; 0 means play once and loop zero times
Mix_Music* g_music;
Mix_Chunk* g_bouncing_sfx;
Mix_Chunk* g_pain_sfx;

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

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    // Start Audio
    Mix_OpenAudio(
        CD_QUAL_FREQ,        // the frequency to playback audio at (in Hz)
        MIX_DEFAULT_FORMAT,  // audio format
        AUDIO_CHAN_AMT,      // number of channels (1 is mono, 2 is stereo, etc).
        AUDIO_BUFF_SIZE      // audio buffer size in sample FRAMES (total samples divided by channel count)
    );

    g_bouncing_sfx = Mix_LoadWAV(BOUNCE_FILEPATH);
    g_pain_sfx = Mix_LoadWAV(PAIN_FILEPATH);

    // Similar to our custom function load_texture
    g_music = Mix_LoadMUS(BGM_FILEPATH);

    // This will schedule the music object to begin mixing for playback.
    // The first parameter is the pointer to the mp3 we loaded 
    // and second parameter is the number of times to loop.
    Mix_PlayMusic(g_music, LOOP_FOREVER);

    // Set the music to half volume
    // MIX_MAX_VOLUME is a pre-defined constant
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);

    g_font_texture_id = Utility::load_texture("assets/font1.png");

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
    else {
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
            /*if (g_curr_scene->get_state().player->get_scale().x < 0) {
                g_curr_scene->get_state().player->changeAnimationDirection();
            }*/
            g_curr_scene->get_state().player->move_right();
        }
        else if (key_state[SDL_SCANCODE_A]) {
            //if (!g_curr_scene->get_state().player->get_hitted())
           /* if (g_curr_scene->get_state().player->get_scale().x > 0) {
                g_curr_scene->get_state().player->changeAnimationDirection();
            }*/
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
            g_curr_scene->get_state().player->set_player_state(RUN);
            // start effect
            //g_effects->start(TREMBLE);
        }
        else
        {
            //g_curr_scene->get_state().player->set_player_state(REST);
        }


        if (glm::length(g_curr_scene->get_state().player->get_movement()) > 1.0f)
            g_curr_scene->get_state().player->normalise_movement();
    }  
}

void update()
{
    if (curr_scene_index == 0) { return; }
    if (g_curr_scene->get_state().next_scene_id != -1) {
        switch_to_scene(g_levels[g_curr_scene->get_state().next_scene_id]);
        curr_scene_index = g_curr_scene->get_state().next_scene_id;
    }
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

    if (g_curr_scene->get_state().player->get_hitted())
    {
        Mix_PlayChannel(
            NEXT_CHNL,       // using the first channel that is not currently in use...
            g_pain_sfx,  // ...play this chunk of audio...
            PLAY_ONCE        // ...once.
        );
        Mix_Volume(
            ALL_SFX_CHN,        // Set all channels...
            MIX_MAX_VOLUME / 2  // ...to half volume.
        );
        --g_lives;
        g_effects->start(TREMBLE);
        g_curr_scene->get_state().player->set_hitted(false);
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
    
    if (curr_scene_index != 0) {
        glm::vec3 play_postion = g_curr_scene->get_state().player->get_position();
        Utility::draw_text(&g_shader_program, g_font_texture_id, "HP:" + std::to_string(g_lives), 0.5f, -0.25f,
            glm::vec3(play_postion.x - 0.3f, 1.0f + play_postion.y, 0.0f));
    }
    

    if (g_curr_scene != 0) {
        g_effects->render();
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    delete g_levelA;
    delete g_levelB;
    delete g_levelC;
    delete g_effects;
    Mix_FreeChunk(g_pain_sfx);
    Mix_FreeChunk(g_bouncing_sfx);
    Mix_FreeMusic(g_music);

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