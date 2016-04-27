#ifndef __IMTEX_H
#define __IMTEX_H

// Example plugin
//
// This is an example plugin that demonstrates how to add functionality
// to Misfit Model 3D.  This specific example is a filter for texture
// images.  This filter uses ImLib2 to load images formats that Qt doesn't 
// support.
//
// The general approach to writing a plugin is to derive a class from
// a base object that Misfit Model 3D is famliar with--such as a model
// filter, texture filter, tool, or command.  The plugin creates a new
// object and registers that object with the appropriate manager when
// it is initialized (or registers an object creation function).
//
// The plugin must implement five functions:
//
//    PLUGIN_API bool plugin_init()
//       Responsible for initializing the plugin.  Usually it creates
//       a filter or command object and registers it with the
//       appropraite manager class
//
//       Should return true if properly initialized.  If this function
//       returns false, the plugin initialization is assumed to have
//       failed and plugin_uninit() will never be called.
//
//    PLUGIN_API bool plugin_uninit()
//       Responsible for cleanup.  The plugin does not need to
//       delete any registered objects.  This function is necessary
//       to perform any additional cleanup required.
//
//       Should return true if properly shut down
//
//    PLUGIN_API const char * plugin_version()
//       Returns a string with the plugin version number
//       Plugins generally have a major number, minor number, and
//       patch number (for example, "1.2.0")
//
//    PLUGIN_API const char * plugin_mm3d_version()
//       Returns a string that indicates the mm3d version
//       the plugin was compiled for.  The format is 
//       "[MAJOR].[MINOR].[PATCH]"
//       You should just be able to include version.h and return
//       the VERSION constant.
//
//    PLUGIN_API const char * plugin_desc()
//       Returns a string that descibes what the plugin does
//
// For more information about impelementing specific plugin types see the 
// following files in the src directory of the source distribution.
//
// Model filters:
//    filtermgr.h
//    modelfilter.h
//
// Image filters:
//    texmgr.h
//
// Commands:
//    cmdmgr.h
//    command.h
//
// Tools:
//    toolbox.h
//    tool.h
//
//

#include "texmgr.h"

class ImlibTextureFilter : public TextureFilter
{
   public:
      // constructor and destructor
      ImlibTextureFilter();
      virtual ~ImlibTextureFilter();
      
      // TextureFilter functions that we must implement

      // Since we're a plugin we need to delete the filter
      // we created, so we have to override release()
      void release();

      // List of image types we can read (extensions)
      std::list< std::string > getReadTypes();

      // List of image types we can write (extensions)
      std::list< std::string > getWriteTypes();

      // Function to read contents of file and initialize texture object
      Texture::ErrorE readFile( Texture * texture, const char * filename );

      // Indicate if we can read 'filename'
      bool canRead( const char * filename );

   protected:
      std::list< std::string > m_read;
      std::list< std::string > m_write;
};

#endif // __IMTEX_H
