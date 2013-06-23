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
#ifdef ENABLE_GTK

#include <libintl.h>
#include <locale.h>
#include <gtk/gtk.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "1990_action.hpp"
#include "1990_combinaisons.h"
#include "1992_1_1_barres.h"
#include "EF_gtk_rapport.h"
#include "EF_rigidite.h"
#include "EF_calculs.h"
#include "EF_verif.h"

void EF_gtk_calculs_calculer(GtkMenuItem *menuitem, Projet *projet)
/* Description : Exécute le calcul de la structure.
 * Paramètres : GtkMenuItem *menuitem : composant à l'origine de l'évènement,
 *            : Projet *projet : la variable projet.
 * Valeur renvoyée : Aucune.
 */
{
    GList           *rapport = NULL;
    int             erreur;
    GList           *list_parcours;
    
    BUGMSG(projet, , gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUG(EF_calculs_free(projet), );
    BUG(EF_verif_EF(projet, &rapport, &erreur), );
    EF_gtk_rapport(projet, rapport);
    EF_verif_rapport_free(rapport);
    if (erreur != 0)
        return;
    BUG(EF_calculs_initialise(projet), );
    BUG(_1992_1_1_barres_rigidite_ajout_tout(projet), );
    BUG(EF_calculs_genere_mat_rig(projet), );
    list_parcours = projet->actions;
    while (list_parcours)
    {
        Action  *action = list_parcours->data;
        BUG(EF_calculs_resoud_charge(projet, action), );
        
        list_parcours = g_list_next(list_parcours);
    }
    
    BUG(_1990_combinaisons_genere(projet), );
    
    gtk_widget_set_sensitive(projet->list_gtk.comp.menu_resultats_afficher, TRUE);
    
    return;
}


#endif
