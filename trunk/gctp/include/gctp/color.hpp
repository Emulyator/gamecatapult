#ifndef _GCTP_COLOR_HPP_
#define _GCTP_COLOR_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 色情報クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/12 18:39:23
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
namespace gctp {

	struct Color32;
	/** 色
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/12 18:39:29
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	struct Color {
		float r, g, b, a;

		Color() {}
		Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
		explicit Color(const Color32 &src);
		explicit Color(const char *ccode) { set(ccode); }

		Color &operator+=(const Color &rhs)
		{
			r += rhs.r; g += rhs.g; b += rhs.b; a += rhs.a;
			return *this;
		}
		Color &operator-=(const Color &rhs)
		{
			r -= rhs.r; g -= rhs.g; b -= rhs.b; a -= rhs.a;
			return *this;
		}
		Color &operator*=(const Color &rhs)
		{
			r *= rhs.r; g *= rhs.g; b *= rhs.b; a *= rhs.a;
			return *this;
		}
		Color &operator/=(const Color &rhs)
		{
			r /= rhs.r; g /= rhs.g; b /= rhs.b; a /= rhs.a;
			return *this;
		}
		Color &operator*=(float rhs)
		{
			r *= rhs; g *= rhs; b *= rhs; a *= rhs;
			return *this;
		}
		Color &operator/=(float rhs)
		{
			r /= rhs; g /= rhs; b /= rhs; a /= rhs;
			return *this;
		}

		Color operator+(const Color &rhs) const
		{
			return Color(*this)+=rhs;
		}
		Color operator-(const Color &rhs) const
		{
			return Color(*this)-=rhs;
		}
		Color operator*(const Color &rhs) const
		{
			return Color(*this)*=rhs;
		}
		Color operator/(const Color &rhs) const
		{
			return Color(*this)/=rhs;
		}
		Color operator*(float rhs) const
		{
			return Color(*this)*=rhs;
		}
		Color operator/(float rhs) const
		{
			return Color(*this)/=rhs;
		}

		friend Color operator*(float lhs, const Color &rhs)
		{
			return rhs*lhs;
		}
		friend Color operator/(float lhs, const Color &rhs)
		{
			return rhs/lhs;
		}

		operator float *() { return &r; }
		operator const float *() const { return &r; }
#ifdef GCTP_USE_D3DXMATH
		Color(const D3DXCOLOR &src)
		{
			r = src.r; g = src.g; b = src.b; a = src.a;
		}
		operator D3DXCOLOR &() { return *reinterpret_cast<D3DXCOLOR *>(this); }
		operator const D3DXCOLOR &() const { return *reinterpret_cast<const D3DXCOLOR *>(this); }
		operator D3DXCOLOR *() { return reinterpret_cast<D3DXCOLOR *>(this); }
		operator const D3DXCOLOR *() const { return reinterpret_cast<const D3DXCOLOR *>(this); }
#endif
#ifdef D3DCOLORVALUE_DEFINED
		Color(const D3DCOLORVALUE &src)
		{
			r = src.r; g = src.g; b = src.b; a = src.a;
		}
		operator D3DCOLORVALUE &() { return *reinterpret_cast<D3DCOLORVALUE *>(this); }
		operator const D3DCOLORVALUE &() const { return *reinterpret_cast<const D3DCOLORVALUE *>(this); }
		operator D3DCOLORVALUE *() { return reinterpret_cast<D3DCOLORVALUE *>(this); }
		operator const D3DCOLORVALUE *() const { return reinterpret_cast<const D3DCOLORVALUE *>(this); }
#endif
#ifdef D3DCOLOR_DEFINED
		operator D3DCOLOR() const;
#endif

		void set(float r, float g, float b, float a = 1.0f)
		{
			this->r = r; this->g = g; this->b = b; this->a = a;
		}

		void set(const char *ccode);

		Color &setLerp(const Color &lhs, const Color &rhs, float t)
		{
			return *this = lhs + (rhs-lhs)*t;
		}
		Color &setLerp(const Color &trg, float t)
		{
			return *this += (trg-*this)*t;
		}
	};

	/** 32bit色
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/12 18:39:29
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	struct Color32 {
		union {
			struct {
				uchar b, g, r, a;
			};// エンディアンス依存
			uint32_t i32;
		};

		Color32() {}
		Color32(uchar r, uchar g, uchar b, uchar a = 255) : a(a), r(r), g(g), b(b) {}

		explicit Color32(const Color &src)
			: a(static_cast<uchar>(src.a*255))
			, r(static_cast<uchar>(src.r*255))
			, g(static_cast<uchar>(src.g*255))
			, b(static_cast<uchar>(src.b*255))
		{
		}

		explicit Color32(const char *ccode)
		{
			set(ccode);
		}

		Color32 &operator+=(const Color32 &rhs)
		{
			*this = Color32(Color(*this)+Color(rhs));
			return *this;
		}
		Color32 &operator-=(const Color32 &rhs)
		{
			*this = Color32(Color(*this)-Color(rhs));
			return *this;
		}
		Color32 &operator*=(const Color32 &rhs)
		{
			*this = Color32(Color(*this)*Color(rhs));
			return *this;
		}
		Color32 &operator/=(const Color32 &rhs)
		{
			*this = Color32(Color(*this)/Color(rhs));
			return *this;
		}
		Color32 &operator*=(float rhs)
		{
			*this = Color32(Color(*this)*rhs);
			return *this;
		}
		Color32 &operator/=(float rhs)
		{
			*this = Color32(Color(*this)/rhs);
			return *this;
		}

		Color32 operator+(const Color32 &rhs) const
		{
			return Color32(*this)+=rhs;
		}
		Color32 operator-(const Color32 &rhs) const
		{
			return Color32(*this)-=rhs;
		}
		Color32 operator*(const Color32 &rhs) const
		{
			return Color32(*this)*=rhs;
		}
		Color32 operator/(const Color32 &rhs) const
		{
			return Color32(*this)/=rhs;
		}
		Color32 operator*(float rhs) const
		{
			return Color32(*this)*=rhs;
		}
		Color32 operator/(float rhs) const
		{
			return Color32(*this)/=rhs;
		}

		friend Color32 operator*(float lhs, const Color32 &rhs)
		{
			return rhs*lhs;
		}
		friend Color32 operator/(float lhs, const Color32 &rhs)
		{
			return rhs/lhs;
		}

#ifdef D3DCOLOR_DEFINED
		Color32(const D3DCOLOR &src) : i32(src) {}
		operator D3DCOLOR() const { return i32; }
#endif

		void set(uchar r, uchar g, uchar b, uchar a = 255)
		{
			this->r = r; this->g = g; this->b = b; this->a = a;
		}
		
		void set(const char *ccode)
		{
			i32 = 0;
			if(!ccode) return;
			if(ccode[0]=='#') {
				if(strlen(ccode)>=7) {
					// sRGB表記と解釈
					char n[9] = "0x000000";
					memcpy(&n[2], &ccode[1], 6);
					i32 = strtol(n, 0, 0);
				}
			}
			else {
				if(strcmp(ccode, "Black")==0 || strcmp(ccode, "black")==0) {
					// Black = "#000000"
				}
				else if(strcmp(ccode, "Green")==0 || strcmp(ccode, "green")==0) {
					// Green = "#008000"
					i32 = 0x008000;
				}
				else if(strcmp(ccode, "Silver")==0 || strcmp(ccode, "silver")==0) {
					// Silver = "#C0C0C0"
					i32 = 0xC0C0C0;
				}
				else if(strcmp(ccode, "Lime")==0 || strcmp(ccode, "lime")==0) {
					// Lime = "#00FF00"
					i32 = 0x00FF00;
				}
				else if(strcmp(ccode, "Gray")==0 || strcmp(ccode, "gray")==0) {
					// Gray = "#808080"
					i32 = 0x808080;
				}
				else if(strcmp(ccode, "Olive")==0 || strcmp(ccode, "olive")==0) {
					// Olive = "#808000"
					i32 = 0x808000;
				}
				else if(strcmp(ccode, "White")==0 || strcmp(ccode, "white")==0) {
					// White = "#FFFFFF"
					i32 = 0xFFFFFF;
				}
				else if(strcmp(ccode, "Yellow")==0 || strcmp(ccode, "yellow")==0) {
					// Yellow = "#FFFF00"
					i32 = 0xFFFF00;
				}
				else if(strcmp(ccode, "Maroon")==0 || strcmp(ccode, "maroon")==0) {
					// Maroon = "#800000"
					i32 = 0x800000;
				}
				else if(strcmp(ccode, "Navy")==0 || strcmp(ccode, "navy")==0) {
					// Navy = "#000080"
					i32 = 0x000080;
				}
				else if(strcmp(ccode, "Red")==0 || strcmp(ccode, "red")==0) {
					// Red = "#FF0000"
					i32 = 0xFF0000;
				}
				else if(strcmp(ccode, "Blue")==0 || strcmp(ccode, "blue")==0) {
					// Blue = "#0000FF"
					i32 = 0x0000FF;
				}
				else if(strcmp(ccode, "Purple")==0 || strcmp(ccode, "purple")==0) {
					// Purple = "#800080"
					i32 = 0x800080;
				}
				else if(strcmp(ccode, "Teal")==0 || strcmp(ccode, "teal")==0) {
					// Teal = "#008080"
					i32 = 0x008080;
				}
				else if(strcmp(ccode, "Fuchsia")==0 || strcmp(ccode, "fuchsia")==0) {
					// Fuchsia = "#FF00FF"
					i32 = 0xFF00FF;
				}
				else if(strcmp(ccode, "Aqua")==0 || strcmp(ccode, "aqua")==0) {
					// Aqua = "#00FFFF"
					i32 = 0x00FFFF;
				}
			}
			a = 255;
		}

		Color32 &setLerp(const Color32 &lhs, const Color32 &rhs, float t)
		{
			*this = Color32(Color().setLerp(Color(lhs), Color(rhs), t));
			return *this;
		}
		Color32 &setLerp(const Color32 &trg, float t)
		{
			setLerp(*this, trg, t);
			return *this;
		}
	};

	inline Color::Color(const Color32 &src)
		: r(src.r/255.0f)
		, g(src.g/255.0f)
		, b(src.b/255.0f)
		, a(src.a/255.0f)
	{
	}

#ifdef D3DCOLOR_DEFINED
	inline Color::operator D3DCOLOR() const { return Color32(*this); }
#endif

	inline void Color::set(const char *ccode)
	{
		Color32 c(ccode);
		r = c.r/255.0f;
		g = c.g/255.0f;
		b = c.b/255.0f;
		a = c.a/255.0f;
	}

} // namespace gctp

#endif //_GCTP_COLOR_HPP_