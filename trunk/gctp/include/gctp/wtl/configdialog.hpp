#ifndef _GCTP_WTL_CONFIGDIALOG_HPP_
#define _GCTP_WTL_CONFIGDIALOG_HPP_
/**@file
 * 起動設定ダイアログ
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/26 18:17:48
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <atlbase.h>
#include <atlapp.h>
#if (_ATL_VER < 0x0700)
extern CAppModule _Module;
#define _AtlBaseModule _Module
#endif
#include <atlwin.h>
#include <atlcrack.h>
#include <atlctrls.h>
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/device.hpp>

namespace gctp { namespace wtl {

	/** 起動設定ダイアログ
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/26 15:40:36
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class ConfigDialog : public CDialogImpl<ConfigDialog> {
		CButton		okbutton_;
		CComboBox	screens_;
	public:
		enum {IDD = IDD_STARTUPDLG_DIALOG};

		bool	is_fs_;
		uint	mode_;
		std::vector<uint> modes_;

		ConfigDialog(bool is_fs) : is_fs_(is_fs), mode_(0) {}

		BEGIN_MSG_MAP_EX(ConfigDialog)
			MSG_WM_INITDIALOG(onInitDialog)
			COMMAND_ID_HANDLER(IDOK, onCloseCmd)
			COMMAND_ID_HANDLER(IDCANCEL, onCloseCmd)
		END_MSG_MAP()

		LRESULT onInitDialog(HWND /*forcus*/, LPARAM /*initparam*/)
		{
			CenterWindow(GetParent());
			okbutton_ = GetDlgItem(IDOK);
			screens_ = GetDlgItem(IDC_SCREENLIST);
			CheckRadioButton(IDC_RADIO_FSMODE, IDC_RADIO_WNDMODE, (is_fs_)?IDC_RADIO_FSMODE:IDC_RADIO_WNDMODE);
			for(uint i = 0; i < modes_.size(); i++) {
				const D3DDISPLAYMODE &mode = graphic::dx::adapters()[0].modes[modes_[i]];
				addToComboBox(i, mode.Width, mode.Height, mode.Format, mode.RefreshRate);
			}
			if(screens_.GetCount()>0) screens_.SetCurSel(0);
			else {
				screens_.EnableWindow(FALSE);
				okbutton_.EnableWindow(FALSE);
			}
			return TRUE;
		}

		LRESULT onCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			is_fs_ = (IsDlgButtonChecked(IDC_RADIO_FSMODE)==BST_CHECKED);
			int i = screens_.GetItemData(screens_.GetCurSel());
			mode_ = modes_[i];
			EndDialog(wID);
			return 0;
		}
		
		void addMode(uint mode) {
			modes_.push_back(mode);
		}

	private:
		void addToComboBox(DWORD data, DWORD width, DWORD height, DWORD format, DWORD refresh_rate)
		{
			char *str;
			char *_str[2] = { "ハイカラー", "フルカラー", };
			switch(format) {
			case D3DFMT_R8G8B8: case D3DFMT_A8R8G8B8: case D3DFMT_X8R8G8B8:
				str = _str[1];
				break;
			case D3DFMT_R5G6B5: case D3DFMT_X1R5G5B5: case D3DFMT_A1R5G5B5: case D3DFMT_A4R4G4B4:
			case D3DFMT_R3G3B2: case D3DFMT_A8R3G3B2: case D3DFMT_X4R4G4B4:
				str = _str[0];
				break;
			default:
				return;
			};
#ifdef UNICODE
			wchar_t screenstr[256];
			if(refresh_rate==D3DPRESENT_RATE_DEFAULT) wsprintf(screenstr, L"%d×%d:%s:規定値", width, height, str);
			else wsprintf(screenstr, L"%d×%d:%s:%dHz", width, height, str, refresh_rate);
			int idx;
			idx = screens_.AddString(screenstr);
			screens_.SetItemData(idx, data);
#else
			char screenstr[256];
			if(refresh_rate==D3DPRESENT_RATE_DEFAULT) sprintf(screenstr, "%d×%d:%s:規定値", width, height, str);
			else sprintf(screenstr, "%d×%d:%s:%dHz", width, height, str, refresh_rate);
			int idx;
			idx = screens_.AddString(screenstr);
			screens_.SetItemData(idx, data);
#endif
		}
	};

}} //namespace gctp::wtl

#endif //_GCTP_WTL_CONFIGDIALOG_HPP_