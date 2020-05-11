all:
	gcc -o sandbox -g main.c -ldl
	gcc -o sandbox.so -shared -fPIC sandbox.c -ldl
clean:
	rm sandbox.so sandbox