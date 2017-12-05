#ifndef __CRYPTOGRAPHY__
#define __CRYPTOGRAPHY__

#include "Arduino.h"

/// <summary>
/// Helper class to encryp and decryp content
/// </summary>
class Cryptography
{
public:

	/// <summary>
	/// Encrypt the content
	/// </summary>
	/// <param name="content">the content to encrypt.</param>
	/// <param name="size">the size of the content.</param>
	/// <param name="key">the key used to encrypt.</param>
	static void encrypt(unsigned long *content, unsigned int size, unsigned long const key[4]);

	/// <summary>
	/// Decrypt the content.
	/// </summary>
	/// <param name="content">the content to decrypt.</param>
	/// <param name="size">the size of the content</param>
	/// <param name="key">the key used to encrypt the content.</param>
	static void decrypt(unsigned long *content, unsigned int size, unsigned long const key[4]);
};

#endif // __CRYPTOGRAPHY__