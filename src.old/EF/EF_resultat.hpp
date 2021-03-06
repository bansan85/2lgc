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

#ifndef __EF_RESULTAT_H
#define __EF_RESULTAT_H

#include "config.h"
#include "common_projet.hpp"
#include "1990_action.hpp"

bool     EF_resultat_noeud_reaction_appui (std::list <Action *> *liste,
                                           EF_Noeud             *noeud,
                                           uint8_t               indice,
                                           Projet               *p,
                                           std::string          *texte,
                                           double               *mini,
                                           double               *maxi)
                                       __attribute__((__warn_unused_result__));
bool     EF_resultat_noeud_deplacement    (std::list <Action *> *liste,
                                           EF_Noeud             *noeud,
                                           uint8_t               indice,
                                           Projet               *p,
                                           std::string          *texte,
                                           double               *mini,
                                           double               *maxi)
                                       __attribute__((__warn_unused_result__));

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
