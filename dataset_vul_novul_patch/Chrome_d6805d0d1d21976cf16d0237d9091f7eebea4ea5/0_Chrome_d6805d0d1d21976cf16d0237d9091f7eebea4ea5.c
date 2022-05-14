 void EnsureInitializeForAndroidLayoutTests() {
   JNIEnv* env = base::android::AttachCurrentThread();
   content::NestedMessagePumpAndroid::RegisterJni(env);
   content::RegisterNativesImpl(env);

  bool success = base::MessageLoop::InitMessagePumpForUIFactory(
      &CreateMessagePumpForUI);
  CHECK(success) << "Unable to initialize the message pump for Android.";

  base::FilePath files_dir(GetTestFilesDirectory(env));

  base::FilePath stdout_fifo(files_dir.Append(FILE_PATH_LITERAL("test.fifo")));
  EnsureCreateFIFO(stdout_fifo);

  base::FilePath stderr_fifo(
      files_dir.Append(FILE_PATH_LITERAL("stderr.fifo")));
  EnsureCreateFIFO(stderr_fifo);

  base::FilePath stdin_fifo(files_dir.Append(FILE_PATH_LITERAL("stdin.fifo")));
  EnsureCreateFIFO(stdin_fifo);

  success = base::android::RedirectStream(stdout, stdout_fifo, "w") &&
            base::android::RedirectStream(stdin, stdin_fifo, "r") &&
            base::android::RedirectStream(stderr, stderr_fifo, "w");

  CHECK(success) << "Unable to initialize the Android FIFOs.";
}
