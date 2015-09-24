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

	/* Build a vector */
	std::vector<uint8_t> raw_data(test_data, test_data+40);
	
	/* Validate */
	cout << (libmeshenger::ValidatePacket(raw_data) ? "Good" : "Bad") << endl;

	/* Try packet constructor */
	libmeshenger::Packet p(raw_data);

	/* Print calculated values */
	cout << p.length() << endl;
	cout << (int) p.type() << endl;

	/* Construct message */
	libmeshenger::Message m(p);

	/* Print calculated value */
	cout << m.length() << endl;

	/* Print subset vectors (id, body) */
	for(int i = 0; i < m.id().size(); i++) {
		cout << (int) m.id()[i];
	}
	cout << endl;
	
	for(int i = 0; i < 16; i++) {
		cout << (int) m.body()[i];
	}
	cout << endl;

	raw_data[1] = 'm'; /* Mangle packet */
	p = libmeshenger::Packet(raw_data); /* Throw exception */
}
