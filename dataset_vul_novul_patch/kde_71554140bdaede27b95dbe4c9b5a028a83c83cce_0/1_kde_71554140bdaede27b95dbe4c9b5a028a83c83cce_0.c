 ActionReply Smb4KMountHelper::mount(const QVariantMap &args)
 {
   ActionReply reply;
   reply.addData("mh_mountpoint", args["mh_mountpoint"]);
  command << args["mh_unc"].toString();
  command << args["mh_mountpoint"].toString();
  command << args["mh_options"].toStringList();
#elif defined(Q_OS_FREEBSD) || defined(Q_OS_NETBSD)
  command << args["mh_command"].toString();
  command << args["mh_options"].toStringList();
  command << args["mh_unc"].toString();
  command << args["mh_mountpoint"].toString();
#else
#endif
  proc.setProgram(command);

  proc.start();

  if (proc.waitForStarted(-1))
  {
    bool user_kill = false;

   QStringList command;
 #if defined(Q_OS_LINUX)
  command << args["mh_command"].toString();
   command << args["mh_unc"].toString();
   command << args["mh_mountpoint"].toString();
   command << args["mh_options"].toStringList();
 #elif defined(Q_OS_FREEBSD) || defined(Q_OS_NETBSD)
  command << args["mh_command"].toString();
   command << args["mh_options"].toStringList();
   command << args["mh_unc"].toString();
   command << args["mh_mountpoint"].toString();
      {
      }

      if (HelperSupport::isStopped())
      {
        proc.kill();
        user_kill = true;
        break;
      }
      else
      {
      }
    }

    if (proc.exitStatus() == KProcess::CrashExit)
    {
      if (!user_kill)
      {
        reply.setErrorCode(ActionReply::HelperError);
        reply.setErrorDescription(i18n("The mount process crashed."));
        return reply;
      }
      else
      {
      }
    }
    else
    {
      QString stdErr = QString::fromUtf8(proc.readAllStandardError());
      reply.addData("mh_error_message", stdErr.trimmed());
    }
  }
