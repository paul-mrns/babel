/*
** EPITECH PROJECT, 2026
** Untitled (Workspace)
** File description:
** AEncryption.hpp
*/

#ifndef AES_GCM_HPP_
#define AES_GCM_HPP_

#include "AEncryption.hpp"
#include <openssl/evp.h>
#include <openssl/rand.h>

namespace babel {
    class AESGCM : public AEncryption {
        public:
            AESGCM() = default;
            ~AESGCM() override = default;

            // Format [IV (12 bytes)] [TAG (16 bytes)] [CIPHERTEXT (n bytes)]

            std::vector<uint8_t> encrypt(const std::vector<uint8_t>& plaintext) override
            {
                EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
                if (!ctx)
                    throw std::runtime_error("Failed to create EVP context");

                std::vector<uint8_t> iv(12);
                if (RAND_bytes(iv.data(), iv.size()) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    throw std::runtime_error("Failed to generate random IV");
                }

                if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, _key, iv.data()) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    throw std::runtime_error("Failed to init AES-GCM encryption");
                }

                std::vector<uint8_t> ciphertext(plaintext.size());
                int len = 0;
                if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size()) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    throw std::runtime_error("Encryption update failed");
                }

                if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    throw std::runtime_error("Encryption finalization failed");
                }

                std::vector<uint8_t> tag(16);
                if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag.data()) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    throw std::runtime_error("Failed to retrieve auth tag");
                }

                EVP_CIPHER_CTX_free(ctx);

                std::vector<uint8_t> result;
                result.reserve(iv.size() + tag.size() + ciphertext.size());
                result.insert(result.end(), iv.begin(), iv.end());
                result.insert(result.end(), tag.begin(), tag.end());
                result.insert(result.end(), ciphertext.begin(), ciphertext.end());

                return result;
            }

            std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data) override
            {
                if (data.size() < 28)
                    throw std::runtime_error("Encrypted packet too short");

                const uint8_t* iv = data.data();
                const uint8_t* tag = data.data() + 12;
                const uint8_t* ciphertext = data.data() + 28;
                int ciphertext_len = data.size() - 28;

                EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
                if (!ctx)
                    throw std::runtime_error("Failed to create EVP context");

                if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, _key, iv) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    throw std::runtime_error("Failed to init AES-GCM decryption");
                }

                std::vector<uint8_t> plaintext(ciphertext_len);
                int len = 0;
                if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext, ciphertext_len) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    throw std::runtime_error("Decryption update failed");
                }

                if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, (void*)tag) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    throw std::runtime_error("Failed to set auth tag");
                }

                int ret = EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len);
                EVP_CIPHER_CTX_free(ctx);

                if (ret > 0)
                    return plaintext;
                throw std::runtime_error("Auth Tag mismatch: packet is corrupted or key is wrong");
            }
    };
}

#endif