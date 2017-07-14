#include <memory.h>
#include "aacextractor.h"

AACExtractor::AACExtractor(const uint8_t *buf, size_t len)
 : m_buf(buf),
   m_len(len)
{
	if (len < 7)
		return;

	memset(&m_fixHeader, 0, sizeof(m_fixHeader));
	memset(&m_varHeader, 0, sizeof(m_varHeader));
	get_fixed_header();
	get_variable_header();
}

AACExtractor::~AACExtractor()
{
}

uint16_t AACExtractor::dataLen() const
{
	return m_varHeader.aac_frame_length - (m_fixHeader.protection_absent ? 7 : 9);
}

bool AACExtractor::hasCrc() const
{
	return !m_fixHeader.protection_absent;
}

void AACExtractor::get_fixed_header() {
	uint64_t adts = 0;
	const uint8_t *p = m_buf;
	adts |= *p++; adts <<= 8;
	adts |= *p++; adts <<= 8;
	adts |= *p++; adts <<= 8;
	adts |= *p++; adts <<= 8;
	adts |= *p++; adts <<= 8;
	adts |= *p++; adts <<= 8;
	adts |= *p++;

	m_fixHeader.syncword = (adts >> 44);
	m_fixHeader.id = (adts >> 43) & 0x01;
	m_fixHeader.layer = (adts >> 41) & 0x03;
	m_fixHeader.protection_absent = (adts >> 40) & 0x01;
	m_fixHeader.profile = (adts >> 38) & 0x03;
	m_fixHeader.sampling_frequency_index = (adts >> 34) & 0x0f;
	m_fixHeader.private_bit = (adts >> 33) & 0x01;
	m_fixHeader.channel_configuration = (adts >> 30) & 0x07;
	m_fixHeader.original_copy = (adts >> 29) & 0x01;
	m_fixHeader.home = (adts >> 28) & 0x01;
}

void AACExtractor::get_variable_header() {
	uint64_t adts = 0;
	adts = m_buf[0]; adts <<= 8;
	adts |= m_buf[1]; adts <<= 8;
	adts |= m_buf[2]; adts <<= 8;
	adts |= m_buf[3]; adts <<= 8;
	adts |= m_buf[4]; adts <<= 8;
	adts |= m_buf[5]; adts <<= 8;
	adts |= m_buf[6];

	m_varHeader.copyright_identification_bit = (adts >> 27) & 0x01;
	m_varHeader.copyright_identification_start = (adts >> 26) & 0x01;
	m_varHeader.aac_frame_length = (adts >> 13) & ((int)pow((double)2, (int)14) - 1);
	m_varHeader.adts_buffer_fullness = (adts >> 2) & (int)(pow((double)2, (int)11) - 1);
	m_varHeader.number_of_raw_data_blocks_in_frame = adts & 0x03;
}
