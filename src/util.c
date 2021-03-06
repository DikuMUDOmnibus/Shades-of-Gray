/*-
 * Copyright (c) 1998 fjoe <fjoe@iclub.nsu.ru>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: util.c,v 1.20 1999/02/23 22:26:11 fjoe Exp $
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#if !defined (WIN32)
#include <unistd.h>
#endif

#include "merc.h"

#ifdef SUNOS
#	include "compat/compat.h"
#endif

#if defined(WIN32)
#define unlink	_unlink
#endif

void doprintf(DO_FUN *fn, CHAR_DATA* ch, const char* fmt, ...)
{
	char buf[MAX_STRING_LENGTH];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	fn(ch, buf);
	va_end(ap);
}

FILE *dfopen(const char *dir, const char *file, const char *mode)
{
	char name[PATH_MAX];
	FILE *f;
	snprintf(name, sizeof(name), "%s%c%s", dir, PATH_SEPARATOR, file);
	if ((f = fopen(name, mode)) == NULL)
		log_printf("%s: %s", name, strerror(errno));
	return f;
}

int dunlink(const char *dir, const char *file)
{
	char name[PATH_MAX];
	snprintf(name, sizeof(name), "%s%c%s", dir, PATH_SEPARATOR, file);
	return unlink(name);
}

int d2rename(const char *dir1, const char *file1,
	     const char *dir2, const char *file2)
{
	int res;
	char name1[PATH_MAX];
	char name2[PATH_MAX];
	snprintf(name1, sizeof(name1), "%s%c%s", dir1, PATH_SEPARATOR, file1);
	snprintf(name2, sizeof(name2), "%s%c%s", dir2, PATH_SEPARATOR, file2);
#if defined (WIN32)
	res = unlink(name2);
	if (res == -1)
		log_printf("d2rename: can't delete file %s", name2);
#endif
	res = rename(name1, name2);
	if (res < 0)
		log_printf("d2rename: error renaming %s -> %s", name1, name2);
	return res;
}

bool dfexist(const char *dir, const char *file)
{
	struct stat sb;
	char name[PATH_MAX];
	snprintf(name, sizeof(name), "%s%c%s", dir, PATH_SEPARATOR, file);
	return (stat(name, &sb) >= 0);
}

const char *get_filename(const char *name)
{
	const char *p = (p = strrchr(name, PATH_SEPARATOR)) ? ++p : name;
	return str_dup(p);
}

int cmpint(const void *p1, const void *p2)
{
	return *(int*) p1 - *(int*) p2;
}

size_t cstrlen(const char *cstr)
{
	size_t res;

	if (cstr == NULL)
		return 0;

	res = strlen(cstr);
	while ((cstr = strchr(cstr, '{')) != NULL) {
		if (*(cstr+1) == '{')
			res--;
		else
			res -= 2;
		cstr += 2;
	}

	return res;
}

const char *cstrfirst(const char *cstr)
{
	if (cstr == NULL)
		return NULL;

	for (; *cstr == '{'; cstr++)
		if (*(cstr+1))
			cstr++;
	return cstr;
}

char *strtime(time_t time)
{
	char *p = ctime(&time);
	p[24] = '\0';
	return p;
}
