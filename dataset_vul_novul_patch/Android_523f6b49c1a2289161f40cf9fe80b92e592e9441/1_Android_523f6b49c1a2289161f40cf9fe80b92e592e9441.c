 status_t HevcParameterSets::addNalUnit(const uint8_t* data, size_t size) {
     uint8_t nalUnitType = (data[0] >> 1) & 0x3f;
     status_t err = OK;
     switch (nalUnitType) {
         case 32:  // VPS
             err = parseVps(data + 2, size - 2);
             break;
         case 33:  // SPS
             err = parseSps(data + 2, size - 2);
             break;
         case 34:  // PPS
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
