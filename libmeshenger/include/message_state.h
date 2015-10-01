#ifndef __MESSAGE_STATE_H
#define __MESSAGE_STATE_H

/* Message state */

#include <parser.h>

namespace libmeshenger
{
	class MessageState
	{
		public:
			/* Constructor, creates an empty message state engine */
			MessageState();

			/* Add a message to the list of messages that have already
			 * been "seen" and broadcast to all peers
			 * */
			void FlagMessage(Message m);

			/* Return true if a message is "new" (hasn't already been
			 * retransmitted) or false if it has already been "seen"
			 * */
			bool IsMessageNew(Message m);

			/* Add a callback function pointer in the form
			 * of 'void MyCallback(Message m)' to the engine. During processing,
			 * the message will be sent to each callback if
			 * it is new.
			 * */
			void AddCallback(/*TYPE OF FUNCTION POINTER*/ fp); 

			/* Process a message, sending it to all callbacks if
			 * it is "new"
			 * */
			void ProcessMessage(Message m);
	}
}

#endif
