//---------------------------------------------------------------
//
// Parser.cpp
//

#pragma once

#include <memory>
#include <vector>
#include <string>

namespace Common {

//===============================================================================

struct MessageHeader;
struct NetworkMessage;
class NetworkMessageParser
{
public:
	NetworkMessageParser();
	~NetworkMessageParser();

	// Will attempt to extract NetworkMessages out of a stream.
	void ExtractMessages(const std::string& stream, std::vector<NetworkMessage>& messages);

private:
	void ParseHeader(const std::string& stream);

	// We'd better have a valid header before trying to parse content. Recipient is responsible for validating
	// content.
	void ParseContent(const std::string& stream);

	void SwapBuffer();
	void WriteToBuffer(const std::string& data, int size);
	std::string* Buffer() { return m_activeBuffer; }

private:
	enum class State : uint32_t
	{
		Header,
		Content,
		Finalizing
	};

	// Local cache of the header we're looking for.
	std::unique_ptr<NetworkMessage> m_messageCache = std::make_unique<NetworkMessage>();

	// To ensure smooth writes, buffers are swapped after successful header or content reads.
	std::string m_firstBuffer;
	std::string m_secondBuffer;

	// Points to whichever buffer is being written to.
	std::string* m_activeBuffer;

	// We're either parsing a header, or content.
	State m_state = State::Header;

	// Resets on state change.
	uint32_t m_bytesParsedThisState = 0;

	// Keep track of where we are in the stream overall.
	uint32_t m_totalBytesParsed = 0;
};

//===============================================================================

} // namespace Tests
