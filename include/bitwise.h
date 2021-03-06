#pragma once

#include <stdbool.h>
#include <stdint.h>

#define ALIGN_UP_IMPL_2

/*
 * Align `value` to power-of-2 `align_to`, i.e. clear bits [0, align_to).
 *
 * This aligns _downwards_ so e.g.:
 *   align64(0b01`011, 0b01`000) == 0b01`000.
 */
static inline uint64_t align64(uint64_t value, uint64_t align_to)
{
	// A power-of-2 subtracted by 1 sets all lower bits. Complement
	// generates a mask of all other bits.
	return value & ~(align_to - 1);
}

/*
 * Align `value` to power-of-2 `align_to` rounding up to the next aligned value
 * (leaving the value unchanged if already aligned).
 *
 * This aligns _upwards_ so e.g.:
 *   align64_up(0b01`011, 0b01`000) == 0b10`000.
 */
static inline uint64_t align64_up(uint64_t value, uint64_t align_to)
{
	// By adding the lower bits of `align_to` we will be either simply go on
	// to clear these lower bits or we will clear the lower bits of `value +
	// align_to`.
#if defined(ALIGN_UP_IMPL_1)
	return align64(value + align_to - 1, align_to);
#elif defined(ALIGN_UP_IMPL_2)
	// We could alternatively achieve this by invoking two's complement
	// (complement then add 1) which will add `align_to - (value & (align_to
	// - 1))` except if the lower bits are zeroed, achieving the same thing.
	return value + (-value & (align_to - 1));
#else
#error align64_up() implementation not specified.
#endif
}

// Determine whether the specified bit is set in the specified value.
static inline bool is_bit_set(uint64_t value, uint64_t bit)
{
	return !!(value & (1UL << bit));
}

/*
 * Mask high bits up to and including `bit`.
 *
 * e.g. mask_high_bits(0b1110111, 3) == 0b1110000.
 */
static inline uint64_t mask_high_bits(uint64_t value, uint64_t bit)
{
	// Using two's complement: -0b01000 == 0b..10111 + 1 == 0b..11000.
	return value & -(1UL << bit);
}

// Clear the lowest bit of a value (or do nothing if zero).
static inline uint64_t clear_lowest_bit(uint64_t value)
{
	// Two's complement means this is equivalent to value & (~value + 1)
	// meaning the lowest _unset_ bit in the complemented version is flipped
	// which becomes the only bit that &'s with a set bit in `value`.
	const uint64_t lowest_bit_only = value & -value;

	return value - lowest_bit_only;
}

/*
 * Determine whether `a` and `b` share the highest bit.
 *
 * e.g.:
 *   share_highest_bit(0b1100, 0b1000) == true
 *   share_highest_bit(0b0100, 0b1000) == false
 */
static inline bool share_highest_bit(uint64_t a, uint64_t b)
{
	// The only way this can be true is if the highest bit is shared and
	// thus zeroed.
	return (a ^ b) <= b; // Should be equivalent to <= a also.
}

#undef ALIGN_UP_IMPL_1
#undef ALIGN_UP_IMPL_2
