PLUGINS =  \
			  ad3dsfilter.so \

LFLAGS = -g -Wall
CPPFLAGS = -fPIC -DMM3D_EDIT -I../../src -I../../src/libmm3d -I../../src/mm3dcore -I../.. -DPLUGIN

# Mac OS X flags. Uncomment the following line to compile on Mac OS X
#LFLAGS = -g -Wall -undefined suppress -bundle -flat_namespace

all: ${PLUGINS}
	
%.o: %.cc
	g++ ${CPPFLAGS} ${CXXFLAGS} -c $<

%.so: %.o
	g++ ${LFLAGS} -shared -o $*.so $< -l3ds

distclean: clean

clean:
	rm -rf *.o *.so core core.*
