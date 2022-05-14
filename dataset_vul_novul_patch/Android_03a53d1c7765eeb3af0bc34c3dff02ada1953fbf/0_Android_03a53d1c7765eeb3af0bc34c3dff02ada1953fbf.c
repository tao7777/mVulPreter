void InputDispatcher::enqueueDispatchEntryLocked(
 const sp<Connection>& connection, EventEntry* eventEntry, const InputTarget* inputTarget,
 int32_t dispatchMode) {
 int32_t inputTargetFlags = inputTarget->flags;
 if (!(inputTargetFlags & dispatchMode)) {
 return;
 }
    inputTargetFlags = (inputTargetFlags & ~InputTarget::FLAG_DISPATCH_MASK) | dispatchMode;

 DispatchEntry* dispatchEntry = new DispatchEntry(eventEntry, // increments ref
            inputTargetFlags, inputTarget->xOffset, inputTarget->yOffset,
            inputTarget->scaleFactor);

 switch (eventEntry->type) {
 case EventEntry::TYPE_KEY: {
 KeyEntry* keyEntry = static_cast<KeyEntry*>(eventEntry);
        dispatchEntry->resolvedAction = keyEntry->action;
        dispatchEntry->resolvedFlags = keyEntry->flags;

 if (!connection->inputState.trackKey(keyEntry,
                dispatchEntry->resolvedAction, dispatchEntry->resolvedFlags)) {
#if DEBUG_DISPATCH_CYCLE
            ALOGD("channel '%s' ~ enqueueDispatchEntryLocked: skipping inconsistent key event",
                    connection->getInputChannelName());
#endif
 delete dispatchEntry;
 return; // skip the inconsistent event
 }
 break;
 }

 case EventEntry::TYPE_MOTION: {
 MotionEntry* motionEntry = static_cast<MotionEntry*>(eventEntry);
 if (dispatchMode & InputTarget::FLAG_DISPATCH_AS_OUTSIDE) {
            dispatchEntry->resolvedAction = AMOTION_EVENT_ACTION_OUTSIDE;
 } else if (dispatchMode & InputTarget::FLAG_DISPATCH_AS_HOVER_EXIT) {
            dispatchEntry->resolvedAction = AMOTION_EVENT_ACTION_HOVER_EXIT;
 } else if (dispatchMode & InputTarget::FLAG_DISPATCH_AS_HOVER_ENTER) {
            dispatchEntry->resolvedAction = AMOTION_EVENT_ACTION_HOVER_ENTER;
 } else if (dispatchMode & InputTarget::FLAG_DISPATCH_AS_SLIPPERY_EXIT) {
            dispatchEntry->resolvedAction = AMOTION_EVENT_ACTION_CANCEL;
 } else if (dispatchMode & InputTarget::FLAG_DISPATCH_AS_SLIPPERY_ENTER) {
            dispatchEntry->resolvedAction = AMOTION_EVENT_ACTION_DOWN;
 } else {
            dispatchEntry->resolvedAction = motionEntry->action;
 }
 if (dispatchEntry->resolvedAction == AMOTION_EVENT_ACTION_HOVER_MOVE
 && !connection->inputState.isHovering(
                        motionEntry->deviceId, motionEntry->source, motionEntry->displayId)) {
#if DEBUG_DISPATCH_CYCLE
        ALOGD("channel '%s' ~ enqueueDispatchEntryLocked: filling in missing hover enter event",
                connection->getInputChannelName());
#endif
            dispatchEntry->resolvedAction = AMOTION_EVENT_ACTION_HOVER_ENTER;
 }

        dispatchEntry->resolvedFlags = motionEntry->flags;

         if (dispatchEntry->targetFlags & InputTarget::FLAG_WINDOW_IS_OBSCURED) {
             dispatchEntry->resolvedFlags |= AMOTION_EVENT_FLAG_WINDOW_IS_OBSCURED;
         }
        if (dispatchEntry->targetFlags & InputTarget::FLAG_WINDOW_IS_PARTIALLY_OBSCURED) {
            dispatchEntry->resolvedFlags |= AMOTION_EVENT_FLAG_WINDOW_IS_PARTIALLY_OBSCURED;
        }
 
         if (!connection->inputState.trackMotion(motionEntry,
                 dispatchEntry->resolvedAction, dispatchEntry->resolvedFlags)) {
#if DEBUG_DISPATCH_CYCLE
            ALOGD("channel '%s' ~ enqueueDispatchEntryLocked: skipping inconsistent motion event",
                    connection->getInputChannelName());
#endif
 delete dispatchEntry;
 return; // skip the inconsistent event
 }
 break;
 }
 }

 if (dispatchEntry->hasForegroundTarget()) {
        incrementPendingForegroundDispatchesLocked(eventEntry);
 }

    connection->outboundQueue.enqueueAtTail(dispatchEntry);
    traceOutboundQueueLengthLocked(connection);
}
