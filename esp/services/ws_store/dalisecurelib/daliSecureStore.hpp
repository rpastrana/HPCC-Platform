/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2019 HPCC Systems®.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
############################################################################## */

#ifndef DALISECURESTORE_HPP_
#define DALISECURESTORE_HPP_

#ifdef DALISECURESTORE_EXPORTS
    #define DALISECURESTORE_API DECL_EXPORT
#else
    #define DALISECURESTORE_API DECL_IMPORT
#endif

#include "jiface.hpp"

#include "dautils.hpp"
#include "dasds.hpp"
#include "SecureUser.hpp"
#include "espStoreShare.hpp"

#include "ske.hpp"

#define SDS_LOCK_TIMEOUT_KVSTORE (30*1000)

static const char* DALI_SECURESTORE_ROOT_PATH="/SecureStore";
static const char* DALI_SECURESTORE_KEYS_PATH="/Keys";
static const char* DALI_SECURESTORE_KEY_TYPE_ATT="@kty";
static const char* DALI_SECURESTORE_KEY_ID_ATT="@kid";
static const char* DALI_SECURESTORE_KEY_OPS_ATT="@key_ops";
static const char* DALI_SECURESTORE_KEY_ALG_ATT="@alg";
static const char* DALI_SECURESTORE_KEY_ATT="@key";

static const char* DALI_KVSTORE_CREATEDTIME_ATT="@createTime";
static const char* DALI_KVSTORE_EDITEDBY_ATT="@editBy";
static const char* DALI_KVSTORE_EDITEDTIME_ATT="@editTime";
static const char* DALI_KVSTORE_MAXVALSIZE_ATT="@maxValSize";

static unsigned int DALI_KVSTORE_MAXVALSIZE_DEFAULT=1024;

using namespace cryptohelper;

class DALISECURESTORE_API CDALISecureStore : public CInterface, implements IEspStore
{
public:
    IMPLEMENT_IINTERFACE;

    CDALISecureStore() {};
    virtual ~CDALISecureStore() {};

    virtual bool init(const char * name, const char * type, IPropertyTree * cfg)
    {
        VStringBuffer xpath("%s%s", DALI_SECURESTORE_ROOT_PATH, DALI_SECURESTORE_KEYS_PATH);
        ensureSDSPath(xpath.str());

        queryHPCCPKIKeyFiles(nullptr, &pubKeyFileName, &privKeyFileName, &passPhrase);
        if (isEmptyString(pubKeyFileName))
            WARNLOG("CwsstoreEx: Could not fetch public key file from configuration.");
        if (isEmptyString(privKeyFileName))
            throw MakeStringException(-1, "CwsstoreEx: Could not fetch private key file from configuration.");
        if (isEmptyString(pubKeyFileName))
            WARNLOG("CwsstoreEx: Could not fetch key passphrase from configuration.");

        Owned<IMultiException> exceptions;
        if (!isEmptyString(pubKeyFileName))
        {
            try
            {
                pubKey.setown(loadPublicKeyFromFile(pubKeyFileName, passPhrase));
            }
            catch (IException * e)
            {
                if (!exceptions)
                    exceptions.setown(makeMultiException("CDALISecureStore"));

                exceptions->append(* makeWrappedExceptionV(e, -1, "CDALISecureStore: Cannot load public key file"));
                e->Release();
            }
        }

	if (!isEmptyString(privKeyFileName))
	{
	    try
	    {
		privKey.setown(loadPrivateKeyFromFile(privKeyFileName, passPhrase));
	    }
	    catch (IException * e)
	    {
		if (!exceptions)
		    exceptions.setown(makeMultiException("CDALISecureStore"));

		exceptions->append(* makeWrappedExceptionV(e, -1, "CDALISecureStore: Cannot load private key file"));
		e->Release();
	    }
	}

	if (exceptions)
	    throw exceptions;

    ensureAttachedToDali(); //throws if in offline mode

    Owned<IRemoteConnection> conn = querySDS().connect(xpath.str(), myProcessSession(), RTM_LOCK_WRITE, SDS_LOCK_TIMEOUT_KVSTORE);
    if (!conn)
        throw MakeStringException(-1, "CDALISecureStore: Could not access securestore  on dali");

    Owned<IPropertyTree> keystree = conn->getRoot();
    if (!keystree.get())
        throw MakeStringException(-1, "CDALISecureStore:  Could not access securestore  on dali");

    Owned<IPropertyTreeIterator> iter = keystree->getElements("key");
    StringBuffer kname, key, encryptkey, decryptkey;
    /*static const char* DALI_SECURESTORE_KEY_TYPE_ATT="@kty";
static const char* DALI_SECURESTORE_KEY_ID_ATT="@kid";
static const char* DALI_SECURESTORE_KEY_OPS_ATT="@key_ops";
static const char* DALI_SECURESTORE_KEY_ALG_ATT="@alg";
static const char* DALI_SECURESTORE_KEY_ATT="@key";*/
    ForEach(*iter)
    {
        kname.set(iter->query().queryProp(DALI_SECURESTORE_KEY_ID_ATT));
        key.set(iter->query().queryProp(DALI_SECURESTORE_KEY_ATT));
        encryptkey.set(iter->query().queryProp("@encryptkey"));
        decryptkey.set(iter->query().queryProp("@decryptkey"));

        //if (!decryptkey.isEmpty() && strieq(decryptkey.str())
    }

//   EC_KEY            *pEcKey;
//   uint8_t           encoded_key[1000];
//   uint8_t           *pMem;
//   int               keylen = 0;
//
//      /* Get the EC_KEY struct pointer. */
//      pEcKey = (EC_KEY *)EVP_PKEY_CTX_get0_pkey(pubKey->get());
//
//      /* Extract the private key from the EC key struct. */
//      if (pEcKey) {
//         pMem = encoded_key;
//         keylen = i2o_ECPublicKey(pEcKey, &pMem);
//
//         /* Copy the decoded public key into the provided buffer. */
//         if (keylen)
//         {
//            keylen = ipsec_ec_x962_decode(pEcKey, encoded_key, keylen, keybuf);           //fn defined in the end
//         }
//      }
//   }
//
//int d = EVP_PKEY_cmp(pubKey->get(), privKey->get());
//int f = EVP_PKEY_cmp(pubKey->get(), pubKey->get());
//    char aesKey[aesMaxKeySize];
//    if (isEmptyString(kname))
    {
        //char aesIV[aesBlockSize];
        fillRandomData(aesMaxKeySize, aesKey);
        //fillRandomData(aesBlockSize, aesIV);

        //fillRandomData(1024*100, messageMb);
        MemoryBuffer encryptedMessageMb;
        //printf("aesEncryptDecryptTests with %u bytes with 256bit aes key\n", messageMb.length());
        //aesEncrypt(encryptedMessageMb, key.length(), key.str(), aesMaxKeySize, aesKey, nullptr);
        //aesDecrypt(decryptedMessageMb, encryptedMessageMb.length(), encryptedMessageMb.bytes(), aesMaxKeySize, aesKey, nullptr);
        aesEncryptWithRSAEncryptedKey(encryptedMessageMb, aesMaxKeySize, aesKey, *pubKey);

      //MemoryBuffer encrypted;
      //size32_t siz = cryptohelper::aesEncryptWithRSAEncryptedKey(encrypted, strlen(value), value, *pubKey);

      StringBuffer base64;
      JBASE64_Encode(encryptedMessageMb.toByteArray(), encryptedMessageMb.length(), base64, false);

      CDateTime dt;
      dt.setNow();
      StringBuffer str;

      Owned<IPropertyTree> keytree = createPTree("Key");
      if (keytree)
      {
        keytree.setown(createPTree("key"));
	  keytree->setProp("@kid", "imnotsurewhatthiswillbeyet");
	  keytree->setProp("@kty", "keytype");
	  keytree->setProp("@key_ops", "keyopssss");
	  keytree->setProp("@alg", "AES");
	  keytree->setProp("@key", base64);
	  keytree->setProp("@encode", "base64");
	  keytree->setProp("@encryptkey", pubKey->queryKeyName());
	  keytree->setProp("@decryptkey", privKey->queryKeyName());

	  keytree->setProp(DALI_KVSTORE_CREATEDTIME_ATT,dt.getString(str).str());
      }

   // if(!meta)
     //   meta = createProperties(false);
/*
    Owned<IPropertyTree> valuetree = nstree->getPropTree(key);
    if (!valuetree)
    {
        nstree->setProp(key, value);
        valuetree.setown(nstree->getPropTree(key));
        //valuetree->setProp(DALI_KVSTORE_CREATEDTIME_ATT,dt.getString(str).str());
        //valuetree->setProp(DALI_KVSTORE_CREATEDBY_ATT, owner ? owner->getName(): "");
        meta->setProp(DALI_KVSTORE_CREATEDTIME_ATT,dt.getString(str).str());
        meta->setProp(DALI_KVSTORE_CREATEDBY_ATT, owner ? owner->getName(): "");
    }
    else
    {
        //valuetree->setProp(DALI_KVSTORE_EDITEDTIME_ATT,dt.getString(str).str());
        //valuetree->setProp(DALI_KVSTORE_EDITEDBY_ATT, owner ? owner->getName(): "");
        meta->setProp(DALI_KVSTORE_EDITEDTIME_ATT,dt.getString(str).str());
        meta->setProp(DALI_KVSTORE_EDITEDBY_ATT, owner ? owner->getName(): "");
        valuetree->setProp(".", value);
    }
*/
/*
    Owned<IPropertyIterator> iter = meta->getIterator();
    ForEach(*iter.get())
    {
        const char *key = iter->getPropKey();
        if (key && *key)
        {
            const char *value = meta->queryProp(key);
            if (value && *value)
            {
                if(value)
                   valuetree->setProp(key, value);
            }
        }
    }
*/
    //ownertree->setPropTree(thenamespace, LINK(nstree));

            keystree->setPropTree("key", LINK(keytree));
            conn->commit();
        }

        return true;
    }

    IPropertyTree * getStores(const char * namefilter, const char * ownerfilter, const char * typefilter, ISecUser * user);
    bool fetchAllNamespaces(StringArray & namespaces, const char * storename, ISecUser * user, bool global);
    bool createStore(const char * apptype, const char * storename, const char * description, ISecUser * owner, unsigned int maxvalsize);
    bool addNamespace(const char * storename, const char * thenamespace, ISecUser * owner, bool global);
    bool set(const char * storename, const char * thenamespace, const char * key, const char * value, ISecUser * owner, bool global, IProperties * meta = nullptr);
    bool fetchKeySet(StringArray & keyset, const char * storename, const char * ns, ISecUser * user, bool global);
    bool fetch(const char * storename, const char * ns, const char * key, StringBuffer & value, ISecUser * user, bool global);
    IPropertyTree * getAllPairs(const char * storename, const char * ns, ISecUser * user, bool global);
    bool deletekey(const char * storename, const char * thenamespace, const char * key, ISecUser * user, bool global);
    bool deleteNamespace(const char * storename, const char * thenamespace, ISecUser * user, bool global);
    bool fetchKeyProperty(StringBuffer & propval , const char * storename, const char * ns, const char * key, const char * property, ISecUser * username, bool global);
    IPropertyTree * getAllKeyProperties(const char * storename, const char * ns, const char * key, ISecUser * username, bool global);

    bool setOfflineMode(bool offline)
    {
        m_isDetachedFromDali = offline;
        return true;
    }

    bool isAttachedToDali()
    {
        return m_isDetachedFromDali;
    }

    void ensureAttachedToDali()
    {
        if (m_isDetachedFromDali)
            throw MakeStringException(-1, "DALI Secure Store Unavailable while in offline mode!");
    }

private:
    bool m_isDetachedFromDali = false;
    Owned<CLoadedKey> pubKey, privKey;
    const char * pubKeyFileName = nullptr, *privKeyFileName = nullptr, *passPhrase = nullptr;
    char aesKey[aesMaxKeySize];
};

#endif /* DALIKVSTORE_HPP_ */
