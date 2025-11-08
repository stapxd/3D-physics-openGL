#include "Application/PhysicsApplication.h"

int main()
{
    PhysicsApplication app;

    if (app.Init(1280, 768, "3D Physics Engine")) {
        app.Run();
    }

    return 0;
}