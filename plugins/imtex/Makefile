PLUGINS =  \
			  imtex.so \

LFLAGS = -g -Wall
CPPFLAGS = -fPIC -DMM3D_EDIT -I../../src -I../../src/libmm3d -I../../src/mm3dcore -I../.. -DPLUGIN

# Mac OS X flags. Uncomment the following line to compile on Mac OS X
#LFLAGS = -O2 -Wall -lImlib2 -undefined suppress -bundle -flat_namespace

all: ${PLUGINS}
	
%.o: %.cc
	g++ ${CPPFLAGS} ${CXXFLAGS} -c $<

%.so: %.o
	g++ ${LFLAGS} -shared -o $*.so $< -lImlib2

clean:
	rm -rf *.o *.so core core.*
