import matplotlib
matplotlib.use('Agg')  # Ou 'Qt5Agg', 'Agg', 'GTK3Agg', etc.
import matplotlib.pyplot as plt


# Dados do gráfico
meses = ["Janeiro", "Fevereiro", "Março", "Abril"]
vendas = [10, 25, 30, 15]

# Criando o gráfico
plt.figure(figsize=(8, 6))
bars = plt.bar(meses, vendas, color=["#4C72B0", "#55A868", "#C44E52", "#8172B3"], edgecolor="black")

# Adicionando rótulos nos valores das barras
for bar in bars:
    yval = bar.get_height()
    plt.text(bar.get_x() + bar.get_width()/2, yval, f"{yval}", ha="center", va="bottom", fontsize=12, fontweight="bold")

# Configurações do gráfico
plt.xlabel("Meses", fontsize=12)
plt.ylabel("Vendas", fontsize=12)
plt.title("Vendas Mensais", fontsize=14, fontweight="bold")
plt.xticks(rotation=45)  # Rotaciona os rótulos do eixo X, se necessário
plt.grid(axis="y", linestyle="--", alpha=0.7)

# Salvar o gráfico como imagem
plt.savefig("vendas_mensais.png", dpi=300, bbox_inches="tight")

# Exibir o gráfico
plt.show()
