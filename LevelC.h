#include "Scene.h"

class LevelC : public Scene {
public:
    static const int LEVEL_WIDTH = 14;
    static const int LEVEL_HEIGHT = 22;

    ~LevelC();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};
