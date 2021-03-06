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

#include "Eurocode.hpp"
#include "MErreurs.hpp"

norme::Eurocode::Eurocode (std::shared_ptr <const std::string> nom_,
                           ENormeEcAc                           variante_,
                           uint32_t                             options_) :
  INorme (ENorme::EUROCODE,
          static_cast <uint32_t> (variante_),
          options_,
          nom_)
{
}

norme::Eurocode::~Eurocode ()
{
}

uint8_t
norme::Eurocode::getPsiN () const
{
  switch (static_cast <ENormeEcAc> (variante))
  {
    case ENormeEcAc::EU :
    {
      return 17;
    }
    case ENormeEcAc::FR :
    {
      return 22;
    }
    default :
    {
      BUGPARAM (variante, "%u", false, 0, UNDO_MANAGER_NULL)
      break;
    }
  }
}

EAction
norme::Eurocode::getPsiAction (uint8_t type_) const
{
  switch (static_cast <ENormeEcAc> (variante))
  {
    case ENormeEcAc::EU :
    {
      if (type_ == 0)
      {
        return EAction::POIDS_PROPRE;
      }
      else if (type_ == 1)
      {
        return EAction::PRECONTRAINTE;
      }
      else if ((2 <= type_) && (type_ <= 14))
      {
        return EAction::VARIABLE;
      }
      else if (type_ == 15)
      {
        return EAction::ACCIDENTELLE;
      }
      else if (type_ == 16)
      {
        return EAction::SISMIQUE;
      }
      else
      {
        BUGPARAM (type_, "%u", false, EAction::INCONNUE, UNDO_MANAGER_NULL)
      }
    }
    case ENormeEcAc::FR :
    {
      if (type_ == 0) 
      {
        return EAction::POIDS_PROPRE;
      }
      else if (type_ == 1) 
      {
        return EAction::PRECONTRAINTE;
      }
      else if ((2 <= type_) && (type_ <= 18))
      {
        return EAction::VARIABLE;
      }
      else if (type_ == 19)
      {
        return EAction::ACCIDENTELLE;
      }
      else if (type_ == 20)
      {
        return EAction::SISMIQUE;
      }
      else if (type_ == 21)
      {
        return EAction::EAU_SOUTERRAINE;
      }
      else
      {
        BUGPARAM (type_, "%u", false, EAction::INCONNUE, UNDO_MANAGER_NULL)
      }
    }
    default :
    {
      BUGPARAM (variante, "%u", false, EAction::INCONNUE, UNDO_MANAGER_NULL)
      break;
    }
  }
}

std::string const
norme::Eurocode::getPsiDescription (uint8_t type_) const
{
  switch (static_cast <ENormeEcAc> (variante))
  {
    case ENormeEcAc::EU :
    {
      switch (type_)
      {
        case 0 : return gettext ("Permanente");
        case 1 : return gettext ("Précontrainte");
        case 2 : return gettext ("Exploitation : Catégorie A : habitation, zones résidentielles");
        case 3 : return gettext ("Exploitation : Catégorie B : bureaux");
        case 4 : return gettext ("Exploitation : Catégorie C : lieux de réunion");
        case 5 : return gettext ("Exploitation : Catégorie D : commerces");
        case 6 : return gettext ("Exploitation : Catégorie E : stockage");
        case 7 : return gettext ("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN");
        case 8 : return gettext ("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN");
        case 9 : return gettext ("Exploitation : Catégorie H : toits"); 
        case 10 : return gettext ("Neige : Finlande, Islande, Norvège, Suède"); 
        case 11 : return gettext ("Neige : Autres états membres CEN, altitude > 1000 m"); 
        case 12 : return gettext ("Neige : Autres états membres CEN, altitude <= 1000 m"); 
        case 13 : return gettext ("Vent");
        case 14 : return gettext ("Température (hors incendie)");
        case 15 : return gettext ("Accidentelle");
        case 16 : return gettext ("Sismique");
        default :
        {
          BUGPARAM (type_, "%u", false, std::string (), UNDO_MANAGER_NULL)
          break;
        }
      }
    }
    case ENormeEcAc::FR :
    {
      switch (type_)
      {
        case 0 : return gettext ("Permanente");
        case 1 : return gettext ("Précontrainte");
        case 2 : return gettext ("Exploitation : Catégorie A : habitation, zones résidentielles");
        case 3 : return gettext ("Exploitation : Catégorie B : bureaux");
        case 4 : return gettext ("Exploitation : Catégorie C : lieux de réunion");
        case 5 : return gettext ("Exploitation : Catégorie D : commerces");
        case 6 : return gettext ("Exploitation : Catégorie E : stockage");
        case 7 : return gettext ("Exploitation : Catégorie F : zone de trafic, véhicules de poids inférieur à 30 kN");
        case 8 : return gettext ("Exploitation : Catégorie G : zone de trafic, véhicules de poids entre 30 kN et 160 kN");
        case 9 : return gettext ("Exploitation : Catégorie H : toits d'un bâtiment de catégorie A ou B");
        case 10 : return gettext ("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories A ou B");
        case 11 : return gettext ("Exploitation : Catégorie I : toitures accessibles avec locaux des catégories C ou D");
        case 12 : return gettext ("Exploitation : Catégorie K : Hélicoptère sur la toiture");
        case 13 : return gettext ("Exploitation : Catégorie K : Hélicoptère sur la toiture, autres charges (fret, personnel, accessoires ou équipements divers)");
        case 14 : return gettext ("Neige : Saint-Pierre-et-Miquelon");
        case 15 : return gettext ("Neige : Altitude > 1000 m");
        case 16 : return gettext ("Neige : Altitude <= 1000 m");
        case 17 : return gettext ("Vent");
        case 18 : return gettext ("Température (hors incendie)");
        case 19 : return gettext ("Accidentelle");
        case 20 : return gettext ("Sismique");
        case 21 : return gettext ("Eaux souterraines");
        default :
        {
          BUGPARAM (type_, "%u", false, std::string (), UNDO_MANAGER_NULL)
          break;
        }
      }
    }
    default :
    {
      BUGPARAM (variante, "%u", false, std::string (), UNDO_MANAGER_NULL)
      break;
    }
  }
}

double
norme::Eurocode::getPsi0 (uint8_t type_) const
{
  switch (static_cast <ENormeEcAc> (variante))
  {
    case ENormeEcAc::EU :
    {
      switch (type_)
      {
        case 0 : return 0.0;
        case 1 : return 0.0;
        case 2 : return 0.7;
        case 3 : return 0.7;
        case 4 : return 0.7;
        case 5 : return 0.7;
        case 6 : return 1.0;
        case 7 : return 0.7;
        case 8 : return 0.7;
        case 9 : return 0.0;
        case 10 : return 0.7;
        case 11 : return 0.7;
        case 12 : return 0.5;
        case 13 : return 0.6;
        case 14 : return 0.6;
        case 15 : return 0.0;
        case 16 : return 0.0;
        default :
        {
          BUGPARAM (type_, "%u", false, NAN, UNDO_MANAGER_NULL)
          break;
        }
      }
    }
    case ENormeEcAc::FR :
    {
      switch (type_)
      {
        case 0 : return 0.0;
        case 1 : return 0.0;
        case 2 : return 0.7;
        case 3 : return 0.7;
        case 4 : return 0.7;
        case 5 : return 0.7;
        case 6 : return 1.0;
        case 7 : return 0.7;
        case 8 : return 0.7;
        case 9 : return 0.0;
        case 10 : return 0.7;
        case 11 : return 0.7;
        case 12 : return 1.0;
        case 13 : return 0.7;
        case 14 : return 0.7;
        case 15 : return 0.7;
        case 16 : return 0.5;
        case 17 : return 0.6;
        case 18 : return 0.6;
        case 19 : return 0.0;
        case 20 : return 0.0;
        case 21 : return 0.0;
        default :
        {
          BUGPARAM (type_, "%u", false, NAN, UNDO_MANAGER_NULL)
          break;
        }
      }
    }
    default :
    {
      BUGPARAM (variante, "%u", false, NAN, UNDO_MANAGER_NULL)
      break;
    }
  }
}

double
norme::Eurocode::getPsi1 (uint8_t type_) const
{
  switch (static_cast <ENormeEcAc> (variante))
  {
    case ENormeEcAc::EU :
    {
      switch (type_)
      {
        case 0 : return 0.0;
        case 1 : return 0.0;
        case 2 : return 0.5;
        case 3 : return 0.5;
        case 4 : return 0.7;
        case 5 : return 0.7;
        case 6 : return 0.9;
        case 7 : return 0.7;
        case 8 : return 0.5;
        case 9 : return 0.0;
        case 10 : return 0.5;
        case 11 : return 0.5;
        case 12 : return 0.2;
        case 13 : return 0.2;
        case 14 : return 0.5;
        case 15 : return 0.0;
        case 16 : return 0.0;
        default :
        {
          BUGPARAM (type_, "%u", false, NAN, UNDO_MANAGER_NULL)
          break;
        }
      }
    }
    case ENormeEcAc::FR :
    {
      switch (type_)
      {
        case 0 : return 0.0;
        case 1 : return 0.0;
        case 2 : return 0.5;
        case 3 : return 0.5;
        case 4 : return 0.7;
        case 5 : return 0.7;
        case 6 : return 0.9;
        case 7 : return 0.7;
        case 8 : return 0.5;
        case 9 : return 0.0;
        case 10 : return 0.5;
        case 11 : return 0.7;
        case 12 : return 0.9;
        case 13 : return 0.5;
        case 14 : return 0.5;
        case 15 : return 0.5;
        case 16 : return 0.2;
        case 17 : return 0.2;
        case 18 : return 0.5;
        case 19 : return 0.0;
        case 20 : return 0.0;
        case 21 : return 0.0;
        default :
        {
          BUGPARAM (type_, "%u", false, NAN, UNDO_MANAGER_NULL)
          break;
        }
      }
    }
    default :
    {
      BUGPARAM (variante, "%u", false, NAN, UNDO_MANAGER_NULL)
      break;
    }
  }
}

double
norme::Eurocode::getPsi2 (uint8_t type_) const
{
  switch (static_cast <ENormeEcAc> (variante))
  {
    case ENormeEcAc::EU :
    {
      switch (type_)
      {
        case 0 : return 0.0;
        case 1 : return 0.0;
        case 2 : return 0.3;
        case 3 : return 0.3;
        case 4 : return 0.6;
        case 5 : return 0.6;
        case 6 : return 0.8;
        case 7 : return 0.6;
        case 8 : return 0.3;
        case 9 : return 0.0;
        case 10 : return 0.2;
        case 11 : return 0.2;
        case 12 : return 0.0;
        case 13 : return 0.0;
        case 14 : return 0.0;
        case 15 : return 0.0;
        case 16 : return 0.0;
        default :
        {
          BUGPARAM (type_, "%u", false, NAN, UNDO_MANAGER_NULL)
          break;
        }
      }
    }
    case ENormeEcAc::FR :
    {
      switch (type_)
      {
        case 0 : return 0.0;
        case 1 : return 0.0;
        case 2 : return 0.3;
        case 3 : return 0.3;
        case 4 : return 0.6;
        case 5 : return 0.6;
        case 6 : return 0.8;
        case 7 : return 0.6;
        case 8 : return 0.3;
        case 9 : return 0.0;
        case 10 : return 0.3;
        case 11 : return 0.6;
        case 12 : return 0.5;
        case 13 : return 0.3;
        case 14 : return 0.2;
        case 15 : return 0.2;
        case 16 : return 0.0;
        case 17 : return 0.0;
        case 18 : return 0.0;
        case 19 : return 0.0;
        case 20 : return 0.0;
        case 21 : return 0.0;
        default :
        {
          BUGPARAM (type_, "%u", false, NAN, UNDO_MANAGER_NULL)
          break;
        }
      }
    }
    default :
    {
      BUGPARAM (variante, "%u", false, NAN, UNDO_MANAGER_NULL)
      break;
    }
  }
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
