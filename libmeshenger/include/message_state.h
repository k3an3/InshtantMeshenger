#ifndef __MESSAGE_STATE_H
#define __MESSAGE_STATE_H

/* Message state */

#include <parser.h>

namespace libmeshenger
{
	class StateEngine
	{
		public:
			MessageEngine();
			void MessageRetransmitted(Message m);
			bool IsMessageNew(Message m);
	}
}

#endif
