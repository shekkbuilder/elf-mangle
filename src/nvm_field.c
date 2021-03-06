///@file
///@brief	Helper functions to handle data field descriptors
///@copyright	Copyright (C) 2014, 2015  Andre Colomb
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
///@author	Andre Colomb <src@andre.colomb.de>


#include "config.h"

#include "nvm_field.h"
#include "intl.h"

#include <stdio.h>
#include <string.h>

/// Compile diagnostic output messages?
#define DEBUG 0



const nvm_field*
find_field(const char *symbol,
	   const nvm_field fields[], size_t num_fields)
{
    const nvm_field *field;
    int comp;

    for (field = fields; field < fields + num_fields; ++field) {
	comp = strcmp(field->symbol, symbol);
	if (DEBUG) printf("%s: (%s; %s) = %d\n", __func__, field->symbol, symbol, comp);
	if (comp == 0) return field;
	else if (comp > 0) break;
    }
    return NULL;
}



size_t
copy_field_verbatim(const nvm_field *field __attribute__((unused)),
		    char *dst, const char *src,
		    size_t dst_size, size_t src_size)
{
    size_t common_size;

    // Limit copying to the smaller of the source and destination fields
    common_size = src_size < dst_size ? src_size : dst_size;
    memcpy(dst, src, common_size);
    return common_size;
}



size_t
copy_field_noop(const nvm_field *field __attribute__((unused)),
		char *dst __attribute__((unused)), const char *src __attribute__((unused)),
		size_t dst_size __attribute__((unused)), size_t src_size __attribute__((unused)))
{
    return 0;
}
