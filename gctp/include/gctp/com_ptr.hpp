#ifndef _GCTP_COM_PTR_HPP_
#define _GCTP_COM_PTR_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/**@file
 * �V���v����COM�X�}�[�g�|�C���^
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/21 15:53:39
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */

#include <objbase.h>
#include <olectl.h>
#include <assert.h>

namespace gctp {

	/** CComQIPtr�̕��Ŏg���Ă�AddRef��Release���o���Ȃ��ł�Ԃ����߂̂���
	 *
	 * ����ȕ��@�������ł���
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/22 6:29:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template <class T>
	class com_ptr_safe_extructer : public T {
		STDMETHOD_(ULONG, AddRef)()=0;
		STDMETHOD_(ULONG, Release)()=0;
	};

	/** COM�X�}�[�g�|�C���^�N���X�e���v���[�g
	 *
	 * pcomA = pcomB; �� B����A��QueryInterface���������������B
	 *
	 * ATL�̂�� pcomA = 0; ���ł��� pcomA = IAPtr();�Ə��������Ȃ��A�߂�ǂ��B
	 *
	 * �|�C���g����鑤���Q�ƃJ�E���^�������Ă���ꍇ�A������֎~���闝�R�͖����킯�ł����B
	 *
	 * _com_ptr_t��if(pcomA && pcomB) { ... ���ł����Aif((bool)pcomA && (bool)pcomB) { 
	 * �Ƃł����������Ȃ��A�߂�ǂ��B
	 *
	 * ���̂��߉��������B
	 *
	 * ����CComQIPtr���ᐫ�\�ŁA����C���^�[�t�F�[�X���m�̑���ł��A���|�C���^���炾��
	 * QI�������Ă��܂��B�i���������邽�߂�CComQIPtr�ł�operator=(T*)�������āA���̂���
	 * 0������o���Ȃ���ł��ˁj
	 * 
	 * DirectX���������ɂ́A����ŏ\�����Ǝv���B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 15:54:44
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template <class T, const IID* piid = &__uuidof(T)>
	class com_ptr {
	public:
		typedef T _PtrClass;
		com_ptr() : p(0) {}
		com_ptr(const com_ptr<T,piid> &lp) : p(0)
		{
			if((p = lp.p) != NULL) p->AddRef();
		}
		com_ptr(IUnknown* lp) : p(0)
		{
			if(lp != NULL) lp->QueryInterface(*piid, (void **)&p);
		}
		~com_ptr()
		{
			if(p) p->Release();
		}
		void release()
		{
			IUnknown* pTemp = p;
			if (pTemp)
			{
				p = NULL;
				pTemp->Release();
			}
		}
		operator T*() const
		{
			return p;
		}
		T& operator*() const
		{
			assert(p!=NULL); return *p;
		}
		com_ptr_safe_extructer<T>* operator->() const
		{
			assert(p!=NULL);
			return (com_ptr_safe_extructer<T>*)p;
		}
		//The assert on operator& usually indicates a bug.  If this is really
		//what is needed, however, take the address of the p member explicitly.
		T **operator&()
		{
			assert(p==NULL);
			return &p;
		}

		T *operator=(const com_ptr<T,piid>& lp)
		{
			if(lp.p) lp.p->AddRef();
			if(p) p->Release();
			p = lp.p;
			return p;
		}
		T *operator=(IUnknown *lp)
		{
			IUnknown *temp = p;
			p = 0;
			if(lp) lp->QueryInterface(*piid, (void**)&p);
			if(temp) temp->Release();
			return p;
		}
		bool operator!() const
		{
			return (p == NULL);
		}
		bool operator<(T* pT) const
		{
			return p < pT;
		}
		bool operator==(T* pT) const
		{
			return p == pT;
		}
		// Compare two objects for equivalence
		bool isEqual(IUnknown* pOther)
		{
			if (p == NULL && pOther == NULL)
				return true; // They are both NULL objects

			if (p == NULL || pOther == NULL)
				return false; // One is NULL the other is not

			com_ptr<IUnknown> punk1;
			com_ptr<IUnknown> punk2;
			p->QueryInterface(IID_IUnknown, (void**)&punk1);
			pOther->QueryInterface(IID_IUnknown, (void**)&punk2);
			return punk1 == punk2;
		}
		void attach(T* p2)
		{
			if(p) p->Release();
			p = p2;
		}
		T* detach()
		{
			T* pt = p;
			p = NULL;
			return pt;
		}
		HRESULT copyTo(T** ppT)
		{
			assert(ppT != NULL);
			if (ppT == NULL) return E_POINTER;
			*ppT = p;
			if(p) p->AddRef();
			return S_OK;
		}
		HRESULT setSite(IUnknown* punk_parent)
		{
			if(!p) return E_POINTER;
			HRESULT ret;
			IObjectWithSite* child_site = 0;
			ret = p->QueryInterface(IID_IObjectWithSite, (void**)&child_site);
			if(SUCCEEDED(hr) && pChildSite) {
				ret = child_site->SetSite(punk_parent);
				child_site->Release();
			}
			return ret;
		}
		HRESULT advise(IUnknown *unknown, const IID& iid, LPDWORD pdw)
		{
			if(!p) return E_INVALIDARG;
			com_ptr<IConnectionPointContainer> cpc;
			com_ptr<IConnectionPoint> cp;
			HRESULT ret = p->QueryInterface(IID_IConnectionPointContainer, (void**)&cpc.p);
			if(SUCCEEDED(ret)) ret = cpc->FindConnectionPoint(iid, &cp.p);
			if(SUCCEEDED(ret)) ret = cp->Advise(unknown, pdw);
			return ret;
		}
		HRESULT CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL)
		{
			assert(p == NULL);
			return ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)&p);
		}
		HRESULT CoCreateInstance(LPCOLESTR szProgID, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL)
		{
			CLSID clsid;
			HRESULT hr = CLSIDFromProgID(szProgID, &clsid);
			assert(p == NULL);
			if (SUCCEEDED(hr))
				hr = ::CoCreateInstance(clsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)&p);
			return hr;
		}
		template <class Q>
		HRESULT QueryInterface(Q** pp)
		{
			assert(pp != NULL && *pp == NULL);
			return p->QueryInterface(__uuidof(Q), (void**)pp);
		}
		T* p;
	};

	//Specialization to make it work
	template<>
	class com_ptr<IUnknown, &IID_IUnknown>
	{
	public:
		typedef IUnknown _PtrClass;
		com_ptr()
		{
			p=NULL;
		}
		com_ptr(IUnknown* lp)
		{
			//Actually do a QI to get identity
			p=NULL;
			if (lp != NULL)
				lp->QueryInterface(IID_IUnknown, (void **)&p);
		}
		com_ptr(const com_ptr<IUnknown,&IID_IUnknown>& lp)
		{
			if ((p = lp.p) != NULL)
				p->AddRef();
		}
		~com_ptr()
		{
			if (p)
				p->Release();
		}
		void release()
		{
			IUnknown* pTemp = p;
			if (pTemp)
			{
				p = NULL;
				pTemp->Release();
			}
		}
		operator IUnknown*() const
		{
			return p;
		}
		IUnknown& operator*() const
		{
			assert(p!=NULL);
			return *p;
		}
		
		//The assert on operator& usually indicates a bug.  If this is really
		//what is needed, however, take the address of the p member explicitly.
		IUnknown** operator&()
		{
			assert(p==NULL);
			return &p;
		}

		com_ptr_safe_extructer<IUnknown>* operator->() const
		{
			assert(p!=NULL);
			return (com_ptr_safe_extructer<IUnknown>*)p;
		}

		IUnknown *operator=(const com_ptr<IUnknown,&IID_IUnknown>& lp)
		{
			if(lp.p) lp.p->AddRef();
			if(p) p->Release();
			p = lp.p;
			return p;
		}
		IUnknown *operator=(IUnknown *lp)
		{
			//Actually do a QI to get identity
			IUnknown *temp = p;
			p = 0;
			if(lp) lp->QueryInterface(IID_IUnknown, (void**)&p);
			if(temp) temp->Release();
			return p;
		}
		bool operator!() const
		{
			return (p == NULL);
		}
		bool operator<(IUnknown* pT) const
		{
			return p < pT;
		}
		bool operator==(IUnknown* pT) const
		{
			return p == pT;
		}
		// Compare two objects for equivalence
		bool isEqual(IUnknown* pOther)
		{
			if (p == NULL && pOther == NULL)
				return true; // They are both NULL objects

			if (p == NULL || pOther == NULL)
				return false; // One is NULL the other is not

			com_ptr<IUnknown> punk1;
			com_ptr<IUnknown> punk2;
			p->QueryInterface(IID_IUnknown, (void**)&punk1);
			pOther->QueryInterface(IID_IUnknown, (void**)&punk2);
			return punk1 == punk2;
		}
		IUnknown* detach()
		{
			IUnknown* pt = p;
			p = NULL;
			return pt;
		}
		HRESULT copyTo(IUnknown** ppT)
		{
			assert(ppT != NULL);
			if (ppT == NULL)
				return E_POINTER;
			*ppT = p;
			if (p)
				p->AddRef();
			return S_OK;
		}
		HRESULT setSite(IUnknown *punk_parent)
		{
			if(!p) return E_POINTER;
			HRESULT ret;
			IObjectWithSite* child_site = 0;
			ret = p->QueryInterface(IID_IObjectWithSite, (void**)&child_site);
			if(SUCCEEDED(ret) && child_site) {
				ret = child_site->SetSite(punk_parent);
				child_site->Release();
			}
			return ret;
		}
		HRESULT advise(IUnknown *unknown, const IID& iid, LPDWORD pdw)
		{
			if(!p) return E_INVALIDARG;
			com_ptr<IConnectionPointContainer> cpc;
			com_ptr<IConnectionPoint> cp;
			HRESULT ret = p->QueryInterface(IID_IConnectionPointContainer, (void**)&cpc.p);
			if(SUCCEEDED(ret)) ret = cpc->FindConnectionPoint(iid, &cp.p);
			if(SUCCEEDED(ret)) ret = cp->Advise(unknown, pdw);
			return ret;
		}
		HRESULT CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL)
		{
			assert(p == NULL);
			return ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, __uuidof(IUnknown), (void**)&p);
		}
		HRESULT CoCreateInstance(LPCOLESTR szProgID, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL)
		{
			CLSID clsid;
			HRESULT hr = CLSIDFromProgID(szProgID, &clsid);
			assert(p == NULL);
			if (SUCCEEDED(hr))
				hr = ::CoCreateInstance(clsid, pUnkOuter, dwClsContext, __uuidof(IUnknown), (void**)&p);
			return hr;
		}
		template <class Q>
		HRESULT QueryInterface(Q** pp)
		{
			assert(pp != NULL && *pp == NULL);
			return p->QueryInterface(__uuidof(Q), (void**)pp);
		}
		IUnknown* p;
	};

} // namespace gctp

/** COM�|�C���^�錾�w���p�[�}�N��
 *
 * <CODE>TYPEDEF_DXCOMPTR(IDirect3D8, IDirect3DPtr);</CODE>
 *
 * �� IDirect3DPtr ��typedef�����B
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/21 16:06:04
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#define TYPEDEF_DXCOMPTREX(_I, _Name)	typedef gctp::com_ptr<_I, &IID_##_I> _Name;

/** COM�|�C���^�錾�w���p�[�}�N��
 *
 * <CODE>TYPEDEF_DXCOMPTR(IDirect3D8);</CODE>
 *
 * �� IDirect3D8Ptr ��typedef�����B
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/21 16:06:04
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#define TYPEDEF_DXCOMPTR(_I)	TYPEDEF_DXCOMPTREX(_I, _I##Ptr)

#endif //_GCTP_COM_PTR_HPP_