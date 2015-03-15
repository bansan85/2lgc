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

#include <cmath>
#include <cstdarg>
#include <memory>
#include <locale>
#include <sstream>
#include <iostream>

#include "NbUser.hpp"
#include "EUniteTxt.hpp"
#include "MErreurs.hpp"
#include "SString.hpp"

/**
 * \brief Constructeur d'une classe NbUser.
 * \param valeur (in) La valeur initiale.
 * \param unit (in) L'unité du nombre.
 */
NbUser::NbUser (double valeur,
                EUnite unit) :
  val (valeur),
  unite (unit)
{
}

/**
 * \brief Libère une classe NbUser.
 */
NbUser::~NbUser ()
{
}

/**
 * \brief Renvoie la valeur du nombre.
 */
double
NbUser::getVal () const
{
  return val;
}

/**
 * \brief Renvoie l'unité du nombre.
 */
EUnite
NbUser::getUnite () const
{
  return unite;
}

/**
 * \brief Renvoie le nombre sous forme de texte sans respecter le nombre de
 *        décimales..
 */
std::string
NbUser::toString () const
{
  std::string retour;
  uint8_t     width;
  double      test;
  
  if (fabs (val) > 1e15)
  {
    for (width = 0; width <= 15; width++)
    {
      retour = format ("%.*le", width, val);
      sscanf (retour.c_str (), "%le", &test);
      if ((fabs (val) * 0.999999999999999 <= fabs (test)) &&
          (fabs (test) <= fabs (val) * 1.000000000000001))
      {
        break;
      }
    }
  }
  // Sinon on affiche sous forme normale
  else
  {
    for (width = 0; width <= 15; width++)
    {
      retour = format ("%.*lf", width, val);
      sscanf (retour.c_str (), "%lf", &test);
      if ((fabs (val) * 0.999999999999999 <= fabs (test)) &&
          (fabs (test) <= fabs (val) * 1.000000000000001))
      {
        break;
      }
    }
  }
  
  return format ("%.*lf", width, val);
}

bool CHK
NbUser::newXML (xmlNodePtr root) const
{
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                      xmlNewNode (nullptr, BAD_CAST2 ("NbUser")), xmlFreeNode);
  
  BUGCRIT (node.get (),
           false,
           static_cast <UndoManager *> (nullptr),
           gettext ("Erreur d'allocation mémoire.\n"))
  
  std::ostringstream oss;
  
  oss << std::scientific << val;
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("valeur"),
                       BAD_CAST2 (oss.str ().c_str ())),
           false,
           static_cast <UndoManager *> (nullptr),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("unite"),
                       BAD_CAST2 (EUniteConst[static_cast <size_t> (unite)]
                                                                   .c_str ())),
           false,
           static_cast <UndoManager *> (nullptr),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  BUGCRIT (xmlAddChild (root, node.get ()),
           false,
           static_cast <UndoManager *> (nullptr),
           gettext ("Problème depuis la librairie : %s\n"), "xml2")
  
  node.release ();
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */