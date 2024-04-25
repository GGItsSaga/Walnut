#pragma once
#ifndef DECRYPT_H
#define DECRYPT_H
#include <sodium.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdexcept>

class Decrypt {
private:
    std::vector<std::string> filenames_;
    std::string key_;

    std::string readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Error opening file: " + filename);
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        return content;
    }

    void writeFile(const std::string& filename, const std::string& content) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Error creating or opening file: " + filename);
        }

        file.write(content.data(), content.size());
        file.close();
    }

    void decryptAndSave(const std::string& ciphertext, const std::string& key, const std::string& filename) {
        try {
            const size_t expectedSize = crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES;
            if (ciphertext.size() < expectedSize) {
                throw std::runtime_error("Insufficient ciphertext size for decryption");
            }

            // Extract nonce from the beginning of the ciphertext
            std::vector<unsigned char> nonce(ciphertext.begin(), ciphertext.begin() + crypto_secretbox_NONCEBYTES);

            // Decrypt the rest of the ciphertext
            std::vector<unsigned char> decryptedText(ciphertext.size() - crypto_secretbox_NONCEBYTES);

            if (crypto_secretbox_open_easy(
                decryptedText.data(),
                reinterpret_cast<const unsigned char*>(ciphertext.data()) + crypto_secretbox_NONCEBYTES,
                ciphertext.size() - crypto_secretbox_NONCEBYTES,
                nonce.data(), reinterpret_cast<const unsigned char*>(key.data())) != 0) {
                throw std::runtime_error("Error decrypting file");
            }


            std::string result(reinterpret_cast<const char*>(decryptedText.data()), decryptedText.size());
            writeFile("decrypted_" + filename, result);

        }
        catch (const std::exception& e) {
            std::cerr << "Exception during decryption: " << e.what() << std::endl;
            throw;  // Re-throw the exception after logging
        }
    }

public:
    Decrypt() {
        if (sodium_init() < 0) {
            throw std::runtime_error("Error initializing Libsodium");
        }
    }

    ~Decrypt() {
        sodium_memzero(&key_[0], key_.size());
    }

    void setKey(const std::string& key) {
        key_ = key;
    }

    void appendFile(const std::string& filename) {
        filenames_.push_back(filename);
    }

    void decryptAndSaveAll() {
        for (const auto& filename : filenames_) {
            try {
                std::string ciphertext = readFile(filename);
                decryptAndSave(ciphertext, key_, filename);
            }
            catch (const std::exception& e) {
                std::cerr << "Error processing file: " << filename << " - " << e.what() << std::endl;
            }
        }
    }
};


#endif