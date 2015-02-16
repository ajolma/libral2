libral2
=======

A library for various geospatial raster methods and visualization of
geospatial data

HISTORY
=======

This library began a long time ago as the part written in C for a Perl
module Grid.pm. Grid.pm became Geo::Raster and it requires this
library. libral2 is a reboot of libral and it uses an object-oriented
structure, a well-defined API with opaque pointers to classes and
objects, and messaging with objects.

PURPOSE
=======

This library has three purposes. The first is to be a fast and simple
raster algebra calculator. The second is geospatial analysis
algorithms, especially for hydrological terrain analysis. The third is
to provide methods for rendering geospatial data on a graphics
backend.

DEPENDENCIES
============

Raster input and output and vector output are mostly left for GDAL.

GDK and Cairo are used as the graphics backend.

LIMITATIONS
===========

The data type of the cells of the libral rasters (they are called
grids in the code) are either integer or real. The implementation data
types are by default short (two byte int) and float (four byte
real). Use of other data types is not tested.

The libral rasters are in-memory rasters. Thus the size of the rasters
are limited by the amount of RAM in the computer.

NOTES ON THE CODE
=================

ral2/ral.h

The main header to include for using libral2.

TO DO / FUTURE PLANS
====================

There is still very much to do to implement and test the fundamentals.

OTHER CODE FOR RASTER ALGEBRA / TERRAIN ANALYSIS
================================================

For raster algebra / terrain analysis there are also other libraries,
some of which are free. These include

PCRaster http://pcraster.geo.uu.nl/

RiverTools http://rivix.com/

TAPES-G Gallant and Wilson. TAPES-G: A grid-based terrain analysis
program for the environmental sciences. Computers & Geosciences 22(7) 713-722.

TARDEM http://www.crwr.utexas.edu/gis/gishydro99/uwrl/tardem.html

TOPOG http://www.per.clw.csiro.au/topog/

TOPAZ http://www.ars.usda.gov/Main/docs.htm?docid=21167

OpenEV http://openev.sourceforge.net

GRASS http://grass.osgeo.org/
