/***************************************************************************
    copyright            : (C) 2003 by Ismael Orenstein
    email                : orenstein@kde.org
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#ifndef TAGLIB_XINGHEADER_H
#define TAGLIB_XINGHEADER_H

#include "mpegheader.h"
#include "taglib_export.h"

namespace TagLib {

  class ByteVector;

  namespace MPEG {

    class File;

    //! An implementation of the Xing/VBRI headers

    /*!
     * This is a minimalistic implementation of the Xing/VBRI VBR headers.
     * Xing/VBRI headers are often added to VBR (variable bit rate) MP3 streams
     * to make it easy to compute the length and quality of a VBR stream.  Our
     * implementation is only concerned with the total size of the stream (so
     * that we can calculate the total playing time and the average bitrate).
     * It uses <a href="http://home.pcisys.net/~melanson/codecs/mp3extensions.txt">
     * this text</a> and the XMMS sources as references.
     */

    class TAGLIB_EXPORT XingHeader
    {
    public:
      /*!
       * The type of the VBR header.
       */
      enum HeaderType
      {
        /*!
         * Invalid header or no VBR header found.
         */
        Invalid = 0,

        /*!
         * Xing header.
         */
        Xing = 1,

        /*!
         * VBRI header.
         */
        VBRI = 2,
      };

      /*!
       * Parses an Xing/VBRI header based on \a data which contains the entire
       * first MPEG frame.
       */
      XingHeader(const ByteVector &data);

      /*!
       * Destroy this XingHeader instance.
       */
      virtual ~XingHeader();

      /*!
       * Returns true if the data was parsed properly and if there is a valid
       * Xing/VBRI header present.
       */
      bool isValid() const;

      /*!
       * Returns the total number of frames.
       */
      unsigned int totalFrames() const;

      /*!
       * Returns the total size of stream in bytes.
       */
      unsigned int totalSize() const;

      /*!
       * Returns the type of the VBR header.
       */
      HeaderType type() const;

      /*!
       * Returns the offset for the start of this Xing header, given the
       * version and channels of the frame
       *
       * \deprecated Always returns 0.
       */
      TAGLIB_DEPRECATED static int xingHeaderOffset(TagLib::MPEG::Header::Version v,
                                                    TagLib::MPEG::Header::ChannelMode c);

    private:
      XingHeader(const XingHeader &);
      XingHeader &operator=(const XingHeader &);

      void parse(const ByteVector &data);

      class XingHeaderPrivate;
      XingHeaderPrivate *d;
    };
  }
}

#endif
