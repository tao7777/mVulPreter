 status_t HevcParameterSets::addNalUnit(const uint8_t* data, size_t size) {
    if (size < 1) {
        ALOGE("empty NAL b/35467107");
        return ERROR_MALFORMED;
    }
     uint8_t nalUnitType = (data[0] >> 1) & 0x3f;
     status_t err = OK;
     switch (nalUnitType) {
         case 32:  // VPS
            if (size < 2) {
                ALOGE("invalid NAL/VPS size b/35467107");
                return ERROR_MALFORMED;
            }
             err = parseVps(data + 2, size - 2);
             break;
         case 33:  // SPS
            if (size < 2) {
                ALOGE("invalid NAL/SPS size b/35467107");
                return ERROR_MALFORMED;
            }
             err = parseSps(data + 2, size - 2);
             break;
         case 34:  // PPS
            if (size < 2) {
                ALOGE("invalid NAL/PPS size b/35467107");
                return ERROR_MALFORMED;
            }
             err = parsePps(data + 2, size - 2);
             break;
         case 39:  // Prefix SEI
 case 40: // Suffix SEI
 break;
 default:
            ALOGE("Unrecognized NAL unit type.");
 return ERROR_MALFORMED;
 }

 if (err != OK) {
 return err;
 }

    sp<ABuffer> buffer = ABuffer::CreateAsCopy(data, size);
    buffer->setInt32Data(nalUnitType);
    mNalUnits.push(buffer);
 return OK;
}
