/* The class should have a zero argument constructor
 * and use some sort of efficient data structure
 * to store a list of messages that have been seen
 *
 * It should only keep a certain number of messages,
 * then start kicking out old ones to make room
 * for new ones. This should be doable with some
 * added public methods.
 *
 */

#include <state.h>
#include <iostream>

using namespace std;

namespace libmeshenger
{
	void
	packetEngineDebugPrint(string s, int color)
	{
        cout << "\033[1;32m[packet-engine]\033[0m-> " <<
            "\033[1;" << color << "m" << s << "\033[0m" << endl;
	}

	bool
	PacketEngine::compareIds(uint8_t * a, uint8_t * b)
	{
		for(int i = 0; i < 16; i++) {
			if (a[i] != b[i])
				return false;
		}
		return true;
	}

	bool
	PacketEngine::IsPacketNew(Packet &p)
	{
		for(int i = 0; i < seenMessages.size(); i++) {
			if (compareIds(seenMessages[i].data(), p.id().data())) {
                packetEngineDebugPrint("Packet is old", 33);
				return false;
			}
		}

		return true;
	}

	PacketEngine::PacketEngine()
		: packets_processed(0)
	{
		seenMessages = vector<vector<uint8_t>>();
		callbacks = vector<void (*)(Packet&)>();
	}

	void
	PacketEngine::AddCallback(void (*cb)(Packet&))
	{
		callbacks.push_back(cb);
	}

	void
	PacketEngine::ProcessPacket(Packet &p)
	{
		if (IsPacketNew(p)) {
            packetEngineDebugPrint("Processing through callbacks", 35);
			for(int i = 0; i < callbacks.size(); i++) {
				callbacks[i](p);
			}
			seenMessages.push_back(p.id());
		}
		packets_processed++;
	}

	uint32_t
	PacketEngine::PacketsProcessed()
	{
		return packets_processed;
	}
}
