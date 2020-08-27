/***************************************************************************
    copyright            : (C) 2010 by Alex Novichkov
    email                : novichko@atnet.ru

    copyright            : (C) 2006 by Lukáš Lalinský
    email                : lalinsky@gmail.com
                           (original WavPack implementation)
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

#ifndef TAGLIB_APEPROPERTIES_H
#define TAGLIB_APEPROPERTIES_H

#include "taglib_export.h"
#include "audioproperties.h"

namespace TagLib {

  namespace APE {

    class File;

    //! An implementation of audio property reading for APE

    /*!
     * This reads the data from an APE stream found in the AudioProperties
     * API.
     */

    class TAGLIB_EXPORT Properties : public AudioProperties
    {
    public:
      /*!
       * Create an instance of APE::Properties with the data read from the
       * APE::File \a file.
       *
       * \deprecated
       */
      TAGLIB_DEPRECATED Properties(File *file, ReadStyle style = Average);

      /*!
       * Create an instance of APE::Properties with the data read from the
       * APE::File \a file.
       */
      Properties(File *file, long streamLength, ReadStyle style = Average);

      /*!
       * Destroys this APE::Properties instance.
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
       * Returns the number of bits per audio sample.
       */
      int bitsPerSample() const;

      /*!
       * Returns the total number of audio samples in file.
       */
      unsigned int sampleFrames() const;

      /*!
       * Returns APE version.
       */
      int version() const;

    private:
      Properties(const Properties &);
      Properties &operator=(const Properties &);

      void read(File *file, long streamLength);

      void analyzeCurrent(File *file);
      void analyzeOld(File *file);

      class PropertiesPrivate;
      PropertiesPrivate *d;
    };
  }
}

#endif
