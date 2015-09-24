#include <parser.h>
#include <vector>
#include <cstdint>
#include <iostream>

using namespace std;

int
main()
{
	
	uint8_t test_data[] =
		{ 'I', 'M', 1, 0, 0, 1, 0, 32, /* Preamble */
			1, 1, 1, 1, 1, 1, 1, 1, /* ID */
			1, 1, 1, 1, 1, 1, 1, 1, /* ID */
			1, 2, 1, 2, 1, 2, 1, 2, /* Message */
			3, 2, 3, 2, 3, 2, 3, 2}; /* Message */

	std::vector<uint8_t> raw_data(test_data, test_data+40);
	
	cout << (libmeshenger::ValidatePacket(raw_data) ? "Good" : "Bad") << endl;

	libmeshenger::Packet p(raw_data);

	cout << p.length() << endl;
	cout << (int) p.type() << endl;

	libmeshenger::Message m(p);

	cout << m.length() << endl;

	for(int i = 0; i < 16; i++) {
		cout << (int) m.id()[i];
	}
	cout << endl;
	
	for(int i = 0; i < 16; i++) {
		cout << (int) m.body()[i];
	}
	cout << endl;
}
