/** @file container_suppl.h
 *
 *  Helper classes shared by all containers. */

/*
 *  GiNaC Copyright (C) 1999-2004 Johannes Gutenberg University Mainz, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __GINAC_CONTAINER_SUPPL_H__
#define __GINAC_CONTAINER_SUPPL_H__

namespace GiNaC {

/** Helper template to allow initialization of containers via an overloaded
 *  comma operator (idea stolen from Blitz++). */
template <typename T, typename STLT>
class container_init {
public:
	container_init(STLT & s) : stlt(s) {}

	container_init<T, STLT> operator,(const T & x)
	{
		stlt.push_back(x);
		return container_init<T, STLT>(stlt);
	}

	// The following specializations produce much tighter code than the
	// general case above

	container_init<T, STLT> operator,(int x)
	{
		stlt.push_back(x);
		return container_init<T, STLT>(stlt);
	}

	container_init<T, STLT> operator,(unsigned int x)
	{
		stlt.push_back(x);
		return container_init<T, STLT>(stlt);
	}

	container_init<T, STLT> operator,(long x)
	{
		stlt.push_back(x);
		return container_init<T, STLT>(stlt);
	}

	container_init<T, STLT> operator,(unsigned long x)
	{
		stlt.push_back(x);
		return container_init<T, STLT>(stlt);
	}

	container_init<T, STLT> operator,(double x)
	{
		stlt.push_back(x);
		return container_init<T, STLT>(stlt);
	}

	container_init<T, STLT> operator,(const symbol & x)
	{
		stlt.push_back(T(x));
		return container_init<T, STLT>(stlt);
	}

private:
	container_init();
	STLT & stlt;
};

} // namespace GiNaC

#endif // ndef __GINAC_CONTAINER_SUPPL_H__
