opj_pi_iterator_t *opj_pi_create_decode(opj_image_t *p_image,
										opj_cp_t *p_cp,
										OPJ_UINT32 p_tile_no)
{
	/* loop */
	OPJ_UINT32 pino;
	OPJ_UINT32 compno, resno;

	/* to store w, h, dx and dy fro all components and resolutions */
	OPJ_UINT32 * l_tmp_data;
	OPJ_UINT32 ** l_tmp_ptr;

	/* encoding prameters to set */
	OPJ_UINT32 l_max_res;
	OPJ_UINT32 l_max_prec;
	OPJ_INT32 l_tx0,l_tx1,l_ty0,l_ty1;
	OPJ_UINT32 l_dx_min,l_dy_min;
	OPJ_UINT32 l_bound;
	OPJ_UINT32 l_step_p , l_step_c , l_step_r , l_step_l ;
	OPJ_UINT32 l_data_stride;

	/* pointers */
	opj_pi_iterator_t *l_pi = 00;
	opj_tcp_t *l_tcp = 00;
	const opj_tccp_t *l_tccp = 00;
	opj_pi_comp_t *l_current_comp = 00;
	opj_image_comp_t * l_img_comp = 00;
	opj_pi_iterator_t * l_current_pi = 00;
	OPJ_UINT32 * l_encoding_value_ptr = 00;

	/* preconditions in debug */
	assert(p_cp != 00);
	assert(p_image != 00);
	assert(p_tile_no < p_cp->tw * p_cp->th);

	/* initializations */
	l_tcp = &p_cp->tcps[p_tile_no];
	l_bound = l_tcp->numpocs+1;

	l_data_stride = 4 * OPJ_J2K_MAXRLVLS;
	l_tmp_data = (OPJ_UINT32*)opj_malloc(
		l_data_stride * p_image->numcomps * sizeof(OPJ_UINT32));
	if
		(! l_tmp_data)
	{
		return 00;
	}
	l_tmp_ptr = (OPJ_UINT32**)opj_malloc(
		p_image->numcomps * sizeof(OPJ_UINT32 *));
	if
		(! l_tmp_ptr)
	{
		opj_free(l_tmp_data);
		return 00;
	}

	/* memory allocation for pi */
	l_pi = opj_pi_create(p_image, p_cp, p_tile_no);
	if (!l_pi) {
		opj_free(l_tmp_data);
		opj_free(l_tmp_ptr);
		return 00;
	}

	l_encoding_value_ptr = l_tmp_data;
	/* update pointer array */
	for
		(compno = 0; compno < p_image->numcomps; ++compno)
	{
		l_tmp_ptr[compno] = l_encoding_value_ptr;
		l_encoding_value_ptr += l_data_stride;
	}
	/* get encoding parameters */
	opj_get_all_encoding_parameters(p_image,p_cp,p_tile_no,&l_tx0,&l_tx1,&l_ty0,&l_ty1,&l_dx_min,&l_dy_min,&l_max_prec,&l_max_res,l_tmp_ptr);

	/* step calculations */
	l_step_p = 1;
	l_step_c = l_max_prec * l_step_p;
	l_step_r = p_image->numcomps * l_step_c;
	l_step_l = l_max_res * l_step_r;

	/* set values for first packet iterator */
	l_current_pi = l_pi;

	/* memory allocation for include */
	/* prevent an integer overflow issue */
 	l_current_pi->include = 00;
 	if (l_step_l <= (SIZE_MAX / (l_tcp->numlayers + 1U)))
 	{
		l_current_pi->include = (OPJ_INT16*) opj_calloc((size_t)(l_tcp->numlayers + 1U) * l_step_l, sizeof(OPJ_INT16));
 	}
 
 	if
		(!l_current_pi->include)
	{
		opj_free(l_tmp_data);
		opj_free(l_tmp_ptr);
		opj_pi_destroy(l_pi, l_bound);
		return 00;
	}

	/* special treatment for the first packet iterator */
	l_current_comp = l_current_pi->comps;
	l_img_comp = p_image->comps;
	l_tccp = l_tcp->tccps;

	l_current_pi->tx0 = l_tx0;
	l_current_pi->ty0 = l_ty0;
	l_current_pi->tx1 = l_tx1;
	l_current_pi->ty1 = l_ty1;

	/*l_current_pi->dx = l_img_comp->dx;*/
	/*l_current_pi->dy = l_img_comp->dy;*/

	l_current_pi->step_p = l_step_p;
	l_current_pi->step_c = l_step_c;
	l_current_pi->step_r = l_step_r;
	l_current_pi->step_l = l_step_l;

	/* allocation for components and number of components has already been calculated by opj_pi_create */
	for
		(compno = 0; compno < l_current_pi->numcomps; ++compno)
	{
		opj_pi_resolution_t *l_res = l_current_comp->resolutions;
		l_encoding_value_ptr = l_tmp_ptr[compno];

		l_current_comp->dx = l_img_comp->dx;
		l_current_comp->dy = l_img_comp->dy;
		/* resolutions have already been initialized */
		for
			(resno = 0; resno < l_current_comp->numresolutions; resno++)
		{
			l_res->pdx = *(l_encoding_value_ptr++);
			l_res->pdy = *(l_encoding_value_ptr++);
			l_res->pw =  *(l_encoding_value_ptr++);
			l_res->ph =  *(l_encoding_value_ptr++);
			++l_res;
		}
		++l_current_comp;
		++l_img_comp;
		++l_tccp;
	}
	++l_current_pi;

	for (pino = 1 ; pino<l_bound ; ++pino )
	{
		l_current_comp = l_current_pi->comps;
		l_img_comp = p_image->comps;
		l_tccp = l_tcp->tccps;

		l_current_pi->tx0 = l_tx0;
		l_current_pi->ty0 = l_ty0;
		l_current_pi->tx1 = l_tx1;
		l_current_pi->ty1 = l_ty1;
		/*l_current_pi->dx = l_dx_min;*/
		/*l_current_pi->dy = l_dy_min;*/
		l_current_pi->step_p = l_step_p;
		l_current_pi->step_c = l_step_c;
		l_current_pi->step_r = l_step_r;
		l_current_pi->step_l = l_step_l;

		/* allocation for components and number of components has already been calculated by opj_pi_create */
		for
			(compno = 0; compno < l_current_pi->numcomps; ++compno)
		{
			opj_pi_resolution_t *l_res = l_current_comp->resolutions;
			l_encoding_value_ptr = l_tmp_ptr[compno];

			l_current_comp->dx = l_img_comp->dx;
			l_current_comp->dy = l_img_comp->dy;
			/* resolutions have already been initialized */
			for
				(resno = 0; resno < l_current_comp->numresolutions; resno++)
			{
				l_res->pdx = *(l_encoding_value_ptr++);
				l_res->pdy = *(l_encoding_value_ptr++);
				l_res->pw =  *(l_encoding_value_ptr++);
				l_res->ph =  *(l_encoding_value_ptr++);
				++l_res;
			}
			++l_current_comp;
			++l_img_comp;
			++l_tccp;
		}
		/* special treatment*/
		l_current_pi->include = (l_current_pi-1)->include;
		++l_current_pi;
	}
	opj_free(l_tmp_data);
	l_tmp_data = 00;
	opj_free(l_tmp_ptr);
	l_tmp_ptr = 00;
	if
		(l_tcp->POC)
	{
		opj_pi_update_decode_poc (l_pi,l_tcp,l_max_prec,l_max_res);
	}
	else
	{
		opj_pi_update_decode_not_poc(l_pi,l_tcp,l_max_prec,l_max_res);
	}
	return l_pi;
}
