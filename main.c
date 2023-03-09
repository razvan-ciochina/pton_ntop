#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

union internet_addr{
    struct in_addr ipv4_addr;
    struct in6_addr ipv6_addr;
};

#define INADDR_SZ sizeof(struct in_addr)
#define IN6ADDR_SZ sizeof(struct in6_addr)

int main(int argc, char *argv[]) {
    union internet_addr read_addr;
    union internet_addr output_addr;
    int domain_in, domain_out;
    char str[INET6_ADDRSTRLEN];
    int s;
    char *c;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s {i4|i6|<num>} {i4|i6|<num>} string\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    domain_in = (strcmp(argv[1], "i4") == 0) ? AF_INET :
        (strcmp(argv[1], "i6") == 0) ? AF_INET6 : atoi(argv[1]);

    domain_out = (strcmp(argv[2], "i4") == 0) ? AF_INET :
        (strcmp(argv[2], "i6") == 0) ? AF_INET6 : atoi(argv[1]);

    s = inet_pton(domain_in, argv[3], &read_addr);

    printf("Received: %s\n", argv[3]);
    printf("In domain: %s\n", domain_in == AF_INET ? "AF_INET4" : "AF_INET6");
    printf("Out domain: %s\n", domain_out == AF_INET ? "AF_INET4" : "AF_INET6");

    if (s <= 0) {
        if (s == 0)
            fprintf(stderr, "Not in presentation format");
        else
            perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    if (domain_in != domain_out) {
        if (domain_in == AF_INET6 && domain_out == AF_INET) {
            printf("Cannot convert ipv6 to ipv4.\n");
            exit(EXIT_FAILURE);
        }
        printf("Converting IPv4 to IPv6...\n"); 
        memset(output_addr.ipv6_addr.s6_addr, 0, 10);
        output_addr.ipv6_addr.s6_addr[10] = 0xFF;
        output_addr.ipv6_addr.s6_addr[11] = 0xFF;
        memcpy(output_addr.ipv6_addr.s6_addr + 12, &read_addr.ipv4_addr.s_addr, INADDR_SZ);
    } else {
        output_addr = read_addr;
    }

    c = (char *)&output_addr;
    printf("%.2hhx", *c);

    for (; c != ((char *)&output_addr)+16; ++c) {
        printf(":%.2hhx", *c);
    }

    printf("\n");

    if (inet_ntop(domain_out, &output_addr, str, domain_out == AF_INET ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN) == NULL) {
        perror("inet_ntop");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", str);

    exit(EXIT_SUCCESS);
}

