static MagickBooleanType GetMagickModulePath(const char *filename,
  MagickModuleType module_type,char *path,ExceptionInfo *exception)
{
  char
    *module_path;

  assert(filename != (const char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",filename);
   assert(path != (char *) NULL);
   assert(exception != (ExceptionInfo *) NULL);
   (void) CopyMagickString(path,filename,MaxTextExtent);
#if defined(MAGICKCORE_INSTALLED_SUPPORT)
  if (strstr(path,"../") != (char *) NULL)
    {
      errno=EPERM;
      (void) ThrowMagickException(exception,GetMagickModule(),PolicyError,
        "NotAuthorized","`%s'",path);
      return(MagickFalse);
    }
#endif
   module_path=(char *) NULL;
   switch (module_type)
   {
    case MagickImageCoderModule:
    default:
    {
      (void) LogMagickEvent(ModuleEvent,GetMagickModule(),
        "Searching for coder module file \"%s\" ...",filename);
      module_path=GetEnvironmentValue("MAGICK_CODER_MODULE_PATH");
#if defined(MAGICKCORE_CODER_PATH)
      if (module_path == (char *) NULL)
        module_path=AcquireString(MAGICKCORE_CODER_PATH);
#endif
      break;
    }
    case MagickImageFilterModule:
    {
      (void) LogMagickEvent(ModuleEvent,GetMagickModule(),
        "Searching for filter module file \"%s\" ...",filename);
      module_path=GetEnvironmentValue("MAGICK_CODER_FILTER_PATH");
#if defined(MAGICKCORE_FILTER_PATH)
      if (module_path == (char *) NULL)
        module_path=AcquireString(MAGICKCORE_FILTER_PATH);
#endif
      break;
    }
  }
  if (module_path != (char *) NULL)
    {
      register char
        *p,
        *q;

      for (p=module_path-1; p != (char *) NULL; )
      {
        (void) CopyMagickString(path,p+1,MaxTextExtent);
        q=strchr(path,DirectoryListSeparator);
        if (q != (char *) NULL)
          *q='\0';
        q=path+strlen(path)-1;
        if ((q >= path) && (*q != *DirectorySeparator))
          (void) ConcatenateMagickString(path,DirectorySeparator,MaxTextExtent);
        (void) ConcatenateMagickString(path,filename,MaxTextExtent);
        if (IsPathAccessible(path) != MagickFalse)
          {
            module_path=DestroyString(module_path);
            return(MagickTrue);
          }
        p=strchr(p+1,DirectoryListSeparator);
      }
      module_path=DestroyString(module_path);
    }
#if defined(MAGICKCORE_INSTALLED_SUPPORT)
  else
#if defined(MAGICKCORE_CODER_PATH)
    {
      const char
        *directory;

      /*
        Search hard coded paths.
      */
      switch (module_type)
      {
        case MagickImageCoderModule:
        default:
        {
          directory=MAGICKCORE_CODER_PATH;
          break;
        }
        case MagickImageFilterModule:
        {
          directory=MAGICKCORE_FILTER_PATH;
          break;
        }
      }
      (void) FormatLocaleString(path,MaxTextExtent,"%s%s",directory,filename);
      if (IsPathAccessible(path) == MagickFalse)
        {
          ThrowFileException(exception,ConfigureWarning,
            "UnableToOpenModuleFile",path);
          return(MagickFalse);
        }
      return(MagickTrue);
    }
#else
#if defined(MAGICKCORE_WINDOWS_SUPPORT)
    {
      const char
        *registery_key;

      unsigned char
        *key_value;

      /*
        Locate path via registry key.
      */
      switch (module_type)
      {
        case MagickImageCoderModule:
        default:
        {
          registery_key="CoderModulesPath";
          break;
        }
        case MagickImageFilterModule:
        {
          registery_key="FilterModulesPath";
          break;
        }
      }
      key_value=NTRegistryKeyLookup(registery_key);
      if (key_value == (unsigned char *) NULL)
        {
          ThrowMagickException(exception,GetMagickModule(),ConfigureError,
            "RegistryKeyLookupFailed","`%s'",registery_key);
          return(MagickFalse);
        }
      (void) FormatLocaleString(path,MaxTextExtent,"%s%s%s",(char *) key_value,
        DirectorySeparator,filename);
      key_value=(unsigned char *) RelinquishMagickMemory(key_value);
      if (IsPathAccessible(path) == MagickFalse)
        {
          ThrowFileException(exception,ConfigureWarning,
            "UnableToOpenModuleFile",path);
          return(MagickFalse);
        }
      return(MagickTrue);
    }
#endif
#endif
#if !defined(MAGICKCORE_CODER_PATH) && !defined(MAGICKCORE_WINDOWS_SUPPORT)
# error MAGICKCORE_CODER_PATH or MAGICKCORE_WINDOWS_SUPPORT must be defined when MAGICKCORE_INSTALLED_SUPPORT is defined
#endif
#else
  {
    char
      *home;

    home=GetEnvironmentValue("MAGICK_HOME");
    if (home != (char *) NULL)
      {
        /*
          Search MAGICK_HOME.
        */
#if !defined(MAGICKCORE_POSIX_SUPPORT)
        (void) FormatLocaleString(path,MaxTextExtent,"%s%s%s",home,
          DirectorySeparator,filename);
#else
        const char
          *directory;

        switch (module_type)
        {
          case MagickImageCoderModule:
          default:
          {
            directory=MAGICKCORE_CODER_RELATIVE_PATH;
            break;
          }
          case MagickImageFilterModule:
          {
            directory=MAGICKCORE_FILTER_RELATIVE_PATH;
            break;
          }
        }
        (void) FormatLocaleString(path,MaxTextExtent,"%s/lib/%s/%s",home,
          directory,filename);
#endif
        home=DestroyString(home);
        if (IsPathAccessible(path) != MagickFalse)
          return(MagickTrue);
      }
  }
  if (*GetClientPath() != '\0')
    {
      /*
        Search based on executable directory.
      */
#if !defined(MAGICKCORE_POSIX_SUPPORT)
      (void) FormatLocaleString(path,MaxTextExtent,"%s%s%s",GetClientPath(),
        DirectorySeparator,filename);
#else
      char
        prefix[MaxTextExtent];

      const char
        *directory;

      switch (module_type)
      {
        case MagickImageCoderModule:
        default:
        {
          directory="coders";
          break;
        }
        case MagickImageFilterModule:
        {
          directory="filters";
          break;
        }
      }
      (void) CopyMagickString(prefix,GetClientPath(),MaxTextExtent);
      ChopPathComponents(prefix,1);
      (void) FormatLocaleString(path,MaxTextExtent,"%s/lib/%s/%s/%s",prefix,
        MAGICKCORE_MODULES_RELATIVE_PATH,directory,filename);
#endif
      if (IsPathAccessible(path) != MagickFalse)
        return(MagickTrue);
    }
#if defined(MAGICKCORE_WINDOWS_SUPPORT)
  {
    /*
      Search module path.
    */
    if ((NTGetModulePath("CORE_RL_magick_.dll",path) != MagickFalse) ||
        (NTGetModulePath("CORE_DB_magick_.dll",path) != MagickFalse) ||
        (NTGetModulePath("Magick.dll",path) != MagickFalse))
      {
        (void) ConcatenateMagickString(path,DirectorySeparator,MaxTextExtent);
        (void) ConcatenateMagickString(path,filename,MaxTextExtent);
        if (IsPathAccessible(path) != MagickFalse)
          return(MagickTrue);
      }
  }
#endif
  {
    char
      *home;

    home=GetEnvironmentValue("XDG_CONFIG_HOME");
    if (home == (char *) NULL)
      home=GetEnvironmentValue("LOCALAPPDATA");
    if (home == (char *) NULL)
      home=GetEnvironmentValue("APPDATA");
    if (home == (char *) NULL)
      home=GetEnvironmentValue("USERPROFILE");
    if (home != (char *) NULL)
      {
        /*
          Search $XDG_CONFIG_HOME/ImageMagick.
        */
        (void) FormatLocaleString(path,MaxTextExtent,"%s%sImageMagick%s%s",
          home,DirectorySeparator,DirectorySeparator,filename);
        home=DestroyString(home);
        if (IsPathAccessible(path) != MagickFalse)
          return(MagickTrue);
      }
    home=GetEnvironmentValue("HOME");
    if (home != (char *) NULL)
      {
        /*
          Search $HOME/.config/ImageMagick.
        */
        (void) FormatLocaleString(path,MaxTextExtent,
          "%s%s.config%sImageMagick%s%s",home,DirectorySeparator,
          DirectorySeparator,DirectorySeparator,filename);
        if (IsPathAccessible(path) != MagickFalse)
          {
            home=DestroyString(home);
            return(MagickTrue);
          }
        /*
          Search $HOME/.magick.
        */
        (void) FormatLocaleString(path,MaxTextExtent,"%s%s.magick%s%s",home,
          DirectorySeparator,DirectorySeparator,filename);
        home=DestroyString(home);
        if (IsPathAccessible(path) != MagickFalse)
          return(MagickTrue);
      }
  }
  /*
    Search current directory.
  */
  if (IsPathAccessible(path) != MagickFalse)
    return(MagickTrue);
  if (exception->severity < ConfigureError)
    ThrowFileException(exception,ConfigureWarning,"UnableToOpenModuleFile",
      path);
#endif
  return(MagickFalse);
}
