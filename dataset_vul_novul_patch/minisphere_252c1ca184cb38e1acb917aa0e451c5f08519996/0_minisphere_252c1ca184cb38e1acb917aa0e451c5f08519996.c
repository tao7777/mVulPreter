layer_resize(int layer, int x_size, int y_size)
{
	int                 old_height;
	int                 old_width;
	struct map_tile*    tile;
	int                 tile_width;
	int                 tile_height;
 	struct map_tile*    tilemap;
 	struct map_trigger* trigger;
 	struct map_zone*    zone;
	size_t              tilemap_size;
 
 	int x, y, i;
 
	old_width = s_map->layers[layer].width;
	old_height = s_map->layers[layer].height;
 
	tilemap_size = x_size * y_size * sizeof(struct map_tile);
	if (x_size == 0 || tilemap_size / x_size / sizeof(struct map_tile) != y_size
		|| !(tilemap = malloc(tilemap_size)))
 		return false;
 	for (x = 0; x < x_size; ++x) {
 		for (y = 0; y < y_size; ++y) {
			if (x < old_width && y < old_height) {
				tilemap[x + y * x_size] = s_map->layers[layer].tilemap[x + y * old_width];
			}
			else {
				tile = &tilemap[x + y * x_size];
				tile->frames_left = tileset_get_delay(s_map->tileset, 0);
				tile->tile_index = 0;
			}
		}
	}

	free(s_map->layers[layer].tilemap);
	s_map->layers[layer].tilemap = tilemap;
	s_map->layers[layer].width = x_size;
	s_map->layers[layer].height = y_size;

	tileset_get_size(s_map->tileset, &tile_width, &tile_height);
	s_map->width = 0;
	s_map->height = 0;
	for (i = 0; i < s_map->num_layers; ++i) {
		if (!s_map->layers[i].is_parallax) {
			s_map->width = fmax(s_map->width, s_map->layers[i].width * tile_width);
			s_map->height = fmax(s_map->height, s_map->layers[i].height * tile_height);
		}
	}

	for (i = (int)vector_len(s_map->zones) - 1; i >= 0; --i) {
		zone = vector_get(s_map->zones, i);
		if (zone->bounds.x1 >= s_map->width || zone->bounds.y1 >= s_map->height)
			vector_remove(s_map->zones, i);
		else {
			if (zone->bounds.x2 > s_map->width)
				zone->bounds.x2 = s_map->width;
			if (zone->bounds.y2 > s_map->height)
				zone->bounds.y2 = s_map->height;
		}
	}
	for (i = (int)vector_len(s_map->triggers) - 1; i >= 0; --i) {
		trigger = vector_get(s_map->triggers, i);
		if (trigger->x >= s_map->width || trigger->y >= s_map->height)
			vector_remove(s_map->triggers, i);
	}

	return true;
}
