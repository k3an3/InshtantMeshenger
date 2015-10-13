#ifndef __STATE_H
#define __STATE_H

#include <vector>
#include <cstdint>
#include <parser.h>

namespace libmeshenger
{
	class PacketEngine
	{
		private:
		   	std::vector<std::uint8_t[16]> seenMessages;
			std::vector<void (*)(ClearMessage&)> callbacks;
			bool compareIds(uint8_t *, uint8_t *);
		public:
			PacketEngine();
			void ProcessPacket(Packet&);
			void AddCallback(void (*)(ClearMessage&));
			bool IsPacketNew(Packet &);
	};
}
#endif
