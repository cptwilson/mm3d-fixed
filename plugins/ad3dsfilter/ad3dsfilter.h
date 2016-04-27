/*  Misfit Model 3D
 * 
 *  Copyright (c) 2004 Kevin Worcester
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, 
 *  USA.
 *
 *  See the COPYING file for full license text.
 */


#ifndef __AD3DSFILTER_H
#define __AD3DSFILTER_H

#include "modelfilter.h"

#include "lib3ds/file.h"
#include "lib3ds/mesh.h"

#include <stdint.h>


class A3dsFilter : public ModelFilter
{
   public:
      A3dsFilter();
      virtual ~A3dsFilter();

      virtual void release();

      Model::ModelErrorE readFile( Model * model, const char * const filename );
      Model::ModelErrorE writeFile( Model * model, const char * const filename, ModelFilter::Options * );

      bool canRead( const char * filename );
      bool canWrite( const char * filename );
      bool canExport( const char * filename );

      bool isSupported( const char * filename );

      std::list< std::string > getReadTypes();
      std::list< std::string > getWriteTypes();

   protected:
      bool loadNode( Lib3dsNode * node );
      bool loadMesh( Lib3dsMesh * mesh );

      Model      * m_model;
      Lib3dsFile * m_file;
      int          m_curGroup;
};

#endif // __AD3DSFILTER_H
