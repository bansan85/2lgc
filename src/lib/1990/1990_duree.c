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
#include <gmodule.h>

#include "common_erreurs.h"
#include "common_projet.h"


int
_1990_duree_norme_eu (unsigned int type)
/**
 * \brief Renvoie la durée (en année) indicative de la norme européenne. Dans
 *        le cas où la durée indiquée par la norme européenne est un intervalle
 *        d'années, la durée la plus importante est renvoyée. La description de
 *        type est donnée par #_1990_duree_norme_txt_eu.
 * \param type : catégorie de durée d'utilisation de projet.
 * \return
 *   Succès : Durée indicative en année :
 *     - type 0 : 10,
 *     - type 1 : 25,
 *     - type 2 : 30,
 *     - type 3 : 50,
 *     - type 4 : 100.
 * \return
 *   Échec : 0 :
 *     - le type n'existe pas.
 * \warning Fonction interne. Il convient d'utiliser la fonction
 *          #_1990_duree_norme.
 */
{
  switch (type)
  {
    case 0 : { return 10; break; }
    case 1 : { return 25; break; }
    case 2 : { return 30; break; }
    case 3 : { return 50; break; }
    case 4 : { return 100; break; }
    default : { BUGMSG (0, 0, gettext ("Catégorie de durée %u inconnue.\n"), type) break; }
  }
}


int
_1990_duree_norme_fr (unsigned int type)
/**
 * \brief Renvoie la durée (en année) indicative de la norme française. La
 *        description de type est donnée par #_1990_duree_norme_txt_fr.
 * \param type : catégorie de durée d'utilisation de projet.
 * \return
 *   Succès : Durée indicative en année :
 *     - type 0 : 10,
 *     - type 1 : 25,
 *     - type 2 : 25,
 *     - type 3 : 50,
 *     - type 4 : 100.
 * \return
 *   Échec : 0 :
 *     - le type n'existe pas.
 * \warning Fonction interne. Il convient d'utiliser la fonction
 *          #_1990_duree_norme.
 */
{
  switch (type)
  {
    case 0 : { return 10; break; }
    case 1 : { return 25; break; }
    case 2 : { return 25; break; }
    case 3 : { return 50; break; }
    case 4 : { return 100; break; }
    default : { BUGMSG (0, 0, gettext ("Catégorie de durée %u inconnue.\n"), type) break; }
  }
}


char *
_1990_duree_norme_txt_eu (unsigned int type)
/**
 * \brief Renvoie la description des catérogies des durées indicatives de la
 *        norme européenne.
 * \param type : catégorie de durée d'utilisation de projet.
 * \return
 *   Succès : Texte contenant la description :
 *     - type 0 : "Structures provisoires",
 *     - type 1 : "Éléments structuraux remplaçables, par exemple poutres de
 *                 roulement",
 *     - type 2 : "Structures agricoles et similaires",
 *     - type 3 : "Structures de bâtiments et autres structures courantes",
 *     - type 4 : "Structures monumentales de bâtiments, pont et autres
 *                 ouvrages de GC".
 * \return
 *   Échec : NULL :
 *     - le type n'existe pas.
 * \warning Fonction interne. Il convient d'utiliser la fonction
 *          #_1990_duree_norme_txt.
 */
{
  switch (type)
  {
    case 0 : { return gettext ("Structures provisoires"); break; }
    case 1 : { return gettext ("Éléments structuraux remplaçables, par exemple poutres de roulement, appareils d'appui"); break; }
    case 2 : { return gettext ("Structures agricoles et similaires"); break; }
    case 3 : { return gettext ("Structures de bâtiments et autres structures courantes"); break; }
    case 4 : { return gettext ("Structures monumentales de bâtiments, pont et autres ouvrages de génie civil"); break; }
    default : { BUGMSG (0, NULL, gettext ("Catégorie de durée %d inconnue.\n"), type) break; }
  }
}


char *
_1990_duree_norme_txt_fr (unsigned int type)
/**
 * \brief Renvoie la description des catérogies des durées indicative de la
 *        norme française.
 * \param type : catégorie de durée d'utilisation de projet.
 * \return
 *   Succès : Texte contenant la description :
 *     - type 0 : "Structures provisoires",
 *     - type 1 : "Éléments structuraux remplaçables, par exemple poutres de
 *                 roulement",
 *     - type 2 : "Structures agricoles et similaires",
 *     - type 3 : "Structures de bâtiments et autres structures courantes",
 *     - type 4 : "Structures monumentales de bâtiments, pont et autres
 *                 ouvrages de GC".
 * \return
 *   Échec : NULL :
 *     - le type n'existe pas.
 * \warning Fonction interne. Il convient d'utiliser la fonction
 *          #_1990_duree_norme_txt.
 */
{
  switch (type)
  {
    case 0 : { return gettext ("Structures provisoires"); break; }
    case 1 : { return gettext ("Éléments structuraux remplaçables, par exemple poutres de roulement, appareils d'appui"); break; }
    case 2 : { return gettext ("Structures agricoles et similaires"); break; }
    case 3 : { return gettext ("Structures de bâtiments et autres structures courantes"); break; }
    case 4 : { return gettext ("Structures monumentales de bâtiments, pont et autres ouvrages de génie civil"); break; }
    default : { BUGMSG (0, NULL, gettext ("Catégorie de durée %d inconnue.\n"), type) break; }
  }
}


int
_1990_duree_norme (unsigned int type,
                   Norme        norme)
/**
 * \brief Renvoie la durée (en année) indicative de la norme spécifiée.
 * \param type : categorie de durée d'utilisation de projet,
 * \param norme : norme souhaitée.
 * \return
 *   Succès : cf les fonctions _1990_duree_norme_PAYS.\n
 *   Échec : 0 :
 *     - Pays inconnu,
 *     - erreur dans l'une des fonctions _1990_duree_norme_PAYS.
 */
{
  switch (norme)
  {
    case NORME_EU : { return _1990_duree_norme_eu (type); break; }
    case NORME_FR : { return _1990_duree_norme_fr (type); break; }
    default : { BUGMSG (0, 0, gettext ("Norme %d inconnue.\n"), norme) break; }
  }
}


char *
_1990_duree_norme_txt (unsigned int type,
                       Norme        norme)
/**
 * \brief Renvoie la description des catérogies des durées indicatives de la
 *        norme spécifiée.
 * \param type : categorie de durée d'utilisation de projet,
 * \param norme : norme souhaitée.
 * \return
 *   Succès : cf les fonctions _1990_duree_norme_txt_PAYS.\n
 *   Échec : NULL :
 *     - Pays inconnu,
 *     - erreur dans l'une des fonctions _1990_duree_norme_txt_PAYS.
 */
{
  switch (norme)
  {
    case NORME_EU : { return _1990_duree_norme_txt_eu (type); break; }
    case NORME_FR : { return _1990_duree_norme_txt_fr (type); break; }
    default : { BUGMSG (0, NULL, gettext ("Norme %d inconnue.\n"), norme) break; }
  }
}
