/**
 * $Id$
 */
#ifndef eman__obejct__em__
#define eman__obejct__em__ 1

#include <string>
#include <map>
#include <vector>
#include <stdio.h>
#include "log.h"
#include "exception.h"

using std::vector;
using std::string;
using std::map;

namespace EMAN
{
	class EMData;
	class XYData;
	class Aligner;
	class Averager;
	class Cmp;
	class Filter;
	class Projector;
	class Reconstructor;

	enum MapInfoType {
		NORMAL,
		ICOS2F_FIRST_OCTANT,
		ICOS2F_FULL,
		ICOS2F_HALF,
		ICOS3F_HALF,
		ICOS3F_FULL,
		ICOS5F_HALF,
		ICOS5F_FULL,
		ICOS_UNKNOWN
	};

	/** EMObject is a wrapper class for types including int, float,
     * double, etc as defined in ObjectType. Each type is typically used 
     * as follows ('int' is the example):
     *
     *    int a = 12;
     *    EMObject o(a);
     *    EMObject o2 = a; // implicit converter from int to EMObject. 
     *    int a1 = o;      // implicit converter from EMObject to int.
     */
	class EMObject
	{
	public:
		enum ObjectType {
			INT,
			FLOAT,
			DOUBLE,
			STRING,
			EMDATA,
			XYDATA,
			FLOATARRAY,
			UNKNOWN
		};

	public:
		EMObject():type(UNKNOWN)
		{
			n = 0;
			f = 0;
			d = 0;
			emdata = 0;
			xydata = 0;
		}

		EMObject(int num):n(num), type(INT)
		{
		}
		EMObject(float ff):f(ff), type(FLOAT)
		{
		}
		EMObject(double dd):d(dd), type(DOUBLE)
		{
		}
		EMObject(const char *s):str(string(s)), type(STRING)
		{
		}
		EMObject(string s):str(s), type(STRING) {
		}
		EMObject(EMData * em):emdata(em), type(EMDATA) {
		}
		EMObject(XYData * xy):xydata(xy), type(XYDATA) {
		}
		EMObject(const vector < float >&v):farray(v), type(FLOATARRAY)
		{
		}

		~EMObject() {
		}

		operator  int () const;
		operator  float () const;
		operator  double () const;
		operator  const char *() const;
		operator  EMData *() const;
		operator  XYData *() const;

		vector < float >get_farray() const;
		bool is_null() const;
		string to_str() const;
		static const char *get_object_type_name(ObjectType t);

	private:
		union
		{
			int n;
			float f;
			double d;
		};

		EMData *emdata;
		XYData *xydata;
		string str;
		vector < float >farray;
		ObjectType type;
	};


	/** Dict is a dictionary to store <string, EMObject> pair.
     * Typical ways to construct a Dict:
     *
     *      Dict d;
     *      d["lowpass"] = 12.23;
     *      float lowpass1 = d["lowpass"];
     *
     *      Dict d2("lowpass", 12.23);
     */
	class Dict
	{
	public:
		Dict()
		{
		}

		Dict(string key1, EMObject val1)
		{
			dict[key1] = val1;
		}

		Dict(string key1, EMObject val1, string key2, EMObject val2) {
			dict[key1] = val1;
			dict[key2] = val2;
		}

		Dict(string key1, EMObject val1, string key2, EMObject val2, string key3, EMObject val3) {
			dict[key1] = val1;
			dict[key2] = val2;
			dict[key3] = val3;
		}

		Dict(string key1, EMObject val1, string key2, EMObject val2,
			 string key3, EMObject val3, string key4, EMObject val4) {
			dict[key1] = val1;
			dict[key2] = val2;
			dict[key3] = val3;
			dict[key4] = val4;
		}


		Dict(const map < string, EMObject > &d)
		{
			map < string, EMObject >::const_iterator p;
			for (p = d.begin(); p != d.end(); p++) {
				dict[p->first] = p->second;
			}
		}

		~Dict() {
		}

		vector < string > keys()const
		{
			vector < string > result;

			map < string, EMObject >::const_iterator p;
			for (p = dict.begin(); p != dict.end(); p++) {
				result.push_back(p->first);
			}

			return result;
		}

		vector < EMObject > values()const
		{
			vector < EMObject > result;

			map < string, EMObject >::const_iterator p;
			for (p = dict.begin(); p != dict.end(); p++) {
				result.push_back(p->second);
			}

			return result;
		}

		bool has_key(const string & key) const
		{
			map < string, EMObject >::const_iterator p = dict.find(key);
			if (p != dict.end()) {
				return true;
			}
			return false;
		}

		size_t size() const
		{
			return dict.size();
		}

		EMObject get(const string & key)
		{
			return dict[key];
		}

		void put(string key, EMObject val)
		{
			dict[key] = val;
		}

		EMData *set_default(const string & key, EMData * val)
		{
			if (!has_key(key)) {
				dict[key] = EMObject(val);
			}
			return dict[key];
		}

		int set_default(const string & key, int val)
		{
			if (!has_key(key)) {
				dict[key] = val;
			}
			return dict[key];
		}

		float set_default(const string & key, float val)
		{
			if (!has_key(key)) {
				dict[key] = val;
			}
			return dict[key];
		}

		double set_default(const string & key, double val)
		{
			if (!has_key(key)) {
				dict[key] = val;
			}
			return dict[key];
		}

		map < string, EMObject > &get_dict() {
			return dict;
		}

		map < string, EMObject > get_dict()const
		{
			return dict;
		}

		EMObject & operator[] (const string & key)
		{
			return dict[key];
		}

		EMObject operator[] (const string & key) const
		{
			return dict[key];
		}

	private:
		mutable map < string, EMObject > dict;
	};

	/** TypeDict is a dictionary to store <string, EMObject::ObjectType> pair.
     * It is mainly used to store filter-like class's parameter
     * information: <parameter-name, parameter-type>.
     * Typical usage of this class:
     *
     *    TypeDict d;
     *    d.put("with", EMObject::EMDATA);
     *    d.put("lowpass", EMObject::FLOAT);
     *
     *    string lowpass_type = d["lowpass"];
     */
	class TypeDict
	{
	public:
		TypeDict()
		{
		}

		~TypeDict()
		{
		}

		vector < string > keys()const
		{
			vector < string > result;
			map < string, string >::const_iterator p;

			for (p = dict.begin(); p != dict.end(); p++) {
				result.push_back(p->first);
			}

			return result;
		}

		size_t size() const
		{
			return dict.size();
		}

		void put(string key, EMObject::ObjectType o)
		{
			dict[key] = EMObject::get_object_type_name(o);
		}

		string get(string key)
		{
			return dict[key];
		}

		string operator[] (const string & key)
		{
			return dict[key];
		}

		void dump() const;

	private:
		map < string, string > dict;
	};

	class NotExistingObjectError : public Exception {
	public:
		NotExistingObjectError(const string& file = "unknown", int line = 0,
							   const string& desc = "none")
			: Exception(file, line, desc) {}
	};
	
	/** Factory is used to store objects to create new instances.
     * It is a singleton template. Typical usages are as follows:
     *
     *   1. How to define a new factory (e.g. Filter Factory):
     *   
     *      template<> Factory<Filter>::Factory()
     *      {
     *         force_add(&AbsoluateValueFilter::NEW);
     *         force_add(&BooleanFilter::NEW);
     *      }
     *
     *
     *   2. How to use a Factory (e.g. Filter Factory):
     *
     *      Factory<Filter> *factory = Factory<Filter>::instance();
     *	    Filter *f = Factory<Filter>.get("AbsoluateValue");
     *      Filter *f2 = Factory<Filter>.get("GaussLowpass", Dict("lowpass", EMObject(12));
     */
	template < class T > class Factory {
	public:
		typedef T *(*InstanceType) ();

		static void add(InstanceType i);
		static T *get(string instance_name) throw (NotExistingObjectError);
		static T *get(string instance_name, const Dict & params) throw (NotExistingObjectError);
		static vector < string > get_list();

	private:
		Factory();
		Factory(const Factory < T > &);
		~Factory();
		static void init();
		void force_add(InstanceType i);

		static Factory < T > *my_instance;
		map < string, InstanceType > my_dict;
	};

	template < class T > Factory < T > *Factory < T >::my_instance = 0;

	template < class T > void Factory < T >::init()
	{
		if (!my_instance) {
			my_instance = new Factory < T > ();
		}
	}

	template < class T > void Factory < T >::force_add(InstanceType new_instance)
	{
		T *i = new_instance();
		string name = i->get_name();
		my_dict[name] = new_instance;
		delete i;
		i = 0;
	}


	template < class T > void Factory < T >::add(InstanceType new_instance)
	{
		init();

		T *i = new_instance();
		string name = i->get_name();
		typename map < string, InstanceType >::iterator fi = my_instance->my_dict.find(name);

		if (fi == my_instance->my_dict.end()) {
			my_instance->my_dict[name] = new_instance;
		}
		delete i;
		i = 0;
	}

	template < class T > T * Factory < T >::get(string instancename) throw(NotExistingObjectError){
		init();
		typename map < string, InstanceType >::iterator fi =
			my_instance->my_dict.find(instancename);
		if (fi != my_instance->my_dict.end()) {
			return my_instance->my_dict[instancename] ();
		}
		
		string desc = string("No such an instance existing: ") + instancename;
		throw NotExistingObjectError(__FILE__, __LINE__, desc);

		return 0;
	}

	template < class T > T * Factory < T >::get(string instancename, const Dict & params)
		throw(NotExistingObjectError) {
		init();

		typename map < string, InstanceType >::iterator fi =
			my_instance->my_dict.find(instancename);

		if (fi != my_instance->my_dict.end()) {
			T *i = my_instance->my_dict[instancename] ();
			i->set_params(params);
			return i;
		}		

		string desc = string("No such an instance existing: ") + instancename;
		throw NotExistingObjectError(__FILE__, __LINE__, desc);

		return 0;
	}

	template < class T > vector < string > Factory < T >::get_list() {
		init();
		vector < string > result;
		typename map < string, InstanceType >::const_iterator p;
		for (p = my_instance->my_dict.begin(); p != my_instance->my_dict.end(); p++) {
			result.push_back(p->first);
		}

		return result;
	}

	template < class T > void dump_factory()
	{
		vector < string > item_names = Factory < T >::get_list();

		for (size_t i = 0; i < item_names.size(); i++) {
			T *item = Factory < T >::get(item_names[i]);
			printf("%s\n", item->get_name().c_str());
			TypeDict td = item->get_param_types();
			td.dump();
		}
	}

}

#endif
