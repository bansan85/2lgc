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

#ifndef __COMMON_MATHS_H
#define __COMMON_MATHS_H

#include <cholmod.h>

#define MAX(X, Y)  ((X) > (Y) ? (X) : (Y))
#define MIN(X, Y)  ((X) < (Y) ? (X) : (Y))
#define ABS(X) ((X) > 0 ? (X) : -(X))

// erreur_relative_egale
// Vérifie que x == y avec une erreur relative maximale D
// y étant la valeur recherchée et x la valeur à vérifier
#define ERREUR_RELATIVE_EGALE(x, y) ( ((x) == 0.) || ((y) == 0.) ? (MAX(ABS(x),ABS(y))-MIN(ABS(x),ABS(y)) < (ERREUR_RELATIVE_MIN)) : ((MAX(ABS(x),ABS(y))-MIN(ABS(x),ABS(y)))/(MIN(ABS(x),ABS(y))) < (ERREUR_RELATIVE_MIN) ))

#define ERREUR_RELATIVE_PUISSANCE 14 // Avec 15, on peut se retrouver avec 2 décimales fausses
#define ERREUR_RELATIVE_MIN 1e-14

double common_math_arrondi(double nombre);
void common_math_arrondi_triplet(cholmod_triplet *triplet);
void common_math_arrondi_sparse(cholmod_sparse *sparse);

#endif
