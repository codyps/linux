// SPDX-License-Identifier: GPL-2.0-only
/*
 * Reconstruction of the TG3040 #860 Arelink protocol and software cipher.
 * Firmware pseudocode and constant addresses are recorded in the workbench.
 * The original DNS_01xx names implement the ordinary AES block transform;
 * the DNS_00xx routines implement the device framing and EEPROM protocol.
 */
#include "arelink.h"
#include "arelink-tables.h"

/* Stock initialized slots: version pointer followed by cipher state.
 * Original declarations are unknown; two bytes after block_status are padding.
 * Invalid stored key sizes and zero PRNG seeds retain the documented behavior.
 */
static struct {
    const char * volatile version;
    u16 block_status;
    u8 reserved_2[2];
    volatile u32 random_state;
    u8 key_size;
/* The native block-status writer uses an assembler alias into this struct.
 * GCC cannot see that write and otherwise moves the initialized state to
 * .rodata, where stock's write would fault. Keep the stock-writable section.
 */
} __attribute__((packed)) alpu_initialized
    __attribute__((aligned(8), section(".data"))) = {
    .version = arelink_lib_version, .block_status = 0xffff, .random_state = 0xa37f1da6, .key_size = 2,
};
#define random_state alpu_initialized.random_state
#define key_size alpu_initialized.key_size
#define block_status alpu_initialized.block_status
/* Local aliases retain the independently decoded field offsets. The first
 * pointer occupies the previously missing eight-byte initialized slot.
 */
__asm__(".local block_status\n.type block_status, %object\n.set block_status, alpu_initialized+8\n.size block_status, 2\n"
        ".local random_state\n.type random_state, %object\n.set random_state, alpu_initialized+12\n.size random_state, 4\n"
        ".local key_size\n.type key_size, %object\n.set key_size, alpu_initialized+16\n.size key_size, 1\n");
/* Stock keeps cipher scratch beside protocol state, without serialization.
 * Keep the shared state and its neighboring overwrite victims. TODO: serialize
 * callers and validate all indices in a separate fix. The byte at offset 528
 * and five alignment bytes are reserved; their original declarations are unknown.
 */
static struct {
	u32 rounds, nk;
	u8 aes_input[16], aes_output[16], aes_state[16];
	u8 expanded[240], cipher_key[32];
	u32 passwords[16];
	u8 receive_buffer[72], page_buffer[64];
	u8 reserved_528, initialized, protected_block, reserved_531[5];
	u8 mask_table[16], key[32], authenticated;
} __attribute__((packed)) alpu_state __attribute__((aligned(8)));
#define initialized alpu_state.initialized
#define authenticated alpu_state.authenticated
#define protected_block alpu_state.protected_block
#define key alpu_state.key
#define mask_table alpu_state.mask_table
#define receive_buffer alpu_state.receive_buffer
#define page_buffer alpu_state.page_buffer
#define passwords alpu_state.passwords
/* A typed alias keeps the getter's single-byte address materialization.
 * This is the same initialized byte, not separate state or an original name.
 */
extern u8 alpu_initialized_flag;
__asm__(".local alpu_initialized_flag\n.type alpu_initialized_flag, %object\n"
        ".set alpu_initialized_flag, alpu_state+529\n.size alpu_initialized_flag, 1\n");

/* Same initialized word as the stock srand/rooted generator. No storage.
 * Retain volatile reloads rather than making an unlocked round atomic.
 */
extern volatile u32 alpu_random_word;
__asm__(".local alpu_random_word\n.type alpu_random_word,%object\n.set alpu_random_word,alpu_initialized+12\n.size alpu_random_word,4\n");
/* Compatible native frames, not proof of original declarations or flags.
 * The unshifted word is read before the shifted word at every stage, followed
 * by a fresh return read. Preserve mixed-word races and the absorbing zero.
 * TODO: serialization and zero-seed repair are separate behavior changes.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector"))) static u32 DNS_0019_Func(volatile u32 *state)
{
 *state=*state^(*state<<13);
 *state=*state^(*state>>17);
 *state=*state^(*state<<5);
 return *state;
}
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector"))) static u8 DNS_0020_Func(void)
{
 return DNS_0019_Func(&alpu_random_word);
}
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector"))) u8 arelink_random(void)
{
 u8 result;
 /* Stock rejects both low-byte0 and255. A zero seed can loop forever.
  * TODO: bounded retry and a seed fallback belong in a separate fix.
  */
 do { result=DNS_0020_Func()%255; } while(result==0);
 return result;
}
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector"))) void arelink_srand(u32 seed)
{
 alpu_random_word=seed;
}

/* Stock truncates the count and feedback seed to8/16 bits. Its second input
 * read occurs AFTER the output write, so overlapping buffers can change the
 * next feedback state. TODO: overlap handling is a separate behavior change.
 * These attributes reproduce the native private frame; original flags/spelling
 * are unknown. Keep the retained helper and its two initialization call sites.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0002_Func(const u8 *input, u8 *output, u8 count, u16 state)
{
 u8 i = 0;
 for (; i < count; i++) {
  *output++ = input[i] ^ (state >> 8);
  state = (input[i] + state) * -0x3193 + 0x58bf;
 }
}

/* Stock retains both helpers and computes the mask even for zero nonce/count.
 * Preserve the ordered key/mask reads, byte truncation and overlapping copies.
 * Masks with more than four selected bits can spill into the other nibble.
 * TODO: validate masks and serialize shared state only as separate fixes.
 * These are compatible native frames; original C declarations/flags are unknown.
 */
extern u8 alpu_frame_key[32], alpu_frame_mask[16];
__asm__(".local alpu_frame_key\n.type alpu_frame_key,%object\n.set alpu_frame_key,alpu_state+552\n.size alpu_frame_key,32\n"
        ".local alpu_frame_mask\n.type alpu_frame_mask,%object\n.set alpu_frame_mask,alpu_state+536\n.size alpu_frame_mask,16\n");
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static u8 DNS_0004_Func(u8 nonce)
{
 u8 low, high, sum, highmask, lowmask, result;
 u8 lower, upper;
 u8 i, position, selector, input;
 low=nonce&15;
 high=nonce>>4;
 sum=alpu_frame_key[low]+high;
 highmask=alpu_frame_mask[(u8)(sum>>4)&15];
 lowmask=alpu_frame_mask[sum&15];
 upper=0; position=0; selector=highmask; input=sum;
 for(i=0;i<8;i++) {
  if(selector&1) {
   if(input&1) upper|=1<<position;
   position++;
  }
  selector>>=1; input>>=1;
 }
 lower=0; position=0; selector=lowmask; input=nonce;
 for(i=0;i<8;i++) {
  if(selector&1) {
   if(input&1) lower|=1<<position;
   position++;
  }
  selector>>=1; input>>=1;
 }
 result=(upper<<4)|lower;
 return result;
}
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0005_Func(const u8 *input,u8 *output,u8 count)
{
 u8 i,nonce,mask;
 nonce=*input++;
 mask=DNS_0004_Func(nonce);
 if(nonce==0) {
  for(i=0;i<count;i++) *output++=*input++;
 } else {
  for(i=0;i<count;i++) *output++=*input++^mask;
 }
}

/* Private test wrappers use these source aliases; no extra function bodies. */
#define frame_mask DNS_0004_Func
#define unpack DNS_0005_Func

/* Compatible native bodies; original C spelling/types/flags remain unknown.
 * Counts and XOR masks truncate to bytes. Reverse-copy reads/writes sequentially;
 * overlapping input/output can overwrite later input. An empty reverse computes
 * its before-start end pointer but never dereferences it. TODO: overlap and empty
 * pointer-policy changes belong in separate fixes, not stock reconstruction.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0006_Func(u8 *data,u8 count,u8 mask)
{
 u8 i=0;
 for(;i<count;i++) *data++ ^=mask;
}
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0007_Func(const u8 *input,u8 *output,u8 count)
{
 const u8 *end=input+count-1;
 u8 i=0;
 for(;i<count;i++) *output++=*end--;
}

/* A typed view of the same stock password words; no new storage.
 * The four independently decoded loads must remain separate and word-wide.
 */
extern volatile u32 alpu_passwords[16];
__asm__(".local alpu_passwords\n.type alpu_passwords,%object\n.set alpu_passwords,alpu_state+328\n.size alpu_passwords,64\n");
/* Compatible private-frame spelling, not recovered original declarations.
 * Six wire bytes live at frame+0; its eight unused tail bytes preserve the
 * observed slots below mask/nonce. The original array extent is unknown.
 * Four word reads intentionally permit mixed passwords. TODO: serialization
 * and index validation belong in separate fixes; the caller checks block<15.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector"))) static void DNS_0008_Func(u8 block)
{
 u8 nonce,mask,frame[14];
 nonce=arelink_random();
 mask=DNS_0004_Func(nonce);
 frame[0]=nonce;
 frame[1]=block;
 frame[2]=(u8)(alpu_passwords[block]>>24);
 *((u8 *)&frame[2]+1)=(u8)(alpu_passwords[block]>>16);
 *((u8 *)&frame[2]+2)=(u8)(alpu_passwords[block]>>8);
 *((u8 *)&frame[2]+3)=(u8)alpu_passwords[block];
 DNS_0006_Func(frame+1,5,mask);
 arelink_write_data(0x6000,frame,6);
}

/* Compatible private frames. Sixteen-byte capacity preserves the observed
 * argument and wire slots, but the original array extents are unknown.
 * Only5/3/1 bytes are written or sent; do not initialize the unused tails.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector"))) static void DNS_0009_Func(u8 mode,u8 operation)
{
 u8 frame[16];
 frame[0]=0x80;
 frame[1]=0xc3;
 frame[2]=0x68;
 frame[3]=mode;
 frame[4]=operation;
 arelink_write_data(0x7000,frame,5);
}
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector"))) static void DNS_0010_Func(u16 address,u8 operation)
{
 u8 frame[16];
 frame[0]=address>>8;
 frame[1]=address;
 frame[2]=operation;
 arelink_write_data(0x7008,frame,3);
}
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector"))) static void DNS_0011_Func(u8 value)
{
 u8 frame[16];
 frame[0]=value;
 arelink_write_data(0x7004,frame,1);
}

/* Stock input builders use inline byte loops, not DNS_0007 calls.
 * Keep external reads byte-wide and ordered; caller private frames remain
 * reconstructed. TODO: synchronization/overlap repairs are separate changes.
 */
static inline __attribute__((always_inline)) void reverse_inline(const u8 *input, u8 *output, u8 count)
{
 u8 i;
 for(i=0;i<count;i++) output[i]=((const volatile u8 *)input)[count-1-i];
}
static inline __attribute__((always_inline)) void copy_frame_input(const u8 *input, u8 *output, u8 count)
{
 u8 i;
 for(i=0;i<count;i++) output[i]=((const volatile u8 *)input)[i];
}

static void send_frame(u16 address, u8 *frame, unsigned int count)
{
 u8 mask;
 frame[0]=arelink_random();
 mask=frame_mask(frame[0]);
 DNS_0006_Func(frame+1,count,mask);
 arelink_write_data(address,frame,count+1);
}

#define command DNS_0009_Func
#define select_address DNS_0010_Func
#define finish() DNS_0011_Func(0)

static inline __attribute__((always_inline)) void select_address_inline(u16 address, u8 operation)
{
	u8 data[] = { address >> 8, address, operation };
	arelink_write_data(0x7008, data, sizeof(data));
}

static void cipher_request(const u8 *input, u8 decrypt, u8 reverse_input)
{
	u8 frame[18];
	if (reverse_input)
		reverse_inline(input, frame + 1, 16);
	else
		copy_frame_input(input, frame + 1, 16);
	frame[17] = decrypt;
	send_frame(0x2000, frame, 17);
}

/* Compatible source and flags, not recovered original spelling.
 * Multiply uses only coefficient bits0..4 and always calls xtime ten times.
 * TODO: generalize high coefficients only as a separate behavior change;
 * stock inverse-mix callers use9,11,13,14 and do not expose that limitation.
 */

/* This typed alias names the independently rooted 16-byte matrix at+40.
 * It allocates no storage; its original C identifier is unknown. Stock lacks
 * serialization around this shared matrix. TODO: locking is a separate fix.
 */
extern u8 alpu_aes_matrix[4][4];
__asm__(".local alpu_aes_matrix\n.type alpu_aes_matrix, %object\n"
        ".set alpu_aes_matrix, alpu_state+40\n.size alpu_aes_matrix,16\n");
/* Keep stock's repeated shared-matrix reads and writes, without locking.
 * TODO: serialize the cipher only as a separate behavior change.
 */

/* Stock copies the complete256-byte table to its private stack on
 * every lookup. Preserve that footprint and all copy accesses. TODO: a direct
 * const lookup is a separate behavior/performance change. The structure-copy
 * spelling reproduces the initializer's block copy without duplicate ROM.
 */
struct alpu_lookup_table { u8 byte[256]; } __attribute__((__may_alias__));


/* Typed alias only; stock does not validate this240-byte table's
 * index. Rounds15..32 consume neighboring known shared fields. TODO: validate
 * internal round arguments only as a separate behavior change. Later indices
 * leave the recovered pool and have only bounded-prefix verification.
 */
extern u8 alpu_round_key[240];
__asm__(".local alpu_round_key\n.type alpu_round_key, %object\n"
        ".set alpu_round_key, alpu_state+56\n.size alpu_round_key,240\n");

extern int alpu_aes_nr, alpu_aes_nk;
extern u8 alpu_cipher_key[32];
extern const u8 alpu_aes_rcon[11];
__asm__(".local alpu_aes_nr\n.type alpu_aes_nr, %object\n.set alpu_aes_nr, alpu_state\n.size alpu_aes_nr,4\n"
        ".local alpu_aes_nk\n.type alpu_aes_nk, %object\n.set alpu_aes_nk, alpu_state+4\n.size alpu_aes_nk,4\n"
        ".local alpu_cipher_key\n.type alpu_cipher_key, %object\n.set alpu_cipher_key, alpu_state+296\n.size alpu_cipher_key,32\n"
        ".local alpu_aes_rcon\n.type alpu_aes_rcon, %object\n.set alpu_aes_rcon, alpu_constants+512\n.size alpu_aes_rcon,11\n");

extern u8 alpu_aes_input[16], alpu_aes_output[16];
__asm__(".local alpu_aes_input\n.type alpu_aes_input,%object\n.set alpu_aes_input,alpu_state+8\n.size alpu_aes_input,16\n"
        ".local alpu_aes_output\n.type alpu_aes_output,%object\n.set alpu_aes_output,alpu_state+24\n.size alpu_aes_output,16\n");

/* Keep the observed native helper order; no padding or extra storage. */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static u8 DNS_0115_Func(u8 x)
{
 struct alpu_lookup_table copy = *(const struct alpu_lookup_table *)arelink_aes_isbox;
 return copy.byte[x];
}

__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static u8 DNS_0114_Func(u8 x)
{
 struct alpu_lookup_table copy = *(const struct alpu_lookup_table *)arelink_aes_sbox;
 return copy.byte[x];
}

/* Stock temporary bytes occupy a four-byte-aligned slot. This spelling
 * reproduces the observed placement; original padding/declaration is unknown.
 * Counters wrap at eight bits; Nr/Nk and table indices are unchecked. TODO:
 * validate only in a separate fix. No claim is made for C division by zero;
 * the pinned native code retains stock SDIV/MSUB behavior and no new guard.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0113_Func(void)
{
 u8 i,j,k;
 u8 tempa[4] __attribute__((aligned(4)));
 for(i=0;i<alpu_aes_nk;i++) {
  alpu_round_key[i*4+0]=alpu_cipher_key[i*4+0];
  alpu_round_key[i*4+1]=alpu_cipher_key[i*4+1];
  alpu_round_key[i*4+2]=alpu_cipher_key[i*4+2];
  alpu_round_key[i*4+3]=alpu_cipher_key[i*4+3];
 }
 for(;i<4*(alpu_aes_nr+1);i++) {
  for(j=0;j<4;j++) tempa[j]=alpu_round_key[(i-1)*4+j];
  if(i%alpu_aes_nk==0) {
   k=tempa[0];tempa[0]=tempa[1];tempa[1]=tempa[2];tempa[2]=tempa[3];tempa[3]=k;
   tempa[0]=DNS_0114_Func(tempa[0]);tempa[1]=DNS_0114_Func(tempa[1]);
   tempa[2]=DNS_0114_Func(tempa[2]);tempa[3]=DNS_0114_Func(tempa[3]);
   tempa[0]^=alpu_aes_rcon[i/alpu_aes_nk];
  } else if(alpu_aes_nk>6 && i%alpu_aes_nk==4) {
   tempa[0]=DNS_0114_Func(tempa[0]);tempa[1]=DNS_0114_Func(tempa[1]);
   tempa[2]=DNS_0114_Func(tempa[2]);tempa[3]=DNS_0114_Func(tempa[3]);
  }
  alpu_round_key[i*4+0]=alpu_round_key[(i-alpu_aes_nk)*4+0]^tempa[0];
  alpu_round_key[i*4+1]=alpu_round_key[(i-alpu_aes_nk)*4+1]^tempa[1];
  alpu_round_key[i*4+2]=alpu_round_key[(i-alpu_aes_nk)*4+2]^tempa[2];
  alpu_round_key[i*4+3]=alpu_round_key[(i-alpu_aes_nk)*4+3]^tempa[3];
 }
}

__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0112_Func(u8 round)
{
 u8 i,j;
 for(i=0;i<4;i++)
  for(j=0;j<4;j++)
   alpu_aes_matrix[j][i] ^= alpu_round_key[(round*4+i)*4+j];
}

__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0111_Func(void)
{
 u8 i,j;
 for(i=0;i<4;i++)
  for(j=0;j<4;j++)
   alpu_aes_matrix[i][j] = DNS_0114_Func(alpu_aes_matrix[i][j]);
}

__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0110_Func(void)
{
 u8 i,j;
 for(i=0;i<4;i++)
  for(j=0;j<4;j++)
   alpu_aes_matrix[i][j] = DNS_0115_Func(alpu_aes_matrix[i][j]);
}

/* Preserve immediate row stores and reuse of one temporary.
 * Hoisting reads changes stock accesses even when the final matrix agrees.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0109_Func(void)
{
 u8 t;
 t=alpu_aes_matrix[1][0];
 alpu_aes_matrix[1][0] = alpu_aes_matrix[1][1];
 alpu_aes_matrix[1][1] = alpu_aes_matrix[1][2];
 alpu_aes_matrix[1][2] = alpu_aes_matrix[1][3];
 alpu_aes_matrix[1][3] = t;
 t=alpu_aes_matrix[2][0];
 alpu_aes_matrix[2][0] = alpu_aes_matrix[2][2];
 alpu_aes_matrix[2][2] = t;
 t=alpu_aes_matrix[2][1];
 alpu_aes_matrix[2][1] = alpu_aes_matrix[2][3];
 alpu_aes_matrix[2][3] = t;
 t=alpu_aes_matrix[3][0];
 alpu_aes_matrix[3][0] = alpu_aes_matrix[3][3];
 alpu_aes_matrix[3][3] = alpu_aes_matrix[3][2];
 alpu_aes_matrix[3][2] = alpu_aes_matrix[3][1];
 alpu_aes_matrix[3][1] = t;
}

__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0108_Func(void)
{
 u8 t;
 t=alpu_aes_matrix[1][3];
 alpu_aes_matrix[1][3] = alpu_aes_matrix[1][2];
 alpu_aes_matrix[1][2] = alpu_aes_matrix[1][1];
 alpu_aes_matrix[1][1] = alpu_aes_matrix[1][0];
 alpu_aes_matrix[1][0] = t;
 t=alpu_aes_matrix[2][0];
 alpu_aes_matrix[2][0] = alpu_aes_matrix[2][2];
 alpu_aes_matrix[2][2] = t;
 t=alpu_aes_matrix[2][1];
 alpu_aes_matrix[2][1] = alpu_aes_matrix[2][3];
 alpu_aes_matrix[2][3] = t;
 t=alpu_aes_matrix[3][0];
 alpu_aes_matrix[3][0] = alpu_aes_matrix[3][1];
 alpu_aes_matrix[3][1] = alpu_aes_matrix[3][2];
 alpu_aes_matrix[3][2] = alpu_aes_matrix[3][3];
 alpu_aes_matrix[3][3] = t;
}

__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector"))) static u8 xtime(u8 x)
{
 return (x << 1) ^ (((x >> 7) & 1) * 0x1b);
}

__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector"))) static u8 Multiply(u8 x, u8 y)
{
 return ((y & 1) * x) ^ ((y >> 1 & 1) * xtime(x)) ^
        ((y >> 2 & 1) * xtime(xtime(x))) ^
        ((y >> 3 & 1) * xtime(xtime(xtime(x)))) ^
        ((y >> 4 & 1) * xtime(xtime(xtime(xtime(x)))));
}

__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0107_Func(void)
{
 u8 i, Tmp, Tm, t;
 for (i = 0; i < 4; i++) {
  t = alpu_aes_matrix[0][i];
  Tmp = alpu_aes_matrix[0][i] ^ alpu_aes_matrix[1][i] ^ alpu_aes_matrix[2][i] ^ alpu_aes_matrix[3][i];
  Tm = alpu_aes_matrix[0][i] ^ alpu_aes_matrix[1][i]; Tm = xtime(Tm);
  alpu_aes_matrix[0][i] ^= Tm ^ Tmp;
  Tm = alpu_aes_matrix[1][i] ^ alpu_aes_matrix[2][i]; Tm = xtime(Tm);
  alpu_aes_matrix[1][i] ^= Tm ^ Tmp;
  Tm = alpu_aes_matrix[2][i] ^ alpu_aes_matrix[3][i]; Tm = xtime(Tm);
  alpu_aes_matrix[2][i] ^= Tm ^ Tmp;
  Tm = alpu_aes_matrix[3][i] ^ t; Tm = xtime(Tm);
  alpu_aes_matrix[3][i] ^= Tm ^ Tmp;
 }
}

__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0106_Func(void)
{
 u8 i, a, b, c, d;
 for (i = 0; i < 4; i++) {
  a = alpu_aes_matrix[0][i];
  b = alpu_aes_matrix[1][i];
  c = alpu_aes_matrix[2][i];
  d = alpu_aes_matrix[3][i];
  alpu_aes_matrix[0][i] = Multiply(a,14) ^ Multiply(b,11) ^ Multiply(c,13) ^ Multiply(d,9);
  alpu_aes_matrix[1][i] = Multiply(a,9) ^ Multiply(b,14) ^ Multiply(c,11) ^ Multiply(d,13);
  alpu_aes_matrix[2][i] = Multiply(a,13) ^ Multiply(b,9) ^ Multiply(c,14) ^ Multiply(d,11);
  alpu_aes_matrix[3][i] = Multiply(a,11) ^ Multiply(b,13) ^ Multiply(c,9) ^ Multiply(d,14);
 }
}

/* Substitute before shifting in encryption. The operations commute only
 * numerically: shared/private access and helper-entry order remain observable.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0105_Func(void)
{
 /* Preserve the dead entry initialization and the byte round counter.
  * Corrupt Nr can wrap/underflow into neighboring fields or loop forever. */
 u8 i,j,round=0;
 for(i=0;i<4;i++) for(j=0;j<4;j++) alpu_aes_matrix[j][i]=alpu_aes_input[i*4+j];
 DNS_0112_Func(0);
 for(round=1;round<alpu_aes_nr;round++) {
  DNS_0111_Func(); DNS_0109_Func(); DNS_0107_Func(); DNS_0112_Func(round);
 }
 DNS_0111_Func(); DNS_0109_Func(); DNS_0112_Func(alpu_aes_nr);
 for(i=0;i<4;i++) for(j=0;j<4;j++) alpu_aes_output[i*4+j]=alpu_aes_matrix[j][i];
}

__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0104_Func(void)
{
 /* Preserve the dead entry initialization and the byte round counter.
  * Corrupt Nr can wrap/underflow into neighboring fields or loop forever. */
 u8 i,j,round=0;
 for(i=0;i<4;i++) for(j=0;j<4;j++) alpu_aes_matrix[j][i]=alpu_aes_input[i*4+j];
 DNS_0112_Func(alpu_aes_nr);
 for(round=alpu_aes_nr-1;round>0;round--) {
  DNS_0108_Func(); DNS_0110_Func(); DNS_0112_Func(round); DNS_0106_Func();
 }
 DNS_0108_Func(); DNS_0110_Func(); DNS_0112_Func(0);
 for(i=0;i<4;i++) for(j=0;j<4;j++) alpu_aes_output[i*4+j]=alpu_aes_matrix[j][i];
}

/* Stored selector bytes other than 0/1 select 256 bits, including values
 * rejected by the public setter. Preserve signed division and saved-pointer
 * advancement; replacing this with an unsigned shift changes native behavior.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
void DNS_0101_Func(u8 selector, const u8 *input)
{
 u8 i;
 if(selector==0) alpu_aes_nr=128;
 else if(selector==1) alpu_aes_nr=192;
 else alpu_aes_nr=256;
 alpu_aes_nk=alpu_aes_nr/32;
 alpu_aes_nr=alpu_aes_nk+6;
 for(i=0;i<alpu_aes_nk*4;i++) alpu_cipher_key[i]=*input++;
}

__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
void DNS_0102_Func(const u8 *input,u8 *output)
{
 u8 i;
 for(i=0;i<16;i++) alpu_aes_input[i]=*input++;
 DNS_0113_Func();
 DNS_0105_Func();
 for(i=0;i<16;i++) *output++=alpu_aes_output[i];
}

__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
void DNS_0103_Func(const u8 *input,u8 *output)
{
 u8 i;
 for(i=0;i<16;i++) alpu_aes_input[i]=*input++;
 DNS_0113_Func();
 DNS_0104_Func();
 for(i=0;i<16;i++) *output++=alpu_aes_output[i];
}

/* These private byte operations are also used before their definitions below.
 * Do not replace them with the kernel's wider/bulk memory implementations.
 */
static void *___memset(void *destination, int value, unsigned int count);
static void ___memcpy(void *destination, const void *source, u8 count);

/* Typed views of the independently rooted stock fields; no new storage. */
extern u8 alpu_key[32],alpu_mask[16],alpu_receive[72],alpu_protected;
extern u16 alpu_status;
__asm__(".local alpu_key\n.type alpu_key,%object\n.set alpu_key,alpu_state+552\n.size alpu_key,32\n"
        ".local alpu_mask\n.type alpu_mask,%object\n.set alpu_mask,alpu_state+536\n.size alpu_mask,16\n"
        ".local alpu_receive\n.type alpu_receive,%object\n.set alpu_receive,alpu_state+392\n.size alpu_receive,72\n"
        ".local alpu_protected\n.type alpu_protected,%object\n.set alpu_protected,alpu_state+530\n.size alpu_protected,1\n"
        ".local alpu_status\n.type alpu_status,%object\n.set alpu_status,alpu_initialized+8\n.size alpu_status,2\n");
/* Compatible frame and observed buffer slot; original C types/extents/flags
 * are unknown. Preserve all24 reads, stale-buffer completion, zero seeding,
 * optional forward serial copy and the unconditional success result.
 * Serial output is unchecked: aliases can overwrite the seed and key-size byte.
 * TODO: alias validation, transport errors, seed repair and serialization are separate fixes.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
u8 arelink_init(u8 *serial)
{
 u8 attempt=0;
 u8 decoded[5] __attribute__((aligned(8)));
 DNS_0002_Func(arelink_encoded_key,alpu_key,32,0xab36);
 DNS_0002_Func(arelink_encoded_mask,alpu_mask,16,0xab36);
 ___memset(alpu_receive,255,6);
 for(;;) {
  arelink_delay(1);
  arelink_read_data(0x5000,6,alpu_receive);
  if((alpu_receive[0]&15)>4 || ++attempt>23) break;
 }
 arelink_srand(alpu_receive[0]);
 DNS_0005_Func(alpu_receive,decoded,5);
 if(serial) ___memcpy(serial,decoded,5);
 ___memcpy(alpu_key,decoded,5);
 alpu_initialized_flag=1;
 /* Stock materializes zero, masks it to four bits, then truncates to a byte.
  * The unoptimized C mask preserves the native operation and byte store.
  * The original C spelling remains unknown.
  */
 { register int zero __asm__("w0")=0;
   zero &= 15;
   alpu_protected=zero;
 }
 alpu_status=0xffff;
 return 0;
}

/* Stock's private helpers truncate lengths to a byte and copy forwards even
 * for overlapping buffers. Preserve these effects and the unbounded NUL scan.
 * TODO: bounded lengths/overlap handling belong in a separate behavior fix.
 * Their unoptimized bodies and the getter match the recovered stock code.
 */
__attribute__((__used__, __noinline__, __optimize__("O0")))
static u8 ___strlen(const char *text)
{
	const char *end = text;
	while (*end++)
		;
	return (u8)(end - text) - 1;
}

/* Stock has no canary here. Keep its recovered private stack-slot order;
 * this attribute reproduces the body, not proven original compiler flags.
 */
__attribute__((__used__, __noinline__, __optimize__("O0", "no-stack-protector")))
static void ___memcpy(void *destination, const void *source, u8 count)
{
	u8 *dst = destination;
	const u8 *src = source;
	while (count--)
		*dst++ = *src++;
}

/* Stock compares unsigned bytes, left before right, and returns only -1/0/1.
 * A negative signed count skips all input reads. Preserve the private stack
 * slots as well; these declarations/attributes are compatible reconstructions.
 */
__attribute__((__used__, __noinline__, __optimize__("O0", "no-stack-protector")))
static int ___memcmp(const void *left, const void *right, int count)
{
	const unsigned char *a = left;
	const unsigned char *b = right;
	int i = 0;
	unsigned int x, y;
	for (; i < count; i++) {
		x = *a++;
		y = *b++;
		if (x != y)
			return x > y ? 1 : -1;
	}
	return 0;
}

/* Stock writes bytes backward and wraps its 32-bit counter, including zero's
 * final decrement. TODO: conventional bounds/order changes are separate fixes;
 * unsynchronized callers can observe the shipped individual byte stores.
 */
__attribute__((__used__, __noinline__, __optimize__("O0", "no-stack-protector")))
static void *___memset(void *destination, int value, unsigned int count)
{
	unsigned char *dst = destination;
	while (count--)
		dst[count] = value;
	return destination;
}

__attribute__((__noinline__, __optimize__("O0")))
u8 get_lib_version(u8 *output, u8 *length)
{
	if (!alpu_initialized_flag)
		return 2;
	/* Three separate pointer loads can observe intervening writers. Keep
	 * their order, including capturing the copy source before the second
	 * length calculation. Do not cache the pointer or add synchronization.
	 */
	*length = ___strlen(alpu_initialized.version);
	___memcpy(output, alpu_initialized.version,
		 ___strlen(alpu_initialized.version));
	return 0;
}

extern u8 alpu_key_size;
/* Compatible slots and frame capacities, not original declaration proof.
 * The setter reloads the shared selector after its store: a writer can change
 * the transmitted value, even beyond the checked0..2 range. Preserve that race.
 * TODO: locking and revalidation belong in a separate behavior change.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector"))) static void DNS_0013_Func(u8 selector)
{
 u8 nonce,mask;
 u8 frame[2] __attribute__((aligned(8)));
 nonce=arelink_random();
 mask=DNS_0004_Func(nonce);
 frame[0]=nonce;
 frame[1]=selector;
 DNS_0006_Func(frame+1,1,mask);
 arelink_write_data(0x5010,frame,2);
}
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector"))) u8 arelink_set_aes_key_size(u8 size)
{
 if(!alpu_initialized_flag) return 2;
 if(size>2) return 3;
 alpu_key_size=size;
 DNS_0013_Func(alpu_key_size);
 return 0;
}

/* Stock duplicates both polling branches, sends once before polling, and
 * clears receive only once. Short later reads retain old payload bytes.
 * It ignores transport status and leaves shared state unsynchronized.
 * TODO: fix those bugs separately from this compatible reconstruction.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0014_Func(const u8 *input,u8 decrypt,u8 reverse_input)
{
 u8 i,nonce,mask;
 u8 frame[18] __attribute__((aligned(8)));
 const u8 *cursor;
 if(reverse_input==1) {
  cursor=input+15;
  for(i=0;i<=15;i++) frame[i+1]=*cursor--;
 } else {
  for(i=0;i<=15;i++) frame[i+1]=*input++;
 }
 nonce=arelink_random();
 mask=DNS_0004_Func(nonce);
 frame[0]=nonce;
 frame[17]=decrypt;
 DNS_0006_Func(frame+1,17,mask);
 arelink_write_data(0x2000,frame,18);
}
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
u8 arelink_req_enc_dec(const u8 *input,u8 *output,u8 decrypt)
{
 u8 decoded[16] __attribute__((aligned(8)));
 u8 attempt=0;
 if(!alpu_initialized_flag) return 2;
 if(decrypt>1) return 3;
 ___memset(alpu_receive,0,17);
 if(decrypt==0) {
  DNS_0014_Func(input,0,1);
  for(;;) {
   arelink_delay(1);
   arelink_read_data(0x2000,17,alpu_receive);
   if(alpu_receive[0] || ++attempt>4) break;
  }
  DNS_0005_Func(alpu_receive,decoded,16);
  DNS_0007_Func(decoded,output,16);
  return 0;
 } else {
  DNS_0014_Func(input,1,1);
  for(;;) {
   arelink_delay(1);
   arelink_read_data(0x2000,17,alpu_receive);
   if(alpu_receive[0] || ++attempt>4) break;
  }
  DNS_0005_Func(alpu_receive,decoded,16);
  DNS_0007_Func(decoded,output,16);
  return 0;
 }
}

extern u8 alpu_key_size, alpu_key[32];
__asm__(".local alpu_key_size\n.type alpu_key_size,%object\n.set alpu_key_size,alpu_initialized+16\n.size alpu_key_size,1\n"
        ".local alpu_key\n.type alpu_key,%object\n.set alpu_key,alpu_state+552\n.size alpu_key,32\n");
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
u8 arelink_req_sw_enc_dec(const u8 *input,u8 *output,u8 decrypt)
{
 if(!alpu_initialized_flag) return 2;
 if(decrypt>1) return 3;
 {
  DNS_0101_Func(alpu_key_size,alpu_key);
  if(decrypt==0) { DNS_0102_Func(input,output);return 0; }
  else { DNS_0103_Func(input,output);return 0; }
 }
}

/* Stock tests initialization only once and ignores transport status. Buffers
 * and aliases remain unchecked. TODO: bounds/error/serialization fixes must
 * remain separate from this compatible native reconstruction.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector"))) u8 arelink_bypass_mode(const u8 *input,u8 *output)
{
 if(!alpu_initialized_flag) return 2;
 arelink_write_data(0x4000,input,16);
 arelink_delay(1);
 arelink_read_data(0x4000,16,output);
 return 0;
}

/* Stock tests initialization once; a busy response is accepted after five
 * reads. A short later read retains earlier receive bytes because the buffer
 * is cleared only once. Preserve unchecked aliases and ignored I/O status.
 * TODO: bounds, transport error and serialization fixes are separate changes.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0015_Func(const u8 *input,u8 *output)
{
 u8 i,nonce,mask;
 u8 frame[17] __attribute__((aligned(8)));
 const u8 *cursor;
 u8 attempt=0;
 ___memset(alpu_receive,0,16);
 cursor=input+15;
 for(i=0;i<=15;i++) frame[i+1]=*cursor--;
 nonce=arelink_random();
 mask=DNS_0004_Func(nonce);
 frame[0]=nonce;
 DNS_0006_Func(frame+1,16,mask);
 arelink_write_data(0,frame,17);
 arelink_delay(1);
 ___memset(alpu_receive,0,17);
 for(;;) {
  arelink_read_data(0,17,alpu_receive);
  if(alpu_receive[0] || ++attempt>4) break;
 }
 DNS_0005_Func(alpu_receive,frame,16);
 DNS_0007_Func(frame,output,16);
}
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
u8 arelink_req_cypher(const u8 *input,u8 *output)
{
 if(!alpu_initialized_flag) return 2;
 DNS_0015_Func(input,output);
 return 0;
}

extern u8 alpu_authenticated;
__asm__(".local alpu_authenticated\n.type alpu_authenticated,%object\n.set alpu_authenticated,alpu_state+584\n.size alpu_authenticated,1\n");
/* Compatible saved arguments, status byte and eight-byte-aligned buffer slots.
 * Original declarations/extents/flags remain unknown. Preserve dead status
 * initialization and the ignored key-size error. Zero seed leaves state alone.
 * TODO: validate the setter result and serialize shared state separately.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
u8 arelink_authentication(u8 size,u32 seed,const u8 *challenge)
{
 u8 status=0;
 u8 encrypted[16] __attribute__((aligned(8)));
 u8 response[16] __attribute__((aligned(8)));
 alpu_authenticated=0;
 if(seed) arelink_srand(seed);
 status=arelink_set_aes_key_size(size);
 status=arelink_req_sw_enc_dec(challenge,encrypted,0);
 if(status==0) {
  ___memset(response,0,16);
  arelink_delay(2);
  status=arelink_req_cypher(encrypted,response);
  if(status==0) {
   if((signed char)___memcmp(challenge,response,16)==0) alpu_authenticated=1;
  }
 }
 return alpu_authenticated;
}

/* Stock masks the index only after rejecting blocks above fourteen.
 * The protection test precedes the mask; keep this ordering.
 */
extern u8 alpu_protected_block;
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static u8 set_password(u8 block,u32 password)
{
 if(!alpu_initialized_flag) return 2;
 if(block>14) return 5;
 if(alpu_protected_block && block<alpu_protected_block+1) return 8;
 block &= 15;
 alpu_passwords[block]=password;
 return 0;
}

/* Stock polls six status samples per round and ignores transport return values.
 * Preserve the byte counters and signed halfword arithmetic. Stock
 * dereferences result without checking for NULL; fix separately.
 */
extern u16 alpu_block_status;
__asm__(".local alpu_block_status\n.type alpu_block_status,%object\n.set alpu_block_status,alpu_initialized+8\n.size alpu_block_status,2\n");
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static u8 check_block_auth_status(u8 block,u8 *result)
{
 u8 decoded[5],attempt=0,clear=0,set=0;
 if(!alpu_initialized_flag) return 2;
 if(block>14) return 5;
 if(alpu_protected_block && block<alpu_protected_block+1) return 8;
 block &= 15;
 ___memset(alpu_receive,255,3);
again_first:
 DNS_0008_Func(block);
 arelink_read_data(0x6010,3,alpu_receive);
 DNS_0005_Func(alpu_receive,decoded,2);
 alpu_block_status=(u16)(s16)((s16)(decoded[0]<<8)|(s16)decoded[1]);
 if((alpu_block_status>>block)&1) set++;
 else clear++;
 if(attempt++<=4) goto again_first;
 if(set>clear) *result=0;else *result=1;
 attempt=0;
again_second:
 DNS_0008_Func(block);
 arelink_read_data(0x6010,3,alpu_receive);
 DNS_0005_Func(alpu_receive,decoded,2);
 alpu_block_status=(u16)(s16)((s16)(decoded[0]<<8)|(s16)decoded[1]);
 if((alpu_block_status>>block)&1) {
  if(*result==0) goto done;
 } else {
  if(*result==1) goto done;
 }
 if(attempt++<=4) goto again_second;
done:
 return 0;
}

/* Stock sends up to six requests, but polls a busy response without a limit.
 * Preserve that nontermination; TODO: bound it in a separate behavior fix.
 * The unused tails of the aligned frames retain the stock stack layout;
 * their original C extents are unknown.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static u8 arelink_change_password(u8 block,u32 password)
{
 u8 frame[16] __attribute__((aligned(8)));
 u8 decoded[8] __attribute__((aligned(8)));
 u8 status,mask,nonce,attempt=0;
 if(!alpu_initialized_flag) return 2;
 if(block>14) return 5;
 if(alpu_protected_block && block<alpu_protected_block+1) return 8;
 nonce=arelink_random();
 mask=DNS_0004_Func(nonce);
 frame[0]=nonce;
 frame[1]=block;
 frame[2]=(u8)(password>>24);
 *((u8 *)&frame[2]+1)=(u8)(password>>16);
 *((u8 *)&frame[2]+2)=(u8)(password>>8);
 *((u8 *)&frame[2]+3)=(u8)password;
 DNS_0006_Func(frame+1,5,mask);
again:
 arelink_write_data(0x6020,frame,6);
 do {
  ___memset(receive_buffer,0,2);
  arelink_delay(2);
  arelink_read_data(0x6030,2,receive_buffer);
  DNS_0005_Func(receive_buffer,decoded,1);
  status=decoded[0];
 } while(status&0x10);
 if((s8)status<0) {
  if(attempt++<=4) goto again;
  nonce=arelink_random();
  mask=DNS_0004_Func(nonce);
  frame[0]=nonce;
  frame[1]=1;
  DNS_0006_Func(frame+1,1,mask);
  arelink_write_data(0x6040,frame,2);
  arelink_delay(20);
  return 9;
 }
 return 0;
}

/* Stock's private blank-page classifier uses byte counters and returns0
 * for empty input as well as all-zero/all0x49 pages. Preserve that policy and
 * its private frame; original declarations/compiler flags are unknown.
 * TODO: input-policy or bounds changes belong in a separate fix.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static u8 DNS_0003_Func(const u8 *input, u8 count)
{
 u8 zeros=0, erased=0, i=0, value;
 for (;i<count;i++) {
  value=*input++;
  if (value==0) zeros++;
  else if (value==0x49) erased++;
 }
 if (zeros==count) return 0;
 if (erased==count) return 0;
 return 1;
}

extern u8 alpu_protected_block,alpu_page_buffer[64];
__asm__(".local alpu_protected_block\n.type alpu_protected_block,%object\n.set alpu_protected_block,alpu_state+530\n.size alpu_protected_block,1\n" ".local alpu_page_buffer\n.type alpu_page_buffer,%object\n.set alpu_page_buffer,alpu_state+464\n.size alpu_page_buffer,64\n");
/* Stock clears the shared reply once and retries reads without resending.
 * Its byte progress counter can wrap for long signed lengths. Preserve both.
 * TODO: bounds and transport-status fixes belong in a separate patch.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0016_Func(const u8 *input,u8 *output,int length)
{
 u8 decoded[16] __attribute__((aligned(8)));
 u8 done,blocks,attempt;
 blocks=0;attempt=0;
 ___memset(alpu_receive,0,17);
 done=0;
 while(done<length) {
  blocks++;
  attempt=0;
  DNS_0014_Func(input,1,0);
  for(;;) {
   arelink_delay(1);
   arelink_read_data(0x2000,17,alpu_receive);
   if(alpu_receive[0] || ++attempt>4) break;
  }
  DNS_0005_Func(alpu_receive,decoded,16);
  DNS_0007_Func(decoded,output,16);
  input+=16;output+=16;done+=16;
 }
}

/* Stock clears receive only once before retrying and ignores read status.
 * Short reads can retain old payload bytes. Preserve for compatibility.
 * TODO: validate transport and serialize page state separately.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static u8 arelink_page_read(u8 page,u8 *data,u8 encrypted)
{
 u16 address;
 u8 attempt=0;
 if(!alpu_initialized_flag) return 2;
 if(page>29) return 4;
 if(alpu_protected_block && page<(alpu_protected_block+1)*2) return 7;
 encrypted &= 1;
 address=(page+0x200)*64;
 DNS_0009_Func(0,8);
 ___memset(alpu_receive,0,64);
 for(;;) {
  arelink_delay(1);
  arelink_read_data(address,65,alpu_receive);
  if(alpu_receive[0] || ++attempt>4) break;
 }
 DNS_0005_Func(alpu_receive,alpu_page_buffer,64);
 if(encrypted==1) {
  if(DNS_0003_Func(alpu_page_buffer,64)!=0)
   DNS_0016_Func(alpu_page_buffer,data,64);
  else ___memcpy(data,alpu_page_buffer,64);
 } else ___memcpy(data,alpu_page_buffer,64);
 return 0;
}

/* Stock setup writes one zero byte and ignores every transport status. */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0018_Func(u16 address)
{
 u8 zero __attribute__((aligned(8)));
 DNS_0009_Func(4,1);
 zero=0;
 arelink_write_data(0x7100,&zero,1);
 DNS_0010_Func(address,0);
 DNS_0011_Func(0);
 arelink_delay(10);
}

/* Stock uses a byte progress counter, which can wrap and keep long
 * transfers running indefinitely. Preserve it for compatibility.
 * TODO: fix the length/progress type in a behavior-changing patch.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static void DNS_0017_Func(const u8 *data,int length,u16 address,u8 encrypted)
{
 u8 frame[17] __attribute__((aligned(8)));
 u8 mask,nonce,j,done,blocks;
 const u8 *cursor;
 blocks=0;done=0;
 while(done<length) {
  blocks++;
  nonce=arelink_random();
  mask=DNS_0004_Func(nonce);
  frame[0]=nonce;
  if(encrypted==1) {
   cursor=data+15;
   j=0;
   while(j<=15) {frame[j+1]=*cursor--;j++;}
   data+=16;
  } else {
   j=0;
   while(j<=15) {frame[j+1]=*data++;j++;}
  }
  DNS_0006_Func(frame+1,16,mask);
  arelink_write_data(0x7100,frame,17);
  arelink_delay(1);
  frame[0]=address>>8;
  frame[1]=address;
  frame[2]=encrypted;
  arelink_write_data(0x7008,frame,3);
  address+=16;
  done+=16;
 }
 DNS_0011_Func(0);
 arelink_delay(15);
}

/* Stock validates initialization, page and protection before writing.
 * Transport statuses from setup and transfer are ignored. Preserve both.
 * TODO: validate transport errors in a separate behavior-changing patch.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
static u8 arelink_page_write(u8 page,const u8 *data,u8 encrypted)
{
 u16 address;
 if(!alpu_initialized_flag) return 2;
 if(page>29) return 4;
 if(alpu_protected_block && page<(alpu_protected_block+1)*2) return 7;
 encrypted &= 1;
 address=(page+0x200)*64;
 DNS_0018_Func(address);
 DNS_0009_Func(8,1);
 DNS_0017_Func(data,64,address,encrypted);
 return 0;
}

/* Stock checks authentication before the data pointer, ignores the
 * set_password return, and collapses auth-check failures to status 18.
 * TODO: validate password/block errors and serialize shared state in a
 * separate behavior-changing patch.
 */
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
u8 arelink_eeprom_write(u32 password, u8 page, const u8 *data, u8 encrypted)
{
 u8 result, block, status;
 result=0; status=0; block=page>>1;
 if(!alpu_authenticated) return 15;
 if(!data) return 12;
 set_password(block,password);
 status=check_block_auth_status(block,&result);
 if(status==0 && result==1) return arelink_page_write(page,data,encrypted);
 return 18;
}
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
u8 arelink_eeprom_read(u32 password, u8 page, u8 *data, u8 encrypted)
{
 u8 result, block, status;
 result=0; status=0; block=page>>1;
 if(!alpu_authenticated) return 15;
 if(!data) return 12;
 set_password(block,password);
 status=check_block_auth_status(block,&result);
 if(status==0 && result==1) return arelink_page_read(page,data,encrypted);
 return 18;
}
__attribute__((__used__,__noinline__,__optimize__("O0","no-stack-protector")))
u8 arelink_eeprom_pwchg(u8 page, u32 old_password, u32 new_password)
{
 u8 result, block, status;
 result=0; status=0; block=page>>1;
 if(!alpu_authenticated) return 15;
 set_password(block,old_password);
 status=check_block_auth_status(block,&result);
 if(status==0 && result==1) return arelink_change_password(block,new_password);
 return 18;
}
