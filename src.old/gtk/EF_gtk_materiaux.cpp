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
#include "common_math.hpp"
#include "common_selection.hpp"
#include "common_text.hpp"
#include "1992_1_1_materiaux.hpp"
#include "1992_1_1_barres.hpp"
#include "1993_1_1_materiaux.hpp"
#include "EF_materiaux.hpp"
#include "1992_1_1_gtk_materiaux.hpp"
#include "1993_1_1_gtk_materiaux.hpp"
#include "EF_gtk_materiaux.hpp"


GTK_WINDOW_CLOSE (ef, materiaux);


GTK_WINDOW_DESTROY (ef, materiaux, );


GTK_WINDOW_KEY_PRESS (ef, materiaux);


/**
 * \brief Supprime un matériau sans dépendance si la touche SUPPR est appuyée.
 * \param treeview : composant à l'origine de l'évènement,
 * \param event : Caractéristique de l'évènement,
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE si la touche SUPPR est pressée, FALSE sinon.
 *   Echec : FALSE :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 *  
 */
extern "C"
gboolean
EF_gtk_materiaux_treeview_key_press (GtkTreeView *treeview,
                                     GdkEvent    *event,
                                     Projet      *p)
{
  BUGPARAMCRIT (p, "%p", p, FALSE)
  BUGCRIT (UI_MATX.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Matériau"); )
  
  if (event->key.keyval == GDK_KEY_Delete)
  {
    GtkTreeIter   Iter;
    GtkTreeModel *model;
    EF_Materiau  *materiau;
    
    std::list <EF_Materiau *> liste_materiaux;
    
    if (!gtk_tree_selection_get_selected (
                                        gtk_tree_view_get_selection (treeview),
                                          &model,
                                          &Iter))
    {
      return FALSE;
    }
    
    gtk_tree_model_get (model, &Iter, 0, &materiau, -1);
    
    liste_materiaux.push_back (materiau);
    if (!_1992_1_1_barres_cherche_dependances (p,
                                              NULL,
                                              NULL,
                                              NULL,
                                              &liste_materiaux,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              false))
    {
      BUG (EF_materiaux_supprime (materiau, p),
           FALSE)
    }
    
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/**
 * \brief Modification du nom d'un matériau.
 * \param cell : cellule en cours,
 * \param path_string : path de la ligne en cours,
 * \param new_text : le nouveau nom,
 * \param p : la variable projet.
 * \return Rien.
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_materiaux_edit_nom (GtkCellRendererText *cell,
                           gchar               *path_string,
                           gchar               *new_text,
                           Projet              *p)
{
  GtkTreeModel *model;
  GtkTreeIter   iter;
  GtkTreePath  *path;
  EF_Materiau  *materiau;
  
  std::string   str_tmp (new_text);
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_MATX.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Matériau"); )
  
  model = GTK_TREE_MODEL (UI_MATX.materiaux);
  path = gtk_tree_path_new_from_string (path_string);
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_path_free (path);
  gtk_tree_model_get (model, &iter, 0, &materiau, -1);
  if ((materiau->nom.compare (new_text) == 0) || (strcmp (new_text, "") == 0))
  {
    return;
  }
  
  if (EF_materiaux_cherche_nom (p, &str_tmp, false))
  {
    return;
  }
  
  switch (materiau->type)
  {
    case MATERIAU_BETON :
    {
      BUG (_1992_1_1_materiaux_modif (p,
                                      materiau,
                                      &str_tmp,
                                      m_f (NAN, FLOTTANT_ORDINATEUR),
                                      m_f (NAN, FLOTTANT_ORDINATEUR),
                                      m_f (NAN, FLOTTANT_ORDINATEUR),
                                      m_f (NAN, FLOTTANT_ORDINATEUR),
                                      m_f (NAN, FLOTTANT_ORDINATEUR),
                                      m_f (NAN, FLOTTANT_ORDINATEUR),
                                      m_f (NAN, FLOTTANT_ORDINATEUR),
                                      m_f (NAN, FLOTTANT_ORDINATEUR),
                                      m_f (NAN, FLOTTANT_ORDINATEUR),
                                      m_f (NAN, FLOTTANT_ORDINATEUR),
                                      m_f (NAN, FLOTTANT_ORDINATEUR),
                                      m_f (NAN, FLOTTANT_ORDINATEUR),
                                      m_f (NAN, FLOTTANT_ORDINATEUR),
                                      m_f (NAN, FLOTTANT_ORDINATEUR),
                                      m_f (NAN, FLOTTANT_ORDINATEUR)),
          )
      break;
    }
    case MATERIAU_ACIER :
    {
      BUG (_1993_1_1_materiaux_modif (p,
                                      materiau,
                                      &str_tmp,
                                      m_f (NAN, FLOTTANT_ORDINATEUR),
                                      m_f (NAN, FLOTTANT_ORDINATEUR),
                                      m_f (NAN, FLOTTANT_ORDINATEUR),
                                      m_f (NAN, FLOTTANT_ORDINATEUR)),
          )
      break;
    }
    default :
    {
      FAILINFO ( ,
                (gettext ("Le type de matériau %d n'existe pas.\n"),
                          materiau->type); )
      break;
    }
  }
  
  return;
}


/**
 * \brief En fonction de la sélection, active ou désactive le bouton supprimer.
 * \param treeselection : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_materiaux_select_changed (GtkTreeSelection *treeselection,
                                 Projet           *p)
{
  GtkTreeModel *model;
  GtkTreeIter   Iter;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_MATX.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Matériau"); )
  
  // Si aucune section n'est sélectionnée, il n'est pas possible d'en supprimer
  // ou d'en éditer une.
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
                                       gtk_builder_get_object (UI_MATX.builder,
                                              "EF_materiaux_treeview_select")),
                                        &model,
                                        &Iter))
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                    UI_MATX.builder, "EF_materiaux_boutton_supprimer_direct")),
                              FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                      UI_MATX.builder, "EF_materiaux_boutton_supprimer_menu")),
                              FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                            UI_MATX.builder, "EF_materiaux_boutton_modifier")),
                              FALSE);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                    UI_MATX.builder, "EF_materiaux_boutton_supprimer_direct")),
                            TRUE);
    gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                      UI_MATX.builder, "EF_materiaux_boutton_supprimer_menu")),
                            FALSE);
  }
  else
  {
    EF_Materiau *materiau;
    
    std::list <EF_Materiau *> liste_materiaux;
    
    gtk_tree_model_get (model, &Iter, 0, &materiau, -1);
    
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                            UI_MATX.builder, "EF_materiaux_boutton_modifier")),
                              TRUE);
    
    liste_materiaux.push_back (materiau);
    if (_1992_1_1_barres_cherche_dependances (p,
                                              NULL,
                                              NULL,
                                              NULL,
                                              &liste_materiaux,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              false))
    {
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                    UI_MATX.builder, "EF_materiaux_boutton_supprimer_direct")),
                                FALSE);
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                      UI_MATX.builder, "EF_materiaux_boutton_supprimer_menu")),
                                TRUE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                    UI_MATX.builder, "EF_materiaux_boutton_supprimer_direct")),
                              FALSE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                      UI_MATX.builder, "EF_materiaux_boutton_supprimer_menu")),
                              TRUE);
    }
    else
    {
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                    UI_MATX.builder, "EF_materiaux_boutton_supprimer_direct")),
                                TRUE);
      gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                      UI_MATX.builder, "EF_materiaux_boutton_supprimer_menu")),
                                FALSE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                    UI_MATX.builder, "EF_materiaux_boutton_supprimer_direct")),
                              TRUE);
      gtk_widget_set_visible (GTK_WIDGET (gtk_builder_get_object (
                      UI_MATX.builder, "EF_materiaux_boutton_supprimer_menu")),
                              FALSE);
    }
  }
  
  return;
}


/**
 * \brief Affiche la liste des dépendances dans le menu lorsqu'on clique sur le
 *        bouton.
 * \param widget : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_materiaux_boutton_supprimer_menu (GtkButton *widget,
                                         Projet    *p)
{
  GtkTreeModel *model;
  GtkTreeIter   Iter;
  EF_Materiau  *materiau;
  
  std::list <EF_Materiau *>  liste_materiaux;
  std::list <EF_Noeud    *> *liste_noeuds_dep;
  std::list <EF_Barre    *> *liste_barres_dep;
  std::list <Charge      *> *liste_charges_dep;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_MATX.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Matériau"); )
  
  // Si aucune section n'est sélectionnée, il n'est pas possible d'en supprimer
  // ou d'en éditer une.
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
     gtk_builder_get_object (UI_MATX.builder, "EF_materiaux_treeview_select")),
                                        &model,
                                        &Iter))
  {
    FAILINFO ( , (gettext ("Aucun élément n'est sélectionné.\n")); )
  }
  
  gtk_tree_model_get (model, &Iter, 0, &materiau, -1);
  
  liste_materiaux.push_back (materiau);
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &liste_materiaux,
                                             NULL,
                                             NULL,
                                             &liste_noeuds_dep,
                                             NULL,
                                             &liste_barres_dep,
                                             NULL,
                                             &liste_charges_dep,
                                             false),
      , )
  
  if ((!liste_noeuds_dep->empty ()) ||
      (!liste_barres_dep->empty ()) ||
      (!liste_charges_dep->empty ()))
  {
    std::string desc;
    
    desc = common_text_dependances (liste_noeuds_dep,
                                    liste_barres_dep,
                                    liste_charges_dep,
                                    p);
    gtk_menu_item_set_label (GTK_MENU_ITEM (gtk_builder_get_object (
                       UI_MATX.builder, "EF_materiaux_supprimer_menu_barres")),
                             desc.c_str ());
  }
  else
  {
    FAILINFO ( , (gettext ("L'élément ne possède aucune dépendance.\n")); )
  }
  
  delete liste_noeuds_dep;
  delete liste_barres_dep;
  delete liste_charges_dep;
  
  return;
}


/**
 * \brief Supprime le matériau sélectionné dans le treeview, y compris les
 *        barres l'utilisant.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_materiaux_supprimer_menu_barres (GtkButton *button,
                                        Projet    *p)
{
  GtkTreeIter   iter;
  GtkTreeModel *model;
  EF_Materiau  *materiau;
  
  std::list <EF_Materiau *>  liste_materiaux;
  std::list <EF_Barre    *> *liste_barres_dep;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_MATX.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Matériau"); )
  
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
                                       gtk_builder_get_object (UI_MATX.builder,
                                              "EF_materiaux_treeview_select")),
                                        &model,
                                        &iter))
  {
    return;
  }
  
  gtk_tree_model_get (model, &iter, 0, &materiau, -1);
  
  liste_materiaux.push_back (materiau);
  BUG (_1992_1_1_barres_cherche_dependances (p,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &liste_materiaux,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &liste_barres_dep,
                                             NULL,
                                             NULL,
                                             false),
      , )
  BUG (_1992_1_1_barres_supprime_liste (p, NULL, liste_barres_dep),
       ,
       delete liste_barres_dep; )
  delete liste_barres_dep;
  BUG (EF_materiaux_supprime (materiau, p), )
  
  BUG (m3d_rafraichit (p), )
  
  return;
}


/**
 * \brief Supprime le matériau sélectionné dans le treeview.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_materiaux_supprimer_direct (GtkButton *button,
                                   Projet    *p)
{
  GtkTreeIter   iter;
  GtkTreeModel *model;
  EF_Materiau  *materiau;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_MATX.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Matériau"); )
  
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
                                       gtk_builder_get_object (UI_MATX.builder,
                                              "EF_materiaux_treeview_select")),
                                        &model,
                                        &iter))
  {
    return;
  }
  
  gtk_tree_model_get (model, &iter, 0, &materiau, -1);
  
  BUG (EF_materiaux_supprime (materiau, p), )
  
  return;
}


/**
 * \brief Affiche la distance vy de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_materiaux_render_0 (GtkTreeViewColumn *tree_column,
                           GtkCellRenderer   *cell,
                           GtkTreeModel      *tree_model,
                           GtkTreeIter       *iter,
                           gpointer           data2)
{
  EF_Materiau *materiau;
  
  gtk_tree_model_get (tree_model, iter, 0, &materiau, -1);
  BUGPARAM (materiau, "%p", materiau, )
  
  g_object_set (cell, "text", materiau->nom.c_str (), NULL);
  
  return;
}


/**
 * \brief Affiche la distance vy de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_materiaux_render_1 (GtkTreeViewColumn *tree_column,
                           GtkCellRenderer   *cell,
                           GtkTreeModel      *tree_model,
                           GtkTreeIter       *iter,
                           gpointer           data2)
{
  EF_Materiau *materiau;
  
  gtk_tree_model_get (tree_model, iter, 0, &materiau, -1);
  BUGPARAM (materiau, "%p", materiau, )
  
  switch (materiau->type)
  {
    case MATERIAU_BETON :
    {
      g_object_set (cell, "text", gettext ("Béton"), NULL);
      break;
    }
    case MATERIAU_ACIER :
    {
      g_object_set (cell, "text", gettext ("Acier"), NULL);
      break;
    }
    default :
    {
      g_object_set (cell, "text", gettext ("Inconnu"), NULL);
      break;
    }
  }
  
  return;
}


/**
 * \brief Affiche la distance vy de la section.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_materiaux_render_2 (GtkTreeViewColumn *tree_column,
                           GtkCellRenderer   *cell,
                           GtkTreeModel      *tree_model,
                           GtkTreeIter       *iter,
                           gpointer           data2)
{
  EF_Materiau *materiau;
  
  gtk_tree_model_get (tree_model, iter, 0, &materiau, -1);
  BUGPARAM (materiau, "%p", materiau, )
  
  switch (materiau->type)
  {
    case MATERIAU_BETON :
    case MATERIAU_ACIER :
    {
      std::string c;
      
      c = EF_materiaux_get_description (materiau);
      g_object_set (cell, "markup", c.c_str (), NULL);
      
      break;
    }
    default :
    {
      g_object_set(cell, "markup", gettext ("Inconnu"), NULL);
      break;
    }
  }
  
  return;
}


/**
 * \brief Edite les matériaux sélectionnés.
 * \param widget : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 * Echec :
 *   - p == NULL,
 *   - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_materiaux_edit_clicked (GtkWidget *widget,
                               Projet    *p)
{
  GtkTreeIter   iter;
  GtkTreeModel *model;
  GList        *list, *list_parcours;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_MATX.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Matériaux"); )
  
  // On récupère la liste des matériaux à éditer.
  list = gtk_tree_selection_get_selected_rows (GTK_TREE_SELECTION (
                            gtk_builder_get_object (UI_MATX.builder,
                                              "EF_materiaux_treeview_select")),
                                               &model);
  list_parcours = g_list_first (list);
  for ( ; list_parcours != NULL; list_parcours = g_list_next (list_parcours))
  {
    if (gtk_tree_model_get_iter (model,
                                 &iter,
                                 (GtkTreePath *) list_parcours->data))
    {
  // Et on les édite les unes après les autres.
      EF_Materiau *materiau;
      
      gtk_tree_model_get (model, &iter, 0, &materiau, -1);
      
      switch (materiau->type)
      {
        case MATERIAU_BETON :
        {
          BUG (_1992_1_1_gtk_materiaux (p, materiau),
               ,
               g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
                 g_list_free (list); )
          break;
        }
        case MATERIAU_ACIER :
        {
          BUG (_1993_1_1_gtk_materiaux (p, materiau),
               ,
               g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
                 g_list_free (list); )
          break;
        }
        default :
        {
          FAILINFO ( ,
                    (gettext ("Le type de matériau %d n'existe pas.\n"),
                              materiau->type);
                      g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
                      g_list_free (list); )
          break;
        }
      }
    }
  }
  g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
  g_list_free (list);
  
  return;
}


/**
 * \brief Lance la fenêtre d'édition du matériau sélectionné en cas de
 *        double-clique dans le tree-view.
 * \param widget : composant à l'origine de l'évènement,
 * \param event : Information sur l'évènement,
 * \param p : la variable projet.
 * \return
 *   Succès : TRUE s'il y a édition via un double-clique, FALSE sinon.\n
 *   Echec : FALSE :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
gboolean
EF_gtk_materiaux_double_clicked (GtkWidget *widget,
                                 GdkEvent  *event,
                                 Projet    *p)
{
  BUGPARAMCRIT (p, "%p", p, FALSE)
  BUGCRIT (UI_MATX.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Matériaux"); )
  
  if ((event->type == GDK_2BUTTON_PRESS) &&
      (gtk_widget_get_sensitive (GTK_WIDGET (gtk_builder_get_object (
               UI_MATX.builder, "EF_materiaux_boutton_modifier")))))
  {
    EF_gtk_materiaux_edit_clicked (widget, p);
    return TRUE;
  }
  else
  {
    return common_gtk_treeview_button_press_unselect (GTK_TREE_VIEW (widget),
                                                      (GdkEventButton*) event,
                                                      p);
  }
}


/**
 * \brief Création de la fenêtre permettant d'afficher les matériaux sous forme
 *        d'un tableau.
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
void
EF_gtk_materiaux (Projet *p)
{
  std::list <EF_Materiau *>::iterator it;
  
  BUGPARAM (p, "%p", p, )
  if (UI_MATX.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_MATX.window));
    return;
  }
  
  UI_MATX.builder = gtk_builder_new ();
  BUGCRIT (gtk_builder_add_from_resource (UI_MATX.builder,
                                        "/org/2lgc/codegui/ui/EF_materiaux.ui",
                                          NULL) != 0,
           ,
           (gettext ("La génération de la fenêtre %s a échouée.\n"),
                     "Matériau"); )
  gtk_builder_connect_signals (UI_MATX.builder, p);
  
  UI_MATX.window = GTK_WIDGET (gtk_builder_get_object (UI_MATX.builder,
                                                       "EF_materiaux_window"));
  UI_MATX.materiaux = GTK_TREE_STORE (gtk_builder_get_object (UI_MATX.builder,
                                                    "EF_materiaux_treestore"));
  
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_MATX.builder,
                                             "EF_materiaux_treeview_column0")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_MATX.builder,
                                               "EF_materiaux_treeview_cell0")),
    EF_gtk_materiaux_render_0,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_MATX.builder,
                                             "EF_materiaux_treeview_column1")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_MATX.builder,
                                               "EF_materiaux_treeview_cell1")),
    EF_gtk_materiaux_render_1,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_MATX.builder,
                                             "EF_materiaux_treeview_column2")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_MATX.builder,
                                               "EF_materiaux_treeview_cell2")),
    EF_gtk_materiaux_render_2,
    p,
    NULL);
  
  it = p->modele.materiaux.begin ();
  while (it != p->modele.materiaux.end ())
  {
    EF_Materiau *materiau = *it;
    
    gtk_tree_store_append (UI_MATX.materiaux, &materiau->Iter_fenetre, NULL);
    gtk_tree_store_set (UI_MATX.materiaux,
                        &materiau->Iter_fenetre,
                        0, materiau,
                        -1);
    
    ++it;
  }
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_MATX.window),
                                GTK_WINDOW (UI_GTK.window));
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
