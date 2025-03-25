#include <SDL2/SDL.h>
#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 600;

// Dados do gráfico
int valores[] = {100, 200, 150, 300, 250};
const int numBarras = sizeof(valores) / sizeof(valores[0]);

void drawGraph(SDL_Renderer* renderer) {
    int barWidth = WIDTH / numBarras;

    for (int i = 0; i < numBarras; i++) {
        SDL_Rect rect = {i * barWidth, HEIGHT - valores[i], barWidth - 10, valores[i]};
        SDL_SetRenderDrawColor(renderer, 0, 191, 255, 255); // Azul claro
        SDL_RenderFillRect(renderer, &rect);
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Erro ao inicializar SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Gráfico em C++",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        drawGraph(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
