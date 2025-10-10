/******************************************************************************/
/* File name: rabbit.h                                                        */
/*----------------------------------------------------------------------------*/
/* Rabbit C source code in ECRYPT format                                      */
/*----------------------------------------------------------------------------*/
/* Copyright (C) Cryptico A/S. All rights reserved.                           */
/*                                                                            */
/* YOU SHOULD CAREFULLY READ THIS LEGAL NOTICE BEFORE USING THIS SOFTWARE.    */
/*                                                                            */
/* This software is developed by Cryptico A/S and/or its suppliers.           */
/* All title and intellectual property rights in and to the software,         */
/* including but not limited to patent rights and copyrights, are owned by    */
/* Cryptico A/S and/or its suppliers.                                         */
/*                                                                            */
/* The software may be used solely for non-commercial purposes                */
/* without the prior written consent of Cryptico A/S. For further             */
/* information on licensing terms and conditions please contact Cryptico A/S  */
/* at info@cryptico.com                                                       */
/*                                                                            */
/* Cryptico, CryptiCore, the Cryptico logo and "Re-thinking encryption" are   */
/* either trademarks or registered trademarks of Cryptico A/S.                */
/*                                                                            */
/* Cryptico A/S shall not in any way be liable for any use of this software.  */
/* The software is provided "as is" without any express or implied warranty.  */
/*                                                                            */
/******************************************************************************/

#include "ecrypt-sync.h"
#include "ecrypt-portable.h"

/* Square a 32-bit unsigned integer to obtain the 64-bit result and return */
/* the upper 32 bits XOR the lower 32 bits */
static u32 RABBIT_g_func(u32 x);

/* Calculate the next internal state */
static void RABBIT_next_state(RABBIT_ctx *p_instance);

/* No initialization is needed for Rabbit */
void ECRYPT_init(void);

/* Key setup */
void ECRYPT_keysetup(ECRYPT_ctx* ctx, const u8* key, u32 keysize, u32 ivsize);

/* IV setup */
void ECRYPT_ivsetup(ECRYPT_ctx* ctx, const u8* iv);

/* Encrypt/decrypt a message of any size */
void ECRYPT_process_bytes(int action, ECRYPT_ctx* ctx, const u8* input, 
          u8* output, u32 msglen);

/* Generate keystream */
void ECRYPT_keystream_bytes(ECRYPT_ctx* ctx, u8* keystream, u32 length);

/* Encrypt/decrypt a number of full blocks */
void ECRYPT_process_blocks(int action, ECRYPT_ctx* ctx, const u8* input, 
          u8* output, u32 blocks);