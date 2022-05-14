void BufferQueueConsumer::dump(String8& result, const char* prefix) const {
 const IPCThreadState* ipc = IPCThreadState::self();
 const pid_t pid = ipc->getCallingPid();
 const uid_t uid = ipc->getCallingUid();
 if ((uid != AID_SHELL)
 && !PermissionCache::checkPermission(String16(

             "android.permission.DUMP"), pid, uid)) {
         result.appendFormat("Permission Denial: can't dump BufferQueueConsumer "
                 "from pid=%d, uid=%d\n", pid, uid);
        android_errorWriteWithInfoLog(0x534e4554, "27046057", uid, NULL, 0);
     } else {
         mCore->dump(result, prefix);
     }
}
