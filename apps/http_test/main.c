#include <lib.h>
#include <local_conf.h>

#define PACKET_BUF_SIZE	1024

#define SRC_PORT	51268
#define HTTP_PORT	80

#define FRAME_TYPE_IP	0x0800

#define IP_HEADER_PROTOCOL_TCP	0x06
#define IP_HEADER_FLAGS_DF	0x4000
#define IP_HEADER_FLAGS_MF	0x2000

#define TCP_HEADER_LEN_SHIFT	12
#define TCP_FLAGS_SYN	0x0002
#define TCP_FLAGS_PSH	0x0008
#define TCP_FLAGS_ACK	0x0010

struct __attribute__((packed)) ethernet_header {
	unsigned char dst_mac[6];
	unsigned char src_mac[6];
	unsigned short type;
};

struct __attribute__((packed)) ip_header {
	unsigned char ihl:4;
	unsigned char version:4;
	unsigned char service_type;
	unsigned short total_length;
	unsigned short identification;
	unsigned short fragment_offset;
	unsigned char ttl;
	unsigned char protocol;
	unsigned short header_checksum;
	unsigned char src_ip[4];
	unsigned char dst_ip[4];
};

struct __attribute__((packed)) tcp_header {
	unsigned short src_port;
	unsigned short dst_port;
	unsigned int seq_num;
	unsigned int ack_num;
	unsigned short header_length_flags;
	unsigned short window_size;
	unsigned short check_sum;
	unsigned short urgent_pointer;
};

struct __attribute__((packed)) tcp_header_options_a {
	struct __attribute__((packed)) {
		unsigned char kind;
		unsigned char length;
		unsigned short mss_value;
	} max_seg_size;
	struct __attribute__((packed)) {
		unsigned char kind;
		unsigned char length;
	} tcp_sack_permit;
	struct __attribute__((packed)) {
		unsigned char kind;
		unsigned char length;
		unsigned int timestamp_value;
		unsigned int timestamp_echo_replay;
	} timestamp;
	unsigned char nop;
	struct __attribute__((packed)) {
		unsigned char kind;
		unsigned char length;
		unsigned char shift_count;
	} window_scale;
};

struct __attribute__((packed)) tcp_header_options_b {
	unsigned char nop_1;
	unsigned char nop_2;
	struct __attribute__((packed)) {
		unsigned char kind;
		unsigned char length;
		unsigned int timestamp_value;
		unsigned int timestamp_echo_replay;
	} timestamp;
};

struct tcp_session {
	unsigned char dst_mac[6];
	unsigned char dst_ip[4];
	unsigned short id;
	unsigned short src_port;
	unsigned short dst_port;
	unsigned int seq_num;
	unsigned int ack_num;
};

unsigned char own_ip[4] = {LOCAL_IP_0, LOCAL_IP_1, LOCAL_IP_2, LOCAL_IP_3};
unsigned char default_gw_mac[6] = {
	DEFAULT_GW_MAC_0, DEFAULT_GW_MAC_1, DEFAULT_GW_MAC_2,
	DEFAULT_GW_MAC_3, DEFAULT_GW_MAC_4, DEFAULT_GW_MAC_5};
/* unsigned char cupnes_com_ip[4] = {49, 212, 139, 172}; */
/* 0x31, 0xd4, 0x8b, 0xac */
unsigned char cupnes_com_ip[4] = {192, 168, 10, 6};
char http_request[] = "GET /\r\n";

unsigned char own_mac[6];
unsigned char send_buf[PACKET_BUF_SIZE];
unsigned char recv_buf[PACKET_BUF_SIZE];
struct tcp_session session_buf;

unsigned short swap_byte_2(unsigned short data);
unsigned int swap_byte_4(unsigned int data);

unsigned short get_ip_checksum(struct ip_header *ip_h);
unsigned short get_tcp_checksum(struct tcp_header *tcp_h, struct tcp_session *session);

struct tcp_session *connect(
	unsigned char dst_mac[], unsigned char dst_ip[],
	unsigned short src_port, unsigned short dst_port);
void http_get(struct tcp_session *session);
void http_rcv(struct tcp_session *session);
void disconnect(struct tcp_session *session);

int main(void)
{
	set_fg(255, 255, 255);
	set_bg(0, 70, 250);
	clear_screen();

	get_mac(own_mac);

	struct tcp_session *session;

	session = connect(default_gw_mac, cupnes_com_ip, SRC_PORT, HTTP_PORT);

	http_get(session);
	http_rcv(session);

	puts("HTTP RCVED\r\n");

	/* disconnect(session); */

	return 0;
}

unsigned short swap_byte_2(unsigned short data)
{
	return (data << 8) | (data >> 8);
}

unsigned int swap_byte_4(unsigned int data)
{
	return (data << 24) | ((data << 8) & 0x00ff0000)
		| ((data >> 8) & 0x0000ff00) | (data >> 24);
}

unsigned short get_ip_checksum(struct ip_header *ip_h)
{
	unsigned short *p = (unsigned short *)ip_h;
	unsigned int i;
	unsigned int sum = 0;

	for (i = 0; i < (sizeof(struct ip_header) / 2); i++)
		sum += swap_byte_2(*p++);

	unsigned int carry = sum >> 16;
	sum &= 0x0000ffff;

	sum += carry;

	unsigned short t = sum;
	return ~t;
}

unsigned short get_tcp_checksum(struct tcp_header *tcp_h,
				struct tcp_session *session)
{
	union pseudo_tcp_packet {
		unsigned short raw[6];
		struct __attribute__((packed)) {
			unsigned char src_ip[4];
			unsigned char dst_ip[4];
			unsigned char zero;
			unsigned char protocol;
			unsigned short tcp_len;
		};
	} ppacket;

	unsigned int i;
	memcpy(ppacket.src_ip, own_ip, 4);
	memcpy(ppacket.dst_ip, session->dst_ip, 4);
	ppacket.zero = 0;
	ppacket.protocol = IP_HEADER_PROTOCOL_TCP;
	ppacket.tcp_len = swap_byte_2(sizeof(struct tcp_header));

	unsigned int sum = 0;

	for (i = 0; i < 6; i++)
		sum += swap_byte_2(ppacket.raw[i]);

	unsigned short *p = (unsigned short *)tcp_h;

	for (i = 0; i < (sizeof(struct tcp_header) / 2); i++)
		sum += swap_byte_2(*p++);

	unsigned int carry = sum >> 16;
	sum &= 0x0000ffff;

	sum += carry;

	unsigned short t = sum;
	return ~t;
}

struct tcp_session *connect_init(
	unsigned char dst_mac[], unsigned char dst_ip[],
	unsigned short src_port, unsigned short dst_port)
{
	struct datetime dt;
	get_datetime(&dt);

	unsigned char i;

	for (i = 0; i < 6; i++)
		session_buf.dst_mac[i] = dst_mac[i];
	for (i = 0; i < 4; i++)
		session_buf.dst_ip[i] = dst_ip[i];
	session_buf.id = (unsigned short)dt.min << 8 | dt.sec;
	session_buf.src_port = src_port;
	session_buf.dst_port = dst_port;
	session_buf.seq_num =
		(unsigned int)dt.week << 24 | (unsigned int)dt.hour << 16
		| (unsigned int)dt.min << 8 | dt.sec;
	session_buf.ack_num = 0;

	return &session_buf;
}

void connect_syn(struct tcp_session *session)
{
	unsigned long long base_addr;
	struct ethernet_header *eth_h;
	struct ip_header *ip_h;
	struct tcp_header *tcp_h;
	/* struct tcp_header_options_a *tcp_opt_h; */

	unsigned int i;
	unsigned char *p;

	/* frame */
	base_addr = (unsigned long long)send_buf;
	eth_h = (struct ethernet_header *)base_addr;
	memcpy(eth_h->dst_mac, session->dst_mac, 6);
	memcpy(eth_h->src_mac, own_mac, 6);
	eth_h->type = swap_byte_2(FRAME_TYPE_IP);

	/* p = (unsigned char *)eth_h; */
	/* for (i = 0; i < sizeof(struct ethernet_header); i++) { */
	/* 	puth(*p++, 2); */
	/* 	putchar(' '); */
	/* } */
	/* puts("\r\n"); */

	/* ip */
	base_addr += sizeof(struct ethernet_header);
	ip_h = (struct ip_header *)base_addr;
	ip_h->version = 4;
	ip_h->ihl = 5;
	ip_h->service_type = 0x00;
	ip_h->total_length = swap_byte_2(sizeof(struct ip_header) + sizeof(struct tcp_header));
	ip_h->identification = swap_byte_2(session->id);
	/* ip_h->fragment_offset = swap_byte_2(IP_HEADER_FLAGS_DF); */
	ip_h->fragment_offset = 0;
	ip_h->ttl = 64;
	ip_h->protocol = IP_HEADER_PROTOCOL_TCP;
	ip_h->header_checksum = 0;
	memcpy(ip_h->src_ip, own_ip, 4);
	memcpy(ip_h->dst_ip, session->dst_ip, 4);
	ip_h->header_checksum = swap_byte_2(get_ip_checksum(ip_h));

	/* p = (unsigned char *)ip_h; */
	/* for (i = 0; i < sizeof(struct ip_header); i++) { */
	/* 	puth(*p++, 2); */
	/* 	putchar(' '); */
	/* } */
	/* puts("\r\n"); */

	/* tcp */
	base_addr += sizeof(struct ip_header);
	tcp_h = (struct tcp_header *)base_addr;
	tcp_h->src_port = swap_byte_2(session->src_port);
	tcp_h->dst_port = swap_byte_2(session->dst_port);
	tcp_h->seq_num = swap_byte_4(session->seq_num);
	tcp_h->ack_num = swap_byte_4(session->ack_num);
	/* tcp_h->header_length_flags = */
	/* 	swap_byte_2((10U << TCP_HEADER_LEN_SHIFT) | TCP_FLAGS_SYN); */
	tcp_h->header_length_flags =
		swap_byte_2((5U << TCP_HEADER_LEN_SHIFT) | TCP_FLAGS_SYN);
	tcp_h->window_size = swap_byte_2(29200);
	/* tcp_h->check_sum = swap_byte_2(0xc221); */
	tcp_h->check_sum = 0;
	tcp_h->urgent_pointer = 0;
	tcp_h->check_sum = swap_byte_2(get_tcp_checksum(tcp_h, session));

	/* /\* tcp (options) *\/ */
	/* base_addr += sizeof(struct tcp_header); */
	/* tcp_opt_h = (struct tcp_header_options_a *)base_addr; */
	/* tcp_opt_h->max_seg_size.kind = 2; */
	/* tcp_opt_h->max_seg_size.length = 4; */
	/* tcp_opt_h->max_seg_size.mss_value = swap_byte_2(1460); */
	/* tcp_opt_h->tcp_sack_permit.kind = 4; */
	/* tcp_opt_h->tcp_sack_permit.length = 2; */
	/* tcp_opt_h->timestamp.kind = 8; */
	/* tcp_opt_h->timestamp.length = 10; */
	/* tcp_opt_h->timestamp.timestamp_value = swap_byte_4(145763); */
	/* tcp_opt_h->timestamp.timestamp_echo_replay = 0; */
	/* tcp_opt_h->nop = 1; */
	/* tcp_opt_h->window_scale.kind = 3; */
	/* tcp_opt_h->window_scale.length = 3; */
	/* tcp_opt_h->window_scale.shift_count = 7; */

	/* p = (unsigned char *)tcp_h; */
	/* for (i = 0; i < sizeof(struct tcp_header); i++) { */
	/* 	puth(*p++, 2); */
	/* 	putchar(' '); */
	/* } */
	/* puts("\r\n"); */

	/* unsigned short len = */
	/* 	sizeof(struct ethernet_header) + sizeof(struct ip_header) */
	/* 	+ sizeof(struct tcp_header) */
	/* 	+ sizeof(struct tcp_header_options_a); */

	unsigned short len =
		sizeof(struct ethernet_header) + sizeof(struct ip_header)
		+ sizeof(struct tcp_header);

	p = (unsigned char *)send_buf;
	for (i = 0; i < len; i++) {
		puth(*p++, 2);
		putchar(' ');
	}
	puts("\r\n");

	send_packet(send_buf, len);

	session->id++;
	session->seq_num++;
}

void connect_synack(struct tcp_session *session)
{
	while (1) {
		unsigned short len;
		receive_packet(recv_buf, &len);

		if (len == 0)
			continue;

		struct tcp_header *tcp_h = (struct tcp_header *)(
			recv_buf + sizeof(struct ethernet_header)
			+ sizeof(struct ip_header));

		if (!(tcp_h->header_length_flags & TCP_FLAGS_SYN))
			continue;

		if (!(tcp_h->header_length_flags & TCP_FLAGS_ACK))
			continue;

		if (tcp_h->ack_num != session->seq_num)
			continue;

		session->ack_num = tcp_h->ack_num + 1;
		break;
	}
}

void connect_ack(struct tcp_session *session)
{
	unsigned long long base_addr;
	struct ip_header *ip_h;
	struct tcp_header *tcp_h;
	struct tcp_header_options_b *tcp_opt_h;

	/* frame */
	base_addr = (unsigned long long)send_buf;

	/* ip */
	base_addr += sizeof(struct ethernet_header);
	ip_h = (struct ip_header *)base_addr;
	ip_h->total_length = swap_byte_2(52);
	ip_h->identification = swap_byte_2(session->id);

	/* tcp */
	base_addr += sizeof(struct ip_header);
	tcp_h = (struct tcp_header *)base_addr;
	tcp_h->seq_num = swap_byte_4(session->seq_num);
	tcp_h->ack_num = swap_byte_4(session->ack_num);
	tcp_h->header_length_flags =
		swap_byte_2((8U << TCP_HEADER_LEN_SHIFT) | TCP_FLAGS_ACK);
	tcp_h->window_size = swap_byte_2(229);

	/* tcp (options) */
	base_addr += sizeof(struct tcp_header);
	tcp_opt_h = (struct tcp_header_options_b *)base_addr;
	tcp_opt_h->nop_1 = 1;
	tcp_opt_h->nop_2 = 1;
	tcp_opt_h->timestamp.kind = 8;
	tcp_opt_h->timestamp.length = 10;
	tcp_opt_h->timestamp.timestamp_value = swap_byte_4(145770);
	tcp_opt_h->timestamp.timestamp_echo_replay = swap_byte_4(2512521363);

	unsigned short len =
		sizeof(struct ethernet_header) + sizeof(struct ip_header)
		+ sizeof(struct tcp_header)
		+ sizeof(struct tcp_header_options_b);

	send_packet(send_buf, len);

	session->id++;
	session->seq_num++;
}

struct tcp_session *connect(
	unsigned char dst_mac[], unsigned char dst_ip[],
	unsigned short src_port, unsigned short dst_port)
{
	struct tcp_session *session =
		connect_init(dst_mac, dst_ip, src_port, dst_port);
	connect_syn(session);
	connect_synack(session);
	connect_ack(session);

	return session;
}

void http_get(struct tcp_session *session)
{
	unsigned long long base_addr;
	struct ip_header *ip_h;
	struct tcp_header *tcp_h;
	struct tcp_header_options_b *tcp_opt_h;
	unsigned char *http_req_p;

	/* frame */
	base_addr = (unsigned long long)send_buf;

	/* ip */
	base_addr += sizeof(struct ethernet_header);
	ip_h = (struct ip_header *)base_addr;
	ip_h->total_length = swap_byte_2(52);
	ip_h->identification = swap_byte_2(session->id);

	/* tcp */
	base_addr += sizeof(struct ip_header);
	tcp_h = (struct tcp_header *)base_addr;
	tcp_h->seq_num = swap_byte_4(session->seq_num);
	tcp_h->ack_num = swap_byte_4(session->ack_num);
	tcp_h->header_length_flags =
		swap_byte_2((8U << TCP_HEADER_LEN_SHIFT) | TCP_FLAGS_PSH
			    | TCP_FLAGS_ACK);
	tcp_h->window_size = swap_byte_2(229);

	/* tcp (options) */
	base_addr += sizeof(struct tcp_header);
	tcp_opt_h = (struct tcp_header_options_b *)base_addr;
	tcp_opt_h->nop_1 = 1;
	tcp_opt_h->nop_2 = 1;
	tcp_opt_h->timestamp.kind = 8;
	tcp_opt_h->timestamp.length = 10;
	tcp_opt_h->timestamp.timestamp_value = swap_byte_4(145770);
	tcp_opt_h->timestamp.timestamp_echo_replay = swap_byte_4(2512521363);

	/* http */
	base_addr += sizeof(struct tcp_header_options_b);
	http_req_p = (unsigned char *)base_addr;
	memcpy(http_req_p, http_request, sizeof(http_request));

	unsigned short len =
		sizeof(struct ethernet_header) + sizeof(struct ip_header)
		+ sizeof(struct tcp_header)
		+ sizeof(struct tcp_header_options_b) + sizeof(http_request);

	send_packet(send_buf, len);

	session->id++;
	session->seq_num++;
}

void http_rcv(struct tcp_session *session)
{
	while (1) {
		unsigned short len;
		receive_packet(recv_buf, &len);

		if (len == 0)
			continue;

		unsigned long long base_addr =
			(unsigned long long)recv_buf
			+ sizeof(struct ethernet_header)
			+ sizeof(struct ip_header);
		struct tcp_header *tcp_h = (struct tcp_header *)base_addr;

		if (!(tcp_h->header_length_flags & TCP_FLAGS_PSH))
			continue;

		if (!(tcp_h->header_length_flags & TCP_FLAGS_ACK))
			continue;

		base_addr += sizeof(struct tcp_header)
			+ sizeof(struct tcp_header_options_b);

		len -= sizeof(struct ethernet_header) + sizeof(struct ip_header)
			+ sizeof(struct tcp_header)
			+ sizeof(struct tcp_header_options_b);

		unsigned char *http_rcv = (unsigned char *)base_addr;
		unsigned int i;
		for (i = 0; i < len; i++) {
			puth(*http_rcv++, 2);
			putchar(' ');
		}
		puts("\r\n");

		session->ack_num = tcp_h->ack_num + 1;
		break;
	}
}

void disconnect(struct tcp_session *session __attribute__((unused)))
{
	return;
}
