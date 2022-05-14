 static char *get_pid_environ_val(pid_t pid,char *val){
  int temp_size = 500;
  char *temp = malloc(temp_size);
  
   int i=0;
   int foundit=0;
   FILE *fp;

  sprintf(temp,"/proc/%d/environ",pid);

  fp=fopen(temp,"r");
  if(fp==NULL)
    return NULL;
 
   
   for(;;){
    
    if (i >= temp_size) {
      temp_size *= 2;
      temp = realloc(temp, temp_size);
    }
      
     temp[i]=fgetc(fp);    
 
     if(foundit==1 && (temp[i]==0 || temp[i]=='\0' || temp[i]==EOF)){
      char *ret;
      temp[i]=0;
      ret=malloc(strlen(temp)+10);
      sprintf(ret,"%s",temp);
      fclose(fp);
      return ret;
    }

    switch(temp[i]){
    case EOF:
      fclose(fp);
      return NULL;
    case '=':
      temp[i]=0;
      if(!strcmp(temp,val)){
	foundit=1;
      }
      i=0;
      break;
    case '\0':
      i=0;
      break;
    default:
      i++;
    }
  }
}
