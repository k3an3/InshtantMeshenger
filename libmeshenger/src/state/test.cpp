#include <state.h>
#include <parser.h>

#include <string>
#include <iostream>

using namespace std;
using namespace libmeshenger;

void
testCb(ClearMessage& m)
{
	cout << "Printing message:" << endl;
	cout << m.bodyString() << endl;
}


int 
main()
{
	PacketEngine e;

	ClearMessage m("Hello, world! This is an awesome test packet!");

	cout << m.body().size() << endl;

	cout << "Testing CB" << endl;
	testCb(m);

	Packet p(m);

	cout << "Registering Callback" << endl;
	e.AddCallback(testCb);

	cout << "Processing packet 1" << endl;
	e.ProcessPacket(p);

	cout << "Processing packet again" << endl;
	e.ProcessPacket(p);

	m = ClearMessage("Hello again! This is another awesome packet!");
	p = Packet(m);

	e.ProcessPacket(p);

	cout << e.PacketsProcessed() << endl;
}
