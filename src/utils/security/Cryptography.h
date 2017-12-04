#pragma once
#include <stdint.h>
#define VERSION "0.1"

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
	static void encrypt(uint32_t *content, unsigned size, uint32_t const key[4]);

	/// <summary>
	/// Decrypt the content.
	/// </summary>
	/// <param name="content">the content to decrypt.</param>
	/// <param name="size">the size of the content</param>
	/// <param name="key">the key used to encrypt the content.</param>
	static void decrypt(uint32_t *content, unsigned size, uint32_t const key[4]);
};