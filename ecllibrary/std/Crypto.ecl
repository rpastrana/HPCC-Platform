/*##############################################################################
## HPCC SYSTEMS software Copyright (C) 2019 HPCC Systems®.  All rights reserved.
############################################################################## */


EXPORT Crypto := MODULE


IMPORT lib_cryptolib;


/**
 * Returns set of supported Hash Algorithms
 *
 * @return        SET OF STRING containing all supported Hash Algorithms
 */
EXPORT SET OF STRING SupportedHashAlgorithms() := lib_cryptolib.CryptoLib.SupportedHashAlgorithms();


/**
 * Returns set of supported CipherAlgorithms
 *
 * @return        SET OF STRING containing all supported Cipher Algorithms
 */
EXPORT SET OF STRING SupportedSymmetricCipherAlgorithms() := lib_cryptolib.CryptoLib.SupportedSymmetricCipherAlgorithms();


/**
 * Returns set of supported Public Key Algorithms
 *
 * @return        SET OF STRING containing all supported Public Key Algorithms
 */
EXPORT SET OF STRING SupportedPublicKeyAlgorithms() := lib_cryptolib.CryptoLib.SupportedPublicKeyAlgorithms();



/**
 * Hashing module containing all the supported hashing functions.
 *
 * @param   hashAlgorithm  The Hashing algorithm to use, as returned by SupportedHashAlgorithms()
 */
EXPORT Hashing(VARSTRING hashAlgorithm) := MODULE
    /**
     * Create a hash of the given data, using a hash algorithm that
     * was returned by SupportedHashAlgorithms()
     *
     * @param   inputData      Data to hash
     * @return                 Hashed contents
     */
    EXPORT DATA Hash(DATA inputData) := FUNCTION
        RETURN lib_cryptolib.CryptoLib.Hash(hashAlgorithm, inputData);
    END;
END; // Hashing module

//-----

/**
 * Encryption module containing all symmetric encryption/decryption functions
 *
 * @param   algorithm      Symmetric algorithm to use, as returned by SupportedSymmetricCipherAlgorithms()
 * @param   passphrase     Passphrase to use for encryption/encryption
 */
EXPORT SymmetricEncryption(VARSTRING algorithm, VARSTRING passphrase) := MODULE
    /**
     * Encrypt the given data, using the specified passphrase and symmetric cipher
     * algorithm that was returned by SupportedSymmetricCipherAlgorithms()
     *
     * @param   inputData  Contents to encrypt
     * @return             Encrypted cipher
     */
    EXPORT DATA Encrypt(DATA inputData) := FUNCTION
        RETURN lib_cryptolib.CryptoLib.SymmetricEncrypt( algorithm, passphrase, inputData );
    END;
    
    /**
     * Decrypt the given cipher, using the specified passphrase and symmetric cipher
     * algorithm that was returned by SupportedSymmetricCipherAlgorithms()
     *
     * @param   encryptedData  Contents to decrypt
     * @return                 Decrypted data
     */
    EXPORT DATA Decrypt(DATA encryptedData) := FUNCTION
        RETURN lib_cryptolib.CryptoLib.SymmetricDecrypt( algorithm, passphrase, encryptedData );
    END;
END; // SymmetricEncryption module




/**
 * Encryption module containing all asymmetric encryption/decryption/digital
 * signing/signature verification functions
 *
 * @param   pkAlgorithm    ASymmetric algorithm to use, as returned by SupportedPublicKeyAlgorithms()
 * @param   publicKeyFile  File specification of PEM formatted public key file
 * @param   privateKeyFile File specification of PEM formatted private key file
 * @param   passphrase     Passphrase to use for encryption/encryption/signing/verifying
 */
EXPORT PublicKeyEncryption(VARSTRING pkAlgorithm, VARSTRING publicKeyFile = '', VARSTRING privateKeyFile = '', VARSTRING passphrase = '') := MODULE
    /**
     * Encrypt the given data, using the specified public key file,
     * passphrase, and algorithm
     *
     * @param   inputData    Contents to Encrypt
     * @return               Encrypted data
     */
    EXPORT DATA Encrypt(DATA inputData) := FUNCTION
        RETURN lib_cryptolib.CryptoLib.Encrypt( pkAlgorithm, publicKeyFile, passphrase, inputData);
    END;

    /**
     * Decrypt the given encrypted data, using the specified private key file,
     * passphrase, and algorithm
     *
     * @param   encryptedData    Contents to Decrypt
     * @return                   Decrypted data
     */
    EXPORT DATA Decrypt(DATA encryptedData) := FUNCTION
        RETURN lib_cryptolib.CryptoLib.Decrypt( pkAlgorithm, privateKeyFile, passphrase, encryptedData);
    END;

    /**
     * Create a digital signature of the given data, using the
     * specified private key file, passphrase and algorithm
     *
     * @param   inputData    Contents to sign
     * @return               Computed Digital signature
     */
    EXPORT DATA Sign( DATA inputData) := FUNCTION
        RETURN lib_cryptolib.CryptoLib.Sign( pkAlgorithm, privateKeyFile, passphrase, inputData);
    END;

    /**
     * Verify the given digital signature of the given data, using
     * the specified public key file, passphrase and algorithm
     *
     * @param   signature      Signature to verify
     * @param   signedData     Data used to create signature
     * @return                 Boolean TRUE/FALSE
     */
    EXPORT BOOLEAN VerifySignature(DATA signature, DATA signedData) := FUNCTION
        RETURN lib_cryptolib.CryptoLib.VerifySignature( pkAlgorithm, publicKeyFile, passphrase, signature, signedData);
    END;
END; // PublicKeyEncryption module



/**
  * Encryption module containing all asymmetric encryption/decryption/digital
  * signing/signature verification functions
  *
  * @param   pkAlgorithm    ASymmetric algorithm to use, as returned by SupportedPublicKeyAlgorithms()
  * @param   publicKeyBuff  PEM formatted Public key buffer
  * @param   privateKeyBuff PEM formatted Private key buffer
  * @param   passphrase     Passphrase to use for encryption/encryption/signing/verifying
  */
EXPORT PublicKeyEncryptionFromBuffer(VARSTRING pkAlgorithm, VARSTRING publicKeyBuff = '', VARSTRING privateKeyBuff = '', VARSTRING passphrase = '') := MODULE
    /**
      * Encrypt the given data, using the specified public key, passphrase,
      * and algorithm
      *
      * @param   inputData    Contents to Encrypt
      * @return               Encrypted data
      */
    EXPORT DATA Encrypt(DATA inputData) := FUNCTION
        RETURN lib_cryptolib.CryptoLib.EncryptBuff( pkAlgorithm, publicKeyBuff, passphrase, inputData);
    END;
    
    /**
      * Decrypt the given data, using the specified private key, passphrase,
      * and algorithm
      *
      * @param   encryptedData  Contents to Decrypt
      * @return                 Decrypted data
      */
    EXPORT DATA Decrypt(DATA encryptedData) := FUNCTION
        RETURN lib_cryptolib.CryptoLib.DecryptBuff(pkAlgorithm, privateKeyBuff, passphrase, encryptedData);
    END;

    /**
      * Create a digital signature of the given data, using the specified private key,
      * passphrase, and algorithm
      *
      * @param   inputData    Contents to sign
      * @return               Computed digital signature
      */
    EXPORT DATA Sign(DATA inputData) := FUNCTION
        RETURN lib_cryptolib.CryptoLib.SignBuff( pkAlgorithm, privateKeyBuff, passphrase, inputData);
    END;

    /**
      * Verify the given digital signature of the given data, using the specified public key,
      * passphrase, and algorithm
      *
      * @param   signature      Signature to verify     
      * @param   signedData     Data used to create signature
      * @return                 Booolean TRUE if signature is valid, otherwise FALSE
      */
    EXPORT BOOLEAN VerifySignature(DATA signature, DATA signedData) := FUNCTION
        RETURN lib_cryptolib.CryptoLib.VerifySignatureBuff( pkAlgorithm, publicKeyBuff, passphrase, signature, signedData);
    END;
    
END; // PublicKeyEncryption module


END; // Crypto module
