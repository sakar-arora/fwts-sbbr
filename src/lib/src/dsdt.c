/*
 * Copyright (C) 2010 Canonical
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#include <stdlib.h>
#include <stdio.h>

#define DSDT_FILE	"/proc/acpi/dsdt"

#include "framework.h"

char *dsdt_read(log *log)
{
	FILE *fp;
	char *dsdt = NULL;
	char buffer[4096];
	int len = 0;
	int error = 0;

	if ((fp = fopen(DSDT_FILE, "r")) == NULL) {
		log_error(log, "Cannot open DSDT file %s\n", DSDT_FILE);
		return NULL;
	}

	while (!feof(fp)) {
		int n;

		if ((n = fread(buffer, 1, sizeof(buffer), fp)) == 0) {
			if (ferror(fp)) {
				error = 1;
				break;
			}
			
		}
		if ((dsdt = realloc(dsdt, len + n)) == NULL) {
			error = 1;
			break;
		}
		memcpy(dsdt + len, buffer, n);
		len += n;
	}

	fclose(fp);

	if (error) {
		free(dsdt);
		dsdt = NULL;
	}
		
	return dsdt;
}

int dsdt_copy(log *log, const char *destination)
{
	FILE *dsdt;
	FILE *dest;
	char buffer[4096];
	int error = 0;

	if ((dsdt = fopen(DSDT_FILE, "r")) == NULL) {
		log_error(log, "Cannot open DSDT file %s\n", DSDT_FILE);
		return 1;
	}

	if ((dest = fopen(destination, "w")) == NULL) {
		log_error(log, "Cannot open file %s\n", destination);
		return 1;
	}
	
	while (!feof(dsdt)) {
		int n ;

		if ((n = fread(buffer, 1, sizeof(buffer), dsdt)) == 0) {
			if (ferror(dsdt)) {
				error = 1;
				break;
			}
			
		}
		if (n != fwrite(buffer, 1, n, dest)) {
			error = 1;
			break;
		}
	}

	fclose(dsdt);
	fclose(dest);

	if (error)
		unlink(destination);
		
	return error;
}
