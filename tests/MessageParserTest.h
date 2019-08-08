//---------------------------------------------------------------
//
// MessageParserTest.h
//

#pragma once

#include "common/NetworkMessageParser.h"
#include "common/NetworkTypes.h"
#include "proto/TestMessage.pb.h"

#include <string>
#include <vector>
#include <google/protobuf/util/message_differencer.h>

namespace Tests {
//===============================================================================

struct HelperFixture {

	HelperFixture()
		: networkParser(std::make_unique<Common::NetworkMessageParser>())
	{}

	void ExtractMessages(const std::string& data, std::vector<Common::NetworkMessage>& messages)
	{
		networkParser->ExtractMessages(data, messages);
	}

	std::string PackageMessage(const std::string& messageData, Common::MessageId id)
	{
		std::string buffer;
		Common::MessageHeader header{ id, messageData.size() };
		buffer.resize(messageData.size() + sizeof(header));
		std::memcpy(&buffer[0], &header, sizeof(header));
		std::memcpy(&buffer[0] + sizeof(header), messageData.c_str(), messageData.size());
		return buffer;
	}

	Hydra::TestMessage GetDefaultTestMessage()
	{
		Hydra::TestMessage message;
		message.set_some_test_message("This is a cool test message.");
		message.set_value_1(55);
		message.set_value_2(69);
		message.set_value_3(100);

		return message;
	}

	bool VerifyEqual(int size, Common::MessageId id, const Hydra::TestMessage& original,
		const Common::NetworkMessage& parsedMessage)
	{
		using MessageDifferencer = google::protobuf::util::MessageDifferencer;

		Hydra::TestMessage deserialiedMessage;
		if (!deserialiedMessage.ParseFromString(parsedMessage.messageData))
		{
			return false;
		}

		return MessageDifferencer::Equals(deserialiedMessage, original)
			&& parsedMessage.header.messageLength == size
			&& parsedMessage.header.messageType == id;
		
	}

	std::unique_ptr<Common::NetworkMessageParser> networkParser;
};

//===============================================================================
} // namespace Tests
