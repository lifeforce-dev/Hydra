//---------------------------------------------------------------
//
// Parser.cpp
//

#include "common/NetworkMessageParser.h"
#include "common/Log.h"
#include "common/NetworkTypes.h"

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
		if (m_state == State::Header || m_state == State::Content)
		{
			Parse(stream);
		}
		if (m_state == State::Finalizing)
		{
			messages.push_back(std::move(*m_messageCache.get()));
			TransitionState();
		}
	}

	// We're done with this stream.
	m_totalBytesParsed = 0;
}


void NetworkMessageParser::Parse(const std::string& stream)
{
	uint32_t bytesRemaining = stream.size() - m_totalBytesParsed;
	if (bytesRemaining == 0)
		return;

	uint32_t targetSize = m_state == State::Header ? s_headerSize : m_messageCache->header.messageLength;
	uint32_t sizeToCopy = std::min(bytesRemaining, targetSize);
	// If we have more data than is left in this message, cut it off and handle it in another state.
	if (sizeToCopy + m_bytesParsedThisState > targetSize)
	{
		sizeToCopy = targetSize - m_bytesParsedThisState;
	}

	WriteToBuffer(stream.substr(m_totalBytesParsed, sizeToCopy), sizeToCopy);
	if (m_bytesParsedThisState == targetSize)
	{

		if (m_state == State::Header)
		{
			std::memcpy(&m_messageCache->header, &(*Buffer())[0], targetSize);
		}
		else if (m_state == State::Content)
		{
			m_messageCache->messageData = std::move(*Buffer());
		}

		SwapBuffer();
		TransitionState();
		m_bytesParsedThisState = 0;
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

void NetworkMessageParser::TransitionState()
{
	if (m_state == State::Header)
		m_state = State::Content;
	else if (m_state == State::Content)
		m_state = State::Finalizing;
	else if (m_state == State::Finalizing)
		m_state = State::Header;
}

void NetworkMessageParser::WriteToBuffer(const std::string& data, int size)
{
	Buffer()->append(data);
	m_bytesParsedThisState += data.size();
	m_totalBytesParsed += data.size();
}

//===============================================================================

} // namespace Tests
