#include <proc.h>
#include <elf.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#define Ehdr_Size 64
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#define Ehdr_Size 52
#endif

#if defined(__ISA_AM_NATIVE__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_X86__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_MIPS32__)
# define EXPECT_TYPE EM_MIPS
#elif defined(__riscv)
# define EXPECT_TYPE EM_RISCV
#elif defined(__ISA_LOONGARCH32R__)
# define EXPECT_TYPE EM_LOONGARCH 
#else 
# error unspported ISA __ISA__
#endif

static uintptr_t loader(PCB *pcb, const char *filename)
{
  Elf_Ehdr ehdr;
  if(ramdisk_read(&ehdr,0,Ehdr_Size) != Ehdr_Size){
    printf("Fail to read !\n");
    return 0;
  }
  if (ehdr.e_ident[EI_MAG0] != ELFMAG0 ||
      ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
      ehdr.e_ident[EI_MAG2] != ELFMAG2 ||
      ehdr.e_ident[EI_MAG3] != ELFMAG3)
  {
    printf("Error , Not an ELF file \n");
    return -1;
  }
  if(ehdr.e_machine != EXPECT_TYPE){
    printf("Error  Wrong ISA \n");
    return -1;
  }
  Elf_Phdr phdrs[ehdr.e_phnum];
  ramdisk_read(phdrs , ehdr.e_phoff,ehdr.e_phnum * ehdr.e_phentsize);

  for(int i=0;i<ehdr.e_phnum;i++){
    Elf_Phdr *p = &phdrs[i];
    if(p->p_type != PT_LOAD){
      continue;
    }
    void *start = &ramdisk_start + p->p_offset;
    ramdisk_write(start,&ramdisk + p->p_vaddr,p->p_filesz);
    
    if(p->p_memsz > p->p_filesz){
      memset(&ramdisk + p->p_offset + p->p_filesz, 0, p->p_memsz - p->p_filesz);
    }
    printf("Successfully load the %drd segmant \n",i);

  }
  uint32_t entry = ehdr.e_entry;
  return entry;

}

void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
