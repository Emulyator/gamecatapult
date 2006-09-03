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
#include <iostream>
#include <fstream>
#include <ios>
#include <gctp/argmap.hpp>
#include <gctp/archive.hpp>
#include <_direct.h>
#include <direct.h>
#include <sys/stat.h>
#include <locale.h>

using namespace gctp;
using namespace std;

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
	;
}

void addRecursive(ArchiveEditable &arc, std::string path, DIR *dir)
{
	struct dirent *ent;
	while(ent = readdir(dir)) {
		if(strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
			string fname;
			if(path.empty()) fname = ent->d_name;
			else fname = path + "/" + ent->d_name;
			DIR *dir_;
			if(dir_ = opendir(fname.c_str())) {
				addRecursive(arc, fname, dir_);
				closedir(dir_);
			}
			else {
				cout << "add " << fname << endl;
				arc.add(fname.c_str());
			}
		}
	}
}

}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "jpn");

	ArgMap arg;
	arg.defAlias('o', "output");
	arg.defAlias('d', "root");
	arg.defAlias('r', "remove");
	arg.defAlias('x', "extract");
	arg.defAlias('f', "forceupdate");
	arg.defAlias('a', "align");
	arg.defAlias('X', "extractall");
	arg.defAlias('u', "update");
	arg.defAlias('l', "list");
	arg.addHasValKey("output");
	arg.addHasValKey("root");
	arg.addHasValKey("remove");
	arg.addHasValKey("extract");
	arg.addHasValKey("forceupdate");
	arg.addHasValKey("align");
	if(!arg.parse(argc, argv)) {
		print_help();
		return 1;
	}
	else {
		if(arg.hasKey("list") && arg.hasKey("")) {
			Archive arc(arg[""].front().c_str());
			arc.printIndex(cout);
			return 0;
		}
		if(arg.hasKey("extractall") && arg.hasKey("")) {
			Archive arc(arg[""].front().c_str());
			if(arg.hasKey("root")) {
				if(_chdir(arg["root"].front().c_str())) return 2;
			}
			arc.extract();
			return 0;
		}
		if(arg.hasKey("extract") && arg.hasKey("")) {
			Archive arc(arg[""].front().c_str());
			if(arg.hasKey("root")) {
				if(_chdir(arg["root"].front().c_str())) return 2;
			}
			for(ArgMap::ValListItr i = arg["extract"].begin(); i != arg["extract"].end(); i++) {
				arc.extract(i->c_str(), i->c_str());
			}
			return 0;
		}
		string arcname;
		if(arg.hasKey("output")) arcname = arg["output"].front();
		else if(arg.hasKey("")) {
			struct _stat st;
			if(0 == _stat(arg[""].front().c_str(), &st)) {
				arcname = arg[""].front() + ".gcar";
			}
			else {
				cerr << arg[""].front() << ": ファイルが開けません" << endl;
				return 2;
			}
		}
		else {
			print_help();
			return 1;
		}
		ArchiveEditable arc(arcname.c_str());
		if(!arc.is_open()) {
			cerr << arcname << ": ファイルが開けません" << endl;
			return 2;
		}
		if(arg.hasKey("root")) {
			if(_chdir(arg["root"].front().c_str())) {
				cerr << arg["root"].front() << ": ディレクトリが開けません" << endl;
				return 2;
			}
		}
		if(arg.hasKey("remove")) {
			for(ArgMap::ValListItr i = arg["remove"].begin(); i != arg["remove"].end(); i++) {
				arc.remove(i->c_str());
			}
		}
		if(arg.hasKey("update")) {
			arc.update();
		}
		if(arg.hasKey("forceupdate")) {
			for(ArgMap::ValListItr i = arg["forceupdate"].begin(); i != arg["forceupdate"].end(); i++) {
				arc.forceUpdate(i->c_str());
			}
		}
		if(arg.hasKey("align")) {
			int align = atol(arg["align"].front().c_str());
			cout << "align " << align << endl;
			if(align && align%4==0) arc.setAlign(align);
		}
		if(arg.hasKey("")) {
			for(ArgMap::ValListItr i = arg[""].begin(); i != arg[""].end(); i++) {
				DIR *dir;
				if(dir = opendir(i->c_str())) {
					addRecursive(arc, *i, dir);
					closedir(dir);
				}
				else {
					cout << "add " << *i << endl;
					arc.add(i->c_str());
				}
			}
		}
		else {
			DIR *dir;
			if(dir = opendir(".")) {
				addRecursive(arc, "", dir);
				closedir(dir);
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
