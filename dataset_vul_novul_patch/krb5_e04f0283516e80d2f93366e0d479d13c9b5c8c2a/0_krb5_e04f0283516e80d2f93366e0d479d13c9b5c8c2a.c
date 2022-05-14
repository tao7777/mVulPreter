 static int gss_iakerbmechglue_init(void)
 {
     struct gss_mech_config mech_iakerb;
 
     memset(&mech_iakerb, 0, sizeof(mech_iakerb));
     mech_iakerb.mech = &iakerb_mechanism;

    mech_iakerb.mechNameStr = "iakerb";
    mech_iakerb.mech_type = (gss_OID)gss_mech_iakerb;
    gssint_register_mechinfo(&mech_iakerb);

    return 0;
}
