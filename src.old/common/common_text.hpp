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

#ifndef __COMMON_TEXT_H
#define __COMMON_TEXT_H

#include "config.h"
#include "common_projet.hpp"

std::string format (const std::string fmt,
                    ...);

double   common_text_str_to_double  (char          *texte,
                                     double         val_min,
                                     bool           min_include,
                                     double         val_max,
                                     bool           max_include)
                                       __attribute__((__warn_unused_result__));
wchar_t *strcasestr_internal        (const wchar_t *haystack,
                                     const wchar_t *needle)
                                       __attribute__((__warn_unused_result__));
std::string common_text_dependances (std::list <EF_Noeud *> *liste_noeuds,
                                     std::list <EF_Barre *> *liste_barres,
                                     std::list <Charge   *> *liste_charges,
                                     Projet                 *p)
                                       __attribute__((__warn_unused_result__));
wchar_t *common_text_get_line       (FILE          *fichier)
                                       __attribute__((__warn_unused_result__));
char    *common_text_wcstostr_dup   (const wchar_t *texte)
                                       __attribute__((__warn_unused_result__));
wchar_t *common_text_strtowcs_dup   (const char    *texte)
                                       __attribute__((__warn_unused_result__));
#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
