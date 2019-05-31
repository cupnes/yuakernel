#pragma once

#define SMP_MAGIC_IDENT	(('_'<<24)|('P'<<16)|('M'<<8)|'_')

struct __attribute__((packed)) mp_floating {
	char mpf_signature[4];		/* "_MP_" 			*/
	unsigned int mpf_physptr;	/* Configuration table address	*/
	unsigned char mpf_length;	/* Our length (paragraphs)	*/
	unsigned char mpf_specification;/* Specification version	*/
	unsigned char mpf_checksum;	/* Checksum (makes sum 0)	*/
	unsigned char mpf_feature1;	/* Standard or configuration ? 	*/
	unsigned char mpf_feature2;	/* Bit7 set for IMCR|PIC	*/
	unsigned char mpf_feature3;	/* Unused (0)			*/
	unsigned char mpf_feature4;	/* Unused (0)			*/
	unsigned char mpf_feature5;	/* Unused (0)			*/
};

struct __attribute__((packed)) mp_config_table {
	char mpc_signature[4];		// string "PCMP"
#define MPC_SIGNATURE "PCMP"
	unsigned short mpc_length;	/* Size of table */
	char  mpc_spec;			/* 0x01 or 0x04 */
	char  mpc_checksum;		// checksum of the table
	char  mpc_oem[8];		// OEM ID: system hardware manufacturer
	char  mpc_productid[12];	// Product familly ID
	unsigned int mpc_oemptr;	/* 0 if not present, optional */
	unsigned short mpc_oemsize;	/* 0 if not present, optional */
	unsigned short mpc_oemcount;	// entry count
	unsigned int mpc_lapic;	/* Local APIC address */
	unsigned int reserved;
};

void mp_init(void);
