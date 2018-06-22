struct RSDP {
	char Signature[8];
	unsigned char Checksum;
	char OEMID[6];
	unsigned char Revision;
	unsigned int RsdtAddress;
	unsigned int Length;
	unsigned long long XsdtAddress;
	unsigned char Extended_Checksum;
	unsigned char Reserved[3];
};
