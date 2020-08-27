/***************************************************************************
    copyright            : (C) 2002 - 2008 by Scott Wheeler
    email                : wheeler@kde.org
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

#ifndef TAGLIB_VORBISPROPERTIES_H
#define TAGLIB_VORBISPROPERTIES_H

#include "taglib_export.h"
#include "audioproperties.h"

namespace TagLib {

/*
 * This is just to make this appear to be in the Ogg namespace in the
 * documentation.  The typedef below will make this work with the current code.
 * In the next BIC version of TagLib this will be really moved into the Ogg
 * namespace.
 */

#ifdef DOXYGEN
  namespace Ogg {
#endif

  namespace Vorbis {

    class File;

    //! An implementation of audio property reading for Ogg Vorbis

    /*!
     * This reads the data from an Ogg Vorbis stream found in the AudioProperties
     * API.
     */

    class TAGLIB_EXPORT Properties : public AudioProperties
    {
    public:
      /*!
       * Create an instance of Vorbis::Properties with the data read from the
       * Vorbis::File \a file.
       */
      Properties(File *file, ReadStyle style = Average);

      /*!
       * Destroys this VorbisProperties instance.
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
       * Returns the Vorbis version, currently "0" (as specified by the spec).
       */
      int vorbisVersion() const;

      /*!
       * Returns the maximum bitrate as read from the Vorbis identification
       * header.
       */
      int bitrateMaximum() const;

      /*!
       * Returns the nominal bitrate as read from the Vorbis identification
       * header.
       */
      int bitrateNominal() const;

      /*!
       * Returns the minimum bitrate as read from the Vorbis identification
       * header.
       */
      int bitrateMinimum() const;

    private:
      Properties(const Properties &);
      Properties &operator=(const Properties &);

      void read(File *file);

      class PropertiesPrivate;
      PropertiesPrivate *d;
    };
  }

/*
 * To keep compatibility with the current version put Vorbis in the Ogg namespace
 * only in the docs and provide a typedef to make it work.  In the next BIC
 * version this will be removed and it will only exist in the Ogg namespace.
 */

#ifdef DOXYGEN
  }
#else
  namespace Ogg { namespace Vorbis { typedef TagLib::AudioProperties AudioProperties; } }
#endif

}

#endif
