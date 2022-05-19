scanner: scanner.o reader.o lineReader.o dataValidation.o gettersAndSetters.o lineReaderTwo.o fileBuilding.o
	gcc -ansi -Wall -pedantic scanner.o reader.o lineReader.o dataValidation.o gettersAndSetters.o lineReaderTwo.o fileBuilding.o -o assembler 
scanner.o: scanner.c functions.h
	gcc -c -ansi -Wall -pedantic scanner.c
reader.o: reader.c functions.h
	gcc -c -ansi -Wall -pedantic reader.c
lineReader.o: lineReader.c functions.h
	gcc -c -ansi -Wall -pedantic lineReader.c
dataValidation.o: dataValidation.c functions.h
	gcc -c -ansi -Wall -pedantic dataValidation.c
gettersAndSetters.o: gettersAndSetters.c functions.h
	gcc -c -ansi -Wall -pedantic gettersAndSetters.c
lineReaderTwo.o: lineReaderTwo.c functions.h
	gcc -c -ansi -Wall -pedantic lineReaderTwo.c
fileBuilding.o: fileBuilding.c functions.h
	gcc -c -ansi -Wall -pedantic fileBuilding.c
