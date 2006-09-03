#ifndef _GCTP_SERIALIZER_HPP_
#define _GCTP_SERIALIZER_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �V���A���C�U�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/09 23:00:42
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/pointervector.hpp>
#include <gctp/iobstream.hpp>

namespace gctp {

	/** GameCatapult�V���A���C�U�N���X
	 *
	 * �V���A���C�Y�Ɋւ����́APtr�̈������c���̂݁B�i�ꉞ�����B�����A�܂���肠��j
	 *
	 * �V���A���C�Y�̉ߒ��Ő��������I�u�W�F�N�g������Serializer����[�ێ����Ă��܂��B\n
	 * �Ȃ���Ń��[�h���I�������Serializer����̂����悤�ɃR�[�f�B���O���ׂ��B
	 *
	 * @code
	 * // �t�@�C��"serialize"�ɃV���A���C�Y�����
	 * obfstream stream("serialize");
	 * Serializer serializer(stream);
	 * serializer << foo << bar;
	 * @endcode
	 *
	 * Pointer���V���A���C�Y����ɂ́A����Ŏw�����I�u�W�F�N�g���A�N���X�o�^�A�t�@�N�g���[�o�^
	 * ����Ă���K�v������B\n
	 * �����V���A���C�Y������I�u�W�F�N�g�́AGCTP_DECLARE_CLASS GCTP_IMPLEMENT_CLASS �������K�v������B
	 * @see Class
	 * @see Factory
	 * @see GCTP_DECLARE_SERIALIZE
	 * @see GCTP_IMPREMENT_SERIALIZE_BEGIN
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/09 23:01:14
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Serializer {
	public:
		/// �ǂݍ��ݗp�ɏ�����
		Serializer(ibstream &stream);
		/// �������ݗp�ɏ�����
		Serializer(obstream &stream);
		/// �I�[�v������Ă��邩�H
		bool isOpen() { return istream_ || ostream_; }
		/// �ǂݍ��݃��[�h���H
		bool isLoading() { return istream_ ? (!istream_->eof()) : false; }
		/// �������݃��[�h���H
		bool isWriting() { return ostream_ ? true : false; }
		/// �ǂݍ��݃X�g���[����Ԃ�
		ibstream &istream() { return *istream_; }
		/// �������݃X�g���[����Ԃ�
		obstream &ostream() { return *ostream_; }
		/// �ǂݍ��ݎ��ɐ��삳�ꂽ�I�u�W�F�N�g��ێ�����x�N�^
		PtrVector ptrs;
		/// �w��|�C���^�[�Ɠ����l������ptrs���̃C���f�b�N�X+1�̒l��Ԃ�
		std::size_t findPtr(Ptr ptr);

		class LoadFailExeption {};
		void checkVersion(int ver);
	private:
		ibstream *istream_;
		obstream *ostream_;
		enum{ HEAD = 'PTCG', MAGIC = 'IRES', _FORCE_DWORD_ = 0x7FFFFFFF };
	};

	/** �V���A���C�Y���Z�q�e���v���[�g
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/09 23:02:24
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<typename _T>
	Serializer &operator<<(Serializer& lhs, _T &rhs)
	{
		if(lhs.isLoading()) lhs.istream() >> rhs;
		else if(lhs.isWriting()) lhs.ostream() << rhs;
		return lhs;
	}

	/** Object�p�ɓ��ꉻ�����V���A���C�Y���Z�q
	 *
	 * Object�h���́AObject�ɃL���X�g���邱�ƁB
	 */
	template<>
	Serializer &operator<<(Serializer &lhs, Object &rhs);

	/** Ptr�p�ɓ��ꉻ�����V���A���C�Y���Z�q
	 *
	 * ���L�I�u�W�F�N�g�𐳂����V���A���C�Y���邽�߁B
	 *
	 * Pointer�̊e���ꉻ�o�[�W�����́APtr�ɃL���X�g���邱�ƁB
	 */
	template<>
	Serializer &operator<<(Serializer &lhs, Ptr &rhs);

	/** Hndl�p�ɓ��ꉻ�����V���A���C�Y���Z�q
	 *
	 * ���L�I�u�W�F�N�g�𐳂����V���A���C�Y���邽�߁B
	 *
	 * Handle�̊e���ꉻ�o�[�W�����́AHndl�ɃL���X�g���邱�ƁB
	 */
	template<>
	Serializer &operator<<(Serializer &lhs, Hndl &rhs);
	
	/** Pointer�p�V���A���C�Y���Z�q
	 *
	 * ������Ƌ���̍�
	 */
	template<typename PointeeType> Serializer &operator>>(Serializer &lhs, Pointer<PointeeType> &rhs) {
		return lhs << (*reinterpret_cast<Ptr *>(&rhs));
	}

	/** Handle�p�V���A���C�Y���Z�q
	 *
	 * ������Ƌ���̍�
	 */
	template<typename HandledType> Serializer &operator>>(Serializer &lhs, Handle<HandledType> &rhs) {
		return lhs << (*reinterpret_cast<Hndl *>(&rhs));
	}

} // namespace gctp

/** �V���A���C�Y�@�\��ǉ����邽�߂̃}�N��
 *
 * @code
 *  class Foo : Object {
 *     Foo();
 *     int menber1;
 *     int menber2;
 *     Pointer<Bar> ptr_member;
 *  
 *  GCTP_DECLARE_SERIALIZE	//<-�������N���X��`���ɏ���
 *  };
 *
 * in impleremt file.
 *
 * //������cpp�t�@�C�����ɏ���
 * GCTP_IMPLEMENT_SERIALIZE(Foo, Object)
 *    GCTP_SERIALIZE(<< menber2 << menber2 >> ptr_menber)
 * GCTP_IMPLEMENT_SERIALIZE_END();
 * @endcode
 *
 * �A�N�Z�X�w��q���܂܂�Ă��邱�Ƃɒ��ӁB���̃}�N���̒����protected�ɂȂ�B
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2003/01/05 13:30:37
 * $Id: store.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#define GCTP_DECLARE_SERIALIZE						\
	protected:										\
		virtual void serialize(gctp::Serializer &);

/** ��L�錾�}�N���ɑΉ���������}�N��
 *
 * @code
 *  class Foo : public Object {
 *     Foo();
 *  GCTP_DECLARE_CLASS
 *  GCTP_DECLARE_SERIALIZE	//<-�������N���X��`���ɏ���
 *     int menber1;
 *     int menber2;
 *     Pointer<Bar> ptr_member;
 *  };
 *
 * in impleremt file.
 *
 * //������cpp�t�@�C�����ɏ���
 * GCTP_IMPLEMENT_CLASS(Foo, Object)
 * GCTP_IMPLEMENT_SERIALIZE_BEGIN(0, Foo, Object)
 *    GCTP_SERIALIZE(<< menber2 << menber2 >> ptr_menber)	// �V���A���C�Y���郁���o��<<�Ōq���ď���
 * GCTP_IMPLEMENT_SERIALIZE_END();
 * @endcode
 *
 * �K���X�[�p�[�N���X���w�肷��
 *
 * Pointer<>���V���A���C�Y�������ꍇ�́AGCTP_SERIALIZEPTR�}�N����>>���Z�q���g���B
 * �������A�����Hndl.lock�Ŋm�ۂ���Ptr�ɑ΂��čs��Ȃ����ƁBHndl�Ƃ��ēo�^����Ă���I�u�W�F�N�g��
 * �V���A���C�Y����ƁA���[�h����Hndl�o�^���ꂽ�I�u�W�F�N�g�Ƃ͕ʂɃC���X�^���X������Ă��܂��B
 * Hndl.lock�͂����Z�����ԁA�����̃A�g�~�b�N�����ێ����邽�߂ɗp�ӂ��ꂽ���̂ŁA����Ŋm�ۂ����I�u�W�F�N�g��
 * �V���A���C�Y���悤�Ƃ����~���́A�Ԉ�������̂ł���B
 *
 * @param _ver �o�[�W�����ԍ� �����o�\�����ς������A����ȑO�̃V���A���C�Y�f�[�^�������ɂȂ�悤�ɁA�ʂ̒l���w�肷��
 * @param _classname �N���X��
 * @param _superclassname �p�������N���X��
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2003/01/05 13:30:37
 *
 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#define GCTP_IMPREMENT_SERIALIZE_BEGIN(_ver, _classname, _superclassname)	\
	void _classname::serialize(gctp::Serializer &_serializer_) {			\
		_superclassname::serialize(_serializer_);							\
		_serializer_.checkVersion(_ver);
#define GCTP_SERIALIZE(_members)	_serializer_ _members;
#define GCTP_SERIALIZEPTR(_member)	(*reinterpret_cast<Ptr *>(&_member))
#define GCTP_SERIALIZEHNDL(_member)	(*reinterpret_cast<Hndl *>(&_member))

#define GCTP_IMPREMENT_SERIALIZE_END	\
	}

#endif //_GCTP_SERIALIZER_HPP_