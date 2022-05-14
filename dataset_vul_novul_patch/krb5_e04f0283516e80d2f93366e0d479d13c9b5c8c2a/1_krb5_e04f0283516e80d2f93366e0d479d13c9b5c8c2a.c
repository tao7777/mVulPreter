 static int gss_iakerbmechglue_init(void)
 {
     struct gss_mech_config mech_iakerb;
    struct gss_config iakerb_mechanism = krb5_mechanism;
    /* IAKERB mechanism mirrors krb5, but with different context SPIs */
    iakerb_mechanism.gss_accept_sec_context = iakerb_gss_accept_sec_context;
    iakerb_mechanism.gss_init_sec_context   = iakerb_gss_init_sec_context;
    iakerb_mechanism.gss_delete_sec_context = iakerb_gss_delete_sec_context;
    iakerb_mechanism.gss_acquire_cred       = iakerb_gss_acquire_cred;
    iakerb_mechanism.gssspi_acquire_cred_with_password
                                    = iakerb_gss_acquire_cred_with_password;
 
     memset(&mech_iakerb, 0, sizeof(mech_iakerb));
     mech_iakerb.mech = &iakerb_mechanism;

    mech_iakerb.mechNameStr = "iakerb";
    mech_iakerb.mech_type = (gss_OID)gss_mech_iakerb;
    gssint_register_mechinfo(&mech_iakerb);

    return 0;
}
