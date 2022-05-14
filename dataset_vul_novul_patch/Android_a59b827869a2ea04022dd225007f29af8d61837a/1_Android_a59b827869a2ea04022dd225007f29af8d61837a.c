status_t IPCThreadState::executeCommand(int32_t cmd)
{
 BBinder* obj;
 RefBase::weakref_type* refs;
 status_t result = NO_ERROR;
 
 switch ((uint32_t)cmd) {
 case BR_ERROR:
        result = mIn.readInt32();
 break;
 
 case BR_OK:
 break;
 
 case BR_ACQUIRE:
        refs = (RefBase::weakref_type*)mIn.readPointer();
        obj = (BBinder*)mIn.readPointer();
        ALOG_ASSERT(refs->refBase() == obj,
 "BR_ACQUIRE: object %p does not match cookie %p (expected %p)",
                   refs, obj, refs->refBase());
        obj->incStrong(mProcess.get());
        IF_LOG_REMOTEREFS() {
            LOG_REMOTEREFS("BR_ACQUIRE from driver on %p", obj);
            obj->printRefs();
 }
        mOut.writeInt32(BC_ACQUIRE_DONE);
        mOut.writePointer((uintptr_t)refs);
        mOut.writePointer((uintptr_t)obj);
 break;
 
 case BR_RELEASE:
        refs = (RefBase::weakref_type*)mIn.readPointer();
        obj = (BBinder*)mIn.readPointer();
        ALOG_ASSERT(refs->refBase() == obj,
 "BR_RELEASE: object %p does not match cookie %p (expected %p)",
                   refs, obj, refs->refBase());
        IF_LOG_REMOTEREFS() {
            LOG_REMOTEREFS("BR_RELEASE from driver on %p", obj);
            obj->printRefs();
 }
        mPendingStrongDerefs.push(obj);
 break;
 
 case BR_INCREFS:
        refs = (RefBase::weakref_type*)mIn.readPointer();
        obj = (BBinder*)mIn.readPointer();
        refs->incWeak(mProcess.get());
        mOut.writeInt32(BC_INCREFS_DONE);
        mOut.writePointer((uintptr_t)refs);
        mOut.writePointer((uintptr_t)obj);
 break;
 
 case BR_DECREFS:
        refs = (RefBase::weakref_type*)mIn.readPointer();
        obj = (BBinder*)mIn.readPointer();
        mPendingWeakDerefs.push(refs);
 break;
 
 case BR_ATTEMPT_ACQUIRE:
        refs = (RefBase::weakref_type*)mIn.readPointer();
        obj = (BBinder*)mIn.readPointer();
 
 {
 const bool success = refs->attemptIncStrong(mProcess.get());
            ALOG_ASSERT(success && refs->refBase() == obj,
 "BR_ATTEMPT_ACQUIRE: object %p does not match cookie %p (expected %p)",
                       refs, obj, refs->refBase());
 
            mOut.writeInt32(BC_ACQUIRE_RESULT);
            mOut.writeInt32((int32_t)success);
 }
 break;
 
 case BR_TRANSACTION:
 {
            binder_transaction_data tr;
            result = mIn.read(&tr, sizeof(tr));
            ALOG_ASSERT(result == NO_ERROR,
 "Not enough command data for brTRANSACTION");
 if (result != NO_ERROR) break;
 
 Parcel buffer;
            buffer.ipcSetDataReference(
 reinterpret_cast<const uint8_t*>(tr.data.ptr.buffer),
                tr.data_size,
 reinterpret_cast<const binder_size_t*>(tr.data.ptr.offsets),
                tr.offsets_size/sizeof(binder_size_t), freeBuffer, this);
 
 const pid_t origPid = mCallingPid;
 const uid_t origUid = mCallingUid;
 const int32_t origStrictModePolicy = mStrictModePolicy;
 const int32_t origTransactionBinderFlags = mLastTransactionBinderFlags;

            mCallingPid = tr.sender_pid;
            mCallingUid = tr.sender_euid;
            mLastTransactionBinderFlags = tr.flags;

 int curPrio = getpriority(PRIO_PROCESS, mMyThreadId);
 if (gDisableBackgroundScheduling) {
 if (curPrio > ANDROID_PRIORITY_NORMAL) {
                    setpriority(PRIO_PROCESS, mMyThreadId, ANDROID_PRIORITY_NORMAL);
 }
 } else {
 if (curPrio >= ANDROID_PRIORITY_BACKGROUND) {
                    set_sched_policy(mMyThreadId, SP_BACKGROUND);
 }
 }


 Parcel reply;
 status_t error;
            IF_LOG_TRANSACTIONS() {
 TextOutput::Bundle _b(alog);
                alog << "BR_TRANSACTION thr " << (void*)pthread_self()
 << " / obj " << tr.target.ptr << " / code "
 << TypeCode(tr.code) << ": " << indent << buffer
 << dedent << endl
 << "Data addr = "
 << reinterpret_cast<const uint8_t*>(tr.data.ptr.buffer)
 << ", offsets addr="

                     << reinterpret_cast<const size_t*>(tr.data.ptr.offsets) << endl;
             }
             if (tr.target.ptr) {
                sp<BBinder> b((BBinder*)tr.cookie);
                error = b->transact(tr.code, buffer, &reply, tr.flags);
 
             } else {
                 error = the_context_object->transact(tr.code, buffer, &reply, tr.flags);
 }

 
 if ((tr.flags & TF_ONE_WAY) == 0) {
                LOG_ONEWAY("Sending reply to %d!", mCallingPid);
 if (error < NO_ERROR) reply.setError(error);
                sendReply(reply, 0);
 } else {
                LOG_ONEWAY("NOT sending reply to %d!", mCallingPid);
 }
 
            mCallingPid = origPid;
            mCallingUid = origUid;
            mStrictModePolicy = origStrictModePolicy;
            mLastTransactionBinderFlags = origTransactionBinderFlags;

            IF_LOG_TRANSACTIONS() {
 TextOutput::Bundle _b(alog);
                alog << "BC_REPLY thr " << (void*)pthread_self() << " / obj "
 << tr.target.ptr << ": " << indent << reply << dedent << endl;
 }
 
 }
 break;
 
 case BR_DEAD_BINDER:
 {
 BpBinder *proxy = (BpBinder*)mIn.readPointer();
            proxy->sendObituary();
            mOut.writeInt32(BC_DEAD_BINDER_DONE);
            mOut.writePointer((uintptr_t)proxy);
 } break;
 
 case BR_CLEAR_DEATH_NOTIFICATION_DONE:
 {
 BpBinder *proxy = (BpBinder*)mIn.readPointer();
            proxy->getWeakRefs()->decWeak(proxy);
 } break;
 
 case BR_FINISHED:
        result = TIMED_OUT;
 break;
 
 case BR_NOOP:
 break;
 
 case BR_SPAWN_LOOPER:
        mProcess->spawnPooledThread(false);
 break;
 
 default:
        printf("*** BAD COMMAND %d received from Binder driver\n", cmd);
        result = UNKNOWN_ERROR;
 break;
 }

 if (result != NO_ERROR) {
        mLastError = result;
 }
 
 return result;
}
