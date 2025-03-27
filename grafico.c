#include <stdio.h>

typedef struct dado
{
    char mes[10];
    int cotacao;
} dado;


dado *  leitura() {
    FILE * arq = fopen("arquivo.txt", "r");
    dado * dados[50];
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo\n");
        return;
    }
    
    char data[10];
    float cotacao;
    while (fscanf(arq, "%s %d", &data, &cotacao) == 2) {
        printf("Mes: %s, Vendas: %d\n", data, cotacao);
    }
    fclose(arq);
}
void gerarGrafico() {
    FILE *gnuplot = popen("gnuplot -persistent", "w");
    
    fprintf(gnuplot, "set title 'Vendas Mensais'\n");
    fprintf(gnuplot, "set xlabel 'Meses'\n");
    fprintf(gnuplot, "set ylabel 'Vendas'\n");
    fprintf(gnuplot, "plot '-' with linespoints title 'Vendas'\n");
    fprintf(gnuplot, "1 10\n2 25\n3 30\n4 15\n");
    fprintf(gnuplot, "e\n");
    pclose(gnuplot);
}

int main() {
    gerarGrafico();
    return 0;
}