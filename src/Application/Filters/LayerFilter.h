/*
 For more information, please see: http://software.sci.utah.edu

 The MIT License

 Copyright (c) 2009 Scientific Computing and Imaging Institute,
 University of Utah.


 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
 */
 
#ifndef APPLICATION_FILTERS_BASEFILTER_H 
#define APPLICATION_FILTERS_BASEFILTER_H 
 
// Boost includes
#include <boost/smart_ptr.hpp> 
#include <boost/utility.hpp> 
 
// Core includes
#include <Core/Utils/Runnable.h>
#include <Core/Volume/DataVolume.h>
#include <Core/Volume/MaskVolume.h>
#include <Core/DataBlock/MaskDataBlockManager.h> 

// Application includes
#include <Application/Layer/DataLayer.h> 
#include <Application/Layer/MaskLayer.h> 
#include <Application/Layer/LabelLayer.h> 
 
#include <Application/Layer/LayerAbstractFilter.h>

namespace Seg3D
{

// CLASS BASEFILTER:
// This class provides the basic underlying framework for running filters in a separate thread
// from the application thread. It provides a series of functions common to all the filters.

class LayerFilter;
typedef boost::shared_ptr<LayerFilter> LayerFilterHandle;
typedef boost::weak_ptr<LayerFilter>   LayerFilterWeakHandle;

class LayerFilterPrivate;
typedef boost::shared_ptr<LayerFilterPrivate> LayerFilterPrivateHandle;

class LayerFilter : public LayerAbstractFilter
{

public:
  LayerFilter();
  virtual ~LayerFilter();
    
  // -- abort handling -- 
public:   
  // RAISE_ABORT:
  // Raise the abort flag
  void raise_abort();
    
  // CHECK_ABORT:
  // Check the abort flag
  bool check_abort(); 
    
  // ABORT_AND_WAIT:  
  // NOTE: When undoing asynchronous layer operations, one may need to wait until the filter
  // can be aborted. If not the state of the program is unclear. Hence this function will ensure
  // that the filter has finished processing and that all locks are cleared.
  // NOTE: This function should be run on the application thread only
  virtual void abort_and_wait();  
    
  // CONNECT_ABORT:
  // Monitor the abort flag of a layer
  void connect_abort( const LayerHandle& layer ); 
    
  // -- shortcuts into the LayerManager --  
public:
  // FIND_LAYER:
  // Find a layer in the layer manager with this id
  bool find_layer( const std::string& layer_id, LayerHandle& layer );
  
  // LOCK_FOR_USE:
  // Lock a layer for usage, i.e. we need the data/mask volume but we will not change
  // the data.
  // NOTE: This function can only be run from the application thread
  // NOTE: The BaseFilter class records which layers are locked and will schedule an unlock
  // for each layer that was not unlocked by the time this class is destroyed.
  bool lock_for_use( LayerHandle layer );

  // LOCK_FOR_PROCESSING:
  // Lock a layer for processing, i.e. when we change the data contained in the data/mask of the
  // layer.
  // NOTE: This function can only be run from the application thread.
  // NOTE: The BaseFilter class records which layers are locked and will schedule an unlock
  // for each layer that was not unlocked by the time this class is destroyed.
  bool lock_for_processing( LayerHandle layer );
  
  // CREATE_AND_LOCK_DATA_LAYER_FROM_LAYER:
  // Create a new data layer with the same dimensions as another layer, the layer is immediately
  // locked as it does not contain any data and will be in the creating state.
  // NOTE: This function can only be run from the application thread
  bool create_and_lock_data_layer_from_layer( LayerHandle src_layer, LayerHandle& dst_layer );

  // CREATE_AND_LOCK_DATA_LAYER:
  // Create a new data layer with the given grid transform, the layer is immediately
  // locked as it does not contain any data and will be in the creating state.
  // NOTE: This function can only be run from the application thread
  bool create_and_lock_data_layer( const Core::GridTransform& grid_trans, 
    LayerHandle src_layer, LayerHandle& dst_layer );

  // CREATE_AND_LOCK_MASK_LAYER_FROM_LAYER:
  // Create a new mask layer with the same dimensions as another layer, the layer is immediately
  // locked as it does not contain any data and will be in the creating state.
  // NOTE: This function can only be run from the application thread.
  bool create_and_lock_mask_layer_from_layer( LayerHandle src_layer, LayerHandle& dst_layer );  
  
  // CREATE_AND_LOCK_MASK_LAYER:
  // Create a new mask layer with the given grid transform, the layer is immediately
  // locked as it does not contain any data and will be in the creating state.
  // NOTE: This function can only be run from the application thread
  bool create_and_lock_mask_layer( const Core::GridTransform& grid_trans, 
    LayerHandle src_layer, LayerHandle& dst_layer );

  // DISPATCH_UNLOCK_LAYER:
  // Schedule a layer to be unlocked.
  bool dispatch_unlock_layer( LayerHandle layer );

  // DISPATCH_DELETE_LAYER:
  // Schedule a layer to be deleted
  bool dispatch_delete_layer( LayerHandle layer );

  // TODO:
  // These next two functions need to atomically set the provenance information as well
  // This is currently not yet implemented
  // --JS
  
  // DISPATCH_INSERT_DATA_VOLUME_INTO_LAYER:
  // Schedule a new data volume to be inserted into a layer
  bool dispatch_insert_data_volume_into_layer( LayerHandle layer, 
    Core::DataVolumeHandle data, bool update_histogram );
    
  // DISPATCH_INSERT_MASK_VOLUME_INTO_LAYER:
  // Schedule a new mask volume to be inserted into a layer
  bool dispatch_insert_mask_volume_into_layer( LayerHandle layer, 
    Core::MaskVolumeHandle mask );

  // -- filter specific information --
public:
  // GET_FILTER_NAME:
  // This functions returns the name of the filter that is used in the error report.
  virtual std::string get_filter_name() const = 0;
  
  // GET_LAYER_PREFIX:
  // This function returns the name of the filter. The latter is prepended to the new layer name, 
  // when a new layer is generated. 
  virtual std::string get_layer_prefix() const = 0;

protected:
  // RUN_FILTER:
  // run the filter
  virtual void run_filter() = 0;

  // RUN:
  // The function called by runnable
  virtual void run();

  // -- error handling --
public:
  // REPORT_ERROR:
  // Report an error to the user
  void report_error( const std::string& error );

  // GET_KEY:
  // Get the unique filter key
  Layer::filter_key_type get_key()  const;

  // -- internals --
private:
  LayerFilterPrivateHandle private_;

};

} // end namespace Seg3D


#define SCI_BEGIN_TYPED_RUN( DATATYPE ) \
public:\
  virtual void run_filter()\
  {\
    switch ( DATATYPE )\
    {\
      case Core::DataType::CHAR_E: this->typed_run_filter<signed char>(); break;\
      case Core::DataType::UCHAR_E: this->typed_run_filter<unsigned char>(); break;\
      case Core::DataType::SHORT_E: this->typed_run_filter<short>(); break;\
      case Core::DataType::USHORT_E: this->typed_run_filter<unsigned short>(); break;\
      case Core::DataType::INT_E: this->typed_run_filter<int>(); break;\
      case Core::DataType::UINT_E: this->typed_run_filter<unsigned int>(); break;\
      case Core::DataType::FLOAT_E: this->typed_run_filter<float>(); break;\
      case Core::DataType::DOUBLE_E: this->typed_run_filter<double>(); break;\
    };\
  }\
\
  template< class VALUE_TYPE>\
  void typed_run_filter()\
  {\
  
#define SCI_END_TYPED_RUN() \
  }


#define SCI_BEGIN_RUN( ) \
public:\
  virtual void run_filter()\
  {\

#define SCI_END_RUN() \
  }
  

#endif