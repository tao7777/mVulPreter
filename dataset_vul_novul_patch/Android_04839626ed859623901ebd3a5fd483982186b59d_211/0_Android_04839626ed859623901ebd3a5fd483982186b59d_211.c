long mkvparser::UnserializeString(
  if (status) {
     delete[] str;
     str = NULL;
 
    return status;
  }
 
  str[size] = '\0';
 
  return 0;  // success
 }
