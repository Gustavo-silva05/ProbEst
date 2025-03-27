all: node 


node: fetch-data.js
	node fetch-data.js

c: grafico.c
	gcc grafico.c -o grafico

c+: grafico.cpp
	g++ grafico.cpp -o grafico

run: 
	./grafico

clean:
	rm -f *.png grafico *.txt
	clear
