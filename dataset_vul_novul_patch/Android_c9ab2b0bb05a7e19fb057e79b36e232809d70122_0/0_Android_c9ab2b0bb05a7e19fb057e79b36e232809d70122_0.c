 status_t Camera2Client::dump(int fd, const Vector<String16>& args) {
    return BasicClient::dump(fd, args);
}

status_t Camera2Client::dumpClient(int fd, const Vector<String16>& args) {
     String8 result;
     result.appendFormat("Client2[%d] (%p) Client: %s PID: %d, dump:\n",
             mCameraId,
            getRemoteCallback()->asBinder().get(),
 String8(mClientPackageName).string(),
            mClientPid);
    result.append("  State: ");
#define CASE_APPEND_ENUM(x) case x: result.append(#x "\n"); break;

 const Parameters& p = mParameters.unsafeAccess();

    result.append(Parameters::getStateName(p.state));

    result.append("\n  Current parameters:\n");
    result.appendFormat("    Preview size: %d x %d\n",
            p.previewWidth, p.previewHeight);
    result.appendFormat("    Preview FPS range: %d - %d\n",
            p.previewFpsRange[0], p.previewFpsRange[1]);
    result.appendFormat("    Preview HAL pixel format: 0x%x\n",
            p.previewFormat);
    result.appendFormat("    Preview transform: %x\n",
            p.previewTransform);
    result.appendFormat("    Picture size: %d x %d\n",
            p.pictureWidth, p.pictureHeight);
    result.appendFormat("    Jpeg thumbnail size: %d x %d\n",
            p.jpegThumbSize[0], p.jpegThumbSize[1]);
    result.appendFormat("    Jpeg quality: %d, thumbnail quality: %d\n",
            p.jpegQuality, p.jpegThumbQuality);
    result.appendFormat("    Jpeg rotation: %d\n", p.jpegRotation);
    result.appendFormat("    GPS tags %s\n",
            p.gpsEnabled ? "enabled" : "disabled");
 if (p.gpsEnabled) {
        result.appendFormat("    GPS lat x long x alt: %f x %f x %f\n",
                p.gpsCoordinates[0], p.gpsCoordinates[1],
                p.gpsCoordinates[2]);
        result.appendFormat("    GPS timestamp: %lld\n",
                p.gpsTimestamp);
        result.appendFormat("    GPS processing method: %s\n",
                p.gpsProcessingMethod.string());
 }

    result.append("    White balance mode: ");
 switch (p.wbMode) {
        CASE_APPEND_ENUM(ANDROID_CONTROL_AWB_MODE_AUTO)
        CASE_APPEND_ENUM(ANDROID_CONTROL_AWB_MODE_INCANDESCENT)
        CASE_APPEND_ENUM(ANDROID_CONTROL_AWB_MODE_FLUORESCENT)
        CASE_APPEND_ENUM(ANDROID_CONTROL_AWB_MODE_WARM_FLUORESCENT)
        CASE_APPEND_ENUM(ANDROID_CONTROL_AWB_MODE_DAYLIGHT)
        CASE_APPEND_ENUM(ANDROID_CONTROL_AWB_MODE_CLOUDY_DAYLIGHT)
        CASE_APPEND_ENUM(ANDROID_CONTROL_AWB_MODE_TWILIGHT)
        CASE_APPEND_ENUM(ANDROID_CONTROL_AWB_MODE_SHADE)
 default: result.append("UNKNOWN\n");
 }

    result.append("    Effect mode: ");
 switch (p.effectMode) {
        CASE_APPEND_ENUM(ANDROID_CONTROL_EFFECT_MODE_OFF)
        CASE_APPEND_ENUM(ANDROID_CONTROL_EFFECT_MODE_MONO)
        CASE_APPEND_ENUM(ANDROID_CONTROL_EFFECT_MODE_NEGATIVE)
        CASE_APPEND_ENUM(ANDROID_CONTROL_EFFECT_MODE_SOLARIZE)
        CASE_APPEND_ENUM(ANDROID_CONTROL_EFFECT_MODE_SEPIA)
        CASE_APPEND_ENUM(ANDROID_CONTROL_EFFECT_MODE_POSTERIZE)
        CASE_APPEND_ENUM(ANDROID_CONTROL_EFFECT_MODE_WHITEBOARD)
        CASE_APPEND_ENUM(ANDROID_CONTROL_EFFECT_MODE_BLACKBOARD)
        CASE_APPEND_ENUM(ANDROID_CONTROL_EFFECT_MODE_AQUA)
 default: result.append("UNKNOWN\n");
 }

    result.append("    Antibanding mode: ");
 switch (p.antibandingMode) {
        CASE_APPEND_ENUM(ANDROID_CONTROL_AE_ANTIBANDING_MODE_AUTO)
        CASE_APPEND_ENUM(ANDROID_CONTROL_AE_ANTIBANDING_MODE_OFF)
        CASE_APPEND_ENUM(ANDROID_CONTROL_AE_ANTIBANDING_MODE_50HZ)
        CASE_APPEND_ENUM(ANDROID_CONTROL_AE_ANTIBANDING_MODE_60HZ)
 default: result.append("UNKNOWN\n");
 }

    result.append("    Scene mode: ");
 switch (p.sceneMode) {
 case ANDROID_CONTROL_SCENE_MODE_UNSUPPORTED:
            result.append("AUTO\n"); break;
        CASE_APPEND_ENUM(ANDROID_CONTROL_SCENE_MODE_ACTION)
        CASE_APPEND_ENUM(ANDROID_CONTROL_SCENE_MODE_PORTRAIT)
        CASE_APPEND_ENUM(ANDROID_CONTROL_SCENE_MODE_LANDSCAPE)
        CASE_APPEND_ENUM(ANDROID_CONTROL_SCENE_MODE_NIGHT)
        CASE_APPEND_ENUM(ANDROID_CONTROL_SCENE_MODE_NIGHT_PORTRAIT)
        CASE_APPEND_ENUM(ANDROID_CONTROL_SCENE_MODE_THEATRE)
        CASE_APPEND_ENUM(ANDROID_CONTROL_SCENE_MODE_BEACH)
        CASE_APPEND_ENUM(ANDROID_CONTROL_SCENE_MODE_SNOW)
        CASE_APPEND_ENUM(ANDROID_CONTROL_SCENE_MODE_SUNSET)
        CASE_APPEND_ENUM(ANDROID_CONTROL_SCENE_MODE_STEADYPHOTO)
        CASE_APPEND_ENUM(ANDROID_CONTROL_SCENE_MODE_FIREWORKS)
        CASE_APPEND_ENUM(ANDROID_CONTROL_SCENE_MODE_SPORTS)
        CASE_APPEND_ENUM(ANDROID_CONTROL_SCENE_MODE_PARTY)
        CASE_APPEND_ENUM(ANDROID_CONTROL_SCENE_MODE_CANDLELIGHT)
        CASE_APPEND_ENUM(ANDROID_CONTROL_SCENE_MODE_BARCODE)
 default: result.append("UNKNOWN\n");
 }

    result.append("    Flash mode: ");
 switch (p.flashMode) {
        CASE_APPEND_ENUM(Parameters::FLASH_MODE_OFF)
        CASE_APPEND_ENUM(Parameters::FLASH_MODE_AUTO)
        CASE_APPEND_ENUM(Parameters::FLASH_MODE_ON)
        CASE_APPEND_ENUM(Parameters::FLASH_MODE_TORCH)
        CASE_APPEND_ENUM(Parameters::FLASH_MODE_RED_EYE)
        CASE_APPEND_ENUM(Parameters::FLASH_MODE_INVALID)
 default: result.append("UNKNOWN\n");
 }

    result.append("    Focus mode: ");
 switch (p.focusMode) {
        CASE_APPEND_ENUM(Parameters::FOCUS_MODE_AUTO)
        CASE_APPEND_ENUM(Parameters::FOCUS_MODE_MACRO)
        CASE_APPEND_ENUM(Parameters::FOCUS_MODE_CONTINUOUS_VIDEO)
        CASE_APPEND_ENUM(Parameters::FOCUS_MODE_CONTINUOUS_PICTURE)
        CASE_APPEND_ENUM(Parameters::FOCUS_MODE_EDOF)
        CASE_APPEND_ENUM(Parameters::FOCUS_MODE_INFINITY)
        CASE_APPEND_ENUM(Parameters::FOCUS_MODE_FIXED)
        CASE_APPEND_ENUM(Parameters::FOCUS_MODE_INVALID)
 default: result.append("UNKNOWN\n");
 }

    result.append("   Focus state: ");
 switch (p.focusState) {
        CASE_APPEND_ENUM(ANDROID_CONTROL_AF_STATE_INACTIVE)
        CASE_APPEND_ENUM(ANDROID_CONTROL_AF_STATE_PASSIVE_SCAN)
        CASE_APPEND_ENUM(ANDROID_CONTROL_AF_STATE_PASSIVE_FOCUSED)
        CASE_APPEND_ENUM(ANDROID_CONTROL_AF_STATE_PASSIVE_UNFOCUSED)
        CASE_APPEND_ENUM(ANDROID_CONTROL_AF_STATE_ACTIVE_SCAN)
        CASE_APPEND_ENUM(ANDROID_CONTROL_AF_STATE_FOCUSED_LOCKED)
        CASE_APPEND_ENUM(ANDROID_CONTROL_AF_STATE_NOT_FOCUSED_LOCKED)
 default: result.append("UNKNOWN\n");
 }

    result.append("    Focusing areas:\n");
 for (size_t i = 0; i < p.focusingAreas.size(); i++) {
        result.appendFormat("      [ (%d, %d, %d, %d), weight %d ]\n",
                p.focusingAreas[i].left,
                p.focusingAreas[i].top,
                p.focusingAreas[i].right,
                p.focusingAreas[i].bottom,
                p.focusingAreas[i].weight);
 }

    result.appendFormat("    Exposure compensation index: %d\n",
            p.exposureCompensation);

    result.appendFormat("    AE lock %s, AWB lock %s\n",
            p.autoExposureLock ? "enabled" : "disabled",
            p.autoWhiteBalanceLock ? "enabled" : "disabled" );

    result.appendFormat("    Metering areas:\n");
 for (size_t i = 0; i < p.meteringAreas.size(); i++) {
        result.appendFormat("      [ (%d, %d, %d, %d), weight %d ]\n",
                p.meteringAreas[i].left,
                p.meteringAreas[i].top,
                p.meteringAreas[i].right,
                p.meteringAreas[i].bottom,
                p.meteringAreas[i].weight);
 }

    result.appendFormat("    Zoom index: %d\n", p.zoom);
    result.appendFormat("    Video size: %d x %d\n", p.videoWidth,
            p.videoHeight);

    result.appendFormat("    Recording hint is %s\n",
            p.recordingHint ? "set" : "not set");

    result.appendFormat("    Video stabilization is %s\n",
            p.videoStabilization ? "enabled" : "disabled");

    result.appendFormat("    Selected still capture FPS range: %d - %d\n",
            p.fastInfo.bestStillCaptureFpsRange[0],
            p.fastInfo.bestStillCaptureFpsRange[1]);

    result.append("  Current streams:\n");
    result.appendFormat("    Preview stream ID: %d\n",
            getPreviewStreamId());
    result.appendFormat("    Capture stream ID: %d\n",
            getCaptureStreamId());
    result.appendFormat("    Recording stream ID: %d\n",
            getRecordingStreamId());

    result.append("  Quirks for this camera:\n");
 bool haveQuirk = false;
 if (p.quirks.triggerAfWithAuto) {
        result.appendFormat("    triggerAfWithAuto\n");
        haveQuirk = true;
 }
 if (p.quirks.useZslFormat) {
        result.appendFormat("    useZslFormat\n");
        haveQuirk = true;
 }
 if (p.quirks.meteringCropRegion) {
        result.appendFormat("    meteringCropRegion\n");
        haveQuirk = true;
 }
 if (p.quirks.partialResults) {
        result.appendFormat("    usePartialResult\n");
        haveQuirk = true;
 }
 if (!haveQuirk) {
        result.appendFormat("    none\n");
 }

    write(fd, result.string(), result.size());

    mStreamingProcessor->dump(fd, args);

    mCaptureSequencer->dump(fd, args);

    mFrameProcessor->dump(fd, args);

    mZslProcessor->dump(fd, args);

 return dumpDevice(fd, args);
#undef CASE_APPEND_ENUM
}
