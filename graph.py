import matplotlib.pyplot as plt

# Dados do gráfico
meses = ["Janeiro", "Fevereiro", "Março", "Abril"]
vendas = [10, 25, 30, 15]

# Criando o gráfico
plt.figure(figsize=(8, 6))
plt.bar(meses, vendas, color="skyblue", edgecolor="black")

# Configurações do gráfico
plt.xlabel("Meses")
plt.ylabel("Vendas")
plt.title("Vendas Mensais")
plt.grid(axis="y", linestyle="--", alpha=0.7)

# Exibir o gráfico
plt.show()


