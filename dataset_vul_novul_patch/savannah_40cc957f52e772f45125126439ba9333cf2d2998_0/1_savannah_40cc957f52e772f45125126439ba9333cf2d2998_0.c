int Tar::ReadHeaders( void )
{
  FILE *in;
  TarHeader lHeader;
  TarRecord lRecord;
  unsigned int iBegData = 0;
  char buf_header[512];
  
  in = fopen(mFilePath.fn_str(), "rb");
  
  if(in == NULL)
    {
      wxLogFatalError(_("Error: File '%s' not found!  Cannot read data."), mFilePath.c_str());
      return 1;
    }
  
  wxString lDmodDizPath;
  mmDmodDescription = _T("");
  mInstalledDmodDirectory = _T("");

  int total_read = 0;
  while (true)
    {
      memset(&lHeader, 0, sizeof(TarHeader));
      memset(&lRecord, 0, sizeof(TarRecord));
      
      fread((char*)&lHeader.Name, 100, 1, in);
      fread((char*)&lHeader.Mode, 8, 1, in);
      fread((char*)&lHeader.Uid, 8, 1, in);
      fread((char*)&lHeader.Gid, 8, 1, in);
      fread((char*)&lHeader.Size, 12, 1, in);
      fread((char*)&lHeader.Mtime, 12, 1, in);
      fread((char*)&lHeader.Chksum, 8, 1, in);
      fread((char*)&lHeader.Linkflag, 1, 1, in);
      fread((char*)&lHeader.Linkname, 100, 1, in);
      fread((char*)&lHeader.Magic, 8, 1, in);
      fread((char*)&lHeader.Uname, 32, 1, in);
      fread((char*)&lHeader.Gname, 32, 1, in);
      fread((char*)&lHeader.Devmajor, 8, 1, in);
      fread((char*)&lHeader.Devminor, 8, 1, in);
      fread((char*)&lHeader.Padding, 167, 1, in);
      total_read += 512;

      if(!VerifyChecksum(&lHeader))
        {
	  wxLogFatalError(_("Error: This .dmod file has an invalid checksum!  Cannot read file."));
	  return 1;
        }
        
      strncpy(lRecord.Name, lHeader.Name, 100);
      
      if (strcmp(lHeader.Name, "\xFF") == 0)
	continue;
      
      sscanf((const char*)&lHeader.Size, "%o", &lRecord.iFileSize);
      lRecord.iFilePosBegin = total_read;
      
      if(strcmp(lHeader.Name, "") == 0)
        {
	  break;
        }
      
      wxString lPath(lRecord.Name, wxConvUTF8);
       wxString lPath(lRecord.Name, wxConvUTF8);
       if (mInstalledDmodDirectory.Length() == 0)
         {
	  mInstalledDmodDirectory = lPath.SubString( 0, lPath.Find( '/' ) );
 	  lDmodDizPath = mInstalledDmodDirectory + _T("dmod.diz");
 	  lDmodDizPath.LowerCase();
         }
	}
      else
	{
	  int remaining = lRecord.iFileSize;
	  char buf[BUFSIZ];
	  while (remaining > 0)
	    {
	      if (feof(in))
		break; // TODO: error, unexpected end of file
	      int nb_read = fread(buf, 1, (remaining > BUFSIZ) ? BUFSIZ : remaining, in);
	      remaining -= nb_read;
	    }
	}
      total_read += lRecord.iFileSize;
      TarRecords.push_back(lRecord);
      
      int padding_size = (512 - (total_read % 512)) % 512;
      fread(buf_header, 1, padding_size, in);
      total_read += padding_size;
    }
