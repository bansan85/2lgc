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

#include <locale>
#include <memory>

#include <string.h>
#include <libxml/tree.h>

#include "common_erreurs.hpp"
#include "common_projet.hpp"
#include "common_tooltip.hpp"

/**
 * \brief Génère une fenêtre de type tooltip contenant les informations
 *        spécifiées par 'nom'. Les informations sont contenues dans le fichier
 *        DATADIR"/tooltips.xml" sous format XML.
 * \param nom : nom de la fenêtre.
 * \return
 *   Succès : Un pointeur vers la fenêtre.\n
 *   Échec : NULL :
 *     - en cas d'erreur d'allocation mémoire,
 *     - si le fichier n'est pas correct.
 */
GtkWindow *
common_tooltip_generation (xmlChar *nom)
{
  xmlDocPtr  doc;
  xmlNodePtr racine;
  xmlNodePtr n0;
  xmlChar   *nom1;
  
  // Ouverture et vérification du fichier DATADIR"/tooltips.xml" par rapport au
  // format XML.
  doc = xmlParseFile (DATADIR"/tooltips.xml");
  INFO (doc,
        NULL,
        (gettext ("Le fichier '%s' est introuvable ou corrompu.\n"),
                  DATADIR"/tooltips.xml"); )
  
  // Récupération du noeud racine.
  racine = xmlDocGetRootElement (doc);
  INFO (racine,
        NULL,
        (gettext ("Le fichier '%s' est vide.\n"), DATADIR"/tooltips.xml");
          xmlFreeDoc (doc);
          xmlCleanupParser (); )
  
  for (n0 = racine; n0 != NULL; n0 = n0->next)
  {
    // Vérification que le nom du noeud racine est bien "liste".
    if ((strcmp ((char *) n0->name, "liste") == 0) &&
        (n0->type == XML_ELEMENT_NODE) &&
        (n0->children != NULL))
    {
      xmlNodePtr n1;
      
      for (n1 = n0->children; n1 != NULL; n1 = n1->next)
      {
        // Vérification que chaque noeud enfant d'un noeud "liste" est un noeud
        // "tooltip".
        if ((strcmp ((char *) n1->name, "tooltip") == 0) &&
            (n1->type == XML_ELEMENT_NODE) &&
            (n1->children != NULL))
        {
          // Si la propriété "reference" d'un noeud "tooltip" correspond à
          // l'élément à afficher Alors
          nom1 = xmlGetProp (n1, BAD_CAST "reference");
          if (strcmp ((char *) nom1, (char *)nom) == 0)
          {
            GtkWidget *pwindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
            GtkWidget *pgrid = gtk_grid_new ();
            GtkWidget *w_prev = NULL;
            GdkRGBA    color;
            xmlNodePtr n2;
            
            //   Initialisation de la fenêtre graphique
            gtk_widget_show (pgrid);
            gtk_window_set_position (GTK_WINDOW (pwindow), GTK_WIN_POS_CENTER);
            gtk_window_set_title (GTK_WINDOW (pwindow), (char *) nom1);
            gtk_container_add (GTK_CONTAINER (pwindow), pgrid);
            gtk_window_set_decorated (GTK_WINDOW (pwindow), FALSE);
            color.red = 1.;
            color.green = 1.;
            color.blue = 0.75;
            color.alpha = 1.;
            gtk_widget_override_background_color (pwindow,
                                                  GTK_STATE_FLAG_NORMAL,
                                                  &color);
            
            for (n2 = n1->children; n2 != NULL; n2 = n2->next)
            {
              //   Pour chaque noeud enfant
              if (n2->type == XML_ELEMENT_NODE)
              {
                xmlChar *contenu = xmlNodeGetContent (n2);
                
                INFO (contenu,
                      NULL,
                      (gettext ("Le fichier '%s' est corrompu.\n"),
                               DATADIR"/tooltips.xml");
                        xmlFreeDoc (doc);
                        xmlCleanupParser (); )
                //     Si le nom du noeud est "image" Alors
                //       Insertion de l'image dans la fenêtre tooltip.
                //     FinSi
                if (strcmp ((char *) n2->name, "image") == 0)
                {
                  char      *nom_fichier;
                  GdkPixbuf *image;
                  GError    *error = NULL;
                  GtkWidget *element;
                  
                  BUGCRIT (nom_fichier = g_strdup_printf ("%s/%s",
                                                          DATADIR,
                                                          contenu),
                           NULL,
                           (gettext ("Erreur d'allocation mémoire.\n"));
                             xmlFree (contenu);
                             xmlFreeDoc (doc);
                             xmlCleanupParser (); )
                  BUGCRIT (image = gdk_pixbuf_new_from_file (nom_fichier,
                                                             &error),
                           NULL,
                           (gettext ("Impossible d'ouvrir l'image %s : %s.\n"),
                                     nom_fichier,
                                     error->message);
                             xmlFree (contenu);
                             xmlFreeDoc (doc);
                             xmlCleanupParser ();
                             free (nom_fichier); )
                  element = gtk_image_new_from_pixbuf (image);
                  g_object_unref (image);
                  free (nom_fichier);
                  gtk_misc_set_alignment (GTK_MISC (element), 0., 0.5);
                  if (w_prev == NULL)
                  {
                    gtk_grid_attach (GTK_GRID (pgrid), element, 0, 0, 1, 1);
                  }
                  else
                  {
                    gtk_grid_attach_next_to (GTK_GRID (pgrid),
                                             element,
                                             w_prev,
                                             GTK_POS_BOTTOM,
                                             1,
                                             1);
                  }
                  w_prev = element;
                  gtk_widget_show (element);
                }
                //     Si le nom du noeud est "texte" Alors
                //       Insertion du texte dans la fenêtre tooltip.
                //     FinSi
                else if (strcmp ((char *) n2->name, "texte") == 0)
                {
                  GtkWidget *element;
                  
                  element = gtk_label_new ((char *) contenu);
                  
                  gtk_label_set_line_wrap (GTK_LABEL (element), TRUE);
                  gtk_label_set_justify (GTK_LABEL (element),
                                         GTK_JUSTIFY_FILL);
                  gtk_misc_set_alignment (GTK_MISC (element), 0., 0.5);
                  if (w_prev == NULL)
                  {
                    gtk_grid_attach (GTK_GRID (pgrid), element, 0, 0, 1, 1);
                  }
                  else
                  {
                    gtk_grid_attach_next_to (GTK_GRID (pgrid),
                                             element,
                                             w_prev,
                                             GTK_POS_BOTTOM,
                                             1,
                                             1);
                  }
                  w_prev = element;
                  gtk_widget_show (element);
                }
                //     Si le nom du noeud est "dimensions" Alors
                //       Attribution des nouvelles dimensions de la fenêtre tooltip.
                //     FinSi
                else if (strcmp ((char *) n2->name, "dimensions") == 0)
                {
                  int16_t largeur, hauteur;
                  
                  std::unique_ptr <char> fake
                                    (new char [strlen ((char *) contenu) + 1]);
                  
                  if (sscanf ((char *) contenu,
                              "%hdx%hd %s",
                              &largeur,
                              &hauteur,
                              fake.get ()) != 2)
                  {
                    FAILINFO (NULL,
                              (gettext ("'%s' n'est pas de la forme 'largeurxhauteur'.\n"),
                                        (char *) contenu);
                                xmlFree (contenu);
                                xmlFreeDoc (doc);
                                xmlCleanupParser (); )
                  }
                  else
                  {
                    gtk_window_set_default_size (GTK_WINDOW (pwindow),
                                                 largeur,
                                                 hauteur);
                  }
                }
                xmlFree (contenu);
              }
              //   FinPour
            }
            xmlFreeDoc (doc);
            xmlCleanupParser ();
            return GTK_WINDOW (pwindow);
          }
          // FinSi
        }
      }
    }
  }
  
  xmlFreeDoc (doc);
  xmlCleanupParser ();
  FAILINFO (NULL,
            (gettext ("Le fichier '%s' est corrompu.\n"),
                     DATADIR"/tooltips.xml"); )
}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
