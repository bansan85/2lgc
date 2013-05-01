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
#include <M3d++.hpp>

extern "C" {

#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>
#include <string.h>
#include "common_projet.h"
#include "common_erreurs.h"
#include "common_math.h"
#include "common_selection.h"
#include "EF_noeuds.h"
#include "1992_1_1_barres.h"
#include "common_m3d.hpp"

gboolean m3d_init(Projet *projet)
/* Description : Initialise l'affichage graphique de la structure.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    // Trivial
    Gtk_m3d     *m3d;
    SGlobalData *global_data;
    CM3dLight   *light;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    m3d = &projet->list_gtk.m3d;
    m3d->drawing = gtk_drawing_area_new();
    BUGMSG(m3d->data = malloc(sizeof(SGlobalData)), FALSE, gettext("Erreur d'allocation mémoire.\n"));
    memset(m3d->data, 0, sizeof(SGlobalData));
    
    global_data = (SGlobalData*)m3d->data;
    global_data->scene = new CM3dScene();
    global_data->scene->reverse_y_axis();
    global_data->scene->show_repere(true, 1.1);
    global_data->scene->set_ambient_light(1.);
    global_data->scene->set_show_type(SOLID);
    
    light = new CM3dLight("lumiere 1", DIFFUS, 1);
    light->set_position(10., 20., -20.);
    global_data->scene->add_light(light);
    
    g_signal_connect(m3d->drawing, "draw", G_CALLBACK(m3d_draw), global_data);
    g_signal_connect(m3d->drawing, "configure-event", G_CALLBACK(m3d_configure_event), projet);
    
    BUG(projet_init_graphique(projet), FALSE);
    
    return TRUE;
}


gboolean m3d_configure_event(GtkWidget *drawing, GdkEventConfigure *ev, gpointer *data2)
/* Description : Configuration de la caméra en fonction de la taille du composant graphique.
 * Paramètres : GtkWidget *drawing : composant graphique,
 *              GdkEventConfigure *ev : caractéristique de l'évènement,
 *              gpointer *data2 : données SGlobalData.
 * Valeur renvoyée : FALSE.
 */
{
    Projet      *projet = (Projet*) data2;
    SGlobalData *data = (SGlobalData*)projet->list_gtk.m3d.data;
    
    if ((data->camera == NULL) && (projet->list_gtk.comp.window != NULL))
        BUG(m3d_camera_axe_x_z_y(projet), FALSE);
    
    data->camera->set_size_of_window(ev->width, ev->height);
    
    if (data->camera->get_window_height() < data->camera->get_window_width())
        data->camera->set_d(data->camera->get_window_height() / (2 * tan(data->camera->get_angle() / 2)));
    else
        data->camera->set_d(data->camera->get_window_width() / (2 * tan(data->camera->get_angle() / 2)));
    data->scene->rendering(data->camera);
    
    return FALSE;
}


gboolean m3d_draw(GtkWidget *drawing, GdkEventExpose* ev, gpointer *data)
/* Description : Rendu de l'image 3D dans le widget Zone-dessin.
 * Paramètres : GtkWidget *drawing : composant graphique,
 *              GdkEventConfigure *ev : caractéristique de l'évènement,
 *              gpointer *data2 : données SGlobalData.
 * Valeur renvoyée : FALSE.
 */
{
    SGlobalData *data2 = (SGlobalData*)data;

    data2->scene->show_to_GtkDrawingarea(drawing, data2->camera);
    
    return FALSE;
}


gboolean m3d_key_press(GtkWidget *widget, GdkEventKey *event, Projet *projet)
{
    SGlobalData *vue  = (SGlobalData*)projet->list_gtk.m3d.data;
    
    if (event->type == GDK_KEY_PRESS)
    {
        double  x1, y1, z1;
        double  x2, y2, z2;
        CM3dVertex  *vect;
        CM3dVertex S1;
        CM3dVertex som1_rotx;
        CM3dVertex som1_roty;
        
        switch (event->keyval)
        {
            case GDK_KEY_KP_Add :
            case GDK_KEY_plus :
            {
                vect = vue->camera->get_position();
                vect->get_coordinates(&x1, &y1, &z1);
                vect = vue->camera->get_target_vector();
                vect->get_coordinates(&x2, &y2, &z2);
                
                vue->camera->set_position(x1+x2, y1+y2, z1+z2);
                vect = vue->camera->get_target();
                vect->get_coordinates(&x1, &y1, &z1);
                vue->camera->set_target(x1+x2, y1+y2, z1+z2);
                
                BUG(m3d_rafraichit(projet), FALSE);
                
                break;
            }
            case GDK_KEY_KP_Subtract :
            case GDK_KEY_minus :
            {
                vect = vue->camera->get_position();
                vect->get_coordinates(&x1, &y1, &z1);
                vect = vue->camera->get_target_vector();
                vect->get_coordinates(&x2, &y2, &z2);
                
                vue->camera->set_position(x1-x2, y1-y2, z1-z2);
                vect = vue->camera->get_target();
                vect->get_coordinates(&x1, &y1, &z1);
                vue->camera->set_target(x1-x2, y1-y2, z1-z2);
                
                BUG(m3d_rafraichit(projet), FALSE);
                
                break;
            }
            case GDK_KEY_KP_Right :
            case GDK_KEY_Right :
            case GDK_KEY_d :
            case GDK_KEY_D :
            {
                S1.set_coordinates(1., 0., 0.);
                S1.y_rotate(&som1_roty, vue->camera->get_cosy(), vue->camera->get_siny());
                som1_roty.x_rotate(&som1_rotx, vue->camera->get_cosx(), vue->camera->get_sinx());
                som1_rotx.z_rotate(&S1, vue->camera->get_cosz(), vue->camera->get_sinz());
                S1.get_coordinates(&x2,&y2,&z2);
                
                vect = vue->camera->get_position();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_position(x1+x2, y1+y2, z1+z2);
                
                vect = vue->camera->get_target();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_target(x1+x2, y1+y2, z1+z2);
                
                BUG(m3d_rafraichit(projet), FALSE);
                
                break;
            }
            case GDK_KEY_KP_Left :
            case GDK_KEY_Left :
            case GDK_KEY_q :
            case GDK_KEY_Q :
            {
                S1.set_coordinates(-1., 0., 0.);
                S1.y_rotate(&som1_roty, vue->camera->get_cosy(), vue->camera->get_siny());
                som1_roty.x_rotate(&som1_rotx, vue->camera->get_cosx(), vue->camera->get_sinx());
                som1_rotx.z_rotate(&S1, vue->camera->get_cosz(), vue->camera->get_sinz());
                S1.get_coordinates(&x2,&y2,&z2);
                
                vect = vue->camera->get_position();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_position(x1+x2, y1+y2, z1+z2);
                
                vect = vue->camera->get_target();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_target(x1+x2, y1+y2, z1+z2);
                
                BUG(m3d_rafraichit(projet), FALSE);
                
                break;
            }
            case GDK_KEY_KP_Up :
            case GDK_KEY_Up :
            case GDK_KEY_z :
            case GDK_KEY_Z :
            {
                if (vue->scene->y_axis_is_inverted())
                    S1.set_coordinates(0., 1., 0.);
                else
                    S1.set_coordinates(0., -1., 0.);
                S1.y_rotate(&som1_roty, vue->camera->get_cosy(), vue->camera->get_siny());
                som1_roty.x_rotate(&som1_rotx, vue->camera->get_cosx(), vue->camera->get_sinx());
                som1_rotx.z_rotate(&S1, vue->camera->get_cosz(), vue->camera->get_sinz());
                S1.get_coordinates(&x2,&y2,&z2);
                
                vect = vue->camera->get_position();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_position(x1+x2, y1+y2, z1+z2);
                
                vect = vue->camera->get_target();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_target(x1+x2, y1+y2, z1+z2);
                
                BUG(m3d_rafraichit(projet), FALSE);
                
                break;
            }
            case GDK_KEY_KP_Down :
            case GDK_KEY_Down :
            case GDK_KEY_s :
            case GDK_KEY_S :
            {
                if (vue->scene->y_axis_is_inverted())
                    S1.set_coordinates(0., -1., 0.);
                else
                    S1.set_coordinates(0., 1., 0.);
                S1.y_rotate(&som1_roty, vue->camera->get_cosy(), vue->camera->get_siny());
                som1_roty.x_rotate(&som1_rotx, vue->camera->get_cosx(), vue->camera->get_sinx());
                som1_rotx.z_rotate(&S1, vue->camera->get_cosz(), vue->camera->get_sinz());
                S1.get_coordinates(&x2,&y2,&z2);
                
                vect = vue->camera->get_position();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_position(x1+x2, y1+y2, z1+z2);
                
                vect = vue->camera->get_target();
                vect->get_coordinates(&x1,&y1,&z1);
                vue->camera->set_target(x1+x2, y1+y2, z1+z2);
                
                BUG(m3d_rafraichit(projet), FALSE);
                
                break;
            }
            default :
            {
                break;
            }
        }
    }
   
    
    return FALSE;
}



gboolean m3d_camera_axe_x_z_y(Projet *projet)
/* Description : Positionne la caméra pour voir toute la structure dans le plan xz vers la
 *               direction y.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE.
 *   Échec : FALSE :
 *             projet == NULL,
 *             projet->list_gtk.m3d == NULL,
 *             m3d->data == NULL,
 *             en cas d'erreur due à une fonction interne.
 */
{
    Gtk_m3d     *m3d;
    SGlobalData *vue;
    double      x, y, z; // Les coordonnées de la caméra
    double      x1, y1; // valeurs permettant de récupérer les coordonnées des vecteurs.
    EF_Noeud    *noeud; // Noeud en cours d'étude
    EF_Point    point; // Position du noeud en cours d'étude
    double      ph[3], pb[3], pg[3], pd[3]; // Les coordonnées des noeuds en 3D
    double      ph2, pb2, pg2, pd2; // Leur projection en 2D
    GList       *list_parcours; // Noeud en cours d'étude
    CM3dVertex  v1; // Vecteur permettant de créer le polygone
    CM3dPolygon *poly; // Polygone qui servira à obtenir la projection dans la fenêtre 2D.
    GtkAllocation   allocation; // Dimension de la fenêtre 2D.
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUGMSG(projet->list_gtk.comp.window, FALSE, gettext("La fenêtre graphique %s n'est pas initialisée.\n"), "principale");
    
    m3d = &projet->list_gtk.m3d;
    BUGMSG(m3d->data, FALSE, gettext("Paramètre %s incorrect.\n"), "m3d->data");
    vue = (SGlobalData*)m3d->data;
    
    // Aucune noeud, on ne fait rien
    if (projet->modele.noeuds == NULL)
        return TRUE;
    
    // Un seul noeud, on l'affiche en gros plan.
    noeud = (EF_Noeud*)projet->modele.noeuds->data;
    BUG(EF_noeuds_renvoie_position(noeud, &point), FALSE);
    
    ph[0] = common_math_get(point.x);
    ph[1] = common_math_get(point.y);
    ph[2] = common_math_get(point.z);
    pb[0] = common_math_get(point.x);
    pb[1] = common_math_get(point.y);
    pb[2] = common_math_get(point.z);
    pg[0] = common_math_get(point.x);
    pg[1] = common_math_get(point.y);
    pg[2] = common_math_get(point.z);
    pd[0] = common_math_get(point.x);
    pd[1] = common_math_get(point.y);
    pd[2] = common_math_get(point.z);
    
    // On cherche le xmin, xmax, zmin, zmax et ymin de l'ensemble des noeuds afin de définir
    // la position optimale de la caméra.
    gtk_widget_get_allocation(GTK_WIDGET(m3d->drawing), &allocation);
    ph2 = common_math_get(point.z);
    pb2 = common_math_get(point.z);
    pg2 = common_math_get(point.x);
    pd2 = common_math_get(point.x);
    y = common_math_get(point.y);
    list_parcours = g_list_next(projet->modele.noeuds);
    while (list_parcours != NULL)
    {
        noeud = (EF_Noeud*)list_parcours->data;
        BUG(EF_noeuds_renvoie_position(noeud, &point), FALSE);
        
        if (pg2 > common_math_get(point.x))
            pg2 = common_math_get(point.x);
        if (pd2 < common_math_get(point.x))
            pd2 = common_math_get(point.x);
        if (pb2 > common_math_get(point.z))
            pb2 = common_math_get(point.z);
        if (ph2 < common_math_get(point.z))
            ph2 = common_math_get(point.z);
        if (y > common_math_get(point.y))
            y = common_math_get(point.y);
        
        list_parcours = g_list_next(list_parcours);
    }
    
    x = (pg2+pd2)/2.;
    y = y-0.5;
    z = (pb2+ph2)/2.;
    if (vue->camera == NULL)
    {
        vue->camera = new CM3dCamera(x, y, z, x, y+1., z, 90, allocation.width, allocation.height);
        vue->camera->rotation_on_axe_of_view(0);
    }
    else
    {
        vue->camera->set_position(x, y, z);
        vue->camera->set_target(x, y+1., z);
        vue->camera->rotation_on_axe_of_view(0);
    }
    
    // Depuis la position de la caméra, on cherche les 4 noeuds délimitant le rectangle
    // optimal. Pour cela, on parcours tous les noeuds, on calcule leur projection dans la
    // fenêtre et on cherche les valeurs xmin, xmax, ymin et ymax de la fenetre.
    v1.set_coordinates(ph[0], ph[1], ph[2]);
    poly = new CM3dPolygon(v1, v1, v1);
    poly->convert_by_camera_view(vue->camera);
    poly->convert_to_2d(vue->camera);
    poly->get_vertex1_to_2d()->get_coordinates(&x1, &y1, NULL);
    delete poly;
    ph2 = y1;
    pb2 = y1;
    pg2 = x1;
    pd2 = x1;
    list_parcours = g_list_next(projet->modele.noeuds);
    while (list_parcours != NULL)
    {
        noeud = (EF_Noeud*)list_parcours->data;
        BUG(EF_noeuds_renvoie_position(noeud, &point), FALSE);
        
        v1.set_coordinates(common_math_get(point.x), common_math_get(point.y), common_math_get(point.z));
        
        poly = new CM3dPolygon(v1, v1, v1);
        poly->convert_by_camera_view(vue->camera);
        poly->convert_to_2d(vue->camera);
        poly->get_vertex1_to_2d()->get_coordinates(&x1, &y1, NULL);
        delete poly;
        
        if (pg2 > x1)
        {
            pg[0] = common_math_get(point.x);
            pg[1] = common_math_get(point.y);
            pg[2] = common_math_get(point.z);
            pg2 = x1;
        }
        if (pd2 < x1)
        {
            pd[0] = common_math_get(point.x);
            pd[1] = common_math_get(point.y);
            pd[2] = common_math_get(point.z);
            pd2 = x1;
        }
        if (pb2 > y1)
        {
            pb[0] = common_math_get(point.x);
            pb[1] = common_math_get(point.y);
            pb[2] = common_math_get(point.z);
            pb2 = y1;
        }
        if (ph2 < y1)
        {
            ph[0] = common_math_get(point.x);
            ph[1] = common_math_get(point.y);
            ph[2] = common_math_get(point.z);
            ph2 = y1;
        }
        
        list_parcours = g_list_next(list_parcours);
    }
    
    // Détail pour la fonction x.
    // Soit deux droite :
    // La première passe par le noeud le plus à droite,
    // la deuxième passe par le noeud le plus à gauche.
    // Comme on regarde avec un angle de 90° (45° en haut et 45° en bas), le coefficient
    // directeur de la droite 1 est de -1 et de la droite 2 de 1.
    // L'objectif est de trouver l'intersection entre les deux droite.
    // => droite 1 :  f(x) = -x1+b=y1 soit b = y1+x1
    // => droite 2 :  f(x) =  x2+b=y2 soit b = y2-x2
    // En cherchant l'égalité entre les deux barres, on obtient la position de la caméra en x.
    // Il suffit de faire la même chose pour obtenir la position de la caméra en z.
    // Pour obtenir la position en y, il suffit de calculer l'ordonnée des deux courbes et de
    // prendre le cas le plus défavorable. Je rajoute forfaitairement -1 afin que les lignes
    // ne soient pas trop sur le bord de la fenêtre.
    // Un shéma aurait été plus simple mais je ne suis pas un spécialiste en art ASCII...
    x = -((pd[1]-pd[0])-pg[1]-pg[0])/2.;
    z = -((ph[1]-ph[2])-pb[1]-pb[2])/2.;
    y = MIN(pd[1]-pd[0]+pg[1]+pg[0], ph[1]-ph[2]+pb[1]+pb[2])/2. - 1.;
    vue->camera->set_position(x, y, z);
    vue->camera->set_target(x, y+1., z);
    
    BUG(m3d_rafraichit(projet), FALSE);
    
    return TRUE;
}


gboolean m3d_actualise_graphique(Projet *projet, GList *noeuds, GList *barres)
/* Description : Met à jour l'affichage graphique en actualisant l'affichage des noeuds et barres passés en argument. Les listes contient une série de pointeur.
 * Paramètres : Projet *projet : la variable projet,
 *            : GList *noeuds : Liste de pointeurs vers les noeuds à actualiser,
 *            : GList *barres : Liste de pointeurs vers les barres à actualiser.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL,
 *             noeud == NULL,
 *             en cas d'erreur due à une fonction interne.
 */
{
    GList *noeuds_dep, *barres_dep;
    GList *list_parcours;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    BUG(_1992_1_1_barres_cherche_dependances(projet, NULL, noeuds, NULL, NULL, NULL, barres, &noeuds_dep, &barres_dep, NULL, FALSE, TRUE), FALSE);
    
    list_parcours = noeuds_dep;
    while (list_parcours != NULL)
    {
        BUG(m3d_noeud(&projet->list_gtk.m3d, (EF_Noeud *)list_parcours->data), FALSE);
        list_parcours = g_list_next(list_parcours);
    }
    g_list_free(noeuds_dep);
    
    list_parcours = barres_dep;
    while (list_parcours != NULL)
    {
        BUG(m3d_barre(&projet->list_gtk.m3d, (Beton_Barre *)list_parcours->data), FALSE);
        list_parcours = g_list_next(list_parcours);
    }
    g_list_free(barres_dep);
    
    return TRUE;
}


gboolean m3d_rafraichit(Projet *projet)
/* Description : Force le rafraichissement de l'affichage graphique.
 *               Nécessaire après l'utilisation d'une des fonctions d'actualisation de
 *                 l'affichage graphique.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    SGlobalData *vue;
    
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    vue = (SGlobalData*)projet->list_gtk.m3d.data;
    // On force l'actualisation de l'affichage
    vue->scene->rendering(vue->camera);
    gtk_widget_queue_draw(projet->list_gtk.m3d.drawing);
    
    return TRUE;
}


void* m3d_noeud(void *donnees_m3d, EF_Noeud *noeud)
/* Description : Crée un noeud dans l'affichage graphique. Si le noeud existe, il est détruit au
 *               préalable.
 * Paramètres : void *donnees_m3d : données SGlobalData,
 *              EF_Noeud *noeud : noeud à ajouter ou à actualiser.
 * Valeur renvoyée :
 *   Succès : Pointeur vers le nouvel objet noeud.
 *   Échec : NULL :
 *             noeud == NULL,
 *             donnees_m3d == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    CM3dObject  *cube;
    char        *nom;
    EF_Point    point;
    SGlobalData *vue;
    
    BUGMSG(noeud, NULL, gettext("Paramètre %s incorrect.\n"), "noeud");
    BUGMSG(donnees_m3d, NULL, gettext("Paramètre %s incorrect.\n"), "donnees_m3d");
    
    BUGMSG(nom = g_strdup_printf("noeud %u", noeud->numero), NULL, gettext("Erreur d'allocation mémoire.\n"));
    BUG(EF_noeuds_renvoie_position(noeud, &point), NULL);
    
    vue = (SGlobalData*)((Gtk_m3d *)donnees_m3d)->data;
    
    cube = vue->scene->get_object_by_name(nom);
    if (cube != NULL)
        vue->scene->remove_object(cube);
    
    cube = M3d_cube_new(nom, .1);
    cube->set_ambient_reflexion (1.);
    cube->set_smooth(GOURAUD);
    vue->scene->add_object(cube);
    cube->set_position(common_math_get(point.x), common_math_get(point.y), common_math_get(point.z));
    
    free(nom);
    
    return cube;
}


void m3d_noeud_free(void *donnees_m3d, EF_Noeud *noeud)
/* Description : Supprimer un noeud dans l'affichage graphique.
 * Paramètres : void *donnees_m3d : données SGlobalData,
 *              EF_Noeud *noeud : noeud à ajouter ou à actualiser.
 * Valeur renvoyée :
 *   Succès : Pointeur vers le nouvel objet noeud.
 *   Échec : NULL :
 *             noeud == NULL,
 *             donnees_m3d == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    CM3dObject  *cube;
    char        *nom;
    SGlobalData *vue;
    
    BUGMSG(noeud, , gettext("Paramètre %s incorrect.\n"), "noeud");
    BUGMSG(donnees_m3d, , gettext("Paramètre %s incorrect.\n"), "donnees_m3d");
    
    BUGMSG(nom = g_strdup_printf("noeud %u", noeud->numero), , gettext("Erreur d'allocation mémoire.\n"));
    
    vue = (SGlobalData*)((Gtk_m3d *)donnees_m3d)->data;
    
    cube = vue->scene->get_object_by_name(nom);
    vue->scene->remove_object(cube);
    
    free(nom);
    
    return;
}


gboolean m3d_barre(void *donnees_m3d, Beton_Barre *barre)
/* Description : Crée une barre dans l'affichage graphique. Si la barre existe, elle est
 *               détruite au préalable.
 * Paramètres : void *donnees_m3d : données graphiques,
 *              Beton_Barre *barre : barre devant être représentée.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             donnees_m3d == NULL,
 *             barre == NULL,
 *             si la longueur de la barre est nulle,
 *             le type de barre est inconnu,
 *             en cas d'erreur d'allocation mémoire,
 *             en cas d'erreur d'une fonction interne.
 */
{
    Gtk_m3d     *m3d;
    SGlobalData *vue;
    CM3dObject  *objet;
    char        *tmp;
    CM3dObject  *tout;
    double      longueur;
    
    BUGMSG(donnees_m3d, FALSE, gettext("Paramètre %s incorrect.\n"), "donnees_m3d");
    BUGMSG(barre, FALSE, gettext("Paramètre %s incorrect.\n"), "barre");
    
    m3d = (Gtk_m3d *)donnees_m3d;
    vue = (SGlobalData*)m3d->data;
    
    // On supprime l'élément s'il existe déjà
    BUGMSG(tmp = g_strdup_printf("barre %u", barre->numero), FALSE, gettext("Erreur d'allocation mémoire.\n"));

    longueur = EF_noeuds_distance(barre->noeud_debut, barre->noeud_fin);
    BUG(!isnan(longueur), FALSE);
    
    objet = vue->scene->get_object_by_name(tmp);
    if (objet != NULL)
        vue->scene->remove_object(objet);
    if (ERREUR_RELATIVE_EGALE(longueur, 0.))
        return TRUE;
    
    switch (barre->section->type)
    {
        EF_Point p_d, p_f;
        case SECTION_RECTANGULAIRE :
        {
            double      y, z;
            CM3dObject  *bas, *haut, *gauche, *droite;
            Section_T   *section = (Section_T *)barre->section->data;
            
            droite = M3d_plan_new("", longueur, common_math_get(section->hauteur_retombee), 1);
            droite->rotations(180., 0., 0.);
            droite->set_position(0., -common_math_get(section->largeur_retombee)/2., 0.);
            
            gauche = M3d_plan_new("", longueur, common_math_get(section->hauteur_retombee), 1);
            gauche->set_position(0., common_math_get(section->largeur_retombee)/2., 0.);
            
            bas = M3d_plan_new("", longueur, common_math_get(section->largeur_retombee), 1);
            bas->rotations(90., 180., 0.);
            bas->set_position(0., 0., -common_math_get(section->hauteur_retombee)/2.);
            
            haut = M3d_plan_new("", longueur, common_math_get(section->largeur_retombee), 1);
            haut->rotations(90., 0., 0.);
            haut->set_position(0., 0., common_math_get(section->hauteur_retombee)/2.);
            
            tout = M3d_object_new_group(tmp, droite, gauche, bas, haut, NULL);
            
            delete droite;
            delete gauche;
            delete bas;
            delete haut;
            
            switch(barre->numero)
            {
                case 0:
                {
                    tout->set_color(255, 0, 0);
                    break;
                }
                case 1:
                {
                    tout->set_color(0, 255, 0);
                    break;
                }
                case 2:
                {
                    tout->set_color(0, 0, 255);
                    break;
                }
                default : break;

            }
            tout->set_smooth(GOURAUD);
            BUG(_1992_1_1_barres_angle_rotation(barre->noeud_debut, barre->noeud_fin, &y, &z), FALSE);
            tout->rotations(0., -y/M_PI*180., z/M_PI*180.);
            BUG(EF_noeuds_renvoie_position(barre->noeud_debut, &p_d), FALSE);
            BUG(EF_noeuds_renvoie_position(barre->noeud_fin, &p_f), FALSE);
            tout->set_position((common_math_get(p_d.x)+common_math_get(p_f.x))/2., (common_math_get(p_d.y)+common_math_get(p_f.y))/2., (common_math_get(p_d.z)+common_math_get(p_f.z))/2.);
            tout->set_ambient_reflexion(0.8);
            
            vue->scene->add_object(tout);
            
            break;
            
        }
        case SECTION_T :
        {
            Section_T *section = (Section_T *)barre->section->data;
            
            double  y, z;
            double  lt = common_math_get(section->largeur_table);
            double  lr = common_math_get(section->largeur_retombee);
            double  ht = common_math_get(section->hauteur_table);
            double  hr = common_math_get(section->hauteur_retombee);
            double  cdgh = (lt*ht*ht/2.+lr*hr*(ht+hr/2.))/(lt*ht+lr*hr);
            double  cdgb = (ht+hr)-cdgh;
            
            CM3dObject  *retombee_inf, *retombee_droite, *retombee_gauche, *dalle_bas_droite, *dalle_bas_gauche, *dalle_droite, *dalle_gauche, *dalle_sup;
            
            retombee_inf = M3d_plan_new("", longueur, lr, 1);
            retombee_inf->rotations(90., 180., 0.);
            retombee_inf->set_position(0., 0., -cdgb);
            
            retombee_droite = M3d_plan_new("", longueur, hr, 1);
            retombee_droite->rotations(180., 0., 0.);
            retombee_droite->set_position(0., -lr/2., -cdgb+hr/2.);
            
            retombee_gauche = M3d_plan_new("", longueur, hr, 1);
            retombee_gauche->set_position(0., lr/2., -cdgb+hr/2.);
            
            dalle_bas_gauche = M3d_plan_new("", longueur, (lt-lr)/2., 1);
            dalle_bas_gauche->rotations(90., 180., 0.);
            dalle_bas_gauche->set_position(0., lr/2.+(lt-lr)/4., -cdgb+hr);
            
            dalle_bas_droite = M3d_plan_new("", longueur, (lt-lr)/2., 1);
            dalle_bas_droite->rotations(90., 180., 0.);
            dalle_bas_droite->set_position(0., -lr/2.-(lt-lr)/4., -cdgb+hr);
            
            dalle_droite = M3d_plan_new("", longueur, ht, 1);
            dalle_droite->rotations(180., 0., 0.);
            dalle_droite->set_position(0., -lt/2., -cdgb+hr+ht/2.);
            
            dalle_gauche = M3d_plan_new("", longueur, ht, 1);
            dalle_gauche->set_position(0., lt/2., -cdgb+hr+ht/2.);
            
            dalle_sup = M3d_plan_new("", longueur, lt, 1);
            dalle_sup->rotations(90., 0., 0.);
            dalle_sup->set_position(0., 0, -cdgb+hr+ht);
            
            tout = M3d_object_new_group(tmp, retombee_inf, retombee_droite, retombee_gauche, dalle_bas_droite, dalle_bas_gauche, dalle_droite, dalle_gauche, dalle_sup, NULL);
            
            delete retombee_inf;
            delete retombee_droite;
            delete retombee_gauche;
            delete dalle_bas_droite;
            delete dalle_bas_gauche;
            delete dalle_droite;
            delete dalle_gauche;
            delete dalle_sup;
            
            switch(barre->numero)
            {
                case 0:
                {
                    tout->set_color(255, 0, 0);
                    break;
                }
                case 1:
                {
                    tout->set_color(0, 255, 0);
                    break;
                }
                case 2:
                {
                    tout->set_color(0, 0, 255);
                    break;
                }
                default : break;

            }
            tout->set_ambient_reflexion(0.8);
            tout->set_smooth(GOURAUD);
            BUG(_1992_1_1_barres_angle_rotation(barre->noeud_debut, barre->noeud_fin, &y, &z), FALSE);
            tout->rotations(0., -y/M_PI*180., z/M_PI*180.);
            BUG(EF_noeuds_renvoie_position(barre->noeud_debut, &p_d), FALSE);
            BUG(EF_noeuds_renvoie_position(barre->noeud_fin, &p_f), FALSE);
            tout->set_position((common_math_get(p_d.x)+common_math_get(p_f.x))/2., (common_math_get(p_d.y)+common_math_get(p_f.y))/2., (common_math_get(p_d.z)+common_math_get(p_f.z))/2.);
            
            vue->scene->add_object(tout);
            
            break;
            
        }
        case SECTION_CARREE :
        {
            Section_T   *section = (Section_T *)barre->section->data;
            double      y, z;
            CM3dObject  *bas, *haut, *gauche, *droite;
            
            droite = M3d_plan_new("", longueur, common_math_get(section->largeur_table), 1);
            droite->rotations(180., 0., 0.);
            droite->set_position(0., -common_math_get(section->largeur_table)/2., 0.);
            
            gauche = M3d_plan_new("", longueur, common_math_get(section->largeur_table), 1);
            gauche->set_position(0., common_math_get(section->largeur_table)/2., 0.);
            
            bas = M3d_plan_new("", longueur, common_math_get(section->largeur_table), 1);
            bas->rotations(90., 180., 0.);
            bas->set_position(0., 0., -common_math_get(section->largeur_table)/2.);
            
            haut = M3d_plan_new("", longueur, common_math_get(section->largeur_table), 1);
            haut->rotations(90., 0., 0.);
            haut->set_position(0., 0., common_math_get(section->largeur_table)/2.);
            
            tout = M3d_object_new_group(tmp, droite, gauche, bas, haut, NULL);
            
            delete droite;
            delete gauche;
            delete bas;
            delete haut;
            
            switch(barre->numero)
            {
                case 0:
                {
                    tout->set_color(255, 0, 0);
                    break;
                }
                case 1:
                {
                    tout->set_color(0, 255, 0);
                    break;
                }
                case 2:
                {
                    tout->set_color(0, 0, 255);
                    break;
                }
                default : break;

            }
            tout->set_ambient_reflexion(0.8);
            tout->set_smooth(GOURAUD);
            BUG(_1992_1_1_barres_angle_rotation(barre->noeud_debut, barre->noeud_fin, &y, &z), FALSE);
            tout->rotations(0., -y/M_PI*180., z/M_PI*180.);
            BUG(EF_noeuds_renvoie_position(barre->noeud_debut, &p_d), FALSE);
            BUG(EF_noeuds_renvoie_position(barre->noeud_fin, &p_f), FALSE);
            tout->set_position((common_math_get(p_d.x)+common_math_get(p_f.x))/2., (common_math_get(p_d.y)+common_math_get(p_f.y))/2., (common_math_get(p_d.z)+common_math_get(p_f.z))/2.);
            
            vue->scene->add_object(tout);
            
            break;
            
        }
        case SECTION_CIRCULAIRE :
        {
            Section_Circulaire *section = (Section_Circulaire *)barre->section->data;
            double  y, z;
            
            tout = M3d_cylindre_new(tmp, common_math_get(section->diametre)/2., longueur, 12);
            tout->rotations(0., 0., 90.);
            switch(barre->numero)
            {
                case 0:
                {
                    tout->set_color(255, 0, 0);
                    break;
                }
                case 1:
                {
                    tout->set_color(0, 255, 0);
                    break;
                }
                case 2:
                {
                    tout->set_color(0, 0, 255);
                    break;
                }
                default : break;

            }
            tout->set_ambient_reflexion(0.8);
            tout->set_smooth(GOURAUD);
            BUG(_1992_1_1_barres_angle_rotation(barre->noeud_debut, barre->noeud_fin, &y, &z), FALSE);
            tout->rotations(0., -y/M_PI*180., z/M_PI*180.);
            BUG(EF_noeuds_renvoie_position(barre->noeud_debut, &p_d), FALSE);
            BUG(EF_noeuds_renvoie_position(barre->noeud_fin, &p_f), FALSE);
            tout->set_position((common_math_get(p_d.x)+common_math_get(p_f.x))/2., (common_math_get(p_d.y)+common_math_get(p_f.y))/2., (common_math_get(p_d.z)+common_math_get(p_f.z))/2.);
            
            vue->scene->add_object(tout);
            
            break;
            
        }
        default :
        {
            BUGMSG(0, FALSE, gettext("Type de section %d inconnu.\n"), barre->section->type);
            break;
        }
    }
    
    free(tmp);
    
    return TRUE;
}


void m3d_barre_free(void *donnees_m3d, Beton_Barre *barre)
/* Description : Supprimer une barre dans l'affichage graphique.
 * Paramètres : void *donnees_m3d : données SGlobalData,
 *              EF_Noeud *noeud : noeud à ajouter ou à actualiser.
 * Valeur renvoyée :
 *   Succès : Pointeur vers le nouvel objet noeud.
 *   Échec : NULL :
 *             noeud == NULL,
 *             donnees_m3d == NULL,
 *             en cas d'erreur d'allocation mémoire.
 */
{
    CM3dObject  *cube;
    char        *nom;
    SGlobalData *vue;
    
    BUGMSG(barre, , gettext("Paramètre %s incorrect.\n"), "noeud");
    BUGMSG(donnees_m3d, , gettext("Paramètre %s incorrect.\n"), "donnees_m3d");
    
    BUGMSG(nom = g_strdup_printf("barre %u", barre->numero), , gettext("Erreur d'allocation mémoire.\n"));
    
    vue = (SGlobalData*)((Gtk_m3d *)donnees_m3d)->data;
    
    cube = vue->scene->get_object_by_name(nom);
    vue->scene->remove_object(cube);
    
    free(nom);
    
    return;
}


gboolean m3d_free(Projet *projet)
/* Description : Libère l'espace mémoire alloué pour l'affichage graphique de la structure.
 * Paramètres : Projet *projet : la variable projet.
 * Valeur renvoyée :
 *   Succès : TRUE
 *   Échec : FALSE :
 *             projet == NULL.
 */
{
    // Trivial
    BUGMSG(projet, FALSE, gettext("Paramètre %s incorrect.\n"), "projet");
    
    delete ((SGlobalData*)projet->list_gtk.m3d.data)->scene;
    delete ((SGlobalData*)projet->list_gtk.m3d.data)->camera;
    free(projet->list_gtk.m3d.data);
    projet->list_gtk.m3d.data = NULL;
    
    return TRUE;
}


}

#endif
