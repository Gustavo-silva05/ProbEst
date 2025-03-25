#include <stdio.h>

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