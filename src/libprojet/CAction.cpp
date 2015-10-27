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

#include "CAction.hpp"
#include "EUniteTxt.hpp"
#include "MErreurs.hpp"
#include "SString.hpp"

CAction::CAction (std::shared_ptr <std::string> nom_,
                  uint8_t                       type_) :
  IActionGroupe (nom_),
  type (type_),
  action_predominante (false),
  charges (),
  psi0 (std::shared_ptr <INb> (nullptr)),
  psi1 (std::shared_ptr <INb> (nullptr)),
  psi2 (std::shared_ptr <INb> (nullptr)),
  parametres (std::shared_ptr <IParametres> (nullptr)),
  deplacement (nullptr),
  forces (nullptr),
  efforts_noeuds (nullptr),
  efforts ({ { {}, {}, {}, {}, {}, {} } }),
  deformation ({ { {}, {}, {}} }),
  rotation ({ { {}, {}, {}} })
{
}

CAction::~CAction ()
{
}

uint8_t
CAction::getType () const
{
  return type;
}

INb const *
CAction::getPsi (uint8_t psi) const
{
  BUGPARAM (psi, "%u", psi <= 2, nullptr, UNDO_MANAGER_NULL)

  if (psi == 0)
  {
    return psi0.get ();
  }
  else if (psi == 1)
  {
    return psi1.get ();
  }
  else
  {
    return psi2.get ();
  }
}

bool CHK
CAction::emptyCharges () const
{
  return charges.empty ();
}

std::string const
CAction::getDescription (uint8_t type_) const
{
  return parametres->getpsiDescription (type_);
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
