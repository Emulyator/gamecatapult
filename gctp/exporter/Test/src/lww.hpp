#ifndef _LWW_HPP_
#define _LWW_HPP_

extern "C" {
#include <lwio.h>
#include <lwhost.h>
#include <lwserver.h>
#include <lwsurf.h>
#include <lwimage.h>
#include <lwmodeler.h>
#include <lwrender.h>
#include <lwmeshedt.h>
#include <lwpanel.h>
#include <lwcomring.h>
#include <lwdyna.h>
}

#include <vector>

namespace LWW {

	class Item;
	class Object;
	//class Light;
	//class Camera;
	class Bone;
	class Channel;
	class ChannelGroup;
	class Envelope;
	class MeshInfo;
	class Surface;
	class Texture;
	class TextureLayer;
	
#define DefInterfaceGetter(_Type, _Name, _ServiceName, _Variable)	\
	public: _Type *get##_Name() { if(!_Variable) _Variable = global<_Type>(_ServiceName); return _Variable; }	\
	private: _Type *_Variable;
#define DefInterfaceGetter2(_Name, _ServiceName, _Variable)	DefInterfaceGetter(LW##_Name, _Name, _ServiceName, _Variable)
#define DefInterfaceGetter3(_Name, _ServiceName, _Variable)	DefInterfaceGetter(_Name, _Name, _ServiceName, _Variable)

	class Globals {
	public:
		Globals(GlobalFunc *globalfunc) { memset(this, 0, sizeof(Globals)); globalfunc_ = globalfunc; }

		GlobalFunc *globalfunc() const { return globalfunc_; }

		template<typename _Type>
		_Type *global(const char *servicename, int usage = GFUSE_TRANSIENT)
		{
			return reinterpret_cast<_Type *>(globalfunc_(servicename, usage));
		}
		
		Item firstItem(LWItemType type);
		Object firstObject();
		//Camera firstCemera(LWItemType type);
		//Light firstLight(LWItemType type);

		int numObjects() { return getObjectFuncs()->numObjects(); }
		int maxLayers(int obj)  { return getObjectFuncs()->maxLayers(obj); }
		MeshInfo layerMesh( int obj_no, int layer_no );

		Surface createSurface(const char *objname, const char *surfname);
		Surface firstSurface();
		Surface getSurface(const char *name, const char *objname);
		std::vector<Surface> getSurfacesByName(const char *name, const char *objname);
		std::vector<Surface> getSurfacesByObject(const char *name);

		// ã§í 
		DefInterfaceGetter3(ContextMenuFuncs, LWCONTEXTMENU_GLOBAL, contextmenufuncs_);
		DefInterfaceGetter2(EnvelopeFuncs, LWENVELOPEFUNCS_GLOBAL, envelopefuncs_);
		DefInterfaceGetter2(ChannelInfo, LWCHANNELINFO_GLOBAL, channelinfo_);
		DefInterfaceGetter2(ColorActivateFunc, LWCOLORACTIVATEFUNC_GLOBAL, coloractivatefunc_);
		DefInterfaceGetter2(ComRing, LWCOMRING_GLOBAL, comring_);
		DefInterfaceGetter2(ObjectFuncs, LWOBJECTFUNCS_GLOBAL, objectfuncs_);
		DefInterfaceGetter2(DirInfoFunc, LWDIRINFOFUNC_GLOBAL, dirinfofuncs_);
		DefInterfaceGetter2(SurfaceFuncs, LWSURFACEFUNCS_GLOBAL, surfacefuncs_);
		DefInterfaceGetter2(TextureFuncs, LWTEXTUREFUNCS_GLOBAL, texturefuncs_);
		DefInterfaceGetter2(ImageList, LWIMAGELIST_GLOBAL, imagelist_);

		// ÉåÉCÉAÉEÉg
		DefInterfaceGetter2(LMonFuncs, LWLMONFUNCS_GLOBAL, lmonfuncs_);
		DefInterfaceGetter2(SceneInfo, LWSCENEINFO_GLOBAL, sceneinfo_);
		DefInterfaceGetter2(ItemInfo, LWITEMINFO_GLOBAL, iteminfo_);
		DefInterfaceGetter2(ObjectInfo, LWOBJECTINFO_GLOBAL, objectinfo_);
		DefInterfaceGetter2(BoneInfo, LWBONEINFO_GLOBAL, boneinfo_);
		DefInterfaceGetter2(InterfaceInfo, LWINTERFACEINFO_GLOBAL, interfaceinfo_);

		// ÉÇÉfÉâÅ[
		DefInterfaceGetter3(DynaMonitorFuncs, LWDYNAMONITORFUNCS_GLOBAL, dynamonitorfuncs_);
		DefInterfaceGetter2(FontListFuncs, LWFONTLISTFUNCS_GLOBAL, fontlistfuncs_);
		DefInterfaceGetter2(StateQueryFuncs, LWSTATEQUERYFUNCS_GLOBAL, statequeryfuncs_);

	private:
		GlobalFunc *globalfunc_;
	};

#undef DefInterfaceGetter
#undef DefInterfaceGetter2
#undef DefInterfaceGetter3

	template<typename _Type>
	class BaseObject {
	public:
		typedef _Type IDType;
		BaseObject() : globals_(0), id_(0) {}
		BaseObject(Globals *globals, _Type id) : globals_(globals), id_(id) {}

		Globals *globals() const { return globals_; }

		_Type id() { return id_; }

		operator bool () const
		{
			return id_ != 0;
		}

		bool operator==(const BaseObject &rhs) const
		{
			return id_ == rhs.id_;
		}

		bool operator!=(const BaseObject &rhs) const
		{
			return id_ != rhs.id_;
		}

		bool operator>(const BaseObject &rhs) const
		{
			return id_ > rhs.id_;
		}

		bool operator<(const BaseObject &rhs) const
		{
			return id_ < rhs.id_;
		}

		bool operator>=(const BaseObject &rhs) const
		{
			return id_ >= rhs.id_;
		}

		bool operator<=(const BaseObject &rhs) const
		{
			return id_ <= rhs.id_;
		}

	protected:
		Globals *globals_;
		_Type id_;
	};

	class Item : public BaseObject<LWItemID> {
	public:
		Item() {}
		Item(Globals *globals, LWItemID id) : BaseObject<LWItemID>(globals, id) {}

		Object toObject();
		//Camera toCamera();
		//Light toLight();
		Bone toBone();

		Item next() { return Item(globals_, globals_->getItemInfo()->next(id_)); }
		Bone firstBone();
		Item firstChild() { return Item(globals_, globals_->getItemInfo()->firstChild(id_)); }
		Item nextChild(Item prev) { return Item(globals_, globals_->getItemInfo()->nextChild(id_, prev.id_)); }
		Item parent() { return Item(globals_, globals_->getItemInfo()->parent(id_)); }
		Item target() { return Item(globals_, globals_->getItemInfo()->target(id_)); }
		Item goal() { return Item(globals_, globals_->getItemInfo()->goal(id_)); }
		LWItemType type() { return globals_->getItemInfo()->type(id_); }
		const char *name() { return globals_->getItemInfo()->name(id_); }
		void param(LWItemParam param, LWTime time, LWDVector vec) { globals_->getItemInfo()->param(id_, param, time, vec); }
		unsigned int limits(LWItemParam param, LWDVector min, LWDVector max) { return globals_->getItemInfo()->limits(id_, param, min, max); }
		const char *getTag(int idx) { return globals_->getItemInfo()->getTag(id_, idx); }
		void setTag(int idx, const char *tag) { return globals_->getItemInfo()->setTag(id_, idx, tag); }
		ChannelGroup channelGroup();
		const char *server(const char *classname, int idx) { return globals_->getItemInfo()->server(id_, classname, idx); }
		unsigned int serverFlags(const char *classname, int idx) { return globals_->getItemInfo()->serverFlags(id_, classname, idx); }
		void controller(LWItemParam param, int type[3]) { return globals_->getItemInfo()->controller(id_, param, type); }
		unsigned int flags() { return globals_->getItemInfo()->flags(id_); }
		LWTime lookAhead() { return globals_->getItemInfo()->lookAhead(id_); }
		double goalStrength(LWTime time) { return globals_->getItemInfo()->goalStrength(id_, time); }
		void stiffness(LWItemParam param, LWDVector vec) { return globals_->getItemInfo()->stiffness(id_, param, vec); }
		unsigned int axisLocks(LWItemParam param) { return globals_->getItemInfo()->axisLocks(id_, param); }
		unsigned int maxLookSteps() { return globals_->getItemInfo()->maxLookSteps(id_); }
		double reliableDistance() { return globals_->getItemInfo()->reliableDistance(id_); }
		unsigned int goalObjective() { return globals_->getItemInfo()->goalObjective(id_); }
		double ikfkBlending(LWTime time) { return globals_->getItemInfo()->ikfkBlending(id_, time); }
		unsigned int ikInitialState() { return globals_->getItemInfo()->ikInitialState(id_); }
		LWFrame ikInitialStateFrame(LWTime time) { return globals_->getItemInfo()->ikInitialStateFrame(id_, time); }
		unsigned int ikSoft(LWTime time, unsigned int *distanceType, double *min, double *max) { return globals_->getItemInfo()->ikSoft(id_, time, distanceType, min, max); }
		Item pole() { return Item(globals_, globals_->getItemInfo()->pole(id_)); }
		Item sameItem(LWItemParam param) { return Item(globals_, globals_->getItemInfo()->sameItem(id_, param)); }
		double sameItemBlend(LWItemParam param, LWTime time) { return globals_->getItemInfo()->sameItemBlend(id_, param, time); }
		unsigned int sameItemBlendMethod(LWItemParam param) { return globals_->getItemInfo()->sameItemBlendMethod(id_, param); }
		unsigned int sameItemFlags(LWItemParam param) { return globals_->getItemInfo()->sameItemFlags(id_, param); }
	};

	class Bone : public Item {
	public:
		Bone() : Item() {}
		Bone(Globals *globals, LWItemID id) : Item(globals, id) {}

		unsigned int flags() { return globals_->getBoneInfo()->flags(id_); }
		void restParam(LWItemParam param, LWDVector vec) { globals_->getBoneInfo()->restParam(id_, param, vec); }
		double restLength() { return globals_->getBoneInfo()->restLength(id_); }
		void limits(double *inner, double *outer) { globals_->getBoneInfo()->limits(id_, inner, outer); }
		const char *weightMap() { return globals_->getBoneInfo()->weightMap(id_); }
		double strength() { return globals_->getBoneInfo()->strength(id_); }
		int falloff() { return globals_->getBoneInfo()->falloff(id_); }
		void jointComp(double *self, double *parent) { globals_->getBoneInfo()->jointComp(id_, self, parent); }
		void muscleFlex(double *self, double *parent) { globals_->getBoneInfo()->muscleFlex(id_, self, parent); }
		int bonetype() { return globals_->getBoneInfo()->type(id_); }
		double twist() { return globals_->getBoneInfo()->twist(id_); }
		int transform(int transform, int relativeto, LWFMatrix3 m, LWFVector pos, LWFVector end) { return globals_->getBoneInfo()->transform(id_, transform, relativeto, m, pos, end); }
		void muscleBulge(double *self, double *parent) { return globals_->getBoneInfo()->muscleBulge(id_, self, parent); }
		void muscleBulgeMap(LWTextureID *self, LWTextureID *parent) { return globals_->getBoneInfo()->muscleBulgeMap(id_, self, parent); }
		void displacementMap(LWTextureID *self, LWTextureID *parent) { return globals_->getBoneInfo()->displacementMap(id_, self, parent); }
	};

	class Object : public Item {
	public:
		Object() : Item() {}
		Object(Globals *globals, LWItemID id) : Item(globals, id) {}

		const char *filename() { return globals_->getObjectInfo()->filename(id_); }
		int numPoints() { return globals_->getObjectInfo()->numPoints(id_); }
		int numPolygons() { return globals_->getObjectInfo()->numPolygons(id_); }
		unsigned int shadowOpts() { return globals_->getObjectInfo()->shadowOpts(id_); }
		double dissolve(LWTime in_time) { return globals_->getObjectInfo()->dissolve(id_, in_time); }
		MeshInfo meshInfo(int frozen_flag);
		unsigned int flags() { return globals_->getObjectInfo()->flags(id_); }
		double fog(LWTime in_time) { return globals_->getObjectInfo()->fog(id_, in_time); }
		LWTextureID dispMap() { return globals_->getObjectInfo()->dispMap(id_); }
		LWTextureID clipMap() { return globals_->getObjectInfo()->clipMap(id_); }
		void patchLevel(int *for_display, int *for_render) { globals_->getObjectInfo()->patchLevel(id_, for_display, for_render); }
		void metaballRes(double *for_display, double *for_render) { globals_->getObjectInfo()->metaballRes(id_, for_display, for_render); }
		Item boneSource() { return Item(globals_, globals_->getObjectInfo()->boneSource(id_)); }
		Item morphTarget() { return Item(globals_, globals_->getObjectInfo()->morphTarget(id_)); }
		double morphAmount(LWTime in_time) { return globals_->getObjectInfo()->morphAmount(id_, in_time); }
		unsigned int edgeOpts() { return globals_->getObjectInfo()->edgeOpts(id_); }
		void edgeColor(LWTime in_time, LWDVector color) { globals_->getObjectInfo()->edgeColor(id_, in_time, color); }
		int subdivOrder() { return globals_->getObjectInfo()->subdivOrder(id_); }
		double polygonSize(LWTime in_time) { return globals_->getObjectInfo()->polygonSize(id_, in_time); }
		int excluded(LWItemID lightitem) { return globals_->getObjectInfo()->excluded(id_, lightitem); }
		void matteColor(LWTime in_time, LWDVector color) { return globals_->getObjectInfo()->matteColor(id_, in_time, color); }
		double thickness(LWTime in_time, int type) { return globals_->getObjectInfo()->thickness(id_, in_time, type); }
		double edgeZScale(LWTime in_time) { return globals_->getObjectInfo()->edgeZScale(id_, in_time); }
		double shrinkEdgesNominalDistance(LWTime in_time) { return globals_->getObjectInfo()->shrinkEdgesNominalDistance(id_, in_time); }
		double maxDissolveDistance() { return globals_->getObjectInfo()->maxDissolveDistance(id_); }
		double bumpDistance() { return globals_->getObjectInfo()->bumpDistance(id_); }
		unsigned int getGroupIndex() { return globals_->getObjectInfo()->getGroupIndex(id_); }
		int dispMapOrder() { return globals_->getObjectInfo()->dispMapOrder(id_); }
		int bumpOrder() { return globals_->getObjectInfo()->bumpOrder(id_); }
		NodeEditorID getNodeEditor() { return globals_->getObjectInfo()->getNodeEditor(id_); }
		int nodeOrder() { return globals_->getObjectInfo()->nodeOrder(id_); }
		void bounds(LWDVector min, LWDVector max) { return globals_->getObjectInfo()->bounds(id_, min, max); }
		double shadowoffsetdistance() { return globals_->getObjectInfo()->shadowoffsetdistance(id_); }
	};

	class ChannelGroup : public BaseObject<LWChanGroupID> {
	public:
		ChannelGroup() {}
		ChannelGroup(Globals *globals, LWChanGroupID id) : BaseObject<LWChanGroupID>(globals, id) {}

		Channel firstChannel();
		ChannelGroup create(const char *name) { return ChannelGroup(globals_, globals_->getEnvelopeFuncs()->createGroup(id_, name)); }
		void destroy() { return globals_->getEnvelopeFuncs()->destroyGroup(id_); id_ = 0; }

		Envelope createEnvelope(const char *name, int type);

		ChannelGroup next() { return ChannelGroup(globals_, globals_->getChannelInfo()->nextGroup(globals_->getChannelInfo()->groupParent(id_), id_)); }
		const char *name() { return globals_->getChannelInfo()->groupName(id_); }
		ChannelGroup parent() { return ChannelGroup(globals_, globals_->getChannelInfo()->groupParent(id_)); }
	};

	class Channel : public BaseObject<LWChannelID> {
	public:
		Channel() {}
		Channel(Globals *globals, LWChannelID id) : BaseObject<LWChannelID>(globals, id) {}

		Channel next() { return Channel(globals_, globals_->getChannelInfo()->nextChannel(globals_->getChannelInfo()->channelParent(id_), id_)); }
		const char *name() { return globals_->getChannelInfo()->channelName(id_); }
		ChannelGroup parent() { return ChannelGroup(globals_, globals_->getChannelInfo()->channelParent(id_)); }
		int type() { return globals_->getChannelInfo()->channelType(id_); }

		double evaluate(LWTime chantime) { return globals_->getChannelInfo()->channelEvaluate(id_, chantime); }
		Envelope envelope();
		int setEvent(LWChanEventFunc ev, void *data) { return globals_->getChannelInfo()->setChannelEvent(id_, ev, data); }
		const char *server(const char *classname, int idx ) { return globals_->getChannelInfo()->server(id_, classname, idx); }
		unsigned int serverFlags(const char *classname, int idx ) { return globals_->getChannelInfo()->serverFlags(id_, classname, idx); }
		LWInstance serverInstance(const char *classname, int idx ) { return globals_->getChannelInfo()->serverInstance(id_, classname, idx); }
		int serverApply(const char *classname, const char *name, int flags ) { return globals_->getChannelInfo()->serverApply(id_, classname, name, flags); }
		void serverRemove(const char *classname, const char *name, LWInstance inst ) { return globals_->getChannelInfo()->serverRemove(id_, classname, name, inst); }
	};

	class Envelope : public BaseObject<LWEnvelopeID> {
	public:
		Envelope() {}
		Envelope(Globals *globals, LWEnvelopeID id) : BaseObject<LWEnvelopeID>(globals, id) {}

		void destroy() { globals_->getEnvelopeFuncs()->destroy(id_); id_ = 0; }
		LWError copy(LWEnvelopeID from) { return globals_->getEnvelopeFuncs()->copy(id_, from); }
		LWError load(const LWLoadState *load) { return globals_->getEnvelopeFuncs()->load(id_, load); }
		LWError save(const LWSaveState *save) { return globals_->getEnvelopeFuncs()->save(id_, save); }
		double evaluate(LWTime attime) { return globals_->getEnvelopeFuncs()->evaluate(id_, attime); }
		int edit(LWChanGroupID group, int flags, void *data) { return globals_->getEnvelopeFuncs()->edit(group, id_, flags, data); }
		int envAge() { return globals_->getEnvelopeFuncs()->envAge(id_); }
		LWEnvKeyframeID createKey(LWEnvelopeID env, LWTime keytime, double value) { return globals_->getEnvelopeFuncs()->createKey(id_, keytime, value); }
		void destroyKey(LWEnvKeyframeID key) { return globals_->getEnvelopeFuncs()->destroyKey(id_, key); }
		LWEnvKeyframeID findKey(LWTime keytime) { return globals_->getEnvelopeFuncs()->findKey(id_, keytime); }
		LWEnvKeyframeID nextKey(LWEnvKeyframeID key) { return globals_->getEnvelopeFuncs()->nextKey(id_, key); }
		LWEnvKeyframeID prevKey(LWEnvKeyframeID key) { return globals_->getEnvelopeFuncs()->prevKey(id_, key); }
		int keySet(LWEnvKeyframeID key, LWKeyTag tag, void *value) { return globals_->getEnvelopeFuncs()->keySet(id_, key, tag, value); }
		int keyGet(LWEnvKeyframeID key, LWKeyTag tag, void *value) { return globals_->getEnvelopeFuncs()->keyGet(id_, key, tag, value); }
		int setEnvEvent(LWEnvEventFunc  ev, void *data) { return globals_->getEnvelopeFuncs()->setEnvEvent(id_, ev, data); }
		int egSet(LWChanGroupID group, int tag, void *value) { return globals_->getEnvelopeFuncs()->egSet(id_, group, tag, value); }
		int egGet(LWChanGroupID group, int tag, void *value) { return globals_->getEnvelopeFuncs()->egGet(id_, group, tag, value); }
	};

	class MeshInfo : public BaseObject<LWMeshInfoID> {
	public:
		MeshInfo() {}
		MeshInfo(Globals *globals, LWMeshInfoID id) : BaseObject<LWMeshInfoID>(globals, id) {}

		void *priv() { return id_->priv; }
		void destroy() { return id_->destroy(id_); }

		int numPoints() { return id_->numPoints(id_); }
		int numPolygons() { return id_->numPolygons(id_); }

		size_t scanPoints(LWPntScanFunc *func, void *userdata) { return id_->scanPoints(id_, func, userdata); }
		size_t scanPolys(LWPolScanFunc *func, void *userdata) { return id_->scanPolys(id_, func, userdata); }

		void pntBasePos(LWPntID point, LWFVector pos) { return id_->pntBasePos(id_, point, pos); }
		void pntOtherPos(LWPntID point, LWFVector pos) { return id_->pntOtherPos(id_, point, pos); }

		/*
		LWVMAP_PICK LWID_('P','I','C','K')
		LWVMAP_WGHT LWID_('W','G','H','T')
		LWVMAP_MNVW LWID_('M','N','V','W')
		LWVMAP_TXUV LWID_('T','X','U','V')
		LWVMAP_MORF LWID_('M','O','R','F')
		LWVMAP_SPOT LWID_('S','P','O','T')
		LWVMAP_RGB  LWID_('R','G','B',' ')
		LWVMAP_RGBA LWID_('R','G','B','A')
		LWVMAP_NORM LWID_('N','O','R','M')
		*/
		void *pntVLookup(LWID type, const char *name) { return id_->pntVLookup(id_, type, name); }
		int pntVSelect(void *vmap) { return id_->pntVSelect(id_, vmap); }
		int pntVGet(LWPntID point, float *vector) { return id_->pntVGet(id_, point, vector); }

		/*
		LWPOLTYPE_FACE - face
		LWPOLTYPE_CURV - higher order curve
		LWPOLTYPE_PTCH - subdivision control cage polygon
		LWPOLTYPE_MBAL - metaball
		LWPOLTYPE_BONE - bone
		*/
		LWID polType(LWPolID polygon) { return id_->polType(id_, polygon); }
		int polSize(LWPolID polygon) { return id_->polSize(id_, polygon); }
		LWPntID polVertex(LWPolID polygon, int vert_index) { return id_->polVertex(id_, polygon, vert_index); }
		/*
		LWPTAG_SURF
		LWPTAG_PART
		LWPTAG_TXUV
		LWPTAG_COLR
		*/
		const char *polTag(LWPolID polygon, LWID tag) { return id_->polTag(id_, polygon, tag); }

		int pntVPGet(LWPntID point, LWPolID polygon, float *vector) { return id_->pntVPGet(id_, point, polygon, vector); }
		/*
		EDDF_SELECT
		EDDF_DELETE
		EDPF_CCSTART
		EDPF_CCEND
		*/
		unsigned int polFlags(LWPolID polygon) { return id_->polFlags(id_, polygon); }

		int pntVIDGet(LWPntID point, float *vector, void *vmap) { return id_->pntVIDGet(id_, point, vector, vmap); }
		int pntVPIDGet(LWPntID point, LWPolID polygon, float *vector, void *vmap) { return id_->pntVPIDGet(id_, point, polygon, vector, vmap); }

		/*
		LWGPTF_SURFACE
		LWGPTF_FLAT
		LWGPTF_LINEAR
		LWGPTF_SUBD
		*/
		int polTypeFlags(LWPolID polygon) { return id_->polTypeFlags(id_, polygon); }

		int polBaseNormal(LWPolID polygon, LWFVector out_normal) { return id_->polBaseNormal(id_, polygon, out_normal); }
		int polOtherNormal(LWPolID polygon, LWFVector out_normal) { return id_->polOtherNormal(id_, polygon, out_normal); }
		int pntBaseNormal(LWPolID polygon, LWPntID point, LWFVector out_normal) { return id_->pntBaseNormal(id_, polygon, point, out_normal); }
		int pntOtherNormal(LWPolID polygon, LWPntID point, LWFVector out_normal) { return id_->pntOtherNormal(id_, polygon, point, out_normal); }

		LWMeshIteratorID createMeshIterator(int iterator_type) { return id_->createMeshIterator(id_, iterator_type); }
		void destroyMeshIterator(LWMeshIteratorID iterator) { return id_->destroyMeshIterator(id_, iterator); }
		void *iterateMesh(LWMeshIteratorID iterator) { return id_->iterateMesh(id_, iterator); }
		void resetMeshIterator(LWMeshIteratorID iterator) { return id_->resetMeshIterator(id_, iterator); }

		struct Itr {
			Itr(MeshInfo &mesh, int type) : mesh_(mesh), itr_(mesh.createMeshIterator(type))
			{
			}
			~Itr()
			{
				mesh_.destroyMeshIterator(itr_);
			}
			void reset()
			{
				mesh_.resetMeshIterator(itr_);
			}
			void *next() { return mesh_.iterateMesh(itr_); }
			MeshInfo &mesh_;
			LWMeshIteratorID itr_;
		};
		struct PolygonItr : Itr {
			PolygonItr(MeshInfo &mesh) : Itr(mesh, LWMESHITER_POLYGON), current_(0)
			{
			}
			LWPolID next() { return current_ = (LWPolID)Itr::next(); }
			LWPolID current() { return current_; }
			LWPolID current_;
		};
		struct PointItr : Itr {
			PointItr(MeshInfo &mesh) : Itr(mesh, LWMESHITER_POINT), current_(0)
			{
			}
			LWPntID next() { return current_ = (LWPntID)Itr::next(); }
			LWPntID current() { return current_; }
			LWPntID current_;
		};

		int setMeshEvent(LWMeshEventFunc *eventfunc, void *userdata) { return id_->setMeshEvent(id_, eventfunc, userdata); }

		/// LWObjectFuncs Ç©ÇÁãtà¯Ç´Ç∑ÇÈ
		int objIdx(int *layer_no = 0)
		{
			int nobj = globals_->getObjectFuncs()->numObjects();
			for(int i = 0; i < nobj; i++) {
				int nlyr = globals_->getObjectFuncs()->maxLayers(i);
				for(int j = 0; j < nlyr; j++) {
					if(globals_->getObjectFuncs()->layerMesh(i, j) == id_) {
						if(layer_no) *layer_no = j;
						return i;
					}
				}
			}
			return 0;
		}
		/// LWObjectFuncs Ç©ÇÁãtà¯Ç´Ç∑ÇÈ
		const char *filename()
		{
			int obj = objIdx();
			return globals_->getObjectFuncs()->filename(obj);
		}
		/// LWObjectFuncs Ç©ÇÁãtà¯Ç´Ç∑ÇÈ
		const char *name()
		{
			int lyr;
			int obj = objIdx(&lyr);
			return globals_->getObjectFuncs()->layerName(obj, lyr);
		}
	};

	struct ScanPolys {
		ScanPolys(MeshInfo &mesh) : mesh(mesh) {}
		MeshInfo &mesh;

		virtual size_t proc(LWPolID polygon) = 0;

		size_t scan()
		{
			return mesh.scanPolys((LWPolScanFunc *)&scanfunc, this);
		}

		static size_t scanfunc(ScanPolys *self, LWPolID polygon)
		{
			return self->proc(polygon);
		}
	};

	struct ScanPnts {
		ScanPnts(MeshInfo &mesh) : mesh(mesh) {}
		MeshInfo &mesh;
		
		virtual size_t proc(LWPntID point) = 0;
		
		size_t scan()
		{
			return mesh.scanPoints((LWPntScanFunc *)&scanfunc, this);
		}

		static size_t scanfunc(ScanPnts *self, LWPntID point)
		{
			return self->proc(point);
		}
	};

	class Surface : public BaseObject<LWSurfaceID> {
	public:
		Surface() {}
		Surface(Globals *globals, LWSurfaceID id) : BaseObject<LWSurfaceID>(globals, id) {}

		Surface next() { return Surface(globals_, globals_->getSurfaceFuncs()->next(id_)); }
		const char *name() { return globals_->getSurfaceFuncs()->name(id_); }
		const char *sceneObject() { return globals_->getSurfaceFuncs()->name(id_); }

		/*
		 channel name
		SURF_COLR	 "BaseColor"
		SURF_LUMI	 "Luminosity"
		SURF_DIFF	 "Diffuse"
		SURF_SPEC	 "Specularity"
		SURF_REFL	 "Reflectivity"
		SURF_TRAN	 "Transparency"
		SURF_TRNL	 "Translucency"
		SURF_RIND	 "IOR"
		SURF_BUMP	 "Bump"
		SURF_GLOS	 "Glossiness"
		SURF_BUF1	 "SpecialBuffer1"
		SURF_BUF2	 "SpecialBuffer2"
		SURF_BUF3	 "SpecialBuffer3"
		SURF_BUF4    "SpecialBuffer4"
		SURF_SHRP	 "DiffuseSharpness"
		SURF_BDRP    "BumpDropoff"
		SURF_SMAN	 "SmoothingAngle"
		SURF_RSAN	 "ReflectionSeamAngle"
		SURF_TSAN	 "RefractionSeamAngle"
		SURF_RBLR	 "ReflectionBlurring"
		SURF_TBLR	 "RefractionBlurring"
		SURF_CLRF	 "ColorFilter"
		SURF_CLRH	 "ColorHighlights"
		SURF_ADTR	 "AdditiveTransparency"
		SURF_AVAL	 "AlphaValue"
		SURF_GVAL	 "GlowValue"
		SURF_LCOL	 "LineColor"
		SURF_LSIZ	 "LineSize"
		SURF_ALPH	 "AlphaOptions"
		SURF_RFOP	 "ReflectionOptions"
		SURF_TROP	 "RefractionOptions"
		SURF_SIDE	 "Sidedness"
		SURF_NVSK	 "ExcludeFromVStack"
		SURF_GLOW	 "Glow"
		SURF_LINE	 "RenderOutlines"
		SURF_RIMG	 "ReflectionImage"
		SURF_TIMG	 "RefractionImage"
		SURF_VCOL	 "VertexColoring"
		SURF_NORM	 "VertexNormal"
		*/
		int getInt(const char *channel) { return globals_->getSurfaceFuncs()->getInt(id_, channel); }
		double *getFlt(const char *channel) { return globals_->getSurfaceFuncs()->getFlt(id_, channel); }
		LWEnvelopeID getEnv(const char *channel) { return globals_->getSurfaceFuncs()->getEnv(id_, channel); }
		Texture getTex(const char *channel);
		LWImageID getImg(const char *channel) { return globals_->getSurfaceFuncs()->getImg(id_, channel); }

		LWChanGroupID chanGrp() { return globals_->getSurfaceFuncs()->chanGrp(id_); }
		const char *getColorVMap() { return globals_->getSurfaceFuncs()->getColorVMap(id_); }
		void setColorVMap(const char *vmapname,int type) { return globals_->getSurfaceFuncs()->setColorVMap(id_, vmapname, type); }

		/*LWSurfLibID createLib() { return Surface(globals_, globals_->getSurfaceFuncs()->name(id_)); }
		void destroyLib(LWSurfLibID lib) { return Surface(globals_, globals_->getSurfaceFuncs()->name(id_)); }
		void copyLib(LWSurfLibID to, LWSurfLibID from);
		LWSurfLibID objectLib(const char *objname); 
		LWSurfLibID loadLib(const char *name); 
		LWError saveLib(LWSurfLibID lib, const char *name);
		int slibCount(LWSurfLibID lib);
		LWSurfaceID slibByIndex(LWSurfLibID lib,int idx);*/
		NodeEditorID getNodeEditor() { return globals_->getSurfaceFuncs()->getNodeEditor(id_); }

		int setInt(const char *channel, int val) { return globals_->getSurfaceFuncs()->setInt(id_, channel, val); }
		int setFlt(const char *channel, double *val) { return globals_->getSurfaceFuncs()->setFlt(id_, channel, val); }
		int setEnv(const char *channel, LWEnvelopeID val) { return globals_->getSurfaceFuncs()->setEnv(id_, channel, val); }
		int setTex(const char *channel, LWTextureID val) { return globals_->getSurfaceFuncs()->setTex(id_, channel, val); }
		int setImg(const char *channel, LWImageID val) { return globals_->getSurfaceFuncs()->setImg(id_, channel, val); }
		int rename(const char *name) { return globals_->getSurfaceFuncs()->rename(id_, name); }
		int copy(LWSurfaceID from) { return globals_->getSurfaceFuncs()->copy(id_, from); }

		//void copyLibByName(LWSurfLibID to, LWSurfLibID from) { return globals_->getSurfaceFuncs()->copyLibByName(to, from); }
		const char *getNormalVMap() { return globals_->getSurfaceFuncs()->getNormalVMap(id_); }
	};

	class Texture : public BaseObject<LWTextureID> {
	public:
		Texture() {}
		Texture(Globals *globals, LWTextureID id) : BaseObject<LWTextureID>(globals, id) {}

		/*LWTxtrContextID	contextCreate(LWTxtrParamFuncs funcs);
		void contextDestroy(LWTxtrContextID	gc);
		void contextAddParam(LWTxtrContextID gc, LWTxtrParamDesc pd);

		LWTextureID	create(int returnType, const char *name, LWTxtrContextID gc, void *userData);*/
		void destroy() { globals_->getTextureFuncs()->destroy(id_); }
		void copy(LWTextureID from) { globals_->getTextureFuncs()->copy(id_, from); }
		void newtime(LWTime t, LWFrame f) { globals_->getTextureFuncs()->newtime(id_, t, f); }
		void cleanup() { globals_->getTextureFuncs()->cleanup(id_); }
		void load(const LWLoadState	*state) { globals_->getTextureFuncs()->load(id_, state); }
		void save(const LWSaveState	*state) { globals_->getTextureFuncs()->save(id_, state); }
		double evaluate(LWMicropolID mp, double *rv) { return globals_->getTextureFuncs()->evaluate(id_, mp, rv); }
		void setEnvGroup(LWChanGroupID grp) { globals_->getTextureFuncs()->setEnvGroup(id_, grp); }

		TextureLayer firstLayer();
		TextureLayer lastLayer();
		TextureLayer layerAdd(int layertype);

		//double noise(double p[3]);	// returns Perlin's noise values in [0,1]
		void *userData() { return globals_->getTextureFuncs()->userData(id_); }
		ChannelGroup envGroup() { return ChannelGroup(globals_, globals_->getTextureFuncs()->envGroup(id_)); }

		const char *name() { return globals_->getTextureFuncs()->name(id_); }
		int type() { return globals_->getTextureFuncs()->type(id_); }
		//LWTxtrContextID context(LWTextureID txtr);
	};

	class TextureLayer : public BaseObject<LWTLayerID> {
	public:
		TextureLayer() {}
		TextureLayer(Globals *globals, LWTLayerID id) : BaseObject<LWTLayerID>(globals, id) {}

		TextureLayer next() { return TextureLayer(globals_, globals_->getTextureFuncs()->nextLayer(globals_->getTextureFuncs()->texture(id_), id_)); }
		void setType(int type) { globals_->getTextureFuncs()->layerSetType(id_, type); }
		int type() { return globals_->getTextureFuncs()->layerType(id_); }
		double evaluate(LWMicropolID mp, double *rv) { return globals_->getTextureFuncs()->layerEvaluate(id_, mp, rv); }
		ChannelGroup layerEnvGroup(LWTLayerID layer) { return ChannelGroup(globals_, globals_->getTextureFuncs()->layerEnvGroup(id_)); }
		int	setParam(int tag, void *data) { return globals_->getTextureFuncs()->setParam(id_, tag, data); }
		int	getParam(int tag, void *data) { return globals_->getTextureFuncs()->getParam(id_, tag, data); }
		void evaluateUV(int wAxis, int oAxis, double oPos[3], double wPos[3], double uv[2]) { globals_->getTextureFuncs()->evaluateUV(id_, wAxis, oAxis, oPos, wPos, uv); }

		Texture texture() { return Texture(globals_, globals_->getTextureFuncs()->texture(id_)); }
	};

	class Image : public BaseObject<LWImageID> {
	public:
		Image() {}
		Image(Globals *globals, LWImageID id) : BaseObject<LWImageID>(globals, id) {}

		//LWImageID first();
		Image next() { return Image(globals_, globals_->getImageList()->next(id_)); }
		//LWImageID load( const char *filename);
		const char *name() { return globals_->getImageList()->name(id_); }
		const char *filename(LWFrame frame) { return globals_->getImageList()->filename(id_, frame); }
		int isColor() { return globals_->getImageList()->isColor(id_); }
		void needAA() { return globals_->getImageList()->needAA(id_); }
		void size(int *w, int *h) { return globals_->getImageList()->size(id_, w, h); }
		/*LWBufferValue luma( LWImageID img, int x, int y );
		void RGB( LWImageID img, int x, int y, LWBufferValue values[3] );
		double lumaSpot( LWImageID img, double x, double y, double spotSize, int in_blend);
		void RGBSpot( LWImageID img, double x, double y, double spotSize, int in_blend, double rgb[3]);
		void clear( LWImageID img );
		LWImageID sceneLoad( const LWLoadState *load );
		void sceneSave( const LWSaveState *save, LWImageID img );
		int hasAlpha( LWImageID img);
		LWBufferValue alpha( LWImageID img, int x, int y );
		double alphaSpot( LWImageID img, double x, double y, double spotSize, int in_blend);
		LWPixmapID evaluate( LWImageID img, LWTime t);
		void changeEvent( LWImageEventFunc func, int code);
		int replace( LWImageID img, const char *filename );
		LWPixmapID create( const char* name, int width, int height, LWImageType type );*/
		//void saverNotifyAttach( LWInstance, LWImageSaverNotify );
		//void saverNotifyDetach( LWInstance );
		//void saverNotifyMarkUsage( LWTextureID );
	};

	inline Item Globals::firstItem(LWItemType type) { return Item(this, getItemInfo()->first(type, 0)); }
	inline Object Globals::firstObject() { return Object(this, getItemInfo()->first(LWI_OBJECT, 0)); }
	//inline Camera Globals::firstCamera() { return Camera(this, getItemInfo()->first(LWI_LIGHT, 0)); }
	//inline Light Globals::firstLight() { return Light(this, getItemInfo()->first(LWI_CAMERA, 0)); }
	inline MeshInfo Globals::layerMesh(int obj_no, int layer_no) { return MeshInfo(this, getObjectFuncs()->layerMesh( obj_no, layer_no )); }
	inline Surface Globals::createSurface(const char *objname, const char *surfname) { return Surface(this, getSurfaceFuncs()->create(objname, surfname)); }
	inline Surface Globals::firstSurface() { return Surface(this, getSurfaceFuncs()->first()); }
	inline Surface Globals::getSurface(const char *name, const char *objname)
	{
		LWSurfaceID *array = getSurfaceFuncs()->byName(name, objname);
		return Surface(this, array ? *array : 0);
	}
	inline std::vector<Surface> Globals::getSurfacesByName(const char *name, const char *objname)
	{
		std::vector<Surface> ret;
		LWSurfaceID *array = getSurfaceFuncs()->byName(name, objname);
		while(*array) ret.push_back(Surface(this, *array++));
		return ret;
	}
	inline std::vector<Surface> Globals::getSurfacesByObject(const char *objname)
	{
		std::vector<Surface> ret;
		LWSurfaceID *array = getSurfaceFuncs()->byObject(objname);
		while(*array) ret.push_back(Surface(this, *array++));
		return ret;
	}

	inline Object Item::toObject() { return Object(globals_, id_); }
	inline Bone Item::toBone() { return Bone(globals_, id_); }
	//inline Camera Item::toCamera() { return Camera(globals_, id_); }
	//inline Light Item::toLight() { return Light(globals_, id_); }
	inline Bone Item::firstBone() { return Bone(globals_, globals_->getItemInfo()->first(LWI_BONE, id_)); }
	inline ChannelGroup Item::channelGroup() { return ChannelGroup(globals_, globals_->getItemInfo()->chanGroup(id_)); }

	inline MeshInfo Object::meshInfo(int frozen_flag = TRUE) { return MeshInfo(globals_, globals_->getObjectInfo()->meshInfo(id_, frozen_flag)); }

	inline Envelope ChannelGroup::createEnvelope(const char *name, int type) { return Envelope(globals_, globals_->getEnvelopeFuncs()->create(id_, name, type)); }
	inline Channel ChannelGroup::firstChannel() { return Channel(globals_, globals_->getChannelInfo()->nextChannel(id_, 0)); }

	inline Envelope Channel::envelope() { return Envelope(globals_, globals_->getChannelInfo()->channelEnvelope(id_)); }

	inline Texture Surface::getTex(const char *channel) { return Texture(globals_, globals_->getSurfaceFuncs()->getTex(id_, channel)); }

	inline TextureLayer Texture::firstLayer() { return TextureLayer(globals_, globals_->getTextureFuncs()->firstLayer(id_)); }
	inline TextureLayer Texture::lastLayer() { return TextureLayer(globals_, globals_->getTextureFuncs()->lastLayer(id_)); }
	inline TextureLayer Texture::layerAdd(int layertype) { return TextureLayer(globals_, globals_->getTextureFuncs()->layerAdd(id_, layertype)); }

}

#endif /* _LWW_HPP_  */
