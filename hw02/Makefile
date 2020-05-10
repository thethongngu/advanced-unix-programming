all:
	g++ -o sandbox -g main.cpp -ldl
	g++ -o sandbox.so -shared -fPIC -std=c++17 sandbox.cpp utils.h -ldl
clean:
	rm sandbox.so sandbox