/** @file main.cpp
 * gcar (GameCatapult Archiver)
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/12/30
 * $Id: main.cpp,v 1.4 2003/12/21 00:56:42 SAM Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
/** @mainpage
 * 当初の構想から大きく変わった。karと同じ思想で作ろうとしていたが、それをやめた。
 * gcarはすでに組みあがっているアーカイブに対してファイルの追加、ファイル内容のアップデート
 * が可能なようにする。
 */
#ifdef _MSC_VER
#pragma warning(disable:4786 4503)
#endif // _MSC_VER
#include <gctp/targmap.hpp>
#include <gctp/turi.hpp>
#include <gctp/archive.hpp>
#include <gctp/archiveeditable.hpp>
#include <gctp/dbgout.hpp>
#include <gctp/cstr.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ios>
#include <_direct.h>
#include <direct.h>
#include <sys/stat.h>
#include <locale.h>
#include <io.h>
#include <fcntl.h>

using namespace gctp;
using namespace std;

#ifdef _UNICODE
#define _tststat _stat64i32
#else
#define _tststat _stat
#endif

namespace {

void print_help()
{
	cout <<
"gcar : [option] filename_or_directory ...\n"
"  option : -o | --output output_filename\n"
"         : -d | --root root_directory\n"
"         : -r | --remove remove_filename\n"
"         : -x | --extract extract_filename\n"
"         : -f | --forceupdate update_filename\n"
"         : -a | --align page_align\n"
"         : -X | --extractall\n"
"         : -u | --update\n"
"         : -l | --list\n"
"         : -a | --abstruct\n"
"         : -c | --crypt password\n"
"\n"
"example:\n"
"   new archive : gcar dirname\n"
"         (to make dirname.gcar)\n"
"   update archive : gcar -u dirname\n"
"         (sync dirname.gcar with dirname directory.)\n"
	;
}

void addRecursive(ArchiveEditable &arc, std::basic_string<_TCHAR> path, TDIR *dir)
{
	tdirent *ent;
	while(ent = treaddir(dir)) {
		if(_tcscmp(ent->d_name, _T(".")) && _tcscmp(ent->d_name, _T(".."))) {
			basic_string<_TCHAR> fname;
			if(path.empty()) fname = ent->d_name;
			else fname = path + _T("/") + ent->d_name;
			TDIR *dir_;
			if(dir_ = topendir(fname.c_str())) {
				addRecursive(arc, fname, dir_);
				tclosedir(dir_);
			}
			else {
				//cout << "add " << CStr(fname.c_str()) << endl;
				arc.add(fname.c_str());
			}
		}
	}
}

}

int _tmain(int argc, _TCHAR *argv[])
{
	setlocale(LC_ALL, "");
	locale::global(locale("japanese"));
	_setmode(_fileno(stdout), _O_BINARY);
	_setmode(_fileno(stderr), _O_BINARY);
	//GCTP_LOGFILE_OPEN(_T("log.txt"));
	
	TArgMap arg;
	arg.defAlias(_T('o'), _T("output"));
	arg.defAlias(_T('d'), _T("root"));
	arg.defAlias(_T('r'), _T("remove"));
	arg.defAlias(_T('x'), _T("extract"));
	arg.defAlias(_T('f'), _T("forceupdate"));
	arg.defAlias(_T('a'), _T("align"));
	arg.defAlias(_T('X'), _T("extractall"));
	arg.defAlias(_T('u'), _T("update"));
	arg.defAlias(_T('l'), _T("list"));
	arg.defAlias(_T('c'), _T("crypt"));
	arg.addHasValKey(_T("output"));
	arg.addHasValKey(_T("root"));
	arg.addHasValKey(_T("remove"));
	arg.addHasValKey(_T("extract"));
	arg.addHasValKey(_T("forceupdate"));
	arg.addHasValKey(_T("align"));
	arg.addHasValKey(_T("crypt"));
	if(!arg.parse(argc, argv)) {
		print_help();
		return 1;
	}
	else {
		if(arg.hasKey(_T("list")) && arg.hasKey(_T(""))) {
			Archive arc(arg[_T("")].front().c_str(), arg.hasKey(_T("crypt"))?CStr(arg[_T("crypt")].front().c_str()).c_str():0);
			if(!arc.isOpen()) {
				cerr << CStr(arg[_T("")].front().c_str()) << ": ファイルが開けません" << endl;
				return 2;
			}
			arc.printIndex(cout);
			return 0;
		}
		if(arg.hasKey(_T("extractall")) && arg.hasKey(_T(""))) {
			Archive arc(arg[_T("")].front().c_str(), arg.hasKey(_T("crypt"))?CStr(arg[_T("crypt")].front().c_str()).c_str():0);
			if(!arc.isOpen()) {
				cerr << CStr(arg[_T("")].front().c_str()) << ": ファイルが開けません" << endl;
				return 2;
			}
			if(arg.hasKey(_T("root"))) {
				if(_tchdir(arg[_T("root")].front().c_str())) return 2;
			}
			arc.extract();
			return 0;
		}
		if(arg.hasKey(_T("extract")) && arg.hasKey(_T(""))) {
			Archive arc(arg[_T("")].front().c_str(), arg.hasKey(_T("crypt"))?CStr(arg[_T("crypt")].front().c_str()).c_str():0);
			if(!arc.isOpen()) {
				cerr << CStr(arg[_T("")].front().c_str()) << ": ファイルが開けません" << endl;
				return 2;
			}
			if(arg.hasKey(_T("root"))) {
				if(_tchdir(arg[_T("root")].front().c_str())) return 2;
			}
			for(WArgMap::ValListItr i = arg[_T("extract")].begin(); i != arg[_T("extract")].end(); i++) {
				arc.extract(i->c_str(), i->c_str());
			}
			return 0;
		}
		basic_string<_TCHAR> arcname;
		if(arg.hasKey(_T("output"))) arcname = arg[_T("output")].front();
		else if(arg.hasKey(_T(""))) {
			struct _tststat st;
			if(0 == _tstat(arg[_T("")].front().c_str(), &st)) {
				if((st.st_mode&_S_IFDIR)) {
					arcname = arg[_T("")].front() + _T(".gcar");
				}
				else {
					TURI uri = arg[_T("")].front();
					if(uri.majorextension() == _T("gcar")) {
						arg[_T("")].front() = uri.raw().substr(uri.raw().length()-5);
					}
				}
			}
			else {
				cerr << CStr(arg[_T("")].front().c_str()) << ": ディレクトリが開けません" << endl;
				return 2;
			}
		}
		else {
			print_help();
			return 1;
		}
		ArchiveEditable arc(arcname.c_str(), arg.hasKey(_T("crypt"))?CStr(arg[_T("crypt")].front().c_str()).c_str():0, arg.hasKey(_T("crypt"))?ArchiveEditable::CRYPT:ArchiveEditable::UNCRYPT);
		arc.setWorkBufferSize(8*1024*1024);
		if(!arc.isOpen()) {
			cerr << CStr(arcname.c_str()) << ": ファイルが開けません" << endl;
			return 2;
		}
		if(arg.hasKey(_T("root"))) {
			if(_tchdir(arg[_T("root")].front().c_str())) {
				cerr << CStr(arg[_T("root")].front().c_str()) << ": ディレクトリが開けません" << endl;
				return 2;
			}
		}
		if(arg.hasKey(_T("remove"))) {
			for(WArgMap::ValListItr i = arg[_T("remove")].begin(); i != arg[_T("remove")].end(); i++) {
				arc.remove(i->c_str());
			}
		}
		if(arg.hasKey(_T("update"))) {
			arc.update();
		}
		if(arg.hasKey(_T("forceupdate"))) {
			for(WArgMap::ValListItr i = arg[_T("forceupdate")].begin(); i != arg[_T("forceupdate")].end(); i++) {
				arc.forceUpdate(i->c_str());
			}
		}
		if(arg.hasKey(_T("align"))) {
			int align = _tstol(arg[_T("align")].front().c_str());
			cout << "align " << align << endl;
			if(align && align%4==0) arc.setAlign(align);
		}
		if(arg.hasKey(_T(""))) {
			if(arg[_T("")].size()==1) {
				if(_tchdir(arg[_T("")].front().c_str())) {
					cerr << CStr(arg[_T("")].front().c_str()) << ": ディレクトリが開けません" << endl;
					return 2;
				}
				TDIR *dir;
				if(dir = topendir(_T("."))) {
					addRecursive(arc, _T(""), dir);
					tclosedir(dir);
				}
				else {
					//cout << "add " << CStr(arg[_T("")].front().c_str()) << endl;
					arc.add(arg[_T("")].front().c_str());
				}
			}
			else {
				for(TArgMap::ValListItr i = arg[_T("")].begin(); i != arg[_T("")].end(); i++) {
					TDIR *dir;
					if(dir = topendir(i->c_str())) {
						addRecursive(arc, *i, dir);
						tclosedir(dir);
					}
					else {
						//cout << "add " << CStr(i->c_str()) << endl;
						arc.add(i->c_str());
					}
				}
			}
		}
		else {
			TDIR *dir;
			if(dir = topendir(_T("."))) {
				addRecursive(arc, _T(""), dir);
				tclosedir(dir);
			}
			else {
				cerr << "格納ファイルを指定してください。" << endl;
				return 1;
			}
		}
		arc.commit();
	}
	return 0;
}
