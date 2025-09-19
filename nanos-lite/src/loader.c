#include <proc.h>
#include <elf.h>

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;
#define RAMDISK_SIZE ((&ramdisk_end) - (&ramdisk_start))
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#define Elf_Size 64
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#define Elf_Size 52
#endif

#if defined(__ISA_AM_NATIVE__)
#define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_X86__)
#define EXPECT_TYPE EM_X86_64
#elif defined(__riscv)
#define EXPECT_TYPE EM_RISCV
#elif defined(__ISA_LOONGARCH32R__)
#define EXPECT_TYPE EM_MIPS
#else
#error unsupported ISA __ISA__
#endif

int fs_open(const char *filename, int flags, int mode);
size_t fs_lseek(int fd, size_t offset, int whence);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void* buf, size_t len);
int fs_close(int fd);
enum {SEEK_SET, SEEK_CUR, SEEK_END};

static uintptr_t loader(PCB *pcb, const char *filename)
{
  int fd = fs_open(filename,0,0);
  Elf_Ehdr ehdr;
  fs_lseek(fd,0,SEEK_SET);
  fs_read(fd,&ehdr,Elf_Size);
  //assert(*(uint32_t *)ehdr.e_ident == 0x7f454c46);
  assert(ehdr.e_machine == EXPECT_TYPE);
  Elf_Phdr phdrs[ehdr.e_phnum];
  fs_lseek(fd,ehdr.e_phoff,SEEK_SET);
  fs_read(fd,phdrs,ehdr.e_phentsize * ehdr.e_phnum);
  for (int i = 0; i < ehdr.e_phnum; i++)
  {
    Elf_Phdr *p = &phdrs[i];
    if (p->p_type == PT_LOAD)
    {
      fs_lseek(fd,p->p_offset,SEEK_SET);
      fs_read(fd,(void *)p->p_vaddr,p->p_filesz);
      if (p->p_memsz > p->p_filesz)
      {
        memset((void *)p->p_vaddr + p->p_filesz,0,p->p_memsz-p->p_filesz);        
      }
    }
  }
  fs_close(fd);

  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
