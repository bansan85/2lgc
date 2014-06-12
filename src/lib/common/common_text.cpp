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
#include "common_projet.hpp"
#include "common_selection.hpp"
#include "common_erreurs.hpp"
#include "common_math.hpp"
#include "common_text.hpp"

#include <string.h>
#include <locale>
#include <cmath>
#include <cstdarg>

/**
 * \brief Affiche les limites de la garantie (articles 15, 16 et 17 de la
 *        licence GPL).
 * \return Rien.
 */
void
show_warranty ()
{
  printf (gettext ("15. Disclaimer of Warranty.\n"));
  printf (gettext ("\n"));
  printf (gettext ("THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY\n"));
  printf (gettext ("APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT\n"));
  printf (gettext ("HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY\n"));
  printf (gettext ("OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,\n"));
  printf (gettext ("THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\n"));
  printf (gettext ("PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM\n"));
  printf (gettext ("IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF\n"));
  printf (gettext ("ALL NECESSARY SERVICING, REPAIR OR CORRECTION.\n"));
  printf (gettext ("\n"));
  printf (gettext ("16. Limitation of Liability.\n"));
  printf (gettext ("\n"));
  printf (gettext ("IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\n"));
  printf (gettext ("WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS\n"));
  printf (gettext ("THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY\n"));
  printf (gettext ("GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE\n"));
  printf (gettext ("USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF\n"));
  printf (gettext ("DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD\n"));
  printf (gettext ("PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS),\n"));
  printf (gettext ("EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF\n"));
  printf (gettext ("SUCH DAMAGES.\n"));
  printf (gettext ("\n"));
  printf (gettext ("17. Interpretation of Sections 15 and 16.\n"));
  printf (gettext ("\n"));
  printf (gettext ("If the disclaimer of warranty and limitation of liability provided\n"));
  printf (gettext ("above cannot be given local legal effect according to their terms,\n"));
  printf (gettext ("reviewing courts shall apply local law that most closely approximates\n"));
  printf (gettext ("an absolute waiver of all civil liability in connection with the\n"));
  printf (gettext ("Program, unless a warranty or assumption of liability accompanies a\n"));
  printf (gettext ("copy of the Program in return for a fee.\n"));
  return;
}

/**
 * \brief Affiche l'aide lorsque l'utilisateur lance le programme avec l'option
 *        -h.
 * \return Rien.
 */
void
show_help ()
{
  printf (gettext ("Utilisation : codegui [OPTION]... [FILE]...\n"));
  printf (gettext ("Options :\n"));
  printf (gettext ("\t-h, --help : affiche le présent menu\n"));
  printf (gettext ("\t-w, --warranty : affiche les limites de garantie du logiciel\n"));
  return;
}


/**
 * \brief Équivalent de sprintf mais sécurisé ou encore de g_strdup_printf mais
 *        en version std::string. Honteusement volé de
 *        http://stackoverflow.com/questions/2342162#3742999.
 * \param fmt : le texte à formater,
 * \param ... : les divers paramètres.
 * \return Le texte formaté en format std::string.
 */
std::string
format (const std::string fmt,
        ...)
{
  int         size = 1024;
  bool        b = false;
  va_list     marker;
  std::string s;
  
  while (!b)
  {
    int         n;
    
    s.resize (size);
    va_start (marker, fmt);
    n = vsnprintf ((char *) s.c_str (), size, fmt.c_str (), marker);
    va_end (marker);
    if ((n > 0) && ((b = (n < size)) == true))
    {
      s.resize (n);
    }
    else
    {
      size = size * 2;
    }
  }
  return s;
}


/**
 * \brief Sur la base d'une chaîne de caractères, renvoie un nombre flottant
 *        compris entre les valeurs val_min et val_max. S'il ne contient pas de
 *        nombre ou hors domaine, la valeur renvoyée est NAN.
 * \param texte : le texte à étudier,
 * \param val_min : borne inférieure,
 * \param min_include : le nombre de la borne inférieure est-il autorisé ?,
 * \param val_max : borne supérieure
 * \param max_include : le nombre de la borne supérieure est-il autorisé ?.
 * \return
 *   Succès : la valeur du nombre.\n
 *   Echec : NAN.
 */
double
common_text_str_to_double (char    *texte,
                           double   val_min,
                           bool     min_include,
                           double   val_max,
                           bool     max_include)
{
  bool          min_check, max_check;
  char         *fake, *textebis;
  double        nombre;
  struct lconv *locale_conv;
  char         *retour;
  
  BUGCRIT (fake = (char *) malloc (sizeof (char) * (strlen (texte) + 1)),
           NAN,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  BUGCRIT (textebis = (char *) malloc (sizeof (char) * (strlen (texte) + 1)),
           NAN,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (fake); )
  strcpy (textebis, texte);
  
  // On remplace par la bonne décimale.
  locale_conv = localeconv ();
  if (strcmp (locale_conv->decimal_point, ".") == 0)
  {
    retour = strchr (textebis, ',');
    while (retour != NULL)
    {
      *retour = '.';
      retour = strchr (textebis, ',');
    }
  }
  else if (strcmp (locale_conv->decimal_point, ",") == 0)
  {
    retour = strchr (textebis, '.');
    while (retour != NULL)
    {
      *retour = ',';
      retour = strchr (textebis, '.');
    }
  }
  else
  {
    FAILCRIT (NAN,
              (gettext ("Impossible de déterminer le caractère décimal.\n"));
                free (fake);
                free (textebis); )
  }
  
  if (sscanf (textebis, "%lf%s", &nombre, fake) != 1)
  {
    min_check = false;
    max_check = false;
  }
  else
  {
    if (std::isinf (val_min) == -1)
    {
      min_check = true;
    }
    else if ((min_include) && (errrel (nombre, val_min)))
    {
      min_check = true;
    }
    else if (nombre > val_min)
    {
      min_check = true;
    }
    else
    {
      min_check = false;
    }
      
    if (std::isinf (val_max) == 1)
    {
      max_check = true;
    }
    else if ((max_include) && (errrel (nombre, val_max)))
    {
      max_check = true;
    }
    else if (nombre < val_max)
    {
      max_check = true;
    }
    else
    {
      max_check = false;
    }
  }
  free (fake);
  free (textebis);
  
  if ((min_check) && (max_check))
  {
    return nombre;
  }
  else
  {
    return NAN;
  }
}


/**
 * \brief Est un équivalent de strcasestr avec gestion des accents et ne
 *        recherche l'aiguille qu'en début de chaque mot.
 * \param haystack : moule de foin,
 * \param needle : aiguille.
 * \return
 *   Succès : pointeur vers le texte.\n
 *   Échec : NULL.
 */
wchar_t *
strcasestr_internal (const wchar_t *haystack,
                     const wchar_t *needle)
{
#define CAR_UP   L"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÑÐÒÓÔÕÖØÙÚÛÜÝàáâãäåèçéêëìíîïðñòóôõöøùúûüýÿ" // NS (nsiqcppstyle)
#define CAR_DOWN L"aaaaaaæceeeeiiiindoooooouuuuyaaaaaaeceeeiiiionoooooouuuuyy"
  
  uint8_t  i;
  wchar_t *meule, *aiguille;
  wchar_t *tmp;
  
  BUGPARAM (haystack, "%p", haystack, NULL)
  BUGPARAM (needle, "%p", needle, NULL)
  
  BUGCRIT (meule = (wchar_t *) malloc (sizeof (wchar_t) *
                                                      (wcslen (haystack) + 1)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  wcscpy (meule, haystack);
  BUGCRIT (aiguille = (wchar_t *) malloc (sizeof (wchar_t) *
                                                        (wcslen (needle) + 1)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n"));
             free (meule); )
  wcscpy (aiguille, needle);
  
  for (i = 0; meule[i]; i++)
  {
    meule[i] = (wchar_t) tolower (meule[i]);
  }
  for (i = 0; aiguille[i]; i++)
  {
    aiguille[i] = (wchar_t) tolower (aiguille[i]);
  }
  
  for (i = 0; i < wcslen (CAR_UP); i++)
  {
    wchar_t *retour;
    
    retour = wcschr (meule, CAR_UP[i]);
    
    while (retour != NULL)
    {
      *retour = CAR_DOWN[i];
      retour = wcschr (meule, CAR_UP[i]);
    }
    
    retour = wcschr (aiguille, CAR_UP[i]);
    
    while (retour != NULL)
    {
      *retour = CAR_DOWN[i];
      retour = wcschr (meule, CAR_UP[i]);
    }
  }
  
  tmp = wcsstr (meule, aiguille);
  // Si ce n'est pas au début, il faut vérifier que c'est au début d'un mot
  while ((tmp != NULL) &&
         (tmp != meule) &&
         (tmp[-1] != ' ') &&
         (tmp[-1] != '-'))
  {
    tmp = wcsstr (tmp + 1, aiguille);
  }
  
  free (meule);
  free (aiguille);
  
  return tmp;
  
}


/**
 * \brief Renvoie la liste des types d'éléments dépendants sous forme de texte.
 *        liste_noeuds et liste_barres viennent de
 *        #_1992_1_1_barres_cherche_dependances.
 * \param liste_noeuds : liste des noeuds,
 * \param liste_barres : liste des barres,
 * \param liste_charges : liste des charges.
 * \param p : la variable projet.
 * \return
 *   Succès : pointeur vers le texte.\n
 *   Échec : NULL :
 *     - Erreur d'allocation mémoire.
 */
std::string
common_text_dependances (std::list <EF_Noeud *> *liste_noeuds,
                         std::list <EF_Barre *> *liste_barres,
                         std::list <Charge   *> *liste_charges,
                         Projet                 *p)
{
  std::string retour, tmp;
  
  if (!liste_noeuds->empty ())
  {
    tmp = common_selection_noeuds_en_texte (liste_noeuds);
    if (std::next (liste_noeuds->begin ()) == liste_noeuds->end ())
    {
      retour = std::string (gettext ("noeud")) + " : " + tmp;
    }
    else
    {
      retour = std::string (gettext ("noeuds")) + " : " + tmp;
    }
  }
  
  if (!liste_barres->empty ())
  {
    tmp = common_selection_barres_en_texte (liste_barres);
    
    if (retour != "")
    {
      retour += ", ";
    }
    
    if (std::next (liste_barres->begin ()) == liste_barres->end ())
    {
      retour += std::string (gettext ("barre"));
    }
    else
    {
      retour += std::string (gettext ("barres"));
    }
    
    retour += " : " + tmp;
  }
  
  if (liste_charges != NULL)
  {
    tmp = common_selection_charges_en_texte (liste_charges, p);
    
    if (retour != "")
    {
      retour += ", ";
    }
    
    if (std::next (liste_charges->begin ()) == liste_charges->end ())
    {
      retour += std::string (gettext ("charge"));
    }
    else
    {
      retour += std::string (gettext ("charges"));
    }
    
    retour += " : " + tmp;
  }
  
  retour = std::string (gettext ("et ces dépendances")) + " (" + tmp + ")";
  
  return retour;
}


/**
 * \brief Renvoie la ligne en cours de la variable fichier.
 * \param fichier : la variable fichier.
 * \return
 *   Succès : pointeur vers la ligne de texte.\n
 *   Échec : NULL :
 *     - fichier == NULL,
 *     - Erreur d'allocation mémoire.
 */
wchar_t *
common_text_get_line (FILE *fichier)
{
  const uint16_t CUR_MAX = 256;
  wchar_t *buffer, *retour = NULL;
  
  BUGCRIT (buffer = (wchar_t *) malloc (sizeof (wchar_t) * (CUR_MAX + 1U)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  
  do
  {
    wchar_t *ligne_tmp;
    
    if ((fgetws (buffer, CUR_MAX, fichier) == NULL) && (retour == NULL))
    {
      free (buffer);
      return NULL;
    }
    ligne_tmp = retour;
    if (ligne_tmp == NULL)
    {
      BUGCRIT (retour = (wchar_t *) malloc (sizeof (wchar_t) *
                                                        (wcslen (buffer) + 1)),
               NULL,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 free (buffer); )
      wcscpy (retour, buffer);
    }
    else
    {
      BUGCRIT (retour = (wchar_t *) malloc (sizeof (wchar_t) *
                                   (wcslen (ligne_tmp) + wcslen (buffer) + 1)),
               NULL,
               (gettext ("Erreur d'allocation mémoire.\n"));
                 free (buffer);
                 free (ligne_tmp); )
      wcscpy (retour, ligne_tmp);
      wcscat (retour, buffer);
      free (ligne_tmp);
    }
    
    // On a atteint la fin de la ligne
    if (buffer[wcslen (buffer) - 1] == '\n')
    {
      free (buffer);
      // Suppression du retour chariot
      retour[wcslen (retour) - 1] = 0;
      
      return retour;
    }
  } while (true);
}


/**
 * \brief Renvoie le texte sous format char *, y compris l'allocation mémoire.
 * \param texte : le texte à convertir.
 * \return
 *   Succès : pointeur vers le texte converti ou NULL si texte == NULL.\n
 *   Échec : NULL :
 *     - Erreur d'allocation mémoire.
 */
char *
common_text_wcstostr_dup (const wchar_t *texte)
{
  char *tmp;
  
  if (texte == NULL)
  {
    return NULL;
  }
  
  BUGCRIT (tmp = (char *) malloc (sizeof (char) *
                                              (wcstombs (NULL, texte, 0) + 1)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  wcstombs (tmp, texte, wcstombs (NULL, texte, 0) + 1);
  
  return tmp;
}


/**
 * \brief Renvoie le texte sous format wchar_t *, y compris l'allocation
 *        mémoire.
 * \param texte : le texte à convertir.
 * \return
 *   Succès : pointeur vers le texte converti ou NULL si texte == NULL.\n
 *   Échec : NULL :
 *     - Erreur d'allocation mémoire.
 */
wchar_t *
common_text_strtowcs_dup (const char *texte)
{
  wchar_t *tmp;
  
  if (texte == NULL)
  {
    return NULL;
  }
  
  BUGCRIT (tmp = (wchar_t *) malloc (sizeof (wchar_t) *
                                              (mbstowcs (NULL, texte, 0) + 1)),
           NULL,
           (gettext ("Erreur d'allocation mémoire.\n")); )
  mbstowcs (tmp, texte, mbstowcs (NULL, texte, 0) + 1);
  
  return tmp;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
