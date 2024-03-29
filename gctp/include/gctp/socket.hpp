//---------------------------------------------------------------------------
/*
                 ソケットのラッパ Ver.1.4  by Gimite 市川
  
  TCP/IPソケットのラッパです。Windows（WinSock2）とUNIX（POSIX Socket）に対
  応しています。
  
  Gimite 市川 <gimite@mx12.freecom.ne.jp>
  http://gimite.ddo.jp/gimite/cppmess.htm
*/
//---------------------------------------------------------------------------
#ifndef GIMITE_socketH
#define GIMITE_socketH
//---------------------------------------------------------------------------
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#  define GIMITE_WIN32
#endif
//---------------------------------------------------------------------------
#include <iostream>
#include <ios>//streamsize
#include <streambuf>//basic_streambuf
#include <string>//char_traits, basic_string
#include <vector>//vector
#ifdef GIMITE_WIN32
#  include <WinSock2.h>
#  pragma comment(lib,"ws2_32.lib")
#else
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <unistd.h>
#endif
//---------------------------------------------------------------------------
namespace gimite{
//---------------------------------------------------------------------------
#ifdef GIMITE_WIN32
  typedef SOCKET socket_t;
  typedef int sock_result_t;
  typedef int sock_len_t;
  typedef unsigned long sock_uint32_t;
#else
  typedef int socket_t;
  typedef ssize_t sock_result_t;
  typedef socklen_t sock_len_t;
  typedef u_int32_t sock_uint32_t;
#endif
//---------------------------------------------------------------------------
//ソケットシステムを初期化
inline bool startup_socket(){
#ifdef GIMITE_WIN32
  WSADATA wsaData;
  return WSAStartup(0x0002, &wsaData)==0;
#else
  return true;
#endif
}

//---------------------------------------------------------------------------
//ソケットシステムを後始末
inline void cleanup_socket(){
#ifdef GIMITE_WIN32
  WSACleanup();
#endif
}

//---------------------------------------------------------------------------
//IPアドレス
class ip_address
{
public:
  explicit ip_address(sock_uint32_t addr= INADDR_NONE){
#ifdef GIMITE_WIN32
    addr_.S_un.S_addr= addr;
#else
    addr_.s_addr= addr;
#endif
  }
  explicit ip_address(const in_addr& addr): addr_(addr){}
  ip_address(const std::string& host){
    solve_address(host.c_str());
  }
  ip_address(const char* host){
    solve_address(host);
  }
  ip_address& operator=(const std::string& host){
    solve_address(host.c_str());
    return *this;
  }
  ip_address& operator=(const char* host){
    solve_address(host);
    return *this;
  }
  //アドレスを表すin_addr構造体
  in_addr addr()const{
    return addr_;
  }
  //アドレスを表す32ビット整数
  sock_uint32_t as_int()const{
#ifdef GIMITE_WIN32
    return addr_.S_un.S_addr;
#else
    return addr_.s_addr;
#endif
  }

private:
  in_addr addr_;
  
  //ホスト名orIPの文字列表現からIPアドレスを得る
  void solve_address(const char* host){
    sock_uint32_t uaddr= ::inet_addr(host);
    if (uaddr==INADDR_NONE){
      hostent* ent= ::gethostbyname(host);
      if (ent && ent->h_addr_list[0])
        uaddr= *(sock_uint32_t*)ent->h_addr_list[0];
    }
#ifdef GIMITE_WIN32
    addr_.S_un.S_addr= uaddr;
#else
    addr_.s_addr= uaddr;
#endif
  }
  
  //比較、同値判定。
  friend bool operator<(const ip_address& lhs, const ip_address& rhs){
		return lhs.as_int()<rhs.as_int();
	}
	friend bool operator>(const ip_address& lhs, const ip_address& rhs){ return rhs<lhs; }
	friend bool operator<=(const ip_address& lhs, const ip_address& rhs){ return !(lhs>rhs); }
	friend bool operator>=(const ip_address& lhs, const ip_address& rhs){ return !(lhs<rhs); }
	friend bool operator==(const ip_address& lhs, const ip_address& rhs){ return lhs>=rhs && lhs<=rhs; }
	friend bool operator!=(const ip_address& lhs, const ip_address& rhs){ return !(lhs==rhs); }
  
  //stream出力。
  friend std::ostream& operator<<(std::ostream& os, const ip_address& addr){
		sock_uint32_t uaddr= addr.as_int();
		for (int i= 0; i<4; ++i){
			if (i>0) os << ".";
			os << (int)(unsigned char)(uaddr >> i*8);
		}
		return os;
	}
  
};

//---------------------------------------------------------------------------
//IPアドレス+ポート番号
struct socket_address
{
  explicit socket_address(const ip_address& i= ip_address(), int p= 0)
    : ip(i), port(p){}
  
  //IPアドレス
  ip_address ip;
  //ポート番号
  int port;
  
  //比較、同値判定。
  friend bool operator<(const socket_address& lhs, const socket_address& rhs){
		if (lhs.ip<rhs.ip) return true;
		if (lhs.ip>rhs.ip) return false;
		return lhs.port<rhs.port;
	}
	friend bool operator>(const socket_address& lhs, const socket_address& rhs){ return rhs<lhs; }
	friend bool operator<=(const socket_address& lhs, const socket_address& rhs){ return !(lhs>rhs); }
	friend bool operator>=(const socket_address& lhs, const socket_address& rhs){ return !(lhs<rhs); }
	friend bool operator==(const socket_address& lhs, const socket_address& rhs){
		return lhs>=rhs && lhs<=rhs;
	}
	friend bool operator!=(const socket_address& lhs, const socket_address& rhs){ return !(lhs==rhs); }
  
  //stream出力。
  friend std::ostream& operator<<(std::ostream& os, const socket_address& saddr){
		os << saddr.ip << ":" << saddr.port;
		return os;
	}
	
};

//---------------------------------------------------------------------------
//ストリームソケット用streambuf
class socket_streambuf: public std::streambuf
{
  typedef std::streambuf::int_type int_type;
  typedef std::streambuf::traits_type traits_type;
  
public:
  explicit socket_streambuf(socket_t sock= -1, std::size_t buf_size= 1024)
      : sock_(sock), buffer_(buf_size){}
  socket_streambuf(const ip_address& host, int port, 
        std::size_t buf_size= 1024)
      : sock_(-1), buffer_(buf_size){
    open(host, port);
  }
  virtual ~socket_streambuf(){
    close();
  }
  //接続
  bool open(const ip_address& host, int port){
    if (is_open()) close();
    if (host.as_int()==INADDR_NONE) return false;
    sock_= ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock_==-1) return false;
    sockaddr_in addr;
    addr.sin_family= AF_INET;
    addr.sin_port= htons((unsigned short)port);
    addr.sin_addr= host.addr();
    if (::connect(sock_, (const sockaddr*)&addr, sizeof(sockaddr_in))==-1)
      close();
    return is_open();
  }
  //切断
  void close(){
    if (sock_==-1) return;
#ifdef GIMITE_WIN32
    ::closesocket(sock_);
#else
    ::close(sock_);
#endif
    sock_= -1;
  }
  //ソケットハンドルを閉じずに解放
  socket_t release(){
    socket_t sock= sock_;
    sock_= -1;
    return sock;
  }
  //送信
  int send(const void* buffer, int size){
    return ::send(sock_, (const char*)buffer, size, 0);
  }
  //受信
  int recv(void* buffer, int size){
    return ::recv(sock_, (char*)buffer, size, 0);
  }
  //正常に接続されてるか
  bool is_open()const{ return sock_!=-1; }
  //ソケットハンドル
  socket_t socket()const{ return sock_; }

protected:
  //1文字送信
  virtual int_type overflow(int_type c= std::char_traits<char>::eof()){
    if (!traits_type::eq_int_type(c, traits_type::eof())){
      char cc= char(c);
      if (traits_type::eq_int_type(xsputn(&cc, 1), 0))
        return traits_type::eof();
    }
    return 'a';
  }
  //n文字送信
  virtual std::streamsize xsputn(const char_type* s, std::streamsize n){
    if (!is_open()) return 0;
    sock_result_t res= ::send(sock_, s, int(n), 0);
    return res==-1? 0 : res;
  }
  //受信バッファが空になった
  virtual int_type underflow(){
    if (!is_open()) return traits_type::eof();
    if (!gptr() || gptr()>=egptr()){
      if (gptr() && gptr()>eback())
        buffer_[0]= *(gptr()-1);
      else
        buffer_[0]= '\0';
      sock_result_t size= ::recv(sock_, &buffer_[1], int(buffer_.size()-1), 0);
      if (size<=0)
        return traits_type::eof();
      else
        setg(&buffer_[0], &buffer_[1], &buffer_[size+1]);
    }
    return traits_type::to_int_type(*gptr());
  }

private:
  socket_t sock_;
  std::vector<char> buffer_;

};

//---------------------------------------------------------------------------
//ストリームソケット用stream
class socket_stream: public std::iostream
{
public:
  //すぐには接続しないコンストラクタ
  socket_stream(): std::iostream(0){
    rdbuf(&sbuf_);
    setstate(std::ios::badbit);
  }
  //すぐに接続するコンストラクタ
  socket_stream(const ip_address& host, int port)
      : std::iostream(0), sbuf_(host, port){
    rdbuf(&sbuf_);
    if (!sbuf_.is_open()) setstate(std::ios::badbit);
  }
  //既存のソケットを指定するコンストラクタ
  explicit socket_stream(socket_t sock)
      : std::iostream(0), sbuf_(sock){
    rdbuf(&sbuf_);
    if (!sbuf_.is_open()) setstate(std::ios::badbit);
  }
  //デストラクタ
  virtual ~socket_stream(){ rdbuf(0); }
  //接続
  void open(const ip_address& host, int port){
    clear();
    sbuf_.open(host, port);
    if (!sbuf_.is_open()) setstate(std::ios::badbit);
  }
  //切断
  void close(){
    setstate(std::ios::badbit);
    sbuf_.close();
  }
  //ソケットハンドルを閉じずに解放
  socket_t release(){
    setstate(std::ios::badbit);
    return sbuf_.release();
  }
  //送信
  int send(const void* buffer, int size){ return sbuf_.send(buffer, size); }
  //受信
  int recv(void* buffer, int size){ return sbuf_.recv(buffer, size); }
  //ソケットハンドル
  socket_t socket()const{ return sbuf_.socket(); }
  
private:
  socket_streambuf sbuf_;

};

//---------------------------------------------------------------------------
//バインドして使うソケット
class bound_socket
{
public:
  //通常のコンストラクタ
  explicit bound_socket(int type)
    : type_(type), sock_(-1), is_bound_(false){}
  //既存のソケットハンドルを使うコンストラクタ
  bound_socket(int type, socket_t sock, bool is_bound)
    : type_(type), sock_(sock), is_bound_(is_bound){}
  //デストラクタ
  virtual ~bound_socket(){
    close();
  }
  //ソケットをクローズ
  void close(){
    is_bound_= false;
    if (sock_==-1) return;
#ifdef GIMITE_WIN32
    ::closesocket(sock_);
#else
    ::close(sock_);
#endif
    sock_= -1;
  }
  //ソケットハンドルを解放
  socket_t release(){
    socket_t sock= sock_;
    sock_= -1;
    is_bound_= false;
    return sock;
  }
  //既存のソケットを指定
  void socket(socket_t sock, bool is_bound){
    close();
    sock_= sock;
    is_bound_= is_bound;
  }
  //ソケットのタイプ
  int type()const{ return type_; }
  //ソケットハンドル
  socket_t socket()const{ return sock_; }
  //正しくバインドされているか
  operator const void*()const{ return is_bound_? this : NULL; }
  bool operator!()const{ return !is_bound_; }

protected:
  //バインド
  bool pbind(int port){
    if (is_bound_) return false;
    if (sock_==-1){
      sock_= ::socket(AF_INET, type_, 0);
      if (sock_==-1) return false;
    }
    sockaddr_in addr;
    addr.sin_family= AF_INET;
    addr.sin_port= htons(port);
    addr.sin_addr= ip_address(sock_uint32_t(INADDR_ANY)).addr();
      //注：cygwin gcc 3.3.1ではINADDR_ANYの型がsock_uint32_tではないので、
      //    曖昧さを無くすためのキャストが必要。
    if (::bind(sock_, (const sockaddr*)&addr, sizeof(addr))==-1)
      return false;
    is_bound_= true;
    return true;
  }
  
private:
  const int type_;
  socket_t sock_;
  bool is_bound_;
  
};

//---------------------------------------------------------------------------
//サーバ用ストリームソケット
class server_stream_socket: public bound_socket
{
public:
  //すぐにはバインドしないコンストラクタ
  server_stream_socket()
    : bound_socket(SOCK_STREAM){}
  //すぐにバインドするコンストラクタ
  explicit server_stream_socket(int port, int backlog= 5)
      : bound_socket(SOCK_STREAM){
    bind(port, backlog);  
  }
  //既存のソケットハンドルを使うコンストラクタ
  server_stream_socket(socket_t sock, bool is_bound)
    : bound_socket(SOCK_STREAM, sock, is_bound){}
  //バインド
  bool bind(int port, int backlog= 5){
    if (!pbind(port)) return false;
    if (::listen(socket(), backlog)!=-1) return true;
    close();
    return false;
  }
  //接続要求を受け入れる
  socket_t accept(){
    return ::accept(socket(), 0, 0);
  }
  
};

//---------------------------------------------------------------------------
//ダイアグラムソケット
class diagram_socket: public bound_socket
{
public:
  //すぐにはバインドしないコンストラクタ
  diagram_socket()
    : bound_socket(SOCK_DGRAM){}
  //すぐにバインドするコンストラクタ
  explicit diagram_socket(int port)
      : bound_socket(SOCK_DGRAM){
    bind(port);
  }
  //既存のソケットハンドルを使うコンストラクタ
  diagram_socket(socket_t sock, bool is_bound)
    : bound_socket(SOCK_DGRAM, sock, is_bound){}
  //バインド
  bool bind(int port){
    return pbind(port);
  }
  //データを受信
  int recvfrom(void* buffer, int size, socket_address* addr= 0,
      int flags= 0){
    sockaddr_in saddr;
    sock_len_t addr_len= sizeof(saddr);
    int result= ::recvfrom(socket(), (char*)buffer, size, flags,
      (sockaddr*)&saddr, &addr_len);
      //注：Winsockのrecvfromの第2パラメータ型はchar*なので
      //    キャストが必要。
    if (addr){
      addr->ip= ip_address(saddr.sin_addr);
      addr->port= ntohs(saddr.sin_port);
    }
    return result;
  }
  //データを送信
  int sendto(const void* buffer, int size, const socket_address* addr,
      int flags= 0){
    sockaddr_in saddr;
    saddr.sin_family= AF_INET;
    saddr.sin_addr= addr->ip.addr();
    saddr.sin_port= htons(addr->port);
    return ::sendto(socket(), (const char*)buffer, size, flags,
      (const sockaddr*)&saddr, sizeof(saddr));
      //注：Winsockのsendtoの第2パラメータ型はconst char*なので
      //    キャストが必要。
  }
  
};

//---------------------------------------------------------------------------
}//namespace gimite
//---------------------------------------------------------------------------
#endif
