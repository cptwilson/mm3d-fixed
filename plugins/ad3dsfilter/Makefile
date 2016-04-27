PLUGINS =  \
			  ad3dsfilter.so \

LFLAGS = -g -Wall
CFLAGS = -g -Wall -DMM3D_EDIT -I../../src -I../../src/libmm3d -I../../src/mm3dcore -I../.. -Ilib3ds -DPLUGIN

# Mac OS X flags. Uncomment the following line to compile on Mac OS X
#LFLAGS = -g -Wall -undefined suppress -bundle -flat_namespace

all: lib3ds_lib ${PLUGINS}
	
lib3ds_lib: lib3ds/Makefile
	make -C lib3ds all

lib3ds/Makefile: lib3ds/configure.in
	cd lib3ds && ./configure && cd ..

%.o: %.cc
	g++ ${CFLAGS} -c $<

%.so: %.o
	g++ ${LFLAGS} -shared -o $*.so $< lib3ds/lib3ds/lib3ds.a

distclean: clean
	make -C lib3ds distclean

clean:
	rm -rf *.o *.so core core.*
	make -C lib3ds clean
