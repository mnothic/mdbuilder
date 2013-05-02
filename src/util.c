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
#include "util.h"

int file_exist(char *file)
{
    FILE *fp;
    if ( (fp = fopen(file,"r")) != NULL ) {
        fclose(fp);
        return 1;
    }
    return 0;
}

int set_pid_file(char *pfile, pid_t pid)
{
    FILE *fp;
    if ((fp = fopen(pfile, "w")) != NULL)
    {
        fprintf(fp,"%d",pid);
        fclose(fp);
    }
    else
        return 1;
    return 0;
}

char *mdb_sql(char *sql,char *val)
{
    char *buff = NULL;
    int len;

    if (sql != NULL && val != NULL) {
        len = strlen(sql)+strlen(val)+sizeof(char);
        buff = (char *)malloc(len);
    } else
        return NULL;

    if ( buff != NULL )
        snprintf(buff,len,sql,val);
    else
        perror("mdb_strcat no memory");

    if ( buff )
        return buff;
    else
        return NULL;
}

void mdb_free(char *p)
{
    if (p != NULL) {
        free(p);
        p = NULL;
    }
}

char *mdb_alloc_str(char *str)
{
    int len;
    char *sp = (char *)malloc(sizeof(char));
    if (str == NULL)
        return NULL;
    len = strlen(str)+sizeof(char);
    sp = realloc((char *)sp,len);
    if (sp != NULL)
        strncpy(sp,str,len);
    else {
        free(sp);
        return NULL;
    }
    return (char *)sp;
}
/**
* reemplazar system() en el siguiente prototipo
* por un borrado recursivo observando l
* a structura stat del archivo encontrado;
* esto es solo para probar el borrado
* @return int
* @param char *
*/
int rm(char *path)
{
    char * cmd = (char *)malloc(sizeof(char));
    char rm[] = "rm -rf ";
    int len,r;

    len = strlen(rm)+strlen(path)+sizeof(char);
    cmd = realloc(cmd,len);
    if (cmd == NULL)
        return -1;
    snprintf(cmd,len,"%s%s",rm,path);
    r = system(cmd);
    free(cmd);
    if (dir_exist(path)) {
        openlog(PACKAGE,LOG_PID,LOG_DAEMON);
        syslog(LOG_ALERT,"Can't delete directory %s",path);
        closelog();
        return 1;
    }
    return 0;
}

int dir_exist(char *dir_path)
{
    DIR *dp;
    if ((dp = opendir(dir_path)) != NULL) {
        closedir(dp);
        return 1;
    }
    return 0;
}

/* FreeBSD mkdir function code! /usr/src/bin/mkdir.c */
int build(char *path, mode_t omode)
{
        struct stat sb;
        mode_t numask, oumask;
        int first, last, retval;
        char *p;

        p = path;
        oumask = 0;
        retval = 1;
        if (p[0] == '/')                /* Skip leading '/'. */
                ++p;
        for (first = 1, last = 0; !last ; ++p) {
                if (p[0] == '\0')
                        last = 1;
                else if (p[0] != '/')
                        continue;
                *p = '\0';
                if (!last && p[1] == '\0')
                        last = 1;
                if (first) {
                        /*
                         * POSIX 1003.2:
                         * For each dir operand that does not name an existing
                         * directory, effects equivalent to those caused by the
                         * following command shall occcur:
                         *
                         * mkdir -p -m $(umask -S),u+wx $(dirname dir) &&
                         *    mkdir [-m mode] dir
                         *
                         * We change the user's umask and then restore it,
                         * instead of doing chmod's.
                         */
                        oumask = umask(0);
                        numask = oumask & ~(S_IWUSR | S_IXUSR);
                        (void)umask(numask);
                        first = 0;
                }
                if (last)
                        (void)umask(oumask);
                if (mkdir(path, last ? omode : S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
                        if (errno == EEXIST || errno == EISDIR) {
                                if (stat(path, &sb) < 0) {
                                        warn("%s", path);
                                        retval = 0;
                                        break;
                                } else if (!S_ISDIR(sb.st_mode)) {
                                        if (last)
                                                errno = EEXIST;
                                        else
                                                errno = ENOTDIR;
                                        warn("%s", path);
                                        retval = 0;
                                        break;
                                }
                                if (last)
                                        retval = 2;
                        } else {
                                warn("%s", path);
                                retval = 0;
                                break;
                        }
                }
                if (!last)
                    *p = '/';
        }
        if (!first && !last)
                (void)umask(oumask);
        return (retval);
}

