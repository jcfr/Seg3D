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

#include <Core/Interface/Interface.h>
#include <Core/State/Actions/ActionSet.h>

#include <QtUtils/Bridge/detail/QtSpinBoxConnector.h>

namespace QtUtils
{

QtSpinBoxConnector::QtSpinBoxConnector( QSpinBox* parent, 
  Core::StateIntHandle& state, bool blocking ) :
  QtConnectorBase( parent, blocking ),
  spinbox_( parent ),
  state_( state )
{
  QPointer< QtSpinBoxConnector > qpointer( this );
  
  {
    Core::StateEngine::lock_type lock( Core::StateEngine::GetMutex() );
    parent->setValue( state->get() );
  }

  this->connect( parent, SIGNAL( valueChanged( int ) ), SLOT( set_state( int ) ) );
  this->add_connection( state->value_changed_signal_.connect(
    boost::bind( &QtSpinBoxConnector::SetSpinBoxValue, qpointer, _1, _2 ) ) );
}

QtSpinBoxConnector::QtSpinBoxConnector( QDoubleSpinBox* parent, 
  Core::StateDoubleHandle& state, bool blocking ) :
  QtConnectorBase( parent, blocking ),
  double_spinbox_( parent ),
  state_( state )
{
  QPointer< QtSpinBoxConnector > qpointer( this );
  
  {
    Core::StateEngine::lock_type lock( Core::StateEngine::GetMutex() );
    parent->setValue( state->get() );
  }

  this->connect( parent, SIGNAL( valueChanged( double ) ), SLOT( set_state( double ) ) );
  this->add_connection( state->value_changed_signal_.connect(
    boost::bind( &QtSpinBoxConnector::SetDoubleSpinBoxValue, qpointer, _1, _2 ) ) );
}

QtSpinBoxConnector::~QtSpinBoxConnector()
{
  this->disconnect_all();
}

void QtSpinBoxConnector::SetSpinBoxValue( QPointer< QtSpinBoxConnector > qpointer,
    int val, Core::ActionSource source )
{
  if ( source == Core::ActionSource::INTERFACE_WIDGET_E )
  {
    return;
  }

  if ( !Core::Interface::IsInterfaceThread() )
  {
    Core::Interface::PostEvent( boost::bind( &QtSpinBoxConnector::SetSpinBoxValue,
      qpointer, val, source ) );
    return;
  }

  if ( qpointer.isNull() )
  {
    return;
  }

  qpointer->block();
  qpointer->spinbox_->setValue( val );
  qpointer->unblock();
}

void QtSpinBoxConnector::SetDoubleSpinBoxValue( QPointer< QtSpinBoxConnector > qpointer,
    double val, Core::ActionSource source )
{
  if ( source == Core::ActionSource::INTERFACE_WIDGET_E )
  {
    return;
  }

  if ( !Core::Interface::IsInterfaceThread() )
  {
    Core::Interface::PostEvent( boost::bind( &QtSpinBoxConnector::SetDoubleSpinBoxValue,
      qpointer, val, source ) );
    return;
  }

  if ( qpointer.isNull() )
  {
    return;
  }

  qpointer->block();
  qpointer->double_spinbox_->setValue( val );
  qpointer->unblock();
}

void QtSpinBoxConnector::set_state( int val )
{
  if( !this->is_blocked() )
  {
    Core::ActionSet::Dispatch( this->state_, val );
  }
}

void QtSpinBoxConnector::set_state( double val )
{
  if( !this->is_blocked() )
  {
    Core::ActionSet::Dispatch( this->state_, val );
  }
}
  
} // end namespace QtUtils