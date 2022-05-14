 ActionReply Smb4KMountHelper::mount(const QVariantMap &args)
 {
  //
  // The action reply
  //
   ActionReply reply;
   
  // Get the mount executable
  //
  const QString mount = findMountExecutable();
  
  //
   QMapIterator<QString, QVariant> it(args);
    proc.setOutputChannelMode(KProcess::SeparateChannels);
    proc.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
#if defined(Q_OS_LINUX)
    proc.setEnv("PASSWD", entry["mh_url"].toUrl().password(), true);
#endif
     QVariantMap entry = it.value().toMap();
     
    // Check the executable
    //
    if (mount != entry["mh_command"].toString())
    {
      // Something weird is going on, bail out.
      reply.setType(ActionReply::HelperErrorType);
      return reply;
    }
    else
    {
      // Do nothing
    }
    
    //
     KProcess proc(this);
    command << entry["mh_mountpoint"].toString();
    command << entry["mh_options"].toStringList();
#elif defined(Q_OS_FREEBSD) || defined(Q_OS_NETBSD)
    command << entry["mh_command"].toString();
    command << entry["mh_options"].toStringList();
    command << entry["mh_unc"].toString();
    command << entry["mh_mountpoint"].toString();
#else
#endif
    proc.setProgram(command);

    proc.start();
    
    if (proc.waitForStarted(-1))
    {
      bool userKill = false;
     QStringList command;
 #if defined(Q_OS_LINUX)
    command << mount;
     command << entry["mh_unc"].toString();
     command << entry["mh_mountpoint"].toString();
     command << entry["mh_options"].toStringList();
 #elif defined(Q_OS_FREEBSD) || defined(Q_OS_NETBSD)
    command << mount;
     command << entry["mh_options"].toStringList();
     command << entry["mh_unc"].toString();
     command << entry["mh_mountpoint"].toString();
        else
        {
        }

        if (HelperSupport::isStopped())
        {
          proc.kill();
          userKill = true;
          break;
        }
        else
        {
        }
      }

      if (proc.exitStatus() == KProcess::CrashExit)
      {
        if (!userKill)
        {
          reply.setType(ActionReply::HelperErrorType);
          reply.setErrorDescription(i18n("The mount process crashed."));
          break;
        }
        else
        {
        }
      }
      else
      {
        QString stdErr = QString::fromUtf8(proc.readAllStandardError());
        reply.addData(QString("mh_error_message_%1").arg(index), stdErr.trimmed());
      }
    }
