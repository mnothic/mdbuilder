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
#include "parser.h"
#include "util.h"

char *config_path = "mdbuilder.conf";

int main(int argc, char **argv)
{
    pid_t md_pid;
    int ch,dflag = 0;

    while ((ch = getopt(argc,argv,"vihdc:")) != -1) {
        switch (ch) {
            case 'v':
                printf("[%s] Version %.2f\n",PACKAGE,VERSION);
                exit(0);
            break;

            case 'i':
                info();
            break;

            case 'h':
                usage(argv[0]);
            break;

            case 'd':
                dflag = 1;
            break;

            case 'c':
                config_path = optarg;
            break;

            default:
                usage(argv[0]);
        }
    }
    argc -= optind;
    argv += optind;

    if (file_exist(config_path) == 0)
       create_cfg(config_path);

    if (parse_file(config_path) == 1) {
        perror("ERROR: config invalid parameter\n");
        mdb_write_log(LOG_ALERT,"ERROR: config invalid parameter");
        exit(2);
    }

    if (dflag == 1) {
        md_pid = fork();
        if (md_pid > 0) {
            if (set_pid_file(pidfile_h,md_pid) != 0) {
                mdb_write_log(LOG_ALERT,"ERROR: Can't write pidfile");
                exit(-1);
            }
            chdir(basehome_h);
            exit(0);
        }
        else if (md_pid < 0){
            perror("ERROR: Can't Create child proces\n");
            mdb_write_log(LOG_ALERT,"ERROR: Can't Create child proces");
            exit(1);
        }

        signal(SIGCHLD,SIG_IGN); /* ignore child */
        signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
        signal(SIGTTOU,SIG_IGN);
        signal(SIGTTIN,SIG_IGN);
        signal(SIGHUP,signal_listener); /* catch hangup signal */
        signal(SIGTERM,signal_listener); /* catch kill signal */
    }

    if (dbconnect(dsn_h) == -1) {
        printf("ERROR: connection failed\n");
        mdb_write_log(LOG_ALERT,"ERROR: connection failed");
        exit(3);
    }

    for(;;sleep(1)) {

        if (find_accounts() == -1) {
            perror("ERROR: find new acounts failed\n");
            mdb_write_log(LOG_ALERT,"ERROR: find new acounts failed");
        }
        if (seek_garbage() == -1) {
            perror("ERROR: find new acounts failed\n");
            mdb_write_log(LOG_ALERT,"ERROR: find new acounts failed");
        }
    }

    PQfinish(c);
exit(0);

}

void usage(char *bind)
{
    printf("%s [-hdivc] [file config]\n",bind);
    exit(0);
}
