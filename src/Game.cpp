#include "Game.h"
#include "Globals.h"
#include "ResourceManager.h"
#include <stdio.h>

Game::Game()
{
    state = GameState::MAIN_MENU;
    scene = nullptr;
    img_menu_up = nullptr;
    img_menu_down = nullptr;

    target = {};
    src = {};
    dst = {};
}
Game::~Game()
{
    if (scene != nullptr)
    {
        scene->Release();
        delete scene;
        scene = nullptr;
    }
}
AppStatus Game::Initialise(float scale)
{
    float w, h;
    w = WINDOW_WIDTH * scale;
    h = WINDOW_HEIGHT * scale;

    //Initialise window
    InitWindow((int)w, (int)h, "Pengo");

    //Render texture initialisation, used to hold the rendering result so we can easily resize it
    target = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (target.id == 0)
    {
        LOG("Failed to create render texture");
        return AppStatus::ERROR;
    }
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    src = { 0, 0, WINDOW_WIDTH, -WINDOW_HEIGHT };
    dst = { 0, 0, w, h };

    //Load resources
    if (LoadResources() != AppStatus::OK)
    {
        LOG("Failed to load resources");
        return AppStatus::ERROR;
    }

    int framesCounter = 0;
    int framesSpeed = 8;            // Number of spritesheet frames shown by second
    SetTargetFPS(60);
    //Set the target frame rate for the application
    SetTargetFPS(60);
    //Disable the escape key to quit functionality
    SetExitKey(0);
  

    return AppStatus::OK;
}
AppStatus Game::LoadResources()
{
    ResourceManager& data = ResourceManager::Instance();
    
    if (data.LoadTexture(Resource::IMG_MENU_UP, "images/pantallas/1.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }

    if (data.LoadTexture(Resource::IMG_MENU_DOWN, "images/pantallas/down.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    sheet = LoadTexture("resources/spritesheet.png");
    Vector2 position = { 350.0f, 280.0f };
    Rectangle frameRec = { 0.0f, 0.0f, (float)sheet.width / 16, (float)sheet.height };

    img_menu_up = data.GetTexture(Resource::IMG_MENU_UP);
    img_menu_down = data.GetTexture(Resource::IMG_MENU_DOWN);

    
    return AppStatus::OK;
}
AppStatus Game::BeginPlay()
{
    scene = new Scene();
    if (scene == nullptr)
    {
        LOG("Failed to allocate memory for Scene");
        return AppStatus::ERROR;
    }
    if (scene->Init() != AppStatus::OK)
    {
        LOG("Failed to initialise Scene");
        return AppStatus::ERROR;
    }

    return AppStatus::OK;
}
void Game::FinishPlay()
{
    scene->Release();
    delete scene;
    scene = nullptr;
}
AppStatus Game::Update()
{
    //Check if user attempts to close the window, either by clicking the close button or by pressing Alt+F4
    if(WindowShouldClose()) return AppStatus::QUIT;

    static float frameTime = 0.0f;
    static int frameIndex = 0;
    const int totalFrames = 16;
    const float frameSpeed = 2;

    switch (state)
    {
    case GameState::MAIN_MENU: {


            Vector2 position = { 350.0f, 280.0f };

            frameTime += GetFrameTime();
                if (frameTime >= frameSpeed) {
                    frameTime = 0.0f;
                    frameIndex = (frameIndex + 1) % totalFrames;
                }

                // Actualizar frameRec con el frame actual
                Rectangle frameRec = {
                    frameIndex * (float)sheet.width / totalFrames,
                    0.0f,
                    (float)sheet.width / totalFrames,
                    (float)sheet.height
                };

                // Dibujar
                BeginDrawing();

                DrawTexture(sheet, 15, 40, WHITE);
                DrawRectangleLines(15, 40, sheet.width, sheet.height, LIME);
                DrawTextureRec(sheet, frameRec, position, WHITE); // Dibujar solo el frame actual
                DrawRectangleLines(15 + (int)frameRec.x, 40 + (int)frameRec.y, (int)frameRec.width, (int)frameRec.height, RED);

                EndDrawing();

                if (IsKeyPressed(KEY_ESCAPE)) return AppStatus::QUIT;
                if (IsKeyPressed(KEY_SPACE))
                {
                    if (BeginPlay() != AppStatus::OK) return AppStatus::ERROR;
                    state = GameState::PLAYING;
                }
                break;
        }
    case GameState::PLAYING:{
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    FinishPlay();
                    state = GameState::MAIN_MENU;
                }
                else
                {
                    //Game logic
                    scene->Update();
                }
                break;
        }
        return AppStatus::OK;
    }
}
void Game::Render()
{
    //Draw everything in the render texture, note this will not be rendered on screen, yet
    BeginTextureMode(target);
    ClearBackground(BLACK);
    
    switch (state)
    {
        case GameState::MAIN_MENU:
            DrawTexture(*img_menu_up, 0, 0, WHITE);
            DrawTexture(*img_menu_down, 0, 156, WHITE);
            break;

        case GameState::PLAYING:
            scene->Render();
            break;
    }
    
    EndTextureMode();

    //Draw render texture to screen, properly scaled
    BeginDrawing();
    DrawTexturePro(target.texture, src, dst, { 0, 0 }, 0.0f, WHITE);
    EndDrawing();
}
void Game::Cleanup()
{
    UnloadResources();
    CloseWindow();
}
void Game::UnloadResources()
{
    ResourceManager& data = ResourceManager::Instance();
    data.ReleaseTexture(Resource::IMG_MENU_UP);
    data.ReleaseTexture(Resource::IMG_MENU_DOWN);

    UnloadRenderTexture(target);
}