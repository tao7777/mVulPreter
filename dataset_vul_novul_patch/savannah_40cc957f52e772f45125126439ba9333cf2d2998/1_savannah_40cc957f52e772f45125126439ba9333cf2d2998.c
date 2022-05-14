InstallVerifyFrame::InstallVerifyFrame(const wxString& lDmodFilePath)
: InstallVerifyFrame_Base(NULL, wxID_ANY, _T(""))
{
  mConfig = Config::GetConfig();

  prepareDialog();
  
  int flags = wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_REMAINING_TIME;
  wxProgressDialog lPrepareProgress(_("Preparing"),
    _("The D-Mod archive is being decompressed in a temporary file."), 100, this, flags);
  
  BZip lBZip(lDmodFilePath);
  mTarFilePath = lBZip.Extract(&lPrepareProgress);
  
  if (mTarFilePath.Len() != 0)
     {
       Tar lTar(mTarFilePath);
      lTar.ReadHeaders();
      
       wxString lDmodDescription = lTar.getmDmodDescription();
						"\n"
						"The D-Mod will be installed in subdirectory '%s'."),
					      lTar.getInstalledDmodDirectory().c_str());
        }
      else
        {
	  int lBreakChar = lDmodDescription.Find( '\r' );
	  if ( lBreakChar <= 0 )
            {
	      lBreakChar = lDmodDescription.Find( '\n' );
            }
	  mDmodName = lDmodDescription.SubString( 0, lBreakChar - 1 );
	  this->SetTitle(_("DFArc - Install D-Mod - ") + mDmodName);
        }
      mDmodDescription->SetValue(lDmodDescription);
      
      mInstallButton->Enable(true);
    }
