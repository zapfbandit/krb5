/*
 * Copyright (C) 1998 by the FundsXpress, INC.
 * 
 * All rights reserved.
 * 
 * Export of this software from the United States of America may require
 * a specific license from the United States Government.  It is the
 * responsibility of any person or organization contemplating export to
 * obtain such a license before exporting.
 * 
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
 * distribute this software and its documentation for any purpose and
 * without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that
 * the name of FundsXpress. not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  FundsXpress makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "k5-int.h"
#include "enc_provider.h"
#include "hash_provider.h"
#include "etypes.h"
#include "old.h"
#include "raw.h"
#include "dk.h"
#include "arcfour.h"
#include "aes_s2k.h"

/* these will be linear searched.  if they ever get big, a binary
   search or hash table would be better, which means these would need
   to be sorted.  An array would be more efficient, but that assumes
   that the keytypes are all near each other.  I'd rather not make
   that assumption. */

const struct krb5_keytypes krb5_enctypes_list[] = {
    { ENCTYPE_DES_CBC_CRC,
      "des-cbc-crc", "DES cbc mode with CRC-32",
      &krb5int_enc_des, &krb5int_hash_crc32,
      8,
      krb5_old_encrypt_length, krb5_old_encrypt, krb5_old_decrypt,
      krb5int_des_string_to_key,
      NULL, /*PRF*/
      CKSUMTYPE_RSA_MD5,
      NULL  /*AEAD*/ },
    { ENCTYPE_DES_CBC_MD4,
      "des-cbc-md4", "DES cbc mode with RSA-MD4",
      &krb5int_enc_des, &krb5int_hash_md4,
      8,
      krb5_old_encrypt_length, krb5_old_encrypt, krb5_old_decrypt,
      krb5int_des_string_to_key,
      NULL, /*PRF*/
      CKSUMTYPE_RSA_MD4,
      NULL  /*AEAD*/  },
    { ENCTYPE_DES_CBC_MD5,
      "des-cbc-md5", "DES cbc mode with RSA-MD5",
      &krb5int_enc_des, &krb5int_hash_md5,
      8,
      krb5_old_encrypt_length, krb5_old_encrypt, krb5_old_decrypt,
      krb5int_des_string_to_key,
      NULL, /*PRF*/
      CKSUMTYPE_RSA_MD5,
      NULL  /*AEAD*/ },
    { ENCTYPE_DES_CBC_MD5,
      "des", "DES cbc mode with RSA-MD5", /* alias */
      &krb5int_enc_des, &krb5int_hash_md5,
      8,
      krb5_old_encrypt_length, krb5_old_encrypt, krb5_old_decrypt,
      krb5int_des_string_to_key,
      NULL, /*PRF*/
      CKSUMTYPE_RSA_MD5,
      NULL  /*AEAD*/ },

    { ENCTYPE_DES_CBC_RAW,
      "des-cbc-raw", "DES cbc mode raw",
      &krb5int_enc_des, NULL,
      8,
      krb5_raw_encrypt_length, krb5_raw_encrypt, krb5_raw_decrypt,
      krb5int_des_string_to_key,
      NULL, /*PRF*/
      0,
      NULL  /*AEAD*/ },
    { ENCTYPE_DES3_CBC_RAW,
      "des3-cbc-raw", "Triple DES cbc mode raw",
      &krb5int_enc_des3, NULL,
      8,
      krb5_raw_encrypt_length, krb5_raw_encrypt, krb5_raw_decrypt,
      krb5int_dk_string_to_key,
      NULL, /*PRF*/
      0,
      NULL  /*AEAD*/ },

    { ENCTYPE_DES3_CBC_SHA1,
      "des3-cbc-sha1", "Triple DES cbc mode with HMAC/sha1",
      &krb5int_enc_des3, &krb5int_hash_sha1,
      8,
      krb5_dk_encrypt_length, krb5_dk_encrypt, krb5_dk_decrypt,
      krb5int_dk_string_to_key,
      NULL, /*PRF*/
      CKSUMTYPE_HMAC_SHA1_DES3,
      &krb5int_aead_dk },
    { ENCTYPE_DES3_CBC_SHA1,	/* alias */
      "des3-hmac-sha1", "Triple DES cbc mode with HMAC/sha1",
      &krb5int_enc_des3, &krb5int_hash_sha1,
      8,
      krb5_dk_encrypt_length, krb5_dk_encrypt, krb5_dk_decrypt,
      krb5int_dk_string_to_key,
      NULL, /*PRF*/
      CKSUMTYPE_HMAC_SHA1_DES3,
      &krb5int_aead_dk },
    { ENCTYPE_DES3_CBC_SHA1,	/* alias */
      "des3-cbc-sha1-kd", "Triple DES cbc mode with HMAC/sha1",
      &krb5int_enc_des3, &krb5int_hash_sha1,
      8,
      krb5_dk_encrypt_length, krb5_dk_encrypt, krb5_dk_decrypt,
      krb5int_dk_string_to_key,
      NULL, /*PRF*/
      CKSUMTYPE_HMAC_SHA1_DES3,
      &krb5int_aead_dk },

    { ENCTYPE_DES_HMAC_SHA1,
      "des-hmac-sha1", "DES with HMAC/sha1",
      &krb5int_enc_des, &krb5int_hash_sha1,
      8,
      krb5_dk_encrypt_length, krb5_dk_encrypt, krb5_dk_decrypt,
      krb5int_dk_string_to_key,
      NULL, /*PRF*/
      0,
      NULL },
    { ENCTYPE_ARCFOUR_HMAC, 
      "arcfour-hmac","ArcFour with HMAC/md5", &krb5int_enc_arcfour,
      &krb5int_hash_md5,
      0,
krb5_arcfour_encrypt_length, krb5_arcfour_encrypt,
      krb5_arcfour_decrypt, krb5int_arcfour_string_to_key,
      NULL, /*PRF*/
      CKSUMTYPE_HMAC_MD5_ARCFOUR,
      &krb5int_aead_arcfour },
    { ENCTYPE_ARCFOUR_HMAC,  /* alias */
      "rc4-hmac", "ArcFour with HMAC/md5", &krb5int_enc_arcfour,
      &krb5int_hash_md5,
      0,
      krb5_arcfour_encrypt_length, krb5_arcfour_encrypt,
      krb5_arcfour_decrypt, krb5int_arcfour_string_to_key,
      NULL, /*PRF*/
      CKSUMTYPE_HMAC_MD5_ARCFOUR,
      &krb5int_aead_arcfour },
    { ENCTYPE_ARCFOUR_HMAC,  /* alias */
      "arcfour-hmac-md5", "ArcFour with HMAC/md5", &krb5int_enc_arcfour,
      &krb5int_hash_md5,
      0,
      krb5_arcfour_encrypt_length, krb5_arcfour_encrypt,
      krb5_arcfour_decrypt, krb5int_arcfour_string_to_key,
      NULL, /*PRF*/
      CKSUMTYPE_HMAC_MD5_ARCFOUR,
      &krb5int_aead_arcfour },
    { ENCTYPE_ARCFOUR_HMAC_EXP, 
      "arcfour-hmac-exp", "Exportable ArcFour with HMAC/md5",
      &krb5int_enc_arcfour,
      &krb5int_hash_md5,
      0,
      krb5_arcfour_encrypt_length, krb5_arcfour_encrypt,
      krb5_arcfour_decrypt, krb5int_arcfour_string_to_key,
      NULL, /*PRF*/
      CKSUMTYPE_HMAC_MD5_ARCFOUR,
      &krb5int_aead_arcfour },
    { ENCTYPE_ARCFOUR_HMAC_EXP, /* alias */
      "rc4-hmac-exp", "Exportable ArcFour with HMAC/md5",
      &krb5int_enc_arcfour,
      &krb5int_hash_md5,
      0,
      krb5_arcfour_encrypt_length, krb5_arcfour_encrypt,
      krb5_arcfour_decrypt, krb5int_arcfour_string_to_key,
      NULL, /*PRF*/
      CKSUMTYPE_HMAC_MD5_ARCFOUR,
      &krb5int_aead_arcfour },
    { ENCTYPE_ARCFOUR_HMAC_EXP, /* alias */
      "arcfour-hmac-md5-exp", "Exportable ArcFour with HMAC/md5",
      &krb5int_enc_arcfour,
      &krb5int_hash_md5,
      0,
      krb5_arcfour_encrypt_length, krb5_arcfour_encrypt,
      krb5_arcfour_decrypt, krb5int_arcfour_string_to_key,
      NULL, /*PRF*/
      CKSUMTYPE_HMAC_MD5_ARCFOUR,
      &krb5int_aead_arcfour },

    { ENCTYPE_AES128_CTS_HMAC_SHA1_96,
      "aes128-cts-hmac-sha1-96", "AES-128 CTS mode with 96-bit SHA-1 HMAC",
      &krb5int_enc_aes128, &krb5int_hash_sha1,
      16,
      krb5int_aes_encrypt_length, krb5int_aes_dk_encrypt, krb5int_aes_dk_decrypt,
      krb5int_aes_string_to_key,
      krb5int_dk_prf,
      CKSUMTYPE_HMAC_SHA1_96_AES128,
      &krb5int_aead_aes },
    { ENCTYPE_AES128_CTS_HMAC_SHA1_96, /* alias */
      "aes128-cts", "AES-128 CTS mode with 96-bit SHA-1 HMAC",
      &krb5int_enc_aes128, &krb5int_hash_sha1,
      16,
      krb5int_aes_encrypt_length, krb5int_aes_dk_encrypt, krb5int_aes_dk_decrypt,
      krb5int_aes_string_to_key,
      krb5int_dk_prf,
      CKSUMTYPE_HMAC_SHA1_96_AES128,
      &krb5int_aead_aes },
    { ENCTYPE_AES256_CTS_HMAC_SHA1_96,
      "aes256-cts-hmac-sha1-96", "AES-256 CTS mode with 96-bit SHA-1 HMAC",
      &krb5int_enc_aes256, &krb5int_hash_sha1,
      16,
      krb5int_aes_encrypt_length, krb5int_aes_dk_encrypt, krb5int_aes_dk_decrypt,
      krb5int_aes_string_to_key,
      krb5int_dk_prf,
      CKSUMTYPE_HMAC_SHA1_96_AES256,
      &krb5int_aead_aes },
    { ENCTYPE_AES256_CTS_HMAC_SHA1_96, /* alias */
      "aes256-cts", "AES-256 CTS mode with 96-bit SHA-1 HMAC",
      &krb5int_enc_aes256, &krb5int_hash_sha1,
      16,
      krb5int_aes_encrypt_length, krb5int_aes_dk_encrypt, krb5int_aes_dk_decrypt,
      krb5int_aes_string_to_key,
      krb5int_dk_prf,
      CKSUMTYPE_HMAC_SHA1_96_AES256,
      &krb5int_aead_aes },
    { ENCTYPE_AES128_CCM_128,
      "aes128-ccm-128", "AES-128 CTR mode with 128-bit CBC MAC",
      &krb5int_enc_aes128_ctr, NULL,
      16,
      NULL, NULL, NULL,
      krb5int_aes_string_to_key,
      krb5int_dk_prf,
      CKSUMTYPE_AES128_CBC,
      &krb5int_aead_ccm
    },
    { ENCTYPE_AES256_CCM_128,
      "aes256-ccm-128", "AES-256 CTR mode with 128-bit CBC MAC",
      &krb5int_enc_aes256_ctr, NULL,
      16,
      NULL, NULL, NULL,
      krb5int_aes_string_to_key,
      krb5int_dk_prf,
      CKSUMTYPE_AES128_CBC,
      &krb5int_aead_ccm
    }
};

const int krb5_enctypes_length =
sizeof(krb5_enctypes_list)/sizeof(struct krb5_keytypes);
