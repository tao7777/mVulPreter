 status_t ProCamera2Client::dump(int fd, const Vector<String16>& args) {
    return BasicClient::dump(fd, args);
}

status_t ProCamera2Client::dumpClient(int fd, const Vector<String16>& args) {
     String8 result;
     result.appendFormat("ProCamera2Client[%d] (%p) PID: %d, dump:\n",
             mCameraId,
            getRemoteCallback()->asBinder().get(),
            mClientPid);
    result.append("  State: ");

    mFrameProcessor->dump(fd, args);

 return dumpDevice(fd, args);
}
