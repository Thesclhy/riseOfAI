#include "LevelB.h"
#include "Utility.h"
#include <vector>

#define LEVEL_HEIGHT 14
#define ENEMY_AMOUNT 4


unsigned int LEVELB_DATA[] =
{
    3 , 4 , 4 , 4 , 4 , 4 , 4 , 4 , 4 , 4 , 4 , 4 , 4 , 5 ,
    15, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 17,
    15, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 17,
    15, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 17,
    15, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 47, 47, 47, 17,
    15, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 17,
    15, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 17,
    15, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 17,
    15, 47, 47, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 29,
    15, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
    15, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
    15, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
    15, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
    27, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
};

LevelB::~LevelB()
{
    delete m_game_state.player;
    delete m_game_state.map;
    delete[] m_game_state.enemy;
    //for (int i = 0; i < ENEMY_AMOUNT; ++i) {
    //    delete[] m_game_state.enemy[i];      // delete each Entity*
    //}
    delete m_game_state.checkpoint;
}

void LevelB::initialise()
{
    m_game_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("assets/pic/tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELB_DATA, map_texture_id,
        1.0f, 12, 6);

    std::vector<std::vector<int>> animations =
    {
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },  // Rest
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },     // Charging
    };

    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);

    std::vector<GLuint> texture_ids =
    {
        Utility::load_texture("assets/pic/player/Idle (32x32).png"),
        Utility::load_texture("assets/pic/player/Run (32x32).png")
    };

    m_game_state.enemy = new Entity * [ENEMY_AMOUNT];
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

    m_game_state.player->set_position(glm::vec3(1.0f, -2.0f, 0.0f));
    m_game_state.player->set_scale(glm::vec3(1.0f, 1.0f, 0.0f));


    // ENEMY
    std::vector<std::vector<int>> slime_animations =
    {
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },  // WALKING
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },  // IDLE
        { 0, 1, 2, 3, 4, 5 },     // HIT
    };

    std::vector<GLuint> slime_texture_ids =
    {
        Utility::load_texture("assets/pic/slime/Idle-Run.png"),
        Utility::load_texture("assets/pic/slime/Idle-Run.png"),
        Utility::load_texture("assets/pic/slime/Hit.png"),
    };

    for (size_t i = 0; i < ENEMY_AMOUNT-1; i++)
    {
        m_game_state.enemy[i] = new Entity(
            slime_texture_ids,               // texture id
            1.0f,                      // speed
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
    }
    m_game_state.enemy[0]->set_position(glm::vec3(6.0f, -3.0f, 0.0f));
    m_game_state.enemy[1]->set_position(glm::vec3(8.0f, -6.0f, 0.0f));
    m_game_state.enemy[2]->set_position(glm::vec3(4.0f, -9.0f, 0.0f));
    

    std::vector<std::vector<int>> checkpoint_animations =
    {
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }  // WALKING
    };

    std::vector<GLuint> checkpoint_texture_ids =
    {
        Utility::load_texture("assets/pic/Checkpoint (Flag Idle)(64x64).png")
    };

    std::vector<std::vector<int>> pig_animations =
    {
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },  // WALKING
        { 0, 1, 2, 3, 4, 5, 6, 7, 8},  // IDLE
    };

    std::vector<GLuint> pig_texture_ids =
    {
        Utility::load_texture("assets/pic/angry_pig/Run (36x30).png"),
        Utility::load_texture("assets/pic/angry_pig/Idle (36x30).png"),
    };

    m_game_state.enemy[3] = new Entity(
        pig_texture_ids,               // texture id
        3.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        pig_animations,                // animation index sets
        0.0f,                      // animation time
        12,                         // animation frame amount
        0,                         // current animation index
        12,                         // animation column amount
        1,                         // animation row amount
        0.7f,                      // width
        0.7f,                      // height
        ENEMY,                     // entity type
        RUNNER,                    // ai type
        WALKING                       // ai state
    );
    m_game_state.enemy[3]->set_position(glm::vec3(8.0f, -3.0f, 0.0f));



    //checkpoint
    m_game_state.checkpoint = new Entity(
        checkpoint_texture_ids,               // texture id
        0.3f,                      // speed
        acceleration,              // acceleration
        checkpoint_animations,                // animation index sets
        0.0f,                      // animation time
        10,                         // animation frame amount
        0,                         // current animation index
        10,                         // animation column amount
        1,                         // animation row amount
        0.7f,                      // width
        1.5f,                      // height
        CHECKPOINT                     // entity type       
    );
    m_game_state.checkpoint->set_player_state(REST);
    m_game_state.checkpoint->set_position(glm::vec3(12.0f, -8.0f, 0.0f));
    m_game_state.checkpoint->set_scale(glm::vec3(1.5f, 1.5f, 0.0f));

    
}

void LevelB::update(float delta_time)
{
    if (m_game_state.checkpoint->get_hitted()) {
        m_game_state.next_scene_id = 3;
    }
   
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemy, ENEMY_AMOUNT,
        m_game_state.map);

    for (size_t i = 0; i < ENEMY_AMOUNT; i++)
    {      
        m_game_state.enemy[i]->update(delta_time, m_game_state.player, nullptr, 0,
            m_game_state.map);
    }

    m_game_state.checkpoint->update(delta_time, m_game_state.checkpoint, &m_game_state.player, 1,
        m_game_state.map);

    //if (m_game_state.player->get_position().y < -10.0f) m_game_state.next_scene_id = 0;
}

void LevelB::render(ShaderProgram* program)
{
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    m_game_state.checkpoint->render(program);
    for (size_t i = 0; i < ENEMY_AMOUNT; i++)
    {
        m_game_state.enemy[i]->render(program);
    }
}
