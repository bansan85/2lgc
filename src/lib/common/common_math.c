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

#include "config.h"
#include <libintl.h>
#include <locale.h>
#include <math.h>
#include <gmodule.h>
#include "common_math.h"


G_MODULE_EXPORT double common_math_arrondi_nombre(double nombre)
/* Description : Arrondi un nombre en supprimant la partie négligeable (ERREUR_RELATIVE_MIN)
 *   L'algorithme est perfectible puisque lors de l'arrondi, une nouvelle imprécision apparait
 *   et certains nombres peuvent être arrondi en 1.09999999 ou -23.000000001.
 * Paramètres : double nombre : le nombre à arrondir.
 * Valeur renvoyée : le nombre arrondi.
 */
{
    double          puissance;
    
    // Si le nombre est égal parfaitement à 0. Alors
    //     Renvoie 0.
    // FinSi
    // Multiplication du nombre par 10^(ERREUR_RELATIVE_PUISSANCE-ceil(log10(ABS(nombre))))
    //   afin d'avoir une partie entière composée de ERREUR_RELATIVE_PUISSANCE chiffres.
    // Conservation de la partie entière uniquement du nombre.
    // Division par 10^(ERREUR_RELATIVE_PUISSANCE-ceil(log10(ABS(nombre)))).
    // Renvoie du nombre.
    if (ERREUR_RELATIVE_EGALE(nombre, 0.))
        return 0.;
    puissance = ERREUR_RELATIVE_PUISSANCE-ceil(log10(ABS(nombre)));
    nombre = nombre*pow(10, puissance);
    modf(nombre, &nombre);
    return nombre/pow(10., puissance);
}


G_MODULE_EXPORT void common_math_arrondi_triplet(cholmod_triplet *triplet)
/* Description : Arrondi un triplet en supprimant la partie négligeable (ERREUR_RELATIVE_MIN).
 * Paramètres : cholmod_triplet *triplet : la variable triplet à arrondir.
 * Valeur renvoyée : Aucune.
 */
{
    double          *ax;
    unsigned int    i;
    
    // Trivial
    ax = (double*)triplet->x;
    for (i=0;i<triplet->nnz;i++)
        ax[i] = common_math_arrondi_nombre(ax[i]);
    
    return;
}


G_MODULE_EXPORT void common_math_arrondi_sparse(cholmod_sparse *sparse)
/* Description : Arrondi un sparse en supprimant la partie négligeable (ERREUR_RELATIVE_MIN).
 * Paramètres : cholmod_sparse *sparse : la matrice sparse à arrondir.
 * Valeur renvoyée : Aucune.
 */
{
    double          *ax;
    unsigned int    i;
    
    // Trivial
    ax = (double*)sparse->x;
    for (i=0;i<sparse->nzmax;i++)
        ax[i] = common_math_arrondi_nombre(ax[i]);
    
    return;
}


G_MODULE_EXPORT void common_math_double_to_char(double nombre, char *dest, int decimales)
/* Description : Converti un nombre double en char *.
                 Dest doit déjà être alloué. 30 caractères devrait être suffisant.
 * Paramètres : double nombre : nombre à convertir,
 *              char *dest : nombre converti,
 *              int decimales : nombre de décimales au maximum.
 * Valeur renvoyée : Aucune.
 */
{
    double  test;
    int     width;
    
    if (decimales > 20)
        decimales = 20;
    
    // Si le nombre est supérieur à 1e15, on affiche sous forme scientifique
    if (ABS(nombre) > 1e15)
    {
        for (width = 0; width<=decimales; width++)
        {
            sprintf(dest, "%.*le", width, nombre);
            sscanf(dest, "%le", &test);
            if ((fabs(nombre)*0.999999999999999 <= fabs(test)) && (fabs(test) <= fabs(nombre)*1.000000000000001))
                break;
        }
    }
    // Sinon on affiche sous forme normale
    else
    {
        for (width = 0; width<=decimales; width++)
        {
            sprintf(dest, "%.*lf", width, nombre);
            sscanf(dest, "%lf", &test);
            if ((fabs(nombre)*0.999999999999999 <= fabs(test)) && (fabs(test) <= fabs(nombre)*1.000000000000001))
                break;
        }
    }
    
    return;
}