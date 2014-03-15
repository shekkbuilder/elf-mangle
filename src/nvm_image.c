///@file
///@brief	Handle input and output of blob data to raw binary files
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


#include "config.h"

#include "symbol_list.h"
#include "nvm_field.h"
#include "intl.h"

#ifdef USE_MMAP
#include <sys/mman.h>
#endif
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifdef DEBUG
#undef DEBUG
#endif

// Default to seek-based implementation
#ifndef USE_MMAP
#define USE_MMAP 0
#endif



/// Structure of data passed to read iterator function
struct read_symbols_config {
    /// Reference to the source of binary data
    union {
	/// Base address of memory mapped data
	const char*	addr;
	/// File descriptor for file-based data
	int		fd;
    }			source;
    /// Size of source data in bytes
    size_t		size;
};



///@brief Iterator function to read symbol content from memory mapped image
///@see symbol_list_iterator_f
static const nvm_symbol*
read_symbol_mem_iterator(
    const nvm_symbol *symbol,	///< [in] Target symbol to copy data
    const void *arg)		///< [in] Data source configuration
{
    const struct read_symbols_config *conf = arg;
    size_t available;

    // Abort iteration if no data is available
    if (! conf || ! conf->source.addr) return symbol;

    if (symbol->blob_address) {					//destination ok
	// Calculate maximum size of available source data
	available = conf->size - symbol->offset;
	if (available > symbol->size) available = symbol->size;	//limit to symbol size
	memcpy(symbol->blob_address, conf->source.addr + symbol->offset, available);
    }
    return NULL;	//continue iterating
}



///@brief Iterator function to read symbol content from image file
///@see symbol_list_iterator_f
static const nvm_symbol*
read_symbol_seek_iterator(
    const nvm_symbol *symbol,	///< [in] Target symbol to copy data
    const void *arg)		///< [in] Data source configuration
{
    const struct read_symbols_config *conf = arg;
    size_t bytes_read;

    if (symbol->blob_address) {					//destination ok
	if (symbol->offset == (size_t) lseek(conf->source.fd, symbol->offset, SEEK_SET)) {
#ifdef DEBUG
	    fprintf(stderr, _("%s: copy %zu bytes from file offset %zu to %p\n"), __func__,
		    symbol->size, symbol->offset, symbol->blob_address);
#endif
	    bytes_read = read(conf->source.fd, symbol->blob_address, symbol->size);
	    if (bytes_read != symbol->size) {
		fprintf(stderr, _("Failed to read %s from file offset %zu to %p (%s)\n"),
			symbol->field->symbol, symbol->offset, symbol->blob_address,
			strerror(errno));
		//FIXME return symbol;	//stops iterating
	    }
	}
    }
    return NULL;	//continue iterating
}



void
nvm_image_merge_file(const char *filename,
		     const nvm_symbol list[], int list_size,
		     size_t blob_size)
{
    struct read_symbols_config conf = { .size = blob_size };
    char *mapped = NULL;
    int fd;
    struct stat st;

    if (! filename || ! blob_size) return;

    fd = open(filename, O_RDONLY);
    if (fd == -1) {		//file not opened
	fprintf(stderr, _("Cannot open image \"%s\" (%s)\n"), filename, strerror(errno));
    } else {
	fstat(fd, &st);
	if (st.st_size == 0) {
	    fprintf(stderr, _("Image file \"%s\" is empty\n"), filename);
	} else {
	    if (blob_size - st.st_size > 0) {
		fprintf(stderr, _("Image file \"%s\" is too small, %zu of %zu bytes missing\n"),
			filename, blob_size - st.st_size, blob_size);
		blob_size = st.st_size;
	    }

#if USE_MMAP
	    mapped = mmap(NULL, blob_size, PROT_READ, MAP_PRIVATE, fd, 0);
	    if (mapped == MAP_FAILED) {
		mapped = NULL;
		fprintf(stderr, "%s: mmap() failed (%s)\n", __func__, strerror(errno));
	    }
#endif
	    if (mapped) {
		conf.source.addr = mapped;
		symbol_list_foreach(list, list_size, read_symbol_mem_iterator, &conf);
#if USE_MMAP
		munmap(mapped, blob_size);
#endif
	    } else {
		conf.source.fd = fd;
		symbol_list_foreach(list, list_size, read_symbol_seek_iterator, &conf);
	    }
	}
	close(fd);
    }
}



void
nvm_image_write_file(const char *filename,
		     const char* blob, size_t blob_size)
{
    int fd;

    if (! filename || ! blob || ! blob_size) return;

    fd = open(filename, O_WRONLY | O_CREAT, 0660);
    if (fd == -1) {		//file not opened
	fprintf(stderr, _("Cannot open image \"%s\" (%s)\n"), filename, strerror(errno));
    } else {
	if (ftruncate(fd, (off_t) blob_size) != 0) {
	    fprintf(stderr, _("Cannot resize image file \"%s\" to %zu bytes (%s)\n"),
		    filename, blob_size, strerror(errno));
	} else {
	    write(fd, blob, blob_size);
	}
	close(fd);
    }
}