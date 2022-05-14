void RilSapSocket::sendDisconnect() {
 size_t encoded_size = 0;
 uint32_t written_size;
 size_t buffer_size = 0;
 pb_ostream_t ostream;
 bool success = false;

    RIL_SIM_SAP_DISCONNECT_REQ disconnectReq;


    if ((success = pb_get_encoded_size(&encoded_size, RIL_SIM_SAP_DISCONNECT_REQ_fields,
         &disconnectReq)) && encoded_size <= INT32_MAX) {
         buffer_size = encoded_size + sizeof(uint32_t);
        uint8_t buffer[buffer_size];
         written_size = htonl((uint32_t) encoded_size);
         ostream = pb_ostream_from_buffer(buffer, buffer_size);
         pb_write(&ostream, (uint8_t *)&written_size, sizeof(written_size));
        success = pb_encode(&ostream, RIL_SIM_SAP_DISCONNECT_REQ_fields, buffer);

 if(success) {
 pb_bytes_array_t *payload = (pb_bytes_array_t *)calloc(1,
 sizeof(pb_bytes_array_t) + written_size);
 if (!payload) {
                RLOGE("sendDisconnect: OOM");
 return;
 }
            memcpy(payload->bytes, buffer, written_size);
            payload->size = written_size;
 MsgHeader *hdr = (MsgHeader *)malloc(sizeof(MsgHeader));
 if (!hdr) {
                RLOGE("sendDisconnect: OOM");
                free(payload);
 return;
 }
            hdr->payload = payload;
            hdr->type = MsgType_REQUEST;
            hdr->id = MsgId_RIL_SIM_SAP_DISCONNECT;
            hdr->error = Error_RIL_E_SUCCESS;
            dispatchDisconnect(hdr);
 }

         else {
             RLOGE("Encode failed in send disconnect!");
         }
     }
 }
