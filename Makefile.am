ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -fno-builtin -Wall -std=c99 -pedantic

test: all test.c
	$(COMPILE) -o test test.c -L.libs -lm -lral2 ${LIBS}
	export LD_LIBRARY_PATH='.libs'; ./test

lib_LTLIBRARIES = libral2.la

libral2_la_HEADERFILES = \
	include/ral2/class.h \
	include/ral2/error.h \
	include/ral2/ordinal.h \
	include/ral2/integer.h \
	include/ral2/real.h \
	include/ral2/string.h \
	include/ral2/interval.h \
	include/ral2/data_point.h \
	include/ral2/classifier.h \
	include/ral2/linear_interpolation.h \
	include/ral2/color.h \
	include/ral2/symbol.h \
	include/ral2/storage.h \
	include/ral2/array.h \
	include/ral2/kvp.h \
	include/ral2/hash.h \
	include/ral2/bin.h \
	include/ral2/geometry.h \
	include/ral2/feature.h \
	include/ral2/layer.h \
	include/ral2/pixbuf.h \
	include/ral2/coords.h \
	include/ral2/grid.h \
	include/ral2/grid/dem.h \
	include/ral2/grid/image.h \
	include/ral2/grid/io.h \
	include/ral2/grid/neighborhood.h \
	include/ral2/grid/rasterize.h \
	include/ral2/grid/resample.h \
	include/ral2/grid/statistics.h \
	include/ral2/raster.h \
	include/ral2/vector.h \
	include/ral2/style.h \
	include/ral2/ral.h

libral2_la_SOURCES = \
	src/class.c \
	src/object.c \
	src/error.c \
	src/ordinal.c \
	src/integer.c \
	src/real.c \
	src/string.c \
	src/interval.c \
	src/data_point.c \
	src/classifier.c \
	src/linear_interpolation.c \
	src/grayscale.c \
	src/hue_scale.c \
	src/color_component_scale.c \
	src/color.c \
	src/symbol.c \
	src/storage.c \
	src/array.c \
	src/kvp.c \
	src/hash.c \
	src/bin.c \
	src/feature.c \
	src/geometry.c \
	src/cell.c \
	src/grid.c \
	src/grid/dem.c \
	src/grid/image.c \
	src/grid/io.c \
	src/grid/neighborhood.c \
	src/grid/rasterize.c \
	src/grid/resample.c \
	src/grid/statistics.c \
	src/style.c \
	src/layer.c \
	src/vector.c \
	src/rcoords.c \
	src/wcoords.c \
	src/wbox.c \
	src/raster.c \
	src/pixbuf.c \
	${libral2_la_HEADERFILES}

libral2_la_LDFLAGS = -version-info 1:0:0 -module

EXTRA_DIST = test.c \
	autogen.sh \
	COPYING \
	include/ral2/config.h.in \
	include/priv/config.h.in \
	include/priv/util.h \
	include/priv/msg.h \
	include/priv/class.h \
	include/priv/geometry.h \
	include/priv/cell.h \
	include/priv/grid.h \
	include/priv/dem.h \
	include/priv/pixbuf.h \
	include/priv/priv.h \
	index.dox \
	libral2.pc.in

nobase_include_HEADERS = ${libral2_la_HEADERFILES}

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libral2.pc

clean-local:
	rm -f test

MAINTAINERCLEANFILES = include/priv/config.h.in
