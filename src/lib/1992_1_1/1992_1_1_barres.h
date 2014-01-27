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

#ifndef __1992_1_1_BARRES_H
#define __1992_1_1_BARRES_H

#include "config.h"
#include "common_projet.h"


gboolean _1992_1_1_barres_init (Projet *p)
                                       __attribute__((__warn_unused_result__));

gboolean _1992_1_1_barres_ajout (Projet *p,
                                 Type_Element    type,
                                 Section        *section,
                                 EF_Materiau    *materiau,
                                 EF_Noeud       *noeud_debut,
                                 EF_Noeud       *noeud_fin,
                                 Flottant        angle,
                                 EF_Relachement *relachement,
                                 unsigned int    discretisation_element)
                                       __attribute__((__warn_unused_result__));

EF_Barre *_1992_1_1_barres_cherche_numero     (Projet        *p,
                                               unsigned int   numero,
                                               gboolean       critique)
                                       __attribute__((__warn_unused_result__));
gboolean _1992_1_1_barres_cherche_dependances (Projet        *p,
                                               GList         *appuis,
                                               GList         *noeuds,
                                               GList         *sections,
                                               GList         *materiaux,
                                               GList         *relachements,
                                               GList         *barres,
                                               GList        **noeuds_dep,
                                               GList        **barres_dep,
                                               GList        **charges_dep,
                                               gboolean       numero,
                                               gboolean       origine)
                                       __attribute__((__warn_unused_result__));
gboolean _1992_1_1_barres_angle_rotation      (EF_Noeud      *debut,
                                               EF_Noeud      *fin,
                                               double        *y,
                                               double        *z)
                                       __attribute__((__warn_unused_result__));

gboolean _1992_1_1_barres_change_type        (EF_Barre       *barre,
                                              Type_Element    type,
                                              Projet         *p)
                                       __attribute__((__warn_unused_result__));
gboolean _1992_1_1_barres_change_section     (EF_Barre       *barre,
                                              Section        *section,
                                              Projet         *p)
                                       __attribute__((__warn_unused_result__));
gboolean _1992_1_1_barres_change_materiau    (EF_Barre       *barre,
                                              EF_Materiau    *materiau,
                                              Projet         *p)
                                       __attribute__((__warn_unused_result__));
gboolean _1992_1_1_barres_change_angle       (EF_Barre       *barre,
                                              Flottant        angle,
                                              Projet         *p)
                                       __attribute__((__warn_unused_result__));
gboolean _1992_1_1_barres_change_noeud       (EF_Barre       *barre,
                                              EF_Noeud       *noeud,
                                              gboolean        noeud_1,
                                              Projet         *p)
                                       __attribute__((__warn_unused_result__));
gboolean _1992_1_1_barres_change_relachement (EF_Barre       *barre,
                                              EF_Relachement *relachement,
                                              Projet         *p) __attribute__((__warn_unused_result__));

gboolean _1992_1_1_barres_rigidite_ajout     (Projet         *p,
                                              EF_Barre       *element)
                                       __attribute__((__warn_unused_result__));
gboolean _1992_1_1_barres_rigidite_ajout_tout(Projet         *p)
                                       __attribute__((__warn_unused_result__));

gboolean _1992_1_1_barres_supprime_liste     (Projet         *p,
                                              GList          *liste_noeuds,
                                              GList          *liste_barres)
                                       __attribute__((__warn_unused_result__));
void     _1992_1_1_barres_free_foreach       (EF_Barre       *barre,
                                              Projet         *p);
gboolean _1992_1_1_barres_free               (Projet         *p)
                                       __attribute__((__warn_unused_result__));

#endif
