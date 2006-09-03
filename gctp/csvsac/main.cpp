#ifdef _MSC_VER
# pragma warning(disable:4786 4503)
# if _MSC_VER < 1300
#  define for if(0); else for
# else
#  pragma conform(forScope, on)
# endif
#endif

#include <gctp/argmap.hpp>
#include <gctp/csvsa.hpp>
#include <iostream>
#include <fstream>
#include <strstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <math.h>
#include <ctype.h>
#include <locale.h>

using namespace std;
using namespace gctp;

namespace {
	void print_help()
	{
		cout <<
"CSV(Comma Separated Value) Structure Array Compiler  version 1.0\n\n"
"csvsac : [option] input_filename_or_directory ...\n"
"option : -o | --output=output_filename\n"
"       : -e | --extension=add_extension_string\n"
"       : -s | --script=script_filename\n"
		;
	}
} // anonymous namespace

int main(int argc, char *argv[])
{
	setlocale(LC_CTYPE, "");

	ArgMap arg;
	arg.defAlias('o', "output");
	arg.defAlias('e', "extention");
	arg.defAlias('s', "script");
	arg.addHasValKey("output");
	arg.addHasValKey("extention");
	arg.addHasValKey("script");
	if(!arg.parse(argc, argv)) {
		print_help();
		return 1;
	}
	else {
		string ext(".bin"), outdir("");
		if(arg.hasKey("output")) {
			outdir = arg["output"].front();
			if(outdir[outdir.length()-1]!='/') outdir += "/";
		}
		if(arg.hasKey("extention")) {
			ext = arg["extention"].front();
		}
		if(arg.hasKey("")) {
			for(ArgMap::ValListItr i = arg[""].begin(); i != arg[""].end(); i++) {
				string outfn(outdir + *i);
				string::size_type pos;
				if((string::npos != (pos = outfn.rfind(".txt")))&&outfn.length()-pos==4) outfn.replace(pos, string::npos, ext.c_str(), 4);
				else if((string::npos != (pos = outfn.rfind(".csv")))&&outfn.length()-pos==4) outfn.replace(pos, string::npos, ext.c_str(), 4);
				else if((string::npos != (pos = outfn.rfind(".TXT")))&&outfn.length()-pos==4) outfn.replace(pos, string::npos, ext.c_str(), 4);
				else if((string::npos != (pos = outfn.rfind(".CSV")))&&outfn.length()-pos==4) outfn.replace(pos, string::npos, ext.c_str(), 4);
				else outfn += ext;
				cerr << "compiling...  " << *i << " -> " << outfn << endl;
				CSVSAChunk::parse(ifstream(i->c_str()), ofstream(outfn.c_str(), ios::out|ios::binary));
			}
		}
		else {
			print_help();
		}
	}
	return 0;
}
