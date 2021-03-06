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

#include <string.h>

#include <gtk/gtk.h>

#include <algorithm>
#include <memory>
#include <locale>

#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "common_gtk.hpp"
#include "common_math.hpp"
#include "common_text.hpp"
#include "common_selection.hpp"
#include "EF_sections.hpp"
#include "EF_gtk_sections.hpp"
#include "EF_gtk_section_personnalisee.hpp"

GTK_WINDOW_KEY_PRESS (ef, section_personnalisee);


GTK_WINDOW_DESTROY (ef, section_personnalisee,

  if (!UI_SEC_PE.keep)
  {
    GtkTreeIter iter;
    
    if (gtk_tree_model_get_iter_first(UI_SEC_PE.model, &iter))
    {
      do
      {
        GtkTreeIter iter2;
        
        if (gtk_tree_model_iter_children(UI_SEC_PE.model, &iter2, &iter))
        {
          do
          {
            EF_Point  *point;
            
            gtk_tree_model_get(UI_SEC_PE.model, &iter, 0, &point, -1);
            free(point);
          } while (gtk_tree_model_iter_next(UI_SEC_PE.model, &iter2));
        }
      } while (gtk_tree_model_iter_next(UI_SEC_PE.model, &iter));
    }
  }

);


GTK_WINDOW_CLOSE (ef, section_personnalisee);


/**
 * \brief Récupère toutes les données de la fenêtre permettant d'ajouter ou
 *        d'éditer une section personnalisée.
 * \param p : la variable projet,
 * \param j : moment d’inertie de torsion en m⁴,
 * \param iy : moment d’inertie de flexion selon l'axe y en m⁴,
 * \param iz : moment d’inertie de flexion selon l'axe z en m⁴,
 * \param vy : distance horizontale entre le centre de gravité et la
 *             fibre extrême droite en m,
 * \param vyp : distance horizontale entre le centre de gravité et la
 *              fibre extrême gauche en m,
 * \param vz : distance verticale entre le centre de gravité et la
 *             fibre extrême haute en m,
 * \param vzp : distance verticale entre le centre de gravité et la
 *              fibre extrême basse en m,
 * \param s : aire de section en m²,
 * \param forme : la forme de la section sous forme d'une liste de liste de
 *                points,
 * \param ok_forme : ok_forme si la forme est correcte,
 * \param nom : le nom de la section,
 * \param description : la description de la section.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL,
 *     - j == NULL,
 *     - iy == NULL,
 *     - iz == NULL,
 *     - vy == NULL,
 *     - vyp == NULL,
 *     - vz == NULL,
 *     - vzp == NULL,
 *     - s == NULL,
 *     - forme == NULL,
 *     - ok_forme == NULL,
 *     - nom == NULL,
 *     - description == NULL,
 *     - interface graphique non initialisée,
 *     - en cas d'erreur d'allocation mémoire.
 */
bool
EF_gtk_section_personnalisee_recupere_donnees (
  Projet      *p,
  double      *j,
  double      *iy,
  double      *iz,
  double      *vy,
  double      *vyp,
  double      *vz,
  double      *vzp,
  double      *s,
  std::list <std::list <EF_Point *> *> **forme,
  bool        *ok_forme,
  std::string *nom,
  std::string *description)
{
  GtkTextIter    start, end;
  GtkTextBuffer *textbuffer;
  bool           ok = true;
  GtkTreeIter    iter;
  char          *txt;
  
  BUGPARAMCRIT (p, "%p", p, false)
  BUGPARAMCRIT (j, "%p", j, false)
  BUGPARAMCRIT (iy, "%p", iy, false)
  BUGPARAMCRIT (iz, "%p", iz, false)
  BUGPARAMCRIT (vy, "%p", vy, false)
  BUGPARAMCRIT (vyp, "%p", vyp, false)
  BUGPARAMCRIT (vz, "%p", vz, false)
  BUGPARAMCRIT (vzp, "%p", vzp, false)
  BUGPARAMCRIT (s, "%p", s, false)
  BUGPARAMCRIT (forme, "%p", forme, false)
  BUGPARAMCRIT (nom, "%p", nom, false)
  BUGPARAMCRIT (description, "%p", description, false)
  BUGCRIT (UI_SEC_PE.builder,
           false,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Section Personnalisee"); )
  
  *j = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (UI_SEC_PE.builder,
                                         "EF_section_personnalisee_buffer_j")),
                    0, false,
                    INFINITY, false);
  if (std::isnan (*j))
  {
    ok = false;
  }
  
  *iy = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                     UI_SEC_PE.builder, "EF_section_personnalisee_buffer_iy")),
                     0, false,
                     INFINITY, false);
  if (std::isnan (*iy))
  {
    ok = false;
  }
  
  *iz = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                     UI_SEC_PE.builder, "EF_section_personnalisee_buffer_iz")),
                     0, false,
                     INFINITY, false);
  if (std::isnan (*iz))
  {
    ok = false;
  }
  
  *vy = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                     UI_SEC_PE.builder, "EF_section_personnalisee_buffer_vy")),
                     0, false,
                     INFINITY, false);
  if (std::isnan (*vy))
  {
    ok = false;
  }
  
  *vyp = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                    UI_SEC_PE.builder, "EF_section_personnalisee_buffer_vyp")),
                    0, false,
                    INFINITY, false);
  if (std::isnan (*vyp))
  {
    ok = false;
  }
  
  *vz = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                     UI_SEC_PE.builder, "EF_section_personnalisee_buffer_vz")),
                     0, false,
                     INFINITY, false);
  if (std::isnan (*vz))
  {
    ok = false;
  }
  
  *vzp = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                    UI_SEC_PE.builder, "EF_section_personnalisee_buffer_vzp")),
                      0, false,
                      INFINITY, false);
  if (std::isnan (*vzp))
  {
    ok = false;
  }
  
  *s = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (UI_SEC_PE.builder,
                                         "EF_section_personnalisee_buffer_s")),
                    0, false,
                    INFINITY, false);
  if (std::isnan (*s))
  {
    ok = false;
  }
  
  *forme = new std::list <std::list <EF_Point *> *> ();
  if (!gtk_tree_model_get_iter_first (UI_SEC_PE.model, &iter))
  {
    *ok_forme = false;
    ok = false;
  }
  else
  {
    do
    {
      GtkTreeIter iter2;
      
      if (gtk_tree_model_iter_children (UI_SEC_PE.model, &iter2, &iter))
      {
        std::list <EF_Point *> *liste_noeuds = new std::list <EF_Point *> ();
        
        EF_Point *point;
        
        gtk_tree_model_get (UI_SEC_PE.model, &iter2, 0, &point, -1);
        // Comme ça, on ajoute pas le dernier point qui est le même que le
        // premier.
        while (gtk_tree_model_iter_next (UI_SEC_PE.model, &iter2))
        {
          liste_noeuds->push_back (point);
          
          gtk_tree_model_get (UI_SEC_PE.model, &iter2, 0, &point, -1);
        }
        
        if (!liste_noeuds->empty ())
        {
          (*forme)->push_back (liste_noeuds);
        }
        else
        {
          delete liste_noeuds;
        }
      }
      
    } while (gtk_tree_model_iter_next (UI_SEC_PE.model, &iter));
    
    *ok_forme = EF_sections_personnalisee_verif_forme (*forme, true);
    
    if (!*ok_forme)
    {
      ok = false;
    }
  }
  
  // Si tous les paramètres sont corrects
  textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                     gtk_builder_get_object (UI_SEC_PE.builder,
                            "EF_section_personnalisee_textview_description")));
  gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
  gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
  txt = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
  *description = txt;
  free (txt);
  
  textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                     gtk_builder_get_object (UI_SEC_PE.builder,
                                    "EF_section_personnalisee_textview_nom")));
  gtk_text_buffer_get_iter_at_offset (textbuffer, &start, 0);
  gtk_text_buffer_get_iter_at_offset (textbuffer, &end, -1);
  txt = gtk_text_buffer_get_text (textbuffer, &start, &end, FALSE);
  *nom = txt;
  free (txt);
  
  gtk_text_buffer_remove_all_tags (textbuffer, &start, &end);
  
  if (UI_SEC_PE.section == NULL)
  {
    if ((nom->empty ()) || (EF_sections_cherche_nom (p, nom, false)))
    {
      gtk_text_buffer_apply_tag_by_name (textbuffer, "mauvais", &start, &end);
      ok = false;
    }
    else
    {
      gtk_text_buffer_apply_tag_by_name (textbuffer, "OK", &start, &end);
    }
  }
  else if ((nom->empty ()) ||
           ((UI_SEC_PE.section->nom.compare (*nom) != 0) &&
            (EF_sections_cherche_nom (p, nom, false))))
  {
    gtk_text_buffer_apply_tag_by_name (textbuffer, "mauvais", &start, &end);
    ok = false;
  }
  else
  {
    gtk_text_buffer_apply_tag_by_name (textbuffer, "OK", &start, &end);
  }
  
  return ok;
}


/**
 * \brief Vérifie si l'ensemble des éléments est correct pour activer le bouton
 *        add/edit.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_section_personnalisee_check (GtkWidget *button,
                                    Projet    *p)
{
  double                j, iy, iz, vy, vyp, vz, vzp, s;
  bool                  ok_forme;
  std::string           nom, description;
  Section               section;
  Section_Personnalisee data;
  GdkPixbuf            *pixbuf;
  GtkCssProvider       *cssprovider = gtk_css_provider_new();
  
  std::list <std::list <EF_Point *> *> *forme;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC_PE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Section Personnalisee"); )
  
  if (!EF_gtk_section_personnalisee_recupere_donnees (p,
                                                      &j,
                                                      &iy,
                                                      &iz,
                                                      &vy,
                                                      &vyp,
                                                      &vz,
                                                      &vzp,
                                                      &s,
                                                      &forme,
                                                      &ok_forme,
                                                      &nom,
                                                      &description))
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
               UI_SEC_PE.builder, "EF_section_personnalisee_button_add_edit")),
                              FALSE);
  }
  else
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
               UI_SEC_PE.builder, "EF_section_personnalisee_button_add_edit")),
                              TRUE);
  }
  
  if (!ok_forme)
  {
    gtk_css_provider_load_from_data (cssprovider,
       "GtkLabel {\n"
       "  font-weight: bold;\n"
       "  color: #FF0000;\n"
       "}\n", -1, NULL);
  }
  else
  {
    gtk_css_provider_load_from_data (cssprovider,
       "GtkLabel {\n"
       "  font-weight: normal;\n"
       "  color: #000000;\n"
       "}\n", -1, NULL);
  }
  gtk_style_context_add_provider (gtk_widget_get_style_context (GTK_WIDGET (
                                     gtk_builder_get_object (UI_SEC_PE.builder,
                                     "EF_section_personnalisee_label_forme"))),
                                  GTK_STYLE_PROVIDER (cssprovider),
                                  GTK_STYLE_PROVIDER_PRIORITY_USER);
  g_object_unref (cssprovider);
  
  section.type = SECTION_PERSONNALISEE;
  section.data = &data;
  data.forme.assign (forme->begin (), forme->end ());
  
  BUG (pixbuf = EF_gtk_sections_dessin (&section, 32, 32),
       ,
       for_each (forme->begin (),
                 forme->end (),
                 std::default_delete <std::list <EF_Point *> > ());
       delete forme; )
  
  gtk_image_set_from_pixbuf (GTK_IMAGE (gtk_builder_get_object (
                   UI_SEC_PE.builder, "EF_section_personnalisee_image_forme")),
                             pixbuf);
  
  g_object_unref (pixbuf);
  
  for_each (forme->begin (),
            forme->end (),
            std::default_delete <std::list <EF_Point *> > ());
  delete forme;
  
  return;
}


/**
 * \brief Ferme la fenêtre en ajoutant la section.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
void
EF_gtk_section_personnalisee_ajouter_clicked (GtkButton *button,
                                              Projet    *p)
{
  double      j, iy, iz, vy, vyp, vz, vzp, s;
  bool        ok_forme;
  std::string texte, description;
  
  std::list <std::list <EF_Point *> *> *forme;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC_PE.builder,
           ,
           (gettext("La fenêtre graphique %s n'est pas initialisée.\n"),
                    "Ajout Section Personnalisee"); )
  
  if (!EF_gtk_section_personnalisee_recupere_donnees (p,
                                                      &j,
                                                      &iy,
                                                      &iz,
                                                      &vy,
                                                      &vyp,
                                                      &vz,
                                                      &vzp,
                                                      &s,
                                                      &forme,
                                                      &ok_forme,
                                                      &texte,
                                                      &description))
  {
    for_each (forme->begin (),
              forme->end (),
              std::default_delete <std::list <EF_Point *> > ());
    delete forme;
    return;
  }
  
  BUG (EF_sections_personnalisee_ajout (p,
                                        &texte,
                                        &description,
                                        m_f (j, FLOTTANT_UTILISATEUR),
                                        m_f (iy, FLOTTANT_UTILISATEUR),
                                        m_f (iz, FLOTTANT_UTILISATEUR),
                                        m_f (vy, FLOTTANT_UTILISATEUR),
                                        m_f (vyp, FLOTTANT_UTILISATEUR),
                                        m_f (vz, FLOTTANT_UTILISATEUR),
                                        m_f (vzp, FLOTTANT_UTILISATEUR),
                                        m_f (s, FLOTTANT_UTILISATEUR),
                                        forme),
      ,
      for_each (forme->begin (),
                forme->end (),
                std::default_delete <std::list <EF_Point *> > ());
        delete forme; )
  
  UI_SEC_PE.keep = true;
  gtk_widget_destroy (UI_SEC_PE.window);
  
  return;
}


/**
 * \brief Ferme la fenêtre en appliquant les modifications.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
void
EF_gtk_section_personnalisee_modifier_clicked (GtkButton *button,
                                               Projet    *p)
{
  double      j, iy, iz, vy, vyp, vz, vzp, s;
  bool        ok_forme;
  std::string texte, description;
  
  std::list <std::list <EF_Point *> *> *forme;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC_PE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Section Personnalisee"); )
  
  if (!EF_gtk_section_personnalisee_recupere_donnees (p,
                                                      &j,
                                                      &iy,
                                                      &iz,
                                                      &vy,
                                                      &vyp,
                                                      &vz,
                                                      &vzp,
                                                      &s,
                                                      &forme,
                                                      &ok_forme,
                                                      &texte,
                                                      &description))
  {
    for_each (forme->begin (),
              forme->end (),
              std::default_delete <std::list <EF_Point *> > ());
    delete forme;
    return;
  }
  
  BUG (EF_sections_personnalisee_modif (p,
                                        UI_SEC_PE.section,
                                        &texte,
                                        &description,
                                        m_f (j, FLOTTANT_UTILISATEUR),
                                        m_f (iy, FLOTTANT_UTILISATEUR),
                                        m_f (iz, FLOTTANT_UTILISATEUR),
                                        m_f (vy, FLOTTANT_UTILISATEUR),
                                        m_f (vyp, FLOTTANT_UTILISATEUR),
                                        m_f (vz, FLOTTANT_UTILISATEUR),
                                        m_f (vzp, FLOTTANT_UTILISATEUR),
                                        m_f (s, FLOTTANT_UTILISATEUR),
                                        forme),
      ,
      for_each (forme->begin (),
                forme->end (),
                std::default_delete <std::list <EF_Point *> > ());
        delete forme; )
  
  UI_SEC_PE.keep = true;
  gtk_widget_destroy (UI_SEC_PE.window);
  
  return;
}


/**
 * \brief Affiche la première colonne qui indique soit le numéro du groupe de
 *        points, soit le numéro du point.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_section_personnalisee_render_0 (GtkTreeViewColumn *tree_column,
                                       GtkCellRenderer   *cell,
                                       GtkTreeModel      *tree_model,
                                       GtkTreeIter       *iter,
                                       gpointer           data2)
{
  GtkTreeIter iter2;
  char       *tmp;
  Projet     *p = (Projet *) data2;
  int32_t     nombre;
  std::string rendu;
  
  // C'est une ligne de groupe de points
  if (!gtk_tree_model_iter_parent (UI_SEC_PE.model, &iter2, iter))
  {
    tmp = gtk_tree_model_get_string_from_iter (UI_SEC_PE.model, iter);
    nombre = atoi (tmp);
    g_free (tmp);
    rendu = format (gettext ("Groupe %d"), nombre + 1);
    g_object_set (cell, "text", rendu.c_str (), NULL);
  }
  else
  {
    char *tmp2;
    
    tmp = gtk_tree_model_get_string_from_iter (UI_SEC_PE.model, iter);
    tmp2 = strchr (tmp, ':') + 1;
    nombre = atoi (tmp2);
    g_free (tmp);
    g_free (tmp2);
    rendu = format (gettext ("Point %d"), nombre + 1);
    g_object_set (cell, "text", rendu.c_str (), NULL);
  }
  
  return;
}


/**
 * \brief Affiche les coordonnées en x du point.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_section_personnalisee_render_1 (GtkTreeViewColumn *tree_column,
                                       GtkCellRenderer   *cell,
                                       GtkTreeModel      *tree_model,
                                       GtkTreeIter       *iter,
                                       gpointer           data2)
{
  EF_Point   *point;
  std::string tmp;
  
  gtk_tree_model_get (tree_model, iter, 0, &point, -1);
  
  if (point == NULL)
  {
    g_object_set (cell, "text", "", NULL);
    return;
  }
  
  conv_f_c (point->x, &tmp, DECIMAL_DISTANCE);
  
  g_object_set (cell, "text", tmp.c_str (), NULL);
  
  return;
}


/**
 * \brief Affiche les coordonnées en y du point.
 * \param tree_column : composant à l'origine de l'évènement,
 * \param cell : la cellule en cours d'édition,
 * \param tree_model : le mode en cours d'édition,
 * \param iter : la ligne en cours d'édition,
 * \param data2 : la variable projet.
 * \return Rien.
 */
void
EF_gtk_section_personnalisee_render_2 (GtkTreeViewColumn *tree_column,
                                       GtkCellRenderer   *cell,
                                       GtkTreeModel      *tree_model,
                                       GtkTreeIter       *iter,
                                       gpointer           data2)
{
  EF_Point   *point;
  std::string tmp;
  
  gtk_tree_model_get (tree_model, iter, 0, &point, -1);
  
  if (point == NULL)
  {
    g_object_set (cell, "text", "", NULL);
    return;
  }
  
  conv_f_c (point->y, &tmp, DECIMAL_DISTANCE);
  
  g_object_set (cell, "text", tmp.c_str (), NULL);
  
  return;
}


/**
 * \brief Permet de activer/désactiver les boutons + et - correspondant au
 *        treeview en fonction de la selection.
 * \param treeselection : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_section_personnalisee_select_change (GtkTreeSelection *treeselection,
                                            Projet           *p)
{
  GtkTreeIter iter, iter2;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC_PE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Section Personnalisee"); )
  
  if (!gtk_tree_selection_get_selected (treeselection, NULL, &iter))
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
        UI_SEC_PE.builder, "EF_section_personnalisee_button_treeview_remove")),
                              FALSE);
  }
  else
  {
    gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
        UI_SEC_PE.builder, "EF_section_personnalisee_button_treeview_remove")),
                              TRUE);
    // Possède un parent donc les coordonnées peuvent être éditées.
    if (gtk_tree_model_iter_parent (UI_SEC_PE.model, &iter2, &iter))
    {
      g_object_set (gtk_builder_get_object (UI_SEC_PE.builder,
                                            "EF_section_treeview_cell1"),
                    "editable", TRUE,
                    NULL);
      g_object_set (gtk_builder_get_object (UI_SEC_PE.builder,
                                            "EF_section_treeview_cell2"),
                    "editable", TRUE,
                    NULL);
    }
    else
    {
      g_object_set (gtk_builder_get_object (UI_SEC_PE.builder,
                                            "EF_section_treeview_cell1"),
                    "editable", FALSE,
                    NULL);
      g_object_set (gtk_builder_get_object (UI_SEC_PE.builder,
                                            "EF_section_treeview_cell2"),
                    "editable", FALSE,
                    NULL);
    }
  }
  
  return;
}


/**
 * \brief Ajoute une ligne dans le treeview contenant la liste des points.
 * \param widget : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - en cas d'erreur d'allocation mémoire,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_section_personnalisee_treeview_add (GtkToolButton *widget,
                                           Projet        *p)
{
  GtkTreeIter iter;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC_PE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Section Personnalisee"); )
  
  // On ajoute un groupe de points
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
     gtk_builder_get_object (UI_SEC_PE.builder, "EF_section_treeview_select")),
                                        NULL,
                                        &iter))
  {
    gtk_tree_store_append (GTK_TREE_STORE (UI_SEC_PE.model), &iter, NULL);
    gtk_tree_store_set (GTK_TREE_STORE (UI_SEC_PE.model), &iter, 0, NULL, -1);
  }
  else
  {
    GtkTreeIter iter2;
    GtkTreeIter iter3;
    EF_Point   *point;
    
    point = new EF_Point;
    point->x = m_f (0., FLOTTANT_UTILISATEUR);
    point->y = m_f (0., FLOTTANT_UTILISATEUR);
    point->z = m_f (0., FLOTTANT_UTILISATEUR);
    
    // Si iter est un parent
    if (!gtk_tree_model_iter_parent (UI_SEC_PE.model, &iter2, &iter))
    {
      // Si la liste est vide, on ajout un point à la liste afin que la section
      // soit toujours fermée.
      if (!gtk_tree_model_iter_has_child (UI_SEC_PE.model, &iter))
      {
        gtk_tree_store_append (GTK_TREE_STORE (UI_SEC_PE.model),
                               &iter2,
                               &iter);
        gtk_tree_store_set (GTK_TREE_STORE (UI_SEC_PE.model),
                            &iter2,
                            0, point,
                            -1);
      }
      gtk_tree_model_iter_nth_child (UI_SEC_PE.model,
                                     &iter3,
                                     &iter,
                                     gtk_tree_model_iter_n_children (
                                                               UI_SEC_PE.model,
                                                                   &iter) - 1);
      gtk_tree_store_insert_before (GTK_TREE_STORE (UI_SEC_PE.model),
                                    &iter2,
                                    &iter,
                                    &iter3);
      gtk_tree_store_set (GTK_TREE_STORE (UI_SEC_PE.model),
                          &iter2,
                          0, point,
                          -1);
    }
    else
    {
      GtkTreeIter iter_test = iter;
      
      if (gtk_tree_model_iter_next (UI_SEC_PE.model, &iter_test))
      {
        gtk_tree_store_insert_after (GTK_TREE_STORE (UI_SEC_PE.model),
                                     &iter3,
                                     &iter2,
                                     &iter);
      }
      else
      {
        gtk_tree_store_insert_before (GTK_TREE_STORE (UI_SEC_PE.model),
                                      &iter3,
                                      &iter2,
                                      &iter);
      }
      gtk_tree_store_set (GTK_TREE_STORE (UI_SEC_PE.model),
                          &iter3,
                          0, point,
                          -1);
    }
  }
  
  EF_gtk_section_personnalisee_check (NULL, p);
  
  return;
}


/**
 * \brief Permet de supprimer une ligne dans le treeview contenant la liste des
 *        points.
 * \param widget : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_section_personnalisee_treeview_remove (GtkToolButton *widget,
                                              Projet        *p)
{
  // Pour mémoire, pendant toute la durée d'affichage de la fenêtre, les points
  // sont stockés dans des points alloués via malloc et accessible uniquement
  // via le treeview. A la fermeture de la fenêtre, si la forme n'est pas
  // stockée, tous les points sont libérés. Si la forme est utilisée, les
  // points sont intégrés dans une liste de liste de points.
  GtkTreeIter iter;
  EF_Point   *point;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC_PE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Section Personnalisee"); )
  
  if (!gtk_tree_selection_get_selected (GTK_TREE_SELECTION (
     gtk_builder_get_object (UI_SEC_PE.builder, "EF_section_treeview_select")),
                                        NULL,
                                        &iter))
  {
    return;
  }
  
  gtk_tree_model_get (UI_SEC_PE.model, &iter, 0, &point, -1);
  
  // C'est un parent
  if (point == NULL)
  {
    GtkTreeIter iter2;
    
    if (gtk_tree_model_iter_children (UI_SEC_PE.model, &iter2, &iter))
    {
      do
      {
        gtk_tree_model_get (UI_SEC_PE.model, &iter, 0, &point, -1);
        free (point);
      } while (gtk_tree_model_iter_next (UI_SEC_PE.model, &iter2));
    }
    gtk_tree_store_remove (GTK_TREE_STORE (UI_SEC_PE.model), &iter);
  }
  else
  {
    GtkTreeIter iter2 = iter;
    GtkTreeIter iter3 = iter;
    
    // Si ce n'est pas le premiere ou le dernier point
    if ((gtk_tree_model_iter_next (UI_SEC_PE.model, &iter2)) &&
        (gtk_tree_model_iter_previous (UI_SEC_PE.model, &iter3)))
    {
      free (point);
      gtk_tree_store_remove (GTK_TREE_STORE (UI_SEC_PE.model), &iter);
    }
    else
    {
      GtkTreeIter parent;
      gint        n_child;
      
      // On récupère le premier et dernier point qui est le même
      gtk_tree_model_iter_parent (UI_SEC_PE.model, &parent, &iter);
      gtk_tree_model_iter_children (UI_SEC_PE.model, &iter2, &parent);
      n_child = gtk_tree_model_iter_n_children (UI_SEC_PE.model, &parent);
      gtk_tree_model_iter_nth_child (UI_SEC_PE.model,
                                     &iter3,
                                     &parent,
                                     n_child - 1);
      
      // Un seul noeud, on supprime les deux.
      if (n_child == 2)
      {
        gtk_tree_store_remove (GTK_TREE_STORE (UI_SEC_PE.model), &iter3);
        gtk_tree_store_remove (GTK_TREE_STORE (UI_SEC_PE.model), &iter2);
        free (point);
      }
      else
      {
        EF_Point *point2;
        
        // On supprime la première ligne
        if (gtk_tree_model_iter_next (UI_SEC_PE.model, &iter))
        {
          // Alors, c'est le deuxième point qui devient le point de base
          // et on supprime la première ligne.
          gtk_tree_model_get (UI_SEC_PE.model, &iter, 0, &point2, -1);
          gtk_tree_store_set (GTK_TREE_STORE (UI_SEC_PE.model),
                              &iter3,
                              0, point2,
                              -1);
          gtk_tree_store_remove (GTK_TREE_STORE (UI_SEC_PE.model), &iter2);
        }
        else
        {
          // Alors, c'est l'avant dernier point qui devient le point de base
          // et on supprime la dernière ligne.
          iter = iter3;
          gtk_tree_model_iter_previous (UI_SEC_PE.model, &iter);
          gtk_tree_model_get (UI_SEC_PE.model, &iter, 0, &point2, -1);
          gtk_tree_store_set (GTK_TREE_STORE (UI_SEC_PE.model),
                              &iter2,
                              0, point2,
                              -1);
          gtk_tree_store_remove (GTK_TREE_STORE (UI_SEC_PE.model), &iter3);
        }
        free (point);
      }
    }
    
  }
  
  EF_gtk_section_personnalisee_check (NULL, p);
  
  return;
}


/**
 * \brief Supprime un point ou un groupe de points si la touche SUPPR est
 *        appuyée.
 * \param treeview : composant à l'origine de l'évènement,
 * \param event : Caractéristique de l'évènement,
 * \param p : la variable projet.
 * \return TRUE si la touche SUPPR est pressée, FALSE sinon.\n
 *   Echec : FALSE :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
gboolean
EF_gtk_section_personnalisee_treeview_key_press (GtkTreeView *treeview,
                                                 GdkEvent    *event,
                                                 Projet      *p)
{
  BUGPARAMCRIT (p, "%p", p, FALSE)
  BUGCRIT (UI_SEC_PE.builder,
           FALSE,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Section Personnalisee"); )
  
  if (event->key.keyval == GDK_KEY_Delete)
  {
    if (!gtk_tree_selection_get_selected (gtk_tree_view_get_selection (
                                                                     treeview),
                                          NULL,
                                          NULL))
    {
      return FALSE;
    }
    
    EF_gtk_section_personnalisee_treeview_remove (NULL, p);
    
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/**
 * \brief Change la coordonnée en x du noeud de la forme de la section.
 * \param cell : cellule en cours,
 * \param path_string : path de la ligne en cours,
 * \param new_text : nouvelle valeur,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_section_personnalisee_edit_x (GtkCellRendererText *cell,
                                     gchar               *path_string,
                                     gchar               *new_text,
                                     Projet              *p)
{
  GtkTreeIter iter;
  double      conversion;
  EF_Point   *point;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC_PE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Section Personnalisee"); )
  
  gtk_tree_model_get_iter_from_string (UI_SEC_PE.model, &iter, path_string);
  gtk_tree_model_get (UI_SEC_PE.model, &iter, 0, &point, -1);
  
  conversion = common_text_str_to_double (new_text,
                                          -INFINITY, false,
                                          INFINITY, false);
  if (!std::isnan (conversion))
  {
    point->x = m_f (conversion, FLOTTANT_UTILISATEUR);
  }
  
  EF_gtk_section_personnalisee_check (NULL, p);
  
  return;
}


/**
 * \brief Change la coordonnée en y du noeud de la forme de la section.
 * \param cell : cellule en cours,
 * \param path_string : path de la ligne en cours,
 * \param new_text : nouvelle valeur,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_section_personnalisee_edit_y (GtkCellRendererText *cell,
                                     gchar               *path_string,
                                     gchar               *new_text,
                                     Projet              *p)
{
  GtkTreeIter iter;
  double      conversion;
  EF_Point   *point;
  
  BUGPARAMCRIT (p, "%p", p, )
  BUGCRIT (UI_SEC_PE.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Section Personnalisee"); )
  
  gtk_tree_model_get_iter_from_string (UI_SEC_PE.model, &iter, path_string);
  gtk_tree_model_get (UI_SEC_PE.model, &iter, 0, &point, -1);
  
  conversion = common_text_str_to_double (new_text,
                                          -INFINITY, false,
                                          INFINITY, false);
  if (!std::isnan (conversion))
  {
    point->y = m_f (conversion, FLOTTANT_UTILISATEUR);
  }
  
  EF_gtk_section_personnalisee_check (NULL, p);
  
  return;
}


/**
 * \brief Affichage de la fenêtre permettant de créer ou modifier une section
 *        de type personnalisé.
 * \param p : la variable projet,
 * \param section : section à modifier. NULL si nouvelle section,
 * \return
 *   Succès : true.\n
 *   Echec : false :
 *     - p == NULL,
 *     - en cas d'erreur d'allocation mémoire,
 *     - interface graphique impossible à générer.
 */
bool
EF_gtk_section_personnalisee (Projet  *p,
                              Section *section)
{
  BUGPARAM (p, "%p", p, false)
  
  if (UI_SEC_PE.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_SEC_PE.window));
    if (UI_SEC_PE.section == section)
    {
      return true;
    }
  }
  else
  {
    UI_SEC_PE.builder = gtk_builder_new ();
    BUGCRIT (gtk_builder_add_from_resource (UI_SEC_PE.builder,
                           "/org/2lgc/codegui/ui/EF_sections_personnalisee.ui",
                                            NULL) != 0,
             false,
             (gettext ("La génération de la fenêtre %s a échouée.\n"),
                       "Ajout Section Personnalisee"); )
    gtk_builder_connect_signals (UI_SEC_PE.builder, p);
    UI_SEC_PE.window = GTK_WIDGET (gtk_builder_get_object (UI_SEC_PE.builder,
                                           "EF_section_personnalisee_window"));
    UI_SEC_PE.model = GTK_TREE_MODEL (gtk_builder_get_object (
                     UI_SEC_PE.builder, "EF_section_personnalisee_treestore"));
    UI_SEC_PE.keep = false;
  }
  
  if (section == NULL)
  {
    gtk_window_set_title (GTK_WINDOW (UI_SEC_PE.window),
                          gettext ("Ajout d'une section personnalisée"));
    UI_SEC_PE.section = NULL;
    
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object (
               UI_SEC_PE.builder, "EF_section_personnalisee_button_add_edit")),
                          gettext ("_Ajouter"));
    g_signal_connect (gtk_builder_get_object (UI_SEC_PE.builder,
                                   "EF_section_personnalisee_button_add_edit"),
                      "clicked",
                     G_CALLBACK (EF_gtk_section_personnalisee_ajouter_clicked),
                      p);
    EF_gtk_section_personnalisee_check (NULL, p);
  }
  else
  {
    std::string            tmp;
    Section_Personnalisee *data;
    
    std::list <std::list <EF_Point *> *>::iterator it;
    
    gtk_window_set_title (GTK_WINDOW (UI_SEC_PE.window),
                          gettext ("Modification d'une section personnalisée"));
    UI_SEC_PE.section = section;
    BUGCRIT (UI_SEC_PE.section->type == SECTION_PERSONNALISEE,
             false,
             (gettext ("La section à modifier n'est pas personnalisée.\n")); )
    data = (Section_Personnalisee *) UI_SEC_PE.section->data;
    
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                     gtk_builder_get_object (UI_SEC_PE.builder,
                                    "EF_section_personnalisee_textview_nom"))),
                              UI_SEC_PE.section->nom.c_str (),
                              -1);
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (
                                     gtk_builder_get_object (UI_SEC_PE.builder,
                            "EF_section_personnalisee_textview_description"))),
                              data->description.c_str (),
                              -1);
    conv_f_c (data->j, &tmp, DECIMAL_M4);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                      UI_SEC_PE.builder, "EF_section_personnalisee_buffer_j")),
                              tmp.c_str (),
                              -1);
    conv_f_c (data->iy, &tmp, DECIMAL_M4);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                     UI_SEC_PE.builder, "EF_section_personnalisee_buffer_iy")),
                              tmp.c_str (),
                              -1);
    conv_f_c (data->iz, &tmp, DECIMAL_M4);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                     UI_SEC_PE.builder, "EF_section_personnalisee_buffer_iz")),
                              tmp.c_str (),
                              -1);
    conv_f_c (data->vy, &tmp, DECIMAL_DISTANCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                     UI_SEC_PE.builder, "EF_section_personnalisee_buffer_vy")),
                              tmp.c_str (),
                              -1);
    conv_f_c (data->vyp, &tmp, DECIMAL_DISTANCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                    UI_SEC_PE.builder, "EF_section_personnalisee_buffer_vyp")),
                              tmp.c_str (),
                              -1);
    conv_f_c (data->vz, &tmp, DECIMAL_DISTANCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                     UI_SEC_PE.builder, "EF_section_personnalisee_buffer_vz")),
                              tmp.c_str (),
                              -1);
    conv_f_c (data->vzp, &tmp, DECIMAL_DISTANCE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                    UI_SEC_PE.builder, "EF_section_personnalisee_buffer_vzp")),
                              tmp.c_str (),
                              -1);
    conv_f_c (data->s, &tmp, DECIMAL_SURFACE);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gtk_builder_get_object (
                      UI_SEC_PE.builder, "EF_section_personnalisee_buffer_s")),
                              tmp.c_str (),
                              -1);
    
    it = data->forme.begin ();
    while (it != data->forme.end ())
    {
      GtkTreeIter iter, iter_last;
      EF_Point   *point_bis;
      
      std::list <EF_Point *> *forme_e = *it;
      std::list <EF_Point *>::iterator   it2 = forme_e->begin ();
      
      gtk_tree_store_append (GTK_TREE_STORE (UI_SEC_PE.model), &iter, NULL);
      gtk_tree_store_set (GTK_TREE_STORE (UI_SEC_PE.model),
                          &iter,
                          0, NULL,
                          -1);
      
      // On ajoute le dernier point de la forme qui est le même que le premier.
      if (it2 != forme_e->end ())
      {
        gtk_tree_store_append (GTK_TREE_STORE (UI_SEC_PE.model),
                               &iter_last,
                               &iter);
        point_bis = new EF_Point;
        memcpy (point_bis, *it2, sizeof (EF_Point));
        gtk_tree_store_set (GTK_TREE_STORE (UI_SEC_PE.model),
                            &iter_last,
                            0, point_bis,
                            -1);
      }
      
      while (it2 != forme_e->end ())
      {
        GtkTreeIter iter2;
        
        gtk_tree_store_insert_before (GTK_TREE_STORE (UI_SEC_PE.model),
                                      &iter2,
                                      &iter,
                                      &iter_last);
        // On force le dernier point à être le même que le premier.
        if (it2 != forme_e->begin ())
        {
          point_bis = new EF_Point;
          memcpy (point_bis, *it2, sizeof (EF_Point));
        }
        gtk_tree_store_set (GTK_TREE_STORE (UI_SEC_PE.model),
                            &iter2,
                            0, point_bis,
                            -1);
        
        ++it2;
      }
      
      ++it;
    }
    
    gtk_button_set_label (GTK_BUTTON (gtk_builder_get_object (
               UI_SEC_PE.builder, "EF_section_personnalisee_button_add_edit")),
                          gettext("_Modifier"));
    g_signal_connect (gtk_builder_get_object (UI_SEC_PE.builder,
                                   "EF_section_personnalisee_button_add_edit"),
                      "clicked",
                    G_CALLBACK (EF_gtk_section_personnalisee_modifier_clicked),
                      p);
    
    EF_gtk_section_personnalisee_check (NULL, p);
  }
  
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_SEC_PE.builder,
                                               "EF_section_treeview_column0")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_SEC_PE.builder,
                                                 "EF_section_treeview_cell0")),
    EF_gtk_section_personnalisee_render_0,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_SEC_PE.builder,
                                               "EF_section_treeview_column1")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_SEC_PE.builder,
                                                 "EF_section_treeview_cell1")),
    EF_gtk_section_personnalisee_render_1,
    p,
    NULL);
  gtk_tree_view_column_set_cell_data_func (
    GTK_TREE_VIEW_COLUMN (gtk_builder_get_object (UI_SEC_PE.builder,
                                               "EF_section_treeview_column2")),
    GTK_CELL_RENDERER (gtk_builder_get_object (UI_SEC_PE.builder,
                                                 "EF_section_treeview_cell2")),
    EF_gtk_section_personnalisee_render_2,
    p,
    NULL);
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_SEC_PE.window),
                                GTK_WINDOW (UI_GTK.window));
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
