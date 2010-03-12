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

#ifndef APPLICATION_VIEWER_VIEWER_H
#define APPLICATION_VIEWER_VIEWER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

// STL includes
#include <map>
#include <vector>

// Boost includes 
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

// Application includes
#include <Application/State/State.h>
#include <Application/Viewer/ViewerRenderer.h>

#include <Utils/Core/EnumClass.h>
//#include <Utils/DataBlock/MaskDataSlice.h>

namespace Seg3D
{

// Enums for mouse buttons 
// they have the same values as corresponding Qt ones
SCI_ENUM_CLASS
(
  MouseButton,
  NO_BUTTON_E = 0x00000000,
  LEFT_BUTTON_E = 0x00000001,
  RIGHT_BUTTON_E = 0x00000002,
  MID_BUTTON_E = 0x00000004
)

// Enums for key modifiers
// they have the same values as corresponding Qt ones
SCI_ENUM_CLASS
(
  KeyModifier,
  NO_MODIFIER_E = 0x00000000,
  SHIFT_MODIFIER_E = 0x02000000,
  CONTROL_MODIFIER_E = 0x04000000,
  ALT_MODIFIER_E = 0x08000000
)

class MousePosition
{
public:
  MousePosition() :
    x( 0 ), y( 0 )
  {
  }

  int x;
  int y;
};

class MouseHistory
{
public:
  MousePosition left_start;
  MousePosition right_start;
  MousePosition mid_start;
  MousePosition previous;
  MousePosition current;
};

// Forward declarations
class ViewManipulator;
class Viewer;
typedef boost::shared_ptr< Viewer > ViewerHandle;

// Class definition
class Viewer : public StateHandler
{

  // -- constructor/destructor --
public:
  Viewer( const std::string& key );
  virtual ~Viewer();

  // -- mouse events handling --
public:

  void mouse_move_event( const MouseHistory& mouse_history, int button, int buttons,
      int modifiers );
  void mouse_press_event( const MouseHistory& mouse_history, int button, int buttons,
      int modifiers );
  void mouse_release_event( const MouseHistory& mouse_history, int button, int buttons,
      int modifiers );

  typedef boost::function< bool( const MouseHistory&, int, int, int ) > mouse_event_handler_type;

  void set_mouse_move_handler( mouse_event_handler_type func );
  void set_mouse_press_handler( mouse_event_handler_type func );
  void set_mouse_release_handler( mouse_event_handler_type func );
  void reset_mouse_handlers();

  void resize( int width, int height );
  bool is_volume_view() const;
  StateViewBaseHandle get_active_view_state();

  virtual void state_changed();
  typedef boost::signals2::signal< void() > redraw_signal_type;
  redraw_signal_type redraw_signal_;

private:
  mouse_event_handler_type mouse_move_handler_;
  mouse_event_handler_type mouse_press_handler_;
  mouse_event_handler_type mouse_release_handler_;

  boost::shared_ptr< ViewManipulator > view_manipulator_;

  // -- State information --
public:

  StateOptionHandle view_mode_state_;

  StateView2DHandle axial_view_state_;
  StateView2DHandle coronal_view_state_;
  StateView2DHandle sagittal_view_state_;
  StateView3DHandle volume_view_state_;

  StateRangedIntHandle axial_slice_number_state_;
  StateRangedIntHandle coronal_slice_number_state_;
  StateRangedIntHandle sagittal_slice_number_state_;

  StateBoolHandle slice_lock_state_;
  StateBoolHandle slice_grid_state_;
  StateBoolHandle slice_visible_state_;

  StateBoolHandle volume_lock_state_;
  StateBoolHandle volume_slices_visible_state_;
  StateBoolHandle volume_isosurfaces_visible_state_;
  StateBoolHandle volume_volume_rendering_visible_state_;

//  std::map<std::string, Utils::MaskDataSliceHandle> mask_slices_;

  const static std::string AXIAL_C;
  const static std::string SAGITTAL_C;
  const static std::string CORONAL_C;
  const static std::string VOLUME_C;

};

} // end namespace Seg3D

#endif