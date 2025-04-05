#include "LevelA.h"
#include "Utility.h"
#include <vector>

#define LEVEL_HEIGHT 10
#define ENEMY_AMOUNT 1

unsigned int LEVELA_DATA[] =
{
    15, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
    15, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
    15, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
    15, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
    15, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
    15, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
    15, 45, 45, 45, 1 , 1 , 45, 45, 45, 45, 45, 45, 45, 45,
    15, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
    27, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 44,
    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
};


LevelA::~LevelA()
{
    delete m_game_state.player;
    delete m_game_state.map;
}

void LevelA::initialise()
{
    m_game_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id,
                               1.0f, 12, 6);
    
    std::vector<std::vector<int>> animations =
    {
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },  // Rest
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },     // Charging
    };

    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);
    
    std::vector<GLuint> texture_ids =
    {
        Utility::load_texture("assets/Idle (32x32).png"),
        Utility::load_texture("assets/Run (32x32).png")
    };
    
    m_game_state.player = new Entity(
        texture_ids,               // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        animations,                // animation index sets
        0.0f,                      // animation time
        10,                         // animation frame amount
        0,                         // current animation index
        11,                         // animation column amount
        1,                         // animation row amount
        0.7f,                      // width
        1.0f,                      // height
        PLAYER,                    // entity type
        REST                       // player state
    );

    m_game_state.player->set_position(glm::vec3(4.0f, 0.0f, 0.0f));
    m_game_state.player->set_scale(glm::vec3(1.0f, 1.0f, 0.0f));

    std::vector<std::vector<int>> slime_animations =
    {
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },  // WALKING
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },  // IDLE
        { 0, 1, 2, 3, 4, 5 },     // HIT
    };

    std::vector<GLuint> slime_texture_ids =
    {
        Utility::load_texture("assets/slime/Idle-Run.png"),
        Utility::load_texture("assets/slime/Idle-Run.png"),
        Utility::load_texture("assets/slime/Hit.png"),
    };

    m_game_state.enemy[0] = new Entity(
        slime_texture_ids,               // texture id
        0.3f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        slime_animations,                // animation index sets
        0.0f,                      // animation time
        10,                         // animation frame amount
        0,                         // current animation index
        10,                         // animation column amount
        1,                         // animation row amount
        0.7f,                      // width
        0.7f,                      // height
        ENEMY,                     // entity type
        SLIMEGUARD,                // ai type
        IDLE                       // ai state
    );
    m_game_state.enemy[0]->set_position(glm::vec3(8.0f, 0.0f, 0.0f));
}

void LevelA::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemy[0], 1,
                                m_game_state.map);

    for (size_t i = 0; i < ENEMY_AMOUNT; i++)
    {
        m_game_state.enemy[i]->update(delta_time, m_game_state.player, nullptr, 0,
            m_game_state.map);
    }
    
    if (m_game_state.player->get_position().y < -10.0f) m_game_state.next_scene_id = 1;
}

void LevelA::render(ShaderProgram *program)
{
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    for (size_t i = 0; i < ENEMY_AMOUNT; i++)
    {
        m_game_state.enemy[i]->render(program);
    }
}
