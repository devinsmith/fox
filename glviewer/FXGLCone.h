/********************************************************************************
*                                                                               *
*                        O p e n G L   C o n e    O b j e c t                   *
*                                                                               *
*********************************************************************************
* Copyright (C) 1999,2024 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or modify          *
* it under the terms of the GNU Lesser General Public License as published by   *
* the Free Software Foundation; either version 3 of the License, or             *
* (at your option) any later version.                                           *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 *
* GNU Lesser General Public License for more details.                           *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public License      *
* along with this program.  If not, see <http://www.gnu.org/licenses/>          *
********************************************************************************/
#ifndef FXGLCONE_H
#define FXGLCONE_H


/// OpenGL Cone Object
class FXGLCone : public FXGLShape {
  FXDECLARE(FXGLCone)
public:
  FXfloat height;
  FXfloat radius;
protected:
  FXGLCone();
  virtual void drawshape(FXGLViewer* viewer);
public:

  /// Construct with specified origin, height and radius
  FXGLCone(FXfloat x,FXfloat y,FXfloat z,FXfloat h=1.0f,FXfloat r=1.0f);

  /// Construct with specified origin, height, radius and material
  FXGLCone(FXfloat x,FXfloat y,FXfloat z,FXfloat h,FXfloat r,const FXMaterial& mtl);

  /// Copy constructor
  FXGLCone(const FXGLCone& orig);

  /// Copy this object
  virtual FXGLObject* copy();

  /// Change radius
  virtual void setRadius(FXfloat r);
  FXfloat getRadius() const { return radius; }

  /// Change height
  virtual void setHeight(FXfloat h);
  FXfloat getHeight() const { return height; }

  /// Save to a stream
  virtual void save(FXStream& store) const;

  /// Load from a stream
  virtual void load(FXStream& store);

  /// Destroy
  virtual ~FXGLCone();
  };


#endif
