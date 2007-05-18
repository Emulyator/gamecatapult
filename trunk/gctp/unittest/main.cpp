#ifdef _MSC_VER
#	pragma warning(disable:4786 4503)
#endif

#include <iostream>
#include <string>
#include <locale>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <locale.h>
#if defined _MSC_VER && _MSC_VER == 1400
#include <io.h>
#include <fcntl.h>
#endif
#define WIN32_LEAN_AND_MEAN		// Windows ヘッダーから殆ど使用されないスタッフを除外します
#include <windows.h>
#include <gctp/dbgout.hpp>
#include <gctp/dbgoutbuf.hpp>

static gctp::debuggeroutbuf<char> _sdbgout_buf;
static std::basic_ostream<char> sdbgout(&_sdbgout_buf);		// デバッガアウトプットストリーム

int main(int argc, char* argv[]) {
  std::locale::global(std::locale(std::locale::classic(), std::locale(""), LC_CTYPE));
  gctp::logfile.imbue(std::locale(std::locale::classic(), std::locale(""), LC_CTYPE));
  int format = 2;
  int target = 0;
  std::string xsl;
  std::string ns;
  for ( int i = 1; i < argc; ++i ) {
    std::string arg(argv[i]);
    if ( arg == "--text"      ) format = 0;
    if ( arg == "--xml"       ) format = 1;
    if ( arg == "--compiler"  ) format = 2;
    if ( arg == "--cerr"      ) target = 1;
    if ( arg == "--xsl"       ) xsl = argv[++i];
    if ( arg == "--namespace" ) ns  = argv[++i];
  }
  CppUnit::TextUi::TestRunner runner;
  if ( ns.empty() )
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
  else
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry(ns).makeTest());
  CppUnit::Outputter* outputter = 0;
  std::ostream* stream = target ? &std::cerr : &std::cout;
  switch ( format ) {
  case 0 :
    outputter = new CppUnit::TextOutputter(&runner.result(),*stream);
    break;
  case 1 :
/*    outputter = new CppUnit::XmlOutputter(&runner.result(),*stream, "shift_jis");
    static_cast<CppUnit::XmlOutputter*>(outputter)->setStyleSheet(xsl);
    break;*/
  case 2 :
	outputter = new CppUnit::CompilerOutputter(&runner.result(),sdbgout);
	break;
  }
  runner.setOutputter(outputter);
  return runner.run() ? 0 : 1;
}

#ifdef _MSC_VER
# ifdef GCTP_LITE
#  pragma comment(lib, "Dxerr8.lib")
#  pragma comment(lib, "d3d8.lib")
#  ifdef _DEBUG
#   pragma comment(lib, "luad.lib")
#   pragma comment(lib, "cppunitd.lib")
#  else
#   pragma comment(lib, "lua.lib")
#   pragma comment(lib, "cppunit.lib")
#  endif
# else
#  pragma comment(lib, "Dxerr9.lib")
#  pragma comment(lib, "d3d9.lib")
#  ifdef _DEBUG
#   pragma comment(lib, "d3dx9d.lib")
#   pragma comment(lib, "luad.lib")
#   pragma comment(lib, "cppunitd.lib")
#  else
#   pragma comment(lib, "d3dx9.lib")
#   pragma comment(lib, "lua.lib")
#   pragma comment(lib, "cppunit.lib")
#  endif
# endif
#endif
