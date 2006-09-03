#include <lw/scn3.h>
#include <lw/obj2.h>
#include <lw/lwo2.h>

#if 0

main( int argc, char **argv )
{
	FILE        *fp;
    OBJ2        *obj;
    int          i, ss = FALSE;
	char        *infile = NULL, *outfile = NULL;
	OBJType		 type = UNKNOWN_FILE;
	OBJ2_option  opt;
	MeshOpt     *mesh_opt = NULL;

	memset( &opt, 0x00, sizeof(OBJ2_option) );

	if (argc < 2) { 
		printf("Object Library 2  %s\n", OBJ_VERSION);
		printf("obj2  [-estnmdc] src [dst]\n");
		printf("src : (.lwo) lightwave.\n");
		printf("dst : (.lwo) lightwave.\n");
		printf("-e : make winged-edge structure.\n");
		printf("-s : sort polygons by surface.\n");
		printf("-t : triple all of polygons.\n");
		printf("-m : calc uv mapping datas.\n");
		printf("-n : calc vertex normals.\n");
		printf("-c : vertex color conversion.\n");
		printf("-M : make strip meshes.\n");
		printf("-S : sort surfaces.\n");
		exit(1);
	}

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (strchr( argv[i], 's' )) {
        		opt.ope_code |= OPT_OPE_SORT;
			} 
			if (strchr( argv[i], 't' )) {
        		opt.ope_code |= OPT_OPE_TRIPLE;
				opt.triple_flags  = OPT_TRI_NONE;
			}
			if (strchr( argv[i], 'm' )) {
        		opt.ope_code     |= OPT_OPE_UVMAP;
				opt.uvmap_channel = MAT_CHAN_ALL;
				opt.uvmap_nlayrs  = 10;
				opt.uvmap_flags   = OPT_UV_NONE;
			}
			if (strchr( argv[i], 'c' )) {
        		opt.ope_code |= OPT_OPE_COLOR;
				opt.color_flags   = 0;
			}
			if (strchr( argv[i], 'n' )) {
        		opt.ope_code |= OPT_OPE_NORMAL;
			}
			if (strchr( argv[i], 'e' )) {
        		opt.ope_code |= OPT_OPE_EDGE;
			}
			if (strchr( argv[i], '5' )) {
        		type = LWOB_FILE;
			}
			if (strchr( argv[i], '6' )) {
        		type = LWO2_FILE;
			}
			if (strchr( argv[i], 'M' )) {
				mesh_opt = (MeshOpt *) calloc( sizeof(MeshOpt), 1 );
				mesh_opt->flags    = OBJ_MESH_SORT|OBJ_MESH_SEAM|OBJ_MESH_TRIFAN;
				mesh_opt->level    = OBJ_MESH_STRIP1;
				mesh_opt->max_node = 0;
				mesh_opt->min_fan  = 8;
				mesh_opt->selpols  = NULL;
			}
			if (strchr( argv[i], 'S' )) {
        		ss = TRUE;
			}
		}
		else if (infile == NULL) {
			infile = argv[i];
		}
		else if (outfile == NULL) {
			outfile = argv[i];
		}
	}

    obj = OBJ2_open( infile, &opt );
	if (!obj) {
		printf("OBJECT OPEN ERROR (%s)\n", infile);
		exit(1);
	}
	(*obj->calcBBox)( obj );

	if (mesh_opt) {
		(*obj->makeMeshes)( obj, mesh_opt );
	}

	if (ss) {
		(*obj->sortSurfs)( obj );
	}
	if (outfile == NULL) {
    	(*obj->dumpObj)( obj, stdout );
	}
	else if (strstr( outfile, ".lwo" )) {
		if (type == 0L) {
			type = obj->type;
		}
		(*obj->saveObj)( obj, outfile, type, NULL );
	}
	else if (strstr( outfile, ".txt" )) {
    	fp = fopen( outfile, "wt" );
    	(*obj->dumpObj)( obj, fp );
		fclose( fp );
	}
	if (obj->error != OBJ_NO_ERROR) {
		printf("%s. (%s)\n", (*obj->errorStr)(obj), obj->errfile);
	}

	OBJ2_close( &obj );

	return (0);
}

#else

int main( int argc, char **argv )
{
	SCN3  *scn;
	int    i, flags = 0L;
	char  *filename = NULL;

	OBJ2_option    opt;
	PathConvParam  param;

	if (argc < 2) {
		fprintf(stderr, "scn3 [-o] scene.lws\n");
		exit(1);
	}

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-') {
			if (strchr(argv[i], 'o')) flags |= SCN_FLAG_OBJ2OPEN;
		} else {
			filename = argv[i];
		}
	}

	opt.ope_code      = OPT_OPE_UVMAP|OPT_OPE_NORMAL|OPT_OPE_COLOR;
	opt.format_flags  = 0L;
	opt.normal_flags  = OPT_NORM_COMPRESS;
	opt.uvmap_flags   = OPT_UV_COMPRESS|OPT_UV_UVIEW;
	opt.uvmap_channel = MAT_CHAN_COLR;
	opt.uvmap_nlayrs  = 1;
	opt.color_flags   = OPT_COL_COMPRESS;
	opt.triple_flags  = OPT_TRI_NONE;

	param.path_in   = filename;
	param.directory = opt.content;
	PathConvert( PATHCONV_GETDIRECTORY, &param );

	scn = SCN3_open( filename, flags, &opt );
	if (scn->error != SCN_NO_ERROR) {
		fprintf( stderr, "* ERROR CODE [%d] <%s>\n", scn->error, scn->errfile );
		return 1;
	}
	(*scn->dump)( scn, stdout );
	SCN3_close( &scn );

	return 0;
}

#endif
