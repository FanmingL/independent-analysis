CC = gcc
target = iva
objects = main.o\
		  iva.o\
		  matrix.o\
		  fft.o
cflags =
inc = -I.
lib_dir =
lib = 

$(target):$(objects)
	$(CC) -O2 -o $@ $^ $(lib_dir) $(lib) 
%.o:%.c
	${CC} ${cflags} -O2 -o $@ -c $< $(inc)  

clean:
	rm -rf $(target) $(objects)
