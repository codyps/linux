// SPDX-License-Identifier: GPL-2.0-only
/*
 * Reconstruction of the TG3040 #860 Arelink protocol and software cipher.
 * Firmware pseudocode and constant addresses are recorded in the workbench.
 * The original DNS_01xx names implement the ordinary AES block transform;
 * the DNS_00xx routines implement the device framing and EEPROM protocol.
 */
#include "arelink.h"
#include "arelink-tables.h"

/* Stock initialized state: keep its order and 16-byte pool alignment.
 * Original C declarations are unknown; the two bytes at2..3 are padding.
 * Invalid stored key sizes and zero PRNG seeds retain the documented behavior.
 */
static struct {
    u16 block_status;
    u8 reserved_2[2];
    volatile u32 random_state;
    u8 key_size;
} __attribute__((packed)) alpu_initialized __attribute__((aligned(16))) = {
    .block_status = 0xffff, .random_state = 0xa37f1da6, .key_size = 2,
};
#define random_state alpu_initialized.random_state
#define key_size alpu_initialized.key_size
#define block_status alpu_initialized.block_status
/* Local aliases retain the independently decoded semantic reference names.
 * The zero-offset ELF alias inherits the pool extent; its live field is u16.
 */
__asm__(".local block_status\n.type block_status, %object\n.set block_status, alpu_initialized\n.size block_status, 2\n"
        ".local random_state\n.type random_state, %object\n.set random_state, alpu_initialized+4\n.size random_state, 4\n"
        ".local key_size\n.type key_size, %object\n.set key_size, alpu_initialized+8\n.size key_size, 1\n");
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


u8 arelink_random(void)
{
	u8 result;
	u32 current, shifted;
	/* Each stock stage reads the unshifted value first, then reads the
	 * shifted operand again. Other writers can intervene: preserve those
	 * mixed-value races and the final fresh word read for the returned byte.
	 * Without another writer, zero is absorbing and stock spins here.
	 * TODO: synchronization or seed repair is a separate behavior change.
	 */
	do {
		current = random_state;
		shifted = random_state << 13;
		random_state = current ^ shifted;
		current = random_state;
		shifted = random_state >> 17;
		random_state = current ^ shifted;
		current = random_state;
		shifted = random_state << 5;
		random_state = current ^ shifted;
		result = random_state;
		result += result / 255;
	} while (!result);
	return result;
}

void arelink_srand(u32 seed)
{
	random_state = seed;
}

/* DNS_0002: feedback decoding, with 16-bit wrap at every iteration. */
static void decode_constants(const u8 *input, u8 *output, u8 count)
{
	u16 state = 0xab36;
	unsigned int i;
	for (i = 0; i < count; i++) {
		output[i] = input[i] ^ (state >> 8);
		state = (input[i] + state) * -0x3193 + 0x58bf;
	}
}

/* DNS_0004: gather selected bits from two nibbles into a framing mask. */
static u8 frame_mask(u8 nonce)
{
	u8 sum = key[nonce & 15] + (nonce >> 4);
	u8 selectors[2] = { mask_table[sum >> 4], mask_table[sum & 15] };
	u8 inputs[2] = { sum, nonce }, result[2] = { 0, 0 };
	unsigned int i, bit, out;
	for (i = 0; i < 2; i++) {
		out = 0;
		for (bit = 0; bit < 8; bit++)
			if (selectors[i] & (1 << bit)) {
				result[i] |= ((inputs[i] >> bit) & 1) << out;
				out++;
			}
	}
	return (result[0] << 4) | result[1];
}

static void unpack(const u8 *input, u8 *output, unsigned int count)
{
	u8 mask = input[0] ? frame_mask(input[0]) : 0;
	unsigned int i;
	for (i = 0; i < count; i++)
		output[i] = input[i + 1] ^ mask;
}

static void reverse(const u8 *input, u8 *output, unsigned int count)
{
	unsigned int i;
	for (i = 0; i < count; i++)
		output[i] = input[count - 1 - i];
}

static void send_frame(u16 address, u8 *frame, unsigned int count)
{
	u8 mask;
	unsigned int i;
	frame[0] = arelink_random();
	mask = frame_mask(frame[0]);
	for (i = 1; i <= count; i++)
		frame[i] ^= mask;
	arelink_write_data(address, frame, count + 1);
}

static void command(u8 mode, u8 operation)
{
	u8 data[] = { 0x80, 0xc3, 0x68, mode, operation };
	arelink_write_data(0x7000, data, sizeof(data));
}

static void select_address(u16 address, u8 operation)
{
	u8 data[] = { address >> 8, address, operation };
	arelink_write_data(0x7008, data, sizeof(data));
}

static void finish(void)
{
	u8 zero = 0;
	arelink_write_data(0x7004, &zero, 1);
}

static void cipher_request(const u8 *input, u8 decrypt, u8 reverse_input)
{
	u8 frame[18];
	if (reverse_input)
		reverse(input, frame + 1, 16);
	else
		memcpy(frame + 1, input, 16);
	frame[17] = decrypt;
	send_frame(0x2000, frame, 17);
}

static u8 xtime(u8 x)
{
	return (x << 1) ^ ((x >> 7) * 0x1b);
}

static u8 multiply(u8 x, u8 y)
{
	u8 result = 0;
	while (y) {
		if (y & 1)
			result ^= x;
		x = xtime(x);
		y >>= 1;
	}
	return result;
}

static void aes_block(const u8 *input, u8 *output, u8 decrypt)
{
	u8 shifted[16], temp[4];
	/* DNS_0101 selects 256 bits for every stored byte other than 0 or 1,
	 * even though the public setter rejects bytes above 2. Keep that fallback.
	 */
	unsigned int nk = key_size == 0 ? 4 : key_size == 1 ? 6 : 8;
	unsigned int rounds = nk + 6;
	unsigned int i, j, round, row, column, source;
	u8 *expanded = alpu_state.expanded;
#define STATE(c, r) alpu_state.aes_state[(r) * 4 + (c)]
	alpu_state.rounds = rounds;
	alpu_state.nk = nk;
	for (i = 0; i < nk * 4; i++)
		alpu_state.cipher_key[i] = key[i];
	for (i = 0; i < 16; i++)
		alpu_state.aes_input[i] = input[i];
	memcpy(expanded, alpu_state.cipher_key, nk * 4);
	for (i = nk; i < (rounds + 1) * 4; i++) {
		memcpy(temp, expanded + (i - 1) * 4, 4);
		if (!(i % nk)) {
			u8 first = temp[0];
			for (j = 0; j < 3; j++)
				temp[j] = arelink_aes_sbox[temp[j + 1]];
			temp[3] = arelink_aes_sbox[first];
			temp[0] ^= arelink_aes_rcon[i / nk];
		} else if (nk > 6 && i % nk == 4) {
			for (j = 0; j < 4; j++)
				temp[j] = arelink_aes_sbox[temp[j]];
		}
		for (j = 0; j < 4; j++)
			expanded[i * 4 + j] = expanded[(i - nk) * 4 + j] ^ temp[j];
	}
	/* The stock library transposes its internal state; external blocks use
	 * standard AES column order. */
	for (column = 0; column < 4; column++)
		for (row = 0; row < 4; row++)
			STATE(column, row) = alpu_state.aes_input[column * 4 + row];
	for (round = 0; round <= rounds; round++) {
		unsigned int key_round = decrypt ? rounds - round : round;
		if (round) {
			for (row = 0; row < 4; row++)
				for (column = 0; column < 4; column++) {
					source = (column + (decrypt ? 4 - row : row)) & 3;
					shifted[column * 4 + row] = STATE(source, row);
				}
			for (i = 0; i < 16; i++)
				STATE(i / 4, i % 4) = decrypt ? arelink_aes_isbox[shifted[i]] :
					arelink_aes_sbox[shifted[i]];
			if (!decrypt && round < rounds)
				for (column = 0; column < 4; column++) {
					u8 total = STATE(column, 0) ^ STATE(column, 1) ^
						STATE(column, 2) ^ STATE(column, 3);
					u8 first = STATE(column, 0);
					for (i = 0; i < 3; i++)
						STATE(column, i) ^= total ^ xtime(STATE(column, i) ^ STATE(column, i + 1));
					STATE(column, 3) ^= total ^ xtime(STATE(column, 3) ^ first);
				}
		}
		for (i = 0; i < 16; i++)
			STATE(i / 4, i % 4) ^= expanded[key_round * 16 + i];
		if (decrypt && round && round < rounds)
			for (column = 0; column < 4; column++) {
				for (i = 0; i < 4; i++)
					temp[i] = STATE(column, i);
				for (i = 0; i < 4; i++)
					STATE(column, i) = multiply(temp[i], 14) ^
						multiply(temp[(i + 1) & 3], 11) ^
						multiply(temp[(i + 2) & 3], 13) ^
						multiply(temp[(i + 3) & 3], 9);
			}
	}
	for (column = 0; column < 4; column++)
		for (row = 0; row < 4; row++)
			alpu_state.aes_output[column * 4 + row] = STATE(column, row);
	for (i = 0; i < 16; i++)
		output[i] = alpu_state.aes_output[i];
#undef STATE
}

u8 arelink_init(u8 *serial)
{
	u8 decoded[5];
	unsigned int attempt;
	decode_constants(arelink_encoded_key, key, 32);
	decode_constants(arelink_encoded_mask, mask_table, 16);
	memset(receive_buffer, 255, 6);
	for (attempt = 0; attempt < 24; attempt++) {
		arelink_delay(1);
		arelink_read_data(0x5000, 6, receive_buffer);
		if ((receive_buffer[0] & 15) > 4)
			break;
	}
	arelink_srand(receive_buffer[0]);
	unpack(receive_buffer, decoded, 5);
	if (serial)
		memcpy(serial, decoded, 5);
	memcpy(key, decoded, 5);
	initialized = 1;
	protected_block = 0;
	block_status = 0xffff;
	return 0;
}

u8 get_lib_version(u8 *output, u8 *length)
{
	if (!initialized)
		return 2;
	*length = strlen(arelink_lib_version);
	memcpy(output, arelink_lib_version, *length);
	return 0;
}

u8 arelink_set_aes_key_size(u8 size)
{
	u8 frame[2];
	if (!initialized)
		return 2;
	if (size > 2)
		return 3;
	key_size = size;
	frame[1] = size;
	send_frame(0x5010, frame, 1);
	return 0;
}

u8 arelink_req_enc_dec(const u8 *input, u8 *output, u8 decrypt)
{
	u8 decoded[16];
	unsigned int attempt;
	if (!initialized)
		return 2;
	if (decrypt > 1)
		return 3;
	memset(receive_buffer, 0, 17);
	cipher_request(input, decrypt, 1);
	for (attempt = 0; attempt < 5; attempt++) {
		arelink_delay(1);
		arelink_read_data(0x2000, 17, receive_buffer);
		if (receive_buffer[0])
			break;
	}
	unpack(receive_buffer, decoded, 16);
	reverse(decoded, output, 16);
	return 0;
}

u8 arelink_req_sw_enc_dec(const u8 *input, u8 *output, u8 decrypt)
{
	if (!initialized)
		return 2;
	if (decrypt > 1)
		return 3;
	aes_block(input, output, decrypt);
	return 0;
}

u8 arelink_bypass_mode(const u8 *input, u8 *output)
{
	if (!initialized)
		return 2;
	arelink_write_data(0x4000, input, 16);
	arelink_delay(1);
	arelink_read_data(0x4000, 16, output);
	return 0;
}

u8 arelink_req_cypher(const u8 *input, u8 *output)
{
	u8 frame[17], decoded[16];
	unsigned int attempt;
	if (!initialized)
		return 2;
	memset(receive_buffer, 0, 16);
	reverse(input, frame + 1, 16);
	send_frame(0, frame, 16);
	arelink_delay(1);
	memset(receive_buffer, 0, 17);
	for (attempt = 0; attempt < 5; attempt++) {
		arelink_read_data(0, 17, receive_buffer);
		if (receive_buffer[0])
			break;
	}
	unpack(receive_buffer, decoded, 16);
	reverse(decoded, output, 16);
	return 0;
}

u8 arelink_authentication(u8 size, u32 seed, const u8 *challenge)
{
	u8 encrypted[16], response[16];
	authenticated = 0;
	if (seed)
		arelink_srand(seed);
	arelink_set_aes_key_size(size);
	if (!arelink_req_sw_enc_dec(challenge, encrypted, 0)) {
		memset(response, 0, 16);
		arelink_delay(2);
		if (!arelink_req_cypher(encrypted, response) &&
		    !memcmp(challenge, response, 16))
			authenticated = 1;
	}
	return authenticated;
}

static u8 set_password(u8 block, u32 password)
{
	if (!initialized)
		return 2;
	if (block >= 15)
		return 5;
	if (protected_block && block < protected_block + 1)
		return 8;
	passwords[block] = password;
	return 0;
}

static void password_frame(u8 *frame, u8 block, u32 password)
{
	frame[1] = block;
	frame[2] = password >> 24;
	frame[3] = password >> 16;
	frame[4] = password >> 8;
	frame[5] = password;
}

static void read_block_status(u8 block)
{
	u8 frame[6], decoded[2];
	password_frame(frame, block, passwords[block]);
	send_frame(0x6000, frame, 5);
	arelink_read_data(0x6010, 3, receive_buffer);
	unpack(receive_buffer, decoded, 2);
	block_status = (decoded[0] << 8) | decoded[1];
}

static u8 check_block_auth_status(u8 block, u8 *result)
{
	unsigned int i, clear = 0, set = 0;
	if (!initialized)
		return 2;
	if (block >= 15)
		return 5;
	if (protected_block && block < protected_block + 1)
		return 8;
	memset(receive_buffer, 255, 3);
	for (i = 0; i < 6; i++) {
		read_block_status(block & 15);
		if (block_status & (1 << block))
			set++;
		else
			clear++;
	}
	*result = clear < set ? 0 : 1;
	for (i = 0; i < 6; i++) {
		read_block_status(block & 15);
		if (!!(block_status & (1 << block)) != *result)
			break;
	}
	return 0;
}

static u8 arelink_change_password(u8 block, u32 password)
{
	u8 frame[6], status;
	unsigned int attempt;
	if (!initialized)
		return 2;
	if (block >= 15)
		return 5;
	if (protected_block && block < protected_block + 1)
		return 8;
	password_frame(frame, block, password);
	/* One random mask is reused across retries in the stock protocol. */
	frame[0] = arelink_random();
	for (attempt = 1; attempt < 6; attempt++)
		frame[attempt] ^= frame_mask(frame[0]);
	for (attempt = 0; attempt < 6; attempt++) {
		arelink_write_data(0x6020, frame, 6);
		do {
			memset(receive_buffer, 0, 2);
			arelink_delay(2);
			arelink_read_data(0x6030, 2, receive_buffer);
			unpack(receive_buffer, &status, 1);
		} while (status & 0x10);
		if (!(status & 0x80))
			return 0;
	}
	frame[1] = 1;
	send_frame(0x6040, frame, 1);
	arelink_delay(20);
	return 9;
}

static u8 arelink_page_read(u8 page, u8 *data, u8 encrypted)
{
	u8 decoded[16];
	unsigned int attempt, i, zeros = 0, erased = 0;
	if (!initialized)
		return 2;
	if (page >= 30)
		return 4;
	if (protected_block && page < (protected_block + 1) * 2)
		return 7;
	command(0, 8);
	memset(receive_buffer, 0, 64);
	for (attempt = 0; attempt < 5; attempt++) {
		arelink_delay(1);
		arelink_read_data((page + 0x200) * 64, 65, receive_buffer);
		if (receive_buffer[0])
			break;
	}
	unpack(receive_buffer, page_buffer, 64);
	for (i = 0; i < 64; i++) {
		zeros += page_buffer[i] == 0;
		erased += page_buffer[i] == 0x49;
	}
	if (!(encrypted & 1) || zeros == 64 || erased == 64) {
		memcpy(data, page_buffer, 64);
		return 0;
	}
	memset(receive_buffer, 0, 17);
	for (i = 0; i < 64; i += 16) {
		cipher_request(page_buffer + i, 1, 0);
		for (attempt = 0; attempt < 5; attempt++) {
			arelink_delay(1);
			arelink_read_data(0x2000, 17, receive_buffer);
			if (receive_buffer[0])
				break;
		}
		unpack(receive_buffer, decoded, 16);
		reverse(decoded, data + i, 16);
	}
	return 0;
}

static u8 arelink_page_write(u8 page, const u8 *data, u8 encrypted)
{
	u8 frame[17], zero = 0;
	u16 address = (page + 0x200) * 64;
	unsigned int i;
	if (!initialized)
		return 2;
	if (page >= 30)
		return 4;
	if (protected_block && page < (protected_block + 1) * 2)
		return 7;
	command(4, 1);
	arelink_write_data(0x7100, &zero, 1);
	select_address(address, 0);
	finish();
	arelink_delay(10);
	command(8, 1);
	for (i = 0; i < 64; i += 16) {
		if (encrypted & 1)
			reverse(data + i, frame + 1, 16);
		else
			memcpy(frame + 1, data + i, 16);
		send_frame(0x7100, frame, 16);
		arelink_delay(1);
		select_address(address + i, encrypted & 1);
	}
	finish();
	arelink_delay(15);
	return 0;
}

u8 arelink_eeprom_write(u32 password, u8 page, const u8 *data, u8 encrypted)
{
	u8 result = 0;
	if (!authenticated)
		return 15;
	if (!data)
		return 12;
	set_password(page >> 1, password);
	if (check_block_auth_status(page >> 1, &result) || result != 1)
		return 18;
	return arelink_page_write(page, data, encrypted);
}

u8 arelink_eeprom_read(u32 password, u8 page, u8 *data, u8 encrypted)
{
	u8 result = 0;
	if (!authenticated)
		return 15;
	if (!data)
		return 12;
	set_password(page >> 1, password);
	if (check_block_auth_status(page >> 1, &result) || result != 1)
		return 18;
	return arelink_page_read(page, data, encrypted);
}

u8 arelink_eeprom_pwchg(u8 page, u32 old_password, u32 new_password)
{
	u8 result = 0;
	if (!authenticated)
		return 15;
	set_password(page >> 1, old_password);
	if (check_block_auth_status(page >> 1, &result) || result != 1)
		return 18;
	return arelink_change_password(page >> 1, new_password);
}
