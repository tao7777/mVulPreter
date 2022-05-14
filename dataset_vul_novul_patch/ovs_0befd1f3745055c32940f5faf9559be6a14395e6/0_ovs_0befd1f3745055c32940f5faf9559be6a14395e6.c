    OVS_REQUIRES(ofproto_mutex)
{
     const struct rule_actions *actions = rule_get_actions(rule);
 
     /* A rule may not be reinserted. */
    ovs_assert(rule->state != RULE_INSERTED);
 
     if (rule->hard_timeout || rule->idle_timeout) {
         ovs_list_insert(&ofproto->expirable, &rule->expirable);
    }
    cookies_insert(ofproto, rule);
    eviction_group_add_rule(rule);
    if (actions->has_meter) {
        meter_insert_rule(rule);
    }
    if (actions->has_groups) {
        const struct ofpact_group *a;
        OFPACT_FOR_EACH_TYPE_FLATTENED (a, GROUP, actions->ofpacts,
                                        actions->ofpacts_len) {
            struct ofgroup *group;

            group = ofproto_group_lookup(ofproto, a->group_id, OVS_VERSION_MAX,
                                         false);
            ovs_assert(group != NULL);
            group_add_rule(group, rule);
        }
    }

    rule->state = RULE_INSERTED;
}
