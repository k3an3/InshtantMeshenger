/* crypto.h */

#ifndef __CRYPTO_H
#define __CRYPTO_H

#include <cryptopp/rsa.h>
#include <parser.h>

namespace libmeshenger
{
	class CryptoEngine
	{
		friend class EncryptedMessage;
		private:
			CryptoPP::RSA::PrivateKey m_privkey;
		public:
			bool tryDecrypt(EncryptedMessage&);
			void encryptMessage(EncryptedMessage&, CryptoPP::RSAFunction);

			CryptoEngine(CryptoPP::InvertibleRSAFunction);
	};
}

#endif
