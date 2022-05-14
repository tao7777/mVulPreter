 int main(int argc __unused, char** argv)
 {
    limitProcessMemory(
        "ro.media.maxmem", /* property that defines limit */
        SIZE_MAX, /* upper limit in bytes */
        65 /* upper limit as percentage of physical RAM */);

     signal(SIGPIPE, SIG_IGN);
     char value[PROPERTY_VALUE_MAX];
     bool doLog = (property_get("ro.test_harness", value, "0") > 0) && (atoi(value) == 1);
 pid_t childPid;
 if (doLog && (childPid = fork()) != 0) {
        strcpy(argv[0], "media.log");
        sp<ProcessState> proc(ProcessState::self());
 MediaLogService::instantiate();
 ProcessState::self()->startThreadPool();
 for (;;) {
 siginfo_t info;
 int ret = waitid(P_PID, childPid, &info, WEXITED | WSTOPPED | WCONTINUED);
 if (ret == EINTR) {
 continue;
 }
 if (ret < 0) {
 break;
 }
 char buffer[32];
 const char *code;
 switch (info.si_code) {
 case CLD_EXITED:
                code = "CLD_EXITED";
 break;
 case CLD_KILLED:
                code = "CLD_KILLED";
 break;
 case CLD_DUMPED:
                code = "CLD_DUMPED";
 break;
 case CLD_STOPPED:
                code = "CLD_STOPPED";
 break;
 case CLD_TRAPPED:
                code = "CLD_TRAPPED";
 break;
 case CLD_CONTINUED:
                code = "CLD_CONTINUED";
 break;
 default:
                snprintf(buffer, sizeof(buffer), "unknown (%d)", info.si_code);
                code = buffer;
 break;
 }
 struct rusage usage;
            getrusage(RUSAGE_CHILDREN, &usage);
            ALOG(LOG_ERROR, "media.log", "pid %d status %d code %s user %ld.%03lds sys %ld.%03lds",
                    info.si_pid, info.si_status, code,
                    usage.ru_utime.tv_sec, usage.ru_utime.tv_usec / 1000,
                    usage.ru_stime.tv_sec, usage.ru_stime.tv_usec / 1000);
            sp<IServiceManager> sm = defaultServiceManager();
            sp<IBinder> binder = sm->getService(String16("media.log"));
 if (binder != 0) {
 Vector<String16> args;
                binder->dump(-1, args);
 }
 switch (info.si_code) {
 case CLD_EXITED:
 case CLD_KILLED:
 case CLD_DUMPED: {
                ALOG(LOG_INFO, "media.log", "exiting");
                _exit(0);
 }
 default:
 break;
 }
 }
 } else {
 if (doLog) {
            prctl(PR_SET_PDEATHSIG, SIGKILL); // if parent media.log dies before me, kill me also
            setpgid(0, 0); // but if I die first, don't kill my parent
 }
 InitializeIcuOrDie();
        sp<ProcessState> proc(ProcessState::self());
        sp<IServiceManager> sm = defaultServiceManager();
        ALOGI("ServiceManager: %p", sm.get());
 AudioFlinger::instantiate();
 MediaPlayerService::instantiate();
 ResourceManagerService::instantiate();
 CameraService::instantiate();
 AudioPolicyService::instantiate();
 SoundTriggerHwService::instantiate();
 RadioService::instantiate();
        registerExtensions();
 ProcessState::self()->startThreadPool();
 IPCThreadState::self()->joinThreadPool();
 }
}
