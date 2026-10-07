all:

	gcc -o myMalloc myMalloc.c
	./myMalloc

clean:

	rm myMalloc
