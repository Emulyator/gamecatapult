/*********************************************************************/
/*                                                                   */
/* FILE :        lws2dx.c                                            */
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
	fprintf(stderr, "lws2dx  ver.1.3 (c) 2001, D-STORM,Inc.\n");
	fprintf(stderr, "lws2dx  [options] input.lws [-o output.x]\n");
	fprintf(stderr, "-b  :  binary format.\n");
	fprintf(stderr, "-Oc :  enable output of vertex colors.\n");
	fprintf(stderr, "-Oa :  disable output of animations.\n");
	fprintf(stderr, "-Of :  disable output of frames.\n");
	fprintf(stderr, "-On :  disable output of mesh normals.\n");
	fprintf(stderr, "-Ou :  disable output of texture coordinates.\n");
	fprintf(stderr, "-Om :  disable output of materials.\n");
	fprintf(stderr, "-Ot :  enable output of templates.\n");
	fprintf(stderr, "-Tr :  reverse texture V coordinates.\n");
	fprintf(stderr, "-Me :  external materials.\n");
	fprintf(stderr, "-Ms :  shared external materials.\n");
	fprintf(stderr, "-v7 :  DirectX 7 compatible.\n");
	fprintf(stderr, "-L  :  enable output of light frames.\n");
	fprintf(stderr, "-C  :  enable output of camera frames.\n");
	fprintf(stderr, "-Fk :  export only keyframes (default).\n");
	fprintf(stderr, "-Fs :  export sampling frames.\n");
	fprintf(stderr, "-Fb :  export keyframes and sampling frames.\n");
	fprintf(stderr, "-R  :  remove unusable keyframes.\n");
	fprintf(stderr, "-mk :  use matrix key.\n");
	fprintf(stderr, "-d content : specify the content directory.\n");
	fprintf(stderr, "-f steps   : frame steps.\n");
	fprintf(stderr, "-p name    : frame prefix name.\n");
	fprintf(stderr, "-m nMax    : max of skined weights per vertex.\n");
}


main( int argc, char **argv )
{
	SCN3_ID        scn;
	OBJ2_option    opt;
	char*          infile = NULL;

	if (argc < 2) {
		Usage();
		exit(1);
	}

	dxOptions  options( DX_STATUS_SCN2DX );
	options.outputAnimations = TRUE;
	options.outputFrames     = TRUE;

	memset( &opt, 0x00, sizeof(OBJ2_option) );

	for (int n = 1; n < argc; n++)
	{
		if (argv[n][0] == '-')
		{
			if (strcmp( argv[n], "-b" ) == 0) {
				options.format = DX_FORMAT_BINARY;
			}
			else if (strcmp( argv[n], "-Me" ) == 0) {
				options.materialType = DX_MATTYPE_EXTERN;
			}
			else if (strcmp( argv[n], "-Ms" ) == 0) {
				options.materialType = DX_MATTYPE_SHARED;
			}
			else if (strcmp( argv[n], "-Oc" ) == 0) {
				options.outputVertexColors = TRUE;
			}
			else if (strcmp( argv[n], "-Oa" ) == 0) {
				options.outputAnimations   = FALSE;
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
				options.version  = DX_VERSION_DIRECTX7;
			}
			else if (strcmp( argv[n], "-L" ) == 0) {
				options.outputLightFrames  = TRUE;
			}
			else if (strcmp( argv[n], "-C" ) == 0) {
				options.outputCameraFrames = TRUE;
			}
			else if (strcmp( argv[n], "-d" ) == 0) {
				strcpy( opt.content, argv[++n] );
			}
			else if (strcmp( argv[n], "-f" ) == 0) {
				options.frameSteps = atoi(argv[++n]);
			}
			else if (strcmp( argv[n], "-p" ) == 0) {
				strcpy( options.framePrefix, argv[++n] );
			}
			else if (strcmp( argv[n], "-m" ) == 0) {
				options.useSkinWeightsPerVertex  = TRUE;
				options.nMaxSkinWeightsPerVertex = atoi(argv[++n]);
			}
			else if (strcmp( argv[n], "-Fk" ) == 0) {
				options.frameToExport  = DX_FRM2EXP_KEYFRAMEONLY;
			}
			else if (strcmp( argv[n], "-Fs" ) == 0) {
				options.frameToExport  = DX_FRM2EXP_ALWAYSSAMPLE;
			}
			else if (strcmp( argv[n], "-Fb" ) == 0) {
				options.frameToExport  = DX_FRM2EXP_KEYANDSAMPLE;
			}
			else if (strcmp( argv[n], "-R" ) == 0) {
				options.removeUnusableKeyframes = TRUE;
			}
			else if (strcmp( argv[n], "-mk" ) == 0) {
				options.useMatrixKey  = TRUE;
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

	opt.ope_code      = OPT_OPE_TRIPLE|OPT_OPE_SORT;
	if (options.outputMeshNormals)   opt.ope_code |= OPT_OPE_NORMAL;
	if (options.outputTextureCoords) opt.ope_code |= OPT_OPE_UVMAP;
	if (options.outputVertexColors)  opt.ope_code |= OPT_OPE_COLOR;
	opt.uvmap_channel = MAT_CHAN_COLR|MAT_CHAN_DIFF;
	opt.uvmap_nlayrs  = 1;
	opt.uvmap_flags   = OPT_UV_NONE;
	opt.color_flags   = OPT_COL_NONE;
	opt.triple_flags  = OPT_TRI_CONCAVE;
	opt.color_flags   = OPT_COL_NONE;

	scn = SCN3_open( infile, SCN_FLAG_OBJ2OPEN|SCN_FLAG_MOT2CHAN, &opt );
	if (scn->error != SCN_NO_ERROR) {
		fprintf( stderr, "ERR %d <%s>\n", scn->error, scn->errfile );
		exit(1);
	}

	dxExporter dxs( &options, NULL );

	if (dxs.ExportScene( scn ) == false) {
		fprintf( stderr, "Exporting Failed with %s (%d).", dxs._errCode, dxs._errFile );
		exit(1);
	}

	SCN3_close( &scn );

	return (0);
}

