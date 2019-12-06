#include <fs.h>
#include <common.h>

#define END_OF_FS	0x00

struct file *fs_start;

void fs_init(void *_fs_start)
{
	fs_start = _fs_start;
}

struct file *open(char *name)
{
	struct file *f = fs_start;
	while (f->name[0] != END_OF_FS) {
		if (!strcmp(f->name, name))
			return f;

		f = (struct file *)((unsigned long long)f + sizeof(struct file)
				    + f->size);
	}

	return NULL;
}

static unsigned long long _get_files_all(struct file *files[])
{
	struct file *f = fs_start;
	unsigned int num;

	for (num = 0; f->name[0] != END_OF_FS; num++) {
		files[num] = f;
		f = (struct file *)((unsigned long long)f + sizeof(struct file)
				    + f->size);
	}

	return num;
}

static unsigned long long _get_files_with_prefix(
	struct file *files[], char prefix)
{
	struct file *f;
	unsigned int num = 0;
	for (f = fs_start; f->name[0] != END_OF_FS;) {
		if (f->name[0] == prefix)
			files[num++] = f;

		f = (struct file *)((unsigned long long)f + sizeof(struct file)
				    + f->size);
	}

	return num;
}

unsigned long long get_files(struct file *files[], char prefix)
{
	if (prefix == 0)
		return _get_files_all(files);
	else
		return _get_files_with_prefix(files, prefix);
}
