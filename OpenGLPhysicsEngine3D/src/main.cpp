#include "Application/PhysicsApplication.h"

int main()
{
    PhysicsApplication app;

    if (app.Init(1280, 768, "2D Physics Engine")) {
        app.Run();
    }

    return 0;
}