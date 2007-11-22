/** @file
 * GameCatapult DirectShowクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/25 19:35:25
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/movie/dx/player.hpp>
#include <gctp/dbgout.hpp>
#include <gctp/com_ptr.hpp>
#include <gctp/fileserver.hpp>
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/graphic/texture.hpp>
#include <streams.h>
#include <asyncio.h>
#include <asyncrdr.h>

//#define REGISTER_FILTERGRAPH

using namespace std;

namespace gctp { namespace movie { namespace dx {

	namespace {
		const float min_gain = -5000;

		TYPEDEF_DXCOMPTR(IFilterGraph);
		TYPEDEF_DXCOMPTR(IPin);

		class ArchiveStream : public CAsyncStream
		{
		public:
			ArchiveStream(AbstractFilePtr ptr) :
				fileptr_(ptr),
				m_llLength(ptr->size()),
				m_llPosition(0)
			{
			}

			HRESULT SetPointer(LONGLONG llPos)
			{
				if (llPos < 0 || llPos > m_llLength) {
					//PRNN(_T("????"));
					return S_FALSE;
				}
				else {
					if(llPos < m_llPosition) {
						//PRNN(_T("巻き戻しだ"));
						fileptr_->seek((int)llPos);
					}
					if(llPos > m_llPosition) {
						//PRNN(_T("先送りだ"));
						fileptr_->seek((int)llPos);
					}
					m_llPosition = llPos;
					return S_OK;
				}
			}

			HRESULT Read(PBYTE pbBuffer,
						 DWORD dwBytesToRead,
						 BOOL bAlign,
						 LPDWORD pdwBytesRead)
			{
				CAutoLock lck(&m_csLock);
				if(m_llLength <= m_llPosition) {
					PRNN(_T("なぬ？"));
					return S_FALSE;
				}
				DWORD dwReadLength;
				if(m_llPosition + dwBytesToRead > m_llLength) {
					dwReadLength = (DWORD)(m_llLength - m_llPosition);
				}
				else {
					dwReadLength = dwBytesToRead;
				}

				fileptr_->read(pbBuffer, dwReadLength);
				m_llPosition += dwReadLength;
				*pdwBytesRead = dwReadLength;
				return S_OK;
			}

			LONGLONG Size(LONGLONG *pSizeAvailable)
			{
				*pSizeAvailable = max<long long>(m_llLength - m_llPosition, 0);
				return m_llLength;
			}

			DWORD Alignment()
			{
				return 1;
			}

			void Lock()
			{
				m_csLock.Lock();
			}

			void Unlock()
			{
				m_csLock.Unlock();
			}

		private:
			AbstractFilePtr fileptr_;
			CCritSec       m_csLock;
			const LONGLONG m_llLength;
			LONGLONG       m_llPosition;
		};

		class ArchiveReader : public CAsyncReader
		{
		public:
			//  We're not going to be CoCreate'd so we don't need registration
			//  stuff etc
			STDMETHODIMP Register()
			{
				return S_OK;
			}

			STDMETHODIMP Unregister()
			{
				return S_OK;
			}

			ArchiveReader(ArchiveStream *pStream, CMediaType *pmt, HRESULT *phr) :
				CAsyncReader(NAME("Archive Reader\0"), NULL, pStream, phr)
			{
				m_mt = *pmt;
			}
		};

		class ArchiveMediaFile : public Object {
		public:
			static bool needThis(const TCHAR *path)
			{
				DWORD attr = ::GetFileAttributes(path);
				if(attr != -1 && !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
					// 生ファイルだ
					return false;
				}
				return true;
			}

			ArchiveMediaFile() : archive_stream_(0), archive_reader_(0)
			{
				media_type_.majortype = MEDIATYPE_Stream;
			}

			HRslt open(const _TCHAR *path)
			{
				/*  Find the extension */
				int len = lstrlen(path);
				const _TCHAR *lpType;
				if(len >= 4 && path[len - 4] == TEXT('.')) {
					lpType = path + len - 3;
				}
				else {
					PRNN(_T("Invalid file extension"));
					return E_INVALIDARG;
				}

				/* Set subtype based on file extension */
				if(lstrcmpi(lpType, TEXT("mpg")) == 0) {
					media_type_.subtype = MEDIASUBTYPE_MPEG1System;
				}
				else if(lstrcmpi(lpType, TEXT("mpa")) == 0) {
					media_type_.subtype = MEDIASUBTYPE_MPEG1Audio;
				}
				else if(lstrcmpi(lpType, TEXT("mpv")) == 0) {
					media_type_.subtype = MEDIASUBTYPE_MPEG1Video;
				}
				else if(lstrcmpi(lpType, TEXT("dat")) == 0) {
					media_type_.subtype = MEDIASUBTYPE_MPEG1VideoCD;
				}
				else if(lstrcmpi(lpType, TEXT("avi")) == 0) {
					media_type_.subtype = MEDIASUBTYPE_Avi;
				}
				else if(lstrcmpi(lpType, TEXT("mov")) == 0) {
					media_type_.subtype = MEDIASUBTYPE_QTMovie;
				}
				else if(lstrcmpi(lpType, TEXT("wav")) == 0) {
					media_type_.subtype = MEDIASUBTYPE_WAVE;
				}
				else if(lstrcmpi(lpType, TEXT("mp3")) == 0) {
					media_type_.subtype = MEDIASUBTYPE_MPEG1Audio;
				}
				else {
					PRNN(_T("Unknown file type: ")<<lpType);
					return E_INVALIDARG;
				}

				AbstractFilePtr file_ = fileserver().getFileInterface(path);
				if(!file_) {
					PRNN(_T("Can't open file: ")<<path);
					return E_INVALIDARG;
				}

				archive_stream_ = new ArchiveStream(file_);
				HRslt hr;
				archive_reader_ = new ArchiveReader(archive_stream_, &media_type_, &hr.i);
				if(!hr || archive_reader_ == NULL) {
					file_ = 0;
					if(archive_reader_) delete archive_reader_;
					if(archive_stream_) delete archive_stream_;
					archive_reader_ = 0;
					archive_stream_ = 0;
					PRNN(_T("Could not create filter - ")<<hr);
					return hr;
				}

				//  Make sure we don't accidentally go away!
				archive_reader_->AddRef();
				return hr;
			}

			~ArchiveMediaFile()
			{
				if(archive_reader_) archive_reader_->Release();
				if(archive_stream_) delete archive_stream_;
			}

			CMediaType media_type_;
			ArchiveStream *archive_stream_;
			ArchiveReader *archive_reader_;
		};

		struct __declspec(uuid("{a2a0ae27-9397-47be-bf20-a12ec2a709a5}")) CLSID_TextureRenderer;

		const _TCHAR *getVideoSubtypeString(const GUID *pguid)
		{
			static const struct {
				const GUID *pguid;
				TCHAR name[64];
			} videosubtypes[] = {
				&MEDIASUBTYPE_YVU9              ,TEXT("YVU9\0"),
				&MEDIASUBTYPE_Y411              ,TEXT("YUV 411\0"),
				&MEDIASUBTYPE_Y41P              ,TEXT("Y41P\0"),
				&MEDIASUBTYPE_YUY2              ,TEXT("YUY2\0"),
				&MEDIASUBTYPE_YVYU              ,TEXT("YVYU\0"),
				&MEDIASUBTYPE_UYVY              ,TEXT("UYVY\0"),
				&MEDIASUBTYPE_Y211              ,TEXT("YUV 211\0"),
				&MEDIASUBTYPE_CLJR              ,TEXT("Cirrus YUV 411\0"),
				&MEDIASUBTYPE_IF09              ,TEXT("Indeo YVU9\0"),
				&MEDIASUBTYPE_CPLA              ,TEXT("Cinepak UYVY\0"),
				&MEDIASUBTYPE_MJPG              ,TEXT("Motion JPEG\0"),
				&MEDIASUBTYPE_TVMJ              ,TEXT("TrueVision MJPG\0"),
				&MEDIASUBTYPE_WAKE              ,TEXT("MJPG (Wake)\0"),
				&MEDIASUBTYPE_CFCC              ,TEXT("MJPG (CFCC)\0"),
				&MEDIASUBTYPE_IJPG              ,TEXT("Intergraph JPEG\0"),
				&MEDIASUBTYPE_Plum              ,TEXT("Plum MJPG\0"),
				&MEDIASUBTYPE_RGB1              ,TEXT("RGB1 (Palettized)\0"),
				&MEDIASUBTYPE_RGB4              ,TEXT("RGB4 (Palettized)\0"),
				&MEDIASUBTYPE_RGB8              ,TEXT("RGB8 (Palettized)\0"),
				&MEDIASUBTYPE_RGB565            ,TEXT("RGB565\0"),
				&MEDIASUBTYPE_RGB555            ,TEXT("RGB555\0"),
				&MEDIASUBTYPE_RGB24             ,TEXT("RGB24\0"),
				&MEDIASUBTYPE_RGB32             ,TEXT("RGB32\0"),
				&MEDIASUBTYPE_ARGB32            ,TEXT("ARGB32\0"),
				&MEDIASUBTYPE_Overlay           ,TEXT("Overlay video (from HW)\0"),
				&MEDIASUBTYPE_QTMovie           ,TEXT("Apple QuickTime\0"),
				&MEDIASUBTYPE_QTRpza            ,TEXT("QuickTime RPZA\0"),
				&MEDIASUBTYPE_QTSmc             ,TEXT("QuickTime SMC\0"),
				&MEDIASUBTYPE_QTRle             ,TEXT("QuickTime RLE\0"),
				&MEDIASUBTYPE_QTJpeg            ,TEXT("QuickTime JPEG\0"),
				&MEDIASUBTYPE_dvsd              ,TEXT("Standard DV\0"),
				&MEDIASUBTYPE_dvhd              ,TEXT("High Definition DV\0"),
				&MEDIASUBTYPE_dvsl              ,TEXT("Long Play DV\0"),
				&MEDIASUBTYPE_MPEG1Packet       ,TEXT("MPEG1 Video Packet\0"),
				&MEDIASUBTYPE_MPEG1Payload      ,TEXT("MPEG1 Video Payload\0"),
				&MEDIASUBTYPE_VPVideo           ,TEXT("Video port video\0"),
				&MEDIASUBTYPE_VPVBI             ,TEXT("Video port VBI\0"),
				0, 0
			};
			for(int i = 0; videosubtypes[i].pguid; i++) {
				if(IsEqualGUID(*videosubtypes[i].pguid, *pguid)) return videosubtypes[i].name;
			}
			return _T("Unknown");
		}

		// TextureRenderer Class Declarations
		class TextureRenderer : public CBaseVideoRenderer
		{
		public:
			TextureRenderer(IUnknown *pUnk, HRESULT *phr, bool allow_alpha, bool allow_dynamic = true)
				: CBaseVideoRenderer(__uuidof(CLSID_TextureRenderer), NAME("Texture Renderer"), pUnk, phr)
				, use_dynamic_textures_(allow_dynamic), allow_alpha_(allow_alpha), alpha_(false)
			{
			}
			
		public:
			// Format acceptable?
			HRESULT CheckMediaType(const CMediaType *pmt)
			{
				GCTP_TRACE("CheckMediaType");
				if(!pmt) return E_POINTER;

				VIDEOINFO *pvi = 0;

				// Reject the connection if this is not a video type
				if( *pmt->FormatType() != FORMAT_VideoInfo ) {
					GCTP_TRACE("!FORMAT_VideoInfo");
					return E_INVALIDARG;
				}

				// Only accept RGB24 video
				pvi = (VIDEOINFO *)pmt->Format();

				if(!IsEqualGUID(*pmt->Type(), MEDIATYPE_Video)) {
					GCTP_TRACE("!MEDIATYPE_Video");
					return E_FAIL;
				}
				if(allow_alpha_ && IsEqualGUID(*pmt->Subtype(), MEDIASUBTYPE_ARGB32)) {
					alpha_ = true;
				}
				else if(!IsEqualGUID(*pmt->Subtype(), MEDIASUBTYPE_RGB24)) {
					const GUID *pguid = pmt->Subtype();
					char ss[128];
					sprintf_s(ss, "0x%08x, 0x%04x, 0x%04x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x", pguid->Data1, pguid->Data2, pguid->Data3, pguid->Data4[0], pguid->Data4[1], pguid->Data4[2], pguid->Data4[3], pguid->Data4[4], pguid->Data4[5], pguid->Data4[6], pguid->Data4[7]);
					GCTP_TRACE("!MEDIASUBTYPE_RGB24 "<<getVideoSubtypeString(pmt->Subtype())<<" "<<ss);
					return E_FAIL;
				}
				return S_OK;
			}

			// Video format notification
			HRESULT SetMediaType(const CMediaType *pmt)
			{
				GCTP_TRACE("SetMediaType");
				if(!pmt) return E_POINTER;

				HRslt hr;
				// Retrive the size of this media type
				graphic::dx::D3DCAPS caps;
				VIDEOINFO *pviBmp;                      // Bitmap info header
				pviBmp = (VIDEOINFO *)pmt->Format();

				width_  = pviBmp->bmiHeader.biWidth;
				// Bmpの反転格納の情報を伝えるために、heightは符号付。
				// 上下逆の場合、heightがマイナスになる（BMP情報と逆な点に注意）
				height_ = -pviBmp->bmiHeader.biHeight;
				pitch_  = (width_ * 3 + 3) & ~(3); // We are forcing RGB24
				PRNN("Movie Size : "<<width_<<" "<<height_<<" "<<pitch_);

				// here let's check if we can use dynamic textures
				ZeroMemory(&caps, sizeof(graphic::dx::D3DCAPS));
				hr = graphic::device().impl().ptr()->GetDeviceCaps( &caps );
				if( use_dynamic_textures_ && !(caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) )
				{
					use_dynamic_textures_ = false;
				}

				// Create the texture that maps to this media type
				texture_ = new graphic::Texture;
				hr = texture_->setUp(use_dynamic_textures_ ? graphic::Texture::WORK : graphic::Texture::NORMAL, abs(width_), abs(height_), alpha_ ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8, 1);
				if(use_dynamic_textures_ && !hr) {
					use_dynamic_textures_ = false;
					hr = texture_->setUp(use_dynamic_textures_ ? graphic::Texture::WORK : graphic::Texture::NORMAL, abs(width_), abs(height_), alpha_ ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8, 1);
				}
				if(!hr) {
					//Msg(TEXT("Could not create the D3DX texture!  hr=0x%x"), hr);
					GCTP_TRACE(_T("Could not create the D3DX texture!:")<<hr);
					return hr;
				}
				if(use_dynamic_textures_) {
					GCTP_TRACE(_T("Dynamic"));
				}
				else {
					GCTP_TRACE(_T("Non Dynamic"));
				}

				// CreateTexture can silently change the parameters on us
				D3DSURFACE_DESC ddsd;
				ZeroMemory(&ddsd, sizeof(ddsd));
				if(!(hr = texture_->get()->GetLevelDesc(0, &ddsd))) {
					//Msg(TEXT("Could not get level Description of D3DX texture! hr = 0x%x"), hr);
					GCTP_TRACE(_T("Could not create the D3DX texture!:")<<hr);
					return hr;
				}

				{
					graphic::dx::IDirect3DSurfacePtr surf;
					if(hr = texture_->get()->GetSurfaceLevel(0, &surf))
						surf->GetDesc(&ddsd);
				}

				// Save format info
				texture_format_ = ddsd.Format;
				GCTP_TRACE(_T("Texture size : ") << ddsd.Width << " , " << ddsd.Height);

				if (texture_format_ != D3DFMT_X8R8G8B8 && texture_format_ != D3DFMT_A8R8G8B8
				 && texture_format_ != D3DFMT_X1R5G5B5 && texture_format_ != D3DFMT_A1R5G5B5) {
					//Msg(TEXT("Texture is format we can't handle! Format = 0x%x"), texture_format_);
					GCTP_TRACE(_T("Could not create the D3DX texture!:")<<hr);
					return VFW_E_TYPE_NOT_ACCEPTED;
				}

				return S_OK;
			}

			void FillTo32From32(BYTE *pTxtBuffer, LONG lTxtPitch, BYTE *pBmpBuffer)
			{
				UINT dwordWidth = width_ / 4; // aligned width of the row, in DWORDS
											  // (pixel by 3 bytes over sizeof(DWORD))
				UINT dwordModWidth = width_ % 4;

				UINT height = abs(height_);
				UINT width_len = abs(height_)*4;
				for(UINT row = 0; row < height; row++)
				{
					memcpy(pTxtBuffer, pBmpBuffer, width_len);
					pBmpBuffer += pitch_;
					pTxtBuffer += lTxtPitch;
				}
			}

			void FillTo32From24(BYTE *pTxtBuffer, LONG lTxtPitch, BYTE *pBmpBuffer)
			{
				//GCTP_TRACE(_T("D3DFMT_X8R8G8B8|D3DFMT_A8R8G8B8"));

				// Instead of copying data bytewise, we use DWORD alignment here.
				// We also unroll loop by copying 4 pixels at once.
				//
				// original BYTE array is [b0][g0][r0][b1][g1][r1][b2][g2][r2][b3][g3][r3]
				//
				// aligned DWORD array is     [b1 r0 g0 b0][g2 b2 r1 g1][r3 g3 b3 r2]
				//
				// We want to transform it to [ff r0 g0 b0][ff r1 g1 b1][ff r2 g2 b2][ff r3 b3 g3]
				// below, bitwise operations do exactly this.

				BYTE  * pbS = NULL;
				DWORD * pdwS = NULL;
				DWORD * pdwD = NULL;

				UINT dwordWidth = width_ / 4; // aligned width of the row, in DWORDS
											  // (pixel by 3 bytes over sizeof(DWORD))
				UINT dwordModWidth = width_ % 4;

				UINT height = abs(height_);
				for(UINT row = 0; row < height; row++)
				{
					pdwS = (DWORD*)pBmpBuffer;
					pdwD = (DWORD*)pTxtBuffer;

					for(UINT col = 0; col < dwordWidth; col++)
					{
						pdwD[0] =  pdwS[0] | 0xFF000000;
						pdwD[1] = ((pdwS[1]<<8)  | 0xFF000000) | (pdwS[0]>>24);
						pdwD[2] = ((pdwS[2]<<16) | 0xFF000000) | (pdwS[1]>>16);
						pdwD[3] = 0xFF000000 | (pdwS[2]>>8);
						pdwD +=4;
						pdwS +=3;
					}

					// we might have remaining (misaligned) bytes here
					pbS = (BYTE*) pdwS;
					for(UINT col = 0; col < dwordModWidth; col++)
					{
						*pdwD = 0xFF000000     |
								(pbS[2] << 16) |
								(pbS[1] <<  8) |
								(pbS[0]);
						pdwD++;
						pbS += 3;
					}

					pBmpBuffer += pitch_;
					pTxtBuffer += lTxtPitch;
				}// for rows
			}

			void FillTo16From32(BYTE *pTxtBuffer, LONG lTxtPitch, BYTE *pBmpBuffer)
			{
				//GCTP_TRACE(_T("D3DFMT_A1R5G5B5"));

				int height = abs(height_);
				for(int y = 0; y < height; y++)
				{
					BYTE *pBmpBufferOld = pBmpBuffer;
					BYTE *pTxtBufferOld = pTxtBuffer;

					for (int x = 0; x < width_; x++)
					{
						*(WORD *)pTxtBuffer = (WORD)
							(((pBmpBuffer[3] & 0xF0)?0x8000:0) +
							((pBmpBuffer[2] & 0xF8) << 7) +
							((pBmpBuffer[1] & 0xF8) << 2) +
							(pBmpBuffer[0] >> 3));

						pTxtBuffer += 2;
						pBmpBuffer += 4;
					}

					pBmpBuffer = pBmpBufferOld + pitch_;
					pTxtBuffer = pTxtBufferOld + lTxtPitch;
				}
			}

			void FillTo16From24(BYTE *pTxtBuffer, LONG lTxtPitch, BYTE *pBmpBuffer)
			{
				//GCTP_TRACE(_T("D3DFMT_A1R5G5B5"));

				int height = abs(height_);
				for(int y = 0; y < height; y++)
				{
					BYTE *pBmpBufferOld = pBmpBuffer;
					BYTE *pTxtBufferOld = pTxtBuffer;

					for (int x = 0; x < width_; x++)
					{
						*(WORD *)pTxtBuffer = (WORD)
							(0x8000 +
							((pBmpBuffer[2] & 0xF8) << 7) +
							((pBmpBuffer[1] & 0xF8) << 2) +
							(pBmpBuffer[0] >> 3));

						pTxtBuffer += 2;
						pBmpBuffer += 3;
					}

					pBmpBuffer = pBmpBufferOld + pitch_;
					pTxtBuffer = pTxtBufferOld + lTxtPitch;
				}
			}

			// New video sample
			HRESULT DoRenderSample(IMediaSample *pSample)
			{
				HRslt hr;

				if(!pSample) {
					GCTP_TRACE("E_POINTER");
					return E_POINTER;
				}
				if(!texture_) {
					GCTP_TRACE("E_UNEXPECTED");
					return E_UNEXPECTED;
				}

				graphic::dx::IDirect3DTexturePtr texture = texture_->get();
				if(!texture) {
					GCTP_TRACE("E_UNEXPECTED");
					return E_UNEXPECTED;
				}

				// Get the video bitmap buffer
				BYTE  *pBmpBuffer;
				pSample->GetPointer( &pBmpBuffer );

				// Lock the Texture
				D3DLOCKED_RECT d3dlr;
				if( use_dynamic_textures_ ) {
					if(!(hr = texture->LockRect(0, &d3dlr, 0, D3DLOCK_DISCARD))) {
						GCTP_TRACE(hr);
						return hr;
					}
				}
				else {
					if(!(hr = texture->LockRect(0, &d3dlr, 0, 0))) {
						GCTP_TRACE(hr);
						return hr;
 					}
				}
				// Get the texture buffer & pitch
				BYTE *pTxtBuffer = static_cast<byte *>(d3dlr.pBits);
				LONG lTxtPitch = d3dlr.Pitch;
				//PRNN("Pitch "<<lTxtPitch);

				// Copy the bits
				if (texture_format_ == D3DFMT_X8R8G8B8 || texture_format_ == D3DFMT_A8R8G8B8) {
					FillTo32From24(pTxtBuffer, lTxtPitch, pBmpBuffer);
				}
				else if (texture_format_ == D3DFMT_A1R5G5B5) {
					FillTo16From24(pTxtBuffer, lTxtPitch, pBmpBuffer);
				}
				else {
					GCTP_TRACE(_T("非対応なテクスチャフォーマット"));
				}

				// Unlock the Texture
				if( !(hr = texture->UnlockRect(0)) ) {
					GCTP_TRACE(hr);
					return hr;
				}

				return S_OK;
			}
			
			bool use_dynamic_textures_;
			bool allow_alpha_; // ARGB32 only
			bool alpha_; // RGB24 or ARGB32
			LONG width_;   // Video width
			LONG height_;  // Video Height
			LONG pitch_;   // Video Pitch,
			Pointer<graphic::Texture> texture_;
			D3DFORMAT texture_format_;
		};

#ifdef REGISTER_FILTERGRAPH
		//-----------------------------------------------------------------------------
		// Running Object Table functions: Used to debug. By registering the graph
		// in the running object table, GraphEdit is able to connect to the running
		// graph. This code should be removed before the application is shipped in
		// order to avoid third parties from spying on your graph.
		//-----------------------------------------------------------------------------
		DWORD dwROTReg = 0xfedcba98;

		HRESULT AddToROT(IUnknown *pUnkGraph)
		{
			IMoniker * pmk;
			IRunningObjectTable *pROT;
			if (FAILED(GetRunningObjectTable(0, &pROT))) {
				return E_FAIL;
			}

			WCHAR wsz[256];
			(void)StringCchPrintfW(wsz, NUMELMS(wsz),L"FilterGraph %08x  pid %08x\0", (DWORD_PTR) 0, GetCurrentProcessId());

			HRESULT hr = CreateItemMoniker(L"!", wsz, &pmk);
			if (SUCCEEDED(hr))
			{
				// Use the ROTFLAGS_REGISTRATIONKEEPSALIVE to ensure a strong reference
				// to the object.  Using this flag will cause the object to remain
				// registered until it is explicitly revoked with the Revoke() method.
				//
				// Not using this flag means that if GraphEdit remotely connects
				// to this graph and then GraphEdit exits, this object registration
				// will be deleted, causing future attempts by GraphEdit to fail until
				// this application is restarted or until the graph is registered again.
				hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,
									pmk, &dwROTReg);
				pmk->Release();
			}

			pROT->Release();
			return hr;
		}

		void RemoveFromROT()
		{
			IRunningObjectTable *pirot=0;

			if (SUCCEEDED(GetRunningObjectTable(0, &pirot)))
			{
				pirot->Revoke(dwROTReg);
				pirot->Release();
			}
		}
#endif

	}

	/** デストラクタ
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Player::~Player()
	{
#ifdef REGISTER_FILTERGRAPH
		// Pull graph from Running Object Table (Debug)
		RemoveFromROT();
#endif
		if(media_control_) media_control_->Stop();
	}

	/** セットアップ
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Player::openForTexture(const _TCHAR *path, bool audio_on)
	{
		HRslt hr;

		is_ready_ = false;
		hwnd_ = 0;
		// Create the filter graph
		if(!(hr = graph_builder_.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC)))
			return hr;

#ifdef REGISTER_FILTERGRAPH
		// Register the graph in the Running Object Table (for debug purposes)
		AddToROT(graph_builder_);
#endif

		// Create the Texture Renderer object
		TextureRenderer *pTR = new TextureRenderer(NULL, &hr.i, true);// DirectShow Texture renderer
		if(!(hr) || !pTR) {
			delete pTR;
			//Msg(TEXT("Could not create texture renderer object!  hr=0x%x"), hr);
			GCTP_TRACE(_T("Could not create texture renderer object! : ")<<hr);
			return hr;
		}

		// Get a pointer to the IBaseFilter on the TextureRenderer, add it to graph
		renderer_ = (IBaseFilter *)pTR;
		if(!(hr = graph_builder_->AddFilter(renderer_, L"TEXTURERENDERER"))) {
			//Msg(TEXT("Could not add renderer filter to graph!  hr=0x%x"), hr);
			GCTP_TRACE(_T("Could not add renderer filter to graph! : ") << hr);
			return hr;
		}

		// Determine the file to load based on windows directory
		// Use the standard win32 API to do this.
		WCHAR wFileName[MAX_PATH];
		wFileName[MAX_PATH-1] = 0;    // NULL-terminate

		USES_CONVERSION;
		(void)StringCchCopyW(wFileName, NUMELMS(wFileName), T2W((_TCHAR *)path));

		IPinPtr pFSrcPinOut;    // Source Filter Output Pin
		if(ArchiveMediaFile::needThis(path)) {
			//PRNN(_T("アーカイブ内メディアファイルをストリーム"));
			ArchiveMediaFile *archive_source = new ArchiveMediaFile;
			if(archive_source) archive_source_ = archive_source;
			if(hr = archive_source->open(path)) {
				hr = graph_builder_->AddFilter(archive_source->archive_reader_, 0);
				if(!hr) {
					GCTP_TRACE(hr);
					return hr;
				}
				if(!(hr = archive_source->archive_reader_->FindPin(L"Output", &pFSrcPinOut))) {
					//Msg(TEXT("Could not find output pin!  hr=0x%x"), hr);
					GCTP_TRACE(_T("Could not find output pin! : ") << hr);
					return hr;
				}
			}
			else {
				archive_source_ = 0;
			}
		}
		if(!archive_source_) {
			IBaseFilterPtr pFSrc; // Source Filter
			// Add the source filter to the graph.
			hr = graph_builder_->AddSourceFilter(wFileName, L"SOURCE", &pFSrc);

			// If the media file was not found, inform the user.
			if(hr == VFW_E_NOT_FOUND) {
				//Msg(TEXT("Could not add source filter to graph!  (hr==VFW_E_NOT_FOUND)\r\n\r\n")
				//	TEXT("This sample reads a media file from your windows directory.\r\n")
				//	TEXT("This file is missing from this machine."));
				GCTP_TRACE(_T("\n")
						   _T("\tCould not add source filter to graph!  (hr==VFW_E_NOT_FOUND)\n\n")
						   _T("\tThis sample reads a media file from your windows directory.\n")
						   _T("\tThis file is missing from this machine."));
				return hr;
			}
			else if(!hr) {
				//Msg(TEXT("Could not add source filter to graph!  hr=0x%x"), hr);
				GCTP_TRACE(_T("Could not add source filter to graph! : ") << hr);
				return hr;
			}

			if (!(hr = pFSrc->FindPin(L"Output", &pFSrcPinOut))) {
				//Msg(TEXT("Could not find output pin!  hr=0x%x"), hr);
				GCTP_TRACE(_T("Could not find output pin! : ") << hr);
				return hr;
			}
		}

		if(audio_on) {
			// Render the source filter's output pin.  The Filter Graph Manager
			// will connect the video stream to the loaded CTextureRenderer
			// and will load and connect an audio renderer (if needed).

			if(!(hr = graph_builder_->Render(pFSrcPinOut))) {
				//Msg(TEXT("Could not render source output pin!  hr=0x%x"), hr);
				GCTP_TRACE(_T("Could not render source output pin! : ") << hr);
				return hr;
			}
		}
		else {
			// If no audio component is desired, directly connect the two video pins
			// instead of allowing the Filter Graph Manager to render all pins.

			IPinPtr pFTRPinIn;      // Texture Renderer Input Pin

			// Find the source's output pin and the renderer's input pin
			if(!(hr = pTR->FindPin(L"In", &pFTRPinIn))) {
				//Msg(TEXT("Could not find input pin!  hr=0x%x"), hr);
				GCTP_TRACE(_T("Could not find input pin! : ") << hr);
				return hr;
			}

			// Connect these two filters
			if(!(hr = graph_builder_->Connect(pFSrcPinOut, pFTRPinIn))) {
				//Msg(TEXT("Could not connect pins!  hr=0x%x"), hr);
				GCTP_TRACE(_T("Could not connect pins! : ") << hr);
				return hr;
			}
		}

		if(pTR->texture_) {
			//　これがあるなら正常にCTextureRendererがセットアップされたはず
			// Get the graph's media control, event & position interfaces
			graph_builder_.QueryInterface(&media_control_);
			graph_builder_.QueryInterface(&media_position_);
			graph_builder_.QueryInterface(&media_event_);
			graph_builder_.QueryInterface(&audio_);
			return S_OK;
		}
		else {
			GCTP_TRACE(_T("TextureRenderer製作失敗 : おそらくフォーマット非対応"));
			renderer_ = 0;
			graph_builder_ = 0;
		}

		return E_FAIL;
	}

	/** セットアップ
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Player::open(HWND hwnd, int notify_msgid, const _TCHAR *path, bool audio_on)
	{
		USES_CONVERSION;
		WCHAR wFile[MAX_PATH];
		HRslt hr;

		if(!path) return E_POINTER;
		
		is_ready_ = false;

		// Convert filename to wide character string
		wcsncpy(wFile, T2W((LPTSTR)path), NUMELMS(wFile)-1);
		wFile[MAX_PATH-1] = 0;

		IFilterGraphPtr filter_graph;
		hr = ::CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IFilterGraph, (void **)&filter_graph);
		//if(!(hr = filter_graph.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC)))
		//	return hr;
		if(!hr) return hr;
		
		hwnd_ = 0;
		// Get the interface for DirectShow's GraphBuilder
		//if(!(hr = ::CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&graph_builder_))
		//	return hr;
		graph_builder_ = filter_graph;
		//if(!(hr = graph_builder_.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC)))
		//	return hr;

		if(hr) {
			if(ArchiveMediaFile::needThis(path)) {
				//PRNN(_T("アーカイブ内メディアファイルをストリーム"));
				ArchiveMediaFile *archive_source = new ArchiveMediaFile;
				if(archive_source) archive_source_ = archive_source;
				if(archive_source->open(path)) {
					hr = filter_graph->AddFilter(archive_source->archive_reader_, 0);
					if(!hr) {
						GCTP_TRACE(hr);
						return hr;
					}
					hr = graph_builder_->Render(archive_source->archive_reader_->GetPin(0));
					if(!hr) {
						GCTP_TRACE(hr);
						return hr;
					}
				}
				else {
					archive_source_ = 0;
				}
			}
			// Have the graph builder construct its the appropriate graph automatically
			if(!archive_source_) {
				if(!(hr = graph_builder_->RenderFile(wFile, NULL)))
					return hr;
			}

			// QueryInterface for DirectShow interfaces
			if(!(hr = graph_builder_.QueryInterface(&media_control_)))
				return hr;
			if(!(hr = graph_builder_.QueryInterface(&media_position_)))
				return hr;
			if(!(hr = graph_builder_.QueryInterface(&media_event_)))
				return hr;
			//JIF(pGB->QueryInterface(IID_IMediaControl, (void **)&pMC));
			//JIF(pGB->QueryInterface(IID_IMediaEventEx, (void **)&pME));
			//JIF(pGB->QueryInterface(IID_IMediaSeeking, (void **)&pMS));
			//JIF(pGB->QueryInterface(IID_IMediaPosition, (void **)&pMP));

			// Query for video interfaces, which may not be relevant for audio files
			com_ptr<IVideoWindow, &IID_IVideoWindow> video_window;
			if(!(hr = graph_builder_.QueryInterface(&video_window))) {
				GCTP_TRACE(hr);
				return hr;
			}
			//JIF(pGB->QueryInterface(IID_IVideoWindow, (void **)&pVW));
			com_ptr<IVideoWindow, &IID_IVideoWindow> basic_video;
			if(!(hr = graph_builder_.QueryInterface(&basic_video))) {
				GCTP_TRACE(hr);
				return hr;
			}
			//JIF(pGB->QueryInterface(IID_IBasicVideo, (void **)&pBV));

			// Query for audio interfaces, which may not be relevant for video-only files
			//JIF(pGB->QueryInterface(IID_IBasicAudio, (void **)&pBA));
			if(!(hr = graph_builder_.QueryInterface(&audio_)))
				return hr;

			// Have the graph signal event via window callbacks for performance
			//JIF(pME->SetNotifyWindow((OAHWND)ghApp, WM_GRAPHNOTIFY, 0));
			if(notify_msgid > 0) {
				if(!(hr = media_event_->SetNotifyWindow((OAHWND)hwnd, notify_msgid, 0))) {
					GCTP_TRACE(hr);
					return hr;
				}
			}

			// Is this an audio-only file (no video component)?
			//CheckVisibility();
			if(!basic_video) video_window = 0;
			if(video_window) {
				long visible;
				HRslt hr = video_window->get_Visible(&visible);
				if(!hr) {
					// If this is an audio-only clip, get_Visible() won't work.
					//
					// Also, if this video is encoded with an unsupported codec,
					// we won't see any video, although the audio will work if it is
					// of a supported format.
					//
					if(hr == E_NOINTERFACE) {
						video_window = 0;
					}
					else {
						//Msg(TEXT("Failed(%08lx) in pVW->get_Visible()!\r\n"), hr);
						GCTP_TRACE(hr);
					}
				}
			}
			if(video_window) { // 映像がある
				if(!(hr = video_window->put_Owner((OAHWND)hwnd))) {
					GCTP_TRACE(hr);
					return hr;
				}
				if(!(hr = video_window->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN))) {
					GCTP_TRACE(hr);
					return hr;
				}
				if(!(hr = video_window->put_Top(0))) {
					GCTP_TRACE(hr);
					return hr;
				}
				if(!(hr = video_window->put_Left(0))) {
					GCTP_TRACE(hr);
					return hr;
				}
				//JIF(pVW->put_Owner((OAHWND)ghApp));
				//JIF(pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN));

				//JIF(InitVideoWindow(1, 1));

				// Complete window initialization
				//CheckSizeMenu(ID_FILE_SIZE_NORMAL);
				::ShowWindow(hwnd, SW_SHOWNORMAL);
				::UpdateWindow(hwnd);
				::SetForegroundWindow(hwnd);
				//g_bFullscreen = FALSE;
				//g_PlaybackRate = 1.0;
				//UpdateMainTitle();
				hwnd_ = hwnd;
			}

			if(audio_on) {
			}
			else {
			}

#ifdef REGISTER_FILTERGRAPH
			AddToROT(graph_builder_);
			/*hr = AddGraphToRot(pGB, &g_dwGraphRegister);
			if(FAILED(hr)) {
				Msg(TEXT("Failed to register filter graph with ROT!  hr=0x%x"), hr);
				g_dwGraphRegister = 0;
			}*/
#endif

			// Run the graph to play the media file
			//JIF(pMC->Run());
			// Start the graph running;
			/*if(!(hr = media_control_->Pause())) {
				//Msg(TEXT("Could not run the DirectShow graph!  hr=0x%x"), hr);
				GCTP_TRACE(_T("Could not run the DirectShow graph! : "<<hr));
			}*/

			//g_psCurrent=Running;
		}

		return hr;
	}

	/** 再生中か？
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool Player::isPlaying()
	{
		return loop_count_ < loop_;
	}

	/** 再生
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Player::play(int loop)
	{
		HRslt hr;
		// Start the graph running;
		if(!(hr = media_control_->Run())) {
			//Msg(TEXT("Could not run the DirectShow graph!  hr=0x%x"), hr);
			GCTP_TRACE(_T("Could not run the DirectShow graph! : "<<hr));
			return hr;
		}
		
		loop_count_ = 0;
		loop_ = loop;
		invalidate_timer_ = timeGetTime();
		return hr;
	}

	/** 停止
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 19:24:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Player::stop()
	{
		HRslt hr;
		// Start the graph running;
		if(!(hr = media_control_->Stop())) {
			//Msg(TEXT("Could not run the DirectShow graph!  hr=0x%x"), hr);
			GCTP_TRACE(_T("Could not run the DirectShow graph! : "<<hr));
			return hr;
		}
		loop_count_ = loop_ = 1;
		return hr;
	}

	void Player::setVolume(float volume)
	{
		if(audio_) {
			LONG arg = 0;
			if(volume >= 1) arg = 0;
			else if(volume <= 0) arg = -10000;
			else if(volume > 0) arg = (LONG)((1-volume)*min_gain);
			HRslt hr = audio_->put_Volume(arg);
			if(!hr) GCTP_TRACE(hr);
		}
	}

	float Player::getVolume()
	{
		if(audio_) {
			float volume;
			LONG ret = 0;
			HRslt hr = audio_->get_Volume(&ret);
			if(hr) {
				if(ret >= 0) volume = 1;
				if(ret <= -10000) volume = 0;
				else if(ret > 1) volume = (min_gain-ret)/min_gain;
				return volume;
			}
			else GCTP_TRACE(hr);
		}
		return 0;
	}

	void Player::setPan(float pan)
	{
		if(audio_) {
			LONG arg = 0;
			if(pan >= 1) arg = 10000;
			else if(pan <= -1) arg = -10000;
			else if(pan != 0) arg = (LONG)(-pan*min_gain);
			HRslt hr = audio_->put_Balance(arg);
			if(!hr) GCTP_TRACE(hr);
		}
	}

	float Player::getPan()
	{
		if(audio_) {
			float pan;
			LONG ret = 0;
			HRslt hr = audio_->get_Balance(&ret);
			if(hr) {
				if(ret == 10000) pan = 1;
				else if(ret > 0) pan = -ret/min_gain;
				else if(ret == -10000) pan = -1;
				else if(ret < 0) pan = ret/min_gain;
				if(pan < -1) pan = -1;
				else if(pan > 1) pan = 1;
				return pan;
			}
			else GCTP_TRACE(hr);
		}
		return 0;
	}

	Handle<graphic::Texture> Player::getTexture()
	{
		if(renderer_) {
			IBaseFilter *filter = renderer_;
			if(filter) {
				if(((TextureRenderer *)filter)->texture_)
					return ((TextureRenderer *)filter)->texture_;
			}
		}
		return 0;
	}

	HRslt Player::checkStatus()
	{
		long lEventCode;
		LONG_PTR lParam1, lParam2;
		HRslt hr;

		if (!media_event_) {
			loop_count_ = loop_ = 1;
			return hr;
		}

		if(isPlaying() && hwnd_ && is_ready_) {
			DWORD now = timeGetTime();
			if(now - invalidate_timer_ > 100) {
				invalidate_timer_ = now;
				::InvalidateRect(hwnd_, 0, FALSE);
				// これが結局一番簡単だった…
			}
		}

		/*{
			OAFilterState state;
			hr = media_control_->GetState(INFINITE, &state);
			GCTP_TRACE(hr);
			PRNN("state = "<<state);
		}*/

		// Check for completion events
		hr = media_event_->GetEvent(&lEventCode, &lParam1, &lParam2, 0);
		if(hr) {
			// If we have reached the end of the media file, reset to beginning
			if(EC_COMPLETE == lEventCode) {
				if(loop_ > 0) loop_count_++;
				if(loop_ == 0 || loop_count_ < loop_) {
					hr = media_position_->put_CurrentPosition(0);
				}
			}
			else if(EC_PAUSED == lEventCode) {
				// どうも初期化が終わると一度は来る見たい
				is_ready_ = true;
				PRNN("lEventCode = EC_PAUSED");
			}
			else {
				PRNN("lEventCode = "<<lEventCode);
			}
			// Free any memory associated with this event
			hr = media_event_->FreeEventParams(lEventCode, lParam1, lParam2);
		}
		return hr;
	}

	int Player::width()
	{
		if(renderer_) {
			IBaseFilter *filter = renderer_;
			if(filter) {
				return ((TextureRenderer *)filter)->width_;
			}
		}
		return 0;
	}

	int Player::height()
	{
		if(renderer_) {
			IBaseFilter *filter = renderer_;
			if(filter) {
				return ((TextureRenderer *)filter)->height_;
			}
		}
		return 0;
	}

}}} // namespace gctp
