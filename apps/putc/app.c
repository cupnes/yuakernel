int main(void)
{
	asm volatile ("movq $0, %rdi\n"
		      "movq $'A', %rsi\n"
		      "int $0x80");
	while (1);

	return 0;
}
