/** @file argmap.cpp
 * ����������̉�͂��āA�I�v�V����=>�l�̃}�b�v�����
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/09 20:44:40
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/argmap.hpp>
#include <gctp/cmdline.hpp>
#include <string.h>
#include <iostream>
#include <vector>

using namespace std;

namespace gctp {

	bool ArgMap::hasKey(const string &key) const
	{
		return end() != find(key);
	}

	bool ArgMap::doseKeyHasVal(const string &key) const
	{
		for(HasValKeyList::const_iterator i = have_val_key_.begin(); i != have_val_key_.end(); i++) {
			if((*i) == key) return true;
		}
		return false;
	}

	bool ArgMap::hasAlias(const char opt) const
	{
		return alias_.end() != alias_.find(opt);
	}

	void ArgMap::add(const string &key, const string &val)
	{
		(*this)[key].push_back(val);
	}

	void ArgMap::addHasValKey(const string &key)
	{
		have_val_key_.push_back(key);
	}

	void ArgMap::defAlias(const char opt, const string &key)
	{
		alias_[opt]=key;
	}

	bool ArgMap::checkOpt(const char *arg, string &cur_key)
	{
		bool is_already_have_has_val_key = false;
		while(*arg) {
			char temp[] = {*arg, '\0'};
			if(hasAlias(*arg) && doseKeyHasVal(alias_[*arg])) {
				if(is_already_have_has_val_key) return false; // ��ɒl���K�v�Ȃ��̂��Q�ȏ��x�Ɏw�肳�ꂽ
				is_already_have_has_val_key = true;
				cur_key = alias_[*arg];
			}
			else if(doseKeyHasVal(string(temp))) {
				if(is_already_have_has_val_key) return false; // ��ɒl���K�v�Ȃ��̂��Q�ȏ��x�Ɏw�肳�ꂽ
				is_already_have_has_val_key = true;
				cur_key = string(temp);
			}
			else if(hasAlias(*arg)) {
				(*this)[alias_[*arg]];
				cur_key = "";
			}
			else {
				(*this)[string(temp)];
				cur_key = "";
			}
			arg++;
		}
		return true;
	}

	void ArgMap::clear(void)
	{
		have_val_key_.clear();
		alias_.clear();
		Super::clear();
	}

	/** ���������
	 *
	 * �I�v�V����=>�l�̃}�b�v�ɍ\�z
	 *
	 * �I�v�V�����ɑ΂�����͂ł͂Ȃ������́A�k���X�g�����O�L�[�ɓ���BArgMap[string("")]
	 *
	 * ���̒��Ńv���O�����{�̂̃p�X��ǂݔ�΂��̂ŁAmain�֐��̈��������̂܂ܓn���Ă��������B
	 *
	 * @return �p�[�Y�G���[�̏ꍇfalse
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:48:01
	 * $Id: argmap.cpp,v 1.3 2005/02/21 02:21:12 takasugi Exp $
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	bool ArgMap::parse(int argc, char *argv[])
	{
		argc--; argv++;
		string cur_key;
		for(int i = 0; i < argc; i++) {
			if(argv[i][0] == '-') {
				if(argv[i][1] == '-') {
					if(argv[i][2] == '-') {
						add(string(""), string(argv[i]));
					}
					else {
						cur_key = argv[i]+2;
						if(!doseKeyHasVal(cur_key)) {
							(*this)[cur_key];
							cur_key = "";
						}
					}
				}
				else {
					if(!checkOpt(argv[i]+1, cur_key)) return false;
				}
			}
			else {
				if(cur_key.length()) {
					add(cur_key, string(argv[i]));
					cur_key = "";
				}
				else {
					add(string(""), string(argv[i]));
				}
			}
		}
		return cur_key.length()==0;
	}

	/** ���������
	 *
	 * �I�v�V����=>�l�̃}�b�v�ɍ\�z
	 *
	 * �z��łȂ��A�P��̕����񂩂琶������BWinMain��CmdLine�����p�B
	 *
	 * @return �p�[�Y�G���[�̏ꍇfalse
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:48:01
	 * $Id: argmap.cpp,v 1.3 2005/02/21 02:21:12 takasugi Exp $
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	bool ArgMap::parse(const char *cmdline)
	{
		CmdLine cmdl(cmdline);
		return parse(cmdl.argc(), cmdl.argv());
	}

	/** ���������
	 *
	 * �I�v�V����=>�l�̃}�b�v�ɍ\�z
	 *
	 * �t�@�C�����e���R�}���h�����Ƃ��ĉ��߁B
	 *
	 * @return �p�[�Y�G���[�̏ꍇfalse
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:48:01
	 * $Id: argmap.cpp,v 1.3 2005/02/21 02:21:12 takasugi Exp $
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	bool ArgMap::parse(istream &in)
	{
		CmdLine cmdl(in);
		return parse(cmdl.argc(), cmdl.argv());
	}

} // namespace gctp
