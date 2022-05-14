 void BnCrypto::readVector(const Parcel &data, Vector<uint8_t> &vector) const {
     uint32_t size = data.readInt32();
    vector.insertAt((size_t)0, size);
    data.read(vector.editArray(), size);
 }
