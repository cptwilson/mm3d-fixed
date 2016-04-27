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


#include "ad3dsfilter.h"

#include "model.h"
#include "texture.h"
#include "log.h"
#include "binutil.h"
#include "misc.h"
#include "filtermgr.h"
#include "pluginapi.h"
#include "version.h"

#include "lib3ds/node.h"
#include "lib3ds/material.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <vector>

using std::list;
using std::string;

static A3dsFilter * s_filter = NULL;

static int _nodes = 0;
static int _meshes = 0;
static int _triangles = 0;

bool A3dsFilter::loadNode( Lib3dsNode * node )
{
   Lib3dsNode * child = node->childs;
   while ( child != NULL )
   {
      loadNode( child );
      child = child->next;
   }

   if (node->type==LIB3DS_OBJECT_NODE) {
      _nodes++;

      log_debug( "node name = %s\n", node->name );
      log_debug( "node instance = %s\n", node->data.object.instance );
      for ( unsigned i = 0; i < 4; i++ )
      {
         log_debug( "  %.2f %.2f %.2f %.2f\n", 
               node->matrix[i][0],
               node->matrix[i][1],
               node->matrix[i][2],
               node->matrix[i][3] );
      }
      log_debug( "  pivot: %.2f %.2f %.2f\n", 
            node->data.object.pivot[0],
            node->data.object.pivot[1],
            node->data.object.pivot[2] );
      log_debug( "  pos: %.2f %.2f %.2f\n", 
            node->data.object.pos[0],
            node->data.object.pos[1],
            node->data.object.pos[2] );
      log_debug( "  rot: %.2f %.2f %.2f\n", 
            node->data.object.rot[0],
            node->data.object.rot[1],
            node->data.object.rot[2] );
      log_debug( "  bbox_min: %.2f %.2f %.2f\n", 
            node->data.object.bbox_min[0],
            node->data.object.bbox_min[1],
            node->data.object.bbox_min[2] );
      log_debug( "  bbox_max: %.2f %.2f %.2f\n", 
            node->data.object.bbox_max[0],
            node->data.object.bbox_max[1],
            node->data.object.bbox_max[2] );

      if (strcmp(node->name,"$$$DUMMY")==0) {
         return true;
      }

      /*
      if (!node->user.d) {
         Lib3dsMesh *mesh=lib3ds_file_mesh_by_name(file, node->name);
         if (!mesh) {
            return false;
         }

         _meshes++;

         {
            unsigned p;

            for ( p=0; p<mesh->faces; ++p)
            {
               _triangles++;
               //Lib3dsFace * face = &(mesh->faceL[p]);

               //glVertex3fv(mesh->pointL[face->points[i]].pos);
            }
         }
      }
      */
   }

   return true;
}

bool A3dsFilter::loadMesh( Lib3dsMesh * mesh )
{
   if (!mesh) {
      return false;
   }

   log_debug( "mesh name = %s\n", mesh->name );
   for ( unsigned i = 0; i < 4; i++ )
   {
      log_debug( "  %.2f %.2f %.2f %.2f\n", 
            mesh->matrix[i][0],
            mesh->matrix[i][1],
            mesh->matrix[i][2],
            mesh->matrix[i][3] );
   }

   unsigned vertBase = m_model->getVertexCount();

   for ( unsigned v = 0; v < mesh->points; v++ )
   {
      m_model->addVertex( mesh->pointL[v].pos[0], 
            mesh->pointL[v].pos[1], mesh->pointL[v].pos[2] );
   }

   if ( mesh->faces > 0 )
   {
      int material = m_model->getMaterialByName( mesh->faceL[0].material );
      m_model->setGroupTextureId( m_curGroup, material );

      for ( unsigned p = 0; p < mesh->faces; p++ )
      {
         _triangles++;
         Lib3dsFace * face = &(mesh->faceL[p]);

         int tri = m_model->addTriangle( face->points[0] + vertBase, 
               face->points[1] + vertBase, face->points[2] + vertBase );
         m_model->addTriangleToGroup( m_curGroup, tri );

         if ( face->points[0] < mesh->texels && face->points[1] < mesh->texels
               && face->points[2] < mesh->texels )
         {
            m_model->setTextureCoords( tri, 0, mesh->texelL[ face->points[0] ][0], 
//                  1.0f - mesh->texelL[ face->points[0] ][1] );
                  mesh->texelL[ face->points[0] ][1] );
            m_model->setTextureCoords( tri, 1, mesh->texelL[ face->points[1] ][0], 
//                  1.0f - mesh->texelL[ face->points[1] ][1] );
                  mesh->texelL[ face->points[1] ][1] );
            m_model->setTextureCoords( tri, 2, mesh->texelL[ face->points[2] ][0], 
//                  1.0f - mesh->texelL[ face->points[2] ][1] );
                  mesh->texelL[ face->points[2] ][1] );
         }

         //glVertex3fv(mesh->pointL[face->points[i]].pos);
      }
   }

   return true;
}

A3dsFilter::A3dsFilter()
{
}

A3dsFilter::~A3dsFilter()
{
}

void A3dsFilter::release()
{
   delete this;
}

Model::ModelErrorE A3dsFilter::readFile( Model * model, const char * const filename )
{
   /*
   if ( filename == NULL || filename[0] == '\0' )
   {
      return Model::ERROR_BAD_ARGUMENT;
   }

   FILE * fp = fopen( filename, "rb" );

   if ( fp == NULL )
   {
      switch ( errno )
      {
         case EACCES:
         case EPERM:
            return Model::ERROR_ACCESS_DENIED;
         case ENOENT:
            return Model::ERROR_NO_FILE;
         case EISDIR:
            return Model::ERROR_BAD_DATA;
         default:
            return Model::ERROR_FILE_OPEN;
      }
   }

   string modelPath = "";
   string modelBaseName = "";
   string modelFullName = "";

   normalizePath( filename, modelFullName, modelPath, modelBaseName );

   model->setFilename( modelFullName.c_str() );
      
   fseek( fp, 0, SEEK_END );
   unsigned fileLength = ftell( fp );
   unsigned readLength = fileLength;
   fseek( fp, 0, SEEK_SET );

   uint8_t * fileBuf = new uint8_t[fileLength];
   uint8_t * bufPos = fileBuf;

   fread( fileBuf, fileLength, 1, fp );
   fclose( fp );

   delete[] fileBuf;
   */

   m_model = model;
   m_file = lib3ds_file_load( filename );

   if ( m_file )
   {
      string modelPath = "";
      string modelBaseName = "";
      string modelFullName = "";

      normalizePath( filename, modelFullName, modelPath, modelBaseName );

      model->setFilename( modelFullName.c_str() );
      
      vector<Model::Material *> & modelMaterials = getMaterialList( model );

      m_curGroup = -1;

      _nodes = 0;
      _meshes = 0;
      _triangles = 0;

      Lib3dsMaterial * material = m_file->materials;
      while ( material != NULL )
      {
         log_debug( "%s => %s\n", material->name, material->texture1_map.name );

         Model::Material * mat = Model::Material::get();
         mat->m_name = material->name;

         for ( unsigned i = 0; i < 4; i++ )
         {
            mat->m_ambient[i]  = 0.0f; //material->ambient[i];
            mat->m_diffuse[i]  = material->diffuse[i];
            mat->m_specular[i] = material->specular[i];
            mat->m_emissive[i] = 0.0f;
         }
         mat->m_ambient[3]  = 1.0f;
         mat->m_emissive[3] = 1.0f;
         mat->m_shininess = material->shininess;

         if ( material->texture1_map.name[0] )
         {
            mat->m_type = Model::Material::MATTYPE_TEXTURE;
            // Get absolute path for texture and alpha map
            char * tempFile = strdup( material->texture1_map.name );
            replaceBackslash( tempFile );
            string texturePath = tempFile;
            texturePath = fixAbsolutePath( modelPath.c_str(), texturePath.c_str() );
            texturePath = getAbsolutePath( modelPath.c_str(), texturePath.c_str() );
            mat->m_filename  = texturePath;
            free( tempFile );

            tempFile = strdup( material->opacity_map.name );
            replaceBackslash( tempFile );
            texturePath = tempFile;
            if ( texturePath.length() > 0 )
            {
               texturePath = fixAbsolutePath( modelPath.c_str(), texturePath.c_str() );
               texturePath = getAbsolutePath( modelPath.c_str(), texturePath.c_str() );
            }
            mat->m_alphaFilename = texturePath;
            free( tempFile );
         }
         else
         {
            mat->m_type = Model::Material::MATTYPE_COLOR;
            mat->m_filename = "";
            mat->m_alphaFilename = "";

            for ( unsigned i = 0; i < 4; i++ )
            {
               mat->m_color[i][0] = 255;
               mat->m_color[i][1] = 255;
               mat->m_color[i][2] = 255;
               mat->m_color[i][3] = 255;
            }
         }

         modelMaterials.push_back( mat );

         material = material->next;
      }
      Lib3dsNode * node = m_file->nodes;
      while ( node != NULL )
      {
         loadNode( node );
         node = node->next;
      }
      Lib3dsMesh * mesh = m_file->meshes;
      while ( mesh != NULL )
      {
         _meshes++;
         m_curGroup = m_model->addGroup( mesh->name );
         loadMesh( mesh );
         log_debug( "mesh points & texels  %d & %d\n", mesh->points, mesh->texels );
         mesh = mesh->next;
      }

      log_debug( "%d nodes\n", _nodes );
      log_debug( "%d meshes\n", _meshes );
      log_debug( "%d triangles\n", _triangles );

      lib3ds_file_free( m_file );

      model->setupJoints();
   }
   return Model::ERROR_NONE;
}

Model::ModelErrorE A3dsFilter::writeFile( Model * model, const char * const filename, ModelFilter::Options * )
{
   return Model::ERROR_UNSUPPORTED_OPERATION;
}

bool A3dsFilter::canRead( const char * filename )
{
   log_debug( "canRead( %s )\n", filename );
   log_debug( "  true\n" );
   return true;
}

bool A3dsFilter::canWrite( const char * filename )
{
   log_debug( "canWrite( %s )\n", filename );
   log_debug( "  false\n" );
   return false;
}

bool A3dsFilter::canExport( const char * filename )
{
   log_debug( "canWrite( %s )\n", filename );
   log_debug( "  false\n" );
   return false;
}

bool A3dsFilter::isSupported( const char * filename )
{
   log_debug( "isSupported( %s )\n", filename );
   unsigned len = strlen( filename );

   if ( len >= 4 && strcasecmp( &filename[len-4], ".3ds" ) == 0 )
   {
      log_debug( "  true\n" );
      return true;
   }
   else
   {
      log_debug( "  false\n" );
      return false;
   }
}

list< string > A3dsFilter::getReadTypes()
{
   list<string> rval;
   rval.push_back( "*.3ds" );
   return rval;
}

list< string > A3dsFilter::getWriteTypes()
{
   list<string> rval;
   //rval.push_back( "*.3ds" );
   return rval;
}

#ifdef PLUGIN

//------------------------------------------------------------------
// Plugin functions
//------------------------------------------------------------------

PLUGIN_API bool plugin_init()
{
   if ( s_filter == NULL )
   {
      s_filter = new A3dsFilter();
      FilterManager * texmgr = FilterManager::getInstance();
      texmgr->registerFilter( s_filter );
   }
   log_debug( "3DS model filter plugin initialized\n" );
   return true;
}

// The filter manager will delete our registered filter.
// We have no other cleanup to do
PLUGIN_API bool plugin_uninit()
{
   s_filter = NULL; // FilterManager deletes filters
   log_debug( "3DS model filter plugin uninitialized\n" );
   return true;
}

PLUGIN_API const char * plugin_version()
{
   return "0.8.1";
}

PLUGIN_API const char * plugin_mm3d_version()
{
   return VERSION_STRING;
}

PLUGIN_API const char * plugin_desc()
{
   return "3DS model filter";
}

#endif // PLUGIN
