//---------------------------------------------------------------
//
// Parser.cpp
//

#include "NetworkMessageParser.h"
#include "Log.h"
#include "NetworkTypes.h"

#include <algorithm>


namespace Common {

//===============================================================================

namespace {
	const uint32_t s_headerSize = sizeof(MessageHeader);

	// Increase if needed, but most streams should be smaller than this.
	const int32_t s_defaultReservedBytes = 512;
}

NetworkMessageParser::NetworkMessageParser()
	: m_activeBuffer(&m_firstBuffer)
{
	m_firstBuffer.reserve(s_defaultReservedBytes);
	m_secondBuffer.reserve(s_defaultReservedBytes);
}

NetworkMessageParser::~NetworkMessageParser()
{

}

void NetworkMessageParser::ExtractMessages(const std::string& stream,
	std::vector<Common::NetworkMessage>& messages)
{
	while (m_totalBytesParsed < stream.size())
	{
		// Order matters here. Do not change the order. State can transition while being handled.
		if (m_state == State::Header)
		{
			ParseHeader(stream);
		}
		if (m_state == State::Content)
		{
			ParseContent(stream);
		}
		if (m_state == State::Finalizing)
		{
			messages.push_back(std::move(*m_messageCache.get()));
			m_state = State::Header;
		}
	}

	// We're done with this stream.
	m_totalBytesParsed = 0;
}

void NetworkMessageParser::ParseHeader(const std::string& stream)
{
	// Stream size might be smaller than header size. In which case, we're waiting for more bits.
	uint32_t bytesRemaining = stream.size() - m_totalBytesParsed;
	if (bytesRemaining == 0)
		return;

	uint32_t sizeToCopy = std::min(bytesRemaining, s_headerSize);

	// If we have more data than is left in this message, cut it off and handle it in another state.
	if (sizeToCopy + m_bytesParsedThisState > s_headerSize)
	{
		sizeToCopy = s_headerSize - m_bytesParsedThisState;
	}
	WriteToBuffer(stream.substr(m_totalBytesParsed, sizeToCopy), sizeToCopy);

	if (m_bytesParsedThisState == s_headerSize)
	{
		std::memcpy(&m_messageCache->header, &(*Buffer())[0], s_headerSize);
		SwapBuffer();

		m_bytesParsedThisState = 0;
		// Done parsing the header, let's get the content.
		m_state = State::Content;
	}

}

void NetworkMessageParser::ParseContent(const std::string& stream)
{
	// Total bytes left for this state to handle.
	uint32_t bytesRemaining = stream.size() - m_totalBytesParsed;
	if (bytesRemaining == 0)
		return;

	uint32_t sizeToCopy = std::min(bytesRemaining, m_messageCache->header.messageLength);

	// If we have more data than is left in this message, cut it off and handle it in another state.
	if (sizeToCopy + m_bytesParsedThisState > m_messageCache->header.messageLength)
	{
		sizeToCopy = m_messageCache->header.messageLength - m_bytesParsedThisState;
	}
	WriteToBuffer(stream.substr(m_totalBytesParsed, sizeToCopy), sizeToCopy);

	if (m_bytesParsedThisState == m_messageCache->header.messageLength)
	{
		m_messageCache->messageData = std::move(*Buffer());
		SwapBuffer();
		m_bytesParsedThisState = 0;
		// Done parsing content, let's package it up.
		m_state = State::Finalizing;
	}
}

void NetworkMessageParser::SwapBuffer()
{
	Buffer()->clear();
	
	if (m_activeBuffer == &m_firstBuffer)
	{
		m_activeBuffer = &m_secondBuffer;
	}
	else
	{
		m_activeBuffer = &m_firstBuffer;
	}
}

void NetworkMessageParser::WriteToBuffer(const std::string& data, int size)
{
	Buffer()->append(data);
	m_bytesParsedThisState += data.size();
	m_totalBytesParsed += data.size();
}

//===============================================================================

} // namespace Tests
