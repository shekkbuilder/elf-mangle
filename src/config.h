///@file
///@brief	Compile-time configuration constants
///@copyright	Copyright (C) 2014  Andre Colomb
///
/// This file is part of elf-mangle.
///
/// This file is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Lesser General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// elf-mangle is distributed in the hope that it will be useful, but
/// WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
/// Lesser General Public License for more details.
///
/// You should have received a copy of the GNU Lesser General Public
/// License along with this program.  If not, see
/// <http://www.gnu.org/licenses/>.
///
///@author	Andre Colomb <andre.colomb@auteko.de>


#ifndef CONFIG_H_
#define CONFIG_H_

/// Make use of strdup(), getsubopt()
#define _XOPEN_SOURCE 500

/// Use memory mapping to access binary image
#define USE_MMAP 1
//#undef USE_MMAP

/// Compile debugging code if defined
#define DEBUG
//#undef DEBUG

#endif //CONFIG_H_