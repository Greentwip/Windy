/*
    Copyright 2007-2008 Christian Henning, Andreas Pokorny, Lubomir Bourdev
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_IO_READ_IMAGE_HPP
#define BOOST_GIL_EXTENSION_IO_READ_IMAGE_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief
/// \author Christian Henning, Andreas Pokorny, Lubomir Bourdev \n
///
/// \date   2007-2008 \n
///
////////////////////////////////////////////////////////////////////////////////////////

#include <boost/type_traits/is_base_and_derived.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/mpl/and.hpp>
#include <boost/utility/enable_if.hpp>

#include <boost/gil/extension/toolbox/dynamic_images.hpp>

#include "base.hpp"
#include "io_device.hpp"
#include "path_spec.hpp"
#include "conversion_policies.hpp"

namespace boost{ namespace gil {

/// \ingroup IO

/// \brief Reads an image without conversion. Image memory is allocated.
/// \param file      It's a device. Must satisfy is_input_device metafunction.
/// \param img       The image in which the data is read into. Must satisfy is_read_supported metafunction.
/// \param settings  Specifies read settings depending on the image format.
/// \throw std::ios_base::failure
template < typename Device
         , typename Image
         , typename FormatTag
         >
inline
void read_image( Device&                                 file
               , Image&                                  img
               , const image_read_settings< FormatTag >& settings
               , typename enable_if< mpl::and_< detail::is_input_device< Device    >
                                              , is_format_tag          < FormatTag >
                                              , is_read_supported      < typename get_pixel_type< typename Image::view_t >::type
                                                                       , FormatTag
                                                                       >
                                              >
                                   >::type* /* ptr */ = 0
               )
{
    reader< Device
          , FormatTag
          , read_and_no_convert
          > reader( file
                  , settings
                  );

    reader.init_image( img
                     , reader.get_info()
                     );

    reader.apply( view( img ));
}

/// \brief Reads an image without conversion. Image memory is allocated.
/// \param file      It's a device. Must satisfy is_adaptable_input_device metafunction.
/// \param img       The image in which the data is read into. Must satisfy is_read_supported metafunction.
/// \param settings  Specifies read settings depending on the image format.
/// \throw std::ios_base::failure
template < typename Device
         , typename Image
         , typename FormatTag
         >
inline
void read_image( Device&                                 file
               , Image&                                  img
               , const image_read_settings< FormatTag >& settings
               , typename enable_if< mpl::and_< detail::is_adaptable_input_device< FormatTag
                                                                                 , Device
                                                                                 >
                                              , is_format_tag< FormatTag >
                                              , is_read_supported< typename get_pixel_type< typename Image::view_t >::type
                                                                 , FormatTag
                                                                 >
                                              >
                                   >::type* /* ptr */ = 0
                      )
{
    typedef typename detail::is_adaptable_input_device< FormatTag
                                                      , Device
                                                      >::device_type device_type;

    device_type dev(file);
    reader< device_type
          , FormatTag
          , read_and_no_convert
          > reader( dev
                  , settings
                  );

    reader.init_image( img
                     , reader.get_info()
                     );

    reader.apply( view( img ));
}

/// \brief Reads an image without conversion. Image memory is allocated.
/// \param file_name File name. Must satisfy is_supported_path_spec metafunction.
/// \param img       The image in which the data is read into. Must satisfy is_read_supported metafunction.
/// \param settings  Specifies read settings depending on the image format.
/// \throw std::ios_base::failure
template < typename String
         , typename Image
         , typename FormatTag
         >
inline
void read_image( const String&                           file_name
               , Image&                                  img
               , const image_read_settings< FormatTag >& settings
               , typename enable_if< mpl::and_< detail::is_supported_path_spec< String >
                                              , is_format_tag< FormatTag >
                                              , is_read_supported< typename get_pixel_type< typename Image::view_t >::type
                                                                 , FormatTag
                                                                 >
                                              >
                                   >::type* /* ptr */ = 0
               )
{
    typedef typename detail::file_stream_device<FormatTag>::read_tag read_tag;

    detail::file_stream_device<FormatTag> device( detail::convert_to_string( file_name )
                                                , read_tag()
                                                );

    read_image( device
              , img
              , settings
              );
}

/// \brief Reads an image without conversion. Image memory is allocated.
/// \param file It's a device. Must satisfy is_input_device metafunction.
/// \param img  The image in which the data is read into. Must satisfy is_read_supported metafunction.
/// \param tag  Defines the image format. Must satisfy is_format_tag metafunction.     
/// \throw std::ios_base::failure
template < typename Device
         , typename Image
         , typename FormatTag
         >
inline
void read_image( Device&          file
               , Image&           img
               , const FormatTag&
               , typename enable_if< mpl::and_< detail::is_input_device< Device    >
                                              , is_format_tag          < FormatTag >
                                              , is_read_supported      < typename get_pixel_type< typename Image::view_t >::type
                                                                       , FormatTag
                                                                       >
                                              >
                                   >::type* /* ptr */ = 0
               )
{
    read_image( file
              , img
              , image_read_settings< FormatTag >()
              );
}

/// \brief Reads an image without conversion. Image memory is allocated.
/// \param file      It's a device. Must satisfy is_adaptable_input_device metafunction.
/// \param img       The image in which the data is read into. Must satisfy is_read_supported metafunction.
/// \param tag  Defines the image format. Must satisfy is_format_tag metafunction.
/// \throw std::ios_base::failure
template < typename Device
         , typename Image
         , typename FormatTag
         >
inline
void read_image( Device&          file
               , Image&           img
               , const FormatTag&
               , typename enable_if< mpl::and_< detail::is_adaptable_input_device< FormatTag
                                                                                 , Device
                                                                                 >
                                              , is_format_tag< FormatTag >
                                              , is_read_supported< typename get_pixel_type< typename Image::view_t >::type
                                                                 , FormatTag
                                                                 >
                                              >
                                   >::type* /* ptr */ = 0
               )
{
    typedef typename detail::is_adaptable_input_device< FormatTag, Device >::device_type device_type;
    device_type dev( file );

    read_image( dev
              , img
              , image_read_settings< FormatTag >()
              );
}

/// \brief Reads an image without conversion. Image memory is allocated.
/// \param file_name File name. Must satisfy is_supported_path_spec metafunction.
/// \param img       The image in which the data is read into. Must satisfy is_read_supported metafunction.
/// \param tag       Defines the image format. Must satisfy is_format_tag metafunction.
/// \throw std::ios_base::failure
template < typename String
         , typename Image
         , typename FormatTag
         >
inline
void read_image( const String&    file_name
               , Image&           img
               , const FormatTag&
               , typename enable_if< mpl::and_< detail::is_supported_path_spec< String >
                                              , is_format_tag< FormatTag >
                                              , is_read_supported< typename get_pixel_type< typename Image::view_t >::type
                                                                 , FormatTag
                                                                 >
                                              >
                                   >::type* /* ptr */ = 0
               )
{
    detail::file_stream_device< FormatTag > device( detail::convert_to_string( file_name )
                                                  , typename detail::file_stream_device< FormatTag >::read_tag()
                                                  );

    read_image( device
              , img
              , image_read_settings< FormatTag >()
              );
}

#ifdef BOOST_GIL_IO_ADD_FS_PATH_SUPPORT

/// \brief Reads an image without conversion. Image memory is allocated.
/// \param file_name File name. Must satisfy is_supported_path_spec metafunction.
/// \param img       The image in which the data is read into. Must satisfy is_read_supported metafunction.
/// \throw std::ios_base::failure
template < typename String
         , typename Image
         >
inline
void read_image( const String&    file_name
               , Image&           img
               , typename enable_if< detail::is_supported_path_spec< String >
                                   >::type* /* ptr */ = 0
               )
{
    namespace fs = boost::filesystem;
    namespace al = boost::algorithm;

    fs::path in_path = fs::system_complete( fs::path( file_name, fs::native ));
    
#ifdef BOOST_GIL_EXTENSION_IO_BMP_READ_ENABLED

    if( al::to_upper( fs::extension( in_path )) == ".BMP" )
    {
        read_image( file_name
                  , img
                  , bmp_tag()
                  );

        return;
    }

#endif // BOOST_GIL_EXTENSION_IO_BMP_READ_ENABLED


#ifdef BOOST_GIL_EXTENSION_IO_JPEG_READ_ENABLED

    if( fs::extension( in_path ) == ".JPG" )
    {
        read_image( file_name
                  , img
                  , jpeg_tag()
                  );

        return ;
    }

#endif // BOOST_GIL_EXTENSION_IO_JPEG_READ_ENABLED

#ifdef BOOST_GIL_EXTENSION_IO_PNG_READ_ENABLED

    if( fs::extension( in_path ) == ".PNG" )
    {
        read_image( file_name
                  , img
                  , png_tag()
                  );

        return ;
    }

#endif // BOOST_GIL_EXTENSION_IO_PNG_READ_ENABLED

#ifdef BOOST_GIL_EXTENSION_IO_PNM_READ_ENABLED

    if( fs::extension( in_path ) == ".PNM" )
    {
        read_image( file_name
                  , img
                  , pnm_tag()
                  );

        return ;
    }

#endif // BOOST_GIL_EXTENSION_IO_PNM_READ_ENABLED

#ifdef BOOST_GIL_EXTENSION_IO_TIFF_READ_ENABLED

    if( fs::extension( in_path ) == ".TIF" )
    {
        read_image( file_name
                  , img
                  , tiff_tag()
                  );

        return ;
    }

#endif // BOOST_GIL_EXTENSION_IO_JPEG_READ_ENABLED
}

#endif // BOOST_GIL_IO_ADD_FS_PATH_SUPPORT

///////////////////////////// dynamic images

/// \brief Reads an image without conversion. Image memory is allocated.
/// \param file      It's a device. Must satisfy is_input_device metafunction.
/// \param images    Dynamic image ( mpl::vector ). See boost::gil::dynamic_image extension.
/// \param settings  Specifies read settings depending on the image format.
template < typename Device
         , typename Images
         , typename FormatTag
         >
inline
void read_image( Device&                                 file
               , any_image< Images >&                    images
               , const image_read_settings< FormatTag >& settings
               , typename enable_if< mpl::and_< detail::is_input_device< Device    >
                                              , is_format_tag          < FormatTag >
                                              >
                                   >::type* /* ptr */ = 0
               )
{
    dynamic_image_reader< Device
                        , FormatTag
                        > dyn_reader( file
                                    , settings
                                    );

    dyn_reader.init_image( images
                         , dyn_reader.get_info()
                         );

    dyn_reader.apply( images );
}

/// \brief Reads an image without conversion. Image memory is allocated.
/// \param file      It's a device. Must satisfy is_adaptable_input_device metafunction.
/// \param images    Dynamic image ( mpl::vector ). See boost::gil::dynamic_image extension.
/// \param settings  Specifies read settings depending on the image format.
/// \throw std::ios_base::failure

template < typename Device
         , typename Images
         , typename FormatTag
         >
inline
void read_image( Device&                                 file
               , any_image< Images >&                    images
               , const image_read_settings< FormatTag >& settings
               , typename enable_if< mpl::and_< detail::is_adaptable_input_device< FormatTag
                                                                                 , Device
                                                                                 >
                                              , is_format_tag< FormatTag >
                                              >
                                   >::type* /* ptr */ = 0
                      )
{
    typedef typename detail::is_adaptable_input_device< FormatTag
                                                      , Device
                                                      >::device_type device_type;
    device_type dev( file );

    reader< device_type
          , FormatTag
          , read_and_no_convert
          > reader( dev
                  , settings
                  );

    reader.init_image( images
                     , reader.get_info
                     );

    reader.apply( view( images ));
}

/// \brief Reads an image without conversion. Image memory is allocated.
/// \param file_name File name. Must satisfy is_supported_path_spec metafunction.
/// \param images    Dynamic image ( mpl::vector ). See boost::gil::dynamic_image extension.
/// \param settings  Specifies read settings depending on the image format.
/// \throw std::ios_base::failure
template < typename String
         , typename Images
         , typename FormatTag
         >
inline
void read_image( const String&                           file_name
               , any_image< Images >&                    images
               , const image_read_settings< FormatTag >& settings
               , typename enable_if< mpl::and_< detail::is_supported_path_spec< String >
                                              , is_format_tag< FormatTag >
                                              >
                                   >::type* /* ptr */ = 0
               )
{
    typedef typename detail::file_stream_device< FormatTag >::read_tag read_tag;

    detail::file_stream_device<FormatTag> device( detail::convert_to_string( file_name )
                                                , read_tag()
                                                );

    read_image( device
              , images
              , settings
              );
}

/// \brief Reads an image without conversion. Image memory is allocated.
/// \param file   It's a device. Must satisfy is_input_device metafunction.
/// \param images Dynamic image ( mpl::vector ). See boost::gil::dynamic_image extension.
/// \param tag    Defines the image format. Must satisfy is_format_tag metafunction. 
/// \throw std::ios_base::failure
template < typename Device
         , typename Images
         , typename FormatTag
         >
inline
void read_image( Device&              file
               , any_image< Images >& images
               , const FormatTag&
               , typename enable_if< mpl::and_< detail::is_input_device< Device    >
                                              , is_format_tag          < FormatTag >
                                              >
                                   >::type* /* ptr */ = 0
               )
{
    read_image( file
              , images
              , image_read_settings< FormatTag >()
              );
}

/// \brief Reads an image without conversion. Image memory is allocated.
/// \param file   It's a device. Must satisfy is_adaptable_input_device metafunction.
/// \param images Dynamic image ( mpl::vector ). See boost::gil::dynamic_image extension.
/// \param tag    Defines the image format. Must satisfy is_format_tag metafunction. 
/// \throw std::ios_base::failure
template < typename Device
         , typename Images
         , typename FormatTag
         >
inline
void read_image( Device&              file
               , any_image< Images >& images
               , const FormatTag&
               , typename enable_if< mpl::and_< detail::is_adaptable_input_device< FormatTag
                                                                                 , Device
                                                                                 >
                                              , is_format_tag< FormatTag >
                                              >
                                   >::type* /* ptr */ = 0
               )
{
    typedef typename detail::is_adaptable_input_device< FormatTag
                                                      , Device
                                                      >::device_type device_type;
    device_type dev( file );

    read_image( dev
              , images
              , image_read_settings< FormatTag >()
              );
}

/// \brief Reads an image without conversion. Image memory is allocated.
/// \param file_name File name. Must satisfy is_supported_path_spec metafunction.
/// \param images    Dynamic image ( mpl::vector ). See boost::gil::dynamic_image extension.
/// \param tag       Defines the image format. Must satisfy is_format_tag metafunction. 
/// \throw std::ios_base::failure
template < typename String
         , typename Images
         , typename FormatTag
         >
inline
void read_image( const String&        file_name
               , any_image< Images >& images
               , const FormatTag&
               , typename enable_if< mpl::and_< detail::is_supported_path_spec< String >
                                              , is_format_tag< FormatTag >
                                              >
                                   >::type* /* ptr */ = 0
               )
{
    detail::file_stream_device< FormatTag > device( detail::convert_to_string( file_name )
                                                  , typename detail::file_stream_device< FormatTag >::read_tag()
                                                  );

    read_image( device
              , images
              , image_read_settings< FormatTag >()
              );
}

} // namespace gil
} // namespace boost

#endif // BOOST_GIL_EXTENSION_IO_READ_IMAGE_HPP
