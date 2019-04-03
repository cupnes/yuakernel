#include <lib.h>
#include <local_conf.h>

#define PACKET_BUF_SIZE	1024

#define SRC_PORT	51268
#define HTTP_PORT	80

#define FRAME_TYPE_IP	0x0800

#define IP_HEADER_PROTOCOL_TCP	0x06

/* FIXME: network byte order */
struct __attribute__((packed)) ethrenet_header {
	unsigned char dst_mac[6];
	unsigned char src_mac[6];
	unsigned short type;
};

/* FIXME: network byte order */
struct __attribute__((packed)) ip_header {
	unsigned char version:4;
	unsigned char ihl:4;
	unsigned char service_type;
	unsigned short total_length;
	unsigned short identification;
	unsigned short flags_rsv:1;
	unsigned short flags_df:1;
	unsigned short flags_mf:1;
	unsigned short fragment_offset:13;
	unsigned char ttl;
	unsigned char protocol;
	unsigned short header_checksum;
	unsigned char src_ip[4];
	unsigned char dst_ip[4];
};

struct tcp_session {
	unsigned char dst_ip[6];
};

unsigned char default_gw_mac[6] = {
	DEFAULT_GW_MAC_0, DEFAULT_GW_MAC_1, DEFAULT_GW_MAC_2,
	DEFAULT_GW_MAC_3, DEFAULT_GW_MAC_4, DEFAULT_GW_MAC_5};
unsigned char cupnes_com_ip[4] = {49, 212, 139, 172};

unsigned char own_mac[6];
unsigned char packet_buf[PACKET_BUF_SIZE];
struct tcp_session session_buf;

struct tcp_session *connect(unsigned char dst_ip[], unsigned short src_port,
			    unsigned short dst_port);
void http_test(struct tcp_session *session);
void disconnect(struct tcp_session *session);

int main(void)
{
	set_fg(255, 255, 255);
	set_bg(0, 70, 250);
	clear_screen();

	get_mac(own_mac);

	struct tcp_session *session;

	session = connect(cupnes_com_ip, SRC_PORT, HTTP_PORT);

	/* http_test(session); */

	/* disconnect(session); */

	return 0;
}

struct tcp_session *connect(unsigned char dst_ip[] __attribute__((unused)),
			    unsigned short src_port __attribute__((unused)),
			    unsigned short dst_port __attribute__((unused)))
{
	/* 3ウェイハンドシェイクを確立する */
	unsigned long long buf_base = (unsigned long long)packet_buf;
	struct ethrenet_header *eth_h;
	struct ip_header *ip_h;

	unsigned int i;
	unsigned char *p;

	/* SYN */
	eth_h = (struct ethrenet_header *)buf_base;
	memcpy(eth_h->dst_mac, default_gw_mac, 6);
	memcpy(eth_h->src_mac, own_mac, 6);
	eth_h->type = FRAME_TYPE_IP;

	p = (unsigned char *)eth_h;
	for (i = 0; i < sizeof(struct ethrenet_header); i++) {
		puth(*p++, 2);
		putchar(' ');
	}

	ip_h = (struct ip_header *)(packet_buf + sizeof(struct ethrenet_header));
	ip_h->version = 4;
	ip_h->ihl = 5;
	ip_h->service_type = 0x00;
	ip_h->total_length = 60;
	ip_h->identification = 0xb761;
	ip_h->flags_rsv = 0;
	ip_h->flags_df = 1;
	ip_h->flags_mf = 0;
	ip_h->fragment_offset = 0;
	ip_h->ttl = 64;
	ip_h->protocol = IP_HEADER_PROTOCOL_TCP;
	ip_h->header_checksum = 0x5da3;

	return &session_buf;
}

void http_test(struct tcp_session *session __attribute__((unused)))
{
	return;
}

void disconnect(struct tcp_session *session __attribute__((unused)))
{
	return;
}
