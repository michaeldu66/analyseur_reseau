//https://docs.huihoo.com/doxygen/linux/kernel/3.7/structudphdr.html
#ifndef PORT_H
#define PORT_H
#include <netinet/tcp.h>
#include <stdio.h>
#include "analyse_bootp.h"
#include "analyse_dns.h"
#include "analyse_http.h"

#define PROT_ECHO 7
#define PROT_DNS 53
#define PROT_BOOTP1 67
#define PROT_BOOTP2 68



#define PROT_FTP 21
#define PROT_SSH 22
#define PROT_TELNET 23
#define PROT_SMTP 587
#define PROT_HTTP 80
#define PROT_POP 110
#define PROT_IMAP 143
#define PROT_HTTPS 443




void process_port(int src, int dst, const u_char* packet);
#endif