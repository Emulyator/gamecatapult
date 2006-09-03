#ifndef _GCTP_OBJECT_HPP_
#define _GCTP_OBJECT_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �I�u�W�F�N�g�X�[�p�[�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/09 23:00:42
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/typeinfo.hpp>
#include <gctp/def.hpp>
#include <memory>

namespace gctp {

	class Ptr;
	class Hndl;
	class Serializer;
	namespace detail {
		class Stub;
	}

	/** GameCatapult �폜�A���S���Y���w����
	 *
	 * gctp::Pointer���A���̃I�u�W�F�N�g���폜������@��m�邽�߂ɕK�v�ȏ���񋟂���
	 *
	 * �ʏ�A�������delete�ł���B
	 */
	class AbstractDeleter {
	public:
		/** �����I�Ƀf�X�g���N�^���Ă�
		 *
		 * Deleter��`�̂��߂̃��[�e�B���e�B�[
		 */
		static void destroy(class Object *);
		/** �̈�J������
		 *
		 * ������Ē�`���A�J�X�^���̃������m�ۂɑΉ�����
		 * @code
	// �T�^�I�Ȓ�`
	virtual void operator()(class Object *p) const
	{
		destroy(p); // �f�X�g���N�^���Ăяo��
		alloca().deallocate(p); // �J������
	}
         * @endcode
		 */
		virtual void operator()(class Object *) const = 0;
		/// �k���f���[�^��`�p�̐錾
		static void nulldelete(class Object *) {}
	};

	/// �f���[�^�[��`���ȗ�������e���v���[�g
	template<void (_Func)(class Object *)>
	class Deleter : public AbstractDeleter {
	public:
		virtual void operator()(class Object *p) const
		{
			_Func(p);
		}
		static AbstractDeleter *get()
		{
			static Deleter me;
			return &me;
		}
	private:
		Deleter() {}
	};
	
	/// �������Ȃ��f���[�^
	typedef Deleter<AbstractDeleter::nulldelete> NullDeleter;

	/// STL�����̃A���P�[�^����Deleter���`
	template<class Allocator>
	class DeleterAdaptor : public AbstractDeleter {
	public:
		virtual void operator()(class Object *p) const
		{
			destroy(p);
			Allocator().deallocate(p);
		}
		static AbstractDeleter *get()
		{
			static DeleterAdaptor me;
			return &me;
		}
	private:
		DeleterAdaptor() {}
	};

	/** GameCatapult �I�u�W�F�N�g�X�[�p�[�N���X
	 *
	 * �Q�ƃJ�E���^�E�V���A���C�Y�E�v���p�e�B�[��񋟂���B
	 *
	 * gctp::Pointer / gctp::Handle �Ŏw���I�u�W�F�N�g�͂��ׂĂ��̃N���X�̔h���ł���K�v������
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/09 23:02:30
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Object {
	public:
		/// �Q�ƃJ�E���^�̒l
		uint count() const { return refcount_; }
		/// ���L����Ă��Ȃ����H(true�Ȃ�Q�ƃJ�E���^���P)
		bool unique() const { return (refcount_==1)? true : false; }

		/** Pointer�m�ۂ�}�~
		 *
		 * �X�^�b�N��Ɋm�ۂ��ꂽ��A���̃I�u�W�F�N�g�̃����o�Ƃ��Ċm�ۂ����Object�h���I�u�W�F�N�g
		 * �ɑ΂��Ă�����Ăяo���ƁAHandle�͊m�ۂł��Ă�Pointer�͊m�ۂł��Ȃ��悤�ɂł���B
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/12/03 14:49:11
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		void deleteGuard() { refcount_ = -1; }

		/** �f���[�^�[��ݒ�
		 *
		 * �ʏ��delete���Z�q�ō폜�����̂��D�܂����Ȃ��ꍇ�Agctp::Deleter�̔h�����`����
		 * �����^����B
		 */
		void setDeleter(AbstractDeleter *deleter)
		{
			deleter_ = deleter;
		}

		/** �f���[�^�[���擾
		 *
		 */
		AbstractDeleter *getDeleter() const
		{
			return deleter_;
		}

		/// gctp::Object�p��delete����
		static void destroy(Object *obj)
		{
			if(obj->deleter_) (*obj->deleter_)(obj);
			else delete obj;
		}
		
#ifndef GCTP_USE_STD_RTTI
		/// gctp::Object��gctp::TypeInfo
		static const gctp::TypeInfo &typeinfo() { static gctp::TypeInfo typeinfo("Object", NULL); return typeinfo; }
		/// ���̃I�u�W�F�N�g�̍ł��h�����i��gctp::TypeInfo��Ԃ�
		virtual const gctp::TypeInfo &typeOfThis() const { return typeinfo(); }
#endif

		/// Object�̊e�����o�̓C���X�^���X���ɓƗ��Ȃ̂ŃR�s�[�����Ȃ�
		Object & operator=(const Object &)
		{
			return *this;
		}

		/// �V���A���C�Y
		virtual void serialize(Serializer &) {}

# ifdef GCTP_USE_STD_RTTI
		/// �v���p�e�B�[
		class Property property(const char *property_name);
		/// �v���p�e�B�[���\�b�h�Ăяo��
		bool method(const char *method_name);
# endif
# ifdef GCTP_USE_DYNAMIC_PROPERTY
		/// ���s���v���p�e�B�[�擾
		class DynamicPropertyMap *dynamicProperty() { return dynamic_property_; }
		/// ���s���v���p�e�B�[�ݒ�
		void setDynamicProperty(class DynamicPropertyMap *dpmap);
# endif
	protected:
		/// �f�t�H���g�R���X�g���N�^
		Object() : refcount_(0), stub_(0), deleter_(0)
# ifdef GCTP_USE_DYNAMIC_PROPERTY
			, dynamic_property_(0)
# endif
		{}
		/// Object�̊e�����o�̓C���X�^���X���ɓƗ��Ȃ̂ŃR�s�[�����Ȃ�
		Object(const Object &) : refcount_(0), stub_(0), deleter_(0)
# ifdef GCTP_USE_DYNAMIC_PROPERTY
			, dynamic_property_(0)
# endif
		{}
		virtual ~Object();

		/** �W���̃������m�ۊ֐�
		 *
		 * Factory��`�̃}�N�����ŁAcreate�֐���`�̍ۂɎQ�Ƃ����B
		 *
		 * ���[�U�[��gctp::Object�̔h���N���X�ɂ����Ēu��������ɂ́A
		 * �h���N���X���œ����V�O�l�`�������֐���錾���邾���ł悢�B
		 */
		static void *allocate(std::size_t n) { return ::operator new(n); }

		/** �W����Deleter
		 *
		 * Factory��`�̃}�N�����ŁAcreate�֐���`�̍ۂɎQ�Ƃ����B
		 *
		 * ���[�U�[��gctp::Object�̔h���N���X�ɂ����Ēu��������ɂ́A
		 * �h���N���X���œ����V�O�l�`�������֐���錾���邾���ł悢�B
		 */
		static AbstractDeleter *deleter() { return 0; }

	private:
		friend class Ptr;
		friend class Hndl;
		friend class AbstractDeleter;
		mutable uint refcount_;
		mutable detail::Stub *stub_;
		AbstractDeleter *deleter_;
# ifdef GCTP_USE_DYNAMIC_PROPERTY
		class DynamicPropertyMap *dynamic_property_;
# endif
	};

} // namespace gctp

#endif //_GCTP_OBJECT_HPP_