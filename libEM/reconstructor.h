/**
 * $Id$
 */
#ifndef eman_reconstructor_h__
#define eman_reconstructor_h__ 1

#include "emobject.h"
#include <vector>
#include <map>
#include <string>
#include <math.h>

using std::vector;
using std::map;
using std::string;

namespace EMAN
{

	class EMData;
	class Rotation;


	/** Reconstructor class defines a way to do 3D recontruction.
	 * A reconstruction is done by 3 steps: 1) set up; 2) insert all
	 * image slices; 3) finish up. The last step will return the result.
	 * 
	 * Reconstructor class is the base class for all reconstructors.
     * Each specific Reconstructor class has a unique ID name. This name
     * is used to create a Reconstructor instance or do a reconstruction.
     *
	 * All Reconstructor classes in EMAN are managed by a Factory
	 * pattern. So each Reconstructor class must define:
	 *   a) a unique name to idenfity itself in the factory.
	 *   b) a static method to register itself in the factory.
	 *
     * Typical usages of Reconstructors are as follows:
     * 
     * 1. How to get all the Reconstructor names:
     * 
     *    vector<string> all_reconstructors = Factory<Reconstructor>::get_list();
     * 
     * 2. How to use a Reconstructor
     * 
     *    Reconstructor* r = Factory<Reconstructor>::get("Fourier");
     *    r->setup();
     *    r->insert_slice(slice1, euler1);
     *    insert more
     *    EMData* result = r->finish();
     * 
     * 3. How to define a new Reconstructor type
     *
     *    A new XYZReconstructor class must implement the following functions:
     *    (Please replace 'XYZ' with your own class name).
	 *
     *        void setup();
     *        int insert_slice(EMData * slice, const Rotation & euler);
     *        EMData * finish();
     *        string get_name() const { return "XYZ"; }
     *        static Reconstructor *NEW() { return new XYZReconstructor(); }
     *        TypeDict get_param_types() const;
     */
	class Reconstructor
	{
	  public:
		virtual ~ Reconstructor()
		{
		}

		/** Initialize the reconstructor.
		 * @return 0 if OK. 1 if error.
		 */
		virtual void setup() = 0;

		/** Insert an image slice to the reconstructor. To insert multiple
		 * image slices, call this function multiple times.
		 *
		 * @param slice: Image slice.
		 * @euler: Euler angle of this image slice.
		 * @return: 0 if OK. 1 if error.
		 */
		virtual int insert_slice(EMData * slice, const Rotation & euler) = 0;

		/** Finish reconstruction and return the complete model.
		 * @return The result 3D model.
		 */
		virtual EMData *finish() = 0;

		/** Get the Reconstructor's name. Each Reconstructor is
		 * identified by a unique name.
		 * @return The Reconstructor's name.
		 */
		virtual string get_name() const = 0;

		
		/** Get the Reconstructor's parameters in a key/value dictionary.
		 * @return A key/value pair dictionary containing the parameters.
		 */
		virtual Dict get_params() const
		{
			return params;
		}

		/** Set the Reconstructor's parameters using a key/value dictionary.
		 * @param new_params A dictionary containing the new parameters.
		 */
		virtual void set_params(const Dict & new_params)
		{
			params = new_params;
		}

		/** Get reconstructor parameter information in a dictionary. 
		 * Each parameter has one record in the dictionary. Each 
		 * record contains its name, data-type, and description.
		 *
		 * @return A dictionary containing the parameter info.
		 */	 
		virtual TypeDict get_param_types() const = 0;

	  protected:
		mutable Dict params;
	};

	/** Fourier space 3D reconstruction
     */
	class FourierReconstructor:public Reconstructor
	{
	  public:
		FourierReconstructor();
		~FourierReconstructor();

		void setup();
		int insert_slice(EMData * slice, const Rotation & euler);
		EMData *finish();

		string get_name() const
		{
			return "Fourier";
		}
		static Reconstructor *NEW()
		{
			return new FourierReconstructor();
		}

		TypeDict get_param_types() const
		{
			TypeDict d;
			d.put("size", EMObject::INT);
			d.put("mode", EMObject::INT);
			d.put("weight", EMObject::FLOAT);
			d.put("dlog", EMObject::INT);
			return d;
		}

	  private:
		EMData * image;
		int nx;
		int ny;
		int nz;
	};

	/** Fourier space 3D reconstruction with slices already Wiener filtered.
     */
	class WienerFourierReconstructor:public Reconstructor
	{
	  public:
		WienerFourierReconstructor();
		~WienerFourierReconstructor();

		void setup();
		int insert_slice(EMData * slice, const Rotation & euler);
		EMData *finish();

		string get_name() const
		{
			return "WienerFourier";
		}
		static Reconstructor *NEW()
		{
			return new WienerFourierReconstructor();
		}

		TypeDict get_param_types() const
		{
			TypeDict d;
			d.put("size", EMObject::INT);
			d.put("mode", EMObject::INT);
			d.put("padratio", EMObject::FLOAT);
			d.put("snr", EMObject::FLOATARRAY);
			return d;
		}

	  private:
		EMData * image;
		int nx;
		int ny;
		int nz;
	};

	/** Real space 3D reconstruction using back projection.
     * 
     * Back-projection is a method of 3D reconstruction from 2D
     * projections. It is based on superposing 3D functions
     * ("back-projection bodies") obtained by translating the
     * 2D projections along the directions of projection. 
     */
	class BackProjectionReconstructor:public Reconstructor
	{
	  public:
		BackProjectionReconstructor();
		~BackProjectionReconstructor();

		void setup();
		int insert_slice(EMData * slice, const Rotation & euler);
		EMData *finish();

		string get_name() const
		{
			return "BackProjection";
		}
		static Reconstructor *NEW()
		{
			return new BackProjectionReconstructor();
		}

		TypeDict get_param_types() const
		{
			TypeDict d;
			d.put("size", EMObject::INT);
			d.put("weight", EMObject::FLOAT);
			return d;
		}

	  private:
		EMData * image;
		int nx;
		int ny;
		int nz;
	};

	template <> Factory < Reconstructor >::Factory();

	void dump_reconstructors();
}

#endif
