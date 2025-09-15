#include <am.h>
#include <nemu.h>

#define AUDIO_FREQ_ADDR (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR (AUDIO_ADDR + 0x14)

void __am_audio_init()
{
  outl(AUDIO_FREQ_ADDR, 22050);
  outl(AUDIO_CHANNELS_ADDR, 2);
  outl(AUDIO_SAMPLES_ADDR, 16384);
  outl(AUDIO_SBUF_SIZE_ADDR, 65536);
  outl(AUDIO_INIT_ADDR, 1);
  outl(AUDIO_COUNT_ADDR, 0);
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg)
{
  cfg->present = true;
  cfg->bufsize = inl((uintptr_t)AUDIO_SBUF_SIZE_ADDR);
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl)
{
  ctrl->channels = inl((uintptr_t)AUDIO_CHANNELS_ADDR);
  ctrl->freq = inl((uintptr_t)AUDIO_FREQ_ADDR);
  ctrl->samples = inl((uintptr_t)AUDIO_SAMPLES_ADDR);
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat)
{
  stat->count = inl((uintptr_t)AUDIO_COUNT_ADDR);
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl)
{

  while ((inl((uintptr_t)AUDIO_SBUF_SIZE_ADDR) - inl((uintptr_t)AUDIO_COUNT_ADDR)) <
         (ctl->buf.end - ctl->buf.start))
    ;
  int start = inl((uintptr_t)AUDIO_COUNT_ADDR);
  int count = (ctl->buf.end - ctl->buf.start); 
  int c = inl((uintptr_t)AUDIO_COUNT_ADDR); 
  for(int i = 0;i < count ; i++){
    outb(AUDIO_SBUF_ADDR + start + i, inb((uintptr_t)(ctl->buf.start + i)));
    c++;
  }
  outl(AUDIO_COUNT_ADDR,c);
}
