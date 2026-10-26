# Builds the PC example (needs only g++): make && make demo
UI = ../ui
SRC = simulador.cpp $(UI)/UiApp.cpp $(UI)/UiScreens.cpp $(UI)/UiCanvas.cpp $(UI)/UiConfig.cpp \
      $(UI)/UiGbPalettes.cpp $(UI)/UiMarca.cpp $(UI)/UiCheatParse.cpp $(UI)/UiThumb.cpp $(UI)/UiGlyphs.cpp $(UI)/UiCartArt.cpp

simulador: $(SRC)
	g++ -O2 -I$(UI) $(SRC) -o simulador

demo: simulador
	mkdir -p raiz/game raiz/hdd raiz/usb0 capturas
	./simulador raiz demo.txt capturas

clean:
	rm -rf simulador raiz capturas
