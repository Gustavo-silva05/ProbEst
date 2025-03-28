#include <SDL2/SDL.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

const int WIDTH = 800;
const int HEIGHT = 600;

class dado
{
public:
    string data;
    float cotacao;
    dado(string data, float cot)
    {
        this->data = data;
        this->cotacao = cot;
    };
    ~dado() {};
};

vector<dado> leitura(){
    vector<dado> dados;
    ifstream arq("arquivo.txt");
    string line;
    while(getline(arq, line)){
        stringstream ss(line);
        string data;
        getline(ss, data, ',');
        float cot;
        ss >> cot;
        dados.push_back(dado(data, cot));
    }
    arq.close();
    return dados;
}

// void drawGraph(SDL_Renderer *renderer)
// {
//     int barWidth = WIDTH / numBarras;

//     for (int i = 0; i < numBarras; i++)
//     {
//         SDL_Rect rect = {i * barWidth, HEIGHT - valores[i], barWidth - 10, valores[i]};
//         SDL_SetRenderDrawColor(renderer, 0, 191, 255, 255); // Azul claro
//         SDL_RenderFillRect(renderer, &rect);
//     }
// }

int main()
{
    vector<dado> dados = leitura();
    // for (int i = 0; i < dados.size(); i++)
    // {
    //     cout << dados[i].data << " " << dados[i].cotacao << endl;
    // }
    // if (SDL_Init(SDL_INIT_VIDEO) < 0)
    // {
    //     std::cerr << "Erro ao inicializar SDL: " << SDL_GetError() << std::endl;
    //     return 1;
    // }

    // SDL_Window *window = SDL_CreateWindow("Gráfico em C++",
    //                                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    //                                       WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    // SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // bool running = true;
    // SDL_Event event;

    // while (running)
    // {
    //     while (SDL_PollEvent(&event))
    //     {
    //         if (event.type == SDL_QUIT)
    //         {
    //             running = false;
    //         }
    //     }

    //     SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    //     SDL_RenderClear(renderer);
    //     drawGraph(renderer);
    //     SDL_RenderPresent(renderer);
    // }

    // SDL_DestroyRenderer(renderer);
    // SDL_DestroyWindow(window);
    // SDL_Quit();
    // return 0;
}
