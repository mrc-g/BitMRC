
#ifdef LINUX
#include <string.h> /* memset */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>

#endif

int main(int argc, char ** argv) {

	int dns = 2;

	addrinfo *result = NULL, hints;
	sockaddr_in  *sockaddr_ipv4;

	memset(&hints, 0, sizeof(hints));
	
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	getaddrinfo("bootstrap8444.bitmessage.org", "0", &hints, &result);

	for (addrinfo *ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		if (ptr->ai_family == AF_INET) {
			sockaddr_ipv4 = (sockaddr_in *)ptr->ai_addr;
			printf( "8444 %s\n",inet_ntoa(sockaddr_ipv4->sin_addr));
		}
	}
	freeaddrinfo(result);
	getaddrinfo("bootstrap8080.bitmessage.org", "0", &hints, &result);

	for (addrinfo *ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		if (ptr->ai_family == AF_INET) {
			sockaddr_ipv4 = (sockaddr_in *)ptr->ai_addr;
			printf( "8080 %s\n",inet_ntoa(sockaddr_ipv4->sin_addr));
		}
	}
	freeaddrinfo(result);	
	return 0;
}
