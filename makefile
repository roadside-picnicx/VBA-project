build: 
	gcc -o VBA_projekt.exe src/func.c src/main.c cJSON/cJSON.c

all: 
	gcc -o VBA_projekt.exe src/func.c src/main.c cJSON/cJSON.c
	gcc -o unitTests.exe src/func.c tests/funcTest.c cJSON/cJSON.c -lcunit

build_tests: 
	gcc -o unitTests.exe src/func.c tests/funcTest.c cJSON/cJSON.c -lcunit

clean:
	rm VBA_projekt.exe

clean_all:
	rm VBA_projekt.exe unitTests.exe

clean_tests:
	rm unitTests.exe 
