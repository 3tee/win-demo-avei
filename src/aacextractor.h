#pragma once

#include <math.h>
#include <stdint.h>

// length : 28 bits
struct adts_fixed_header {
	uint16_t syncword : 12;
	uint8_t id : 1;
	uint8_t layer : 2;
	uint8_t protection_absent : 1;
	uint8_t profile : 2;
	uint8_t sampling_frequency_index : 4;
	uint8_t private_bit : 1;
	uint8_t channel_configuration : 3;
	uint8_t original_copy : 1;
	uint8_t home : 1;
};

// length : 28 bits
struct adts_variable_header {
	uint8_t copyright_identification_bit : 1;
	uint8_t copyright_identification_start : 1;
	uint16_t aac_frame_length : 13;
	uint16_t adts_buffer_fullness : 11;
	uint8_t number_of_raw_data_blocks_in_frame : 2;
};


class AACExtractor
{
public:
	AACExtractor(const uint8_t *buf, size_t len = 7);
	~AACExtractor();

	uint16_t dataLen() const;
	bool hasCrc() const;

private:
	void get_fixed_header();
	void get_variable_header();

private:
	const uint8_t* m_buf;
	size_t m_len;
	adts_fixed_header m_fixHeader;
	adts_variable_header m_varHeader;
};
