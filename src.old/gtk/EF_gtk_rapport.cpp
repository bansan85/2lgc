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

#include <locale>

#include <gtk/gtk.h>

#include "common_m3d.hpp"
#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "common_gtk.hpp"
#include "EF_gtk_rapport.hpp"


GTK_WINDOW_CLOSE (ef, rapport);


GTK_WINDOW_DESTROY (ef, rapport, );


GTK_WINDOW_KEY_PRESS (ef, rapport);


/**
 * \brief Création de la fenêtre permettant d'afficher un rapport.
 * \param p : la variable projet,
 * \param rapport : le rapport à afficher.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
void
EF_gtk_rapport (Projet                     *p,
                std::list <Analyse_Comm *> *rapport)
{
  std::list <Analyse_Comm *>::iterator it;
  
  BUGPARAM (p, "%p", p, )
  
  if (UI_RAP.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_RAP.window));
    gtk_list_store_clear (UI_RAP.liste);
  }
  else
  {
    UI_RAP.builder = gtk_builder_new ();
    BUGCRIT (gtk_builder_add_from_resource (UI_RAP.builder,
                                          "/org/2lgc/codegui/ui/EF_rapport.ui",
                                            NULL) != 0,
             ,
             (gettext ("La génération de la fenêtre %s a échouée.\n"),
                       "Rapport"); )
    gtk_builder_connect_signals (UI_RAP.builder, p);
    
    UI_RAP.window = GTK_WIDGET (gtk_builder_get_object (UI_RAP.builder,
                                                        "EF_rapport_window"));
    UI_RAP.liste = GTK_LIST_STORE (gtk_builder_get_object (UI_RAP.builder,
                                                      "EF_rapport_treestore"));
  }
  UI_RAP.rapport.assign (rapport->begin (), rapport->end ());
  
  it = rapport->begin ();
  while (it != rapport->end ())
  {
    Analyse_Comm *analyse = *it;
    GtkTreeIter   Iter;
    GdkPixbuf    *pixbuf;
    
    if (analyse->resultat == 0)
    {
      NOTE (pixbuf = gtk_icon_theme_load_icon (gtk_icon_theme_get_default (),
                                               "gtk-apply",
                                               16,
                                               GTK_ICON_LOOKUP_FORCE_SVG,
                                               NULL),
            gettext ("Erreur lors de l'ouverture de l'image (%s) du thème.\n"),
                     "gtk-apply")
    }
    else if (analyse->resultat == 1)
    {
      NOTE (pixbuf = gtk_icon_theme_load_icon (gtk_icon_theme_get_default (),
                                               "gtk-dialog-warning",
                                               16,
                                               GTK_ICON_LOOKUP_FORCE_SVG,
                                               NULL),
            gettext ("Erreur lors de l'ouverture de l'image (%s) du thème.\n"),
                     "gtk-dialog-warning")
    }
    else if (analyse->resultat == 2)
    {
      NOTE (pixbuf = gtk_icon_theme_load_icon (gtk_icon_theme_get_default (),
                                               "gtk-dialog-error",
                                               16,
                                               GTK_ICON_LOOKUP_FORCE_SVG,
                                               NULL),
            gettext ("Erreur lors de l'ouverture de l'image (%s) du thème.\n"),
                     "gtk-dialog-error")
    }
    else
    {
      FAILCRIT (,
                (gettext ("Le résultat d'un rapport doit être compris entre 0 et 2.\n")); )
    }
    
    gtk_list_store_append (UI_RAP.liste, &Iter);
    gtk_list_store_set (UI_RAP.liste,
                        &Iter,
                        0, pixbuf,
                        1, analyse->analyse.c_str (),
                        2, analyse->commentaire.c_str (),
                        -1);
    if (pixbuf != NULL)
    {
      g_object_unref (pixbuf);
    }

    ++it;
  }
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_RAP.window),
                                GTK_WINDOW (UI_GTK.window));
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
