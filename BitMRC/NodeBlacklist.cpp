/*
 * NodeBlacklist.cpp
 *
 *  Created on: 06.05.2016
 *      Author: steady286
 *
 *      Maintain a blacklist of known-bad nodes
 *      These are initialised statically from values, i've found out during operation
 *      of the original python bitmessage daemon.
 *      We shall make them persistent in some database.
 **/
#include <BitMRC.h>
#include <NodeConnection.h>
#include <NodeBlacklist.h>
#include <stdint.h>


typedef struct {
	uint32_t netaddr_version;
	unsigned char addr[40];
} netversion_t;
typedef struct {
	uint32_t netaddr_version;
	unsigned char octets[16];
} netversion_bin_t;

// inet_pton does not seem to be happy with leading zeros
// leaving these vectors for later...
static netversion_t test_vectors[] = {
		{4, "3.224.16.250"},
/*		{4, "03.224.16.250"},
		{4, "003.224.16.250"},*/
		{4, "3.224.16.250"},
/*		{4, "3.224.016.250"},
		{4, "3.224.16.250"},*/
		{4, "13.153.30.131"},
/*		{4, "013.153.30.131"},
		{4, "013.153.030.131"},
		{4, "13.153.030.131"},*/
		{4, "22.109.5.119"},
		{4, "22.143.5.149"},
		{0,""}
};
static netversion_bin_t test_vectors_bin[] = {
		{4,{ 3, 224, 16, 250} },
		{4,{ 13, 153, 30, 131} },
		{4,{ 22, 109, 5, 119} },
		{4,{ 54, 14, 37, 20} },
		/* fixme: ipv6 address tests
		{6, { 0x2a, 0x01, 0x04e, 0xee, 0x20, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 }},
		{6, { 0x2a, 0x01, 0x04e, 0xee, 0x30, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03 }},
		*/
		{ .0 }};
		
static netversion_t ip_blacklist[] = {
		{4, "3.224.16.250"},
		{4, "6.187.21.161"},
		{4, "7.53.20.47"},
		{4, "7.116.20.110"},
		{4, "9.164.26.190"},
		{4, "13.153.30.131"},
		{4, "20.27.7.1"},
		{4, "21.15.6.21"},
		{4, "22.109.5.119"},
		{4, "22.143.5.149"},
		{4, "23.181.4.175"},
		{4, "26.60.9.38"},
		{4, "26.221.9.199"},
		{4, "28.229.15.255"},
		{4, "29.230.14.252"},
		{4, "33.94.50.68"},
		{4, "34.36.49.62"},
		{4, "36.53.55.47"},
		{4, "43.52.56.46"},
		{4, "48.92.35.70"},
		{4, "48.145.35.139"},
		{4, "51.161.32.187"},
		{4, "54.14.37.20"},
		{4, "54.98.37.120"},
		{4, "94.169.77.179"},
		{4, "100.65.242.1"},
		{4, "101.132.118.158"},
		{4, "102.113.117.107"},
		{4, "103.34.116.56"},
		{4, "103.114.116.104"},
		{4, "103.152.116.130"},
		{4, "103.158.116.132"},
		{4, "114.11.97.17"},
		{4, "116.60.103.38"},
		{4, "116.131.103.153"},
		{4, "132.148.151.142"},
		{4, "133.216.150.194"},
		{4, "137.29.154.7"},
		{4, "138.11.153.17"},
		{4, "139.77.152.87"},
		{4, "139.172.152.182"},
		{4, "140.44.159.54"},
		{4, "141.230.158.252"},
		{4, "144.157.131.135"},
		{4, "144.229.131.255"},
		{4, "145.17.130.11"},
		{4, "147.24.128.2"},
		{4, "147.190.128.164"},
		{4, "152.159.139.133"},
		{4, "153.47.138.53"},
		{4, "154.157.137.135"},
		{4, "156.132.143.158"},
		{4, "157.227.142.249"},
		{4, "160.229.179.255"},
		{4, "162.3.177.25"},
		{4, "164.25.183.3"},
		{4, "164.189.183.167"},
		{4, "164.250.183.224"},
		{4, "165.116.182.110"},
		{4, "167.48.180.42"},
		{4, "167.236.180.246"},
		{4, "169.49.186.43"},
		{4, "169.206.186.212"},
		{4, "171.47.184.53"},
		{4, "171.185.184.163"},
		{4, "172.27.148.1"},
		{4, "176.91.163.65"},
		{4, "179.188.160.166"},
		{4, "185.60.170.38"},
		{4, "185.194.170.216"},
		{4, "187.224.168.250"},
		{4, "192.139.211.145"},
		{4, "193.26.210.0"},
		{4, "195.216.208.194"},
		{4, "196.163.215.185"},
		{4, "196.241.215.235"},
		{4, "197.156.214.134"},
		{4, "203.6.216.28"},
		{4, "215.139.196.145"},
		{4, "240.14.227.20"},
		{4, "241.243.226.233"},
		{4, "241.248.226.226"},
		{4, "242.15.225.21"},
		{4, "242.116.225.110"},
		{4, "242.229.225.255"},
		{4, "243.198.224.220"},
		{4, "243.229.224.255"},
		{4, "243.242.224.232"},
		{4, "245.40.230.50"},
		{4, "245.98.230.120"},
		{4, "245.99.230.121"},
		{4, "246.185.229.163"},
		{4, "249.229.234.255"},
		{4, "250.101.233.127"},
		{4, "250.179.233.169"},
		{4, "251.189.232.167"},
		{4, "252.41.239.51"},
		{4, "253.186.238.160"},
		{4, "254.195.237.217"},
		{4, "254.238.237.244"},
		{6, { "2a01:4eee:3000:0000:00c3:0000:0000:0003" }},
		{0, ""}
};
NodeBlacklist::NodeBlacklist() {

}
NodeBlacklist::~NodeBlacklist() {

}

/** \brief check if an entry is blacklisted.
 * \details both listed and input ips are passed through inet_pton, to eliminate
 * the need for normalising the values
 * \return 0 if entry is not blacklisted
 * \return 1 if entry is blacklisted
 * \return <0 if error */
int NodeBlacklist::is_blacklisted(string ip, uint32_t ip_v) {
	netversion_t * bl = ip_blacklist;

	int ret = 0, fret = 0;

	switch(ip_v) {
	case 4: // do ip4 address conversion
		struct in_addr ip4_in, ip4_black;
		while(bl->netaddr_version != 0 && ret>=0) {
			ret = inet_pton(AF_INET, ip.c_str(), (void*) &ip4_in);
			if (ret >0) {
				ret = inet_pton(AF_INET, (char*)bl->addr, (void*) &ip4_black);
				if(ret >0) {
					if(memcmp(&ip4_in, &ip4_black, sizeof(in_addr)) == 0) {
						fret = 1;
					}
				} 
			}
			bl++;
		}
		break;
	case 6:
		struct in6_addr ip6_in, ip6_black;
		break;
	}
	return fret;
}
/** \brief overloaded function for convenience
 *   */
int NodeBlacklist::is_blacklisted(struct addrinfo * ai, int family) {
	netversion_t * bl = ip_blacklist;

	int ret = 0, fret = 0;
	string ip;
	struct in_addr ip4_in, ip4_black;
	struct addrinfo ai_black;
	uint32_t count=0;
	
	switch(family) {
	case AF_INET: // do ip4 address conversion
		struct in_addr ip4_in, ip4_black;
		while(bl->netaddr_version != 0 && ret>=0) {
			//printf("%s : check %s\n", __func__, bl->addr);
			ret = inet_pton(family, bl->addr, (void*) &ip4_black);
			if(ret >0) {
				count++;
				if(memcmp(ai->ai_addr, &ip4_black, ai->ai_addrlen) == 0) {
					fret = 1;
					break;
				}
			} else {
				printf("%s : conversion failed\n", __func__);
			}
		bl++;
		}
		break;
	case AF_INET6:
		struct in6_addr ip6_in, ip6_black;
		break;
	}
	printf("bintest:checkd %u elements\n",count);
	return fret;
}
/** \brief add a blacklist entry
 * \return 0 if everything was ok
 * \return <0 if error
 */
int NodeBlacklist::add_blacklist_entry(std::string ip, uint32_t ip_v) {
	// \todo: implement
	return 0;
}
/** \brief test the binary address blacklist discovery
 * */
int NodeBlacklist::test_binaddr() {
	int ret = 0, fret = 1, family = 0;
	uint32_t testcount = 0;
	netversion_bin_t * tests = test_vectors_bin;
	struct addrinfo ai;
	unsigned char sabytes[20];
	ai.ai_addr = (struct sockaddr*)sabytes; /* \fixme: ipv6 */
	
	ai.ai_family = AF_INET;
	while(tests->netaddr_version !=0) {
		testcount++;
		switch(tests->netaddr_version) {
			case 4:	ai.ai_addrlen = 4; family = AF_INET; break;
			case 6: ai.ai_addrlen = 16; family = AF_INET6; break;
			default: ai.ai_addrlen = 0;
		}

		memcpy(ai.ai_addr, tests->octets, ai.ai_addrlen);
		if ( 0 == is_blacklisted(&ai, family) ) {
			fret = 0;
			printf("%s element not found: %u\n", __func__, testcount);
		}
		tests++;
	}	
	return fret;
}
/** \brief run the blacklist discovery on the testvectors
 * \return 0 if ok
 */
int NodeBlacklist::test() {

	int ret = 0;
	netversion_t * tests = test_vectors;
	std::string test_ip;
	while(tests->netaddr_version !=0) {
		if ( is_blacklisted(test_ip.assign((char*)tests->addr), tests->netaddr_version) <= 0) {
			printf("Error for address %s\n",tests->addr);
			ret = 1;
		} else {
			printf("Address detected ok: %s\n",tests->addr);
		}
		tests++;
	}
#if 0
else {
	inet_ntop(AF_INET, &ip4_black.s_addr, str, INET6_ADDRSTRLEN);
	printf("ERR: BLACKLIST: %s ", str);
	inet_ntop(AF_INET, &ip4_in.s_addr, str, INET6_ADDRSTRLEN);
	printf(" IN: %s \n",str);
}
#endif
	return ret;
}
