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

#include <stdlib.h>
#include <libintl.h>
#include <cholmod.h>
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_maths.h"
#include "1992_1_1_elements.h"
#include "EF_rigidite.h"
#include "math.h"

/* _1992_1_1_elements_init
 * Description : Initialise la liste des éléments en béton
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int _1992_1_1_elements_init(Projet *projet)
{
	if (projet == NULL)
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	projet->beton.elements = list_init();
	if (projet->beton.elements == NULL)
		BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
	else
		return 0;
}

/* _1992_1_1_elements_ajout
 * Description : Ajoute un élément à la liste des éléments en béton
 * Paramètres : Projet *projet : la variable projet
 *            : Type_Beton_Element type : type de l'élément en béton
 *            : int section : numéro de la section correspondant à l'élément
 *            : int materiau : numéro du matériau en béton de l'élément
 *            : int noeud_debut : numéro de départ de l'élément
 *            : int noeud_fin : numéro de fin de l'élément
 *            : int discretisation_element : nombre d'élément une fois discrétisé
 * Valeur renvoyée :
 *   Succès : 0 même si aucune section n'est existante
 *   Échec : valeur négative si la liste des éléments n'est pas initialisée ou a déjà été libérée
 */
int _1992_1_1_elements_ajout(Projet *projet, Type_Element type, int section, int materiau, int noeud_debut, int noeud_fin, int discretisation_element)
{
	Beton_Element	*element_en_cours, element_nouveau;
	
	if ((projet == NULL) || (projet->beton.elements == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	list_mvrear(projet->beton.elements);
	element_nouveau.element = type;
	
	if (_1992_1_1_sections_cherche_numero(projet, section) != 0)
		BUG(-2);
	element_nouveau.section = list_curr(projet->beton.sections);
	
	if (_1992_1_1_materiaux_cherche_numero(projet, materiau) != 0)
		BUG(-3);
	element_nouveau.materiau = list_curr(projet->beton.materiaux);
	
	if (EF_noeuds_cherche_numero(projet, noeud_debut) != 0)
		BUG(-4);
	element_nouveau.noeud_debut = list_curr(projet->ef_donnees.noeuds);
	
	if (EF_noeuds_cherche_numero(projet, noeud_fin) != 0)
		BUG(-5);
	element_nouveau.noeud_fin = list_curr(projet->ef_donnees.noeuds);
	
	element_nouveau.discretisation_element = discretisation_element;
	if (discretisation_element != 0)
	{
		int	i;
		element_nouveau.noeuds_intermediaires = malloc(sizeof(EF_Noeud)*(discretisation_element));
		if (element_nouveau.noeuds_intermediaires == NULL)
			BUGTEXTE(-6, gettext("Erreur d'allocation mémoire.\n"));
		for (i=0;i<discretisation_element;i++)
		{
			double		dx, dy, dz;
			dx = (element_nouveau.noeud_fin->position.x-element_nouveau.noeud_debut->position.x)/(discretisation_element+1);
			dy = (element_nouveau.noeud_fin->position.y-element_nouveau.noeud_debut->position.y)/(discretisation_element+1);
			dz = (element_nouveau.noeud_fin->position.z-element_nouveau.noeud_debut->position.z)/(discretisation_element+1);
			if (EF_noeuds_ajout(projet, element_nouveau.noeud_debut->position.x+dx*(i+1), element_nouveau.noeud_debut->position.y+dy*(i+1), element_nouveau.noeud_debut->position.z+dz*(i+1), -1) != 0)
				BUG(-6);
			element_nouveau.noeuds_intermediaires[i] = list_rear(projet->ef_donnees.noeuds);
		}
	}
	else
		element_nouveau.noeuds_intermediaires = NULL;
	
	element_nouveau.matrice_rotation = NULL;
	element_nouveau.matrice_rigidite_locale = NULL;
	element_nouveau.matrice_rigidite_globale = NULL;
	
	element_en_cours = (Beton_Element *)list_rear(projet->beton.elements);
	if (element_en_cours == NULL)
		element_nouveau.numero = 0;
	else
		element_nouveau.numero = element_en_cours->numero+1;
	
	if (list_insert_after(projet->beton.elements, &(element_nouveau), sizeof(element_nouveau)) == NULL)
		BUGTEXTE(-8, gettext("Erreur d'allocation mémoire.\n"));
	
	return 0;
}


/* _1992_1_1_elements_cherche_numero
 * Description : Positionne dans la liste des éléments en béton l'élément courant au numéro souhaité
 * Paramètres : Projet *projet : la variable projet
 *            : int numero : le numéro de la section
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int _1992_1_1_elements_cherche_numero(Projet *projet, int numero)
{
	if ((projet == NULL) || (projet->beton.elements == NULL) || (list_size(projet->beton.elements) == 0))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	list_mvfront(projet->beton.elements);
	do
	{
		Beton_Element	*element = list_curr(projet->beton.elements);
		
		if (element->numero == numero)
			return 0;
	}
	while (list_mvnext(projet->beton.elements) != NULL);
	
	BUGTEXTE(-2, gettext("Élément en béton n°%d introuvable.\n"), numero);
}


/* _1992_1_1_elements_rigidite_ajout
 * Description : ajouter une de la rigidité à la matrice
 * Paramètres : Projet *projet : la variable projet
 *            : Beton_Element* element : pointeur vers l'élément en béton
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int _1992_1_1_elements_rigidite_ajout(Projet *projet, int num_element)
{
	EF_Noeud	*noeud1, *noeud2;
	long		*ai, *aj, *ai2, *aj2;	// Pointeur vers les données des triplets
	double		*ax, *ax2;		// Pointeur vers les données des triplets
	double		y, cosx, sinx; // valeurs de la matrice de rotation
	double		xx, yy, zz, ll; // Dimension de la barre dans 3D
	unsigned int	i;
	int		j;
	cholmod_triplet	*triplet;
	cholmod_sparse	*sparse_tmp, *sparse_rotation_transpose;
	Beton_Element	*element;
	Beton_Section_Carre	*section_donnees;
	Beton_Section_Caracteristiques	*section_caract;
	
	if ((projet == NULL) || (projet->ef_donnees.rigidite_triplet == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	if (_1992_1_1_elements_cherche_numero(projet, num_element) != 0)
		BUG(-2);
	element = list_curr(projet->beton.elements);
	section_donnees = element->section;
	section_caract = section_donnees->caracteristiques;
	
	// On commence par calculer la matrice de rotation 3D.
	triplet = cholmod_l_allocate_triplet(12, 12, 32, 0, CHOLMOD_REAL, projet->ef_donnees.c);
	ai = triplet->i;
	aj = triplet->j;
	ax = triplet->x;
	xx = element->noeud_fin->position.x - element->noeud_debut->position.x;
	yy = element->noeud_fin->position.y - element->noeud_debut->position.y;
	zz = element->noeud_fin->position.z - element->noeud_debut->position.z;
	ll = sqrt(xx*xx+yy*yy+zz*zz);
	y = asin(zz/ll);
	sinx = yy / sqrt(ll*ll-zz*zz);
	cosx = copysign(1.0, xx)*sqrt((ll*ll-yy*yy-zz*zz)/(ll*ll-zz*zz));
	for (i=0;i<4;i++)
	{
		ai[i*8+0] = i*3+0; aj[i*8+0] = i*3+0; ax[i*8+0] = cosx*cos(y);
		ai[i*8+1] = i*3+0; aj[i*8+1] = i*3+1; ax[i*8+1] = -sinx;
		ai[i*8+2] = i*3+0; aj[i*8+2] = i*3+2; ax[i*8+2] = -cosx*sin(y);
		ai[i*8+3] = i*3+1; aj[i*8+3] = i*3+0; ax[i*8+3] = sinx*cos(y);
		ai[i*8+4] = i*3+1; aj[i*8+4] = i*3+1; ax[i*8+4] = cosx;
		ai[i*8+5] = i*3+1; aj[i*8+5] = i*3+2; ax[i*8+5] = -sinx*sin(y);
		ai[i*8+6] = i*3+2; aj[i*8+6] = i*3+0; ax[i*8+6] = sin(y);
		ai[i*8+7] = i*3+2; aj[i*8+7] = i*3+2; ax[i*8+7] = cos(y);
	}
	triplet->nnz=32;
	element->matrice_rotation = cholmod_l_triplet_to_sparse(triplet, 0, projet->ef_donnees.c);
	cholmod_l_free_triplet(&triplet, projet->ef_donnees.c);
	
	for (j=0;j<element->discretisation_element+1;j++)
	{
		// Si element->discretisation_element == 0, il n'y a pas besoin de recalculer ll
		if (j==0)
		{
			noeud1 = element->noeud_debut;
			if (element->discretisation_element != 0)
				noeud2 = element->noeuds_intermediaires[0];
			else
				noeud2 = element->noeud_fin;
		}
		else if (j == element->discretisation_element)
		{
			noeud1 = element->noeuds_intermediaires[j-1];
			noeud2 = element->noeud_fin;
		}
		else
		{
			noeud1 = element->noeuds_intermediaires[j-1];
			noeud2 = element->noeuds_intermediaires[j];
		}
		xx = noeud2->position.x - noeud1->position.x;
		yy = noeud2->position.y - noeud1->position.y;
		zz = noeud2->position.z - noeud1->position.z;
		ll = sqrt(xx*xx+yy*yy+zz*zz);
		
		// On calcule la matrice de rigidité. locale
		triplet = cholmod_l_allocate_triplet(12, 12, 40, 0, CHOLMOD_REAL, projet->ef_donnees.c);
		ai = triplet->i;
		aj = triplet->j;
		ax = triplet->x;
		ai[0] = 0;	aj[0] = 0;	ax[0] = element->materiau->ecm*section_caract->a/ll;
		ai[1] = 0;	aj[1] = 6;	ax[1] = -ax[0];
		ai[2] = 1;	aj[2] = 1;	ax[2] = 12*element->materiau->ecm*section_caract->iz/ll/ll/ll;
		ai[3] = 1;	aj[3] = 5;	ax[3] = 6*element->materiau->ecm*section_caract->iz/ll/ll;
		ai[4] = 1;	aj[4] = 7;	ax[4] = -ax[2];
		ai[5] = 1;	aj[5] = 11;	ax[5] = ax[3];
		ai[6] = 2;	aj[6] = 2;	ax[6] = 12*element->materiau->ecm*section_caract->iy/ll/ll/ll;
		ai[7] = 2;	aj[7] = 4;	ax[7] = -6*element->materiau->ecm*section_caract->iy/ll/ll;
		ai[8] = 2;	aj[8] = 8;	ax[8] = -ax[6];
		ai[9] = 2;	aj[9] = 10;	ax[9] = ax[7];
		ai[10] = 3;	aj[10] = 3;	ax[10] = element->materiau->gnu_0_2*section_caract->j/ll;
		ai[11] = 3;	aj[11] = 9;	ax[11] = -ax[10];
		ai[12] = 4;	aj[12] = 2;	ax[12] = ax[7];
		ai[13] = 4;	aj[13] = 4;	ax[13] = 4*element->materiau->ecm*section_caract->iy/ll;
		ai[14] = 4;	aj[14] = 8;	ax[14] = -ax[7];
		ai[15] = 4;	aj[15] = 10;	ax[15] = 2*element->materiau->ecm*section_caract->iy/ll;
		ai[16] = 5;	aj[16] = 1;	ax[16] = ax[3];
		ai[17] = 5;	aj[17] = 5;	ax[17] = 4*element->materiau->ecm*section_caract->iz/ll;
		ai[18] = 5;	aj[18] = 7;	ax[18] = -ax[3];
		ai[19] = 5;	aj[19] = 11;	ax[19] = 2*element->materiau->ecm*section_caract->iz/ll;
		ai[20] = 6;	aj[20] = 0;	ax[20] = -ax[0];
		ai[21] = 6;	aj[21] = 6;	ax[21] = ax[0];
		ai[22] = 7;	aj[22] = 1;	ax[22] = -ax[2];
		ai[23] = 7;	aj[23] = 5;	ax[23] = -ax[3];
		ai[24] = 7;	aj[24] = 7;	ax[24] = ax[2];
		ai[25] = 7;	aj[25] = 11;	ax[25] = -ax[3];
		ai[26] = 8;	aj[26] = 2;	ax[26] = -ax[6];
		ai[27] = 8;	aj[27] = 4;	ax[27] = -ax[7];
		ai[28] = 8;	aj[28] = 8;	ax[28] = ax[6];
		ai[29] = 8;	aj[29] = 10;	ax[29] = -ax[7];
		ai[30] = 9;	aj[30] = 3;	ax[30] = -ax[10];
		ai[31] = 9;	aj[31] = 9;	ax[31] = ax[10];
		ai[32] = 10;	aj[32] = 2;	ax[32] = ax[7];
		ai[33] = 10;	aj[33] = 4;	ax[33] = ax[15];
		ai[34] = 10;	aj[34] = 8;	ax[34] = -ax[7];
		ai[35] = 10;	aj[35] = 10;	ax[35] = ax[13];
		ai[36] = 11;	aj[36] = 1;	ax[36] = ax[3];
		ai[37] = 11;	aj[37] = 5;	ax[37] = ax[19];
		ai[38] = 11;	aj[38] = 7;	ax[38] = -ax[3];
		ai[39] = 11;	aj[39] = 11;	ax[39] = ax[17]; 
		triplet->nnz=40;
		element->matrice_rigidite_locale = cholmod_l_triplet_to_sparse(triplet, 0, projet->ef_donnees.c);
		cholmod_l_free_triplet(&triplet, projet->ef_donnees.c);
		
		// On calcule la matrice locale dans le repère globale.
		// La matrice de rigidité globale est égale à R.K.R-1 mais commr R-1 = RT, on calcul R.K.RT
		sparse_tmp = cholmod_l_ssmult(element->matrice_rotation, element->matrice_rigidite_locale, 0, 1, 0, projet->ef_donnees.c);
		sparse_rotation_transpose = cholmod_l_transpose(element->matrice_rotation, 1, projet->ef_donnees.c);
		element->matrice_rigidite_globale = cholmod_l_ssmult(sparse_tmp, sparse_rotation_transpose, 0, 1, 0, projet->ef_donnees.c);
		cholmod_l_free_sparse(&(sparse_tmp), projet->ef_donnees.c);
		cholmod_l_free_sparse(&(sparse_rotation_transpose), projet->ef_donnees.c);
		triplet = cholmod_l_sparse_to_triplet(element->matrice_rigidite_globale, projet->ef_donnees.c);
		ai = triplet->i;
		aj = triplet->j;
		ax = triplet->x;
		ai2 = projet->ef_donnees.rigidite_triplet->i;
		aj2 = projet->ef_donnees.rigidite_triplet->j;
		ax2 = projet->ef_donnees.rigidite_triplet->x;
		
		// On insère le quatre quarts de la matrice de rigidité globale
		for (i=0;i<triplet->nnz;i++)
		{
			if ((ai[i] < 6) && (aj[i] < 6) && (projet->ef_donnees.noeuds_flags_partielle[noeud1->numero][ai[i]] != -1) && (projet->ef_donnees.noeuds_flags_partielle[noeud1->numero][aj[i]] != -1))
			{
				ai2[projet->ef_donnees.rigidite_triplet_en_cours] = projet->ef_donnees.noeuds_flags_partielle[noeud1->numero][ai[i]];
				aj2[projet->ef_donnees.rigidite_triplet_en_cours] = projet->ef_donnees.noeuds_flags_partielle[noeud1->numero][aj[i]];
				ax2[projet->ef_donnees.rigidite_triplet_en_cours] = ax[i];
				projet->ef_donnees.rigidite_triplet_en_cours++;
			}
			else if ((ai[i] < 6) && (aj[i] >= 6) && (projet->ef_donnees.noeuds_flags_partielle[noeud1->numero][ai[i]] != -1) && (projet->ef_donnees.noeuds_flags_partielle[noeud2->numero][aj[i]-6] != -1))
			{
				ai2[projet->ef_donnees.rigidite_triplet_en_cours] = projet->ef_donnees.noeuds_flags_partielle[noeud1->numero][ai[i]];
				aj2[projet->ef_donnees.rigidite_triplet_en_cours] = projet->ef_donnees.noeuds_flags_partielle[noeud2->numero][aj[i]-6];
				ax2[projet->ef_donnees.rigidite_triplet_en_cours] = ax[i];
				projet->ef_donnees.rigidite_triplet_en_cours++;
			}
			else if ((ai[i] >= 6) && (aj[i] < 6) && (projet->ef_donnees.noeuds_flags_partielle[noeud2->numero][ai[i]-6] != -1) && (projet->ef_donnees.noeuds_flags_partielle[noeud1->numero][aj[i]] != -1))
			{
				ai2[projet->ef_donnees.rigidite_triplet_en_cours] = projet->ef_donnees.noeuds_flags_partielle[noeud2->numero][ai[i]-6];
				aj2[projet->ef_donnees.rigidite_triplet_en_cours] = projet->ef_donnees.noeuds_flags_partielle[noeud1->numero][aj[i]];
				ax2[projet->ef_donnees.rigidite_triplet_en_cours] = ax[i];
				projet->ef_donnees.rigidite_triplet_en_cours++;
			}
			else if ((ai[i] >= 6) && (aj[i] >= 6) && (projet->ef_donnees.noeuds_flags_partielle[noeud2->numero][ai[i]-6] != -1) && (projet->ef_donnees.noeuds_flags_partielle[noeud2->numero][aj[i]-6] != -1))
			{
				ai2[projet->ef_donnees.rigidite_triplet_en_cours] = projet->ef_donnees.noeuds_flags_partielle[noeud2->numero][ai[i]-6];
				aj2[projet->ef_donnees.rigidite_triplet_en_cours] = projet->ef_donnees.noeuds_flags_partielle[noeud2->numero][aj[i]-6];
				ax2[projet->ef_donnees.rigidite_triplet_en_cours] = ax[i];
				projet->ef_donnees.rigidite_triplet_en_cours++;
			}
		}
		cholmod_l_free_triplet(&triplet, projet->ef_donnees.c);
	}
	
	return 0;
}



/* _1992_1_1_elements_free
 * Description : Libère l'ensemble des éléments  en béton
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0 même si aucune section n'est existante
 *   Échec : valeur négative si la liste des éléments n'est pas initialisée ou a déjà été libérée
 */
int _1992_1_1_elements_free(Projet *projet)
{
	if ((projet == NULL) || (projet->beton.elements == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	while (!list_empty(projet->beton.elements))
	{
		Beton_Element *element = list_remove_front(projet->beton.elements);
		
		free(element->noeuds_intermediaires);
		cholmod_l_free_sparse(&(element->matrice_rotation), projet->ef_donnees.c);
		cholmod_l_free_sparse(&(element->matrice_rigidite_locale), projet->ef_donnees.c);
		cholmod_l_free_sparse(&(element->matrice_rigidite_globale), projet->ef_donnees.c);
		
		free(element);
	}
	
	free(projet->beton.elements);
	projet->beton.elements = NULL;
	
	return 0;
}
