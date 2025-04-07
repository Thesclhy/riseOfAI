#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"
#include "glm/glm.hpp"
#include "ShaderProgram.h"
enum EntityType { CHECKPOINT, PLAYER, ENEMY};
enum PlayerState { REST, RUN };
enum AIType     { RUNNER, SLIMEGUARD, FLYER};
enum AIState    { WALKING, IDLE, ATTACKING, ANGTY_RUN };


enum AnimationDirection { LEFT, RIGHT, UP, DOWN };

class Entity
{
private:
    bool m_is_active = true;
    
    std::vector<std::vector<int>> m_animations; // 4x4 array for walking animations

    
    EntityType m_entity_type;
    AIType     m_ai_type;
    AIState    m_ai_state;
    PlayerState m_player_state;
    // ————— TRANSFORMATIONS ————— //
    glm::vec3 m_movement;
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    glm::mat4 m_model_matrix;

    float     m_speed,
              m_jumping_power;
    
    bool m_is_jumping;

    // ————— TEXTURES ————— //
    std::vector<GLuint> m_texture_ids;

    // ————— ANIMATION ————— //
    int m_animation_cols;
    int m_animation_frames,
        m_animation_index,
        m_animation_rows;


    int* m_animation_indices = nullptr;
    float m_animation_time = 0.0f;

    float m_width = 1.0f,
          m_height = 1.0f;
    // ————— COLLISIONS ————— //
    bool m_collided_top    = false;
    bool m_collided_bottom = false;
    bool m_collided_left   = false;
    bool m_collided_right  = false;

    //
    bool hitted = false;
    int moving_dir_factor = 1;
    float cumulate_position = 0.0f;
    glm::vec3 MAX_BOUND_MOVING = glm::vec3(3.0f, 0.0f, 0.0f);
    glm::vec3 INIT_POSITION;

public:
    // ————— STATIC VARIABLES ————— //
    static constexpr int SECONDS_PER_FRAME = 8;

    // ————— METHODS ————— //
    Entity();
    Entity(std::vector<GLuint> texture_ids, float speed, glm::vec3 acceleration,
           float jump_power, std::vector<std::vector<int>> animations,
           float animation_time, int animation_frames, int animation_index,
           int animation_cols, int animation_rows, float width, float height,
           EntityType EntityType, PlayerState player_state);
    Entity(std::vector<GLuint> texture_ids, float speed, glm::vec3 acceleration,
        float jump_power, std::vector<std::vector<int>> animations,
        float animation_time, int animation_frames, int animation_index,
        int animation_cols, int animation_rows, float width, float height,
        EntityType EntityType, AIType ai_type, AIState ai_state);
    Entity(std::vector<GLuint> texture_ids, float speed, glm::vec3 acceleration, std::vector<std::vector<int>> animations,
        float animation_time, int animation_frames, int animation_index,
        int animation_cols, int animation_rows, float width, float height, EntityType EntityType);

    ~Entity();

    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    bool const check_collision(Entity* other) const;
    
    void const check_collision_y(Entity** collidable_entities, int collidable_entity_count);
    void const check_collision_x(Entity** collidable_entities, int collidable_entity_count);
    
    // Overloading our methods to check for only the map
    void const check_collision_y(Map *map);
    void const check_collision_x(Map *map);
    
    void update(float delta_time, Entity *player, Entity **collidable_entities, int collidable_entity_count, Map *map);
    void render(ShaderProgram* program);

    void ai_activate(Entity *player);
    void ai_walk();
    void ai_flying_around();
    void ai_guard(Entity *player);
    
    void normalise_movement() { m_movement = glm::normalize(m_movement); }
    
    void const jump() { m_is_jumping = true; }

    // ————— GETTERS ————— //
    //int const get_lives() { return lives; }
    bool const get_hitted() { return hitted; };
    bool const isJumping() { return m_is_jumping; }
    EntityType const get_entity_type()    const { return m_entity_type;   };
    AIType     const get_ai_type()        const { return m_ai_type;       };
    AIState    const get_ai_state()       const { return m_ai_state;      };
    float const get_jumping_power() const { return m_jumping_power; }
    glm::vec3 const get_position()     const { return m_position; }
    glm::vec3 const get_velocity()     const { return m_velocity; }
    glm::vec3 const get_acceleration() const { return m_acceleration; }
    glm::vec3 const get_movement()     const { return m_movement; }
    glm::vec3 const get_scale()        const { return m_scale; }
    std::vector<GLuint> const get_texture_ids()   const { return m_texture_ids; }
    float     const get_speed()        const { return m_speed; }
    bool      const get_collided_top() const { return m_collided_top; }
    bool      const get_collided_bottom() const { return m_collided_bottom; }
    bool      const get_collided_right() const { return m_collided_right; }
    bool      const get_collided_left() const { return m_collided_left; }
    PlayerState const get_player_state() const { return m_player_state; }
    
    void activate()   { m_is_active = true;  };
    void deactivate() { m_is_active = false; };
    // ————— SETTERS ————— //
    void const changeAnimationDirection() { m_scale.x *= -1; };
    //void const set_lives(int new_lives) { lives = new_lives; }
    //int const decreaLives() { return --lives; }
    //int const increaLives() { return ++lives; }
    void const set_hitted(bool newHitted) { hitted = newHitted; };
    void const set_entity_type(EntityType new_entity_type)  { m_entity_type = new_entity_type;};
    void const set_ai_type(AIType new_ai_type){ m_ai_type = new_ai_type;};
    void const set_ai_state(AIState new_state){ m_ai_state = new_state;};
    void const set_position(glm::vec3 new_position) { m_position = new_position; }
    void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; }
    void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; }
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; }
    void const set_scale(glm::vec3 new_scale) { m_scale = new_scale; }
    void const set_texture_id(std::vector<GLuint> new_texture_ids)
        { m_texture_ids = new_texture_ids; }
    void const set_speed(float new_speed) { m_speed = new_speed; }
    void const set_animation_cols(int new_cols) { m_animation_cols = new_cols; }
    void const set_animation_rows(int new_rows) { m_animation_rows = new_rows; }
    void const set_animation_frames(int new_frames) { m_animation_frames = new_frames; }
    void const set_animation_index(int new_index) { m_animation_index = new_index; }
    void const set_animation_time(float new_time) { m_animation_time = new_time; }
    void const set_jumping_power(float new_jumping_power) { m_jumping_power = new_jumping_power;}
    void const set_width(float new_width) {m_width = new_width; }
    void const set_height(float new_height) {m_height = new_height; }
    void const set_init_position(glm::vec3 int_pos) { INIT_POSITION = int_pos; }
    void const set_animations(std::vector<std::vector<int>> new_animations)
        { m_animations = new_animations; }
    void const set_player_state(PlayerState new_player_state)
    {
        m_player_state = new_player_state;
        
        if (!m_animations.empty() && m_animations.size() > m_player_state)
        {
            m_animation_indices = m_animations[m_player_state].data();
        }
        else
        {
            m_animation_indices = nullptr;
            return;
        }
        
        m_animation_cols = (int) m_animations[m_player_state].size();
    }
    void const set_enemy_state(AIState new_ai_state)
    {
        m_ai_state = new_ai_state;

        if (!m_animations.empty() && m_animations.size() > m_player_state)
        {
            m_animation_indices = m_animations[m_ai_state].data();
        }
        else
        {
            m_animation_indices = nullptr;
            return;
        }

        m_animation_cols = (int)m_animations[m_ai_state].size();
    }
    
    //moving
    void const move_right() {
        if (m_scale.x < 0) {
            changeAnimationDirection();
        }
        m_movement.x = 1.0f;
    }
    void const move_left() {
        if (m_scale.x > 0) {
            changeAnimationDirection();
        }
        m_movement.x = -1.0f;
    }
    /*void const move_right() {
        m_movement.x += 1.0f;
    }*/

};

#endif // ENTITY_H
