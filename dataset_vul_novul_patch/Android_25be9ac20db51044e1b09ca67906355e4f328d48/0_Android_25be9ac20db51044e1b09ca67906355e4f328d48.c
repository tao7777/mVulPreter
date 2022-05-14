 virtual status_t configureVideoTunnelMode(
            node_id node, OMX_U32 portIndex, OMX_BOOL tunneled,
            OMX_U32 audioHwSync, native_handle_t **sidebandHandle ) {
 Parcel data, reply;
        data.writeInterfaceToken(IOMX::getInterfaceDescriptor());
        data.writeInt32((int32_t)node);
        data.writeInt32(portIndex);
        data.writeInt32((int32_t)tunneled);
        data.writeInt32(audioHwSync);

         remote()->transact(CONFIGURE_VIDEO_TUNNEL_MODE, data, &reply);
 
         status_t err = reply.readInt32();
        if (err == OK && sidebandHandle) {
             *sidebandHandle = (native_handle_t *)reply.readNativeHandle();
         }
         return err;
 }
