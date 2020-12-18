#include "../include/analyse_dns.h"

void print_operation_type(uint16_t opcode)
{
    switch (opcode)
    {
    case DNS_OPCODE_QUERY:
        printf("Query - ");
        break;

    case DNS_OPCODE_IQUERY:
        printf("Inverse query - ");
        break;

    case DNS_OPCODE_STATUS:
        printf("Status request - ");
        break;

    case DNS_OPCODE_NOTIFY:
        printf("Notify - ");
        break;

    case DNS_OPCODE_UPDATE:
        printf("Update - ");
        break;
    }
    printf("\n");
}

void print_dns_short(const u_char *packet, int overTCP)
{
    struct dnshdr *dns_hd;
    if (packet == NULL)
        return;

    // si on a du DNS-over-TCP, on ignore les 2 premiers octets
    if (overTCP)
        packet += 2;

    dns_hd = (struct dnshdr *)packet;
    printf("DNS: \t\t");

    // affichage type d'opération
    print_operation_type(dns_hd->opcode);
}



void print_dns_full(const u_char *packet, int overTCP)
{
    struct dnshdr *dns_hd;
    short *size, *type;
    char lg;
    char *bakptr;
    if (packet == NULL)
        return;

    printf("DNS\n");

    // Si over TCP, les 2 premiers octets représentent la taille
    if (overTCP)
    {
        size = (short *)packet;
        packet += 2;
        printf("\tLength: %d\n", ntohs(*size));
    }

    dns_hd = (struct dnshdr *)packet;
    bakptr = (char*)packet;

    // identifiant
    printf("\tTransaction ID: 0x%x", ntohs(dns_hd->id));
    printf("\n");

    // QR (query or reply)
    printf("\tQR: %d : ", dns_hd->qr);
    if (dns_hd->qr == 0)
        printf("query\n");
    else
        printf("response\n");

    // affichage type d'opération
    printf("\tOpcode: %d : ", dns_hd->opcode);
    print_operation_type(dns_hd->opcode);

    // AA (Authoritative Answer)
    printf("\tAA: %d", ntohs(dns_hd->aa));
    printf("\n");

    // TC (TrunCation)
    printf("\tTruncated: %d", dns_hd->tc);
    printf("\n");

    // RD (Recursion Desired)
    printf("\tRecursion desired: %d", dns_hd->rd);
    printf("\n");

    // RA (Recursion Available)
    printf("\tRecursion available: %d", dns_hd->ra);
    printf("\n");

    // Z (Reserved for future use)
    printf("\tZ (future use): %d", ntohs(dns_hd->zero));
    printf("\n");

    // AA (Answer Authentification)
    printf("\tAnswer Authentification: %d", dns_hd->aa);
    printf("\n");

    // RCODE (Response code)
    printf("\tRCODE: %d", ntohs(dns_hd->rcode));
    if (dns_hd->qr == 1)
    {
        if (ntohs(dns_hd->rcode) == 0)
            printf(" : no error condition");
        else
            printf(" : error");
    }
    printf("\n");

    // QDCOUNT
    printf("\tQuestions: %d", ntohs(dns_hd->qcount));
    printf("\n");

    // ANCOUNT
    printf("\tAnswer RRs: %d", ntohs(dns_hd->ancount));
    printf("\n");

    // NSCOUNT
    printf("\tAuthority RRs: %d", ntohs(dns_hd->nscount));
    printf("\n");

    // ADCOUNT
    printf("\tAdditional RRs: %d", ntohs(dns_hd->adcount));
    printf("\n");

    // on saute le header dans packet
    packet += sizeof(struct dnshdr); //DNS_dns_hd_SIZE;

    // Queries
    if (ntohs(dns_hd->qcount) > 0)
    {
        printf("\tQueries:");
        for (int i = 0; i < ntohs(dns_hd->qcount); i++)
        {
            printf("\n\t\t > ");
            // parser tous les labels du nom de domaine
            while (packet[0] != 0)
            {
                lg = packet[0];
                packet++;
                // affichage du label octet par octet
                for (int j = 0; j < lg; j++)
                {
                    printf("%c", packet[0]);
                    packet++;
                }
                printf(".");
            }

            // ignorer le caractère NULL qui marque la fin du DN
            packet++;

            // affichage des informations sur le DN
            printf("\n");
            type = (short *)packet;
            printf("\t\t\t  query type: %d\n", ntohs(*type));
            packet += 2;
            type = (short *)packet;
            printf("\t\t\t  query class: %d\n", ntohs(*type));
            packet += 2;
        }
    }

    // Answers
    printf("\tAnswers:");
    printf("\n\t\t > ");

    for (int i = 0; i < dns_hd->adcount + dns_hd->ancount; i++)
    {
        // checker si pointeur vers DN = les deux premiers bits à 1
        if ((u_char)packet[0] >= 192)
        {
            printf ("/!\\pointer");
            packet += 2;
        }
        else
        {
            while (packet[0] != 0)
            {
                lg = packet[0];
                packet++;
                // affichage du label octet par octet
                for (int j = 0; j < lg; j++)
                {
                    printf("%c", packet[0]);
                    packet++;
                }
                printf(".");
            }
            if(bakptr) {}
        }
        
        // ignorer le caractère NULL qui marque la fin du DN
        packet++;

        // Parser type
        type = (short *)packet;
        printf("\n\t\t > Type: %d", ntohs(*type));
        packet += 2;
        
        return; // on arrete là, car pas eu le temps de finir

        // parser class / TL / RDLength et RDATA pas encore fait
        packet += 2;
        packet += 8;
        packet += 4;
        packet += 4; // peut être variable

    }

    printf("\n");
}


void dns_packet(const u_char * packet){
    if(verbosity == 3)
        print_dns_full(packet, 0);
    else if (verbosity == 2)
        print_dns_short(packet, 0);
    else if (verbosity==1)
        printf("DNS Protocol, improve verbosity for details");
}