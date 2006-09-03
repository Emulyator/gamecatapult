/*********************************************************************/
/*                                                                   */
/* FILE :        lwo2dx.cpp                                          */
/*                                                                   */
/* DESCRIPTION : LightWave 3D to DirectX Export                      */
/*                                                                   */
/* HISTORY :     Oct 29, 2000    written by Yoshiaki Tazaki          */
/*               Aug 08, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000-2001, D-STORM, Inc.                            */
/*                                                                   */
/*********************************************************************/

#include "dxexp.h"

static void Usage( void )
{
	fprintf(stderr, "lwo2dx  ver.1.2  (c) 2001, D-STORM, Inc.\n");
	fprintf(stderr, "lwo2dx  [options] input.lwo [-o output.x]\n");
	fprintf(stderr, "-b  :  binary format.\n");
	fprintf(stderr, "-Oc :  enable output of vertex colors.\n");
	fprintf(stderr, "-Of :  disable output of frames.\n");
	fprintf(stderr, "-On :  disable output of mesh normals.\n");
	fprintf(stderr, "-Ou :  disable output of texture coordinates.\n");
	fprintf(stderr, "-Om :  disable output of materials.\n");
	fprintf(stderr, "-Ot :  enable output of templates.\n");
	fprintf(stderr, "-Tr :  reverse texture V coordinates.\n");
	fprintf(stderr, "-v7 :  DirectX 7 compatible.\n");
	fprintf(stderr, "-s scale :  scale positions.\n");
	fprintf(stderr, "-p name : frame prefix name.\n");
}


main( int argc, char **argv )
{
    OBJ2_ID        obj;
	OBJ2_option    opt;
	char*          infile = NULL;

	if (argc < 2) {
		Usage();
		exit(1);
	}

	dxOptions  options( DX_STATUS_OBJ2DX );

	memset( &opt, 0x00, sizeof(OBJ2_option) );

	for (int n = 1; n < argc; n++)
	{
		if (argv[n][0] == '-')
		{
			if (strcmp( argv[n], "-b" ) == 0) {
				options.format = DX_FORMAT_BINARY;
			}
			else if (strcmp( argv[n], "-Oc" ) == 0) {
				options.outputVertexColors = TRUE;
			}
			else if (strcmp( argv[n], "-Of" ) == 0) {
				options.outputFrames       = FALSE;
			}
			else if (strcmp( argv[n], "-On" ) == 0) {
				options.outputMeshNormals  = FALSE;
			}
			else if (strcmp( argv[n], "-Ou" ) == 0) {
				options.outputTextureCoords  = FALSE;
			}
			else if (strcmp( argv[n], "-Om" ) == 0) {
				options.outputMaterials  = FALSE;
			}
			else if (strcmp( argv[n], "-Ot" ) == 0) {
				options.outputTemplates  = TRUE;
			}
			else if (strcmp( argv[n], "-Tr" ) == 0) {
				options.reverseUVCoords    = TRUE;
			}
			else if (strcmp( argv[n], "-v7" ) == 0) {
				options.version    = DX_VERSION_DIRECTX7;
			}
			else if (strcmp( argv[n], "-s" ) == 0) {
				options.scale = atof(argv[++n]);
			}
			else if (strcmp( argv[n], "-p" ) == 0) {
				strcpy( options.framePrefix, argv[++n] );
			}
			else if (strcmp( argv[n], "-?" ) == 0 ||
					 strcmp( argv[n], "-h" ) == 0) {
				Usage();
				exit(1);
			}
			else {
				fprintf( stderr, "Unknown option specified (%s).\n\n", argv[n] );
				Usage();
				exit(1);
			}
		}
		else if (infile == NULL) {
			infile = argv[n];
		}
	}

	if (infile == NULL) {
		fprintf(stderr, "Error: No input file.\n");
		exit(1);
	}

	opt.ope_code      = OPT_OPE_TRIPLE;
	if (options.outputMeshNormals)   opt.ope_code |= OPT_OPE_NORMAL;
	if (options.outputTextureCoords) opt.ope_code |= OPT_OPE_UVMAP;
	if (options.outputVertexColors)  opt.ope_code |= OPT_OPE_COLOR;
	opt.uvmap_channel = MAT_CHAN_COLR|MAT_CHAN_DIFF;
	opt.uvmap_nlayrs  = 1;
	opt.uvmap_flags   = OPT_UV_NONE;
	opt.color_flags   = OPT_COL_NONE;
	opt.triple_flags  = OPT_TRI_CONCAVE;

    obj = OBJ2_open( infile, &opt );
	if (!obj) {
		fprintf(stderr, "OBJECT OPEN ERROR (%s)\n", infile);
		exit(1);
	}

	dxExporter dxs( &options, NULL );

	if (dxs.ExportObject( obj ) == false) {
		fprintf( stderr, "Exporting Failed with %s (%d).", dxs._errCode, dxs._errFile );
		exit(1);
	}

	OBJ2_close( &obj );

	return (0);
}

