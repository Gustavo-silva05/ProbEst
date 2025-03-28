#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iomanip>

using namespace std;

const int WIDTH = 1800;
const int HEIGHT = 1000;
const int PADDING_LEFT = 60;  // Espaço para os rótulos do eixo Y
const int PADDING_BOTTOM = 50; // Espaço para os rótulos do eixo X

class Dado {
public:
    string data;
    float cotacao;

    Dado(string data, float cot) {
        this->data = data;
        this->cotacao = cot;
    }
};

// Função para ler os dados do arquivo
vector<Dado> leitura() {
    vector<Dado> dados;
    ifstream arq("arquivo.txt");

    if (!arq) {
        cerr << "Erro: Não foi possível abrir o arquivo." << endl;
        return dados;
    }

    string line;
    while (getline(arq, line)) {
        stringstream ss(line);
        string data;
        getline(ss, data, ',');
        data = data.substr(0, 5);
        string cot;
        ss >> cot;
        cot = cot.substr(0, 6);
        float c = stof(cot);
        if (!ss.fail()) {
            dados.push_back(Dado(data, c));
        }
    }
    arq.close();
    
    // Ordena os dados por data (caso seja necessário)
    sort(dados.begin(), dados.end(), [](const Dado& a, const Dado& b) {
        return a.data < b.data;
    });

    return dados;
}

// Função para renderizar texto na tela
void renderText(SDL_Renderer* renderer, TTF_Font* font, const string& text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect textRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &textRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Função para desenhar os eixos X e Y com escala
void drawAxes(SDL_Renderer* renderer, TTF_Font* font, float maxCotacao) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Desenha o eixo X (abscissa)
    SDL_RenderDrawLine(renderer, PADDING_LEFT, HEIGHT - PADDING_BOTTOM, WIDTH, HEIGHT - PADDING_BOTTOM);

    // Desenha o eixo Y (ordenada)
    SDL_RenderDrawLine(renderer, PADDING_LEFT, 0, PADDING_LEFT, HEIGHT - PADDING_BOTTOM);

    SDL_Color textColor = { 0, 0, 0, 255 };

    // Adiciona marcações na escala do eixo Y
    int numDivisoes = 5;
    for (int i = 0; i <= numDivisoes; i++) {
        int y = HEIGHT - PADDING_BOTTOM - (i * (HEIGHT - PADDING_BOTTOM) / numDivisoes);
        float valor = (maxCotacao / numDivisoes) * i;

        SDL_RenderDrawLine(renderer, PADDING_LEFT - 5, y, PADDING_LEFT, y);
        
        string label = to_string(valor);
        renderText(renderer, font, label, 5, y - 10, textColor);
    }
}

// Função para desenhar o gráfico de barras
void drawGraph(SDL_Renderer* renderer, TTF_Font* font, vector<Dado>& dados) {
    if (dados.empty()) return;

    int numBarras = dados.size();
    int barWidth = (WIDTH - PADDING_LEFT) / numBarras;

    float maxCotacao = 0;
    for (const auto& d : dados) {
        if (d.cotacao > maxCotacao) {
            maxCotacao = d.cotacao;
        }
    }

    if (maxCotacao == 0) maxCotacao = 1;

    SDL_Color textColor = { 0, 0, 0, 255 };

    // Desenha cada barra e seus rótulos
    for (int i = 0; i < numBarras; i++) {
        int barHeight = (dados[i].cotacao / maxCotacao) * (HEIGHT - PADDING_BOTTOM - 50);
        SDL_Rect rect = { PADDING_LEFT + i * barWidth, HEIGHT - PADDING_BOTTOM - barHeight, barWidth - 10, barHeight };

        SDL_SetRenderDrawColor(renderer, 0, 191, 255, 255);
        SDL_RenderFillRect(renderer, &rect);

        // Desenha a cotação no topo da barra
        string cotacaoTexto = to_string(dados[i].cotacao);
        renderText(renderer, font, cotacaoTexto, rect.x + (barWidth - 50) / 2, rect.y - 20, textColor);

        // Desenha a data na base da barra
        renderText(renderer, font, dados[i].data, rect.x + 5, HEIGHT - 40, textColor);
    }

    // Desenha os eixos e a escala
    drawAxes(renderer, font, maxCotacao);
}

int main() {
    vector<Dado> dados = leitura();

    for (const auto& d : dados) {
        cout << d.data << " " << d.cotacao << endl;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        cerr << "Erro ao inicializar SDL ou SDL_ttf: " << SDL_GetError() << endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Gráfico em C++",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!window || !renderer) {
        cerr << "Erro ao criar janela ou renderizador: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("Arial.ttf", 16);
    if (!font) {
        cerr << "Erro ao carregar fonte: " << TTF_GetError() << endl;
        SDL_Quit();
        return 1;
    }

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
        drawGraph(renderer, font, dados);
        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
