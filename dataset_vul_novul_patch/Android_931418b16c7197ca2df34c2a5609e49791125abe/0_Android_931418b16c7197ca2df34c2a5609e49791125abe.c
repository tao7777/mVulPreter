netdutils::Status XfrmController::ipSecSetEncapSocketOwner(const android::base::unique_fd& socket,
 int newUid, uid_t callerUid) {
    ALOGD("XfrmController:%s, line=%d", __FUNCTION__, __LINE__);

 const int fd = socket.get();
 struct stat info;
 if (fstat(fd, &info)) {
 return netdutils::statusFromErrno(errno, "Failed to stat socket file descriptor");
 }
 if (info.st_uid != callerUid) {
 return netdutils::statusFromErrno(EPERM, "fchown disabled for non-owner calls");
 }
 if (S_ISSOCK(info.st_mode) == 0) {
 return netdutils::statusFromErrno(EINVAL, "File descriptor was not a socket");

     }
 
     int optval;
    socklen_t optlen = sizeof(optval);
     netdutils::Status status =
         getSyscallInstance().getsockopt(Fd(socket), IPPROTO_UDP, UDP_ENCAP, &optval, &optlen);
     if (status != netdutils::status::ok) {
 return status;
 }
 if (optval != UDP_ENCAP_ESPINUDP && optval != UDP_ENCAP_ESPINUDP_NON_IKE) {
 return netdutils::statusFromErrno(EINVAL, "Socket did not have UDP-encap sockopt set");
 }
 if (fchown(fd, newUid, -1)) {
 return netdutils::statusFromErrno(errno, "Failed to fchown socket file descriptor");
 }

 return netdutils::status::ok;
}
