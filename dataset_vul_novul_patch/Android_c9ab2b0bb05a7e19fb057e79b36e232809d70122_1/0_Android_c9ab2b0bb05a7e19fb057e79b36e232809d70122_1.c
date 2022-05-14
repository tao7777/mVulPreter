 status_t CameraClient::dump(int fd, const Vector<String16>& args) {
    return BasicClient::dump(fd, args);
}

status_t CameraClient::dumpClient(int fd, const Vector<String16>& args) {
     const size_t SIZE = 256;
     char buffer[SIZE];
 
 size_t len = snprintf(buffer, SIZE, "Client[%d] (%p) PID: %d\n",
            mCameraId,
            getRemoteCallback()->asBinder().get(),
            mClientPid);
    len = (len > SIZE - 1) ? SIZE - 1 : len;
    write(fd, buffer, len);
 return mHardware->dump(fd, args);
}
