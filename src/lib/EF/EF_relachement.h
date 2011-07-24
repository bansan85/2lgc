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

#ifndef __EF_RELACHEMENT_H
#define __EF_RELACHEMENT_H

#include "common_projet.h"

typedef enum
{
	EF_RELACHEMENT_LIBRE,
	EF_RELACHEMENT_BLOQUE
} Type_EF_Relachement;

typedef struct __Relachement
{
	unsigned int		numero;
	Type_EF_Relachement	rx_debut;
	Type_EF_Relachement	ry_debut;
	Type_EF_Relachement	rz_debut;
	Type_EF_Relachement	rx_fin;
	Type_EF_Relachement	ry_fin;
	Type_EF_Relachement	rz_fin;
} Relachement;

int EF_relachement_init(Projet *projet);
int EF_relachement_ajout(Projet *projet, Type_EF_Relachement rx_debut, Type_EF_Relachement ry_debut, Type_EF_Relachement rz_debut, Type_EF_Relachement rx_fin, Type_EF_Relachement ry_fin, Type_EF_Relachement rz_fin);
Relachement* EF_relachement_cherche_numero(Projet *projet, unsigned int numero);
int EF_relachement_free(Projet *projet);

#endif