#include <stdio.h>
#include <stdlib.h>

#define TAM 30

typedef struct dado
{
    char date[11];
    float cotacao;
} dado;

dado *leitura(int *tamanho)
{
    FILE *arq = fopen("arquivo.txt", "r");
    if (arq == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        return NULL;
    }

    dado *dados = malloc(TAM * sizeof(dado));
    if (dados == NULL)
    {
        printf("Erro ao alocar memória\n");
        fclose(arq);
        return NULL;
    }

    char line[100];
    int i = 0;
    while (fgets(line, sizeof(line), arq) != NULL && i < TAM)
    {
        sscanf(line, "%10[^,],%f", dados[i].date, &dados[i].cotacao);
        i++;
    }
    fclose(arq);
    
    *tamanho = i; // Retorna o número real de entradas lidas
    return dados;
}

void gerarGrafico()
{
    FILE *gnuplot = popen("gnuplot -persistent", "w");
    if (gnuplot == NULL)
    {
        printf("Erro ao abrir o Gnuplot\n");
        return;
    }

    int tamanho;
    dado *dados = leitura(&tamanho);
    if (dados == NULL)
        return;

    fprintf(gnuplot, "set title 'Cotacao'\n");
    fprintf(gnuplot, "set xlabel 'Data'\n");
    fprintf(gnuplot, "set ylabel 'Cotação'\n");
    fprintf(gnuplot, "set xdata time\n");
    fprintf(gnuplot, "set timefmt '%%d-%%m-%%Y'\n");
    fprintf(gnuplot, "set format x '%%d/%%m'\n");
    fprintf(gnuplot, "plot '-' using 1:2 with linespoints title 'Vendas'\n");

    // Usar apenas os valores realmente lidos
    for (int i = 0; i < tamanho; i++)
    {
        fprintf(gnuplot, "%s %f\n", dados[i].date, dados[i].cotacao);
    }

    fprintf(gnuplot, "e\n");
    pclose(gnuplot);

    free(dados); // Libera a memória alocada
}

int main()
{
    gerarGrafico();
    return 0;
}
