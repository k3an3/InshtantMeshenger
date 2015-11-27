#include <iostream>
#include <string>

#include <tracker.h>
#include <parser.h>

using namespace libmeshenger;
using namespace std;

int
main()
{
	Tracker tracker("http://localhost:3000", "testnode");
	Tracker tracker2("http://localhost:3000", "testnode2");
	tracker.reportNode();
	tracker2.reportNode();
	tracker.reportPacket("testIdentifier");
	tracker.reportHop("testIdentifier", "1", "1", "testnode2");
}
