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
#include "parser.h"
#include "util.h"
/**
* This function parse the file config and set the program
* values which program need
*/
int parse_file(char *ptrcfg)
{
    cfg = cfg_init(options, CFGF_NONE);

    if (cfg_parse(cfg, ptrcfg) == CFG_PARSE_ERROR)
        return CFG_PARSE_ERROR;

    dsn_h = create_dsn(cfg_getstr(cfg,"hostname"),
                      cfg_getstr(cfg,"database"),
                      cfg_getstr(cfg,"user"),
                      cfg_getstr(cfg,"password"));

    table_h         = mdb_alloc_str(cfg_getstr(cfg,"table"));
    rmquery_h       = mdb_alloc_str(cfg_getstr(cfg,"rmquery"));
    getquery_h      = mdb_alloc_str(cfg_getstr(cfg,"getquery"));
    basehome_h      = mdb_alloc_str(cfg_getstr(cfg,"basehome"));
    update_a_h      = mdb_alloc_str(cfg_getstr(cfg,"update_a"));
    update_b_h      = mdb_alloc_str(cfg_getstr(cfg,"update_b"));
    delsql_h        = mdb_alloc_str(cfg_getstr(cfg,"delsql"));
    pidfile_h       = mdb_alloc_str(cfg_getstr(cfg,"pidfile"));
    cfg_free(cfg);
    return 0;
}

/**
* This pointer to the char function return a dsn
* string in the stack ready to use.
*/
char *create_dsn(char *args, ...)
{
    va_list ap;
    int i,len;
    char *s,*vars[] = {"host=","dbname=","user=","password="};
    const char *dsn = (char *)malloc(sizeof(char));
    char *ptr = (char *) dsn;
    va_start(ap,args);
    dsn = 0;
    s = args;
    /**
    * This loop must be write with a while (s) but
    * s never take a NULL value, and begin to take
    * garbage information.
    */
    for (i=0;i<4;i++)
    {
        if (dsn == NULL)
            len = strlen(vars[i])+strlen(s)+1;
        else
            len = strlen(vars[i])+strlen(s)+strlen(dsn)+2;
        dsn = realloc((char *)dsn,len);

        if (dsn == NULL)
        {
            free(ptr);
            err(134,"out of memory, not possible DSN alloc");
            va_end(ap);
            return NULL;
        }
        else
            ptr = (char *)dsn;
        /** NOTE: remeber convert spirntf to vsprintf or similar */
        if (i == 0)
            snprintf(dsn,len,"%s%s",vars[i],s);
        else
            snprintf(dsn,len,"%s %s%s",dsn,vars[i],s);
        s = va_arg(ap,char *);
    }
    va_end(ap);
    return dsn;
}

/**
* This function create the config file
* with a few default values as examples
* if the config file don't exist
*/

int create_cfg(char *file) {
    cfg = cfg_init(options, CFGF_NONE);
    FILE *fp;
    if ((fp = fopen(file, "w")) != NULL)
    {
        cfg_print(cfg, fp);
        fclose(fp);
    }
    else {
        cfg_free(cfg);
        return 1;
    }
    cfg_free(cfg);
    return 0;
}


