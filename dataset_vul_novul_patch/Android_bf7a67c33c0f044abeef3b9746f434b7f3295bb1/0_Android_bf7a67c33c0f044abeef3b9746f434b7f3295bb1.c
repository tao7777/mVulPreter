 void BnCrypto::readVector(const Parcel &data, Vector<uint8_t> &vector) const {
     uint32_t size = data.readInt32();
    if (vector.insertAt((size_t)0, size) < 0) {
        vector.clear();
    }
    if (data.read(vector.editArray(), size) != NO_ERROR) {
        vector.clear();
        android_errorWriteWithInfoLog(0x534e4554, "62872384", -1, NULL, 0);
    }
 }
