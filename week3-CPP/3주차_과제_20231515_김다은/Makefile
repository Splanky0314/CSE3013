cc=g++
cflags = -W -g
target = a.out

objects = main.o str.o

$(target) : $(objects)
	$(cc) $(cflags) -o $(target) $(objects)

%.o : %.c
	$(cc) $(cflags) -c -o $@ $<

main.o str.o : Str.h

.PHONY : clean
clean :
	rm $(target) $(objects)
