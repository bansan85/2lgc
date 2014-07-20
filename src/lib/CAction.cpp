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
#include <algorithm>
#include <memory>
#include <iostream>
#include <locale>

#include "CAction.hpp"
#include "CNbUser.hpp"
#include "MErreurs.hh"


/**
 * \brief Constructeur d'une classe CAction. Par défaut, les coefficients psi
 *        sont défini à NAN. Les valeurs sont automatiquement déterminés lors
 *        de l'insertion dans le projet en fonction de la norme du projet.
 * \param nom_ (in) Le nom de l'action.
 * \param type_ (in) Le type d'action, cf. _1990_action_bat_txt_type.
 * \param undo_ (in) Le gestionnaire des modifications.
 */
CAction::CAction (std::string    nom_,
                  uint8_t        type_,
                  CUndoManager & undo_) :
  IActionGroupe (nom_, undo_)
  , type (type_)
  , action_predominante (false)
  , charges ()
  , psi0 (new CNbUser (NAN, U_))
  , psi1 (new CNbUser (NAN, U_))
  , psi2 (new CNbUser (NAN, U_))
  , deplacement (NULL)
  , forces (NULL)
  , efforts_noeuds (NULL)
  , efforts {{}, {}, {}, {}, {}, {}}
  , deformation {{}, {}, {}}
  , rotation {{}, {}, {}}
#ifdef ENABLE_GTK
  , Iter_fenetre_ac (NULL)
  , Iter_liste (NULL)
#endif
{
}


/**
 * \brief Duplication d'une classe CAction.
 * \param other (in) La classe à dupliquer.
 */
CAction::CAction (const CAction & other) = delete;


/**
 * \brief Assignment operator de CAction.
 * \param other (in) La classe à dupliquer.
 */
CAction &
CAction::operator = (const CAction & other) = delete;


/**
 * \brief Libère une classe CAction.
 */
CAction::~CAction ()
{
  delete this->psi0;
  delete this->psi1;
  delete this->psi2;
}


/**
 * \brief Renvoie le type de l'action.
 */
uint8_t
CAction::getType () const
{
  return type;
}


/**
 * \brief Renvoie le cœfficient psi0.
 */
INb const &
CAction::getpsi0 () const
{
  return *this->psi0;
}


/**
 * \brief Renvoie le cœfficient psi1.
 */
INb const &
CAction::getpsi1 () const
{
  return *this->psi1;
}


/**
 * \brief Renvoie le cœfficient psi2.
 */
INb const &
CAction::getpsi2 () const
{
  return *this->psi2;
}


/**
 * \brief Renvoie true si aucune charge n'est présente.
 */
bool
CAction::emptyCharges () const
{
  return this->charges.size () == 0;
}


/**
 * \brief Renvoie le type de l'action sous forme de texte.
 */
std::string
CAction::getDescription (uint8_t type_)
{
  switch (type_)
  {
    case 0 :
    {
      return gettext ("Permanente");
    }
    case 1 :
    {
      return gettext ("Précontrainte");
    }
    case 2 :
    {
      return gettext ("Exploitation : Catégorie A : habitation, zones résidentielles");
    }
    case 3 :
    {
      return gettext ("Exploitation : Catégorie B : bureaux");
    }
    case 4 :
    {
      return gettext ("Exploitation : Catégorie C : lieux de réunion");
    }
    case 5 :
    {
      return gettext ("Exploitation : Catégorie D : commerces");
    }
    case 6 :
    {
      return gettext ("Exploitation : Catégorie E : stockage");
    }
    case 7 :
    {
      return gettext ("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN");
    }
    case 8 :
    {
      return gettext ("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN");
    }
    case 9 :
    {
      return gettext ("Exploitation : Catégorie H : toits");
    }
    case 10 :
    {
      return gettext ("Exploitation : Catégorie H : toits d'un bâtiment de catégorie A ou B");
    }
    case 11 :
    {
      return gettext ("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories A ou B");
    }
    case 12 :
    {
      return gettext ("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories C ou D");
    }
    case 13 :
    {
      return gettext ("Exploitation : Catégorie K : Hélicoptère sur la toiture");
    }
    case 14 :
    {
      return gettext ("Exploitation : Catégorie K : Hélicoptère sur la toiture, autres charges (fret, personnel, accessoires ou équipements divers)");
    }
    case 15 :
    {
      return gettext ("Neige : Finlande, Islande, Norvège, Suède");
    }
    case 16 :
    {
      return gettext ("Neige : Saint-Pierre-et-Miquelon");
    }
    case 17 :
    {
      return gettext ("Neige : Autres états membres CEN, altitude > 1000 m");
    }
    case 18 :
    {
      return gettext ("Neige : Autres états membres CEN, altitude <= 1000 m");
    }
    case 19 :
    {
      return gettext ("Vent");
    }
    case 20 :
    {
      return gettext ("Température (hors incendie)");
    }
    case 21 :
    {
      return gettext ("Accidentelle");
    }
    case 22 :
    {
      return gettext ("Sismique");
    }
    case 23 :
    {
      return gettext ("Eaux souterraines");
    }
    default :
    {
      return std::string ();
    }
  }
}


/**
 * \brief Converti la fonction d'ajout d'une action sous format XML..
 * \param root (in) Le noeud dans lequel doit être inséré l'action.
 */
bool
CAction::addXML (xmlNodePtr root)
{
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
    xmlNewNode (NULL, reinterpret_cast <const xmlChar *> ("addAction")),
    xmlFreeNode);
  
  BUG (node.get (),
       false,
       &this->getUndoManager (),
       (gettext ("Erreur d'allocation mémoire.\n")); )
  
  BUG (xmlSetProp (
         node.get (),
         reinterpret_cast <const xmlChar *> ("Nom"),
         reinterpret_cast <const xmlChar *> (this->getNom ().c_str ())),
       false,
       &this->getUndoManager ())
  
  BUG (xmlSetProp (
         node.get (),
         reinterpret_cast <const xmlChar *> ("Type"),
         reinterpret_cast <const xmlChar *> (
                         CAction::getDescription (this->getType ()).c_str ())),
       false,
       &this->getUndoManager ())
  
  // psi0
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node0 (
    xmlNewNode (NULL, reinterpret_cast <const xmlChar *> ("psi0")),
    xmlFreeNode);
  BUG (node0.get (),
       false,
       &this->getUndoManager (),
       (gettext ("Erreur d'allocation mémoire.\n")); )
  
  BUG (this->getpsi0 ().newXML (node0.get ()),
       false,
       &this->getUndoManager ())
  
  BUGCRIT (xmlAddChild (node.get (), node0.get ()),
           false,
           &this->getUndoManager (),
           (gettext ("Erreur lors de la génération du fichier XML.\n")); )
  node0.release ();
  
  // psi1
  node0.reset (xmlNewNode (NULL, reinterpret_cast <const xmlChar *> ("psi1")));
  BUG (node0.get (),
       false,
       &this->getUndoManager (),
       (gettext ("Erreur d'allocation mémoire.\n")); )
  
  BUG (this->getpsi1 ().newXML (node0.get ()),
       false,
       &this->getUndoManager ())
  
  BUGCRIT (xmlAddChild (node.get (), node0.get ()),
           false,
           &this->getUndoManager (),
           (gettext ("Erreur lors de la génération du fichier XML.\n")); )
  node0.release ();
  
  // psi2
  node0.reset (xmlNewNode (NULL, reinterpret_cast <const xmlChar *> ("psi2")));
  BUG (node0.get (),
       false,
       &this->getUndoManager (),
       (gettext ("Erreur d'allocation mémoire.\n")); )
  
  BUG (this->getpsi2 ().newXML (node0.get ()),
       false,
       &this->getUndoManager ())
  
  BUGCRIT (xmlAddChild (node.get (), node0.get ()),
           false,
           &this->getUndoManager (),
           (gettext ("Erreur lors de la génération du fichier XML.\n")); )
  node0.release ();
  
  BUGCRIT (xmlAddChild (root, node.get ()),
           false,
           &this->getUndoManager (),
           (gettext ("Erreur lors de la génération du fichier XML.\n")); )
  
  node.release ();
  
  return true;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
