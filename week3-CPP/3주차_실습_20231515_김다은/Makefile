cc=g++
cflags = -W -g
target = main

objects = main.o array.o rangearray.o

$(target) : $(objects)
	$(cc) $(cflags) -o $(target) $(objects)

%.o : %.cpp
	$(cc) $(cflags) -c -o $@ $<

array.o: Array.h

rangearray.o: RangeArray.h

main.o: Array.h RangeArray.h

.PHONY : clean
clean :
	rm $(target) $(objects)
