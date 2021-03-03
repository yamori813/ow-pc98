/* fm.c */
/* FMSOUND Copyright 1997 by Ethan Brodsky.  All rights reserved. */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include "fm.h"

#define _USE_PC_9801_   1
#define _USE_CANBE_SND2 0

#if _USE_PC_9801_
 #if _USE_CANBE_SND2
  // Canbe,118
  #define ADDRESS_PORT 0x1488
  #define STATUS_PORT  0x1488
  #define DATA_PORT    0x1489
 #else
  // SB16
  #define ADDRESS_PORT 0xC8D2
  #define STATUS_PORT  0xC8D2
  #define DATA_PORT    0xC9D2
 #endif
#else
 #define ADDRESS_PORT 0x388
 #define STATUS_PORT  0x388
 #define DATA_PORT    0x389
#endif

#ifdef __TURBOC__
 #define inp inportb
 #define outp outportb
#endif


static BYTE fm_car_ksl[8];		// carrier ksl param (level irrelevant)
static BYTE fm_mod_chars[8];	// modulator chars w/ harmonic mult
static BYTE fm_car_chars[8];	// carrier chars w/ harmonic mult
static BYTE fm_rB0[8];			// high byte of block/f-number/key-on

static PATCH sine  = {{0x00, 0x21}, {0x3F, 0x00}, {0x00, 0xFF}, {0x00, 0x0F}, {0x00, 0x00}, 0x00};
static PATCH piano = {{0x11, 0x21}, {0x90, 0x00}, {0x82, 0xF2}, {0x02, 0x73}, {0x00, 0x00}, 0x0A};

PPTR_PATCH  fm_get_patch_sine(void)  { return &sine;  }
PPTR_PATCH  fm_get_patch_piano(void) { return &piano; }



static void fm_set_reg(int reg, BYTE value)
{
	int i;

	// write register
	outp(ADDRESS_PORT, reg);

	// 3.3 usec delay
	for (i = 0; i < 6; i++)
	{
		inp(ADDRESS_PORT);
	}

	// write value
	outp(DATA_PORT, value);

	// 23 usec delay
	for (i = 0; i < 35; i++)
	{
		inp(ADDRESS_PORT);
	}
}

// play with FM timers to verify existence of chip
int fm_detect(void)
{
	int ret;
	int i;
	BYTE status1, status2;

#if _USE_PC_9801_
 #if _USE_CANBE_SND2
	ret = inp( 0xa460 ) & 0xf0;
	if( (ret== 0x70) || (ret == 0x80) )
	{
		// YMF297 存在確認
		outp( 0x148e,0x00 );
		if( ! (inp( 0x148e+1) & 0x08) )
		{
			// OPNA をマスクする
			outp( 0xa460,ret | 0x03 );

				outp( 0x005f,0x00 );
			outp( 0x0f4a,0x01 );	// サウンド設定
				outp( 0x005f,0x00 );
			outp( 0x0f4b,0x80 );	// 割り込み番号、最上位ビットは不明

				outp( 0x005f,0x00 );
			outp( 0x148a,0x05 );	// 不明
				outp( 0x005f,0x00 );
			outp( 0x148b,0x05 );	// 不明

				outp( 0x005f,0x00 );
			outp( 0x148a,0x08 );	// 不明
				outp( 0x005f,0x00 );
			outp( 0x148b,0x04 );	// 不明

			for( i=0;i<2000;i++ )
			{
				outp( 0x005f,0x00 );
			}

			outp( 0x148b,0x00 );	// 不明
			
			outp( 0x1488,0xf7 );	// 不明
			outp( 0x1489,0xf7 );	// 不明
		}
	}
 #endif
#endif

	fm_set_reg(4, 0x60);
	fm_set_reg(4, 0x80);
	status1 = inp(STATUS_PORT);

	fm_set_reg(2, 0xFF);
	fm_set_reg(4, 0x21);

	for (i = 0; i < 125; i++)
	{
		inp(ADDRESS_PORT);
	}

	status2 = inp(STATUS_PORT);

	fm_set_reg(4, 0x60);
	fm_set_reg(4, 0x80);

	ret = ((status1 & 0xE0) == 0) && ((status2 & 0xE0) == 0xC0);

	return ret;
}

void fm_reset(void)
{
	int i;

	// clear all registers
	for (i = 1; i <= 0xF5; i++)
	{
		fm_set_reg(i, 0x00);
	}

	// enable waveform select (bit 5 of register 0x01)
	fm_set_reg(0x01, 0x20);
}

static void fm_set_op_reg(int reg_base, int ch, int op, BYTE value)
{
	// calculate reg index based on base register, channel, and operator
	int reg = reg_base + ch;
	if (ch > 2)
	{
		reg += 5;
	}
	if (ch > 5)
	{
		reg += 5;
	}
	if (op)
	{
		reg += 3;
	}
	fm_set_reg(reg, value);
}

static void fm_set_ch_reg(int reg_base, int ch, BYTE value)
{
	int reg = reg_base + ch;

	fm_set_reg(reg, value);
}

void fm_load_patch(int ch, PPTR_PATCH patch)
{
	fm_set_op_reg(0x20, ch, 0, patch->chars[0]);
	fm_set_op_reg(0x20, ch, 1, patch->chars[1]);
	fm_set_op_reg(0x40, ch, 0, patch->ksl_lev[0]);
	fm_set_op_reg(0x40, ch, 1, patch->ksl_lev[1]);
	fm_set_op_reg(0x60, ch, 0, patch->att_dec[0]);
	fm_set_op_reg(0x60, ch, 1, patch->att_dec[1]);
	fm_set_op_reg(0x80, ch, 0, patch->sus_rel[0]);
	fm_set_op_reg(0x80, ch, 1, patch->sus_rel[1]);
	fm_set_op_reg(0xE0, ch, 0, patch->wav_sel[0]);
	fm_set_op_reg(0xE0, ch, 1, patch->wav_sel[1]);
	fm_set_ch_reg(0xC0, ch,	patch->fb_conn);

	// parameters required for manipulation of patch
	fm_car_ksl[ch]   = patch->ksl_lev[1];
	fm_mod_chars[ch] = patch->chars[0];
	fm_car_chars[ch] = patch->chars[1];
}

// calculates f-number and block to play a given base frequency
//   (base frequency is frequency before scaling with mult)
// returns:  success=block:f_num, failure=0xFFFF
static WORD fm_get_block_fnum(int freq)
{
	DWORD f_num = 65536UL * freq / 3125;	// 2 * 524288 * freq / 50000
	WORD  block = 0;						// half harmonic

	while( (f_num > 0x3FF) && (block < 8) ) // f_num 9 bits, block 3
	{
		block += 1;							// next block
		f_num /= 2;							// half f_num
	}

	// verify we're haven't exceeded the base freq caps of the synth
	if( block > 7 )
	{
		return 0xFFFF;					   // bad - return value
	}
	else
	{
		return (block << 10) | (WORD)f_num;  // good - assemble B0:A0 params
	}
}


// attempts to play a tone of frequency freq on channel ch at volume vol
int fm_play_tone(int ch, int freq, int vol)
{
	WORD block_fnum;
	int  mult = 1;						// frequency multiplier
	int  c_mult, m_mult;				// car. and mod. multipliers
										// taking harmonics into acct.
	BYTE r20m, r20c, r40c, rA0, rB0;	// FM synth parameters

	// try to find block, f-number, and mult that allows you to play sound
	// first attempt it with mult=1, then double mult, until 8, then try 15
	do
	{
		// attempt to find params for base frequency freq/mult
		block_fnum = fm_get_block_fnum(freq/mult);

		// impossible, try a new mult
		if( block_fnum == 0xFFFF )
		{
			// double mult (1, 2, 4, 8)
			mult *= 2;

			// mult=16 not supported, try 15, then give up
			if( mult == 16 )
			{
				mult = 15;
			}

			// didn't work with mult=15, fail
			if( mult == 15 )
			{
				break;
			}
		}
	}
	while( block_fnum == 0xFFFF );

	// calculate carrier and modulator mults
	m_mult = (fm_mod_chars[ch]&0x0F) * mult;
	c_mult = (fm_car_chars[ch]&0x0F) * mult;

	// verify success (valid block/f-num, mults in range (w/harmonics))
	if( (block_fnum == 0xFFFF) || (m_mult > 15) || (c_mult > 15) )
	{
		return 0;
	}

	r20m = (fm_mod_chars[ch]&0xF0) | m_mult;
	r20c = (fm_car_chars[ch]&0xF0) | c_mult;

	r40c = (fm_car_ksl[ch]&0xC0) | (63-vol); // volume -> attenuation

   // set key-on bit
	block_fnum |= 0x2000;

	rA0  = block_fnum & 0xFF;
	rB0  = block_fnum >> 8;

	fm_rB0[ch] = rB0;

	fm_set_op_reg(0x20, ch, 0, r20m);
	fm_set_op_reg(0x20, ch, 1, r20c);
	fm_set_op_reg(0x40, ch, 1, r40c);
	fm_set_ch_reg(0xA0, ch,	rA0);
	fm_set_ch_reg(0xB0, ch,	rB0);

	return 1;
}

void fm_stop_tone(int ch)
{
	// clear key-on (bit 5 of chreg B0)
	// preserve other bits to avoid changing pitch during decay
	fm_set_ch_reg(0xB0, ch, (fm_rB0[ch] & 0xDF));
}

