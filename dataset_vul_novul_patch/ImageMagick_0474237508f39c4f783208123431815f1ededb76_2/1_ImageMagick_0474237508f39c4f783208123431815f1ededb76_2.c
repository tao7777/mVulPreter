MagickExport void CatchException(ExceptionInfo *exception)
{
   register const ExceptionInfo
     *p;
 
   assert(exception != (ExceptionInfo *) NULL);
   assert(exception->signature == MagickSignature);
   if (exception->exceptions  == (void *) NULL)
    return;
  LockSemaphoreInfo(exception->semaphore);
   ResetLinkedListIterator((LinkedListInfo *) exception->exceptions);
   p=(const ExceptionInfo *) GetNextValueInLinkedList((LinkedListInfo *)
     exception->exceptions);
  while (p != (const ExceptionInfo *) NULL)
   {
    if ((p->severity >= WarningException) && (p->severity < ErrorException))
      MagickWarning(p->severity,p->reason,p->description);
    if ((p->severity >= ErrorException) && (p->severity < FatalErrorException))
      MagickError(p->severity,p->reason,p->description);
     if (p->severity >= FatalErrorException)
       MagickFatalError(p->severity,p->reason,p->description);
     p=(const ExceptionInfo *) GetNextValueInLinkedList((LinkedListInfo *)
       exception->exceptions);
   }
  UnlockSemaphoreInfo(exception->semaphore);
  ClearMagickException(exception);
}
