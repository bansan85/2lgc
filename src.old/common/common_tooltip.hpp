/*
 * 2lgc_code : calcul de résistance des matériaux selon les normes Eurocodes
 * Copyright (C) 2011
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __COMMON_TOOLTIP_H
#define __COMMON_TOOLTIP_H

#include "config.h"
#include "common_projet.hpp"

#ifdef ENABLE_GTK
#include <gtk/gtk.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

GtkWindow *common_tooltip_generation (xmlChar *nom)
                                       __attribute__((__warn_unused_result__));
#endif

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
