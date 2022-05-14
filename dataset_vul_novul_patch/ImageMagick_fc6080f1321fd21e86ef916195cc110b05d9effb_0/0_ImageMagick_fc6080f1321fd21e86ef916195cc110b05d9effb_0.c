MagickExport XMLTreeInfo *NewXMLTree(const char *xml,ExceptionInfo *exception)
{
  char
    **attribute,
    **attributes,
    *tag,
    *utf8;

  int
    c,
    terminal;

  MagickBooleanType
    status;

  register char
    *p;

  register ssize_t
    i;

  size_t
    ignore_depth,
    length;

  ssize_t
    j,
    l;

  XMLTreeRoot
    *root;

  /*
    Convert xml-string to UTF8.
  */
  if ((xml == (const char *) NULL) || (strlen(xml) == 0))
    {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionWarning,
        "ParseError","root tag missing");
      return((XMLTreeInfo *) NULL);
    }
  root=(XMLTreeRoot *) NewXMLTreeTag((char *) NULL);
  length=strlen(xml);
  utf8=ConvertUTF16ToUTF8(xml,&length);
  if (utf8 == (char *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionWarning,
        "ParseError","UTF16 to UTF8 failed");
      return((XMLTreeInfo *) NULL);
    }
  terminal=utf8[length-1];
  utf8[length-1]='\0';
  p=utf8;
  while ((*p != '\0') && (*p != '<'))
    p++;
  if (*p == '\0')
    {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionWarning,
        "ParseError","root tag missing");
      utf8=DestroyString(utf8);
      return((XMLTreeInfo *) NULL);
    }
  attribute=(char **) NULL;
  l=0;
  ignore_depth=0;
  for (p++; ; p++)
  {
    attributes=(char **) sentinel;
    tag=p;
    c=(*p);
    if ((isalpha((int) ((unsigned char) *p)) !=0) || (*p == '_') ||
        (*p == ':') || (c < '\0'))
      {
        /*
          Tag.
        */
        if (root->node == (XMLTreeInfo *) NULL)
          {
            (void) ThrowMagickException(exception,GetMagickModule(),
              OptionWarning,"ParseError","root tag missing");
            utf8=DestroyString(utf8);
            return(&root->root);
          }
        p+=strcspn(p,XMLWhitespace "/>");
        while (isspace((int) ((unsigned char) *p)) != 0)
          *p++='\0';
        if (ignore_depth == 0)
          {
            if ((*p != '\0') && (*p != '/') && (*p != '>'))
              {
                /*
                  Find tag in default attributes list.
                */
                i=0;
                while ((root->attributes[i] != (char **) NULL) &&
                       (strcmp(root->attributes[i][0],tag) != 0))
                  i++;
                attribute=root->attributes[i];
              }
            for (l=0; (*p != '\0') && (*p != '/') && (*p != '>'); l+=2)
            {
              /*
                Attribute.
              */
              if (l == 0)
                attributes=(char **) AcquireQuantumMemory(4,
                  sizeof(*attributes));
              else
                attributes=(char **) ResizeQuantumMemory(attributes,
                  (size_t) (l+4),sizeof(*attributes));
              if (attributes == (char **) NULL)
                {
                  (void) ThrowMagickException(exception,GetMagickModule(),
                    ResourceLimitError,"MemoryAllocationFailed","`%s'","");
                  utf8=DestroyString(utf8);
                  return(&root->root);
                }
              attributes[l+2]=(char *) NULL;
              attributes[l+1]=(char *) NULL;
              attributes[l]=p;
              p+=strcspn(p,XMLWhitespace "=/>");
              if ((*p != '=') && (isspace((int) ((unsigned char) *p)) == 0))
                attributes[l]=ConstantString("");
              else
                {
                  *p++='\0';
                  p+=strspn(p,XMLWhitespace "=");
                  c=(*p);
                  if ((c == '"') || (c == '\''))
                    {
                      /*
                        Attributes value.
                      */
                      p++;
                      attributes[l+1]=p;
                      while ((*p != '\0') && (*p != c))
                        p++;
                      if (*p != '\0')
                        *p++='\0';
                      else
                        {
                          attributes[l]=ConstantString("");
                          attributes[l+1]=ConstantString("");
                          (void) DestroyXMLTreeAttributes(attributes);
                          (void) ThrowMagickException(exception,
                            GetMagickModule(),OptionWarning,"ParseError",
                            "missing %c",c);
                          utf8=DestroyString(utf8);
                          return(&root->root);
                        }
                      j=1;
                      while ((attribute != (char **) NULL) &&
                             (attribute[j] != (char *) NULL) &&
                             (strcmp(attribute[j],attributes[l]) != 0))
                        j+=3;
                      attributes[l+1]=ParseEntities(attributes[l+1],
                        root->entities,(attribute != (char **) NULL) &&
                        (attribute[j] != (char *) NULL) ? *attribute[j+2] :
                        ' ');
                    }
                  attributes[l]=ConstantString(attributes[l]);
                }
              while (isspace((int) ((unsigned char) *p)) != 0)
                p++;
            }
          }
        else
          {
            while((*p != '\0') && (*p != '/') && (*p != '>'))
              p++;
          }
        if (*p == '/')
          {
            /*
              Self closing tag.
            */
            *p++='\0';
            if (((*p != '\0') && (*p != '>')) ||
                ((*p == '\0') && (terminal != '>')))
              {
                if (l != 0)
                  (void) DestroyXMLTreeAttributes(attributes);
                (void) ThrowMagickException(exception,GetMagickModule(),
                  OptionWarning,"ParseError","missing >");
                utf8=DestroyString(utf8);
                return(&root->root);
              }
            if ((ignore_depth == 0) && (IsSkipTag(tag) == MagickFalse))
              {
                ParseOpenTag(root,tag,attributes);
                (void) ParseCloseTag(root,tag,exception);
              }
          }
        else
          {
            c=(*p);
            if ((*p == '>') || ((*p == '\0') && (terminal == '>')))
              {
                *p='\0';
                 if ((ignore_depth == 0) && (IsSkipTag(tag) == MagickFalse))
                   ParseOpenTag(root,tag,attributes);
                 else
                  {
                    ignore_depth++;
                    (void) DestroyXMLTreeAttributes(attributes);
                  }
                 *p=c;
               }
             else
              {
                if (l != 0)
                  (void) DestroyXMLTreeAttributes(attributes);
                (void) ThrowMagickException(exception,GetMagickModule(),
                  OptionWarning,"ParseError","missing >");
                utf8=DestroyString(utf8);
                return(&root->root);
              }
          }
      }
    else
      if (*p == '/')
        {
          /*
            Close tag.
          */
          tag=p+1;
          p+=strcspn(tag,XMLWhitespace ">")+1;
          c=(*p);
          if ((c == '\0') && (terminal != '>'))
            {
              (void) ThrowMagickException(exception,GetMagickModule(),
                OptionWarning,"ParseError","missing >");
              utf8=DestroyString(utf8);
              return(&root->root);
            }
          *p='\0';
          if (ignore_depth == 0 && ParseCloseTag(root,tag,exception) !=
              (XMLTreeInfo *) NULL)
            {
              utf8=DestroyString(utf8);
              return(&root->root);
            }
          if (ignore_depth > 0)
            ignore_depth--;
          *p=c;
          if (isspace((int) ((unsigned char) *p)) != 0)
            p+=strspn(p,XMLWhitespace);
        }
      else
        if (strncmp(p,"!--",3) == 0)
          {
            /*
              Comment.
            */
            p=strstr(p+3,"--");
            if ((p == (char *) NULL) || ((*(p+=2) != '>') && (*p != '\0')) ||
                ((*p == '\0') && (terminal != '>')))
              {
                (void) ThrowMagickException(exception,GetMagickModule(),
                  OptionWarning,"ParseError","unclosed <!--");
                utf8=DestroyString(utf8);
                return(&root->root);
              }
          }
        else
          if (strncmp(p,"![CDATA[",8) == 0)
            {
              /*
                Cdata.
              */
              p=strstr(p,"]]>");
              if (p != (char *) NULL)
                {
                  p+=2;
                  if (ignore_depth == 0)
                    ParseCharacterContent(root,tag+8,(size_t) (p-tag-10),'c');
                }
              else
                {
                  (void) ThrowMagickException(exception,GetMagickModule(),
                    OptionWarning,"ParseError","unclosed <![CDATA[");
                  utf8=DestroyString(utf8);
                  return(&root->root);
                }
            }
          else
            if (strncmp(p,"!DOCTYPE",8) == 0)
              {
                /*
                  DTD.
                */
                for (l=0; (*p != '\0') && (((l == 0) && (*p != '>')) ||
                     ((l != 0) && ((*p != ']') ||
                     (*(p+strspn(p+1,XMLWhitespace)+1) != '>'))));
                  l=(ssize_t) ((*p == '[') ? 1 : l))
                p+=strcspn(p+1,"[]>")+1;
                if ((*p == '\0') && (terminal != '>'))
                  {
                    (void) ThrowMagickException(exception,GetMagickModule(),
                      OptionWarning,"ParseError","unclosed <!DOCTYPE");
                    utf8=DestroyString(utf8);
                    return(&root->root);
                  }
                if (l != 0)
                  tag=strchr(tag,'[')+1;
                if (l != 0)
                  {
                    status=ParseInternalDoctype(root,tag,(size_t) (p-tag),
                      exception);
                    if (status == MagickFalse)
                      {
                        utf8=DestroyString(utf8);
                        return(&root->root);
                      }
                    p++;
                  }
              }
            else
              if (*p == '?')
                {
                  /*
                    Processing instructions.
                  */
                  do
                  {
                    p=strchr(p,'?');
                    if (p == (char *) NULL)
                      break;
                    p++;
                  } while ((*p != '\0') && (*p != '>'));
                  if ((p == (char *) NULL) || ((*p == '\0') &&
                      (terminal != '>')))
                    {
                      (void) ThrowMagickException(exception,GetMagickModule(),
                        OptionWarning,"ParseError","unclosed <?");
                      utf8=DestroyString(utf8);
                      return(&root->root);
                    }
                  ParseProcessingInstructions(root,tag+1,(size_t) (p-tag-2));
                }
              else
                {
                  (void) ThrowMagickException(exception,GetMagickModule(),
                    OptionWarning,"ParseError","unexpected <");
                  utf8=DestroyString(utf8);
                  return(&root->root);
                }
     if ((p == (char *) NULL) || (*p == '\0'))
       break;
     *p++='\0';
     tag=p;
     if ((*p != '\0') && (*p != '<'))
       {
        /*
          Tag character content.
        */
        while ((*p != '\0') && (*p != '<'))
          p++;
        if (*p == '\0')
          break;
        if (ignore_depth == 0)
          ParseCharacterContent(root,tag,(size_t) (p-tag),'&');
      }
    else
      if (*p == '\0')
        break;
  }
  utf8=DestroyString(utf8);
  if (root->node == (XMLTreeInfo *) NULL)
    return(&root->root);
  if (root->node->tag == (char *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionWarning,
        "ParseError","root tag missing");
      return(&root->root);
    }
  (void) ThrowMagickException(exception,GetMagickModule(),OptionWarning,
    "ParseError","unclosed tag: `%s'",root->node->tag);
  return(&root->root);
}
