#include "Menu.h"
#include "Utility.h"
#include <vector>

GLuint font_id;

Menu::~Menu()
{
}

void Menu::initialise()
{
    //m_game_state.player = new Entity();
    font_id = Utility::load_texture("assets/font1.png");
    m_game_state.next_scene_id = 1;
}

void Menu::update(float delta_time)
{
}

void Menu::render(ShaderProgram* program)
{
    
    Utility::draw_text(program, font_id, "PRESS ENTER TO START", 0.5f, -0.3f,
        glm::vec3(-2.5f, 0.0f, 0.0f));
}
