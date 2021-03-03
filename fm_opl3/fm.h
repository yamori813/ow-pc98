/* fm.h */
/* FMSOUND Copyright 1997 by Ethan Brodsky.  All rights reserved. */

#define BYTE  unsigned char
#define WORD  unsigned int
#define DWORD unsigned long

typedef struct
  {
    BYTE chars[2];
    BYTE ksl_lev[2];
    BYTE att_dec[2];
    BYTE sus_rel[2];
    BYTE wav_sel[2];
    BYTE fb_conn;
  } PATCH;

#ifdef __BORLANDC__
typedef far PATCH far *PPTR_PATCH; // simplifes portability to Pascal
#else
typedef PATCH *PPTR_PATCH;         // not Borland: forget Pascal
#endif

PPTR_PATCH fm_get_patch_sine(void);
 // ret: far ptr to sine wave patch, suitable for passing to fm_load_patch

PPTR_PATCH fm_get_patch_piano(void);
 // ret: far ptr to piano patch, suitable for passing to fm_load_patch

int  fm_detect(void);
 // detects OPL2 (or compatible) FM synthesizer at Adlib addresses
 // ret: non-zero if chip detected, zero if not

void fm_reset(void);
 // resets synth

void fm_load_patch(int ch, PPTR_PATCH patch);
 // loads patch (parameters describing instrument) to FM synth
 //   ch:    channel (0-8)
 //   patch: pointer to patch data structure


int  fm_play_tone(int ch, int freq, int vol);
 // starts playing tone (must first call fm_load_patch)
 //   ch:    channel (0-8)
 //   freq:  frequency (Hz)
 //   vol:   volume:  0=-47.25 dB (i.e. silence), 63=-0 dB (i.e. max)

void fm_stop_tone(int ch);
 // stops playing tone
 //   ch:    channel (0-8)
