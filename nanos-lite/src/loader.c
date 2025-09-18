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

static uintptr_t loader(PCB *pcb, const char *filename)
{

  Elf_Ehdr ehdr;
  ramdisk_read(&ehdr, 0, Elf_Size);
  //assert(*(uint32_t *)ehdr.e_ident == 0x7f454c46);
  assert(ehdr.e_machine == EXPECT_TYPE);
  Elf_Phdr phdrs[ehdr.e_phnum];
  ramdisk_read(phdrs, ehdr.e_phoff, ehdr.e_phentsize * ehdr.e_phnum);
  for (int i = 0; i < ehdr.e_phnum; i++)
  {
    Elf_Phdr *p = &phdrs[i];
    if (p->p_type == PT_LOAD)
    {
      ramdisk_read((void *)p->p_vaddr,p->p_offset, p->p_filesz);
      if (p->p_memsz > p->p_filesz)
      {
        memset((void *)p->p_vaddr + p->p_filesz,0,p->p_memsz-p->p_filesz);        
      }
    }
  }

  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
