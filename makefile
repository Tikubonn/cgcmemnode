
export CFLAGS = -I. -Iinclude -Llib

debug: .always 
	make libcgcmemnode.a libcgcmemnode.so CFLAGS="$(CFLAGS) -Og -g -Wall"

release: .always 
	make libcgcmemnode.a libcgcmemnode.so CFLAGS="$(CFLAGS) -O3 -Wall"

test: .always
	make debug 
	make test.exe CFLAGS="$(CFLAGS) -Og -g -Wall"
	gdb test.exe 
	#./test.exe

setup: .always
	git clone https://github.com/tikubonn/bitarray
	git clone https://github.com/tikubonn/memnode
	make release -C bitarray
	cp bitarray/bitarray.h include/bitarray.h
	cp bitarray/libbitarray.so lib/libbitarray.so
	cp bitarray/libbitarray.a lib/libbitarray.a
	make setup release -C memnode
	cp memnode/memnode.h include/memnode.h
	cp memnode/libmemnode.so lib/libmemnode.so
	cp memnode/libmemnode.a lib/libmemnode.a

clean: .always
	rm -f cgcmemnode.o

libbitarray.so: lib/libbitarray.so
	cp lib/libbitarray.so libbitarray.so

libmemnode.so: lib/libmemnode.so
	cp lib/libmemnode.so libmemnode.so

test.exe: test.c libcgcmemnode.so cgcmemnode.h libmemnode.so libbitarray.so
	gcc $(CFLAGS) test.c libcgcmemnode.so libmemnode.so libbitarray.so -o test.exe 

.always:

cgcmemnode.o: cgcmemnode.c cgcmemnode.h 
	gcc $(CFLAGS) -c cgcmemnode.c -o cgcmemnode.o

libcgcmemnode.so: cgcmemnode.o lib/libbitarray.so lib/libmemnode.so
	gcc $(CFLAGS) -shared cgcmemnode.o lib/libbitarray.so lib/libmemnode.so -o libcgcmemnode.so

libcgcmemnode.a: cgcmemnode.o 
	ar r libcgcmemnode.a cgcmemnode.o
