# Some configuration options
#----------------------------

# Any extra options you need
EXTRADEFS=

# Graphics library to use, can be GD (recommended) or ImageMagick (deprecated, outdated).
IMG_LIB=GD
#IMG_LIB=ImageMagick

# In simple mode, by default all data needed for queries is now
# read into memory, using in total about 500 bytes per image. It
# is possible to select a disk cache using mmap for this instead.
# Then the kernel can read this memory into the filecache or
# discard it as needed. The app uses as little memory as possible
# but depending on IO load queries can take longer (sometimes a lot).
# This option is especially useful for a VPS with little memory.
# override DEFS+=-DUSE_DISK_CACHE

# Enable a significantly less memory intensive but slightly slower
# method of storing the image index internally (in simple mode).
override DEFS+=-DUSE_DELTA_QUEUE

# For GCC the C++11 support also needs to be enabled explicitly
override DEFS+=-std=c++11

# This may help or hurt performance. Try it and see for yourself.
override DEFS+=-fomit-frame-pointer

# By default iqdb uses integer math for the similarity computation,
# because it is often slightly faster than floating point math
# (and iqdb cannot make use of SSE et.al.) You can remove this option
# if you wish to compare both versions. This setting has
# negligible impact on the value of the similarity result.
override DEFS+=-DINTMATH

# -------------------------
#  no configuration below
# -------------------------

.SUFFIXES:

all:	iqdb

.PHONY: clean

clean:
	rm -f *.o iqdb

%.o : %.h
%.o : %.cpp
iqdb.o : imgdb.h haar.h auto_clean.h debug.h
imgdb.o : imgdb.h imglib.h haar.h auto_clean.h delta_queue.h debug.h
test-db.o : imgdb.h delta_queue.h debug.h
haar.o :
%.le.o : %.h
iqdb.le.o : imgdb.h haar.h auto_clean.h debug.h
imgdb.le.o : imgdb.h imglib.h haar.h auto_clean.h delta_queue.h debug.h
haar.le.o :

.ALWAYS:

ifeq (${IMG_LIB},GD)
IMG_libs = $(shell pkg-config --libs gdlib libjpeg libpng)
IMG_flags = $(shell pkg-config --cflags gdlib libjpeg libpng)
IMG_objs = resizer.o
override DEFS+=-DLIB_GD
else
ifeq (${IMG_LIB}, ImageMagick)
IMG_libs = $(shell pkg-config --libs ImageMagick)
IMG_flags = $(shell pkg-config --cflags ImageMagick)
IMG_objs =
override DEFS+=-DLIB_ImageMagick
else
$(error Unsupported image library '${IMG_LIB}' selected.)
endif
endif

% : %.o haar.o imgdb.o debug.o ${IMG_objs} # bloom_filter.o
	g++ -o $@ $^ ${CFLAGS} ${LDFLAGS} ${IMG_libs} ${DEFS} ${EXTRADEFS}

test-resizer : test-resizer.o resizer.o debug.o
	g++ -o $@ $^ ${CFLAGS} ${LDFLAGS} -g -lgd -ljpeg -lpng ${DEFS} ${EXTRADEFS} `gdlib-config --ldflags`

%.o : %.cpp
	g++ -c -o $@ $< -O2 ${CFLAGS} -DNDEBUG -Wall -DLinuxBuild -g ${IMG_flags} ${DEFS} ${EXTRADEFS}

%.S:	.ALWAYS
	g++ -S -o $@ $*.cpp -O2 ${CFLAGS} -DNDEBUG -Wall -DLinuxBuild -g ${IMG_flags} ${DEFS} ${EXTRADEFS}

