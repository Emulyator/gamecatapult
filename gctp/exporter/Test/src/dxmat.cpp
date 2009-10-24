/*********************************************************************/
/*                                                                   */
/* FILE :        dxmat.cpp                                           */
/*                                                                   */
/* DESCRIPTION : LightWave 3D to DirectX Export                      */
/*                                                                   */
/* HISTORY :     Oct 29, 2000    written by Yoshiaki Tazaki          */
/*               Oct 02, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000-2001, D-STORM, Inc.                            */
/*                                                                   */
/*********************************************************************/
#include <gctp/vector.hpp>

#include "dxexp.h"

static void DX_ConvertTextureName( char* outfile, char* infile );
static void DX_ConvertString     ( char* outfile, char* infile );
static void DX_MakeRelativePath  ( char* outfile, char* infile, char* directory );

/*
 *
 *  MakeMaterials   - make materials from the object
 *
 */

DX_Material* dxExporter::MakeMaterials( DX_Material* dx_mats, LWW::MeshInfo mesh, bool *uselist )
{
	DX_Material*    dx_mat;
	DX_Material*    dx_fst = NULL;
	DX_Material*    dx_lst = NULL;
	DX_STRING       objname, matname;

	while (dx_mats) {
		dx_lst  = dx_mats;
		dx_mats = dx_mats->next;
	}

	std::vector<LWW::Surface> surfs = globals_.getSurfacesByObject(mesh.filename());
	int num_srf = (int)surfs.size();

	for (int n = 0; n < num_srf; n++) {
		if(uselist && !uselist[n]) continue;

		dx_mat = new DX_Material;
		memset( dx_mat, 0, sizeof(DX_Material) );
		dx_mat->idx  = dx_lst ? dx_lst->idx+1 : 0;
		dx_mat->next = NULL;

		DX_FileUniqName( objname, mesh.filename() );
		sprintf( matname, "%s_%s", objname, surfs[n].name() );
		DX_MakeNodeName( dx_mat->nodeName, matname );
		DX_MakeNodeName( dx_mat->surfName, surfs[n].name() );

		double color[3];
		double *v;
		v = surfs[n].getFlt(SURF_COLR);
		memcpy(color, v, sizeof(color));

		v = surfs[n].getFlt(SURF_DIFF);
		dx_mat->mtrl.faceColor.red       = (DX_FLOAT) (color[0]*(*v));
		dx_mat->mtrl.faceColor.green     = (DX_FLOAT) (color[1]*(*v));
		dx_mat->mtrl.faceColor.blue      = (DX_FLOAT) (color[2]*(*v));
		v = surfs[n].getFlt(SURF_TRAN);
		dx_mat->mtrl.faceColor.alpha     = (DX_FLOAT) (1.0 - *v);

		v = surfs[n].getFlt(SURF_GLOS); //?
		dx_mat->mtrl.power               = (DX_FLOAT) *v;

		v = surfs[n].getFlt(SURF_SPEC);
		dx_mat->mtrl.specularColor.red   = (DX_FLOAT) (color[0]*(*v));
		dx_mat->mtrl.specularColor.green = (DX_FLOAT) (color[1]*(*v));
		dx_mat->mtrl.specularColor.blue  = (DX_FLOAT) (color[2]*(*v));

		v = surfs[n].getFlt(SURF_LUMI);
		dx_mat->mtrl.emissiveColor.red   = (DX_FLOAT) (color[0]*(*v));
		dx_mat->mtrl.emissiveColor.green = (DX_FLOAT) (color[1]*(*v));
		dx_mat->mtrl.emissiveColor.blue  = (DX_FLOAT) (color[2]*(*v));

		strcpy( dx_mat->textureFilename.filename, "" );
		if(surfs[n].getTex(SURF_COLR)) {
			LWW::TextureLayer tl = surfs[n].getTex(SURF_COLR).firstLayer();
			while(tl) {
				LWImageID imgid;
				if(tl.getParam(TXTAG_IMAGE, &imgid)) {
					const char *fname = LWW::Image(&globals_, imgid).filename(0);
					ConvertTextureName( dx_mat->textureFilename.filename, (char *)fname );
					break;
				}
				tl = tl.next();
			}
		}

		if (_options->enbaleDecalTexture) {
			if (surfs[n].getTex(SURF_COLR) && strlen(dx_mat->textureFilename.filename) > 0) {
				dx_mat->mtrl.faceColor.red       = 1.0f;
				dx_mat->mtrl.faceColor.green     = 1.0f;
				dx_mat->mtrl.faceColor.blue      = 1.0f;
				/*dx_mat->mtrl.specularColor.red   = mat->clrh + mat->spci * (1.0f-mat->clrh);
				dx_mat->mtrl.specularColor.green = mat->clrh + mat->spci * (1.0f-mat->clrh);
				dx_mat->mtrl.specularColor.blue  = mat->clrh + mat->spci * (1.0f-mat->clrh);
				dx_mat->mtrl.emissiveColor.red   = mat->lumi;
				dx_mat->mtrl.emissiveColor.green = mat->lumi;
				dx_mat->mtrl.emissiveColor.blue  = mat->lumi;*/
			}
		}

		if (dx_lst) dx_lst->next   = dx_mat;
		if (dx_fst == NULL) dx_fst = dx_mat;
		dx_lst = dx_mat;
	}

	return dx_fst;
}


/*
 *
 *  DisposeMaterials   - make materials from the object
 *
 */

void dxExporter::DisposeMaterials( DX_Material **dx_mats )
{
	DX_Material*  dx_mat = *dx_mats;
	DX_Material*  tmp;

	while (dx_mat)
	{
		tmp = dx_mat->next;
		delete dx_mat;
		dx_mat = tmp;
	}
	*dx_mats = NULL;
}


/*
 *
 *  ConvertTextureName   - convert texture image name
 *
 */

void dxExporter::ConvertTextureName( char* outfile, char* infile )
{
	PathConvParam   param;
	DX_FNAM         bodyname, absolute, relative;

	if (_options->outputTextureImages == TRUE) {
		DX_TextureName ( bodyname, infile, _options->imageExtension );
	} else {
		DX_FileBodyName( bodyname, infile );
	}

	if (_options->textureFilename == DX_TXTNAME_BODYNAME) {
		strcpy( outfile, bodyname );
		return;
	}

	param.directory = _options->textureDirectory;
	param.name      = bodyname;
	param.path_out  = absolute;
	PathConvert( PATHCONV_BUILDPATH, &param );

	if (_options->textureFilename == DX_TXTNAME_ABSOLUTE) {
		DX_ConvertString( outfile, absolute );
		return;
	}

	if (_options->textureFilename == DX_TXTNAME_RELATIVE) {
		DX_MakeRelativePath( relative, absolute, _options->exportDirectory );
		DX_ConvertString( outfile, relative );
	}
}


/*
 *
 *  DX_ConvertString   - convert the filename string to print
 *
 */

static void DX_ConvertString( char* outfile, char* infile )
{
	while (*infile)
	{
		if (*infile == '\\') *outfile++ = '\\';
		*outfile++ = *infile++;
	}
	*outfile = '\0';
}


/*
 *
 *  DX_MakeRelativePath   - convert absolute path to relative path
 *
 */

static void DX_MakeRelativePath( char* outfile, char* infile, char* directory )
{
	DX_FNAM    tmp1, tmp2;
	char       *inf, *dir, *p;

	strcpy( tmp1, infile );
	strcpy( tmp2, directory );

	inf = tmp1;
	dir = tmp2;

	while (toupper(*inf) == toupper(*dir)) {
		inf++; dir++;
	}

	if (inf == tmp1) {				// Infile is not in directory (Absolute)
		strcpy( outfile, infile );
		return;
	}

	if (strlen(dir) == 0) {			// Infile is under directory
		if (*inf == '\\') inf++;
		strcpy( outfile, inf );
		return;
	}

	outfile[0] = '\0';				// Infile is in other sub-directory
	p = strtok( dir, "\\" );
	while (p) {
		strcat( outfile, "..\\" );
		p = strtok(NULL, "\\" );
	}
	strcat( outfile, inf );
}






