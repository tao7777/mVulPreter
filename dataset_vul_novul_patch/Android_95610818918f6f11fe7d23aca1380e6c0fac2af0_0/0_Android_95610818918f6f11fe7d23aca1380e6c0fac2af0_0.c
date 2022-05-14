void RilSapSocket::sendResponse(MsgHeader* hdr) {
 size_t encoded_size = 0;
 uint32_t written_size;
 size_t buffer_size = 0;
 pb_ostream_t ostream;
 bool success = false;

    pthread_mutex_lock(&write_lock);


     if ((success = pb_get_encoded_size(&encoded_size, MsgHeader_fields,
         hdr)) && encoded_size <= INT32_MAX && commandFd != -1) {
         buffer_size = encoded_size + sizeof(uint32_t);
        uint8_t* buffer = (uint8_t*)malloc(buffer_size);
        if (!buffer) {
            RLOGE("sendResponse: OOM");
            pthread_mutex_unlock(&write_lock);
            return;
        }
         written_size = htonl((uint32_t) encoded_size);
         ostream = pb_ostream_from_buffer(buffer, buffer_size);
         pb_write(&ostream, (uint8_t *)&written_size, sizeof(written_size));
        success = pb_encode(&ostream, MsgHeader_fields, hdr);

 if (success) {
            RLOGD("Size: %d (0x%x) Size as written: 0x%x", encoded_size, encoded_size,
        written_size);
            log_hex("onRequestComplete", &buffer[sizeof(written_size)], encoded_size);
            RLOGI("[%d] < SAP RESPONSE type: %d. id: %d. error: %d",
        hdr->token, hdr->type, hdr->id,hdr->error );

 if ( 0 != blockingWrite_helper(commandFd, buffer, buffer_size)) {
                RLOGE("Error %d while writing to fd", errno);
 } else {
                RLOGD("Write successful");
 }
 } else {

             RLOGE("Error while encoding response of type %d id %d buffer_size: %d: %s.",
             hdr->type, hdr->id, buffer_size, PB_GET_ERROR(&ostream));
         }
        free(buffer);
     } else {
     RLOGE("Not sending response type %d: encoded_size: %u. commandFd: %d. encoded size result: %d",
         hdr->type, encoded_size, commandFd, success);
 }

    pthread_mutex_unlock(&write_lock);
}
