unsigned int foo = 0xbeefcafe;

int main(void)
{
	volatile unsigned int a = foo;
	while (1);

	return 0;
}
