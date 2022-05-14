status_t BnDrm::onTransact(
 uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags) {
 switch (code) {
 case INIT_CHECK:
 {
            CHECK_INTERFACE(IDrm, data, reply);
            reply->writeInt32(initCheck());
 return OK;
 }

 case IS_CRYPTO_SUPPORTED:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 uint8_t uuid[16];
            data.read(uuid, sizeof(uuid));
 String8 mimeType = data.readString8();
            reply->writeInt32(isCryptoSchemeSupported(uuid, mimeType));

 return OK;
 }

 case CREATE_PLUGIN:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 uint8_t uuid[16];
            data.read(uuid, sizeof(uuid));
            reply->writeInt32(createPlugin(uuid));
 return OK;
 }

 case DESTROY_PLUGIN:
 {
            CHECK_INTERFACE(IDrm, data, reply);
            reply->writeInt32(destroyPlugin());
 return OK;
 }

 case OPEN_SESSION:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> sessionId;
 status_t result = openSession(sessionId);
            writeVector(reply, sessionId);
            reply->writeInt32(result);
 return OK;
 }

 case CLOSE_SESSION:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> sessionId;
            readVector(data, sessionId);
            reply->writeInt32(closeSession(sessionId));
 return OK;
 }

 case GET_KEY_REQUEST:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> sessionId, initData;

            readVector(data, sessionId);
            readVector(data, initData);
 String8 mimeType = data.readString8();
 DrmPlugin::KeyType keyType = (DrmPlugin::KeyType)data.readInt32();

 KeyedVector<String8, String8> optionalParameters;
 uint32_t count = data.readInt32();
 for (size_t i = 0; i < count; ++i) {
 String8 key, value;
                key = data.readString8();
                value = data.readString8();
                optionalParameters.add(key, value);
 }

 
             Vector<uint8_t> request;
             String8 defaultUrl;
            DrmPlugin::KeyRequestType keyRequestType = DrmPlugin::kKeyRequestType_Unknown;
 
             status_t result = getKeyRequest(sessionId, initData, mimeType,
                     keyType, optionalParameters, request, defaultUrl,
 &keyRequestType);

            writeVector(reply, request);
            reply->writeString8(defaultUrl);
            reply->writeInt32(static_cast<int32_t>(keyRequestType));
            reply->writeInt32(result);
 return OK;
 }

 case PROVIDE_KEY_RESPONSE:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> sessionId, response, keySetId;
            readVector(data, sessionId);
            readVector(data, response);
 uint32_t result = provideKeyResponse(sessionId, response, keySetId);
            writeVector(reply, keySetId);
            reply->writeInt32(result);
 return OK;
 }

 case REMOVE_KEYS:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> keySetId;
            readVector(data, keySetId);
            reply->writeInt32(removeKeys(keySetId));
 return OK;
 }

 case RESTORE_KEYS:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> sessionId, keySetId;
            readVector(data, sessionId);
            readVector(data, keySetId);
            reply->writeInt32(restoreKeys(sessionId, keySetId));
 return OK;
 }

 case QUERY_KEY_STATUS:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> sessionId;
            readVector(data, sessionId);
 KeyedVector<String8, String8> infoMap;
 status_t result = queryKeyStatus(sessionId, infoMap);
 size_t count = infoMap.size();
            reply->writeInt32(count);
 for (size_t i = 0; i < count; ++i) {
                reply->writeString8(infoMap.keyAt(i));
                reply->writeString8(infoMap.valueAt(i));
 }
            reply->writeInt32(result);
 return OK;
 }

 case GET_PROVISION_REQUEST:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 String8 certType = data.readString8();
 String8 certAuthority = data.readString8();

 Vector<uint8_t> request;
 String8 defaultUrl;
 status_t result = getProvisionRequest(certType, certAuthority,
                                                  request, defaultUrl);
            writeVector(reply, request);
            reply->writeString8(defaultUrl);
            reply->writeInt32(result);
 return OK;
 }

 case PROVIDE_PROVISION_RESPONSE:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> response;
 Vector<uint8_t> certificate;
 Vector<uint8_t> wrappedKey;
            readVector(data, response);
 status_t result = provideProvisionResponse(response, certificate, wrappedKey);
            writeVector(reply, certificate);
            writeVector(reply, wrappedKey);
            reply->writeInt32(result);
 return OK;
 }

 case UNPROVISION_DEVICE:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 status_t result = unprovisionDevice();
            reply->writeInt32(result);
 return OK;
 }

 case GET_SECURE_STOPS:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 List<Vector<uint8_t> > secureStops;
 status_t result = getSecureStops(secureStops);
 size_t count = secureStops.size();
            reply->writeInt32(count);
 List<Vector<uint8_t> >::iterator iter = secureStops.begin();
 while(iter != secureStops.end()) {
 size_t size = iter->size();
                reply->writeInt32(size);
                reply->write(iter->array(), iter->size());
                iter++;
 }
            reply->writeInt32(result);
 return OK;
 }

 case GET_SECURE_STOP:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> ssid, secureStop;
            readVector(data, ssid);
 status_t result = getSecureStop(ssid, secureStop);
            writeVector(reply, secureStop);
            reply->writeInt32(result);
 return OK;
 }

 case RELEASE_SECURE_STOPS:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> ssRelease;
            readVector(data, ssRelease);
            reply->writeInt32(releaseSecureStops(ssRelease));
 return OK;
 }

 case RELEASE_ALL_SECURE_STOPS:
 {
            CHECK_INTERFACE(IDrm, data, reply);
            reply->writeInt32(releaseAllSecureStops());
 return OK;
 }

 case GET_PROPERTY_STRING:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 String8 name = data.readString8();
 String8 value;
 status_t result = getPropertyString(name, value);
            reply->writeString8(value);
            reply->writeInt32(result);
 return OK;
 }

 case GET_PROPERTY_BYTE_ARRAY:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 String8 name = data.readString8();
 Vector<uint8_t> value;
 status_t result = getPropertyByteArray(name, value);
            writeVector(reply, value);
            reply->writeInt32(result);
 return OK;
 }

 case SET_PROPERTY_STRING:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 String8 name = data.readString8();
 String8 value = data.readString8();
            reply->writeInt32(setPropertyString(name, value));
 return OK;
 }

 case SET_PROPERTY_BYTE_ARRAY:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 String8 name = data.readString8();
 Vector<uint8_t> value;
            readVector(data, value);
            reply->writeInt32(setPropertyByteArray(name, value));
 return OK;
 }

 case SET_CIPHER_ALGORITHM:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> sessionId;
            readVector(data, sessionId);
 String8 algorithm = data.readString8();
            reply->writeInt32(setCipherAlgorithm(sessionId, algorithm));
 return OK;
 }

 case SET_MAC_ALGORITHM:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> sessionId;
            readVector(data, sessionId);
 String8 algorithm = data.readString8();
            reply->writeInt32(setMacAlgorithm(sessionId, algorithm));
 return OK;
 }

 case ENCRYPT:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> sessionId, keyId, input, iv, output;
            readVector(data, sessionId);
            readVector(data, keyId);
            readVector(data, input);
            readVector(data, iv);
 uint32_t result = encrypt(sessionId, keyId, input, iv, output);
            writeVector(reply, output);
            reply->writeInt32(result);
 return OK;
 }

 case DECRYPT:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> sessionId, keyId, input, iv, output;
            readVector(data, sessionId);
            readVector(data, keyId);
            readVector(data, input);
            readVector(data, iv);
 uint32_t result = decrypt(sessionId, keyId, input, iv, output);
            writeVector(reply, output);
            reply->writeInt32(result);
 return OK;
 }

 case SIGN:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> sessionId, keyId, message, signature;
            readVector(data, sessionId);
            readVector(data, keyId);
            readVector(data, message);
 uint32_t result = sign(sessionId, keyId, message, signature);
            writeVector(reply, signature);
            reply->writeInt32(result);
 return OK;
 }

 case VERIFY:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> sessionId, keyId, message, signature;
            readVector(data, sessionId);
            readVector(data, keyId);
            readVector(data, message);
            readVector(data, signature);
 bool match;
 uint32_t result = verify(sessionId, keyId, message, signature, match);
            reply->writeInt32(match);
            reply->writeInt32(result);
 return OK;
 }

 case SIGN_RSA:
 {
            CHECK_INTERFACE(IDrm, data, reply);
 Vector<uint8_t> sessionId, message, wrappedKey, signature;
            readVector(data, sessionId);
 String8 algorithm = data.readString8();
            readVector(data, message);
            readVector(data, wrappedKey);
 uint32_t result = signRSA(sessionId, algorithm, message, wrappedKey, signature);
            writeVector(reply, signature);
            reply->writeInt32(result);
 return OK;
 }

 case SET_LISTENER: {
        CHECK_INTERFACE(IDrm, data, reply);
        sp<IDrmClient> listener =
            interface_cast<IDrmClient>(data.readStrongBinder());
        reply->writeInt32(setListener(listener));
 return NO_ERROR;
 } break;

 default:
 return BBinder::onTransact(code, data, reply, flags);
 }
}
