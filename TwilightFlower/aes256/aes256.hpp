/*
 * aes256.hpp
 *
 * Copyright (c) 2014, Danilo Treffiletti <urban82@gmail.com>
 * All rights reserved.
 *
 *     This file is part of Aes256.
 *
 *     Aes256 is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU Lesser General Public License as
 *     published by the Free Software Foundation, either version 2.1
 *     of the License, or (at your option) any later version.
 *
 *     Aes256 is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *     GNU Lesser General Public License for more details.
 *
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with Aes256.
 *     If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef AES256_HPP
#define AES256_HPP

#include <vector>

typedef std::vector<unsigned char> ByteArray;

#define BLOCK_SIZE 16
#include <vector>

class Aes256 {
public:
    Aes256(const std::vector<unsigned char>& key);
    ~Aes256();

    static size_t encrypt(const std::vector<unsigned char>& key, const std::vector<unsigned char>& plain, std::vector<unsigned char>& encrypted);
    static size_t encrypt(const std::vector<unsigned char>& key, const unsigned char* plain, const size_t plain_length, std::vector<unsigned char>& encrypted);
    static size_t decrypt(const std::vector<unsigned char>& key, const std::vector<unsigned char>& encrypted, std::vector<unsigned char>& plain);
    static size_t decrypt(const std::vector<unsigned char>& key, const unsigned char* encrypted, const size_t encrypted_length, std::vector<unsigned char>& plain);

    size_t encrypt_start(const size_t plain_length, std::vector<unsigned char>& encrypted);
    size_t encrypt_continue(const std::vector<unsigned char>& plain, std::vector<unsigned char>& encrypted);
    size_t encrypt_continue(const unsigned char* plain, const size_t plain_length, std::vector<unsigned char>& encrypted);
    size_t encrypt_end(std::vector<unsigned char>& encrypted);

    size_t decrypt_start(const size_t encrypted_length);
    size_t decrypt_continue(const std::vector<unsigned char>& encrypted, std::vector<unsigned char>& plain);
    size_t decrypt_continue(const unsigned char* encrypted, const size_t encrypted_length, std::vector<unsigned char>& plain);
    size_t decrypt_end(std::vector<unsigned char>& plain);

private:
    std::vector<unsigned char> m_key;
    std::vector<unsigned char> m_salt;
    std::vector<unsigned char> m_rkey;

    unsigned char m_buffer[3 * BLOCK_SIZE];
    unsigned char m_buffer_pos;
    size_t m_remainingLength;

    bool m_decryptInitialized;

    void check_and_encrypt_buffer(std::vector<unsigned char>& encrypted);
    void check_and_decrypt_buffer(std::vector<unsigned char>& plain);

    void encrypt(unsigned char* buffer);
    void decrypt(unsigned char* buffer);

    void expand_enc_key(unsigned char* rc);
    void expand_dec_key(unsigned char* rc);

    void sub_bytes(unsigned char* buffer);
    void sub_bytes_inv(unsigned char* buffer);

    void copy_key();

    void add_round_key(unsigned char* buffer, const unsigned char round);

    void shift_rows(unsigned char* buffer);
    void shift_rows_inv(unsigned char* buffer);

    void mix_columns(unsigned char* buffer);
    void mix_columns_inv(unsigned char* buffer);
};
#endif /* AES256_HPP */