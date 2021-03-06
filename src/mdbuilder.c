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
#include "mdbuilder.h"

void info()
{
    printf("[*] [mdbuilder] Version %1.f",VERSION);
    printf("\n[*] it's a requeriment of ADEM Systems.");
    printf("\n[*] Developed By J.Medina [AT] eNgine Systems.");
    printf("\n[*] BUILD UNDER BSD LICENCE.\n");
}

int dbconnect(char *constr)
{
    if ((c = PQconnectdb(constr)) == NULL) return -1;   /* connect to the database */

    if (PQstatus(c) == CONNECTION_BAD)                  /* did the database connection fail? */
    {
        mdb_write_log(LOG_ALERT,"Connection to database failed.\n");
        return -1;
    }
    mdb_write_log(LOG_ALERT,"Connection success to database\n");
    return 0;
}

int find_accounts()
{
    int i,e,j,len;
    PGresult *res;
    char *pth = (char *)malloc(sizeof(char));
    char *tmp = (char *)malloc(sizeof(char));
    char *ap = pth = NULL;
    char *at = tmp = NULL;
    char *sql_update = NULL;

    if (PQstatus(c) != CONNECTION_OK) {
        if (dbconnect(dsn_h) == -1) {
            mdb_write_log(LOG_ALERT,"Database connection failed\n");
            mdb_free(pth);
            mdb_free(tmp);
            return -1;
        }
        else {
            mdb_write_log(LOG_ALERT,"Reconnected to database\n");
        }
    }
    /* execute query and return the recordset */
    res = PQexec(c, getquery_h);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        perror("query SELECT failed.\n");
        mdb_free(pth);
        mdb_free(tmp);
        PQclear(res);
        return -1;
    }

    for (i = 0; i < PQntuples(res); i++) {               /* loop through all rows returned */
        len =  strlen(PQgetvalue(res, i, 1))+2;
        len += strlen(basehome_h);
        pth = (char *)realloc(pth,len);
        tmp = (char *)realloc(tmp,len+5);
        if (pth == NULL || tmp == NULL) {
            err(134,"out of memory, impossible pth alloc");
            mdb_free(ap);
            mdb_free(at);
            PQclear(res);
            return -1;
        }
        else {
            ap = pth;
            at = tmp;
        }
        snprintf(pth,len,"%s/%s",basehome_h,PQgetvalue(res, i, 1));

        if ((e = build(pth,S_IRWXU | S_IRWXG)) != 0) {
            if (atoi(PQgetvalue(res,i,2)) == 3)
                sql_update = mdb_sql(update_a_h,PQgetvalue(res,i,0));
            else
                sql_update = mdb_sql(update_b_h,PQgetvalue(res,i,0));
            if (sql_update == NULL)
                continue;
            if ( PQresultStatus(PQexec(c,sql_update)) != PGRES_COMMAND_OK ) {
                fprintf(stderr,"Failed Update user: %s",PQgetvalue(res, i, 1));
            }
            mdb_free(sql_update);
        }
        else {
            printf("Error: at build directory : %d\n", e);
            mdb_free(pth);
            PQclear(res);
            return -1;
        }
        len += 4;
        for (j=0;j<3;j++) {
            memset(tmp,0,strlen(tmp));
            snprintf(tmp,len,"%s/%s",pth,maildir[j]);
            build(tmp,S_IRWXU | S_IRWXG);
        }

    }
    mdb_free(tmp);
    mdb_free(pth);
    PQclear(res);
    return 0;
}

int seek_garbage()
{
    int i,len,nrow;
    char *user,*domain,*p;
    char *delsql;
    PGresult *res;
    char *pth = (char *)malloc(sizeof(char));
    char *ap = pth;

    if (PQstatus(c) != CONNECTION_OK) {
        if (dbconnect(dsn_h) == -1) {
            printf("Database connection failed\n");
            free(pth);
            return -1;
        }
        else {
            mdb_write_log(LOG_ALERT,"Reconnected to database\n");
        }
    }
    /* execute query and return the recordset */
    res = PQexec(c, rmquery_h);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)       /* did the query fail? */
    {
        perror("query SELECT failed.\n");
        free(pth);
        PQclear(res);
        return -1;
    }
    nrow = PQntuples(res);
    for (i = 0; i < nrow; i++) {               /* loop through all rows returned */
        memset(pth,0,strlen(pth));
        len  = strlen(basehome_h)+3;
        len += strlen(PQgetvalue(res, i, 1));

        pth = (char *)realloc(pth,len);
        p = strtok(PQgetvalue(res, i, 1),"@");
        user = p;
        while (p != NULL) {
            p = strtok(NULL,"\n");
            if (p != NULL)
                domain = p;
        }
        if (pth == NULL) {
            err(134,"out of memory, not possible pth alloc");
            free(ap);
            PQclear(res);
            return -1;
        }
        else
            ap = pth;
        snprintf(pth,len,"%s/%s/%s", basehome_h, domain, user);
        delsql = mdb_sql(delsql_h,PQgetvalue(res,i,0));
        if (delsql == NULL)
            continue;

        if (rm(pth) == 0) {
            if ( PQresultStatus(PQexec(c,delsql)) != PGRES_COMMAND_OK ) {
                    fprintf(stderr,"SQL Fail: %s\n",delsql);
                    mdb_write_log(LOG_ALERT,delsql);
            }
        }
        mdb_free(delsql);
    }
    free(pth);
    PQclear(res);
    return 0;
}

void mdb_write_log(int log_priority, char *msg )
{
	openlog(PACKAGE,LOG_PID,LOG_DAEMON);
    syslog(log_priority,msg);
	closelog();
}

/**
* CALLBACK function signal_listener
*
* listen and process Signals
*
* @param int signal
* @return void
*/
void signal_listener(int signal)
{
	switch(signal) {
        case SIGHUP:
            mdb_write_log(LOG_ALERT,"SIGHUP recived");
            break;
        case SIGTERM:
            mdb_write_log(LOG_ALERT,"SIGTERM recived");
            unlink(pidfile_h);
            exit(0);
            break;
	}
}
