 bool Smb4KMountJob::fillArgs(Smb4KShare *share, QMap<QString, QVariant>& map)
 {
  const QString mount = findMountExecutable();
 
   if (mount.isEmpty())
   {
  paths << "/sbin";
  paths << "/usr/bin";
  paths << "/usr/sbin";
  paths << "/usr/local/bin";
  paths << "/usr/local/sbin";
   }
   
  map.insert("mh_command", mount);
  
   QMap<QString, QString> global_options = globalSambaOptions();
   Smb4KCustomOptions *options  = Smb4KCustomOptionsManager::self()->findOptions(share);
      break;
    }
