/**************************************************************************
    copyright            : (C) 2005-2007 by Lukáš Lalinský
    email                : lalinsky@gmail.com
 **************************************************************************/

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

#ifndef TAGLIB_ASFPROPERTIES_H
#define TAGLIB_ASFPROPERTIES_H

#include "audioproperties.h"
#include "tstring.h"
#include "taglib_export.h"

namespace TagLib {

  namespace ASF {

    //! An implementation of ASF audio properties
    class TAGLIB_EXPORT Properties : public AudioProperties
    {
    public:

      /*!
       * Audio codec types can be used in ASF file.
       */
      enum Codec
      {
        /*!
         * Couldn't detect the codec.
         */
        Unknown = 0,

        /*!
         * Windows Media Audio 1
         */
        WMA1,

        /*!
         * Windows Media Audio 2 or above
         */
        WMA2,

        /*!
         * Windows Media Audio 9 Professional
         */
        WMA9Pro,

        /*!
         * Windows Media Audio 9 Lossless
         */
        WMA9Lossless,
      };

      /*!
       * Creates an instance of ASF::Properties.
       */
      Properties();

      /*!
       * Destroys this ASF::Properties instance.
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
       * Returns the codec used in the file.
       *
       * \see codecName()
       * \see codecDescription()
       */
      Codec codec() const;

      /*!
       * Returns the concrete codec name, for example "Windows Media Audio 9.1"
       * used in the file if available, otherwise an empty string.
       *
       * \see codec()
       * \see codecDescription()
       */
      String codecName() const;

      /*!
       * Returns the codec description, typically contains the encoder settings,
       * for example "VBR Quality 50, 44kHz, stereo 1-pass VBR" if available,
       * otherwise an empty string.
       *
       * \see codec()
       * \see codecName()
       */
      String codecDescription() const;

      /*!
       * Returns whether or not the file is encrypted.
       */
      bool isEncrypted() const;

#ifndef DO_NOT_DOCUMENT
      // deprecated
      void setLength(int value);

      void setLengthInMilliseconds(int value);
      void setBitrate(int value);
      void setSampleRate(int value);
      void setChannels(int value);
      void setBitsPerSample(int value);
      void setCodec(int value);
      void setCodecName(const String &value);
      void setCodecDescription(const String &value);
      void setEncrypted(bool value);
#endif

    private:
      class PropertiesPrivate;
      PropertiesPrivate *d;
    };

  }

}

#endif
