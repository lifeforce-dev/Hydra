//---------------------------------------------------------------
//
// MessageParserTest.cpp
//

#include "MessageParserTest.h"

#include "Catch2/catch.hpp"
#include "common/Log.h"
#include <google/protobuf/util/message_differencer.h>
#include <memory>
#include <chrono>

namespace Tests {

namespace {
	const int s_bufferSize = 64000;

} // anon namespace

//===============================================================================

SCENARIO_METHOD(HelperFixture, "Parsing a single fully formed TestMessage.", "[Message Parser]")
{
	GIVEN("A single perfectly constructed message")
	{
		Hydra::TestMessage defaultMessage = GetDefaultTestMessage();
		Common::MessageId defaultId = Common::MessageId::TestMessage;
		std::string packagedMessage = PackageMessage(defaultMessage.SerializeAsString(),
			defaultId);
		WHEN("Parsing full single test message")
		{
			std::vector<Common::NetworkMessage> messages;
			ExtractMessages(packagedMessage, messages);

			THEN("We will have a full message")
			{
				REQUIRE(!messages.empty());

				AND_THEN("The message will be correctly parsed from the data")
				{
					REQUIRE(VerifyEqual(defaultMessage.ByteSize(),
						defaultId, defaultMessage, messages[0]));
				}
			}
		}
	}
}

SCENARIO_METHOD(HelperFixture, "Parsing many fully formed messages.", "[Message Parser]")
{
	Hydra::TestMessage defaultMessage = GetDefaultTestMessage();
	std::string packagedMessage = PackageMessage(defaultMessage.SerializeAsString(),
		Common::MessageId::TestMessage);

	int chunkSize = packagedMessage.size();
	int numMessagesToParse = s_bufferSize / packagedMessage.size();

	GIVEN("A 64k buffer filled with default messages")
	{
		std::string data;
		data.reserve(s_bufferSize);
		data.resize(chunkSize * numMessagesToParse);

		int currentSize = 0;
		for (int i = 0; i < numMessagesToParse; ++i)
		{
			std::memcpy(&data[0] + currentSize, &packagedMessage[0], chunkSize);
			currentSize += chunkSize;
		}

		WHEN("Entire buffer of messages is parsed")
		{
			std::vector<Common::NetworkMessage> messages;
			messages.reserve(1500);
			using clock = std::chrono::high_resolution_clock;

			ExtractMessages(data, messages);

			THEN("We will have a list of the correct number of parsed messages")
			{
				REQUIRE(messages.size() == numMessagesToParse);
			}
			AND_THEN("Each message will be correct")
			{
				for (const auto& msg : messages)
				{
					REQUIRE(VerifyEqual(defaultMessage.ByteSize(),
						Common::MessageId::TestMessage, defaultMessage, messages[0]));
				}
			}
		}
	}
}

SCENARIO_METHOD(HelperFixture, "Messages get parsed in fragmented, partial chunks.", "[Message Parser]")
{
	GIVEN("A packaged test message")
	{
		// This message serializes to EXACTLY 16 bytes.
		Hydra::TestMessage message;
		message.set_some_test_message("12345678");
		message.set_value_1(1);
		message.set_value_2(2);
		message.set_value_3(3);

		// Packaging with the header makes this EXACTLY 24 bytes.
		std::string serializedMessage = PackageMessage(message.SerializeAsString(),
			Common::MessageId::TestMessage);
		std::vector<Common::NetworkMessage> messages;
		REQUIRE(serializedMessage.size() == 24);

		WHEN("Sending full header, then the message in 5 chunks")
		{
			ExtractMessages(serializedMessage.substr(0, 8), messages);
			REQUIRE(messages.empty());
			ExtractMessages(serializedMessage.substr(8, 5), messages);
			REQUIRE(messages.empty());
			ExtractMessages(serializedMessage.substr(13, 3), messages);
			REQUIRE(messages.empty());
			ExtractMessages(serializedMessage.substr(16, 4), messages);
			REQUIRE(messages.empty());
			ExtractMessages(serializedMessage.substr(20, 2), messages);
			REQUIRE(messages.empty());

			// Last chunk should yield a message.
			ExtractMessages(serializedMessage.substr(22, 2), messages);
			REQUIRE(!messages.empty());
			THEN("The parsed message will have been constructed from the bits")
			{
				REQUIRE(VerifyEqual(message.ByteSize(),
					Common::MessageId::TestMessage, message, messages[0]));
			}
		}
		AND_WHEN("Sending header in 4 chunks, then message in 2 chunks")
		{
			ExtractMessages(serializedMessage.substr(0, 2), messages);
			REQUIRE(messages.empty());
			ExtractMessages(serializedMessage.substr(2, 3), messages);
			REQUIRE(messages.empty());
			ExtractMessages(serializedMessage.substr(5, 2), messages);
			REQUIRE(messages.empty());
			ExtractMessages(serializedMessage.substr(7, 1), messages);
			REQUIRE(messages.empty());
			ExtractMessages(serializedMessage.substr(8, 8), messages);
			REQUIRE(messages.empty());

			// Last chunk should yield a message.
			ExtractMessages(serializedMessage.substr(16, 8), messages);
			REQUIRE(!messages.empty());
			THEN("The parsed message will have been constructed from the bits")
			{
				REQUIRE(VerifyEqual(message.ByteSize(),
					Common::MessageId::TestMessage, message, messages[0]));
			}
		}
	}
}

SCENARIO_METHOD(HelperFixture, "Messages get parsed unevenly in addition to being fragmented.")
{
	GIVEN("Two distinct messages that need parsing")
	{
		// This message serializes to EXACTLY 16 bytes.
		Hydra::TestMessage msgA;
		msgA.set_some_test_message("12345678");
		msgA.set_value_1(1);
		msgA.set_value_2(2);
		msgA.set_value_3(3);

		// This message serializes to EXACTLY 16 bytes.
		Hydra::TestMessage msgB;
		msgB.set_some_test_message("abcdefgh");
		msgB.set_value_1(9);
		msgB.set_value_2(8);
		msgB.set_value_3(7);

		std::string packagedMessageBuffer = PackageMessage(msgA.SerializeAsString(),
			Common::MessageId::TestMessage);
		REQUIRE(packagedMessageBuffer.size() == 24);

		packagedMessageBuffer.append(PackageMessage(msgB.SerializeAsString(),
			Common::MessageId::TestMessage));
		REQUIRE(packagedMessageBuffer.size() == 48);

		std::vector<Common::NetworkMessage> messages;
		WHEN("Send msgA header, msgA data in 3 chunks with the last chunk 2 byte overlap into msgB."
			" Then, the rest of msgB")
		{
			ExtractMessages(packagedMessageBuffer.substr(0, 8), messages);
			REQUIRE(messages.empty());
			ExtractMessages(packagedMessageBuffer.substr(8, 12), messages);
			REQUIRE(messages.empty());
			ExtractMessages(packagedMessageBuffer.substr(20, 2), messages);
			REQUIRE(messages.empty());

			ExtractMessages(packagedMessageBuffer.substr(22, 4), messages);
			REQUIRE(messages.size() == 1);

			ExtractMessages(packagedMessageBuffer.substr(26, 22), messages);
			REQUIRE(messages.size() == 2);

			THEN("Both messages will be available and correct.")
			{
				REQUIRE(VerifyEqual(msgA.ByteSize(), Common::MessageId::TestMessage, msgA,
					messages[0]));
				REQUIRE(VerifyEqual(msgB.ByteSize(), Common::MessageId::TestMessage, msgB,
					messages[1]));
			}
		}
		AND_WHEN("Heavy overlapping of messages occurs when parsing")
		{
			for (int i = 0; i < 3; ++i)
			{
				packagedMessageBuffer.append(PackageMessage(msgB.SerializeAsString(),
					Common::MessageId::TestMessage));
			}

			ExtractMessages(packagedMessageBuffer.substr(0, 3), messages);
			ExtractMessages(packagedMessageBuffer.substr(3, 12), messages);
			ExtractMessages(packagedMessageBuffer.substr(15, 4), messages);
			ExtractMessages(packagedMessageBuffer.substr(19, 4), messages);
			ExtractMessages(packagedMessageBuffer.substr(23, 2), messages);
			ExtractMessages(packagedMessageBuffer.substr(25, 1), messages);
			ExtractMessages(packagedMessageBuffer.substr(26, 48), messages);
			ExtractMessages(packagedMessageBuffer.substr(74, 24), messages);
			ExtractMessages(packagedMessageBuffer.substr(98, 6), messages);
			ExtractMessages(packagedMessageBuffer.substr(104, 16), messages);
			REQUIRE(messages.size() == 5);

			THEN("First message will be msgA, next 4 will be msgB")
			{
				REQUIRE(VerifyEqual(msgA.ByteSize(), Common::MessageId::TestMessage, msgA,
					messages[0]));
				for (int i = 1; i < 5; ++i)
				{
					REQUIRE(VerifyEqual(msgB.ByteSize(), Common::MessageId::TestMessage, msgB,
						messages[i]));
				}
			}
		}
	}
}

//===============================================================================

} // namespace Tests
