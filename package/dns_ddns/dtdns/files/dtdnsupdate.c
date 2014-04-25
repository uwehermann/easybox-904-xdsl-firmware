/*
** dtdnsupdate - dtdns.com updater
** Copyright (c) 2002 Christopher Trahan
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define NAME        "dtdnsupdate"
#define VERSION     "0.1.0"
#define COPYRIGHT   "Copyright (c) 2002 Christopher Trahan."

#define WWW_PORT    80
#define SERVER      "www.dtdns.com"
#define SCRIPT      "/api/autodns.cfm"

char username[64];
char password[64];
char cust_ip[16];
int  verbose = 0;

void show_help(void) {
    printf("%s %s dtdns.com updater - %s\n", NAME, VERSION, COPYRIGHT);
    printf("\t-u,--username\tset username (ie - theedge.darktech.org)**\n");
    printf("\t-p,--password\tset password**\n");
    printf("\t-i,--ip\t\t\tset ip address (use 0.0.0.0 for offline mode)\n");
    printf("\t-v,--verbose\tverbose operation\n");
    printf("\t-h,--help\t\tthis help\n\n");
	printf("Example:\t dtdnsupdate -u someone.dtdns.net -p 123456\n");
}

/* parse the command line */
int parse_cmdline(int argc, char *argv[]) {
    int i = 1;

    argc--;
    strcpy(cust_ip, "(auto)"); /* This is set to get your IP by the server */

    while (i <= argc) {
        if ((strcmp(argv[i], "-u") == 0) || (strcmp(argv[i], "--username") == 0)) {
            if (argc >= i + 1) {
                strcpy(username, argv[++i]); /* grab data from next arg and */
                i++;                         /* move ahead to the next arg */
            } else return 0;
        }
        else if ((strcmp(argv[i], "-p") == 0) || (strcmp(argv[i], "--password") == 0)) {
            if (argc >= i + 1) {
                strcpy(password, argv[++i]);
                i++;
            } else return 0;
        }
        else if ((strcmp(argv[i], "-i") == 0) || (strcmp(argv[i], "--ip") == 0)) {
            if (argc >= i + 1) {
                strcpy(cust_ip, argv[++i]);
                i++;
            } else return 0;
        }
        else if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--verbose") == 0)) {
            verbose = 1;  /* enable verbose processing */
            i++;
        }
        else if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
            show_help();
            exit(0);
        }
        else i++; /* unrecognized, check next arg */
    }

    if ((strcmp(username, "") == 0) || (strcmp(password, "") == 0)) {
        fprintf(stderr, "\n\033[1m[ \033[5m\033[31mERROR \033[0m\033[1m] Please supply an username and a password.\33[0m\n\n");
        return 0;
    }

    return 1;
}

/* connect to dtdns.net and do the dns update */
int do_update(void) {
    FILE *rfile;
    char send_buffer[256], temp[1024];
    int sock;

    struct sockaddr_in addr;
    struct hostent *hostinfo;

    /* build http request */
    if (strcmp(cust_ip, "(auto)") != 0) {
        sprintf(send_buffer, "GET %s?id=%s&pw=%s&ip=%s&client=%s HTTP/1.1\nHost: %s\nUser-Agent: %s %s\n\n",
                SCRIPT, username, password, cust_ip, NAME, SERVER, NAME, VERSION);
    } else {
        sprintf(send_buffer, "GET %s?id=%s&pw=%s&client=%s HTTP/1.1\nHost: %s\nUser-Agent: %s %s\n\n",
                SCRIPT, username, password, NAME, SERVER, NAME, VERSION);
    }

    if (verbose) printf("\033[31m[\033[0m\033[1m Sent Information: \033[0m\033[31m]\033[0m\n%s", send_buffer);

    if (!(hostinfo = gethostbyname(SERVER))) {
        fprintf(stderr, "\n\033[1m[ \033[5m\033[31mERROR \033[0m\033[1m] Unable to resolve host - %s\033[0m\n", SERVER);
        return 0;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        fprintf(stderr, "\n\033[1m[ \033[5m\033[31mERROR \033[0m\033[1m] Unable to get network socket\033[0m\n");
        return 0;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(WWW_PORT);
    addr.sin_addr = *(struct in_addr *)*hostinfo->h_addr_list;

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        fprintf(stderr, "\n\033[1m[ \033[5m\033[31mERROR \033[0m\033[1m] Unable to connect\033[0m\n");
        return 0;
    }

    /* send the request to server */
    write(sock, send_buffer, strlen(send_buffer));


    if (verbose) printf("\033[31m[\033[0m\033[1m Request sent... waiting for response \033[0m\033[31m]\033[0m\n\n");

    /* get server reply */
    rfile = fdopen(sock, "r");

    while (!feof(rfile)) {
        fgets(temp, (int) sizeof(temp), (FILE *) rfile);
        if (verbose) printf("%s", temp); /* print info if verbose mode */
    }
    if (verbose) printf("\n");

    /* temp should contain last thing sent (status message) */
    /* check to see if it contains anything unexpected */
    /* "now points to" should only appear in success status message */
    if (!strstr(temp, "now points to")) {
        fprintf(stderr, "\033[1m[ \033[5m\033[31mERROR \033[0m\033[1m] %s\033[0m\n", temp);
        exit(1);
    }

    close(sock);
    return 1;
}

int main(int argc, char *argv[]) {
    /* get command line info */
    if (!parse_cmdline(argc, argv)) {
        fprintf(stderr, "							Usage is as follows **required\n\n");
        show_help();
        return 1;
    }

    /* display operating parameters */
    if (verbose) {
        printf("%s %s - %s\n\n", NAME, VERSION, COPYRIGHT);
        printf("\033[1m[\033[0m username \033[1m]\033[0m %s\n\033[1m[\033[0m password \033[1m]\033[0m %s\n\033[1m[\033[0m ip       \033[1m]\033[0m %s\n\n", username,
               password, cust_ip);
    }

    /* do the update */
    if (!do_update()) {
        fprintf(stderr, "\033[1m[\033[0m\033[31m Notice \033[0m\033[1m]\033[0m Error performing update.\n");
        return 1;
    }

    return 0;
}
