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

#include <locale>

#include "common_projet.hpp"
#include "common_erreurs.hpp"
#include "EF_rigidite.hpp"

/**
 * \brief Initialise à NULL les différentes matrices de rigidité.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
EF_rigidite_init (Projet *p)
{
  BUGPARAM (p, "%p", p, false)
  
  p->calculs.m_part = NULL;
  p->calculs.m_comp = NULL;
  p->calculs.numeric = NULL;
  p->calculs.ap = NULL;
  p->calculs.ai = NULL;
  p->calculs.ax = NULL;
  p->calculs.t_part = NULL;
  p->calculs.t_comp = NULL;
  p->calculs.n_comp = NULL;
  p->calculs.n_part = NULL;
  
  return true;
}


/**
 * \brief Libère la liste contenant la matrice de rigidité.
 * \param p : la variable projet.
 * \return
 *   Succès : true.\n
 *   Échec : false :
 *     - p == NULL.
 */
bool
EF_rigidite_free (Projet *p)
{
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
