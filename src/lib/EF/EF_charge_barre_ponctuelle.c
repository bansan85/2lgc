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
#include <stdio.h>
#include <stdlib.h>
#include <cholmod.h>

#include "1990_actions.h"
#include "1992_1_1_section.h"
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_maths.h"
#include "common_fonction.h"
#include "EF_noeud.h"
#include "EF_charge_barre_ponctuelle.h"


int EF_charge_barre_ponctuelle_ajout(Projet *projet, int num_action, Beton_Barre *barre, 
  int repere_local, double a, double fx, double fy, double fz, double mx, double my,
  double mz)
/* Description : Ajoute une charge ponctuelle à une action et à l'intérieur d'une barre en lui
 *               attribuant le numéro suivant la dernière charge de l'action.
 * Paramètres : Projet *projet : la variable projet,
 *            : int num_action : numero de l'action qui contiendra la charge,
 *            : void *barre : barre qui supportera la charge,
 *            : int repere_local : TRUE si les charges doivent être prise dans le repère local,
 *                                 FALSE pour le repère global,
 *            : double a : position en mètre de la charge par rapport au début de la barre,
 *            : double fx : force suivant l'axe x,
 *            : double fy : force suivant l'axe y,
 *            : double fz : force suivant l'axe z,
 *            : double mx : moment autour de l'axe x,
 *            : double my : moment autour de l'axe y,
 *            : double mz : moment autour de l'axe z.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (projet == NULL) ou
 *             (projet->actions == NULL) ou
 *             (list_size(projet->actions) == 0) ou
 *             (barre == NULL) ou
 *             (_1990_action_cherche_numero(projet, num_action) != 0) ou
 *             (a < 0) ou (a > l)
 *           -2 en cas d'erreur d'allocation mémoire
 */
{
    Action          *action_en_cours;
    Charge_Barre_Ponctuelle *charge_dernier, charge_nouveau;
    
    // Trivial
    BUGMSG(projet, -1, "EF_charge_barre_ponctuelle_ajout\n");
    BUGMSG(projet->actions, -1, "EF_charge_barre_ponctuelle_ajout\n");
    BUGMSG(list_size(projet->actions), -1, "EF_charge_barre_ponctuelle_ajout\n");
    BUGMSG(barre, -1, "EF_charge_barre_ponctuelle_ajout\n");
    BUG(_1990_action_cherche_numero(projet, num_action) == 0, -1);
    action_en_cours = (Action*)list_curr(projet->actions);
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), -1, "EF_charge_barre_ponctuelle_ajout");
    BUGMSG(!((a > EF_noeuds_distance(barre->noeud_debut, barre->noeud_fin)) && (!(ERREUR_RELATIVE_EGALE(a, EF_noeuds_distance(barre->noeud_debut, barre->noeud_fin))))), -1, "EF_charge_barre_ponctuelle_ajout");
    
    charge_nouveau.type = CHARGE_BARRE_PONCTUELLE;
    charge_nouveau.nom = NULL;
    charge_nouveau.description = NULL;
    charge_nouveau.barre = barre;
    charge_nouveau.repere_local = repere_local;
    charge_nouveau.position = a;
    charge_nouveau.fx = fx;
    charge_nouveau.fy = fy;
    charge_nouveau.fz = fz;
    charge_nouveau.mx = mx;
    charge_nouveau.my = my;
    charge_nouveau.mz = mz;
    
    charge_dernier = (Charge_Barre_Ponctuelle *)list_rear(action_en_cours->charges);
    if (charge_dernier == NULL)
        charge_nouveau.numero = 0;
    else
        charge_nouveau.numero = charge_dernier->numero+1;
    
    list_mvrear(action_en_cours->charges);
    BUGMSG(list_insert_after(action_en_cours->charges, &(charge_nouveau), sizeof(charge_nouveau)), -2, gettext("%s : Erreur d'allocation mémoire.\n"), "EF_charge_barre_ponctuelle_ajout");
    
    return 0;
}


int EF_charge_barre_ponctuelle_mx(Beton_Barre *barre, unsigned int discretisation,
  double a, Barre_Info_EF *infos, double mx, double *ma, double *mb)
/* Description : Calcule l'opposé aux moments d'encastrement pour l'élément spécifié soumis
 *               au moment de torsion mx dans le repère local. Les résultats sont renvoyés
 *               par l'intermédiaire des pointeurs ma et mb qui ne peuvent être NULL.
 * Paramètres : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de barre à
 *                         étudier,
 *            : Barre_Info_EF *infos : infos de la partie de barre concerné,
 *            : double mx : moment autour de l'axe x,
 *            : double *ma : pointeur qui contiendra le moment au début de la barre,
 *            : double *mb : pointeur qui contiendra le moment à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (barre == NULL) ou
 *             (infos == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau == 0) ou
 *             (barre->noeud_debut == NULL) ou
 *             (barre->noeud_fin == NULL) ou
 *             (discretisation>barre->discretisation_element) ou
 *             (ma == NULL) ou
 *             (mb == NULL) ou
 *             (kAx == kBx == MAXDOUBLE) ou
 *             (a < 0.) ou (a > l)
 */
{
    EF_Noeud    *debut, *fin;
    double      l, G;

    BUGMSG(barre, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(infos, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(ma, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(mb, -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(!((ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE)) && (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))), -1, "EF_charge_barre_ponctuelle_mx\n");
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), -1, "EF_charge_barre_ponctuelle_mx");
    
    // Les moments aux extrémités de la barre sont déterminés par les intégrales de Mohr
    //   et valent dans le cas général :\end{verbatim}\begin{center}
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_mx.pdf}\end{center}
    // \begin{align*}
    // M_{Bx} = & \frac{\int_0^a \frac{1}{J(x) \cdot G}dx + k_A}{\int_0^L \frac{1}{J(x) \cdot G} dx + k_A + k_B} \cdot M_x\nonumber\\
    // M_{Ax} = & M_x - M_{Bx}\end{align*}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    l = EF_noeuds_distance(debut, fin);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), -1, "EF_charge_barre_ponctuelle_mx");
    
    G = barre->materiau->gnu_0_2;
    
    switch(((Beton_Section_Rectangulaire*)(barre->section))->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      J = _1992_1_1_sections_j(barre->section);
            
    // Pour une section section constante, les moments valent :\end{verbatim}\begin{displaymath}
    // M_{Bx} = \frac{\frac{a}{G \cdot J} +k_{Ax}}{\frac{L}{G \cdot J} +k_{Ax}+k_{Bx}} \cdot M_x\end{displaymath}\begin{verbatim}
            if (ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE))
                *mb = mx;
            else if (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))
                *mb = 0.;
            else
                *mb = (a/(G*J)+infos->kAx)/(l/(G*J)+infos->kAx+infos->kBx)*mx;
            *ma = mx - *mb;
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0., "EF_charge_barre_ponctuelle_mx\n");
            break;
        }
    }
}


int EF_charge_barre_ponctuelle_def_ang_iso_y(Beton_Barre *barre, unsigned int discretisation,
  double a, double fz, double my, double *phia, double *phib)
/* Description : Calcule les angles de rotation autour de l'axe y pour un élément bi-articulé
 *               soumis au chargement fz, my dans le repère local. Les résultats sont renvoyés
 *               par l'intermédiaire des pointeurs phia et phib qui ne peuvent être NULL.
 * Paramètres : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de
 *                         barre à étudier,
 *            : double fz : force suivant l'axe z
 *            : double my : moment autour de l'axe y
 *            : double *phia : pointeur qui contiendra l'angle au début de la barre
 *            : double *phib : pointeur qui contiendra l'angle à la fin de la barre
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau == 0) ou
 *             (barre->noeud_debut == NULL) ou
 *             (barre->noeud_fin == NULL) ou
 *             (discretisation>barre->discretisation_element) ou
 *             (phia == NULL) ou
 *             (phib == NULL) ou
 *             (a < 0.) ou (a > l)
 */
{
    EF_Noeud    *debut, *fin;
    double      l, b, E;

    BUGMSG(barre, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(phia, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(phib, -1, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), -1, "EF_charge_barre_ponctuelle_def_ang_iso_y");
    
    // Les rotations aux extrémités de la barre sont déterminées par les intégrales de Mohr
    //   et valent dans le cas général :\end{verbatim}\begin{center}
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_fz.pdf}\includegraphics[width=8cm]{images/charge_barre_ponctuelle_my.pdf}\par
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_phiay.pdf}\includegraphics[width=8cm]{images/charge_barre_ponctuelle_phiby.pdf}\end{center}\begin{align*}
    // \varphi_A = & \int_0^a \frac{Mf_{11} \cdot Mf_0}{E \cdot I_y(x)} + \int_a^L \frac{Mf_{12} \cdot Mf_0}{E \cdot I_y(x)} + \int_0^a \frac{Mf_{21} \cdot Mf_0}{E \cdot I_y(x)} + \int_a^L \frac{Mf_{22} \cdot Mf_0}{E \cdot I_y(x)} \nonumber\\
    // \varphi_B = & \int_0^a \frac{Mf_{11} \cdot Mf_3}{E \cdot I_y(x)} + \int_a^L \frac{Mf_{12} \cdot Mf_3}{E \cdot I_y(x)} + \int_0^a \frac{Mf_{21} \cdot Mf_3}{E \cdot I_y(x)} + \int_a^L \frac{Mf_{22} \cdot Mf_3}{E \cdot I_y(x)}\end{align*}\begin{align*}
    // \texttt{avec :} Mf_0 = &\frac{L-x}{L} & Mf_{11} = &-\frac{F_z \cdot b \cdot x}{L}\nonumber\\
    //                 Mf_{12} = & -\frac{F_z \cdot a \cdot (L-x)}{L} & Mf_{21} = & -\frac{M_y \cdot x}{L}\nonumber\\
    //                 Mf_{22} = & \frac{M_y \cdot (L-x)}{L} & Mf_3 = & -\frac{x}{L}\end{align*}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    l = EF_noeuds_distance(debut, fin);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), -1, "EF_charge_barre_ponctuelle_def_ang_iso_y");
    b = l-a;
    
    E = barre->materiau->ecm;
    
    switch(((Beton_Section_Rectangulaire*)(barre->section))->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      I = _1992_1_1_sections_iy(barre->section);
    // Pour une section constante, les angles valent :\end{verbatim}\begin{align*}
    // \varphi_A = &-\frac{F_z \cdot a}{6 \cdot E \cdot I_y \cdot L} \cdot b \cdot (2 \cdot L-a)-\frac{M_y}{6 \cdot E \cdot I_y \cdot L} \cdot (L^2-3 \cdot b^2)\nonumber\\
    // \varphi_B = &\frac{F_z \cdot a}{6 \cdot E \cdot I_y \cdot L} \cdot (L^2-a^2)-\frac{M_y}{6 \cdot E \cdot I_y \cdot L} \cdot (L^2-3 \cdot a^2)\end{align*}\begin{verbatim}
            *phia = -fz*a/(6*E*I*l)*b*(2*l-a)-my/(6*E*I*l)*(l*l-3*b*b);
            *phib = fz*a/(6*E*I*l)*(l*l-a*a)-my/(6*E*I*l)*(l*l-3*a*a);
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0, "EF_charge_barre_ponctuelle_def_ang_iso_y\n");
            break;
        }
    }
}


int EF_charge_barre_ponctuelle_def_ang_iso_z(Beton_Barre *barre, unsigned int discretisation,
  double a, double fy, double mz, double *phia, double *phib)
/* Description : Calcule les angles de rotation autour de l'axe z pour un élément bi-articulé
 *               soumis au chargement fy, mz dans le repère local. Les résultats sont renvoyés
 *               par l'intermédiaire des pointeurs phia et phib qui ne peuvent être NULL.
 * Paramètres : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de barre
 *                         à étudier,
 *            : double fy : force suivant l'axe y,
 *            : double mz : moment autour de l'axe z,
 *            : double *phia : pointeur qui contiendra l'angle au début de la barre,
 *            : double *phib : pointeur qui contiendra l'angle à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau == 0) ou
 *             (barre->noeud_debut == NULL) ou
 *             (barre->noeud_fin == NULL) ou
 *             (discretisation>barre->discretisation_element) ou
 *             (phia == NULL) ou
 *             (phib == NULL) ou
 *             (a < 0.) ou (a > l)
 */
{
    EF_Noeud    *debut, *fin;
    double      l, b, E;

    BUGMSG(barre, -1, "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
    BUGMSG(phia, -1, "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
    BUGMSG(phib, -1, "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), -1, "EF_charge_barre_ponctuelle_def_ang_iso_z");
    
    // Les rotations aux extrémités de la barre sont déterminées par les intégrales de Mohr
    //   et valent dans le cas général :\end{verbatim}\begin{center}
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_fy.pdf}\includegraphics[width=8cm]{images/charge_barre_ponctuelle_mz.pdf}\par
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_phiaz.pdf}\includegraphics[width=8cm]{images/charge_barre_ponctuelle_phibz.pdf}\end{center}\begin{align*}
    // \varphi_A = & \int_0^a \frac{Mf_{11} \cdot Mf_0}{E \cdot I_z(x)} + \int_a^L \frac{Mf_{12} \cdot Mf_0}{E \cdot I_z(x)} + \int_0^a \frac{Mf_{21} \cdot Mf_0}{E \cdot I_z(x)} + \int_a^L \frac{Mf_{22} \cdot Mf_0}{E \cdot I_z(x)} \nonumber\\
    // \varphi_B = & \int_0^a \frac{Mf_{11} \cdot Mf_3}{E \cdot I_z(x)} + \int_a^L \frac{Mf_{12} \cdot Mf_3}{E \cdot I_z(x)} + \int_0^a \frac{Mf_{21} \cdot Mf_3}{E \cdot I_z(x)} + \int_a^L \frac{Mf_{22} \cdot Mf_3}{E \cdot I_z(x)}\end{align*}\begin{align*}
    // \texttt{avec :} Mf_0 = &\frac{L-x}{L} & Mf_{11} = &\frac{F_y \cdot b \cdot x}{L}\nonumber\\
    //                 Mf_{12} = & \frac{F_y \cdot a \cdot (L-x)}{L} & Mf_{21} = & -\frac{M_z \cdot x}{L}\nonumber\\
    //                 Mf_{22} = & \frac{M_z \cdot (L-x)}{L} & Mf_3 = & -\frac{x}{L}\end{align*}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    l = EF_noeuds_distance(debut, fin);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), -1, "EF_charge_barre_ponctuelle_def_ang_iso_z");
    b = l-a;
    
    E = barre->materiau->ecm;
    
    switch(((Beton_Section_Rectangulaire*)(barre->section))->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      I = _1992_1_1_sections_iz(barre->section);
            
    // Pour une section constante, les angles valent :\end{verbatim}\begin{displaymath}
    // \varphi_A = \frac{ F_y \cdot a}{6 \cdot E \cdot I_z \cdot L} b \cdot (2 \cdot L-a) - \frac{M_z}{6 \cdot E \cdot I_z \cdot L} \left(L^2-3 \cdot b^2 \right)\end{displaymath}\begin{displaymath}
    // \varphi_B = \frac{-F_y \cdot a}{6 \cdot E \cdot I_z \cdot L} (L^2-a^2) - \frac{M_z}{6 \cdot E \cdot I_z \cdot L} \left(L^2-3 \cdot a^2 \right)\end{displaymath}\begin{verbatim}
            *phia = fy*a/(6*E*I*l)*b*(2*l-a)-mz/(6*E*I*l)*(l*l-3*b*b);
            *phib = -fy*a/(6*E*I*l)*(l*l-a*a)-mz/(6*E*I*l)*(l*l-3*a*a);
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0., "EF_charge_barre_ponctuelle_def_ang_iso_z\n");
            break;
        }
    }
}


int EF_charge_barre_ponctuelle_fonc_rx(Fonction *fonction, Beton_Barre *barre,
  unsigned int discretisation, double a, double max, double mbx)
/* Description : Calcule les déplacements d'une barre en rotation autour de l'axe x en fonction
 *                  des efforts aux extrémités de la poutre soumise à un moment de torsion
 *                  ponctuel à la position a.
 * Paramètres : Fonction *fonction : fonction où sera ajoutée la déformée,
 *            : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position du moment ponctuel autour de l'axe x par rapport au début
 *                         de la partie de barre à étudier,
 *            : double max : moment au début de la barre,
 *            : double mbx : moment à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (fonction == NULL) ou
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau == 0) ou
 *             (barre->noeud_debut == NULL) ou
 *             (barre->noeud_fin == NULL) ou
 *             (discretisation>barre->discretisation_element) ou
 *             (a < 0.) ou (a > l)
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    EF_Noeud    *debut, *fin;
    Barre_Info_EF *infos;
    double      l;
    double      G, debut_barre;

    BUGMSG(fonction, -1, "EF_charge_barre_ponctuelle_fonc_rx\n");
    BUGMSG(barre, -1, "EF_charge_barre_ponctuelle_fonc_rx\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_ponctuelle_fonc_rx\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_ponctuelle_fonc_rx\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_ponctuelle_fonc_rx\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_ponctuelle_fonc_rx\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_ponctuelle_fonc_rx\n");
    infos = &(barre->info_EF[discretisation]);
    BUGMSG(!((ERREUR_RELATIVE_EGALE(infos->kAx, MAXDOUBLE)) && (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))), -1, "EF_charge_barre_ponctuelle_fonc_rx\n");
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), -1, "EF_charge_barre_ponctuelle_fonc_rx");
    
    // La déformation d'une barre soumise à un effort de torsion est défini par les formules :\end{verbatim}\begin{center}
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_mx.pdf}\end{center}\begin{verbatim}
    // Si le noeud B est relaché en rotation Alors\end{verbatim}\begin{align*}
    // r_x(x) = & M_{Ax} \cdot \left( k_{Ax} + \int_0^x \frac{1}{J \cdot G} dX \right) & & \textrm{ pour x variant de 0 à a}\nonumber\\
    // r_x(x) = & M_{Ax} \cdot k_{Ax} + \int_0^a \frac{M_{Ax}}{J \cdot G} dX - \int_a^x \frac{M_{Bx}}{J \cdot G} dX & & \textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
    // Sinon \end{verbatim}\begin{align*}
    // r_x(x) = & M_{Bx} \cdot k_{Bx} + \int_a^L \frac{M_{Bx}}{J \cdot G} dX - \int_x^a \frac{M_{Ax}}{J \cdot G} dX & & \textrm{ pour x variant de 0 à a}\nonumber\\
    // r_x(x) = & M_{Bx} \cdot \left( k_{Bx} + \int_x^L \frac{1}{J \cdot G} dX \right) & & \textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
    // FinSi
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    debut_barre = EF_noeuds_distance(barre->noeud_debut, debut);
    l = EF_noeuds_distance(debut, fin);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), -1, "EF_charge_barre_ponctuelle_fonc_rx");
    
    G = barre->materiau->gnu_0_2;
    
    switch(((Beton_Section_Rectangulaire*)(barre->section))->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      J = _1992_1_1_sections_j(barre->section);
            
            if (ERREUR_RELATIVE_EGALE(infos->kBx, MAXDOUBLE))
            {
                BUG(common_fonction_ajout(fonction, 0., a, max*infos->kAx, max/(G*J), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                BUG(common_fonction_ajout(fonction, a, l, max*infos->kAx+a*(max+mbx)/(G*J), -mbx/(G*J), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
            }
            else
            {
                BUG(common_fonction_ajout(fonction, 0., a, mbx*infos->kBx - (a*(max+mbx)-l*mbx)/(G*J), +max/(G*J), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
                BUG(common_fonction_ajout(fonction, a, l, mbx*(infos->kBx + l/(G*J)), -mbx/(G*J), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
            }
            
    // Pour une section section constante, les moments valent :
    // Si le noeud B est relaché en rotation Alors\end{verbatim}\begin{align*}
    // r_x(x) = & M_{Ax} \cdot \left( k_{Ax} + \frac{x}{J \cdot G} \right) & & \textrm{ pour x variant de 0 à a}\nonumber\\
    // r_x(x) = & M_{Ax} \cdot k_{Ax} + \frac{a \cdot (M_{Ax}+M_{Bx})-M_{Bx} \cdot x}{J \cdot G} & & \textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
    // Sinon \end{verbatim}\begin{align*}
    // r_x(x) = & M_{Bx} \cdot k_{Bx} - \frac{a \cdot (M_{Ax}+M_{Bx})-l \cdot M_{Bx} - M_{Ax} \cdot x}{J \cdot G} & & \textrm{ pour x variant de 0 à a}\nonumber\\
    // r_x(x) = & M_{Bx} \cdot \left( k_{Bx} + \frac{L-x}{J \cdot G} \right) & & \textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
    // FinSi
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0., "EF_charge_barre_ponctuelle_fonc_rx\n");
            break;
        }
    }
}


int EF_charge_barre_ponctuelle_fonc_ry(Fonction *f_rotation, Fonction* f_deform,
  Beton_Barre *barre, unsigned int discretisation, double a, double fz, double my,
  double may, double mby)
/* Description : Calcule les déplacements d'une barre en rotation autour de l'axe y et en
 *                 déformation selon l'axe z en fonction de la charge ponctuelle (fz et my) et
 *                 des efforts aux extrémités de la poutre.
 * Paramètres : Fonction *fonction : fonction où sera ajoutée la déformée,
 *            : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de barre à
 *                         étudier,
 *            : double fz : force suivant l'axe z
 *            : double my : moment autour de l'axe y
 *            : double may : moment au début de la barre,
 *            : double mby : moment à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (f_rotation == NULL) ou
 *             (f_deform == NULL) ou
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau == 0) ou
 *             (barre->noeud_debut == NULL) ou
 *             (barre->noeud_fin == NULL) ou
 *             (discretisation>barre->discretisation_element) ou
 *             (a < 0.) ou (a > l)
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    EF_Noeud    *debut, *fin;
    double      l, b;
    double      E, debut_barre;
    
    BUGMSG(f_rotation, -1, "EF_charge_barre_ponctuelle_fonc_ry\n");
    BUGMSG(f_deform, -1, "EF_charge_barre_ponctuelle_fonc_ry\n");
    BUGMSG(barre, -1, "EF_charge_barre_ponctuelle_fonc_ry\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_ponctuelle_fonc_ry\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_ponctuelle_fonc_ry\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_ponctuelle_fonc_ry\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_ponctuelle_fonc_ry\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_ponctuelle_fonc_ry\n");
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), -1, "EF_charge_barre_ponctuelle_fonc_ry");
    
    // La déformation en rotation d'une barre soumise à un effort de flexion autour de l'axe y
    // est calculée selon le principe des intégrales de Mohr et est définie par les formules :\end{verbatim}\begin{center}
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_fz.pdf}\includegraphics[width=8cm]{images/charge_barre_ponctuelle_my.pdf}
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_my_1.pdf}\includegraphics[width=8cm]{images/charge_barre_ponctuelle_fz_1.pdf}
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_may_mby.pdf}\end{center}\begin{align*}
    // r_y(x) = & \int_0^x \frac{Mf_{11}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{11}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX + \int_a^L \frac{Mf_{12}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{21}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{21}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX + \int_a^L \frac{Mf_{22}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & \textrm{ pour x variant de 0 à a}\end{align*}\begin{align*}
    // r_y(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{12}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{12}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX \nonumber\\
    //          & + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{22}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{22}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX \nonumber\\
    //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{31}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{32}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          &\textrm{ pour x variant de a à L}\end{align*}\begin{align*}
    // f_z(x) = & \int_0^x \frac{Mf_{11}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{11}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_a^L \frac{Mf_{12}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{21}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^a \frac{Mf_{21}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX + \int_a^L \frac{Mf_{22}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          & \textrm{ pour x variant de 0 à a}\end{align*}\begin{align*}
    // f_z(x) = & \int_0^a \frac{Mf_{11}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{12}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{12}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX \nonumber\\
    //          & + \int_0^a \frac{Mf_{21}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_a^x \frac{Mf_{22}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{22}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX \nonumber\\
    //          & + \int_0^x \frac{Mf_{5}(X) \cdot Mf_{41}(X)}{E \cdot I_y(X)} dX + \int_x^L \frac{Mf_{5}(X) \cdot Mf_{42}(X)}{E \cdot I_y(X)} dX\nonumber\\
    //          &\textrm{ pour x variant de a à L}\end{align*}\begin{align*}
    // \textrm{ avec }
    //                 Mf_{11}(X) = &-\frac{F_z \cdot b \cdot X}{L} & Mf_{12}(X) = & -F_z \cdot a \cdot \left( \frac{L-X}{L} \right) \nonumber\\
    //                 Mf_{21}(X) = &-\frac{M_y \cdot X}{L} & Mf_{22}(X) = &\frac{M_y \cdot (L-X)}{L}\nonumber\\
    //                 Mf_{31}(X) = &-\frac{X}{L} & Mf_{32}(X) = &\frac{L-X}{L}\nonumber\\
    //                 Mf_{41}(X) = &-\frac{(L-x) \cdot X}{L} & Mf_{42}(X) = &-\frac{x \cdot (L-X)}{L}\nonumber\\
    //                 Mf_{5}(X) =  &\frac{M_{Ay} \cdot (L-X)}{L}-\frac{M_{By} \cdot X}{L} & & \end{align*}\begin{verbatim}
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    debut_barre = EF_noeuds_distance(barre->noeud_debut, debut);
    l = EF_noeuds_distance(debut, fin);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), -1, "EF_charge_barre_ponctuelle_fonc_ry");
    
    b = l-a;
    E = barre->materiau->ecm;
    
    switch(((Beton_Section_Rectangulaire*)(barre->section))->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      I = _1992_1_1_sections_iy(barre->section);
            BUG(common_fonction_ajout(f_rotation, 0., a,  fz*b/(6*E*I*l)*(-l*l+b*b),  0.,          fz*b/(2*E*I*l),      0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  l, -fz*a/(6*E*I*l)*(2*l*l+a*a), fz*a/(E*I), -fz*a/(2*E*I*l),      0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., a,  my/(6*E*I*l)*(-l*l+3*b*b),  0.,          my/(2*E*I*l),        0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  l, my/(6*E*I*l)*(2*l*l+3*a*a),  -my/(E*I),   my/(2*E*I*l),        0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., l, l/(6*E*I)*(2*may-mby),       -may/(E*I),  (may+mby)/(2*E*I*l), 0., 0., 0., 0., debut_barre) == 0, -3);
            
            BUG(common_fonction_ajout(f_deform, 0., a,  0.,                fz*b/(6*E*I*l)*(l*l-b*b),    0.,                 -fz*b/(6*E*I*l),      0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  l, -fz*a*a*a/(6*E*I),  fz*a/(6*E*I*l)*(a*a+2*l*l),  -fz*a/(2*E*I),      fz*a/(6*E*I*l),       0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., a,  0.,                my/(6*E*I*l)*(l*l-3*b*b),    0.,                 -my/(6*E*I*l),        0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  l, my/(6*E*I)*(3*a*a), -my/(6*E*I*l)*(2*l*l+3*a*a), my/(6*E*I*l)*(3*l), -my/(6*E*I*l),        0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., l, 0.,                 l/(6*E*I)*(-2*may+mby),      may/(2*E*I),        -(mby+may)/(6*E*I*l), 0., 0., 0., debut_barre) == 0, -3);
    // Pour une section constante, les rotations valent :\end{verbatim}\begin{align*}
    // r_y(x) = & \frac{F_z \cdot b}{6 \cdot E \cdot I_y \cdot L} [-L^2+b^2 + 3 \cdot x^2] & &\\
    //          & +\frac{M_y}{6 \cdot E \cdot I_y \cdot L} (-L^2+3 \cdot b^2 + 3 \cdot x^2) & &\\
    //          & +\frac{L}{6 \cdot E \cdot I_y} \cdot \left(2 \cdot M_{Ay}-M_{By} - \frac{6 \cdot M_{Ay}}{L} \cdot x + 3 \cdot \frac{M_{Ay}+M_{By}}{L^2} \cdot x^2 \right) & &\textrm{ pour x variant de 0 à a}\nonumber\\
    // r_y(x) = & -\frac{F_z \cdot a}{6 \cdot E \cdot I_y \cdot L}(2 \cdot L^2+a^2 -6 \cdot L \cdot x + 3 \cdot x^2) & &\\
    //          & +\frac{M_y}{6 \cdot E \cdot I_y \cdot L} \cdot (2 \cdot L^2+3 \cdot a^2 -6 \cdot L \cdot x + 3 \cdot x^2) & &\\
    //          & +\frac{L}{6 \cdot E \cdot I_y} \cdot \left(2 \cdot M_{Ay}-M_{By} - \frac{6 \cdot M_{Ay}}{L} \cdot x + 3 \cdot \frac{M_{Ay}+M_{By}}{L^2} \cdot x^2 \right) & &\textrm{ pour x variant de a à L}\end{align*}\begin{align*}
    // f_z(x) = & \frac{F_z \cdot b \cdot x}{6 \cdot E \cdot I_y \cdot L}  \cdot \left( L^2-b^2 - x^2 \right) & &\\
    //          & + \frac{M_y \cdot x}{6 \cdot E \cdot I_y \cdot L} \cdot \left( L^2-3 \cdot b^2 - x^2 \right) & &\\
    //          & + \frac{x}{6 \cdot E \cdot I_y} \cdot \left( L \cdot (-2 \cdot M_{Ay}+M_{By}) + 3 \cdot M_{Ay} \cdot x - \frac{M_{By}+M_{Ay}}{L} \cdot x^2 \right) & &\textrm{ pour x variant de 0 à a}\nonumber\\
    // f_z(x) = & \frac{F_z \cdot a}{6 \cdot E \cdot I_y \cdot L} \cdot \left( -a^2 \cdot L + (a^2+2 \cdot L^2) \cdot x - 3 \cdot L \cdot x^2 + x^3 \right) & &\\
    //          & + \frac{M_y}{6 \cdot E \cdot I_y \cdot L} \left(3 \cdot a^2 \cdot L - (2 \cdot L^2+3 \cdot a^2) \cdot x + 3 \cdot L \cdot x^2 - x^3 \right) & &\\
    //          & + \frac{x}{6 \cdot E \cdot I_y} \cdot \left( L \cdot (-2 \cdot M_{Ay}+M_{By}) + 3 \cdot M_{Ay} \cdot x - \frac{M_{By}+M_{Ay}}{L} \cdot x^2 \right) & &\textrm{ pour x variant de a à L}\end{align*}\begin{verbatim}
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0, "EF_charge_barre_ponctuelle_fonc_ry\n");
            break;
        }
    }
}


int EF_charge_barre_ponctuelle_fonc_rz(Fonction *f_rotation, Fonction* f_deform,
  Beton_Barre *barre, unsigned int discretisation, double a, double fy, double mz,
  double maz, double mbz)
/* Description : Calcule les déplacements d'une barre en rotation autour de l'axe z et en
 *                 déformation selon l'axe y en fonction de la charge ponctuelle (fy et mz) et
 *                 des efforts aux extrémités de la poutre.
 * Paramètres : Fonction *fonction : fonction où sera ajoutée la déformée,
 *            : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de barre à
 *                         étudier,
 *            : double fy : force suivant l'axe y
 *            : double mz : moment autour de l'axe z
 *            : double maz : moment au début de la barre,
 *            : double mbz : moment à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (f_rotation == NULL) ou
 *             (f_deform == NULL) ou
 *             (barre == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau == 0) ou
 *             (barre->noeud_debut == NULL) ou
 *             (barre->noeud_fin == NULL) ou
 *             (discretisation>barre->discretisation_element) ou
 *             (a < 0.) ou (a > l)
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    EF_Noeud    *debut, *fin;
    double      l, b;
    double      E, debut_barre;
    
    BUGMSG(f_rotation, -1, "EF_charge_barre_ponctuelle_fonc_rz\n");
    BUGMSG(f_deform, -1, "EF_charge_barre_ponctuelle_fonc_rz\n");
    BUGMSG(barre, -1, "EF_charge_barre_ponctuelle_fonc_rz\n");
    BUGMSG(barre->section, -1, "EF_charge_barre_ponctuelle_fonc_rz\n");
    BUGMSG(barre->materiau, -1, "EF_charge_barre_ponctuelle_fonc_rz\n");
    BUGMSG(barre->noeud_debut, -1, "EF_charge_barre_ponctuelle_fonc_rz\n");
    BUGMSG(barre->noeud_fin, -1, "EF_charge_barre_ponctuelle_fonc_rz\n");
    BUGMSG(discretisation<=barre->discretisation_element, -1, "EF_charge_barre_ponctuelle_fonc_rz\n");
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), -1, "EF_charge_barre_ponctuelle_fonc_rz");
    
    // La déformation en rotation d'une barre soumise à un effort de flexion autour de l'axe y
    //   est calculée selon le principe des intégrales de Mohr et est définie par les mêmes
    //   formules que la fonction EF_charge_barre_ponctuelle_fonc_rz à ceci près que fz
    //   est remplacé par fy (il y également un changement de signe de fy lors du calcul des
    //   rotations dû au changement de repère), my par mz, may et mby par maz et mbz (il y
    //   également un changement de signe de mz, maz et mbz lors du calcul des déformations
    //   toujours dû au changement de repère) et Iy par Iz.
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    debut_barre = EF_noeuds_distance(barre->noeud_debut, debut);
    l = EF_noeuds_distance(debut, fin);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), -1, "EF_charge_barre_ponctuelle_fonc_rz");
    
    b = l-a;
    E = barre->materiau->ecm;
    
    switch(((Beton_Section_Rectangulaire*)(barre->section))->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      I = _1992_1_1_sections_iz(barre->section);
            BUG(common_fonction_ajout(f_rotation, 0., a, fy*b/(6*E*I*l)*a*(l+b),     0.,          -fy*b/(2*E*I*l),     0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  l, fy*a/(6*E*I*l)*(2*l*l+a*a), -fy*a/(E*I), fy*a/(2*E*I*l),      0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., a, mz/(6*E*I*l)*(-l*l+3*b*b),  0.,          mz/(2*E*I*l),        0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, a,  l, mz/(6*E*I*l)*(2*l*l+3*a*a), -mz/(E*I),   mz/(2*E*I*l),        0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_rotation, 0., l, l/(6*E*I)*(2*maz-mbz),      -maz/(E*I),  (maz+mbz)/(2*E*I*l), 0., 0., 0., 0., debut_barre) == 0, -3);
            
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  fy*b/(6*E*I*l)*(l*l-b*b),   0.,                  -fy*b/(6*E*I*l),     0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  l, -fy*a*a*a/(6*E*I),   fy*a/(6*E*I*l)*(a*a+2*l*l), -fy*a/(2*E*I),       fy*a/(6*E*I*l),      0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., a, 0.,                  mz/(6*E*I*l)*(-l*l+3*b*b),  0.,                  mz/(6*E*I*l),        0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, a,  l, -mz/(6*E*I)*(3*a*a), mz/(6*E*I*l)*(2*l*l+3*a*a), -mz/(6*E*I*l)*(3*l), mz/(6*E*I*l),        0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(f_deform, 0., l, 0.,                  l/(6*E*I)*(2*maz-mbz),      -maz/(2*E*I),        (mbz+maz)/(6*E*I*l), 0., 0., 0., debut_barre) == 0, -3);
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0, "EF_charge_barre_ponctuelle_fonc_rz\n");
            break;
        }
    }
}


int EF_charge_barre_ponctuelle_n(Fonction *fonction, Beton_Barre *barre,
  unsigned int discretisation, double a, double fax, double fbx)
/* Description : Calcule les déplacements d'une barre selon l'axe x en fonction de l'effort
 *               normal ponctuel n et des réactions d'appuis fax et fbx.
 * Paramètres : Fonction *fonction : fonction où sera ajoutée la déformée,
 *            : Beton_Barre *barre : Barre à étudier,
 *            : unsigned int discretisation : partie de la barre à étudier,
 *            : double a : position de la charge par rapport au début de la partie de barre,
 *            : double n : effort normal de la charge ponctuelle,
 *            : double fax : effort normal au début de la barre,
 *            : double fbx : effort normal à la fin de la barre.
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : -1 en cas de paramètres invalides :
 *             (barre == NULL) ou
 *             (fonction == NULL) ou
 *             (barre->section == NULL) ou
 *             (barre->materiau == 0) ou
 *             (barre->noeud_debut == NULL) ou
 *             (barre->noeud_fin == NULL) ou
 *             (discretisation>barre->discretisation_element) ou
 *             (a < 0.) ou (a > l)
 *           -3 en cas d'erreur due à une fonction interne
 */
{
    EF_Noeud    *debut, *fin;
    double      l, debut_barre;
    double      E;
    
    BUGMSG(fonction, 0., "EF_charge_barre_ponctuelle_n\n");
    BUGMSG(barre, 0., "EF_charge_barre_ponctuelle_n\n");
    BUGMSG(barre->section, 0., "EF_charge_barre_ponctuelle_n\n");
    BUGMSG(barre->materiau, 0., "EF_charge_barre_ponctuelle_n\n");
    BUGMSG(barre->noeud_debut, 0., "EF_charge_barre_ponctuelle_n\n");
    BUGMSG(barre->noeud_fin, 0., "EF_charge_barre_ponctuelle_n\n");
    BUGMSG(discretisation<=barre->discretisation_element, 0., "EF_charge_barre_ponctuelle_n\n");
    BUGMSG(!((a < 0.) && (!(ERREUR_RELATIVE_EGALE(a, 0.)))), -1, "EF_charge_barre_ponctuelle_n");
    
    // La déformation selon l'axe x est par la formule :\end{verbatim}\begin{center}
    // \includegraphics[width=8cm]{images/charge_barre_ponctuelle_n.pdf}\end{center}\begin{align*}
    // f_x(x) = & F_{Ax} \cdot \int_0^x \frac{1}{E \cdot S(x)} dx & & \textrm{ pour x variant de 0 à a} \nonumber\\
    // f_x(x) = & F_{Ax} \cdot \int_0^a \frac{1}{E \cdot S(x)} dx - F_{Bx} \cdot \int_a^x \frac{1}{E \cdot S(x)} dx & & \textrm{ pour x variant de a à l} \end{align*}\begin{verbatim}
    
    if (discretisation == 0)
        debut = barre->noeud_debut;
    else
        debut = barre->noeuds_intermediaires[discretisation-1];
    if (discretisation == barre->discretisation_element)
        fin = barre->noeud_fin;
    else
        fin = barre->noeuds_intermediaires[discretisation];
    
    debut_barre = EF_noeuds_distance(barre->noeud_debut, debut);
    l = EF_noeuds_distance(debut, fin);
    BUGMSG(!((a > l) && (!(ERREUR_RELATIVE_EGALE(a, l)))), -1, "EF_charge_barre_ponctuelle_n");
    
    E = barre->materiau->ecm;
    
    switch(((Beton_Section_Rectangulaire*)(barre->section))->type)
    {
        case BETON_SECTION_RECTANGULAIRE :
        case BETON_SECTION_T :
        case BETON_SECTION_CARRE :
        case BETON_SECTION_CIRCULAIRE :
        {
            double      S = _1992_1_1_sections_s(barre->section);
            
    // Pour une section constante, les déformations valent :\end{verbatim}\begin{align*}
    // f_x(x) = & \frac{F_{Ax} \cdot x}{E \cdot S} & &\textrm{ pour x variant de 0 à a}\nonumber\\
    // f_x(x) = & \frac{a \cdot (F_{Ax} + F_{Bx}) - F_{Bx} \cdot x}{E \cdot S} & & \textrm{ pour x variant de a à l}\end{align*}\begin{verbatim}
            BUG(common_fonction_ajout(fonction, 0., a, 0.,                 fax/(E*S), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
            BUG(common_fonction_ajout(fonction, a,  l, a*(fax+fbx)/(E*S), -fbx/(E*S), 0., 0., 0., 0., 0., debut_barre) == 0, -3);
            return 0;
            break;
        }
        default :
        {
            BUGMSG(0, 0., "EF_charge_barre_ponctuelle_n\n");
            break;
        }
    }
}
