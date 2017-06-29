objects = cli.o tokenizer.o ttymode.o charGetter.o myCommandHistory.o
objects2 = calc.o tokenizer.o
objects3 = listf.o tokenizer.o
objects4 = fref.o tokenizer.o regexp.o

warmup : $(objects) $(objects2) $(objects3) $(objects4)
	gcc -Wall -o cli $(objects)
	gcc -Wall -o calc $(objects2)
	gcc -Wall -o listf $(objects3)
	gcc -Wall -o fref $(objects4)
	
%.o : %.c
	gcc -Wall -c -g $<
	
clean : 
	rm -f cli.o
	rm -f calc.o
	rm -f listf.o
	rm -f fref.o
	rm -f $(objects)
	rm -f $(objects2)
	rm -f $(objects3)
	rm -f $(objects4)
	

