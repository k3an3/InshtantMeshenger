#include <iostream>
#include <string>

#include <tracker.h>
#include <parser.h>

using namespace libmeshenger;
using namespace std;

int
main()
{
	Tracker tracker("http://10.0.242.14", "testnode");
	Tracker tracker2("http://10.0.242.14", "testnode2");
	tracker.reportNode();
	tracker2.reportNode();
	tracker.reportPacket("testIdentifier");
	tracker.reportHop("testIdentifier", "1", "testnode2");
}
