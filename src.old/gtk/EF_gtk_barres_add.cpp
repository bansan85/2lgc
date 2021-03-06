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
#include <cmath>

#include <gtk/gtk.h>

#include "common_m3d.hpp"
#include "common_projet.hpp"
#include "common_math.hpp"
#include "common_erreurs.hpp"
#include "common_gtk.hpp"
#include "common_selection.hpp"
#include "EF_materiaux.hpp"
#include "EF_noeuds.hpp"
#include "EF_relachement.hpp"
#include "EF_sections.hpp"
#include "1992_1_1_barres.hpp"
#include "1992_1_1_materiaux.hpp"
#include "EF_gtk_barres_add.hpp"


GTK_WINDOW_CLOSE(ef, barres_add);


GTK_WINDOW_DESTROY (ef, barres_add, );


GTK_WINDOW_KEY_PRESS (ef, barres_add);


/**
 * \brief Aoute une nouvelle barre.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_barres_add_add_clicked (GtkButton *button,
                               Projet    *p)
{
  int32_t         type;
  char           *nom;
  Section        *section;
  EF_Materiau    *materiau;
  uint32_t        noeud_debut;
  uint32_t        noeud_fin;
  double          angle;
  EF_Relachement *relachement;
  uint16_t        nb_noeuds;
  GtkTreeModel   *model;
  GtkTreeIter     Iter;
  
  std::string     str_tmp;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_BARADD.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Barre"); )
  
  model = gtk_combo_box_get_model (GTK_COMBO_BOX (gtk_builder_get_object (
                    UI_BARADD.builder, "EF_gtk_barres_add_section_combobox")));
  type = gtk_combo_box_get_active (GTK_COMBO_BOX (gtk_builder_get_object (
                    UI_BARADD.builder, "EF_gtk_barres_add_section_combobox")));
  if (type == -1)
  {
    return;
  }
  gtk_tree_model_iter_nth_child (model, &Iter, NULL, type);
  gtk_tree_model_get (model, &Iter, 0, &nom, -1);
  str_tmp.assign (nom);
  BUG (section = EF_sections_cherche_nom (p, &str_tmp, true),
       ,
       free (nom); )
  free (nom);
  
  model = gtk_combo_box_get_model (GTK_COMBO_BOX (gtk_builder_get_object (
                   UI_BARADD.builder, "EF_gtk_barres_add_materiau_combobox")));
  type = gtk_combo_box_get_active (GTK_COMBO_BOX (gtk_builder_get_object (
                   UI_BARADD.builder, "EF_gtk_barres_add_materiau_combobox")));
  if (type == -1)
  {
    return;
  }
  gtk_tree_model_iter_nth_child (model, &Iter, NULL, type);
  gtk_tree_model_get (model, &Iter, 0, &nom, -1);
  str_tmp.assign (nom);
  BUG (materiau = EF_materiaux_cherche_nom (p, &str_tmp, true),
       ,
       free (nom); )
  free (nom);
  
  model = gtk_combo_box_get_model (GTK_COMBO_BOX (gtk_builder_get_object (
                UI_BARADD.builder, "EF_gtk_barres_add_relachement_combobox")));
  type = gtk_combo_box_get_active (GTK_COMBO_BOX (gtk_builder_get_object (
                UI_BARADD.builder, "EF_gtk_barres_add_relachement_combobox")));
  if (type == -1)
  {
    return;
  }
  else if (type == 0)
  {
    relachement = NULL;
  }
  else
  {
    gtk_tree_model_iter_nth_child (model, &Iter, NULL, type);
    gtk_tree_model_get (model, &Iter, 0, &nom, -1);
    str_tmp.assign (nom);
    BUG (relachement = EF_relachement_cherche_nom (p, &str_tmp, true),
         ,
         free (nom); )
    free (nom);
  }
  
  noeud_debut = conv_buff_u (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_BARADD.builder, "EF_gtk_barres_add_noeud1_buffer")),
                             0, true,
                             UINT_MAX, false);
  noeud_fin = conv_buff_u (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_BARADD.builder, "EF_gtk_barres_add_noeud2_buffer")),
                           0, true,
                           UINT_MAX, false);
  if ((EF_noeuds_cherche_numero (p, noeud_debut, true) == NULL) ||
      (EF_noeuds_cherche_numero (p, noeud_fin, true) == NULL))
  {
    return;
  }
  
  angle = conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                         UI_BARADD.builder, "EF_gtk_barres_add_angle_buffer")),
                       -360., false,
                       360., false);
  if (std::isnan (angle))
  {
    return;
  }
  
  type = gtk_combo_box_get_active (GTK_COMBO_BOX (gtk_builder_get_object (
                       UI_BARADD.builder, "EF_gtk_barres_add_type_combobox")));
  
  nb_noeuds = conv_buff_hu (GTK_TEXT_BUFFER (gtk_builder_get_object (
       UI_BARADD.builder, "EF_gtk_barres_add_nb_noeuds_intermediaire_buffer")),
                            0, true,
                            UINT16_MAX, false);
  BUG (nb_noeuds != UINT16_MAX, )
  
  BUG (_1992_1_1_barres_ajout (p,
                               (Type_Element) type,
                               section,
                               materiau,
                               EF_noeuds_cherche_numero (p, noeud_debut, true),
                               EF_noeuds_cherche_numero (p, noeud_fin, true),
                               m_f (angle, FLOTTANT_UTILISATEUR),
                               relachement,
                               nb_noeuds),
      )
  BUG (m3d_rafraichit (p), )
  
  return;
}


/**
 * \brief Vérifie à chaque modification d'un champ si la fenêtre possède toutes
 *        les informations correctes pour créer une barre et active / désactive
 *        en fonction le bouton ajouter.
 * \param widget : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique non initialisée.
 */
extern "C"
void
EF_gtk_barres_add_check_add (GtkWidget *widget,
                             Projet    *p)
{
  bool           ok = false;
  EF_Noeud      *noeud1, *noeud2;
  GtkTextIter    start, end;
  GtkTextBuffer *buff;
  
  BUGPARAM (p, "%p", p, )
  BUGCRIT (UI_BARADD.builder,
           ,
           (gettext ("La fenêtre graphique %s n'est pas initialisée.\n"),
                     "Ajout Barre"); )
  
  noeud1 = EF_noeuds_cherche_numero (
             p,
             conv_buff_u (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_BARADD.builder, "EF_gtk_barres_add_noeud1_buffer")),
             0, true,
             UINT_MAX, false),
           false);
  buff = GTK_TEXT_BUFFER (gtk_builder_get_object (UI_BARADD.builder,
                                           "EF_gtk_barres_add_noeud1_buffer"));
  gtk_text_buffer_get_iter_at_offset (buff, &start, 0);
  gtk_text_buffer_get_iter_at_offset (buff, &end, -1);
  gtk_text_buffer_remove_all_tags (buff, &start, &end);
  if (noeud1 == NULL)
  {
    gtk_text_buffer_apply_tag_by_name (buff, "mauvais", &start, &end);
  }
  else
  {
    gtk_text_buffer_apply_tag_by_name (buff, "OK", &start, &end);
  }
  noeud2 = EF_noeuds_cherche_numero (
             p,
             conv_buff_u (GTK_TEXT_BUFFER (gtk_builder_get_object (
                        UI_BARADD.builder, "EF_gtk_barres_add_noeud2_buffer")),
             0, true,
             UINT_MAX, false),
           false);
  buff = GTK_TEXT_BUFFER (gtk_builder_get_object (UI_BARADD.builder,
                                           "EF_gtk_barres_add_noeud2_buffer"));
  gtk_text_buffer_get_iter_at_offset (buff, &start, 0);
  gtk_text_buffer_get_iter_at_offset (buff, &end, -1);
  gtk_text_buffer_remove_all_tags (buff, &start, &end);
  if (noeud2 == NULL)
  {
    gtk_text_buffer_apply_tag_by_name (buff, "mauvais", &start, &end);
  }
  else
  {
    gtk_text_buffer_apply_tag_by_name (buff, "OK", &start, &end);
  }
  
  if ((gtk_combo_box_get_active (GTK_COMBO_BOX (gtk_builder_get_object (
              UI_BARADD.builder, "EF_gtk_barres_add_type_combobox"))) != -1) &&
      (gtk_combo_box_get_active (GTK_COMBO_BOX (gtk_builder_get_object (
           UI_BARADD.builder, "EF_gtk_barres_add_section_combobox"))) != -1) &&
      (gtk_combo_box_get_active (GTK_COMBO_BOX (gtk_builder_get_object (
          UI_BARADD.builder, "EF_gtk_barres_add_materiau_combobox"))) != -1) &&
      (gtk_combo_box_get_active (GTK_COMBO_BOX (gtk_builder_get_object (
       UI_BARADD.builder, "EF_gtk_barres_add_relachement_combobox"))) != -1) &&
      (noeud1 != NULL) &&
      (noeud2 != NULL) &&
      (noeud1 != noeud2))
  {
    ok = true;
  }
  
  if (conv_buff_u (GTK_TEXT_BUFFER (gtk_builder_get_object (UI_BARADD.builder,
                          "EF_gtk_barres_add_nb_noeuds_intermediaire_buffer")),
                   0, true,
                   UINT_MAX, false) == UINT_MAX)
  {
    ok = false;
  }
  if (std::isnan (conv_buff_d (GTK_TEXT_BUFFER (gtk_builder_get_object (
                         UI_BARADD.builder, "EF_gtk_barres_add_angle_buffer")),
             -360., false,
             360., false)))
  {
    ok = false;
  }
  
  gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                           UI_BARADD.builder, "EF_gtk_barres_add_button_add")),
                            ok);
  
  return;
}


/**
 * \brief Création de la fenêtre permettant d'ajouter des barres.
 * \param button : composant à l'origine de l'évènement,
 * \param p : la variable projet.
 * \return Rien.\n
 *   Echec :
 *     - p == NULL,
 *     - interface graphique impossible à générer.
 */
extern "C"
void
EF_gtk_barres_ajouter (GtkButton *button,
                       Projet    *p)
{
  BUGPARAM (p, "%p", p, )
  if (UI_BARADD.builder != NULL)
  {
    gtk_window_present (GTK_WINDOW (UI_BARADD.window));
    return;
  }
  
  UI_BARADD.builder = gtk_builder_new ();
  INFO (gtk_builder_add_from_resource (UI_BARADD.builder,
                                       "/org/2lgc/codegui/ui/EF_barres_add.ui",
                                       NULL) != 0,
        ,
        (gettext ("La génération de la fenêtre %s a échouée.\n"),
                  "Ajout Barre"); )
  gtk_builder_connect_signals (UI_BARADD.builder, p);
  
  UI_BARADD.window = GTK_WIDGET (gtk_builder_get_object (UI_BARADD.builder,
                                                      "EF_barres_add_window"));
  
  g_object_set (gtk_builder_get_object (UI_BARADD.builder,
                                        "EF_gtk_barres_add_type_combobox"),
                "model", UI_BAR.liste_types,
                NULL);
  g_object_set (gtk_builder_get_object (UI_BARADD.builder,
                                        "EF_gtk_barres_add_section_combobox"),
                "model", UI_SEC.liste_sections,
                NULL);
  g_object_set (gtk_builder_get_object (UI_BARADD.builder,
                                        "EF_gtk_barres_add_materiau_combobox"),
                "model", UI_MAT.liste_materiaux,
                NULL);
  g_object_set (gtk_builder_get_object (UI_BARADD.builder,
                                     "EF_gtk_barres_add_relachement_combobox"),
                "model", UI_REL.liste_relachements,
                NULL);
  
  if (!p->modele.barres.empty ())
  {
    std::string nb_barres;
    
    nb_barres = std::to_string ((*(--p->modele.barres.end ()))->numero + 1);
    gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (UI_BARADD.builder,
                                           "EF_gtk_barres_add_numero_label2")),
                        nb_barres.c_str ());
  }
  else
  {
    gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (UI_BARADD.builder,
                                           "EF_gtk_barres_add_numero_label2")),
                        "0");
  }
  
  gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (
                           UI_BARADD.builder, "EF_gtk_barres_add_button_add")),
                            FALSE);
  
  gtk_window_set_transient_for (GTK_WINDOW (UI_BARADD.window),
                                GTK_WINDOW (UI_GTK.window));
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
