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
#include <string.h>
//#include <SuiteSparseQR_C.h>
#include <time.h>
#include <unistd.h>
#include <values.h>
#include <math.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_maths.h"
#include "common_fonction.h"
#include "EF_rigidite.h"
#include "EF_noeud.h"
#include "1990_actions.h"
#include "1992_1_1_elements.h"

/* EF_calculs_initialise
 * Description : Initialise les divers variables nécessaire à l'ajout des rigidités
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int EF_calculs_initialise(Projet *projet)
{
    unsigned int    i, nnz_max;
    
    // Détermine pour chaque noeud si la ligne / colonne de la matrice de rigidité globale
    // doit être pris en compte dans la résolution du système.
    // le nombre [0] indique la colonne/ligne correspondant au déplacement selon l'axe x
    // le nombre [5] indique la colonne/ligne correspondant à la rotation autour de l'axe z
    // le nombre vaut -1 si la colonne n'est pas inséré dans la matrice de rigidité globale
    projet->ef_donnees.noeuds_flags_partielle = (int**)malloc(sizeof(int*)*list_size(projet->ef_donnees.noeuds));
    if (projet->ef_donnees.noeuds_flags_partielle == NULL)
        BUGTEXTE(-1, gettext("Erreur d'allocation mémoire.\n"));
    for (i=0;i<list_size(projet->ef_donnees.noeuds);i++)
    {
        projet->ef_donnees.noeuds_flags_partielle[i] = (int*)malloc(6*sizeof(int));
        if (projet->ef_donnees.noeuds_flags_partielle[i] == NULL)
            BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
    }
    projet->ef_donnees.noeuds_flags_complete = (int**)malloc(sizeof(int*)*list_size(projet->ef_donnees.noeuds));
    if (projet->ef_donnees.noeuds_flags_complete == NULL)
        BUGTEXTE(-1, gettext("Erreur d'allocation mémoire.\n"));
    for (i=0;i<list_size(projet->ef_donnees.noeuds);i++)
    {
        projet->ef_donnees.noeuds_flags_complete[i] = (int*)malloc(6*sizeof(int));
        if (projet->ef_donnees.noeuds_flags_complete[i] == NULL)
            BUGTEXTE(-2, gettext("Erreur d'allocation mémoire.\n"));
    }
    i = 0; // Nombre de colonnes/lignes de la matrice
    projet->ef_donnees.nb_colonne_matrice_complete = 0;
    list_mvfront(projet->ef_donnees.noeuds);
    do
    {
        EF_Noeud    *noeud = list_curr(projet->ef_donnees.noeuds);
        
        projet->ef_donnees.noeuds_flags_complete[noeud->numero][0] = projet->ef_donnees.nb_colonne_matrice_complete; projet->ef_donnees.nb_colonne_matrice_complete++; // x
        projet->ef_donnees.noeuds_flags_complete[noeud->numero][1] = projet->ef_donnees.nb_colonne_matrice_complete; projet->ef_donnees.nb_colonne_matrice_complete++; // y
        projet->ef_donnees.noeuds_flags_complete[noeud->numero][2] = projet->ef_donnees.nb_colonne_matrice_complete; projet->ef_donnees.nb_colonne_matrice_complete++; // z
        projet->ef_donnees.noeuds_flags_complete[noeud->numero][3] = projet->ef_donnees.nb_colonne_matrice_complete; projet->ef_donnees.nb_colonne_matrice_complete++; // rx
        projet->ef_donnees.noeuds_flags_complete[noeud->numero][4] = projet->ef_donnees.nb_colonne_matrice_complete; projet->ef_donnees.nb_colonne_matrice_complete++; // ry
        projet->ef_donnees.noeuds_flags_complete[noeud->numero][5] = projet->ef_donnees.nb_colonne_matrice_complete; projet->ef_donnees.nb_colonne_matrice_complete++; // rz
        
        if (noeud->appui == NULL)
        {
            projet->ef_donnees.noeuds_flags_partielle[noeud->numero][0] = i; i++; // x
            projet->ef_donnees.noeuds_flags_partielle[noeud->numero][1] = i; i++; // y
            projet->ef_donnees.noeuds_flags_partielle[noeud->numero][2] = i; i++; // z
            projet->ef_donnees.noeuds_flags_partielle[noeud->numero][3] = i; i++; // rx
            projet->ef_donnees.noeuds_flags_partielle[noeud->numero][4] = i; i++; // ry
            projet->ef_donnees.noeuds_flags_partielle[noeud->numero][5] = i; i++; // rz
        }
        else
        {
            EF_Appui    *appui = noeud->appui;
            if (appui->x == EF_APPUI_LIBRE)
                { projet->ef_donnees.noeuds_flags_partielle[noeud->numero][0] = i; i++; }
            else projet->ef_donnees.noeuds_flags_partielle[noeud->numero][0] = -1;
            if (appui->y == EF_APPUI_LIBRE)
                { projet->ef_donnees.noeuds_flags_partielle[noeud->numero][1] = i; i++; }
            else projet->ef_donnees.noeuds_flags_partielle[noeud->numero][1] = -1;
            if (appui->z == EF_APPUI_LIBRE)
                { projet->ef_donnees.noeuds_flags_partielle[noeud->numero][2] = i; i++; }
            else projet->ef_donnees.noeuds_flags_partielle[noeud->numero][2] = -1;
            if (appui->rx == EF_APPUI_LIBRE)
                { projet->ef_donnees.noeuds_flags_partielle[noeud->numero][3] = i; i++; }
            else projet->ef_donnees.noeuds_flags_partielle[noeud->numero][3] = -1;
            if (appui->ry == EF_APPUI_LIBRE)
                { projet->ef_donnees.noeuds_flags_partielle[noeud->numero][4] = i; i++; }
            else projet->ef_donnees.noeuds_flags_partielle[noeud->numero][4] = -1;
            if (appui->rz == EF_APPUI_LIBRE)
                { projet->ef_donnees.noeuds_flags_partielle[noeud->numero][5] = i; i++; }
            else projet->ef_donnees.noeuds_flags_partielle[noeud->numero][5] = -1;
        }
    }
    while (list_mvnext(projet->ef_donnees.noeuds) != NULL);
    projet->ef_donnees.nb_colonne_matrice_partielle = i;
    
    // On détermine au maximum le nombre de triplet il sera nécessaire pour obtenir la matrice de rigidité globale, soit 12*12*nombre_d'éléments (y compris discrétisation)
    nnz_max = 0;
    projet->ef_donnees.triplet_rigidite_partielle_en_cours = 0;
    projet->ef_donnees.triplet_rigidite_complete_en_cours = 0;
    list_mvfront(projet->beton.elements);
    do
    {
        Beton_Element   *element = list_curr(projet->beton.elements);
        
        nnz_max += 12*12*(element->discretisation_element+1);
    }
    while (list_mvnext(projet->beton.elements) != NULL);
    projet->ef_donnees.triplet_rigidite_partielle = cholmod_l_allocate_triplet(projet->ef_donnees.nb_colonne_matrice_partielle, projet->ef_donnees.nb_colonne_matrice_partielle, nnz_max, 0, CHOLMOD_REAL, projet->ef_donnees.c);
    projet->ef_donnees.triplet_rigidite_partielle->nnz = nnz_max;
    projet->ef_donnees.triplet_rigidite_complete = cholmod_l_allocate_triplet(projet->ef_donnees.nb_colonne_matrice_complete, projet->ef_donnees.nb_colonne_matrice_complete, nnz_max, 0, CHOLMOD_REAL, projet->ef_donnees.c);
    projet->ef_donnees.triplet_rigidite_complete->nnz = nnz_max;
    
    return 0;
}

/* EF_calculs_genere_sparse
 * Description : Converti la matrice de rigidité complète de la structure sous forme de liste en une matrice de rigidité partielle (les lignes (et colonnes) dont on connait les déplacements ne sont pas insérées) sous forme d'une matrice sparse
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative si la liste de rigidité n'est pas initialisée ou a déjà été libérée
 */
int EF_calculs_genere_sparse(Projet *projet)
{
    cholmod_triplet     *triplet_rigidite;
    unsigned int        j;
    double          *ax;        // Pointeur vers les données des triplets
    
    if ((projet == NULL) || (projet->ef_donnees.triplet_rigidite_partielle == NULL) || (projet->ef_donnees.triplet_rigidite_complete == NULL))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    // On commence par déterminer la valeur maximale d'une case de la matrice pour 
    // ensuite supprimer les valeurs négligeables.
    projet->ef_donnees.max_rigidite = 0.;
    ax = projet->ef_donnees.triplet_rigidite_complete->x;
    projet->ef_donnees.triplet_rigidite_complete->nzmax = projet->ef_donnees.triplet_rigidite_complete_en_cours;
    for (j=0;j<projet->ef_donnees.triplet_rigidite_complete->nzmax;j++)
    {
        if (ABS(ax[j]) > projet->ef_donnees.max_rigidite)
            projet->ef_donnees.max_rigidite = ABS(ax[j]);
    }
    
    // Cela signifie que tous les noeuds sont bloqués (cas d'une poutre sur deux appuis sans discrétisation par exemple)
    if (projet->ef_donnees.nb_colonne_matrice_partielle == 0)
    {
        triplet_rigidite = cholmod_l_allocate_triplet(0, 0, 0, 0, CHOLMOD_REAL, projet->ef_donnees.c);
        projet->ef_donnees.rigidite_matrice_partielle = cholmod_l_triplet_to_sparse(triplet_rigidite, 0, projet->ef_donnees.c);
        projet->ef_donnees.rigidite_matrice_partielle->stype = 0;
        projet->ef_donnees.rigidite_matrice_complete = cholmod_l_triplet_to_sparse(projet->ef_donnees.triplet_rigidite_complete, 0, projet->ef_donnees.c);
        cholmod_l_drop(projet->ef_donnees.max_rigidite*ERREUR_RELATIVE_MIN, projet->ef_donnees.rigidite_matrice_complete, projet->ef_donnees.c);
        projet->ef_donnees.QR = SuiteSparseQR_C_factorize(0, 0., projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.c);
//      Pour utiliser cholmod dans les calculs de matrices.
//      projet->ef_donnees.factor_rigidite_matrice_partielle = cholmod_l_analyze (projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.c) ;
//      cholmod_l_factorize(projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.factor_rigidite_matrice_partielle, projet->ef_donnees.c);
        cholmod_l_free_triplet(&triplet_rigidite, projet->ef_donnees.c);
        
        return 0;
    }
    
    // On converti la liste des triplets en matrice sparse
    projet->ef_donnees.rigidite_matrice_partielle = cholmod_l_triplet_to_sparse(projet->ef_donnees.triplet_rigidite_partielle, 0, projet->ef_donnees.c);
    projet->ef_donnees.rigidite_matrice_complete = cholmod_l_triplet_to_sparse(projet->ef_donnees.triplet_rigidite_complete, 0, projet->ef_donnees.c);
    // On enlève les valeurs parasites
    cholmod_l_drop(projet->ef_donnees.max_rigidite*ERREUR_RELATIVE_MIN, projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.c);
    cholmod_l_drop(projet->ef_donnees.max_rigidite*ERREUR_RELATIVE_MIN, projet->ef_donnees.rigidite_matrice_complete, projet->ef_donnees.c);
//  cholmod_l_sort(projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.c);
    // On force la matrice à ne pas être symétrique.
    if (projet->ef_donnees.rigidite_matrice_partielle->stype != 0)
    {
        cholmod_sparse *A = cholmod_l_copy(projet->ef_donnees.rigidite_matrice_partielle, 0, 1, projet->ef_donnees.c);
        cholmod_l_free_sparse(&projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.c);
        projet->ef_donnees.rigidite_matrice_partielle = A;
    }
    if (projet->ef_donnees.rigidite_matrice_complete->stype != 0)
    {
        cholmod_sparse *A = cholmod_l_copy(projet->ef_donnees.rigidite_matrice_complete, 0, 1, projet->ef_donnees.c);
        cholmod_l_free_sparse(&projet->ef_donnees.rigidite_matrice_complete, projet->ef_donnees.c);
        projet->ef_donnees.rigidite_matrice_complete = A;
    }
    
/*  Pour utiliser cholmod dans les calculs de matrices.
     Et on factorise la matrice
    projet->ef_donnees.factor_rigidite_matrice_partielle = cholmod_l_analyze (projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.c) ;
    // Normalement, c'est par cette méthode qu'on résoud une matrice non symétrique. Mais en pratique, ça ne marche pas. Pourquoi ?!?
    double beta[2] = {1.e-6, 0.};
    if (cholmod_l_factorize_p(projet->ef_donnees.rigidite_matrice_partielle, beta, NULL, 0, projet->ef_donnees.factor_rigidite_matrice_partielle, projet->ef_donnees.c) == TRUE)
    {
        if (projet->ef_donnees.c->status == CHOLMOD_NOT_POSDEF)
            BUGTEXTE(-1, "Matrice non définie positive.\n");
    }*/
    
    projet->ef_donnees.QR = SuiteSparseQR_C_factorize(0, 0., projet->ef_donnees.rigidite_matrice_partielle, projet->ef_donnees.c);
    
    return 0;
}


/* EF_calculs_resoud_charge
 * Description : Déterminer à partir de la matrice de rigidité globale partielle les déplacements des noeuds pour une action particulière.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative si la liste de rigidité n'est pas initialisée ou a déjà été libérée
 */
int EF_calculs_resoud_charge(Projet *projet, int num_action)
{
    Action          *action_en_cours;
    cholmod_triplet     *triplet_deplacements_totaux, *triplet_deplacements_partiels;
    cholmod_triplet     *triplet_force_partielle, *triplet_force_complete;
    cholmod_sparse      *sparse_force;
    cholmod_dense       *dense_force;
    cholmod_triplet     *triplet_efforts_locaux_finaux, *triplet_efforts_globaux_initiaux, *triplet_efforts_locaux_initiaux, *triplet_efforts_globaux_finaux;
    cholmod_sparse      *sparse_efforts_locaux_initaux, *sparse_efforts_locaux_finaux, *sparse_efforts_globaux_finaux, *sparse_efforts_globaux_initiaux;
    long            *ai, *aj;   // Pointeur vers les données des triplets
    double          *ax;        // Pointeur vers les données des triplets
    long            *ai2, *aj2; // Pointeur vers les données des triplets
    double          *ax2;       // Pointeur vers les données des triplets
    long            *ai3, *aj3; // Pointeur vers les données des triplets
    double          *ax3;       // Pointeur vers les données des triplets
    unsigned int        i, j;
    double          max_effort;
    
    if ((projet == NULL) || (projet->actions == NULL) || (list_size(projet->actions) == 0) || (_1990_action_cherche_numero(projet, num_action) != 0) || (projet->ef_donnees.QR == NULL))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    // On crée la vecteur sparse contenant les actions extérieures
    // On commence par initialiser les vecteurs "force partielle" et "force complet" par des 0.
    action_en_cours = list_curr(projet->actions);
    common_fonction_init(projet, action_en_cours);
    triplet_force_partielle = cholmod_l_allocate_triplet(projet->ef_donnees.rigidite_matrice_partielle->nrow, 1, projet->ef_donnees.rigidite_matrice_partielle->nrow, 0, CHOLMOD_REAL, projet->ef_donnees.c);
    ai = triplet_force_partielle->i;
    aj = triplet_force_partielle->j;
    ax = triplet_force_partielle->x;
    triplet_force_partielle->nnz = projet->ef_donnees.rigidite_matrice_partielle->nrow;
    for (i=0;i<triplet_force_partielle->nnz;i++)
    {
        ai[i] = i;
        aj[i] = 0;
        ax[i] = 0.;
    }
    triplet_force_complete = cholmod_l_allocate_triplet(projet->ef_donnees.rigidite_matrice_complete->nrow, 1, projet->ef_donnees.rigidite_matrice_complete->nrow, 0, CHOLMOD_REAL, projet->ef_donnees.c);
    ai3 = triplet_force_complete->i;
    aj3 = triplet_force_complete->j;
    ax3 = triplet_force_complete->x;
    triplet_force_complete->nnz = projet->ef_donnees.rigidite_matrice_complete->nrow;
    for (i=0;i<triplet_force_complete->nnz;i++)
    {
        ai3[i] = i;
        aj3[i] = 0;
        ax3[i] = 0.;
    }
    
    // On ajoute les charges aux noeuds
    if (list_size(action_en_cours->charges) != 0)
    {
        list_mvfront(action_en_cours->charges);
        do
        {
            Charge_Ponctuelle_Barre *charge_barre = list_curr(action_en_cours->charges);
            if (charge_barre->type == CHARGE_PONCTUELLE_NOEUD)
            {
                // On ajoute au vecteur des efforts les efforts aux noeuds
                Charge_Ponctuelle_Noeud *charge_noeud = list_curr(action_en_cours->charges);
                if (projet->ef_donnees.noeuds_flags_partielle[charge_noeud->noeud->numero][0] != -1)
                    ax[projet->ef_donnees.noeuds_flags_partielle[charge_noeud->noeud->numero][0]] += charge_noeud->x;
                if (projet->ef_donnees.noeuds_flags_partielle[charge_noeud->noeud->numero][1] != -1)
                    ax[projet->ef_donnees.noeuds_flags_partielle[charge_noeud->noeud->numero][1]] += charge_noeud->y;
                if (projet->ef_donnees.noeuds_flags_partielle[charge_noeud->noeud->numero][2] != -1)
                    ax[projet->ef_donnees.noeuds_flags_partielle[charge_noeud->noeud->numero][2]] += charge_noeud->z;
                if (projet->ef_donnees.noeuds_flags_partielle[charge_noeud->noeud->numero][3] != -1)
                    ax[projet->ef_donnees.noeuds_flags_partielle[charge_noeud->noeud->numero][3]] += charge_noeud->rx;
                if (projet->ef_donnees.noeuds_flags_partielle[charge_noeud->noeud->numero][4] != -1)
                    ax[projet->ef_donnees.noeuds_flags_partielle[charge_noeud->noeud->numero][4]] += charge_noeud->ry;
                if (projet->ef_donnees.noeuds_flags_partielle[charge_noeud->noeud->numero][5] != -1)
                    ax[projet->ef_donnees.noeuds_flags_partielle[charge_noeud->noeud->numero][5]] += charge_noeud->rz;
                ax3[projet->ef_donnees.noeuds_flags_complete[charge_noeud->noeud->numero][0]] += charge_noeud->x;
                ax3[projet->ef_donnees.noeuds_flags_complete[charge_noeud->noeud->numero][1]] += charge_noeud->y;
                ax3[projet->ef_donnees.noeuds_flags_complete[charge_noeud->noeud->numero][2]] += charge_noeud->z;
                ax3[projet->ef_donnees.noeuds_flags_complete[charge_noeud->noeud->numero][3]] += charge_noeud->rx;
                ax3[projet->ef_donnees.noeuds_flags_complete[charge_noeud->noeud->numero][4]] += charge_noeud->ry;
                ax3[projet->ef_donnees.noeuds_flags_complete[charge_noeud->noeud->numero][5]] += charge_noeud->rz;
            }
            // Les efforts ne sont pas aux noeuds mais dans la barre.
            // Il faut donc calculer les réactions d'appuis pour chaque cas.
            else
            {
                double      E;              // Module d'Young
                double      Iy, Iz;             // Inertie
                double      xx, yy, zz, l;          // Longueur
                double      position_charge_relative;   // Position de la charge par rapport au début de l'élément discrétisé
                double      debut_barre, fin_barre;     // Début et fin de la barre discrétisée par rapport à la barre complète
                double      kAy, kBy, kAz, kBz;     // Inverse de la raideur autour de l'axe y et z (0 si encastré, infini si articulé)
                double      phiAy, phiBy, phiAz, phiBz; // Rotation sur appui lorsque la barre est isostatique
                double      ay, by, cy, az, bz, cz;     // Souplesse de la barre
                double      MAy, MBy, MAz, MBz;     // Moment créé par la raideur
                double      MAx, MBx;           // Moment aux noeuds dû au moment de torsion
                double      FAx, FBx;           // Réaction d'appui
                double      FAy, FBy, FAz, FBz;     // Réaction d'appui
                EF_Noeud    *noeud_debut, *noeud_fin;   // Le noeud de départ et le noeud de fin, nécessaire en cas de discrétisation
                
                Beton_Element   *element_en_beton = charge_barre->barre;
                
                if ((element_en_beton->type == BETON_ELEMENT_POTEAU) || (element_en_beton->type == BETON_ELEMENT_POUTRE))
                {
                    Beton_Section_Rectangulaire *section = element_en_beton->section;
                    E = element_en_beton->materiau->ecm;
                    Iy = section->caracteristiques->iy;
                    Iz = section->caracteristiques->iz;
                    
                    // Charge ponctuelle dans la barre
                    if (charge_barre->type == CHARGE_PONCTUELLE_BARRE)
                    {
                        // On converti les efforts globaux en efforts locaux dans triplet_efforts_locaux_initiaux
                        if (charge_barre->repere_local == FALSE)
                        {
                            triplet_efforts_globaux_initiaux = cholmod_l_allocate_triplet(12, 1, 12, 0, CHOLMOD_REAL, projet->ef_donnees.c);
                            ai2 = triplet_efforts_globaux_initiaux->i;
                            aj2 = triplet_efforts_globaux_initiaux->j;
                            ax2 = triplet_efforts_globaux_initiaux->x;
                            triplet_efforts_globaux_initiaux->nnz = 12;
                        }
                        else
                        {
                            triplet_efforts_locaux_initiaux = cholmod_l_allocate_triplet(12, 1, 12, 0, CHOLMOD_REAL, projet->ef_donnees.c);
                            ai2 = triplet_efforts_locaux_initiaux->i;
                            aj2 = triplet_efforts_locaux_initiaux->j;
                            ax2 = triplet_efforts_locaux_initiaux->x;
                            triplet_efforts_locaux_initiaux->nnz = 12;
                        }
                        ai2[0] = 0; aj2[0] = 0; ax2[0] = charge_barre->x;
                        ai2[1] = 1; aj2[1] = 0; ax2[1] = charge_barre->y;
                        ai2[2] = 2; aj2[2] = 0; ax2[2] = charge_barre->z;
                        ai2[3] = 3; aj2[3] = 0; ax2[3] = charge_barre->rx;
                        ai2[4] = 4; aj2[4] = 0; ax2[4] = charge_barre->ry;
                        ai2[5] = 5; aj2[5] = 0; ax2[5] = charge_barre->rz;
                        ai2[6] = 6; aj2[6] = 0; ax2[6] = 0.;
                        ai2[7] = 7; aj2[7] = 0; ax2[7] = 0.;
                        ai2[8] = 8; aj2[8] = 0; ax2[8] = 0.;
                        ai2[9] = 9; aj2[9] = 0; ax2[9] = 0.;
                        ai2[10] = 10;   aj2[10] = 0;    ax2[10] = 0.;
                        ai2[11] = 11;   aj2[11] = 0;    ax2[11] = 0.;
                        if (charge_barre->repere_local == FALSE)
                        {
                            sparse_efforts_globaux_initiaux = cholmod_l_triplet_to_sparse(triplet_efforts_globaux_initiaux, 0, projet->ef_donnees.c);
                            cholmod_l_free_triplet(&triplet_efforts_globaux_initiaux, projet->ef_donnees.c);
                            sparse_efforts_locaux_initaux = cholmod_l_ssmult(element_en_beton->matrice_rotation_transpose, sparse_efforts_globaux_initiaux, 0, 1, 0, projet->ef_donnees.c);
                            cholmod_l_free_sparse(&(sparse_efforts_globaux_initiaux), projet->ef_donnees.c);
                            triplet_efforts_locaux_initiaux = cholmod_l_sparse_to_triplet(sparse_efforts_locaux_initaux, projet->ef_donnees.c);
                            ai2 = triplet_efforts_locaux_initiaux->i;
                            aj2 = triplet_efforts_locaux_initiaux->j;
                            ax2 = triplet_efforts_locaux_initiaux->x;
                            cholmod_l_free_sparse(&(sparse_efforts_locaux_initaux), projet->ef_donnees.c);
                        }
                        
                        // On regarde s'il y a une discrétisation et on cherche entre quel noeud et quel noeud se trouve l'effort
                        position_charge_relative = charge_barre->position;
                        if (element_en_beton->discretisation_element == 0)
                        // Pas de discrétisation
                        {
                            noeud_debut = element_en_beton->noeud_debut;
                            noeud_fin = element_en_beton->noeud_fin;
                        }
                        else
                        // On cherche le noeud de départ et le noeud de fin
                        {
                            i = 0;
                            l = -1.;
                            // On regarde pour chaque noeud intermédiaire si la position de la charge devient inférieur à la distance entre le noeud de départ et le noeud intermédiaire
                            while ((i<element_en_beton->discretisation_element) && (l < charge_barre->position))
                            {
                                xx = element_en_beton->noeuds_intermediaires[i]->position.x - element_en_beton->noeud_debut->position.x;
                                yy = element_en_beton->noeuds_intermediaires[i]->position.y - element_en_beton->noeud_debut->position.y;
                                zz = element_en_beton->noeuds_intermediaires[i]->position.z - element_en_beton->noeud_debut->position.z;
                                l = sqrt(xx*xx+yy*yy+zz*zz);
                                i++;
                            }
                            i--;
                            // Alors la position de la charge est compris entre le début du noeud et le premier noeud intermédiaire
                            if (i==0)
                            {
                                noeud_debut = element_en_beton->noeud_debut;
                                noeud_fin = element_en_beton->noeuds_intermediaires[0];
                            }
                            // Alors la position de la charge est compris entre le dernier noeud intermédiaire et le noeud de fin de la barre
                            else if (i == element_en_beton->discretisation_element-1)
                            {
                                noeud_debut = element_en_beton->noeuds_intermediaires[i];
                                noeud_fin = element_en_beton->noeud_fin;
                            }
                            else
                            {
                                noeud_debut = element_en_beton->noeuds_intermediaires[i-1];
                                noeud_fin = element_en_beton->noeuds_intermediaires[i];
                            }
                        }
                        xx = noeud_debut->position.x - element_en_beton->noeud_debut->position.x;
                        yy = noeud_debut->position.y - element_en_beton->noeud_debut->position.y;
                        zz = noeud_debut->position.z - element_en_beton->noeud_debut->position.z;
                        debut_barre = sqrt(xx*xx+yy*yy+zz*zz);
                        position_charge_relative = charge_barre->position-debut_barre;
                        xx = noeud_fin->position.x - element_en_beton->noeud_debut->position.x;
                        yy = noeud_fin->position.y - element_en_beton->noeud_debut->position.y;
                        zz = noeud_fin->position.z - element_en_beton->noeud_debut->position.z;
                        fin_barre = sqrt(xx*xx+yy*yy+zz*zz);
                        l = ABS(fin_barre-debut_barre);
                        
                        // Définition des coefficient kA et kB. Pour rappel, kA et kB correspondent à l'inverse de la raideur.
                        // Pas de relachement possible
                        if (element_en_beton->relachement == NULL)
                        {
                            kAy = 0;
                            kBy = 0;
                            kAz = 0;
                            kBz = 0;
                        }
                        else
                        {
                            if (noeud_debut != element_en_beton->noeud_debut)
                            {
                                kAy = 0;
                                kAz = 0;
                            }
                            else
                            {
                                if (element_en_beton->relachement->ry_debut == EF_RELACHEMENT_BLOQUE)
                                    kAy = 0;
                                else if (element_en_beton->relachement->ry_debut == EF_RELACHEMENT_LIBRE)
                                    kAy = MAXDOUBLE;
                                else
                                    BUG(-1);
                                if (element_en_beton->relachement->rz_debut == EF_RELACHEMENT_BLOQUE)
                                    kAz = 0;
                                else if (element_en_beton->relachement->rz_debut == EF_RELACHEMENT_LIBRE)
                                    kAz = MAXDOUBLE;
                                else
                                    BUG(-1);
                            }
                            
                            if (noeud_fin != element_en_beton->noeud_fin)
                            {
                                kBy = 0;
                                kBz = 0;
                            }
                            else
                            {
                                if (element_en_beton->relachement->ry_fin == EF_RELACHEMENT_BLOQUE)
                                    kBy = 0;
                                else if (element_en_beton->relachement->ry_fin == EF_RELACHEMENT_LIBRE)
                                    kBy = MAXDOUBLE;
                                else
                                    BUG(-1);
                                if (element_en_beton->relachement->rz_fin == EF_RELACHEMENT_BLOQUE)
                                    kBz = 0;
                                else if (element_en_beton->relachement->rz_fin == EF_RELACHEMENT_LIBRE)
                                    kBz = MAXDOUBLE;
                                else
                                    BUG(-1);
                            }
                        }
                        
                        // Détermination de la rotation au noeud de la partie de la poutre discrétisée en la supposant isostatique
                        phiAy =  ax2[2]*position_charge_relative/(6*E*Iy*l)*(l-position_charge_relative)*(2*l-position_charge_relative)+ax2[4]/(6*E*Iy*l)*(l*l-3*(l-position_charge_relative)*(l-position_charge_relative));
                        phiBy = -ax2[2]*position_charge_relative/(6*E*Iy*l)*(l*l-position_charge_relative*position_charge_relative)+ax2[4]/(6*E*Iy*l)*(l*l-3*position_charge_relative*position_charge_relative);
                        phiAz =  ax2[1]*position_charge_relative/(6*E*Iz*l)*(l-position_charge_relative)*(2*l-position_charge_relative)-ax2[5]/(6*E*Iz*l)*(l*l-3*(l-position_charge_relative)*(l-position_charge_relative));
                        phiBz = -ax2[1]*position_charge_relative/(6*E*Iz*l)*(l*l-position_charge_relative*position_charge_relative)-ax2[5]/(6*E*Iz*l)*(l*l-3*position_charge_relative*position_charge_relative);
                        
                        // Calcul des paramètres de souplesse de la poutre
                        ay = l/(3*E*Iy);
                        by = l/(6*E*Iy);
                        cy = ay;
                        az = l/(3*E*Iz);
                        bz = l/(6*E*Iz);
                        cz = az;
                        
                        // Moments créés par les raideurs (encastrement)
                        if ((kAy == MAXDOUBLE) && (kBy == MAXDOUBLE))
                        {
                            MAy = 0.;
                            MBy = 0.;
                        }
                        else if (kAy == MAXDOUBLE)
                        {
                            MAy = 0.;
                            MBy = -phiBy/(cy+kBy);
                        }
                        else if (kBy == MAXDOUBLE)
                        {
                            MAy = -phiAy/(ay+kAy);
                            MBy = 0.;
                        }
                        else
                        {
                            MAy = -(by*phiBy+(cy+kBy)*phiAy)/((ay+kAy)*(cy+kBy)-by*by);
                            MBy = -(by*phiAy+(ay+kAy)*phiBy)/((ay+kAy)*(cy+kBy)-by*by);
                        }
                        if ((kAz == MAXDOUBLE) && (kBz == MAXDOUBLE))
                        {
                            MAz = 0.;
                            MBz = 0.;
                        }
                        else if (kAz == MAXDOUBLE)
                        {
                            MAz = 0.;
                            MBz = phiBz/(cz+kBz);
                        }
                        else if (kBz == MAXDOUBLE)
                        {
                            MAz = phiAz/(az+kAz);
                            MBz = 0.;
                        }
                        else
                        {
                            MAz = (bz*phiBz+(cz+kBz)*phiAz)/((az+kAz)*(cz+kBz)-bz*bz);
                            MBz = (bz*phiAz+(az+kAz)*phiBz)/((az+kAz)*(cz+kBz)-bz*bz);
                        }
                        
                        // Réaction d'appui sur les noeuds
                        FAx = ax2[0]*(l-position_charge_relative)/l;
                        FBx = ax2[0]*(position_charge_relative)/l;
                        FAy = ax2[1]*(l-position_charge_relative)/l-ax2[5]/l+(MBz+MAz)/l;
                        FBy = ax2[1]*position_charge_relative/l+ax2[5]/l-(MBz+MAz)/l;
                        FAz = ax2[2]*(l-position_charge_relative)/l+ax2[4]/l-(MBy+MAy)/l;
                        FBz = ax2[2]*position_charge_relative/l-ax2[4]/l+(MBy+MAy)/l;
                        
                        // Détermination des moments de rotation
                        if ((element_en_beton->relachement == NULL) || ((element_en_beton->relachement->rx_debut == EF_RELACHEMENT_BLOQUE) && (element_en_beton->relachement->rx_fin == EF_RELACHEMENT_BLOQUE)))
                        {
                            MAx = ax2[3]*(l-position_charge_relative)/l;
                            MBx = ax2[3]*(position_charge_relative)/l;
                        }
                        else if ((element_en_beton->relachement->rx_debut == EF_RELACHEMENT_LIBRE) && (element_en_beton->relachement->rx_fin == EF_RELACHEMENT_BLOQUE))
                        {
                            MAx = 0.;
                            MBx = ax2[3];
                        }
                        else if ((element_en_beton->relachement->rx_debut == EF_RELACHEMENT_BLOQUE) && (element_en_beton->relachement->rx_fin == EF_RELACHEMENT_LIBRE))
                        {
                            MAx = ax2[3];
                            MBx = 0.;
                        }
                        else
                            BUG(-1);
                        
                        // Détermination des fonctions des efforts
                        common_fonction_ajout(action_en_cours->fonctions_efforts[0][element_en_beton->numero],
                                              debut_barre,
                                              charge_barre->position,
                                              -FAx,
                                              0.,
                                              0.,
                                              0.,
                                              0.);
                        common_fonction_ajout(action_en_cours->fonctions_efforts[0][element_en_beton->numero],
                                              charge_barre->position,
                                              fin_barre,
                                              FBx,
                                              0.,
                                              0.,
                                              0.,
                                              0.);
                        common_fonction_ajout(action_en_cours->fonctions_efforts[1][element_en_beton->numero],
                                              debut_barre,
                                              charge_barre->position,
                                              -ax2[1]*(l-position_charge_relative)/l+ax2[5]/l,
                                              0.,
                                              0.,
                                              0.,
                                              0.);
                        common_fonction_ajout(action_en_cours->fonctions_efforts[1][element_en_beton->numero],
                                              charge_barre->position,
                                              fin_barre,
                                              ax2[1]*position_charge_relative/l+ax2[5]/l,
                                              0.,
                                              0.,
                                              0.,
                                              0.);
                        common_fonction_ajout(action_en_cours->fonctions_efforts[1][element_en_beton->numero],
                                              debut_barre,
                                              fin_barre,
                                              (-MAz-MBz)/l,
                                              0.,
                                              0.,
                                              0.,
                                              0.);
                        common_fonction_ajout(action_en_cours->fonctions_efforts[2][element_en_beton->numero],
                                              debut_barre,
                                              charge_barre->position,
                                              -ax2[2]*(l-position_charge_relative)/l-ax2[4]/l,
                                              0.,
                                              0.,
                                              0.,
                                              0.);
                        common_fonction_ajout(action_en_cours->fonctions_efforts[2][element_en_beton->numero],
                                              charge_barre->position,
                                              fin_barre,
                                              ax2[2]*position_charge_relative/l-ax2[4]/l,
                                              0.,
                                              0.,
                                              0.,
                                              0.);
                        common_fonction_ajout(action_en_cours->fonctions_efforts[2][element_en_beton->numero],
                                              debut_barre,
                                              fin_barre,
                                              (+MAy+MBy)/l,
                                              0.,
                                              0.,
                                              0.,
                                              0.);
                        common_fonction_ajout(action_en_cours->fonctions_efforts[3][element_en_beton->numero],
                                              debut_barre,
                                              charge_barre->position,
                                              -MAx,
                                              0.,
                                              0.,
                                              0.,
                                              0.);
                        common_fonction_ajout(action_en_cours->fonctions_efforts[3][element_en_beton->numero],
                                              charge_barre->position,
                                              fin_barre,
                                              MBx,
                                              0.,
                                              0.,
                                              0.,
                                              0.);
                        common_fonction_ajout(action_en_cours->fonctions_efforts[4][element_en_beton->numero],
                                              debut_barre,
                                              charge_barre->position,
                                              (ax2[2]*(l-position_charge_relative)/l-ax2[4]/l)*debut_barre,
                                              -ax2[2]*(l-position_charge_relative)/l-ax2[4]/l,
                                              0.,
                                              0.,
                                              0.);
                        common_fonction_ajout(action_en_cours->fonctions_efforts[4][element_en_beton->numero],
                                              charge_barre->position,
                                              fin_barre,
                                              (ax2[2]*(l-position_charge_relative)/l-ax2[4]/l)*debut_barre+charge_barre->position*(-ax2[2]*(l-position_charge_relative)/l-ax2[4]/l-(ax2[2]*position_charge_relative/l-ax2[4]/l)),
                                              ax2[2]*position_charge_relative/l-ax2[4]/l,
                                              0.,
                                              0.,
                                              0.);
                        common_fonction_ajout(action_en_cours->fonctions_efforts[4][element_en_beton->numero],
                                              debut_barre,
                                              fin_barre,
                                              -MAy-((MAy+MBy)/l)*debut_barre,
                                              (MAy+MBy)/l,
                                              0.,
                                              0.,
                                              0.);
                        common_fonction_ajout(action_en_cours->fonctions_efforts[5][element_en_beton->numero],
                                              debut_barre,
                                              charge_barre->position,
                                              -(ax2[1]*(l-position_charge_relative)/l-ax2[5]/l)*debut_barre,
                                              ax2[1]*(l-position_charge_relative)/l-ax2[5]/l,
                                              0.,
                                              0.,
                                              0.);
                        common_fonction_ajout(action_en_cours->fonctions_efforts[5][element_en_beton->numero],
                                              charge_barre->position,
                                              fin_barre,
                                              -(ax2[1]*(l-position_charge_relative)/l-ax2[5]/l)*debut_barre+charge_barre->position*(ax2[1]*(l-position_charge_relative)/l-ax2[5]/l-(-ax2[1]*position_charge_relative/l-ax2[5]/l)),
                                              -ax2[1]*position_charge_relative/l-ax2[5]/l,
                                              0.,
                                              0.,
                                              0.);
                        common_fonction_ajout(action_en_cours->fonctions_efforts[5][element_en_beton->numero],
                                              debut_barre,
                                              fin_barre,
                                              -MAz-((MAz+MBz)/l)*debut_barre,
                                              (MAz+MBz)/l,
                                              0.,
                                              0.,
                                              0.);
                        
                        cholmod_l_free_triplet(&triplet_efforts_locaux_initiaux, projet->ef_donnees.c);
                        
                        // Détermination des fonctions des rotations
                        common_fonction_ajout(action_en_cours->fonctions_rotation[2][element_en_beton->numero],
                                              debut_barre,
                                              charge_barre->position,
                                              ax2[1]*(l-position_charge_relative)/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l)*position_charge_relative*(l+l-position_charge_relative),
                                              0,
                                              -3*ax2[1]*(l-position_charge_relative)/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l),
                                              0.,
                                              0.);
                        common_fonction_ajout(action_en_cours->fonctions_rotation[2][element_en_beton->numero],
                                              charge_barre->position,
                                              fin_barre,
                                              ax2[1]*(position_charge_relative)/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l)*(2*l*l+position_charge_relative*position_charge_relative),
                                              -ax2[1]*(position_charge_relative)/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l)*6*l,
                                              ax2[1]*(position_charge_relative)/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l)*3,
                                              0.,
                                              0.);
                        common_fonction_ajout(action_en_cours->fonctions_rotation[2][element_en_beton->numero],
                                              debut_barre,
                                              fin_barre,
                                              -l/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz)*(2*MAz-MBz),
                                              -1/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz)*(-6)*MAz,
                                              -1/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz)*3*(MAz+MBz)/l,
                                              0,
                                              0.);
                        
                        // Détermination des fonctions des flèches
                        common_fonction_ajout(action_en_cours->fonctions_fleche[1][element_en_beton->numero],
                                              debut_barre,
                                              charge_barre->position,
                                              0.,
                                              ax2[1]*(l-position_charge_relative)/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l)*(l*l-(l-position_charge_relative)*(l-position_charge_relative)),
                                              0.,
                                              -ax2[1]*(l-position_charge_relative)/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l),
                                              debut_barre);
                        common_fonction_ajout(action_en_cours->fonctions_fleche[1][element_en_beton->numero],
                                              charge_barre->position,
                                              fin_barre,
                                              -ax2[1]*position_charge_relative*position_charge_relative*position_charge_relative/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz),
                                              ax2[1]*position_charge_relative/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l)*(position_charge_relative*position_charge_relative+2*l*l),
                                              -ax2[1]*position_charge_relative/(2*element_en_beton->materiau->ecm*section->caracteristiques->iz),
                                              ax2[1]*position_charge_relative/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l),
                                              debut_barre);
                        common_fonction_ajout(action_en_cours->fonctions_fleche[1][element_en_beton->numero],
                                              debut_barre,
                                              fin_barre,
                                              0.,
                                              l/(element_en_beton->materiau->ecm*section->caracteristiques->iz)*(-MAz/3-MBz/6),
                                              MAz/(2*element_en_beton->materiau->ecm*section->caracteristiques->iz),
                                              (MBz-MAz)/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l),
                                              debut_barre);
                                              
                        // Ensuite, on converti dans le repère global
                        triplet_efforts_locaux_finaux = cholmod_l_allocate_triplet(12, 1, 12, 0, CHOLMOD_REAL, projet->ef_donnees.c);
                        ai2 = triplet_efforts_locaux_finaux->i;
                        aj2 = triplet_efforts_locaux_finaux->j;
                        ax2 = triplet_efforts_locaux_finaux->x;
                        triplet_efforts_locaux_finaux->nnz = 12;
                        ai2[0] = 0; aj2[0] = 0; ax2[0] = FAx;
                        ai2[1] = 1; aj2[1] = 0; ax2[1] = FAy;
                        ai2[2] = 2; aj2[2] = 0; ax2[2] = FAz;
                        ai2[3] = 3; aj2[3] = 0; ax2[3] = MAx;
                        ai2[4] = 4; aj2[4] = 0; ax2[4] = MAy;
                        ai2[5] = 5; aj2[5] = 0; ax2[5] = MAz;
                        ai2[6] = 6; aj2[6] = 0; ax2[6] = FBx;
                        ai2[7] = 7; aj2[7] = 0; ax2[7] = FBy;
                        ai2[8] = 8; aj2[8] = 0; ax2[8] = FBz;
                        ai2[9] = 9; aj2[9] = 0; ax2[9] = MBx;
                        ai2[10] = 10;   aj2[10] = 0;    ax2[10] = MBy;
                        ai2[11] = 11;   aj2[11] = 0;    ax2[11] = MBz;
                        sparse_efforts_locaux_finaux = cholmod_l_triplet_to_sparse(triplet_efforts_locaux_finaux, 0, projet->ef_donnees.c);
                        cholmod_l_free_triplet(&triplet_efforts_locaux_finaux, projet->ef_donnees.c);
                        sparse_efforts_globaux_finaux = cholmod_l_ssmult(element_en_beton->matrice_rotation, sparse_efforts_locaux_finaux, 0, 1, 0, projet->ef_donnees.c);
                        cholmod_l_free_sparse(&(sparse_efforts_locaux_finaux), projet->ef_donnees.c);
                        triplet_efforts_globaux_finaux = cholmod_l_sparse_to_triplet(sparse_efforts_globaux_finaux, projet->ef_donnees.c);
                        ai2 = triplet_efforts_globaux_finaux->i;
                        aj2 = triplet_efforts_globaux_finaux->j;
                        ax2 = triplet_efforts_globaux_finaux->x;
                        cholmod_l_free_sparse(&(sparse_efforts_globaux_finaux), projet->ef_donnees.c);
                        
                        // On ajoute les moments et les efforts dans le vecteur de force
                        for (i=0;i<12;i++)
                        {
                            if (ai2[i] < 6)
                            {
                                if (projet->ef_donnees.noeuds_flags_partielle[noeud_debut->numero][ai2[i]] != -1)
                                    ax[projet->ef_donnees.noeuds_flags_partielle[noeud_debut->numero][ai2[i]]] += ax2[i];
                                ax3[projet->ef_donnees.noeuds_flags_complete[noeud_debut->numero][ai2[i]]] += ax2[i];
                            }
                            else
                            {
                                if (projet->ef_donnees.noeuds_flags_partielle[noeud_fin->numero][ai2[i]-6] != -1)
                                    ax[projet->ef_donnees.noeuds_flags_partielle[noeud_fin->numero][ai2[i]-6]] += ax2[i];
                                ax3[projet->ef_donnees.noeuds_flags_complete[noeud_fin->numero][ai2[i]-6]] += ax2[i];
                            }
                        }
                        cholmod_l_free_triplet(&triplet_efforts_globaux_finaux, projet->ef_donnees.c);
                    }
                    // Charge inconnue
                    else
                        BUG(-1);
                }
                // Type d'élément inconnu
                else
                    BUG(-1);
            }
        }
        while (list_mvnext(action_en_cours->charges) != NULL);
    }
    
    // On converti les données dans des structures permettant les calculs via les libraries
    sparse_force = cholmod_l_triplet_to_sparse(triplet_force_partielle, 0, projet->ef_donnees.c);
    cholmod_l_free_triplet(&triplet_force_partielle, projet->ef_donnees.c);
    dense_force = cholmod_l_sparse_to_dense(sparse_force, projet->ef_donnees.c);
    cholmod_l_free_sparse(&sparse_force, projet->ef_donnees.c);
    action_en_cours->forces_complet = cholmod_l_triplet_to_sparse(triplet_force_complete, 0, projet->ef_donnees.c);
    cholmod_l_free_triplet(&triplet_force_complete, projet->ef_donnees.c);
    
//  Pour utiliser cholmod dans les calculs de matrices.
/*  action_en_cours->deplacement_partiel = cholmod_l_spsolve (CHOLMOD_A, projet->ef_donnees.factor_rigidite_matrice_partielle, sparse_force, projet->ef_donnees.c);
    cholmod_sparse *r = cholmod_l_copy_sparse(sparse_force, projet->ef_donnees.c);
    cholmod_l_ssmult(projet->ef_donnees.rigidite_matrice_partielle, action_en_cours->deplacement_partiel, 0, TRUE, TRUE, projet->ef_donnees.c);
    action_en_cours->norm = cholmod_l_norm_sparse(r, 0, projet->ef_donnees.c);
    printf("résidu : %f\n", action_en_cours->norm);
    cholmod_l_free_sparse(&r, projet->ef_donnees.c);
    cholmod_l_write_sparse(stdout, action_en_cours->deplacement_partiel, NULL, NULL, projet->ef_donnees.c);*/
    
    // On résoud le système
    cholmod_dense *Y = SuiteSparseQR_C_qmult(SPQR_QTX, projet->ef_donnees.QR, dense_force, projet->ef_donnees.c);
    cholmod_dense *X = SuiteSparseQR_C_solve(SPQR_RX_EQUALS_B, projet->ef_donnees.QR, Y, projet->ef_donnees.c);
    action_en_cours->deplacement_partiel = cholmod_l_dense_to_sparse(X, TRUE, projet->ef_donnees.c);
    
    // On crée le vecteur déplacement complet
    triplet_deplacements_partiels = cholmod_l_sparse_to_triplet(action_en_cours->deplacement_partiel, projet->ef_donnees.c);
    ai = triplet_deplacements_partiels->i;
    aj = triplet_deplacements_partiels->j;
    ax = triplet_deplacements_partiels->x;
    triplet_deplacements_totaux = cholmod_l_allocate_triplet(projet->ef_donnees.nb_colonne_matrice_complete, 1, projet->ef_donnees.nb_colonne_matrice_complete, 0, CHOLMOD_REAL, projet->ef_donnees.c);
    triplet_deplacements_totaux->nnz = projet->ef_donnees.nb_colonne_matrice_complete;
    ai2 = triplet_deplacements_totaux->i;
    aj2 = triplet_deplacements_totaux->j;
    ax2 = triplet_deplacements_totaux->x;
    j = 0;
    for (i=0;i<list_size(projet->ef_donnees.noeuds);i++)
    {
        ai2[i*6] = i*6; aj2[i*6] = 0;
        if (projet->ef_donnees.noeuds_flags_partielle[i][0] == -1)
            ax2[i*6] = 0.;
        else
        {
            if (ai[j] == projet->ef_donnees.noeuds_flags_partielle[i][0])
            {
                ax2[i*6] = ax[j];
                j++;
            }
            else
                ax2[i*6] = 0.;
        }
        ai2[i*6+1] = i*6+1; aj2[i*6+1] = 0;
        if (projet->ef_donnees.noeuds_flags_partielle[i][1] == -1)
            ax2[i*6+1] = 0.;
        else
        {
            if (ai[j] == projet->ef_donnees.noeuds_flags_partielle[i][1])
            {
                ax2[i*6+1] = ax[j];
                j++;
            }
            else
                ax2[i*6+1] = 0.;
        }
        ai2[i*6+2] = i*6+2; aj2[i*6+2] = 0;
        if (projet->ef_donnees.noeuds_flags_partielle[i][2] == -1)
            ax2[i*6+2] = 0.;
        else
        {
            if (ai[j] == projet->ef_donnees.noeuds_flags_partielle[i][2])
            {
                ax2[i*6+2] = ax[j];
                j++;
            }
            else
                ax2[i*6+2] = 0.;
        }
        ai2[i*6+3] = i*6+3; aj2[i*6+3] = 0;
        if (projet->ef_donnees.noeuds_flags_partielle[i][3] == -1)
            ax2[i*6+3] = 0.;
        else
        {
            if (ai[j] == projet->ef_donnees.noeuds_flags_partielle[i][3])
            {
                ax2[i*6+3] = ax[j];
                j++;
            }
            else
                ax2[i*6+3] = 0.;
        }
        ai2[i*6+4] = i*6+4; aj2[i*6+4] = 0;
        if (projet->ef_donnees.noeuds_flags_partielle[i][4] == -1)
            ax2[i*6+4] = 0.;
        else
        {
            if (ai[j] == projet->ef_donnees.noeuds_flags_partielle[i][4])
            {
                ax2[i*6+4] = ax[j];
                j++;
            }
            else
                ax2[i*6+4] = 0.;
        }
        ai2[i*6+5] = i*6+5; aj2[i*6+5] = 0;
        if (projet->ef_donnees.noeuds_flags_partielle[i][5] == -1)
            ax2[i*6+5] = 0.;
        else
        {
            if (ai[j] == projet->ef_donnees.noeuds_flags_partielle[i][5])
            {
                ax2[i*6+5] = ax[j];
                j++;
            }
            else
                ax2[i*6+5] = 0.;
        }
    }
    cholmod_l_free_triplet(&triplet_deplacements_partiels, projet->ef_donnees.c);
    action_en_cours->deplacement_complet = cholmod_l_triplet_to_sparse(triplet_deplacements_totaux, 0, projet->ef_donnees.c);
    
    // On calcul le résidu. Méthode trouvée dans le fichier cholmod_demo.c de la source de la librairie cholmod.
    cholmod_dense *r = cholmod_l_copy_dense(dense_force, projet->ef_donnees.c);
    double minusone[2] = {-1., 0.}, one[2] = {1., 0.};
    cholmod_l_sdmult(projet->ef_donnees.rigidite_matrice_partielle, 0, minusone, one, X, r, projet->ef_donnees.c);
    double bnorm = cholmod_l_norm_dense(dense_force, 0, projet->ef_donnees.c);
    double rnorm = cholmod_l_norm_dense(r, 0, projet->ef_donnees.c);
    double xnorm = cholmod_l_norm_dense(X, 0, projet->ef_donnees.c);
    double anorm = cholmod_l_norm_sparse(projet->ef_donnees.rigidite_matrice_partielle, 0, projet->ef_donnees.c);
    double axbnorm = (anorm * xnorm + bnorm) ;
    action_en_cours->norm = rnorm / axbnorm ;
    printf("résidu : %e\n", action_en_cours->norm);
    
    // On calcule les efforts dans tous les noeuds, y compris les réactions d'appuis.
    action_en_cours->efforts_noeuds = cholmod_l_ssmult(projet->ef_donnees.rigidite_matrice_complete, action_en_cours->deplacement_complet, 0, TRUE, TRUE, projet->ef_donnees.c);
    max_effort = 0.;
    ax = action_en_cours->efforts_noeuds->x;
    for (j=0;j<action_en_cours->efforts_noeuds->nzmax;j++)
    {
        if (ABS(ax[j]) > max_effort)
            max_effort = ABS(ax[j]);
    }
    cholmod_l_drop(max_effort*ERREUR_RELATIVE_MIN, action_en_cours->efforts_noeuds, projet->ef_donnees.c);
    
    // On libère la mémoire
    cholmod_l_free_dense(&Y, projet->ef_donnees.c);
    cholmod_l_free_dense(&X, projet->ef_donnees.c);
    cholmod_l_free_dense(&r, projet->ef_donnees.c);
    cholmod_l_free_sparse(&sparse_force, projet->ef_donnees.c);
    cholmod_l_free_dense(&dense_force, projet->ef_donnees.c);
    
//  Pour utiliser cholmod dans les calculs de matrices.
//  Troisième méthode de calcul donnant directement les calculs sans passer par une matrice intermédiaire.
//  Est moins intéressant puisqu'il faut résoudre l'intégralité du système pour chaque cas de charge.
//  projet->ef_donnees.rigidite_matrice_partielle->stype = 0;
//  cholmod_sparse *tttt = SuiteSparseQR_C_backslash_sparse(0, 0., projet->ef_donnees.rigidite_matrice_partielle, sparse_force, projet->ef_donnees.c);
//  printf("déplacement 5\n");
//  cholmod_l_write_sparse(stdout, tttt, NULL, NULL, projet->ef_donnees.c);*/
    
    // On parcours tous les éléments et on ajoute les efforts dus aux déplacements.
    list_mvfront(projet->beton.elements);
    do
    {
        Beton_Element       *element_en_beton = list_curr(projet->beton.elements);
        cholmod_triplet     *triplet_deplacement_globaux, *triplet_deplacement_locaux;
        cholmod_sparse      *sparse_deplacement_globaux, *sparse_deplacement_locaux;
        double          xx, yy, zz, l;
        Beton_Section_Rectangulaire *section;
        double          MA, MB;
        
        for (j=0;j<=element_en_beton->discretisation_element;j++)
        {
            EF_Noeud    *noeud_debut, *noeud_fin;   // Le noeud de départ et le noeud de fin, nécessaire en cas de discrétisation
            double      l_debut, l_fin, l_total;
            if (j == 0)
                noeud_debut = element_en_beton->noeud_debut;
            else
                noeud_debut = element_en_beton->noeuds_intermediaires[j-1];
            if (j==element_en_beton->discretisation_element)
                noeud_fin = element_en_beton->noeud_fin;
            else
                noeud_fin = element_en_beton->noeuds_intermediaires[j];
            triplet_deplacement_globaux = cholmod_l_allocate_triplet(12, 1, 12, 0, CHOLMOD_REAL, projet->ef_donnees.c);
            ai = triplet_deplacement_globaux->i;
            aj = triplet_deplacement_globaux->j;
            ax = triplet_deplacement_globaux->x;
            triplet_deplacement_globaux->nnz = 12;
            ax2 = triplet_deplacements_totaux->x;
            for (i=0;i<6;i++)
            {
                ai[i] = i;
                aj[i] = 0;
                ax[i] = ax2[projet->ef_donnees.noeuds_flags_complete[noeud_debut->numero][i]];
            }
            for (i=0;i<6;i++)
            {
                ai[i+6] = i+6;
                aj[i+6] = 0;
                ax[i+6] = ax2[projet->ef_donnees.noeuds_flags_complete[noeud_fin->numero][i]];
            }
            sparse_deplacement_globaux = cholmod_l_triplet_to_sparse(triplet_deplacement_globaux, 0, projet->ef_donnees.c);
            cholmod_l_free_triplet(&triplet_deplacement_globaux, projet->ef_donnees.c);
            sparse_deplacement_locaux = cholmod_l_ssmult(element_en_beton->matrice_rotation_transpose, sparse_deplacement_globaux, 0, 1, 0, projet->ef_donnees.c);
            cholmod_l_free_sparse(&sparse_deplacement_globaux, projet->ef_donnees.c);
            triplet_deplacement_locaux = cholmod_l_sparse_to_triplet(sparse_deplacement_locaux, projet->ef_donnees.c);
            cholmod_l_free_sparse(&sparse_deplacement_locaux, projet->ef_donnees.c);
            section = element_en_beton->section;
            ax = triplet_deplacement_locaux->x;
            xx = noeud_debut->position.x - element_en_beton->noeud_debut->position.x;
            yy = noeud_debut->position.y - element_en_beton->noeud_debut->position.y;
            zz = noeud_debut->position.z - element_en_beton->noeud_debut->position.z;
            l_debut = sqrt(xx*xx+yy*yy+zz*zz);
            xx = element_en_beton->noeud_fin->position.x - element_en_beton->noeud_debut->position.x;
            yy = element_en_beton->noeud_fin->position.y - element_en_beton->noeud_debut->position.y;
            zz = element_en_beton->noeud_fin->position.z - element_en_beton->noeud_debut->position.z;
            l_total = sqrt(xx*xx+yy*yy+zz*zz);
            xx = noeud_fin->position.x - element_en_beton->noeud_debut->position.x;
            yy = noeud_fin->position.y - element_en_beton->noeud_debut->position.y;
            zz = noeud_fin->position.z - element_en_beton->noeud_debut->position.z;
            l_fin = sqrt(xx*xx+yy*yy+zz*zz);
            l = ABS(l_fin-l_debut);
            common_fonction_ajout(action_en_cours->fonctions_efforts[0][element_en_beton->numero], l_debut, l_fin, element_en_beton->materiau->ecm*section->caracteristiques->a*(ax[0]-ax[6])/l, 0., 0., 0., 0.);
            MA = element_en_beton->materiau->ecm*section->caracteristiques->iz*(12*ax[1]/l/l/l-12*ax[7]/l/l/l+6*ax[5]/l/l+6*ax[11]/l/l);
            MB = -element_en_beton->materiau->ecm*section->caracteristiques->iz*(-12*ax[1]/l/l/l+12*ax[7]/l/l/l-6*ax[5]/l/l-6*ax[11]/l/l);
            common_fonction_ajout(action_en_cours->fonctions_efforts[1][element_en_beton->numero], l_debut, l_fin, MA, (-MA+MB)/l, 0., 0., 0.);
            MA = element_en_beton->materiau->ecm*section->caracteristiques->iy*(12*ax[2]/l/l/l-12*ax[8]/l/l/l-6*ax[4]/l/l-6*ax[10]/l/l);
            MB = -element_en_beton->materiau->ecm*section->caracteristiques->iy*(-12*ax[2]/l/l/l+12*ax[8]/l/l/l+6*ax[4]/l/l+6*ax[10]/l/l);
            common_fonction_ajout(action_en_cours->fonctions_efforts[2][element_en_beton->numero], l_debut, l_fin, MA, (-MA+MB)/l, 0., 0., 0.);
            common_fonction_ajout(action_en_cours->fonctions_efforts[3][element_en_beton->numero], l_debut, l_fin, element_en_beton->materiau->gnu_0_2*section->caracteristiques->j*(ax[3]-ax[9])/l, 0., 0., 0., 0.);
            MA = element_en_beton->materiau->ecm*section->caracteristiques->iy*(-6*ax[2]/l/l+6*ax[8]/l/l+4*ax[4]/l+2*ax[10]/l);
            MB = -element_en_beton->materiau->ecm*section->caracteristiques->iy*(-6*ax[2]/l/l+6*ax[8]/l/l+2*ax[4]/l+4*ax[10]/l);
            common_fonction_ajout(action_en_cours->fonctions_efforts[4][element_en_beton->numero], l_debut, l_fin, MA-(-MA+MB)/l*l_debut, (-MA+MB)/l, 0., 0., 0.);
            MA = element_en_beton->materiau->ecm*section->caracteristiques->iz*(6*ax[1]/l/l-6*ax[7]/l/l+4*ax[5]/l+2*ax[11]/l);
            MB = -element_en_beton->materiau->ecm*section->caracteristiques->iz*(6*ax[1]/l/l-6*ax[7]/l/l+2*ax[5]/l+4*ax[11]/l);
            common_fonction_ajout(action_en_cours->fonctions_efforts[5][element_en_beton->numero], l_debut, l_fin, MA-(-MA+MB)/l*l_debut, (-MA+MB)/l, 0., 0., 0.);
            common_fonction_ajout(action_en_cours->fonctions_rotation[2][element_en_beton->numero],
                                  l_debut,
                                  l_fin,
                                  MA/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l)*2*l*l,
                                  MA/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l)*-6*l,
                                  MA/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l)*3,
                                  0,
                                  0.);
            common_fonction_ajout(action_en_cours->fonctions_rotation[2][element_en_beton->numero],
                                  l_debut,
                                  l_fin,
                                  MB/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l)*l*l,
                                  0.,
                                  -MB/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l)*3,
                                  0,
                                  0.);
/*          common_fonction_ajout(action_en_cours->fonctions_fleche[1][element_en_beton->numero],
                                  l_debut,
                                  l_fin,
                                  0.,
                                  MA*l/(3*element_en_beton->materiau->ecm*section->caracteristiques->iz),
                                  -MA/(2*element_en_beton->materiau->ecm*section->caracteristiques->iz),
                                  MA/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l));
            common_fonction_ajout(action_en_cours->fonctions_fleche[1][element_en_beton->numero],
                                  l_debut,
                                  l_fin,
                                  0.,
                                  -MB*l/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz),
                                  0.,
                                  MB/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l));*/
            common_fonction_ajout(action_en_cours->fonctions_fleche[1][element_en_beton->numero],
                                  l_debut,
                                  l_fin,
                                  -MA*l/(3*element_en_beton->materiau->ecm*section->caracteristiques->iz)*l_debut+-MA/(2*element_en_beton->materiau->ecm*section->caracteristiques->iz)*l_debut*l_debut-MA/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l)*l_debut*l_debut*l_debut,
                                  MA*l/(3*element_en_beton->materiau->ecm*section->caracteristiques->iz)-2*-MA/(2*element_en_beton->materiau->ecm*section->caracteristiques->iz)*l_debut+3*MA/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l)*l_debut*l_debut,
                                  -MA/(2*element_en_beton->materiau->ecm*section->caracteristiques->iz)-3*MA/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l)*l_debut,
                                  MA/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l),
                                  0.);
            common_fonction_ajout(action_en_cours->fonctions_fleche[1][element_en_beton->numero],
                                  l_debut,
                                  l_fin,
                                  MB*l/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz)*l_debut-MB/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l)*l_debut*l_debut*l_debut,
                                  -MB*l/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz)+3*MB/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l)*l_debut*l_debut,
                                  -3*MB/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l)*l_debut,
                                  MB/(6*element_en_beton->materiau->ecm*section->caracteristiques->iz*l),
                                  0.);
            common_fonction_ajout(action_en_cours->fonctions_fleche[1][element_en_beton->numero],
                                  l_debut,
                                  l_fin,
                                  (l_debut*ax[7]-l_fin*ax[1])/(l_debut-l_fin),
                                  (ax[1]-ax[7])/(l_debut-l_fin),
                                  0.,
                                  0.,
                                  0.);
            cholmod_l_free_triplet(&triplet_deplacement_locaux, projet->ef_donnees.c);
        }
    }
    while (list_mvnext(projet->beton.elements) != NULL);
    cholmod_l_free_triplet(&triplet_deplacements_totaux, projet->ef_donnees.c);
    
    return 0;
}


/* EF_calculs_affiche_resultats
 * Description : Affiche tous les résultats d'une action
 * Paramètres : Projet *projet : la variable projet
 *            : int num_action : numéro de l'action
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative
 */
int EF_calculs_affiche_resultats(Projet *projet, int num_action)
{
    Action          *action_en_cours;
    unsigned int        i;
    
    if ((projet == NULL) || (projet->actions == NULL) || (list_size(projet->actions) == 0) || (_1990_action_cherche_numero(projet, num_action) != 0))
        BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
    
    // On crée la vecteur sparse contenant les actions extérieures
    // On commence par initialiser les vecteurs "force partielle" et "force complet" par des 0.
    action_en_cours = list_curr(projet->actions);
    printf("Effort aux noeuds & Réactions d'appuis\n");
    common_math_arrondi_sparse(action_en_cours->efforts_noeuds);
    cholmod_l_write_sparse(stdout, action_en_cours->efforts_noeuds, NULL, NULL, projet->ef_donnees.c);
    printf("Déplacements\n");
    common_math_arrondi_sparse(action_en_cours->deplacement_complet);
    cholmod_l_write_sparse(stdout, action_en_cours->deplacement_complet, NULL, NULL, projet->ef_donnees.c);
    if (list_size(projet->beton.elements) != 0)
    {
        for (i=0;i<list_size(projet->beton.elements);i++)
        {
            printf("Barre n°%d, Effort normal\n", i);
            common_fonction_affiche(action_en_cours->fonctions_efforts[0][i]);
            printf("Barre n°%d, Effort Tranchant Y\n", i);
            common_fonction_affiche(action_en_cours->fonctions_efforts[1][i]);
            printf("Barre n°%d, Effort Tranchant Z\n", i);
            common_fonction_affiche(action_en_cours->fonctions_efforts[2][i]);
            printf("Barre n°%d, Moment de torsion\n", i);
            common_fonction_affiche(action_en_cours->fonctions_efforts[3][i]);
            printf("Barre n°%d, Moment de flexion Y\n", i);
            common_fonction_affiche(action_en_cours->fonctions_efforts[4][i]);
            printf("Barre n°%d, Moment de flexion Z\n", i);
            common_fonction_affiche(action_en_cours->fonctions_efforts[5][i]);
        }
    }
    if (list_size(projet->beton.elements) != 0)
    {
        for (i=0;i<list_size(projet->beton.elements);i++)
        {
//          printf("Barre n°%d, Flèche en X\n", i);
//          common_fonction_affiche(action_en_cours->fonctions_fleche[0][i]);
            printf("Barre n°%d, Flèche en Y\n", i);
            common_fonction_affiche(action_en_cours->fonctions_fleche[1][i]);
//          printf("Barre n°%d, Flèche en Z\n", i);
//          common_fonction_affiche(action_en_cours->fonctions_fleche[2][i]);
//          printf("Barre n°%d, Rotation en X\n", i);
//          common_fonction_affiche(action_en_cours->fonctions_rotation[0][i]);
//          printf("Barre n°%d, Rotation en Y\n", i);
//          common_fonction_affiche(action_en_cours->fonctions_rotation[1][i]);
            printf("Barre n°%d, Rotation en Z\n", i);
            common_fonction_affiche(action_en_cours->fonctions_rotation[2][i]);
        }
    }
    
    return 0;
}
