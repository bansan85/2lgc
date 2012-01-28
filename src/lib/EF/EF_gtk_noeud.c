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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <math.h>

#include "common_projet.h"
#include "common_erreurs.h"
#include "common_gtk.h"
#include "common_maths.h"
#include "EF_noeud.h"
#include "EF_charge_noeud.h"
#include "1990_actions.h"
#include "common_selection.h"


void EF_gtk_noeud_annuler(GtkButton *button __attribute__((unused)), GtkWidget *fenetre)
/* Description : Ferme la fenêtre sans effectuer les modifications
 * Paramètres : GtkWidget *button : composant à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    BUGMSG(fenetre, , gettext("Paramètre incorrect\n"));
    gtk_widget_destroy(fenetre);
    return;
}


void EF_gtk_noeud_edit_barre(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, Projet *projet)
/* Description : Changement de la barre ou la position relative d'un noeud
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouvelle valeur,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_EF_Noeud       *gtk_noeud;
    GtkTreeModel            *model;
    GtkTreePath             *path;
    GtkTreeIter             iter;
    gint                    i;
    char                    *fake = (char*)malloc(sizeof(char)*(strlen(new_text)+1));
    double                  convertion;
    EF_Noeud                *noeud;
    gint                    column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->ef_donnees.noeuds, , gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(projet->ef_donnees.noeuds), , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_noeud.window, , gettext("Paramètre incorrect\n"));
    BUGMSG(fake, , gettext("Erreur d'allocation mémoire.\n"));
    
    gtk_noeud = &projet->list_gtk.ef_noeud;
    model = GTK_TREE_MODEL(gtk_noeud->tree_store_barre);
    path = gtk_tree_path_new_from_string(path_string);
    
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &i, -1);
    
    // On vérifie si le texte contient bien un nombre flottant
    BUG(noeud = EF_noeuds_cherche_numero(projet, i), );
    
    if (noeud->type == NOEUD_BARRE)
    {
        if (column == 6)
        {
            EF_Noeud_Barre  *info = noeud->data;
            
            if (sscanf(new_text, "%lf%s", &convertion, fake) == 1)
            {
                info->position_relative_barre = convertion;
                gtk_tree_store_set(gtk_noeud->tree_store_barre, &iter, column, convertion, -1);
            }
        }
        else
            BUGMSG(NULL, , gettext("Paramètre incorrect\n"));
    }
    else
        BUGMSG(NULL, , gettext("Paramètre incorrect\n"));
    
    free(fake);
    gtk_tree_path_free(path);
     
    return;
}


void EF_gtk_noeud_edit_pos_abs(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, Projet *projet)
/* Description : Changement de la position d'un noeud
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouvelle valeur,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_EF_Noeud       *gtk_noeud;
    GtkTreeModel            *model;
    GtkTreePath             *path;
    GtkTreeIter             iter;
    gint                    i;
    char                    *fake = (char*)malloc(sizeof(char)*(strlen(new_text)+1));
    double                  convertion;
    EF_Noeud                *noeud;
    gint                    column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->ef_donnees.noeuds, , gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(projet->ef_donnees.noeuds), , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_noeud.window, , gettext("Paramètre incorrect\n"));
    BUGMSG(fake, , gettext("Erreur d'allocation mémoire.\n"));
    
    gtk_noeud = &projet->list_gtk.ef_noeud;
    model = GTK_TREE_MODEL(gtk_noeud->tree_store_libre);
    path = gtk_tree_path_new_from_string(path_string);
    
    gtk_tree_model_get_iter(model, &iter, path);
    
    i = gtk_tree_path_get_indices(path)[0];
    
    // On vérifie si le texte contient bien un nombre flottant
    if (sscanf(new_text, "%lf%s", &convertion, fake) == 1)
    {
        // On modifie l'action
        BUG(noeud = EF_noeuds_cherche_numero(projet, i), );
        switch (noeud->type)
        {
            case NOEUD_LIBRE :
            {
                EF_Point    *point = noeud->data;
                
                switch (column)
                {
                    case 1:
                    {
                        point->x = convertion;
                        break;
                    }
                    case 2:
                    {
                        point->y = convertion;
                        break;
                    }
                    case 3:
                    {
                        point->z = convertion;
                        break;
                    }
                    default :
                    {
                        BUGMSG(NULL, , gettext("Paramètre incorrect\n"));
                        break;
                    }
                }
                break;
            }
            case NOEUD_BARRE :
            {
                BUGMSG(NULL, , gettext("Paramètre incorrect\n"));
                break;
            }
            default :
            {
                BUGMSG(NULL, , gettext("Paramètre incorrect\n"));
                break;
            }
        }
        
        // On modifie le tree-view-actions
        gtk_tree_store_set(gtk_noeud->tree_store_libre, &iter, column, convertion, -1);
    }
    
    free(fake);
    gtk_tree_path_free(path);
     
    return;
}


void EF_gtk_noeud_edit_pos_relat(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, Projet *projet)
/* Description : Changement de la position d'un noeud de type barre.
 * Paramètres : GtkCellRendererText *cell : cellule en cours,
 *            : gchar *path_string : path de la ligne en cours,
 *            : gchar *new_text : nouvelle valeur,
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_EF_Noeud       *gtk_noeud;
    GtkTreeModel            *model;
    GtkTreePath             *path;
    GtkTreeIter             iter;
    gint                    i;
    char                    *fake = (char*)malloc(sizeof(char)*(strlen(new_text)+1));
    double                  convertion;
    EF_Noeud                *noeud;
    gint                    column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->ef_donnees.noeuds, , gettext("Paramètre incorrect\n"));
    BUGMSG(list_size(projet->ef_donnees.noeuds), , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->list_gtk.ef_noeud.window, , gettext("Paramètre incorrect\n"));
    BUGMSG(fake, , gettext("Erreur d'allocation mémoire.\n"));
    
    gtk_noeud = &projet->list_gtk.ef_noeud;
    model = GTK_TREE_MODEL(gtk_noeud->tree_store_barre);
    path = gtk_tree_path_new_from_string(path_string);
    
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &i, -1);
    
    // On vérifie si le texte contient bien un nombre flottant
    if (sscanf(new_text, "%lf%s", &convertion, fake) == 1)
    {
        // On modifie l'action
        BUG(noeud = EF_noeuds_cherche_numero(projet, i), );
        
        switch (noeud->type)
        {
            case NOEUD_LIBRE :
            {
                BUGMSG(NULL, , gettext("Paramètre incorrect\n"));
                break;
            }
            case NOEUD_BARRE :
            {
                if (column == 6)
                {
                    EF_Noeud_Barre  *info = noeud->data;
                    
                    info->position_relative_barre = convertion;
                }
                else
                    BUGMSG(NULL, , gettext("Paramètre incorrect\n"));
                break;
            }
            default :
            {
                BUGMSG(NULL, , gettext("Paramètre incorrect\n"));
                break;
            }
        }
        
        // On modifie le tree-view-actions
        gtk_tree_store_set(gtk_noeud->tree_store_barre, &iter, column, convertion, -1);
    }
    
    free(fake);
    gtk_tree_path_free(path);
     
    return;
}


void EF_gtk_noeud(Projet *projet)
/* Description : Affichage de la fenêtre permettant de créer ou modifier la liste des noeuds.
 * Paramètres : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
{
    List_Gtk_EF_Noeud   *ef_gtk;
//    Charge_Noeud        *charge_noeud;
    GtkCellRenderer     *pCellRenderer;
/*    GtkTreeIter         iter; */
    GtkTreeViewColumn   *column;
    
    BUGMSG(projet, , gettext("Paramètre incorrect\n"));
    BUGMSG(projet->ef_donnees.noeuds, , gettext("Paramètre incorrect\n"));
    
    ef_gtk = &projet->list_gtk.ef_noeud;
    
    GTK_NOUVELLE_FENETRE(ef_gtk->window, gettext("Gestion des noeuds"), 600, 400)
    
    ef_gtk->table = gtk_table_new(2, 2, FALSE);
    gtk_container_add(GTK_CONTAINER(ef_gtk->window), ef_gtk->table);
    
    // Le notebook
    ef_gtk->notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(ef_gtk->notebook), GTK_POS_TOP);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->notebook, 0, 2, 0, 1, GTK_EXPAND|GTK_FILL, GTK_EXPAND|GTK_FILL, 0, 0);
    
    ef_gtk->table_noeud_libre = gtk_table_new(1, 1, FALSE);
    ef_gtk->label_noeud_libre = gtk_label_new(gettext("Noeuds libres"));
    gtk_notebook_insert_page(GTK_NOTEBOOK(ef_gtk->notebook), ef_gtk->table_noeud_libre, ef_gtk->label_noeud_libre, -1);
    
    ef_gtk->table_noeud_barre = gtk_table_new(1, 1, FALSE);
    ef_gtk->label_noeud_barre = gtk_label_new(gettext("Noeuds intermédiaires"));
    gtk_notebook_insert_page(GTK_NOTEBOOK(ef_gtk->notebook), ef_gtk->table_noeud_barre, ef_gtk->label_noeud_barre, -1);
    
    // Le treeview : 0 : numero, 1 : x, 2 : y, 3 : z, 4 : appui/
    ef_gtk->tree_store_libre = gtk_tree_store_new(5, G_TYPE_INT, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_STRING);
    ef_gtk->tree_view_libre = (GtkTreeView*)gtk_tree_view_new_with_model(GTK_TREE_MODEL(ef_gtk->tree_store_libre));
    ef_gtk->tree_select_libre = gtk_tree_view_get_selection(ef_gtk->tree_view_libre);
    ef_gtk->scroll_libre = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(ef_gtk->scroll_libre), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(ef_gtk->scroll_libre), GTK_WIDGET(ef_gtk->tree_view_libre));
    gtk_table_attach(GTK_TABLE(ef_gtk->table_noeud_libre), GTK_WIDGET(ef_gtk->scroll_libre), 0, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    // Colonne numéro
    pCellRenderer = gtk_cell_renderer_text_new();
    g_object_set(pCellRenderer, "background", "#EEEEEE", NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ef_gtk->tree_view_libre), -1, gettext("Numéro"), pCellRenderer, "text", 0, NULL);
    // Colonne x
    pCellRenderer = gtk_cell_renderer_text_new();
    g_object_set(pCellRenderer, "editable", TRUE, NULL);
    column = gtk_tree_view_column_new();
    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(EF_gtk_noeud_edit_pos_abs), projet);
    gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN(column), "x");
    gtk_tree_view_append_column(GTK_TREE_VIEW(ef_gtk->tree_view_libre), column);
    gtk_tree_view_column_pack_start(column, pCellRenderer, TRUE);
    g_object_set_data(G_OBJECT(pCellRenderer), "column", GINT_TO_POINTER(1));
    gtk_tree_view_column_set_cell_data_func(column, pCellRenderer, gtk_common_render_double, GINT_TO_POINTER(GTK_DECIMAL_DISTANCE), NULL);
    // Colonne y
    pCellRenderer = gtk_cell_renderer_text_new();
    g_object_set(pCellRenderer, "editable", TRUE, NULL);
    column = gtk_tree_view_column_new();
    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(EF_gtk_noeud_edit_pos_abs), projet);
    gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN(column), "y");
    gtk_tree_view_append_column(GTK_TREE_VIEW(ef_gtk->tree_view_libre), column);
    gtk_tree_view_column_pack_start(column, pCellRenderer, TRUE);
    g_object_set_data(G_OBJECT(pCellRenderer), "column", GINT_TO_POINTER(2));
    gtk_tree_view_column_set_cell_data_func(column, pCellRenderer, gtk_common_render_double, GINT_TO_POINTER(GTK_DECIMAL_DISTANCE), NULL);
    // Colonne z
    pCellRenderer = gtk_cell_renderer_text_new();
    g_object_set(pCellRenderer, "editable", TRUE, NULL);
    column = gtk_tree_view_column_new();
    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(EF_gtk_noeud_edit_pos_abs), projet);
    gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN(column), "z");
    gtk_tree_view_append_column(GTK_TREE_VIEW(ef_gtk->tree_view_libre), column);
    gtk_tree_view_column_pack_start(column, pCellRenderer, TRUE);
    g_object_set_data(G_OBJECT(pCellRenderer), "column", GINT_TO_POINTER(3));
    gtk_tree_view_column_set_cell_data_func(column, pCellRenderer, gtk_common_render_double, GINT_TO_POINTER(GTK_DECIMAL_DISTANCE), NULL);
    // Colonne Appui
    pCellRenderer = gtk_cell_renderer_combo_new();
    g_object_set(pCellRenderer, "editable", TRUE, "model", ef_gtk->liste_appuis, "text-column", 0, "has-entry", FALSE, NULL);
//    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(EF_gtk_noeud_edit_z), projet);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ef_gtk->tree_view_libre), -1, gettext("Appui"), pCellRenderer, "text", 4, NULL);
    
    // Le treeview : 0 : numero, 1 : x, 2 : y, 3 : z, 4 : appui, 5 : barre, 6 : position relative
    ef_gtk->tree_store_barre = gtk_tree_store_new(7, G_TYPE_INT, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_STRING, G_TYPE_INT, G_TYPE_DOUBLE);
    ef_gtk->tree_view_barre = (GtkTreeView*)gtk_tree_view_new_with_model(GTK_TREE_MODEL(ef_gtk->tree_store_barre));
    ef_gtk->tree_select_barre = gtk_tree_view_get_selection(ef_gtk->tree_view_barre);
    ef_gtk->scroll_barre = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(ef_gtk->scroll_barre), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(ef_gtk->scroll_barre), GTK_WIDGET(ef_gtk->tree_view_barre));
    gtk_table_attach(GTK_TABLE(ef_gtk->table_noeud_barre), GTK_WIDGET(ef_gtk->scroll_barre), 0, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    // Colonne numéro
    pCellRenderer = gtk_cell_renderer_text_new();
    g_object_set(pCellRenderer, "background", "#EEEEEE", NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ef_gtk->tree_view_barre), -1, gettext("Numéro"), pCellRenderer, "text", 0, NULL);
    // Colonne x
    pCellRenderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN(column), "x");
    gtk_tree_view_append_column(GTK_TREE_VIEW(ef_gtk->tree_view_barre), column);
    gtk_tree_view_column_pack_start(column, pCellRenderer, TRUE);
    g_object_set_data(G_OBJECT(pCellRenderer), "column", GINT_TO_POINTER(1));
    gtk_tree_view_column_set_cell_data_func(column, pCellRenderer, gtk_common_render_double, GINT_TO_POINTER(GTK_DECIMAL_DISTANCE), NULL);
    // Colonne y
    pCellRenderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN(column), "y");
    gtk_tree_view_append_column(GTK_TREE_VIEW(ef_gtk->tree_view_barre), column);
    gtk_tree_view_column_pack_start(column, pCellRenderer, TRUE);
    g_object_set_data(G_OBJECT(pCellRenderer), "column", GINT_TO_POINTER(2));
    gtk_tree_view_column_set_cell_data_func(column, pCellRenderer, gtk_common_render_double, GINT_TO_POINTER(GTK_DECIMAL_DISTANCE), NULL);
    // Colonne z
    pCellRenderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN(column), "z");
    gtk_tree_view_append_column(GTK_TREE_VIEW(ef_gtk->tree_view_barre), column);
    gtk_tree_view_column_pack_start(column, pCellRenderer, TRUE);
    g_object_set_data(G_OBJECT(pCellRenderer), "column", GINT_TO_POINTER(3));
    gtk_tree_view_column_set_cell_data_func(column, pCellRenderer, gtk_common_render_double, GINT_TO_POINTER(GTK_DECIMAL_DISTANCE), NULL);
    // Colonne Appui
    pCellRenderer = gtk_cell_renderer_combo_new();
    g_object_set(pCellRenderer, "editable", TRUE, "model", ef_gtk->liste_appuis, "text-column", 0, "has-entry", FALSE, NULL);
//    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(EF_gtk_noeud_edit_z), projet);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ef_gtk->tree_view_barre), -1, gettext("Appui"), pCellRenderer, "text", 4, NULL);
    // Colonne Barre
    pCellRenderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ef_gtk->tree_view_barre), -1, gettext("Barre"), pCellRenderer, "text", 5, NULL);
    // Colonne Position relative
    pCellRenderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new();
    g_object_set(pCellRenderer, "editable", TRUE, NULL);
    g_signal_connect(pCellRenderer, "edited", G_CALLBACK(EF_gtk_noeud_edit_pos_relat), projet);
    gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN(column), gettext("Position relative"));
    gtk_tree_view_append_column(GTK_TREE_VIEW(ef_gtk->tree_view_barre), column);
    gtk_tree_view_column_pack_start(column, pCellRenderer, TRUE);
    g_object_set_data(G_OBJECT(pCellRenderer), "column", GINT_TO_POINTER(6));
    gtk_tree_view_column_set_cell_data_func(column, pCellRenderer, gtk_common_render_double, GINT_TO_POINTER(GTK_DECIMAL_DISTANCE), NULL);
    
    // On ajoute les noeuds existants.
    if ((projet->ef_donnees.noeuds != NULL) && (list_size(projet->ef_donnees.noeuds) != 0))
    {
        list_mvfront(projet->ef_donnees.noeuds);
        do
        {
            EF_Noeud    *noeud = list_curr(projet->ef_donnees.noeuds);
            EF_Point    *point = EF_noeuds_renvoie_position(noeud);
            GtkTreeIter iter;
            
            BUG(point, );
            
            if (noeud->type == NOEUD_LIBRE)
            {
                gtk_tree_store_append(ef_gtk->tree_store_libre, &iter, NULL);
                gtk_tree_store_set(ef_gtk->tree_store_libre, &iter, 0, noeud->numero, 1, point->x, 2, point->y, 3, point->z, 4, (noeud->appui == NULL ? gettext("Aucun") : noeud->appui->nom), -1);
            }
            else if (noeud->type == NOEUD_BARRE)
            {
                EF_Noeud_Barre  *info = noeud->data;
                
                gtk_tree_store_append(ef_gtk->tree_store_barre, &iter, NULL);
                gtk_tree_store_set(ef_gtk->tree_store_barre, &iter, 0, noeud->numero, 1, point->x, 2, point->y, 3, point->z, 4, (noeud->appui == NULL ? gettext("Aucun") : noeud->appui->nom), 5, info->barre->numero, 6, info->position_relative_barre, -1);
            }
            
            free(point);
        }
        while (list_mvnext(projet->ef_donnees.noeuds) != NULL);
    }
    
    ef_gtk->button_valider = gtk_button_new_from_stock(GTK_STOCK_OK);
    //g_signal_connect(ef_gtk->button_fermer, "clicked", G_CALLBACK(EF_gtk_noeud_annuler), ef_gtk->window);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->button_valider, 0, 1, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    ef_gtk->button_annuler = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
    g_signal_connect(ef_gtk->button_annuler, "clicked", G_CALLBACK(EF_gtk_noeud_annuler), ef_gtk->window);
    gtk_table_attach(GTK_TABLE(ef_gtk->table), ef_gtk->button_annuler, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
    
    gtk_window_set_transient_for(GTK_WINDOW(ef_gtk->window), GTK_WINDOW(projet->list_gtk.comp.window));
    gtk_window_set_modal(GTK_WINDOW(ef_gtk->window), TRUE);
    gtk_widget_show_all(ef_gtk->window);
    
    return;
}


#endif