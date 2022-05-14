SRP_user_pwd *SRP_VBASE_get_by_user(SRP_VBASE *vb, char *username)
 {
     int i;
     SRP_user_pwd *user;
    unsigned char digv[SHA_DIGEST_LENGTH];
    unsigned char digs[SHA_DIGEST_LENGTH];
    EVP_MD_CTX ctxt;
 
     if (vb == NULL)
         return NULL;
     for (i = 0; i < sk_SRP_user_pwd_num(vb->users_pwd); i++) {
         user = sk_SRP_user_pwd_value(vb->users_pwd, i);
         if (strcmp(user->id, username) == 0)
             return user;
     }
     if ((vb->seed_key == NULL) ||
         (vb->default_g == NULL) || (vb->default_N == NULL))
         return NULL;
        if (!(len = t_fromb64(tmp, N)))
            goto err;
        N_bn = BN_bin2bn(tmp, len, NULL);
        if (!(len = t_fromb64(tmp, g)))
            goto err;
        g_bn = BN_bin2bn(tmp, len, NULL);
        defgNid = "*";
    } else {
        SRP_gN *gN = SRP_get_gN_by_id(g, NULL);
        if (gN == NULL)
            goto err;
        N_bn = gN->N;
        g_bn = gN->g;
        defgNid = gN->id;
    }
