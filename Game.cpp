#include "Game.h"
#include <iostream>

Game::Game() {
    std::cout << "Iniciando el juego..." << std::endl;
}

Game::~Game() {
    std::cout << "Saliendo del juego..." << std::endl;
}

void Game::run() {
    while (true) { // Bucle del juego
        processInput();
        update();
        render();
    }
}

void Game::processInput() {
    // Procesar eventos (teclado, mouse, etc.)
}

void Game::update() {
    // Lógica del juego
}

void Game::render() {
    // Dibujar en pantalla
}
