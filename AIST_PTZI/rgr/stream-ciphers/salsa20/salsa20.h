#include "ecrypt-sync.h"

#define ROTATE(v,c) (ROTL32(v,c))
#define XOR(v,w) ((v) ^ (w))
#define PLUS(v,w) (U32V((v) + (w)))
#define PLUSONE(v) (PLUS((v),1))

static void salsa20_wordtobyte(u8 output[64],const u32 input[16]);

void ECRYPT_init(void);

static const char sigma[17] = "expand 32-byte k";
static const char tau[17] = "expand 16-byte k";

void ECRYPT_keysetup(ECRYPT_ctx *x,const u8 *k,u32 kbits,u32 ivbits);

void ECRYPT_ivsetup(ECRYPT_ctx *x,const u8 *iv);

void ECRYPT_encrypt_bytes(ECRYPT_ctx *x,const u8 *m,u8 *c,u32 bytes);
void ECRYPT_decrypt_bytes(ECRYPT_ctx *x,const u8 *c,u8 *m,u32 bytes);

void ECRYPT_keystream_bytes(ECRYPT_ctx *x,u8 *stream,u32 bytes);
