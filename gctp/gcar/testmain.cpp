/** @file main.cpp
 * gcar (GameCatapult Archiver)
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/12/30
 * $Id: testmain.cpp,v 1.1.1.1 2003/01/04 09:34:33 SAM Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
/** @mainpage
 * 当初の構想から大きく変わった。karと同じ思想で作ろうとしていたが、それをやめた。
 * gcarはすでに組みあがっているアーカイブに対してファイルの追加、ファイル内容のアップデート
 * が可能なようにする。
 */
#ifdef _MSC_VER
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#endif // _MSC_VER
#include "argmap.h"
#include "gcarchive.h"
#include <iostream>
#include <fstream>
#include <ios>

using namespace gctp;
using namespace std;

int main(int argc, char *argv[])
{
	ArgMap arg;
	if(!arg.parse(argc, argv)) {
		cout << "ParseError!" << endl;
	}
	for(ArgMap::Itr i = arg.begin(); i != arg.end(); i++) {
		cout << "KEY:" << i->first << ":VAL:";
		for(ArgMap::ValListItr j = i->second.begin(); j != i->second.end(); j++) {
			if(j != i->second.begin()) cout << ":";
			cout << *j;
		}
		cout << endl;
	}
	File fs("test.txt", File::WRITE);
	fs << "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	fs.flush();
/*	fs.insert(3, 5);
	fs.flush();
	cout << "fs.length()=" << fs.length() << endl;*/
	fs.remove(0, 26); // abcijklmnopqrstuvwxyz
	fs.flush().relative_truncate(-26);
	cout << "fs.length()=" << fs.length() << endl;
	
	
	ArchiveEditable arc("test.txt");
	printf("index\n");
	arc.printIndex();
	printf("list\n");
	arc.printList();
	arc.add("File1.txt");
	arc.add("File2.txt");
	arc.add("File3.txt");
	arc.add("File4.txt");
	arc.commit();
/*	arc.remove("File2.txt");
	arc.commit();
	printf("index\n");
	arc.printIndex();
	printf("list\n");
	arc.printList();*/
	arc.extract("_File1.txt", "File1.txt");
	arc.extract("_File3.txt", "File3.txt");
	arc.extract("_File4.txt", "File4.txt");
	return 0;
}
