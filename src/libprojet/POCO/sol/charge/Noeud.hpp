#ifndef POCO_SOL_CHARGE_NOEUD__HPP
#define POCO_SOL_CHARGE_NOEUD__HPP

/*
2lgc_code : calcul de résistance des matériaux selon les normes Eurocodes
Copyright (C) 2011-2015

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Fichier généré automatiquement avec dia2code 0.9.0.
 */

#include "POCO/sol/ICharge.hpp"
#include "POCO/str/INoeud.hpp"

namespace POCO
{
  namespace sol
  {
    namespace charge
    {
      /**
       * \brief Charge ponctuelle appliquée à un nœud.
       */
      class Noeud : public POCO::sol::ICharge
      {
        // Attributes
        private :
          /// Liste des nœuds où est appliquée la charge.
          std::list <POCO::str::INoeud *> noeuds;
          /// Charge ponctuelle en N dans l'axe x.
          INb * fx;
          /// Charge ponctuelle en N dans l'axe y.
          INb * fy;
          /// Charge ponctuelle en N dans l'axe z.
          INb * fz;
          /// Moment ponctuelle en N.m autour de l'axe x.
          INb * mx;
          /// Moment ponctuelle en N.m autour de l'axe y.
          INb * my;
          /// Moment ponctuelle en N.m autour de l'axe z.
          INb * mz;
        // Operations
        public :
          /**
           * \brief Constructeur d'une classe Noeud.
           * \param nom (in) Le nom de la charge.
           * \param noeuds (in) Liste des nœuds où est appliquée la charge.
           * \param fx (in) Charge ponctuelle en N dans l'axe x.
           * \param fy (in) Charge ponctuelle en N dans l'axe y.
           * \param fz (in) Charge ponctuelle en N dans l'axe z.
           * \param mx (in) Moment ponctuelle en N.m autour de l'axe x.
           * \param my (in) Moment ponctuelle en N.m autour de l'axe y.
           * \param mz (in) Moment ponctuelle en N.m autour de l'axe z.
           */
          Noeud (std::string nom, std::list <POCO::str::INoeud *> noeuds, INb * fx, INb * fy, INb * fz, INb * mx, INb * my, INb * mz);
          /**
           * \brief Duplication d'une classe Noeud.
           * \param other (in) La classe à dupliquer.
           */
          Noeud (const Noeud & other) = delete;
          /**
           * \brief Assignment operator de Noeud.
           * \param other (in) La classe à dupliquer.
           * \return Noeud &
           */
          Noeud & operator = (const Noeud & other) = delete;
          /**
           * \brief Destructeur d'une classe Noeud.
           */
          virtual ~Noeud ();
          /**
           * \brief Renvoie la charge ponctuelle dans l'axe x.
           * \return INb const &
           */
          INb const & getfx () const;
          /**
           * \brief Renvoie la charge ponctuelle dans l'axe x.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setfx (INb * val);
          /**
           * \brief Renvoie la charge ponctuelle dans l'axe y.
           * \return INb const &
           */
          INb const & getfy () const;
          /**
           * \brief Renvoie la charge ponctuelle dans l'axe y.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setfy (INb * val);
          /**
           * \brief Renvoie la charge ponctuelle dans l'axe z.
           * \return INb const &
           */
          INb const & getfz () const;
          /**
           * \brief Renvoie la charge ponctuelle dans l'axe z.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setfz (INb * val);
          /**
           * \brief Renvoie le moment ponctuel autour de l'axe x.
           * \return INb const &
           */
          INb const & getmx () const;
          /**
           * \brief Renvoie le moment ponctuel autour de l'axe x.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setmx (INb * val);
          /**
           * \brief Renvoie le moment ponctuel autour de l'axe y.
           * \return INb const &
           */
          INb const & getmy () const;
          /**
           * \brief Renvoie le moment ponctuel autour de l'axe y.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setmy (INb * val);
          /**
           * \brief Renvoie le moment ponctuel autour de l'axe z.
           * \return INb const &
           */
          INb const & getmz () const;
          /**
           * \brief Renvoie le moment ponctuel autour de l'axe z.
           * \param val (in) La nouvelle valeur.
           * \return bool CHK
           */
          bool CHK setmz (INb * val);
          /**
           * \brief Ajoute plusieurs nœuds à la charge.
           * \param noeuds (in) La liste de nœuds à ajouter.
           * \return bool CHK
           */
          bool CHK addNoeuds (std::list <POCO::str::INoeud *> & noeuds);
          /**
           * \brief Enlève plusieurs nœuds à la charge.
           * \param noeuds (in) La liste de nœuds à enlever.
           * \return bool CHK
           */
          bool CHK rmNoeuds (std::list <POCO::str::INoeud *> & noeuds);
      };
    }
  }
}

#endif
