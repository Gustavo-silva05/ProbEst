CXXFLAGS = -Wall -g
M = make


all: node 


node: fetch-data.js
	node fetch-data.js

c: grafico.c 
	gcc grafico.c -o grafico $(CXXFLAGS)
	$(M) run

c+: grafico.cpp 
	g++ grafico.cpp -o grafico -lSDL2 -lSDL2_ttf $(CXXFLAGS)
	$(M) run

run: 
	./grafico

clean:
	rm -f *.png grafico *.txt
	clear
