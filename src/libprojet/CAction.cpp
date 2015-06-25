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
                  uint8_t                       type_,
                  UndoManager                 & undo_) :
  IActionGroupe (nom_, undo_)
  , type (type_)
  , action_predominante (false)
  , charges ()
  , psi0 (std::shared_ptr <INb> (nullptr))
  , psi1 (std::shared_ptr <INb> (nullptr))
  , psi2 (std::shared_ptr <INb> (nullptr))
  , parametres (std::shared_ptr <IParametres> (nullptr))
  , deplacement (nullptr)
  , forces (nullptr)
  , efforts_noeuds (nullptr)
  , efforts ({ { {}, {}, {}, {}, {}, {} } })
  , deformation ({ { {}, {}, {}} })
  , rotation ({ { {}, {}, {}} })
{
}

CAction::~CAction ()
{
}

bool CHK
CAction::addXML (std::string *nom_,
                 std::string *type_,
                 xmlNodePtr   root)
{
  BUGPARAM (static_cast <void *> (root), "%p", root, false, UNDO_MANAGER_NULL)
  
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                                 xmlNewNode (nullptr, BAD_CAST2 ("addAction")),
                                 xmlFreeNode);
  
  BUGCRIT (node.get () != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Erreur d'allocation mémoire.\n")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("Nom"),
                       BAD_CAST2 (nom_->c_str ())) != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Problème depuis la librairie : %s\n", "xml2")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("Type"),
                       BAD_CAST2 (type_->c_str ())) != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Problème depuis la librairie : %s\n", "xml2")
  
  BUGCRIT (xmlAddChild (root, node.get ()) != nullptr,
           false,
           UNDO_MANAGER_NULL,
           "Problème depuis la librairie : %s\n", "xml2")
  
  node.release ();
  
  return true;
}

uint8_t
CAction::getType () const
{
  return type;
}

bool CHK
CAction::setpsiXML (std::string * const nom_,
                    uint8_t             psi,
                    INb                *psin,
                    xmlNodePtr          root) const
{
  BUGPARAM (static_cast <void *> (root), "%p", root, false, &getUndoManager ())
  BUGPARAM (psi, "%u", psi <= 2, false, &getUndoManager ())
  
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                                    xmlNewNode (nullptr, BAD_CAST2 ("setpsi")),
                                    xmlFreeNode);
  
  BUGCRIT (node.get () != nullptr,
           false,
           &getUndoManager (),
           "Erreur d'allocation mémoire.\n")
  
  BUGCRIT (xmlSetProp (
             node.get (),
             BAD_CAST2 ("psi"),
             BAD_CAST2 (psi == 0 ? "0" : psi == 1 ? "1" : "2")) != nullptr,
           false,
           &getUndoManager (),
           "Problème depuis la librairie : %s\n", "xml2")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("Nom"),
                       BAD_CAST2 (nom_->c_str ())) != nullptr,
           false,
           &getUndoManager (),
           "Problème depuis la librairie : %s\n", "xml2")
  
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node0 (
                                       xmlNewNode (nullptr, BAD_CAST2 ("val")),
                                       xmlFreeNode);
  
  BUGCRIT (node0.get () != nullptr,
           false,
           &getUndoManager (),
           "Erreur d'allocation mémoire.\n")
  
  BUGCONT (psin->newXML (node0.get ()), false, &getUndoManager ())
  
  BUGCRIT (xmlAddChild (node.get (), node0.get ()) != nullptr,
           false,
           &getUndoManager (),
           "Problème depuis la librairie : %s\n", "xml2")
  node0.release ();
  
  BUGCRIT (xmlAddChild (root, node.get ()) != nullptr,
           false,
           &getUndoManager (),
           "Problème depuis la librairie : %s\n", "xml2")
  node.release ();
  
  return true;
}

INb const *
CAction::getPsi (uint8_t psi) const
{
  BUGPARAM (psi, "%u", psi <= 2, nullptr, &getUndoManager ())

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
CAction::setPsi (uint8_t psi, std::shared_ptr <INb> val)
{
  BUGPARAM (psi, "%u", psi <= 2, false, &getUndoManager ())

  if (val != NULL)
  {
    BUGPROG (val.get ()->getUnite () == EUnite::U_,
             false,
             &getUndoManager (),
             "L'unité est de type [%s] à la place de [%s].\n",
               EUniteConst[static_cast <size_t> (val.get ()->getUnite ())].
                                                                      c_str (),
               EUniteConst[static_cast <size_t> (EUnite::U_)].c_str ())
  }
  
  BUGCONT (getUndoManager ().ref (), false, &getUndoManager ())
  
  BUGCONT (getUndoManager ().push (
             psi == 0 ? std::bind (&CAction::setPsi, this, 0, psi0) :
               psi == 1 ? std::bind (&CAction::setPsi, this, 1, psi1) :
                 std::bind (&CAction::setPsi, this, 2, psi2),
             std::bind (&CAction::setPsi, this, psi, val),
             val,
             std::bind (&CAction::setpsiXML,
                        this,
                        getNom ().get (),
                        psi,
                        val.get (),
                        std::placeholders::_1),
             val != NULL ?
               format (gettext ("Cœfficient ψ%C de l'action “%s” (%lf)"),
                       0x2080 + psi,
                       getNom ().get ()->c_str (),
                       val.get ()->getVal ()) :
               ""),
           false,
           &getUndoManager ())
  if (psi == 0)
  {
    psi0 = val;
  }
  else if (psi == 1)
  {
    psi1 = val;
  }
  else
  {
    psi2 = val;
  }
  
  BUGCONT (getUndoManager ().unref (), false, &getUndoManager ())
  
  return true;
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

bool CHK
CAction::setParam (std::shared_ptr <IParametres> param,
                   std::shared_ptr <INb>         psi0_,
                   std::shared_ptr <INb>         psi1_,
                   std::shared_ptr <INb>         psi2_)
{
  bool ins = getUndoManager ().getInsertion ();
  
  BUGCONT (getUndoManager ().ref (), false, &getUndoManager ())
  
  getUndoManager ().setInsertion (false);
  
  BUGCONT (setPsi (0, psi0_), false, &getUndoManager ())
  BUGCONT (setPsi (1, psi1_), false, &getUndoManager ())
  BUGCONT (setPsi (2, psi2_), false, &getUndoManager ())
  
  getUndoManager ().setInsertion (ins);
  
  BUGCONT (getUndoManager ().push (
             std::bind (&CAction::setParam,
                        this,
                        parametres,
                        psi0,
                        psi1,
                        psi2),
             std::bind (&CAction::setParam,
                        this,
                        param,
                        psi0_,
                        psi1_,
                        psi2_),
             param,
             std::bind (&CAction::setParamXML,
                        this,
                        getNom ().get (),
                        param != NULL ?
                          param.get ()->getNom ().get () :
                          nullptr,
                        psi0_.get (),
                        psi1_.get (),
                        psi2_.get (),
                        std::placeholders::_1),
             format (gettext ("Paramètres de l'action “%s” (%s)"),
                     getNom ()->c_str (),
                     param != NULL ?
                       param.get ()->getNom ()->c_str () :
                       nullptr)),
           false,
           &getUndoManager ())
  
  BUGCONT (getUndoManager ().pushSuppr (psi0_), false, &getUndoManager ())
  BUGCONT (getUndoManager ().pushSuppr (psi1_), false, &getUndoManager ())
  BUGCONT (getUndoManager ().pushSuppr (psi2_), false, &getUndoManager ())
  
  parametres = param;
  
  BUGCONT (getUndoManager ().unref (), false, &getUndoManager ())
  
  return true;
}

bool CHK
CAction::setParamXML (std::string * action,
                      std::string * param,
                      INb         * psi0_,
                      INb         * psi1_,
                      INb         * psi2_,
                      xmlNodePtr    root) const
{
  BUGPARAM (static_cast <void *> (root), "%p", root, false, &getUndoManager ())
  
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node (
                            xmlNewNode (nullptr, BAD_CAST2 ("actionSetParam")),
                            xmlFreeNode);
  
  BUGCRIT (node.get () != nullptr,
           false,
           &getUndoManager (),
           "Erreur d'allocation mémoire.\n")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("Action"),
                       BAD_CAST2 (action->c_str ())) != nullptr,
           false,
           &getUndoManager (),
           "Problème depuis la librairie : %s\n", "xml2")
  
  BUGCRIT (xmlSetProp (node.get (),
                       BAD_CAST2 ("Param"),
                       param != nullptr ?
                         BAD_CAST2 (param->c_str ()) :
                         BAD_CAST2 ("")) != nullptr,
           false,
           &getUndoManager (),
           "Problème depuis la librairie : %s\n", "xml2")
  
  std::unique_ptr <xmlNode, void (*)(xmlNodePtr)> node0 (
                                      xmlNewNode (nullptr, BAD_CAST2 ("psi0")),
                                      xmlFreeNode);
  
  BUGCRIT (node0.get () != nullptr,
           false,
           &getUndoManager (),
           "Erreur d'allocation mémoire.\n")
  
  BUGCONT (psi0_->newXML (node0.get ()), false, &getUndoManager ())
  
  BUGCRIT (xmlAddChild (node.get (), node0.get ()) != nullptr,
           false,
           &getUndoManager (),
           "Problème depuis la librairie : %s\n", "xml2")
  node0.release ();
  
  node0.reset (xmlNewNode (nullptr, BAD_CAST2 ("psi1")));
  
  BUGCRIT (node0.get () != nullptr,
           false,
           &getUndoManager (),
           "Erreur d'allocation mémoire.\n")
  
  BUGCONT (psi1_->newXML (node0.get ()), false, &getUndoManager ())
  
  BUGCRIT (xmlAddChild (node.get (), node0.get ()) != nullptr,
           false,
           &getUndoManager (),
           "Problème depuis la librairie : %s\n", "xml2")
  node0.release ();
  
  node0.reset (xmlNewNode (nullptr, BAD_CAST2 ("psi2")));
  
  BUGCRIT (node0.get () != nullptr,
           false,
           &getUndoManager (),
           "Erreur d'allocation mémoire.\n")
  
  BUGCONT (psi2_->newXML (node0.get ()), false, &getUndoManager ())
  
  BUGCRIT (xmlAddChild (node.get (), node0.get ()) != nullptr,
           false,
           &getUndoManager (),
           "Problème depuis la librairie : %s\n", "xml2")
  node0.release ();
  
  BUGCRIT (xmlAddChild (root, node.get ()) != nullptr,
           false,
           &getUndoManager (),
           "Problème depuis la librairie : %s\n", "xml2")
  node.release ();
  
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */