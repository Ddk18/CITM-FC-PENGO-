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

    frameIndex = 0;  // Inicializar animación
    frameTime = 0.0f;
    frameRec = { 0.0f, 0.0f, 0.0f, 0.0f };
    sheet = {};
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

    sheet = LoadTexture("images/pantallas/spritesheet.png");
    Vector2 position = { 350.0f, 280.0f };
    Rectangle frameRec = { 0.0f, 0.0f, (float)sheet.width / 16, (float)sheet.height };

    img_menu_up = data.GetTexture(Resource::IMG_MENU_UP);
    img_menu_down = data.GetTexture(Resource::IMG_MENU_DOWN);
    frameRec = { 0.0f, 0.0f, (float)sheet.width / 16, (float)sheet.height };

    
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
    
        if (scene != nullptr) {
            scene->Release(); // Liberar cualquier recurso que Scene haya cargado
            delete scene;     // Eliminar el objeto Scene
            scene = nullptr;  // Asegurarse de que no apunte a un lugar inválido
        }
    
    
}
AppStatus Game::Update()
{
    if (WindowShouldClose()) return AppStatus::QUIT; // El alt+F4

    const int totalFrames = 16;
    const float frameSpeed = 0.75f;  // Velocidad de la animación del menú

    switch (state)
    {
    case GameState::MAIN_MENU: {
        frameTime += GetFrameTime(); // Calcular el tiempo de animación
        if (frameTime >= frameSpeed) {
            frameTime = 0.0f;
            frameIndex = (frameIndex + 1) % totalFrames;
        }

        if (IsKeyPressed(KEY_ESCAPE)) return AppStatus::QUIT; // Salir

        if (IsKeyPressed(KEY_SPACE)) // Jugar
        {
            if (BeginPlay() != AppStatus::OK) return AppStatus::ERROR;
            state = GameState::PLAYING;
        }
        break;
    }

    case GameState::PLAYING: {
        if (IsKeyPressed(KEY_ESCAPE)) // Salir
        {
            FinishPlay();
            state = GameState::MAIN_MENU;
        }
        else
        {
            scene->Update();
        }
        break;
    }

    default:
        return AppStatus::ERROR; // Para manejar casos inesperados
    }

    return AppStatus::OK;
}

void Game::Render()
{
    //esto es por si no encuetra el archivo
    if (sheet.id == 0) {
        LOG("Error: No se pudo cargar la textura sheet");
    }

    // esto es pa que el update me encuentre donde poner las cosas
    BeginTextureMode(target);
    ClearBackground(BLACK);
    
    switch (state)
    {
    case GameState::MAIN_MENU:
    {
        DrawTexture(*img_menu_down, 0, 156, WHITE);

        // Usar `frameIndex` correctamente (actualizado en `Update()`)
        Rectangle frameRec = {
            frameIndex * 336.0f,  // 5376 px dividido en 16 frames
            0.0f,
            336.0f,
            156.0f
        };

        // Dibujar la parte superior del menú con la animación
        DrawTexture(*img_menu_up, 0, 0, WHITE);
        DrawTextureRec(sheet, frameRec, { 0, 0 }, WHITE); // Superponer la animación sobre img_menu_up

        break;
    }

    case GameState::PLAYING:
        scene->Render();
        break;
    }

    EndTextureMode();

    // Dibujar la textura renderizada en la pantalla
    BeginDrawing();
    DrawTexturePro(target.texture, src, dst, { 0, 0 }, 0.0f, WHITE);
    EndDrawing();
}

void Game::Cleanup()
{
    UnloadResources();
    CloseWindow();
}


void Game::UnloadResources() {
    ResourceManager& data = ResourceManager::Instance();

    // Liberar las texturas cargadas
    if (img_menu_up != nullptr) {
        data.ReleaseTexture(Resource::IMG_MENU_UP);
        img_menu_up = nullptr;
    }
    if (img_menu_down != nullptr) {
        data.ReleaseTexture(Resource::IMG_MENU_DOWN);
        img_menu_down = nullptr;
    }

    // Liberar la textura sheet si fue cargada
    if (sheet.id != 0) {
        UnloadTexture(sheet);
        sheet.id = 0;  // Asegurarse de que el id se establezca en 0
    }

    // Liberar la textura renderizada
    if (target.id != 0) {
        UnloadRenderTexture(target);
        target.id = 0;  // Asegurarse de que el id se establezca en 0
    }
}

