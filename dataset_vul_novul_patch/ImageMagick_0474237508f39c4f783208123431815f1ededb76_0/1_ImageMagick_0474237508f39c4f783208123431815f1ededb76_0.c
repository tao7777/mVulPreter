static MagickBooleanType CheckMemoryOverflow(const size_t count,
  const size_t quantum)
{
  size_t
    size;
  size=count*quantum;
  if ((count == 0) || (quantum != (size/count)))
    {
      errno=ENOMEM;
      return(MagickTrue);
    }
  return(MagickFalse);
}
