#include <lib.h>
#include <local_conf.h>

/* #define PACKET_BUF_SIZE	1024 */

/* #define SRC_PORT	51268 */
/* #define HTTP_PORT	80 */

/* #define FRAME_TYPE_IP	0x0800 */

/* struct __attribute__((packed)) ethrenet_header { */
/* 	unsigned char dst_mac[6]; */
/* 	unsigned char src_mac[6]; */
/* 	unsigned short type; */
/* }; */

/* struct tcp_session { */
/* }; */

/* const unsigned char default_gw_mac[6] = { */
/* 	DEFAULT_GW_MAC_0, DEFAULT_GW_MAC_1, DEFAULT_GW_MAC_2, */
/* 	DEFAULT_GW_MAC_3, DEFAULT_GW_MAC_4, DEFAULT_GW_MAC_5}; */
/* const unsigned char cupnes_com_ip[4] = {49, 212, 139, 172}; */

/* unsigned char packet_buf[PACKET_BUF_SIZE]; */
/* struct tcp_session session_buf; */

/* struct tcp_session *connect(unsigned char dst_ip[], unsigned short src_port, */
/* 			    unsigned short dst_port); */
/* void http_test(struct tcp_session *session); */
/* void disconnect(struct tcp_session *session); */

int main(void)
{
	set_fg(255, 255, 255);
	set_bg(0, 70, 250);
	clear_screen();

	unsigned char self_mac[6];
	get_mac(self_mac);
	unsigned char i;
	for (i = 0; i < 6; i++) {
		puth(self_mac[i], 2);
		putc(' ');
	}
	puts("\r\n");

	while (1);


	/* struct tcp_session *session; */

	/* session = connect(cupnes_com_ip, SRC_PORT, HTTP_PORT); */

	/* http_test(session); */

	/* disconnect(session); */

	return 0;
}

/* struct tcp_session *connect(unsigned char dst_ip[], unsigned short src_port, */
/* 			    unsigned short dst_port) */
/* { */
/* 	/\* 3ウェイハンドシェイクを確立する *\/ */

/* 	/\* SYN *\/ */

/* 	return &session_buf; */
/* } */

/* void http_test(void) */
/* { */
/* } */

/* void disconnect(unsigned char dst_ip[], unsigned short src_port, unsigned short dst_port) */
/* { */
/* } */
