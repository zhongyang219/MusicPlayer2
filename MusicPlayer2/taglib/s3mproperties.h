/***************************************************************************
    copyright           : (C) 2011 by Mathias Panzenböck
    email               : grosser.meister.morti@gmx.net
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

#ifndef TAGLIB_S3MPROPERTIES_H
#define TAGLIB_S3MPROPERTIES_H

#include "taglib.h"
#include "audioproperties.h"

namespace TagLib {
  namespace S3M {
    class TAGLIB_EXPORT Properties : public AudioProperties {
      friend class File;
    public:
      /*! Flag bits. */
      enum {
        ST2Vibrato           =   1,
        ST2Tempo             =   2,
        AmigaSlides          =   4,
        Vol0MixOptimizations =   8,
        AmigaLimits          =  16,
        EnableFilter         =  32,
        CustomData           = 128
      };

      Properties(AudioProperties::ReadStyle propertiesStyle);
      virtual ~Properties();

      int length()               const;
      int lengthInSeconds()      const;
      int lengthInMilliseconds() const;
      int bitrate()              const;
      int sampleRate()           const;
      int channels()             const;

      unsigned short lengthInPatterns()  const;
      bool           stereo()            const;
      unsigned short sampleCount()       const;
      unsigned short patternCount()      const;
      unsigned short flags()             const;
      unsigned short trackerVersion()    const;
      unsigned short fileFormatVersion() const;
      unsigned char  globalVolume()      const;
      unsigned char  masterVolume()      const;
      unsigned char  tempo()             const;
      unsigned char  bpmSpeed()          const;

      void setChannels(int channels);

      void setLengthInPatterns (unsigned short lengthInPatterns);
      void setStereo           (bool stereo);
      void setSampleCount      (unsigned short sampleCount);
      void setPatternCount     (unsigned short patternCount);
      void setFlags            (unsigned short flags);
      void setTrackerVersion   (unsigned short trackerVersion);
      void setFileFormatVersion(unsigned short fileFormatVersion);
      void setGlobalVolume     (unsigned char globalVolume);
      void setMasterVolume     (unsigned char masterVolume);
      void setTempo            (unsigned char tempo);
      void setBpmSpeed         (unsigned char bpmSpeed);

    private:
      Properties(const Properties&);
      Properties &operator=(const Properties&);

      class PropertiesPrivate;
      PropertiesPrivate *d;
    };
  }
}

#endif
