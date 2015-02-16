#ifndef	PRIV_H
#define	PRIV_H

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>

/* GDAL */
#include <gdal.h>
#include <ogr_api.h>
#include <ogr_srs_api.h>

/* GTK+ */
#include <glib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "priv/config.h"
#include "ral/config.h"
#include "priv/msg.h"
#include "priv/util.h"
#include "priv/geometry.h"
#include "priv/cell.h"
#include "priv/grid.h"
#include "priv/dem.h"
#include "priv/pixbuf.h"
#include "priv/class.h"

/** a function that can be installed as a CPLErrorHandler and that reports GDAL errors as ral msg's */
void CPL_DLL CPL_STDCALL cpl_error(CPLErr eclass, int code, const char *msg);

#endif
