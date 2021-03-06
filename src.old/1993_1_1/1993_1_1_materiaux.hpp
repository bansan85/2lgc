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

#ifndef __1993_1_1_MATERIAUX_H
#define __1993_1_1_MATERIAUX_H

#include "config.h"
#include "common_projet.hpp"

#define COEFFICIENT_NU_ACIER 0.3
#define MODULE_YOUNG_ACIER 210000000000.

EF_Materiau *_1993_1_1_materiaux_ajout (Projet      *p,
                                        std::string *nom,
                                        Flottant     fy,
                                        Flottant     fu)
                                       __attribute__((__warn_unused_result__));
bool         _1993_1_1_materiaux_modif (Projet      *p,
                                        EF_Materiau *materiau,
                                        std::string *nom,
                                        Flottant     fy,
                                        Flottant     fu,
                                        Flottant     e,
                                        Flottant     nu)
                                       __attribute__((__warn_unused_result__));

std::string _1993_1_1_materiaux_get_description (EF_Materiau *materiau)
                                       __attribute__((__warn_unused_result__));

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
