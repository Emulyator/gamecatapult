/*
 * WEIGHTSETUP.CPP -- LightWave Plugins CommandSequenceClass
 *				 weightsetup source code for CommandSequenceClass.
 *
 *
 * ���̃\�[�X�R�[�h�́ACommandSequenceClass�̃v���O�C�����쐬���邽�߂�
 * ���`��񋟂��Ă��܂��Bweightsetup�̐ړ��q�ŋL�q����Ă���֐���\����
 * �̖��͕̂K�v�ɉ����Ė��̕ύX���Ďg�p���܂��B
 *
 */

#if _MSC_VER > 1000
#pragma once
#pragma warning(disable:4786 4503)
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lwhost.h>
#include <lwserver.h>
#include <lwcmdseq.h>

extern "C" {
#include "lwmodlib.h"
}

#ifdef _MSC_VER
#define for if(0); else for
#endif

#include <gctp/def.hpp>
#include <gctp/dbgout.hpp>
#include <gctp/line.hpp>
#include <map>
#include <vector>
#include <algorithm>
#include <string>

using namespace gctp;

// Skelegon���
// �{�[���E�F�C�g�}�b�v
#define LWPTAG_BNWT LWID_('B','N','W','T')
// Skelegon Editor �ǉ����
//�X�P���S������ӂɔF�����邽�߂�ID
#define LWPTAG_SKID LWID_('S','K','I','D')
// �A�C�e���e�q�֌W
#define LWPTAG_SKPR LWID_('S','K','P','R')
// �X�P���S���̃A�C�e���J���[
#define LWPTAG_SKCL LWID_('S','K','C','L')
// �S�[���I�u�W�F�N�g�̖���
#define LWPTAG_SKGL LWID_('S','K','G','L')

namespace lwpp { namespace modeler {

	class Progress {
		DynaMonitorFuncs *monf_;
		LWMonitor        *mon_;
		bool             done_;
	public:
		Progress(GlobalFunc *global)
			: monf_(reinterpret_cast<DynaMonitorFuncs *>(global( LWDYNAMONITORFUNCS_GLOBAL, GFUSE_TRANSIENT )))
			, mon_(0)
			, done_(false)
		{
		}
		~Progress()
		{
			done();
			if(monf_ && mon_) monf_->destroy(mon_);
		}
		void init(const char *title, const char *caption)
		{
			if(mon_) {
				done();
				if(monf_) monf_->destroy(mon_);
			}
			if(monf_ && !mon_) mon_ = monf_->create(title, caption);
		}
		void init(unsigned int totalstep)
		{
			if(mon_) mon_->init(mon_->data, totalstep);
			done_ = false;
		}
		void init(const char *title, const char *caption, unsigned int totalstep)
		{
			init(title, caption);
			init(totalstep);
		}
		bool step(unsigned int delta)
		{
			int ret = 1;
			if(mon_) ret = mon_->step(mon_->data, delta);
			return ret ? true : false;
		}
		void done()
		{
			if(!done_) {
				if(mon_) mon_->done(mon_->data);
				if(monf_ && mon_) {
					monf_->destroy(mon_);
					mon_ = NULL;
				}
				done_ = true;
			}
		}
	};

}}

namespace {
#if 0
	class BSPTreeFormer {
		static int dumpPoints(void *param, LWPntID p)
		{
			BSPTreeFormer *self = (BSPTreeFormer *)param;
			LWFVector pos;
			prm->mesh->pntBasePos(prm->mesh, p, pos);
			PRNN("base pt pos : "<<pos[0]<<","<<pos[1]<<","<<pos[2]);
			prm->mesh->pntOtherPos(prm->mesh, p, pos);
			PRNN("pt pos : "<<pos[0]<<","<<pos[1]<<","<<pos[2]);
			int wmapnum = prm->objfunc->numVMaps( LWVMAP_WGHT );
			for(int j = 0; j < wmapnum; j++) {
				const char *wmname = prm->objfunc->vmapName(LWVMAP_WGHT, j);
				if(wmname) {
					int dim = prm->objfunc->vmapDim(LWVMAP_WGHT, j);
					if(0 <= dim && dim < 5) {
						void *wmap = prm->mesh->pntVLookup(prm->mesh, LWVMAP_WGHT, wmname);
						float val[4];
						if(prm->mesh->pntVIDGet(prm->mesh, p, val, wmap)) {
							switch(dim) {
							case 0: PRNN("wmap "<<wmname<<" : mapped"); break;
							case 1: PRNN("wmap "<<wmname<<" : "<<val[0]); break;
							case 2: PRNN("wmap "<<wmname<<" : "<<val[0]<<","<<val[1]); break;
							case 3: PRNN("wmap "<<wmname<<" : "<<val[0]<<","<<val[1]<<","<<val[2]); break;
							case 4: PRNN("wmap "<<wmname<<" : "<<val[0]<<","<<val[1]<<","<<val[2]<<","<<val[3]); break;
							}
						}
						else PRNN("wmap "<<wmname<<" is not mapped");
					}
					else PRNN("vmap dimention is too deeper");
				}
			}
			return 0;
		}

		static int dumpPolygons(void *param, LWPolID p)
		{
			BSPTreeFormer *self = (BSPTreeFormer *)param;
			LWID type = prm->mesh->polType(prm->mesh, p);
			switch(type) {
			case LWPOLTYPE_FACE: PRNN("- �t�F�C�X"); break;
			case LWPOLTYPE_CURV: PRNN("- ���K�J�[�u"); break;
			case LWPOLTYPE_PTCH: PRNN("- ��������P�[�W�|���S��"); break;
			case LWPOLTYPE_MBAL: PRNN("- ���^�{�[��"); break;
			case LWPOLTYPE_BONE: PRNN("- �{�[��"); break;
			}
			const char *partname = prm->mesh->polTag( prm->mesh, p, LWPTAG_PART );
			if(partname) PRNN("part : "<<partname);
			int vnum = prm->mesh->polSize( prm->mesh, p );
			for(int i = 0; i < vnum; i++) {
				LWPntID pt = prm->mesh->polVertex(prm->mesh, p, i);
				LWFVector pos;
				prm->mesh->pntBasePos(prm->mesh, pt, pos);
				PRNN("base pt pos : "<<pos[0]<<","<<pos[1]<<","<<pos[2]);
				prm->mesh->pntOtherPos(prm->mesh, pt, pos);
				PRNN("pt pos : "<<pos[0]<<","<<pos[1]<<","<<pos[2]);
				int wmapnum = prm->objfunc->numVMaps( LWVMAP_WGHT );
				for(int j = 0; j < wmapnum; j++) {
					const char *wmname = prm->objfunc->vmapName(LWVMAP_WGHT, j);
					if(wmname) {
						int dim = prm->objfunc->vmapDim(LWVMAP_WGHT, j);
						if(0 <= dim && dim < 5) {
							void *wmap = prm->mesh->pntVLookup(prm->mesh, LWVMAP_WGHT, wmname);
							float val[4];
							if(prm->mesh->pntVPIDGet(prm->mesh, pt, p, val, wmap)) {
								switch(dim) {
								case 0: PRNN("wmap "<<wmname<<" : mapped"); break;
								case 1: PRNN("wmap "<<wmname<<" : "<<val[0]); break;
								case 2: PRNN("wmap "<<wmname<<" : "<<val[0]<<","<<val[1]); break;
								case 3: PRNN("wmap "<<wmname<<" : "<<val[0]<<","<<val[1]<<","<<val[2]); break;
								case 4: PRNN("wmap "<<wmname<<" : "<<val[0]<<","<<val[1]<<","<<val[2]<<","<<val[3]); break;
								}
							}
							else PRNN("wmap "<<wmname<<" is not mapped");
						}
						else PRNN("vmap dimention is too deeper");
					}
				}
			}
			return 0;
		}

	public:
		void proc()
		{
			if(objf_) {
				int num = objf_->numObjects();
				PRNN("obj num : " << num);
				for(int i = 0; i < num; i++) {
					const char *fname = objf_->filename(i);
					if(fname) PRNN("filename : "<<fname);
					else PRNN("filename : none"); // �܂����O�t���ĂȂ��ƁANULL�A���Ă���

					int wmapnum = objf_->numVMaps( LWVMAP_WGHT );
					PRNN("Weight Map num : " << wmapnum);
					for(int j = 0; j < wmapnum; j++) {
						const char *wmname = objf_->vmapName(LWVMAP_WGHT, j);
						if(wmname) PRNN("wmap"<<j<<" : "<<wmname);
						else PRNN("wmap"<<j<<" : ");
						int dim = objf_->vmapDim(LWVMAP_WGHT, j);
						PRNN("wmap"<<j<<" dimention : "<<dim);
					}

					int lnum = objf_->maxLayers(i);
					PRNN("layer num : " << lnum);
					for(int j = 0; j < num; j++) {
						if(objf_->layerExists(i, j) && objf_->layerVis(i, j)) {
							LWFVector pivot;
							objf_->pivotPoint(i, j, pivot);
							PRNN("pivot : "<<pivot[0]<<","<<pivot[1]<<","<<pivot[2]);
							mesh_ = objf_->layerMesh(i, j);
							if(mesh_) {
								int numpt = mesh_->numPoints(mesh_);
								int numpl = mesh_->numPolygons(mesh_);
								PRNN("point num : "<<numpt<<"  polygon num : "<<numpl);
//								ScanParam param = { mesh_, objf_ };
//								mesh_->scanPoints(mesh_, dumpPoints, &param);
//								mesh_->scanPolys(mesh_, dumpPolygons, &param);
								mesh_->scanPolys(mesh_, scanSkelegonCB, this);
								setUpHierarchy();
								compactHierarchy();
							}
						}
						else PRNN("obj"<<i<<"layer"<<j<<" is not visible or not exist");
					}
					for(std::map<std::string, SkelegonInfo>::iterator i = skelegons_.begin(); i != skelegons_.end(); ++i) {
						PRNN(i->first<<":");
						for(std::vector<std::string>::iterator j = i->second.parents.begin(); j != i->second.parents.end(); ++j) {
							PRNN("parent : "<<*j);
						}
						for(std::vector<std::string>::iterator j = i->second.children.begin(); j != i->second.children.end(); ++j) {
							PRNN("child  : "<<*j);
						}
					}
				}
			}
		}
	};
#endif
	// �E�F�C�g�����Z�b�g�A�b�v
	class WeightSetUp {
		static int scanSkelegonCB(void *param, const EDPolygonInfo *p)
		{
			WeightSetUp *self = (WeightSetUp *)param;
			if(p->flags & EDDF_SELECT && !(p->flags & EDDF_DELETE) && p->type == LWPOLTYPE_BONE) {
				const char *partname = meGetPolyTag( p->pol, LWPTAG_PART );
				if(partname) {
					if(p->numPnts == 2) {
						SkelegonAttr attr;
						attr.s = p->points[0];
						attr.e = p->points[1];
						EDPointInfo *pnt = mePointInfo( p->points[0] );
						attr.l.s = VectorC(static_cast<float>(pnt->position[0]), static_cast<float>(pnt->position[1]), static_cast<float>(pnt->position[2]));
						pnt = mePointInfo( p->points[1] );
						attr.l.e = VectorC(static_cast<float>(pnt->position[0]), static_cast<float>(pnt->position[1]), static_cast<float>(pnt->position[2]));
						const char *idname = meGetPolyTag( p->pol, LWPTAG_SKID );
						if(idname) attr.skid = (unsigned)atoi(idname);
						else attr.skid = 0;
						const char *parentname = meGetPolyTag( p->pol, LWPTAG_SKPR );
						if(parentname) attr.parent = (unsigned)atoi(parentname);
						else attr.parent = 0;
						self->skelegons_[partname].attrs.push_back(attr);
						//PRNN(partname << " : "<<attr.s<<","<<attr.e<<","<<attr.l.s<<","<<attr.l.e);
						//PRNN(partname << " : "<<attr.skid<<","<<attr.parent);
					}
					else {
						PRNN("����Ȃ��Ƃ���́H");
					}
				}
			}
			return 0;
		}

		static EDError delPolysVMap(void *param, const EDPolygonInfo *p)
		{
			WeightSetUp *self = (WeightSetUp *)param;
			EDError ret = EDERR_NONE;
			if(p->flags & EDDF_SELECT && !(p->flags & EDDF_DELETE) && p->type != LWPOLTYPE_BONE) {
				for(int i = 0; i < p->numPnts; i++) {
					for(std::map<std::string, SkelegonInfo>::const_iterator j = self->skelegons_.begin(); j != self->skelegons_.end(); ++j) {
						ret = meSetPointVPMap( p->points[i], p->pol, LWVMAP_WGHT, j->first.c_str(), 1, NULL );
						//if(ret != EDERR_NONE) PRNN("meSetPointVPMap error occur : "<<ret);
					}
				}
				if(self->prog_.step(1)) return EDERR_USERABORT;
			}
			return ret;
		}
		
		static EDError setWeightToSkelegon(void *param, const EDPolygonInfo *p)
		{
			WeightSetUp *self = (WeightSetUp *)param;
			EDError ret = EDERR_NONE;
			if(p->flags & EDDF_SELECT && !(p->flags & EDDF_DELETE) && p->type == LWPOLTYPE_BONE) {
				if(p->numPnts == 2) {
					const char *partname = meGetPolyTag( p->pol, LWPTAG_PART );
					if(partname) {
						std::string _partname(partname);
						if(p->numPnts == 2 && !(self->use_calsium_ && (_partname.substr(_partname.length()-2) == "_I" || _partname.substr(_partname.length()-2) == "_F"))) {
							ret = meSetPolyTag( p->pol, LWPTAG_BNWT, partname );
						}
					}
				}
			}
			return ret;
		}

		static EDError editPointsVMap(void *param, const EDPointInfo *p)
		{
			WeightSetUp *self = (WeightSetUp *)param;
			EDError ret = EDERR_NONE;
			if(p->flags & EDDF_SELECT && !(p->flags & EDDF_DELETE)) {
				VectorC pos(static_cast<float>(p->position[0]),static_cast<float>(p->position[1]),static_cast<float>(p->position[2]));
				float dis;
				std::map<std::string, SkelegonInfo>::const_iterator w = self->getNearest(pos, &dis);
#if 0
				float weight = 1.0f;
				ret = meSetPointVMap( p->pnt, LWVMAP_WGHT, w->first.c_str(), 1, &weight );
				if(ret != EDERR_NONE) PRNN("meSetPointVMap error occur : "<<ret);
				for(std::map<std::string, SkelegonInfo>::const_iterator i = self->skelegons_.begin(); i != self->skelegons_.end(); ++i) {
					if(w != i) {// �����͏��O
						ret = meSetPointVMap( p->pnt, LWVMAP_WGHT, i->first.c_str(), 1, NULL );
						if(ret != EDERR_NONE) PRNN("meSetPointVMap error occur : "<<ret);
					}
				}
#else
				float weight_sum = 0;
				std::map<std::string, float> weights;
				weights[w->first] = 1.0f;
				weight_sum += 1.0f;
				float limit = w->second.getLength()*self->limit_mag_;
				if(limit < self->limit_min_) limit = self->limit_min_;
				if(limit > self->limit_max_) limit = self->limit_max_;
				for(std::map<std::string, SkelegonInfo>::const_iterator i = self->skelegons_.begin(); i != self->skelegons_.end(); ++i) {
					if(w != i) {// �����͏��O
						float weight = 0.0f;
						if(limit > 0.0f) {
							float odis;
							if(w->second.parents.end() != std::find(w->second.parents.begin(), w->second.parents.end(), i->first)) {
								odis = self->skelegons_[i->first].getDistance(pos);
								weight = 1.0f - (odis-dis)/limit;
							}
							else if(w->second.children.end() != std::find(w->second.children.begin(), w->second.children.end(), i->first)) {
								odis = self->skelegons_[i->first].getDistance(pos);
								weight = 1.0f - (odis-dis)/limit;
							}
							if(weight > 1.0f) weight = 1.0f;
							if(weight < 0.0f) weight = 0.0f;
							if(weight > 0.0f) PRNN("odis = "<<odis<<"   dis = "<<dis);
						}
						else weight = 0.0f;
						if(weight > 0.0f) {
							weights[i->first] = weight;
							weight_sum += weight;
						}
						else {
							ret = meSetPointVMap( p->pnt, LWVMAP_WGHT, i->first.c_str(), 1, NULL );
							if(ret != EDERR_NONE) PRNN("meSetPointVMap error occur : "<<ret);
						}
					}
				}
#if 1
				// �m�[�}���C�Y
				// �R�~�b�g����܂�GetPointVMap�Ŕ��f����Ȃ��H
				for(std::map<std::string, float>::const_iterator i = weights.begin(); i != weights.end(); ++i) {
					float weight = i->second / weight_sum;
					if(weight > 0.0f) ret = meSetPointVMap( p->pnt, LWVMAP_WGHT, i->first.c_str(), 1, &weight );
					else ret = meSetPointVMap( p->pnt, LWVMAP_WGHT, i->first.c_str(), 1, NULL );
					if(ret != EDERR_NONE) PRNN("meSetPointVMap error occur : "<<ret);
				}
#endif
#endif
				if(self->prog_.step(1)) return EDERR_USERABORT;
			}
			return ret;
		}

		GlobalFunc *global_;
		MeshEditBegin *local_;
		unsigned long locale_;
		LWObjectFuncs *objf_;
		LWXPanelFuncs *xpanf_;
//		LWMeshInfo *mesh_;
		ModData *md_;
		lwpp::modeler::Progress prog_;
		float limit_min_;
		float limit_max_;
		float limit_mag_;
		bool use_calsium_;

		struct SkelegonAttr {
			LineSeg l;
			LWPntID s, e;
			unsigned long skid;
			unsigned long parent;
		};
		struct SkelegonInfo {
			std::vector<SkelegonAttr> attrs;
			std::vector<std::string> parents;
			std::vector<std::string> children;
			void mergeAttr(const std::string &name, const SkelegonInfo &info)
			{
				for(std::vector<SkelegonAttr>::const_iterator i = info.attrs.begin(); i != info.attrs.end(); ++i) {
					attrs.push_back(*i);
				}
			}
			float getLength() const
			{
				float ret = 0;
				std::vector<SkelegonAttr>::const_iterator w = attrs.begin();
				for(std::vector<SkelegonAttr>::const_iterator i = w; i != attrs.end(); ++i)
				{
					float d = distance(w->l.s, i->l.e);
					if(d > ret) ret = d;
				}
				return ret;
			}
			float getDistance(const Vector &pos) const
			{
				float ret = FLT_MAX;
				for(std::vector<SkelegonAttr>::const_iterator i = attrs.begin(); i != attrs.end(); ++i)
				{
					float d = distance(i->l, pos);
					if(d < ret) ret = d;
				}
				return ret;
			}
		};
		std::map<std::string, SkelegonInfo> skelegons_;
		bool mergeSkelegon(std::vector<std::string> &adee, std::map<std::string, SkelegonInfo>::iterator dst, std::map<std::string, SkelegonInfo>::const_iterator src)
		{
			if(src->first.substr(src->first.length()-2) == "_I") {}
			else if(src->first.substr(src->first.length()-2) == "_F") dst->second.mergeAttr(src->first, src->second);
			else return false;
			adee.push_back(src->first);
			for(std::vector<std::string>::const_iterator i = src->second.children.begin(); i != src->second.children.end(); ++i) {
				std::map<std::string, SkelegonInfo>::const_iterator _src = skelegons_.find(*i);
				if(_src != skelegons_.end()) mergeSkelegon(adee, dst, _src);
			}
			return true;
		}

		void setUpHierarchy()
		{
			for(std::map<std::string, SkelegonInfo>::iterator i = skelegons_.begin(); i != skelegons_.end(); ++i) {
				for(std::map<std::string, SkelegonInfo>::iterator j = skelegons_.begin(); j != skelegons_.end(); ++j) {
					if(i!=j) {
						if(i->second.attrs[0].s == j->second.attrs[0].e) i->second.parents.push_back(j->first);
						else if(i->second.attrs[0].e == j->second.attrs[0].s) i->second.children.push_back(j->first);
						else if(i->second.attrs[0].skid && i->second.attrs[0].skid == j->second.attrs[0].parent) {
							i->second.children.push_back(j->first);
						}
						else if(i->second.attrs[0].parent && i->second.attrs[0].parent == j->second.attrs[0].skid) {
							i->second.parents.push_back(j->first);
						}
					}
				}
			}
		}

		void compactHierarchy()
		{
			for(std::map<std::string, SkelegonInfo>::iterator i = skelegons_.begin(); i != skelegons_.end(); ++i) {
				int flag = 0;
				if(use_calsium_) {
					if(i->first.substr(i->first.length()-2) == "_I") flag = 1;
					else if(i->first.substr(i->first.length()-2) == "_F") flag = 2;
				}
				if(flag == 0) {
					std::vector<std::string> adee;
					for(std::vector<std::string>::iterator j = i->second.children.begin(); j != i->second.children.end();) {
						if(use_calsium_) {
							std::map<std::string, SkelegonInfo>::const_iterator _src = skelegons_.find(*j);
							if(_src != skelegons_.end() && mergeSkelegon(adee, i, _src)) j = i->second.children.erase(j);
							else ++j;
						}
						else {
							std::map<std::string, SkelegonInfo>::const_iterator _child = skelegons_.find(*j);
							if(_child != skelegons_.end()) {
								// �q�̕��V�{�[���܂ł̐��������g�̐����Ƃ��Ēǉ�
								if((_child->second.attrs[0].l.s-i->second.attrs[0].l.e).length2() > FLT_EPSILON) {
									i->second.attrs.push_back(i->second.attrs[0]);
									i->second.attrs.back().s = i->second.attrs[0].e;
									i->second.attrs.back().e = _child->second.attrs[0].s;
									i->second.attrs.back().l.s = i->second.attrs[0].l.e;
									i->second.attrs.back().l.e = _child->second.attrs[0].l.s;
									//PRNN("Add Float Bone!");
								}
							}
							++j;
						}
					}
					for(std::vector<std::string>::const_iterator j = adee.begin(); j != adee.end(); ++j) {
						i->second.children.push_back(*j);
					}
				}
			}
			if(use_calsium_) {
				for(std::map<std::string, SkelegonInfo>::iterator i = skelegons_.begin(); i != skelegons_.end();) {
					if(i->first.substr(i->first.length()-2) == "_I") {
						skelegons_.erase(i++);
					}
					else if(i->first.substr(i->first.length()-2) == "_F") {
						skelegons_.erase(i++);
					}
					else ++i;
				}
			}
		}

		std::map<std::string, SkelegonInfo>::const_iterator getNearest(const Vector &pos, float *ret_dis) const
		{
			float dis = FLT_MAX;
			std::map<std::string, SkelegonInfo>::const_iterator ret = skelegons_.end();
			for(std::map<std::string, SkelegonInfo>::const_iterator i = skelegons_.begin(); i != skelegons_.end(); ++i) {
				float d = i->second.getDistance(pos);
				if(d < dis) {
					dis = d;
					ret = i;
				}
			}
			if(ret_dis) *ret_dis = dis;
			return ret;
		}

		enum { ID_LIMIT_MAX = 0x8001, ID_LIMIT_MIN, ID_LIMIT_MAG, ID_USE_CALSIUM, ID_REMOVE_WMAP };

		bool doModal()
		{
		   LWXPanelID panel;

		   static LWXPanelControl ctl[] = {
			  { ID_LIMIT_MAX,     "Limit Max",      "distance"  },
			  { ID_LIMIT_MIN,     "Limit Min",      "distance"  },
			  { ID_LIMIT_MAG,     "Limit Mag",      "percent"   },
			  { ID_USE_CALSIUM,   "Use Calsium",    "iBoolean"  },
			  { 0 }
		   };
		   static LWXPanelDataDesc cdata[] = {
			  { ID_LIMIT_MAX,     "Limit Max",      "distance"  },
			  { ID_LIMIT_MIN,     "Limit Min",      "distance"  },
			  { ID_LIMIT_MAG,     "Limit Mag",      "percent"   },
			  { ID_USE_CALSIUM,   "Use Calsium",    "integer"   },
			  { 0 }
		   };
		   static LWXPanelHint hint[] = {
			  XpLABEL( 0, "Bone weight setup" ),
			  XpMIN( ID_LIMIT_MAX, 0 ),
			  XpMIN( ID_LIMIT_MIN, 0 ),
			  XpDIVADD( ID_LIMIT_MAG ),
			  XpEND
		   };

		   static LWXPanelControl ctlj[] = {
			  { ID_LIMIT_MAX,     "�u�����h���E�̍ő�",      "distance"  },
			  { ID_LIMIT_MIN,     "�u�����h���E�̍ŏ�",      "distance"  },
			  { ID_LIMIT_MAG,     "�u�����h���E(�{�[���̒����̔{��)",      "percent"   },
			  { ID_USE_CALSIUM,   "Calsium���g��",    "iBoolean"  },
			  { 0 }
		   };
		   static LWXPanelDataDesc cdataj[] = {
			  { ID_LIMIT_MAX,     "�u�����h���E�̍ő�",      "distance"  },
			  { ID_LIMIT_MIN,     "�u�����h���E�̍ŏ�",      "distance"  },
			  { ID_LIMIT_MAG,     "�u�����h���E(�{�[���̒����̔{��)",      "percent"   },
			  { ID_USE_CALSIUM,   "Calsium���g��",    "integer"  },
			  { 0 }
		   };
		   static LWXPanelHint hintj[] = {
			  XpLABEL( 0, "�{�[���E�F�C�g�����ݒ�" ),
			  XpMIN( ID_LIMIT_MAX, 0 ),
			  XpMIN( ID_LIMIT_MIN, 0 ),
			  XpDIVADD( ID_LIMIT_MAG ),
			  XpEND
		   };

		   panel = xpanf_->create( LWXP_FORM, locale_ == LANGID_JAPANESE ? ctlj : ctl );
		   if ( !panel ) return false;

		   xpanf_->describe( panel, locale_ == LANGID_JAPANESE ? cdataj : cdata, NULL, NULL );
		   xpanf_->hint( panel, 0, locale_ == LANGID_JAPANESE ? hintj : hint );

		   double limit = 0.08f;
		   xpanf_->formSet( panel, ID_LIMIT_MAX, &limit );
		   limit = 0.01f;
		   xpanf_->formSet( panel, ID_LIMIT_MIN, &limit );
		   limit = 0.05f;
		   xpanf_->formSet( panel, ID_LIMIT_MAG, &limit );
		   int ibool = 0;
		   xpanf_->formSet( panel, ID_USE_CALSIUM, &ibool );

		   if(xpanf_->post( panel )) {
			   limit = *(double *)xpanf_->formGet( panel, ID_LIMIT_MAX );
			   limit_max_ = static_cast<float>(limit);
			   limit = *(double *)xpanf_->formGet( panel, ID_LIMIT_MIN );
			   limit_min_ = static_cast<float>(limit);
			   limit = *(double *)xpanf_->formGet( panel, ID_LIMIT_MAG );
			   limit_mag_ = static_cast<float>(limit);
			   ibool = *(int *)xpanf_->formGet( panel, ID_USE_CALSIUM );
			   use_calsium_ = ibool ? true : false;
			   xpanf_->destroy( panel );
			   return true;
		   }
		   xpanf_->destroy( panel );
		   return false;
		}

	public:
		WeightSetUp(GlobalFunc *global, MeshEditBegin *local) : global_(global), local_(local), prog_(global)
		{
			objf_ = reinterpret_cast<LWObjectFuncs *>(global( LWOBJECTFUNCS_GLOBAL, GFUSE_TRANSIENT ));
			xpanf_ = reinterpret_cast<LWXPanelFuncs *>(global( LWXPANELFUNCS_GLOBAL, GFUSE_TRANSIENT ));
			locale_ = ( unsigned long ) global( LWLOCALEINFO_GLOBAL, GFUSE_TRANSIENT );
		}

		void proc()
		{
			if(objf_ && doModal()) {
				md_ = meInit(global_, local_, 0, 0, OPSEL_USER);
				if(md_) {
					//prog_.init("weightsetup", "Constructing bone weight...", (mePolyCount(OPLYR_FG, EDCOUNT_SELECT)-mePolyCount(OPLYR_FG, EDCOUNT_DELETE))*2+mePointCount(OPLYR_FG, EDCOUNT_SELECT)-mePointCount(OPLYR_FG, EDCOUNT_DELETE));
					prog_.init("weightsetup", "Constructing bone weight...", mePolyCount(OPLYR_FG, EDCOUNT_SELECT)-mePolyCount(OPLYR_FG, EDCOUNT_DELETE)+mePointCount(OPLYR_FG, EDCOUNT_SELECT)-mePointCount(OPLYR_FG, EDCOUNT_DELETE));
					int ret;
					ret = mePolyScan( scanSkelegonCB, this, OPLYR_FG );
					PRNN("mePolyScan ret : "<<ret);
					if(ret == EDERR_NONE) {
						setUpHierarchy();
						compactHierarchy();

						if(skelegons_.size() > 0) {
							ret = mePolyScan( delPolysVMap, this, OPLYR_FG );
							PRNN("mePolyScan ret : "<<ret);
							if(ret == EDERR_NONE) {
								ret = mePointScan( editPointsVMap, this, OPLYR_FG );
								PRNN("mePointScan ret : "<<ret);
								ret = mePolyScan( setWeightToSkelegon, this, OPLYR_FG );
								PRNN("mePointScan ret : "<<ret);
		//						if(ret == EDERR_NONE) {
		//							// ���傤���Ȃ����炱���
		//							WeightNormalize normalize_plugin(global_, local_);
		//							ret = normalize_plugin.proc(md_, prog_); // ���ł���ł��_���Ȃ񂾁H�H�H�H�H�H
		//							PRNN("normalize_plugin.proc ret : "<<ret);
		//							csPlugin("weightsetup.p", LWMESHEDIT_CLASS, THIS_SERVER_NAME2, THIS_SERVER_NAME2); // ����A�v���O�C���ǂݍ��݋@�\
		//							csCmdSeq(THIS_SERVER_NAME2, NULL); // ���႟�A����͂Ȃ񂾁H
		//							md_->local->evaluate( md_->local->data, THIS_SERVER_NAME2 );// ������Ⴄ�c
		//						}
							}
						}
						else {
							LWMessageFuncs *msgf;
							msgf = reinterpret_cast<LWMessageFuncs *>(global_( LWMESSAGEFUNCS_GLOBAL, GFUSE_TRANSIENT ));
							if(locale_ == LANGID_JAPANESE) {
								msgf->error("�{�[���E�F�C�g�𐻍�ł��܂���ł����B", "�X�P���S���ƑΏۂ̃|���S����I�����Ă��������B");
							}
							else {
								msgf->error("Weight Map Setup Fail.", "Select Skelegons and Polygons.");
							}
						}
					}
					prog_.done();
					meDone(ret, 0);
				}
				else PRNN("????");
			}
			else PRNN("????");
		}
	};

	// �E�F�C�g�����m�[�}���C�Y
	class WeightNormalize {
		static int scanSkelegonCB(void *param, const EDPolygonInfo *p)
		{
			WeightNormalize *self = (WeightNormalize *)param;
			if(p->flags & EDDF_SELECT && !(p->flags & EDDF_DELETE) && p->type == LWPOLTYPE_BONE) {
				const char *partname = meGetPolyTag( p->pol, LWPTAG_PART );
				if(partname) self->skelegons_.push_back(partname);
			}
			return 0;
		}
		
		// ��A��VMAP�����邩�`�F�b�N
		static int checkVPMap(void *param, const EDPolygonInfo *p)
		{
			/*WeightNormalize *self = (WeightNormalize *)param;
			if(p->flags & EDDF_SELECT && !(p->flags & EDDF_DELETE) && p->type == LWPOLTYPE_FACE) {
				for(int i = 0; i < p->numPnts; i++) {
					int wmapnum = self->objf_->numVMaps( LWVMAP_WGHT );
					for(std::vector<std::string>::iterator j = self->skelegons_.begin(); j != self->skelegons_.end(); ++j) {
						void *wmap = meSelectPointVMap(NULL, LWVMAP_WGHT, j->c_str());
						if(wmap) {
							float val;
							if(meGetPointVPMap(p->points[i], p->pol, &val)) {
								self->is_exist_vpmap_ = true;
								return EDERR_USERABORT;
							}
						}
					}
				}
			}*/
			return 0;
		}
		
		static EDError normalizePolysVMap(void *param, const EDPolygonInfo *p)
		{
			WeightNormalize *self = (WeightNormalize *)param;
			EDError ret = EDERR_NONE;
			if(p->flags & EDDF_SELECT && !(p->flags & EDDF_DELETE) && p->type == LWPOLTYPE_FACE) {
				for(int i = 0; i < p->numPnts; i++) {
					std::map<std::string, std::pair<bool, float> > weights;
					for(std::vector<std::string>::iterator j = self->skelegons_.begin(); j != self->skelegons_.end(); ++j) {
						void *wmap = meSelectPointVMap(NULL, LWVMAP_WGHT, j->c_str());
						if(wmap) {
							float val;
							/*if(meGetPointVPMap(p->points[i], p->pol, &val)) {
								weights[*j].first = true;
								weights[*j].second = val;
							}
							else */if(meGetPointVMap(p->points[i], &val)) {
								weights[*j].first = false;
								weights[*j].second = val;
							}
						}
					}
					float weight_sum = 0;
					for(std::map<std::string, std::pair<bool, float> >::const_iterator j = weights.begin(); j != weights.end(); ++j) {
						weight_sum += j->second.second;
					}
					for(std::map<std::string, std::pair<bool, float> >::const_iterator j = weights.begin(); j != weights.end(); ++j) {
						if(j->second.first) {
							float weight = j->second.second/weight_sum;
							if(weight>0) ret = meSetPointVPMap( p->points[i], p->pol, LWVMAP_WGHT, j->first.c_str(), 1, &weight );
							else ret = meSetPointVPMap( p->points[i], p->pol, LWVMAP_WGHT, j->first.c_str(), 1, NULL );
						}
						else {
							float weight = j->second.second/weight_sum;
							if(weight>0) ret = meSetPointVMap( p->points[i], LWVMAP_WGHT, j->first.c_str(), 1, &weight );
							else ret = meSetPointVMap( p->points[i], LWVMAP_WGHT, j->first.c_str(), 1, NULL );
						}
					}
				}
				if(self->prog_.step(1)) return EDERR_USERABORT;
			}
			return ret;
		}

		static EDError normalizePointsVMap(void *param, const EDPointInfo *p)
		{
			WeightNormalize *self = (WeightNormalize *)param;
			EDError ret = EDERR_NONE;
			void *wmap = NULL;
			if(p->flags & EDDF_SELECT && !(p->flags & EDDF_DELETE)) {
				std::map<std::string, float> weights;
				for(std::vector<std::string>::iterator j = self->skelegons_.begin(); j != self->skelegons_.end(); ++j) {
					void *wmap = meSelectPointVMap(NULL, LWVMAP_WGHT, j->c_str());
					if(wmap) {
						float val;
						if(meGetPointVMap(p->pnt, &val)) {
							if(val > 0) weights[*j] = val;
							else weights[*j] = 0;
						}
					}
				}
				float weight_sum = 0;
				for(std::map<std::string, float>::const_iterator j = weights.begin(); j != weights.end(); ++j) {
					weight_sum += j->second;
				}
				for(std::map<std::string, float>::const_iterator j = weights.begin(); j != weights.end(); ++j) {
					float weight = 0;
					if(weight_sum > 0) weight = j->second/weight_sum;
					if(weight > 0) ret = meSetPointVMap( p->pnt, LWVMAP_WGHT, j->first.c_str(), 1, &weight );
					else ret = meSetPointVMap( p->pnt, LWVMAP_WGHT, j->first.c_str(), 1, NULL );
				}
				if(self->prog_.step(1)) return EDERR_USERABORT;
			}
			return ret;
		}

		GlobalFunc *global_;
		MeshEditBegin *local_;
		LWObjectFuncs *objf_;
		ModData *md_;
		std::vector<std::string> skelegons_;
		bool is_exist_vpmap_;

		lwpp::modeler::Progress prog_;

	public:
		WeightNormalize(GlobalFunc *global, MeshEditBegin *local) : global_(global), local_(local), prog_(global)
		{
			objf_ = reinterpret_cast<LWObjectFuncs *>(global( LWOBJECTFUNCS_GLOBAL, GFUSE_TRANSIENT ));
		}

		void proc()
		{
			md_ = meInit(global_, local_, 0, 0, OPSEL_USER);
			if(md_) {
				int ret;
				ret = mePolyScan( scanSkelegonCB, this, OPLYR_FG );
				is_exist_vpmap_ = false;
				ret = mePolyScan( checkVPMap, this, OPLYR_FG );
				PRNN("WeightNormalize::proc");
				if(is_exist_vpmap_) {
					prog_.init("PolyWeightNormalize", "Normalizing bone weight(PVMap)...", mePolyCount(OPLYR_FG, EDCOUNT_SELECT)-mePolyCount(OPLYR_FG, EDCOUNT_DELETE));
					ret = mePolyScan( normalizePolysVMap, this, OPLYR_FG );
					PRNN("mePolyScan ret : "<<ret);
				}
				else {
					prog_.init("PointWeightNormalize", "Normalizing bone weight(VMap)...", mePointCount(OPLYR_FG, EDCOUNT_SELECT)-mePointCount(OPLYR_FG, EDCOUNT_DELETE));
					ret = mePointScan( normalizePointsVMap, this, OPLYR_FG );
					PRNN("mePointScan ret : "<<ret);
				}
				meDone(ret, 0);
			}
			else PRNN("????");
		}
	};
}

/*
 *	CommandSequence�̃G���g���[�֐�
 */

XCALL_(static int)
weightsetup( long version, GlobalFunc *global, MeshEditBegin *local, void *serverData )
{
	if (version != LWMODCOMMAND_VERSION)
		return (AFUNC_BADVERSION);

	/*
	* �R�}���h�V�[�N�G���X�v���O�C���̓��f���[���ŃW�I���g�����쐬�E���삷�邽�߂̃R�}���h�𔭍s���܂��B 
	* �܂�MeshDataEdit�N���X�Ɠ������b�V���ҏW�֐��փA�N�Z�X���܂��B
	*
	*	lookup			�R�}���h���̂ɑΉ����鐮���R�[�h��Ԃ��܂��B  
	*						execute�֐��փR�}���h�R�[�h��n���ƃR�}���h�����s����܂��B
	*
	*	execute			�R�}���h�R�[�h�̃R�}���h�𔭍s���܂��B
	*
	* evaluate			�R�}���h���̂ƈ�����p���ăR�}���h�����s���܂��B 
	*/
	
	WeightSetUp self(global, local);
	self.proc();

	return AFUNC_OK;
}


/*
 *	MeshDataEdit�̃G���g���[�֐�
 */

XCALL_(static int)
weightnormalize( long version, GlobalFunc *global, MeshEditBegin *local, void *serverData )
{
	if (version != LWMODCOMMAND_VERSION)
		return (AFUNC_BADVERSION);

	/*
	* �R�}���h�V�[�N�G���X�v���O�C���̓��f���[���ŃW�I���g�����쐬�E���삷�邽�߂̃R�}���h�𔭍s���܂��B 
	* �܂�MeshDataEdit�N���X�Ɠ������b�V���ҏW�֐��փA�N�Z�X���܂��B
	*
	*	lookup			�R�}���h���̂ɑΉ����鐮���R�[�h��Ԃ��܂��B  
	*						execute�֐��փR�}���h�R�[�h��n���ƃR�}���h�����s����܂��B
	*
	*	execute			�R�}���h�R�[�h�̃R�}���h�𔭍s���܂��B
	*
	* evaluate			�R�}���h���̂ƈ�����p���ăR�}���h�����s���܂��B 
	*/
	
	WeightNormalize self(global, local);
	self.proc();

	return AFUNC_OK;
}


/*
 *	���̃v���O�C���T�[�o�[�̃N���X�Ɩ��̂�錾���邽�߂ɕK�v�ȃO���[�o���ϐ�
 */

extern "C" {
ServerTagInfo TagInfo[] = {
	{ "Setup BoneWeights",					SRVTAG_USERNAME|LANGID_USENGLISH },
//	{ "�{�[���E�F�C�g�����ݒ�",				SRVTAG_USERNAME|LANGID_JAPANESE },
	{ "Setup BoneWeights",					SRVTAG_USERNAME|LANGID_JAPANESE },
//	{ "SetupBWs",							SRVTAG_BUTTONNAME|LANGID_USENGLISH },
//	{ "�{�[��W�����ݒ�",					SRVTAG_BUTTONNAME|LANGID_JAPANESE },
	{ "SetupBWs",							SRVTAG_BUTTONNAME },
	{ "Setup BoneWeights Automaticaly",		SRVTAG_DESCRIPTION },
	{ "mappings",                           SRVTAG_CMDGROUP   },
	{ (const char *) NULL },
};
ServerTagInfo TagInfo2[] = {
	{ "Normalize BoneWeights",				SRVTAG_USERNAME|LANGID_USENGLISH },
//	{ "�{�[���E�F�C�g��ԉ�",				SRVTAG_USERNAME|LANGID_JAPANESE	},
	{ "Normalize BoneWeights",				SRVTAG_USERNAME|LANGID_JAPANESE },
//	{ "NormalizeBWs",						SRVTAG_BUTTONNAME|LANGID_USENGLISH },
//	{ "�{�[��W��ԉ�",						SRVTAG_BUTTONNAME|LANGID_JAPANESE },
	{ "NormalizeBWs",						SRVTAG_BUTTONNAME },
	{ "Normalize BoneWeights Automaticaly",	SRVTAG_DESCRIPTION },
	{ "mappings",                           SRVTAG_CMDGROUP   },
	{ (const char *) NULL },
};
ServerRecord ServerDesc[] = {
	/// 33�`127�炵���gamecatapult_lightwave_plugin_��30����������A�K���R�����ȏ�̕�������q����
	{ LWMESHEDIT_CLASS, "gamecatapult_lightwave_mod_plugin_setupbwgt", ( ActivateFunc * )weightsetup, TagInfo },
	{ LWMESHEDIT_CLASS, "gamecatapult_lightwave_mod_plugin_norbwgt", ( ActivateFunc * )weightnormalize, TagInfo2 },
	{ NULL }
};
}

#pragma comment(lib, "DxErr9.lib")
#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "dmoguids.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxtrans.lib")
#pragma comment(lib, "d3dx9.lib")
