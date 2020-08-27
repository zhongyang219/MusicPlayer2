/***************************************************************************
    copyright            : (C) 2003 by Allan Sandfeld Jensen
    email                : kde@carewolf.org
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

#ifndef TAGLIB_FLACPROPERTIES_H
#define TAGLIB_FLACPROPERTIES_H

#include "taglib_export.h"
#include "audioproperties.h"

namespace TagLib {

  namespace FLAC {

    class File;

    //! An implementation of audio property reading for FLAC

    /*!
     * This reads the data from an FLAC stream found in the AudioProperties
     * API.
     */

    class TAGLIB_EXPORT Properties : public AudioProperties
    {
    public:
      /*!
       * Create an instance of FLAC::Properties with the data read from the
       * ByteVector \a data.
       */
       // BIC: switch to const reference
      Properties(ByteVector data, long streamLength, ReadStyle style = Average);

      /*!
       * Create an instance of FLAC::Properties with the data read from the
       * FLAC::File \a file.
       */
       // BIC: remove
      Properties(File *file, ReadStyle style = Average);

      /*!
       * Destroys this FLAC::Properties instance.
       */
      virtual ~Properties();

      /*!
       * Returns the length of the file in seconds.  The length is rounded down to
       * the nearest whole second.
       *
       * \note This method is just an alias of lengthInSeconds().
       *
       * \deprecated
       */
      TAGLIB_DEPRECATED virtual int length() const;

      /*!
       * Returns the length of the file in seconds.  The length is rounded down to
       * the nearest whole second.
       *
       * \see lengthInMilliseconds()
       */
      // BIC: make virtual
      int lengthInSeconds() const;

      /*!
       * Returns the length of the file in milliseconds.
       *
       * \see lengthInSeconds()
       */
      // BIC: make virtual
      int lengthInMilliseconds() const;

      /*!
       * Returns the average bit rate of the file in kb/s.
       */
      virtual int bitrate() const;

      /*!
       * Returns the sample rate in Hz.
       */
      virtual int sampleRate() const;

      /*!
       * Returns the number of audio channels.
       */
      virtual int channels() const;

      /*!
       * Returns the number of bits per audio sample as read from the FLAC
       * identification header.
       */
      int bitsPerSample() const;

      /*!
       * Returns the sample width as read from the FLAC identification
       * header.
       *
       * \note This method is just an alias of bitsPerSample().
       *
       * \deprecated
       */
      TAGLIB_DEPRECATED int sampleWidth() const;

      /*!
       * Return the number of sample frames.
       */
      unsigned long long sampleFrames() const;

      /*!
       * Returns the MD5 signature of the uncompressed audio stream as read
       * from the stream info header.
       */
      ByteVector signature() const;

    private:
      Properties(const Properties &);
      Properties &operator=(const Properties &);

      void read(const ByteVector &data, long streamLength);

      class PropertiesPrivate;
      PropertiesPrivate *d;
    };
  }
}

#endif
