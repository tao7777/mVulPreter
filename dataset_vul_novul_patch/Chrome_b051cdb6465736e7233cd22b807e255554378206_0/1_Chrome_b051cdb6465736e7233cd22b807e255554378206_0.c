int NoOpVerifyCallback(X509_STORE_CTX*, void *) {
  DVLOG(3) << "skipping cert verify";
  return 1;
}
