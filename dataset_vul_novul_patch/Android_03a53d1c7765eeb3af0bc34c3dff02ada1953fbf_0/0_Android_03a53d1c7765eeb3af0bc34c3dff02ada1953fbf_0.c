int32_t InputDispatcher::findTouchedWindowTargetsLocked(nsecs_t currentTime,
 const MotionEntry* entry, Vector<InputTarget>& inputTargets, nsecs_t* nextWakeupTime,
 bool* outConflictingPointerActions) {
 enum InjectionPermission {
        INJECTION_PERMISSION_UNKNOWN,
        INJECTION_PERMISSION_GRANTED,
        INJECTION_PERMISSION_DENIED
 };

 nsecs_t startTime = now();

 int32_t displayId = entry->displayId;
 int32_t action = entry->action;
 int32_t maskedAction = action & AMOTION_EVENT_ACTION_MASK;

 int32_t injectionResult = INPUT_EVENT_INJECTION_PENDING;
 InjectionPermission injectionPermission = INJECTION_PERMISSION_UNKNOWN;
    sp<InputWindowHandle> newHoverWindowHandle;

 const TouchState* oldState = NULL;
 ssize_t oldStateIndex = mTouchStatesByDisplay.indexOfKey(displayId);
 if (oldStateIndex >= 0) {
        oldState = &mTouchStatesByDisplay.valueAt(oldStateIndex);
        mTempTouchState.copyFrom(*oldState);
 }

 bool isSplit = mTempTouchState.split;
 bool switchedDevice = mTempTouchState.deviceId >= 0 && mTempTouchState.displayId >= 0
 && (mTempTouchState.deviceId != entry->deviceId
 || mTempTouchState.source != entry->source
 || mTempTouchState.displayId != displayId);
 bool isHoverAction = (maskedAction == AMOTION_EVENT_ACTION_HOVER_MOVE
 || maskedAction == AMOTION_EVENT_ACTION_HOVER_ENTER
 || maskedAction == AMOTION_EVENT_ACTION_HOVER_EXIT);
 bool newGesture = (maskedAction == AMOTION_EVENT_ACTION_DOWN
 || maskedAction == AMOTION_EVENT_ACTION_SCROLL
 || isHoverAction);
 bool wrongDevice = false;
 if (newGesture) {
 bool down = maskedAction == AMOTION_EVENT_ACTION_DOWN;
 if (switchedDevice && mTempTouchState.down && !down) {
#if DEBUG_FOCUS
            ALOGD("Dropping event because a pointer for a different device is already down.");
#endif
            injectionResult = INPUT_EVENT_INJECTION_FAILED;
            switchedDevice = false;
            wrongDevice = true;
 goto Failed;
 }
        mTempTouchState.reset();
        mTempTouchState.down = down;
        mTempTouchState.deviceId = entry->deviceId;
        mTempTouchState.source = entry->source;
        mTempTouchState.displayId = displayId;
        isSplit = false;
 }

 if (newGesture || (isSplit && maskedAction == AMOTION_EVENT_ACTION_POINTER_DOWN)) {
 /* Case 1: New splittable pointer going down, or need target for hover or scroll. */

 int32_t pointerIndex = getMotionEventActionPointerIndex(action);
 int32_t x = int32_t(entry->pointerCoords[pointerIndex].
                getAxisValue(AMOTION_EVENT_AXIS_X));
 int32_t y = int32_t(entry->pointerCoords[pointerIndex].
                getAxisValue(AMOTION_EVENT_AXIS_Y));
        sp<InputWindowHandle> newTouchedWindowHandle;
 bool isTouchModal = false;

 size_t numWindows = mWindowHandles.size();
 for (size_t i = 0; i < numWindows; i++) {
            sp<InputWindowHandle> windowHandle = mWindowHandles.itemAt(i);
 const InputWindowInfo* windowInfo = windowHandle->getInfo();
 if (windowInfo->displayId != displayId) {
 continue; // wrong display
 }

 int32_t flags = windowInfo->layoutParamsFlags;
 if (windowInfo->visible) {
 if (! (flags & InputWindowInfo::FLAG_NOT_TOUCHABLE)) {
                    isTouchModal = (flags & (InputWindowInfo::FLAG_NOT_FOCUSABLE
 | InputWindowInfo::FLAG_NOT_TOUCH_MODAL)) == 0;
 if (isTouchModal || windowInfo->touchableRegionContainsPoint(x, y)) {
                        newTouchedWindowHandle = windowHandle;
 break; // found touched window, exit window loop
 }
 }

 if (maskedAction == AMOTION_EVENT_ACTION_DOWN
 && (flags & InputWindowInfo::FLAG_WATCH_OUTSIDE_TOUCH)) {

                     int32_t outsideTargetFlags = InputTarget::FLAG_DISPATCH_AS_OUTSIDE;
                     if (isWindowObscuredAtPointLocked(windowHandle, x, y)) {
                         outsideTargetFlags |= InputTarget::FLAG_WINDOW_IS_OBSCURED;
                    } else if (isWindowObscuredLocked(windowHandle)) {
                        outsideTargetFlags |= InputTarget::FLAG_WINDOW_IS_PARTIALLY_OBSCURED;
                     }
 
                     mTempTouchState.addOrUpdateWindow(
                            windowHandle, outsideTargetFlags, BitSet32(0));
 }
 }
 }

 if (newTouchedWindowHandle != NULL
 && newTouchedWindowHandle->getInfo()->supportsSplitTouch()) {
            isSplit = true;
 } else if (isSplit) {
            newTouchedWindowHandle = NULL;
 }

 if (newTouchedWindowHandle == NULL) {
            newTouchedWindowHandle = mTempTouchState.getFirstForegroundWindowHandle();
 if (newTouchedWindowHandle == NULL) {
                ALOGI("Dropping event because there is no touchable window at (%d, %d).", x, y);
                injectionResult = INPUT_EVENT_INJECTION_FAILED;
 goto Failed;
 }
 }

 int32_t targetFlags = InputTarget::FLAG_FOREGROUND | InputTarget::FLAG_DISPATCH_AS_IS;
 if (isSplit) {
            targetFlags |= InputTarget::FLAG_SPLIT;

         }
         if (isWindowObscuredAtPointLocked(newTouchedWindowHandle, x, y)) {
             targetFlags |= InputTarget::FLAG_WINDOW_IS_OBSCURED;
        } else if (isWindowObscuredLocked(newTouchedWindowHandle)) {
            targetFlags |= InputTarget::FLAG_WINDOW_IS_PARTIALLY_OBSCURED;
         }
 
 if (isHoverAction) {
            newHoverWindowHandle = newTouchedWindowHandle;
 } else if (maskedAction == AMOTION_EVENT_ACTION_SCROLL) {
            newHoverWindowHandle = mLastHoverWindowHandle;
 }

 BitSet32 pointerIds;
 if (isSplit) {
 uint32_t pointerId = entry->pointerProperties[pointerIndex].id;
            pointerIds.markBit(pointerId);
 }
        mTempTouchState.addOrUpdateWindow(newTouchedWindowHandle, targetFlags, pointerIds);
 } else {
 /* Case 2: Pointer move, up, cancel or non-splittable pointer down. */

 if (! mTempTouchState.down) {
#if DEBUG_FOCUS
            ALOGD("Dropping event because the pointer is not down or we previously "
 "dropped the pointer down event.");
#endif
            injectionResult = INPUT_EVENT_INJECTION_FAILED;
 goto Failed;
 }

 if (maskedAction == AMOTION_EVENT_ACTION_MOVE
 && entry->pointerCount == 1
 && mTempTouchState.isSlippery()) {
 int32_t x = int32_t(entry->pointerCoords[0].getAxisValue(AMOTION_EVENT_AXIS_X));
 int32_t y = int32_t(entry->pointerCoords[0].getAxisValue(AMOTION_EVENT_AXIS_Y));

            sp<InputWindowHandle> oldTouchedWindowHandle =
                    mTempTouchState.getFirstForegroundWindowHandle();
            sp<InputWindowHandle> newTouchedWindowHandle =
                    findTouchedWindowAtLocked(displayId, x, y);
 if (oldTouchedWindowHandle != newTouchedWindowHandle
 && newTouchedWindowHandle != NULL) {
#if DEBUG_FOCUS
                ALOGD("Touch is slipping out of window %s into window %s.",
                        oldTouchedWindowHandle->getName().string(),
                        newTouchedWindowHandle->getName().string());
#endif
                mTempTouchState.addOrUpdateWindow(oldTouchedWindowHandle,
 InputTarget::FLAG_DISPATCH_AS_SLIPPERY_EXIT, BitSet32(0));

 if (newTouchedWindowHandle->getInfo()->supportsSplitTouch()) {
                    isSplit = true;
 }

 int32_t targetFlags = InputTarget::FLAG_FOREGROUND
 | InputTarget::FLAG_DISPATCH_AS_SLIPPERY_ENTER;
 if (isSplit) {
                    targetFlags |= InputTarget::FLAG_SPLIT;
 }
 if (isWindowObscuredAtPointLocked(newTouchedWindowHandle, x, y)) {
                    targetFlags |= InputTarget::FLAG_WINDOW_IS_OBSCURED;
 }

 BitSet32 pointerIds;
 if (isSplit) {
                    pointerIds.markBit(entry->pointerProperties[0].id);
 }
                mTempTouchState.addOrUpdateWindow(newTouchedWindowHandle, targetFlags, pointerIds);
 }
 }
 }

 if (newHoverWindowHandle != mLastHoverWindowHandle) {
 if (mLastHoverWindowHandle != NULL) {
#if DEBUG_HOVER
            ALOGD("Sending hover exit event to window %s.",
                    mLastHoverWindowHandle->getName().string());
#endif
            mTempTouchState.addOrUpdateWindow(mLastHoverWindowHandle,
 InputTarget::FLAG_DISPATCH_AS_HOVER_EXIT, BitSet32(0));
 }

 if (newHoverWindowHandle != NULL) {
#if DEBUG_HOVER
            ALOGD("Sending hover enter event to window %s.",
                    newHoverWindowHandle->getName().string());
#endif
            mTempTouchState.addOrUpdateWindow(newHoverWindowHandle,
 InputTarget::FLAG_DISPATCH_AS_HOVER_ENTER, BitSet32(0));
 }
 }

 {
 bool haveForegroundWindow = false;
 for (size_t i = 0; i < mTempTouchState.windows.size(); i++) {
 const TouchedWindow& touchedWindow = mTempTouchState.windows[i];
 if (touchedWindow.targetFlags & InputTarget::FLAG_FOREGROUND) {
                haveForegroundWindow = true;
 if (! checkInjectionPermission(touchedWindow.windowHandle,
                        entry->injectionState)) {
                    injectionResult = INPUT_EVENT_INJECTION_PERMISSION_DENIED;
                    injectionPermission = INJECTION_PERMISSION_DENIED;
 goto Failed;
 }
 }
 }
 if (! haveForegroundWindow) {
#if DEBUG_FOCUS
            ALOGD("Dropping event because there is no touched foreground window to receive it.");
#endif
            injectionResult = INPUT_EVENT_INJECTION_FAILED;
 goto Failed;
 }

        injectionPermission = INJECTION_PERMISSION_GRANTED;
 }

 if (maskedAction == AMOTION_EVENT_ACTION_DOWN) {
        sp<InputWindowHandle> foregroundWindowHandle =
                mTempTouchState.getFirstForegroundWindowHandle();
 const int32_t foregroundWindowUid = foregroundWindowHandle->getInfo()->ownerUid;
 for (size_t i = 0; i < mTempTouchState.windows.size(); i++) {
 const TouchedWindow& touchedWindow = mTempTouchState.windows[i];
 if (touchedWindow.targetFlags & InputTarget::FLAG_DISPATCH_AS_OUTSIDE) {
                sp<InputWindowHandle> inputWindowHandle = touchedWindow.windowHandle;
 if (inputWindowHandle->getInfo()->ownerUid != foregroundWindowUid) {
                    mTempTouchState.addOrUpdateWindow(inputWindowHandle,
 InputTarget::FLAG_ZERO_COORDS, BitSet32(0));
 }
 }
 }
 }

 for (size_t i = 0; i < mTempTouchState.windows.size(); i++) {
 const TouchedWindow& touchedWindow = mTempTouchState.windows[i];
 if (touchedWindow.targetFlags & InputTarget::FLAG_FOREGROUND) {
 String8 reason = checkWindowReadyForMoreInputLocked(currentTime,
                    touchedWindow.windowHandle, entry, "touched");
 if (!reason.isEmpty()) {
                injectionResult = handleTargetsNotReadyLocked(currentTime, entry,
                        NULL, touchedWindow.windowHandle, nextWakeupTime, reason.string());
 goto Unresponsive;
 }
 }
 }

 if (maskedAction == AMOTION_EVENT_ACTION_DOWN) {
        sp<InputWindowHandle> foregroundWindowHandle =
                mTempTouchState.getFirstForegroundWindowHandle();
 if (foregroundWindowHandle->getInfo()->hasWallpaper) {
 for (size_t i = 0; i < mWindowHandles.size(); i++) {
                sp<InputWindowHandle> windowHandle = mWindowHandles.itemAt(i);
 const InputWindowInfo* info = windowHandle->getInfo();
 if (info->displayId == displayId
 && windowHandle->getInfo()->layoutParamsType

                                 == InputWindowInfo::TYPE_WALLPAPER) {
                     mTempTouchState.addOrUpdateWindow(windowHandle,
                             InputTarget::FLAG_WINDOW_IS_OBSCURED
                                    | InputTarget::FLAG_WINDOW_IS_PARTIALLY_OBSCURED
                                     | InputTarget::FLAG_DISPATCH_AS_IS,
                             BitSet32(0));
                 }
 }
 }
 }

    injectionResult = INPUT_EVENT_INJECTION_SUCCEEDED;

 for (size_t i = 0; i < mTempTouchState.windows.size(); i++) {
 const TouchedWindow& touchedWindow = mTempTouchState.windows.itemAt(i);
        addWindowTargetLocked(touchedWindow.windowHandle, touchedWindow.targetFlags,
                touchedWindow.pointerIds, inputTargets);
 }

    mTempTouchState.filterNonAsIsTouchWindows();

Failed:
 if (injectionPermission == INJECTION_PERMISSION_UNKNOWN) {
 if (checkInjectionPermission(NULL, entry->injectionState)) {
            injectionPermission = INJECTION_PERMISSION_GRANTED;
 } else {
            injectionPermission = INJECTION_PERMISSION_DENIED;
 }
 }

 if (injectionPermission == INJECTION_PERMISSION_GRANTED) {
 if (!wrongDevice) {
 if (switchedDevice) {
#if DEBUG_FOCUS
                ALOGD("Conflicting pointer actions: Switched to a different device.");
#endif
 *outConflictingPointerActions = true;
 }

 if (isHoverAction) {
 if (oldState && oldState->down) {
#if DEBUG_FOCUS
                    ALOGD("Conflicting pointer actions: Hover received while pointer was down.");
#endif
 *outConflictingPointerActions = true;
 }
                mTempTouchState.reset();
 if (maskedAction == AMOTION_EVENT_ACTION_HOVER_ENTER
 || maskedAction == AMOTION_EVENT_ACTION_HOVER_MOVE) {
                    mTempTouchState.deviceId = entry->deviceId;
                    mTempTouchState.source = entry->source;
                    mTempTouchState.displayId = displayId;
 }
 } else if (maskedAction == AMOTION_EVENT_ACTION_UP
 || maskedAction == AMOTION_EVENT_ACTION_CANCEL) {
                mTempTouchState.reset();
 } else if (maskedAction == AMOTION_EVENT_ACTION_DOWN) {
 if (oldState && oldState->down) {
#if DEBUG_FOCUS
                    ALOGD("Conflicting pointer actions: Down received while already down.");
#endif
 *outConflictingPointerActions = true;
 }
 } else if (maskedAction == AMOTION_EVENT_ACTION_POINTER_UP) {
 if (isSplit) {
 int32_t pointerIndex = getMotionEventActionPointerIndex(action);
 uint32_t pointerId = entry->pointerProperties[pointerIndex].id;

 for (size_t i = 0; i < mTempTouchState.windows.size(); ) {
 TouchedWindow& touchedWindow = mTempTouchState.windows.editItemAt(i);
 if (touchedWindow.targetFlags & InputTarget::FLAG_SPLIT) {
                            touchedWindow.pointerIds.clearBit(pointerId);
 if (touchedWindow.pointerIds.isEmpty()) {
                                mTempTouchState.windows.removeAt(i);
 continue;
 }
 }
                        i += 1;
 }
 }
 }

 if (maskedAction != AMOTION_EVENT_ACTION_SCROLL) {
 if (mTempTouchState.displayId >= 0) {
 if (oldStateIndex >= 0) {
                        mTouchStatesByDisplay.editValueAt(oldStateIndex).copyFrom(mTempTouchState);
 } else {
                        mTouchStatesByDisplay.add(displayId, mTempTouchState);
 }
 } else if (oldStateIndex >= 0) {
                    mTouchStatesByDisplay.removeItemsAt(oldStateIndex);
 }
 }

            mLastHoverWindowHandle = newHoverWindowHandle;
 }
 } else {
#if DEBUG_FOCUS
        ALOGD("Not updating touch focus because injection was denied.");
#endif
 }

Unresponsive:
    mTempTouchState.reset();

 nsecs_t timeSpentWaitingForApplication = getTimeSpentWaitingForApplicationLocked(currentTime);
    updateDispatchStatisticsLocked(currentTime, entry,
            injectionResult, timeSpentWaitingForApplication);
#if DEBUG_FOCUS
    ALOGD("findTouchedWindow finished: injectionResult=%d, injectionPermission=%d, "
 "timeSpentWaitingForApplication=%0.1fms",
            injectionResult, injectionPermission, timeSpentWaitingForApplication / 1000000.0);
#endif
 return injectionResult;
}
