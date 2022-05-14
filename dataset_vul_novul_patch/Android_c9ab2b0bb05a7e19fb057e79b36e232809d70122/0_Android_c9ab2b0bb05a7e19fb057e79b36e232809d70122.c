status_t CameraService::dump(int fd, const Vector<String16>& args) {
 String8 result;
 if (checkCallingPermission(String16("android.permission.DUMP")) == false) {
        result.appendFormat("Permission Denial: "
 "can't dump CameraService from pid=%d, uid=%d\n",
                getCallingPid(),
                getCallingUid());
        write(fd, result.string(), result.size());
 } else {
 bool locked = tryLock(mServiceLock);
 if (!locked) {
            result.append("CameraService may be deadlocked\n");
            write(fd, result.string(), result.size());
 }

 bool hasClient = false;
 if (!mModule) {
            result = String8::format("No camera module available!\n");
            write(fd, result.string(), result.size());
 return NO_ERROR;
 }

        result = String8::format("Camera module HAL API version: 0x%x\n",
                mModule->common.hal_api_version);
        result.appendFormat("Camera module API version: 0x%x\n",
                mModule->common.module_api_version);
        result.appendFormat("Camera module name: %s\n",
                mModule->common.name);
        result.appendFormat("Camera module author: %s\n",
                mModule->common.author);
        result.appendFormat("Number of camera devices: %d\n\n", mNumberOfCameras);
        write(fd, result.string(), result.size());
 for (int i = 0; i < mNumberOfCameras; i++) {
            result = String8::format("Camera %d static information:\n", i);
            camera_info info;

 status_t rc = mModule->get_camera_info(i, &info);
 if (rc != OK) {
                result.appendFormat("  Error reading static information!\n");
                write(fd, result.string(), result.size());
 } else {
                result.appendFormat("  Facing: %s\n",
                        info.facing == CAMERA_FACING_BACK ? "BACK" : "FRONT");
                result.appendFormat("  Orientation: %d\n", info.orientation);
 int deviceVersion;
 if (mModule->common.module_api_version <
                        CAMERA_MODULE_API_VERSION_2_0) {
                    deviceVersion = CAMERA_DEVICE_API_VERSION_1_0;
 } else {
                    deviceVersion = info.device_version;
 }
                result.appendFormat("  Device version: 0x%x\n", deviceVersion);
 if (deviceVersion >= CAMERA_DEVICE_API_VERSION_2_0) {
                    result.appendFormat("  Device static metadata:\n");
                    write(fd, result.string(), result.size());
                    dump_indented_camera_metadata(info.static_camera_characteristics,
                            fd, 2, 4);
 } else {
                    write(fd, result.string(), result.size());
 }
 }

            sp<BasicClient> client = mClient[i].promote();
 if (client == 0) {
                result = String8::format("  Device is closed, no client instance\n");
                write(fd, result.string(), result.size());
 continue;
 }

             hasClient = true;
             result = String8::format("  Device is open. Client instance dump:\n");
             write(fd, result.string(), result.size());
            client->dumpClient(fd, args);
         }
         if (!hasClient) {
             result = String8::format("\nNo active camera clients yet.\n");
            write(fd, result.string(), result.size());
 }

 if (locked) mServiceLock.unlock();

        write(fd, "\n", 1);
        camera3::CameraTraces::dump(fd, args);

 int n = args.size();
 for (int i = 0; i + 1 < n; i++) {
 String16 verboseOption("-v");
 if (args[i] == verboseOption) {
 String8 levelStr(args[i+1]);
 int level = atoi(levelStr.string());
                result = String8::format("\nSetting log level to %d.\n", level);
                setLogLevel(level);
                write(fd, result.string(), result.size());
 }
 }

 }
 return NO_ERROR;
}
