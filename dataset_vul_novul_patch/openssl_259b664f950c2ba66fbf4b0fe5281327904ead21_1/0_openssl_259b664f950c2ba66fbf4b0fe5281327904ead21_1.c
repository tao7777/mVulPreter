static void SRP_user_pwd_free(SRP_user_pwd *user_pwd)
void SRP_user_pwd_free(SRP_user_pwd *user_pwd)
 {
     if (user_pwd == NULL)
         return;
    BN_free(user_pwd->s);
    BN_clear_free(user_pwd->v);
    OPENSSL_free(user_pwd->id);
    OPENSSL_free(user_pwd->info);
    OPENSSL_free(user_pwd);
}
