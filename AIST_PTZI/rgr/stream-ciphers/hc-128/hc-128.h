#include "ecrypt-sync.h"

/* =====================================================================
 *     The following defines the keystream generation function          
 *======================================================================*/

/*h1 function*/
#define h1(ctx, x, y) {    \
     u8 a,c;               \
     a = (u8) (x);         \
     c = (u8) ((x) >> 16);  \
     y = (ctx->T[512+a])+(ctx->T[512+256+c]); \
}

/*h2 function*/
#define h2(ctx, x, y) {    \
     u8 a,c;               \
     a = (u8) (x);         \
     c = (u8) ((x) >> 16); \
     y = (ctx->T[a])+(ctx->T[256+c]); \
}

/*one step of HC-128, update P and generate 32 bits keystream*/
#define step_P(ctx,u,v,a,b,c,d,n){    \
     u32 tem0,tem1,tem2,tem3;         \
     h1((ctx),(ctx->X[(d)]),tem3);              \
     tem0 = ROTR32((ctx->T[(v)]),23);           \
     tem1 = ROTR32((ctx->X[(c)]),10);           \
     tem2 = ROTR32((ctx->X[(b)]),8);            \
     (ctx->T[(u)]) += tem2+(tem0 ^ tem1);       \
     (ctx->X[(a)]) = (ctx->T[(u)]);             \
     (n) = tem3 ^ (ctx->T[(u)]) ;               \
}       

/*one step of HC-128, update Q and generate 32 bits keystream*/
#define step_Q(ctx,u,v,a,b,c,d,n){      \
     u32 tem0,tem1,tem2,tem3;           \
     h2((ctx),(ctx->Y[(d)]),tem3);              \
     tem0 = ROTR32((ctx->T[(v)]),(32-23));      \
     tem1 = ROTR32((ctx->Y[(c)]),(32-10));      \
     tem2 = ROTR32((ctx->Y[(b)]),(32-8));       \
     (ctx->T[(u)]) += tem2 + (tem0 ^ tem1);     \
     (ctx->Y[(a)]) = (ctx->T[(u)]);             \
     (n) = tem3 ^ (ctx->T[(u)]) ;               \
}   

/*16 steps of HC-128, generate 512 bits keystream*/
void generate_keystream(ECRYPT_ctx* ctx, u32* keystream);

/*======================================================*/
/*   The following defines the initialization functions */
/*======================================================*/

#define f1(x)  (ROTR32((x),7) ^ ROTR32((x),18) ^ ((x) >> 3))
#define f2(x)  (ROTR32((x),17) ^ ROTR32((x),19) ^ ((x) >> 10))

/*update table P*/
#define update_P(ctx,u,v,a,b,c,d){      \
     u32 tem0,tem1,tem2,tem3;           \
     tem0 = ROTR32((ctx->T[(v)]),23);           \
     tem1 = ROTR32((ctx->X[(c)]),10);           \
     tem2 = ROTR32((ctx->X[(b)]),8);            \
     h1((ctx),(ctx->X[(d)]),tem3);              \
     (ctx->T[(u)]) = ((ctx->T[(u)]) + tem2+(tem0^tem1)) ^ tem3;         \
     (ctx->X[(a)]) = (ctx->T[(u)]);             \
}  

/*update table Q*/
#define update_Q(ctx,u,v,a,b,c,d){      \
     u32 tem0,tem1,tem2,tem3;      \
     tem0 = ROTR32((ctx->T[(v)]),(32-23));             \
     tem1 = ROTR32((ctx->Y[(c)]),(32-10));             \
     tem2 = ROTR32((ctx->Y[(b)]),(32-8));            \
     h2((ctx),(ctx->Y[(d)]),tem3);              \
     (ctx->T[(u)]) = ((ctx->T[(u)]) + tem2+(tem0^tem1)) ^ tem3; \
     (ctx->Y[(a)]) = (ctx->T[(u)]);                       \
}     

/*16 steps of HC-128, without generating keystream, */
/*but use the outputs to update P and Q*/
void setup_update(ECRYPT_ctx* ctx); /*each time 16 steps*/

void ECRYPT_init(void);
  /* No operation performed */

/* for the 128-bit key:  key[0]...key[15]
*  key[0] is the least significant byte of ctx->key[0] (K_0);
*  key[3] is the most significant byte of ctx->key[0]  (K_0);
*  ...
*  key[12] is the least significant byte of ctx->key[3] (K_3)
*  key[15] is the most significant byte of ctx->key[3]  (K_3)
*
*  for the 128-bit iv:  iv[0]...iv[15]
*  iv[0] is the least significant byte of ctx->iv[0] (IV_0);
*  iv[3] is the most significant byte of ctx->iv[0]  (IV_0);
*  ...
*  iv[12] is the least significant byte of ctx->iv[3] (IV_3)
*  iv[15] is the most significant byte of ctx->iv[3]  (IV_3)
*/

void ECRYPT_keysetup(
  ECRYPT_ctx* ctx, 
  const u8* key, 
  u32 keysize,                /* Key size in bits (128+128*i) */ 
  u32 ivsize);                /* IV size in bits  (128+128*i)*/


void ECRYPT_ivsetup(ECRYPT_ctx* ctx, const u8* iv);

/*========================================================
 *  The following defines the encryption of data stream
 *========================================================
 */

void ECRYPT_process_bytes(
  int action,                 /* 0 = encrypt; 1 = decrypt; */
  ECRYPT_ctx* ctx, 
  const u8* input, 
  u8* output, 
  u32 msglen);               /* Message length in bytes. */ 