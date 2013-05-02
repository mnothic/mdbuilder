/*-
 * Copyright (c) 1990, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
prefix=/usr/local
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: confuse
Version: 2.6
Description: configuration file parser library
Requires:
Libs: -L${libdir} -lconfuse
Cflags: -I${includedir}
**/

#ifndef MD_PARSER_H
#define MD_PARSER_H

#include <confuse.h>
#include <err.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

cfg_t *cfg;

cfg_opt_t options [] = {
    CFG_STR("hostname",     "hostname",                             CFGF_NONE),
    CFG_STR("user",         "myuser",                               CFGF_NONE),
    CFG_STR("password",     "mypassword",                           CFGF_NONE),
    CFG_STR("database",     "dbname",                               CFGF_NONE),
    CFG_STR("getquery",     "SELECT * FROM mytable WHERE state=%s", CFGF_NONE),
    CFG_STR("rmquery",      "SELECT * FROM mytable WHERE state=%s", CFGF_NONE),
    CFG_STR("update_a",     "UPDATE table SET state=0 WHERE id=%s", CFGF_NONE),
    CFG_STR("update_b",     "UPDATE table SET state=0 WHERE id=%s", CFGF_NONE),
    CFG_STR("table",        "mytable",                              CFGF_NONE),
    CFG_STR("basehome",     "/var/vmail",                           CFGF_NONE),
    CFG_STR("delsql",       "DELETE FROM mytable WHERE id = %s",    CFGF_NONE),
    CFG_STR("pidfile",      "/var/run/mdbuilder/mdbd.pid",          CFGF_NONE),
    CFG_END()
};

char *dsn_h;
char *table_h;
char *getquery_h;
char *rmquery_h;
char *update_a_h;
char *update_b_h;
char *basehome_h;
char *delsql_h;
char *pidfile_h;

int parse_file(char *ptrcfg);
int create_cfg(char *file);
char *create_dsn(char *args, ...);

#endif // MD_PARSER_H INCLUDED
