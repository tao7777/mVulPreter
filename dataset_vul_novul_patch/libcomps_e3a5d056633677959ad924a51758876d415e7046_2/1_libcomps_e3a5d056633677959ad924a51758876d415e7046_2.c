void comps_rtree_unite(COMPS_RTree *rt1, COMPS_RTree *rt2) {
    COMPS_HSList *tmplist, *tmp_subnodes;
    COMPS_HSListItem *it;
     struct Pair {
         COMPS_HSList * subnodes;
         char * key;
        char added;
     } *pair, *parent_pair;
 
     pair = malloc(sizeof(struct Pair));
    pair->subnodes = rt2->subnodes;
    pair->key = NULL;

    tmplist = comps_hslist_create();
    comps_hslist_init(tmplist, NULL, NULL, &free);
    comps_hslist_append(tmplist, pair, 0);

    while (tmplist->first != NULL) {
        it = tmplist->first;
        comps_hslist_remove(tmplist, tmplist->first);
        tmp_subnodes = ((struct Pair*)it->data)->subnodes;
        parent_pair = (struct Pair*) it->data;
        free(it);

        for (it = tmp_subnodes->first; it != NULL; it=it->next) {
            pair = malloc(sizeof(struct Pair));
            pair->subnodes = ((COMPS_RTreeData*)it->data)->subnodes;

            if (parent_pair->key != NULL) {
                pair->key = malloc(sizeof(char)
                               * (strlen(((COMPS_RTreeData*)it->data)->key)
                               + strlen(parent_pair->key) + 1));
                memcpy(pair->key, parent_pair->key,
                       sizeof(char) * strlen(parent_pair->key));
                memcpy(pair->key + strlen(parent_pair->key),
                       ((COMPS_RTreeData*)it->data)->key,
                       sizeof(char)*(strlen(((COMPS_RTreeData*)it->data)->key)+1));
            } else {
                pair->key = malloc(sizeof(char)*
                                (strlen(((COMPS_RTreeData*)it->data)->key) +1));
                memcpy(pair->key, ((COMPS_RTreeData*)it->data)->key,
                       sizeof(char)*(strlen(((COMPS_RTreeData*)it->data)->key)+1));
            }
            /* current node has data */
            if (((COMPS_RTreeData*)it->data)->data != NULL) {
                    comps_rtree_set(rt1,
                                    pair->key,
                        rt2->data_cloner(((COMPS_RTreeData*)it->data)->data));
            }
            if (((COMPS_RTreeData*)it->data)->subnodes->first) {
                comps_hslist_append(tmplist, pair, 0);
            } else {
                free(pair->key);
                free(pair);
            }
        }
        free(parent_pair->key);
        free(parent_pair);
    }
    comps_hslist_destroy(&tmplist);
}
