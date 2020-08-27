/***************************************************************************
    copyright            : (C) 2014 by Urs Fleisch
    email                : ufleisch@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it  under the terms of the GNU Lesser General Public License version  *
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

#ifndef TAGLIB_EVENTTIMINGCODESFRAME_H
#define TAGLIB_EVENTTIMINGCODESFRAME_H

#include "id3v2frame.h"
#include "tlist.h"

namespace TagLib {

  namespace ID3v2 {

    //! ID3v2 event timing codes frame
    /*!
     * An implementation of ID3v2 event timing codes.
     */
    class TAGLIB_EXPORT EventTimingCodesFrame : public Frame
    {
      friend class FrameFactory;

    public:

      /*!
       * Specifies the timestamp format used.
       */
      enum TimestampFormat {
        //! The timestamp is of unknown format.
        Unknown              = 0x00,
        //! The timestamp represents the number of MPEG frames since
        //! the beginning of the audio stream.
        AbsoluteMpegFrames   = 0x01,
        //! The timestamp represents the number of milliseconds since
        //! the beginning of the audio stream.
        AbsoluteMilliseconds = 0x02
      };

      /*!
       * Event types defined in id3v2.4.0-frames.txt 4.5. Event timing codes.
       */
      enum EventType {
        Padding                = 0x00,
        EndOfInitialSilence    = 0x01,
        IntroStart             = 0x02,
        MainPartStart          = 0x03,
        OutroStart             = 0x04,
        OutroEnd               = 0x05,
        VerseStart             = 0x06,
        RefrainStart           = 0x07,
        InterludeStart         = 0x08,
        ThemeStart             = 0x09,
        VariationStart         = 0x0a,
        KeyChange              = 0x0b,
        TimeChange             = 0x0c,
        MomentaryUnwantedNoise = 0x0d,
        SustainedNoise         = 0x0e,
        SustainedNoiseEnd      = 0x0f,
        IntroEnd               = 0x10,
        MainPartEnd            = 0x11,
        VerseEnd               = 0x12,
        RefrainEnd             = 0x13,
        ThemeEnd               = 0x14,
        Profanity              = 0x15,
        ProfanityEnd           = 0x16,
        NotPredefinedSynch0    = 0xe0,
        NotPredefinedSynch1    = 0xe1,
        NotPredefinedSynch2    = 0xe2,
        NotPredefinedSynch3    = 0xe3,
        NotPredefinedSynch4    = 0xe4,
        NotPredefinedSynch5    = 0xe5,
        NotPredefinedSynch6    = 0xe6,
        NotPredefinedSynch7    = 0xe7,
        NotPredefinedSynch8    = 0xe8,
        NotPredefinedSynch9    = 0xe9,
        NotPredefinedSynchA    = 0xea,
        NotPredefinedSynchB    = 0xeb,
        NotPredefinedSynchC    = 0xec,
        NotPredefinedSynchD    = 0xed,
        NotPredefinedSynchE    = 0xee,
        NotPredefinedSynchF    = 0xef,
        AudioEnd               = 0xfd,
        AudioFileEnds          = 0xfe
      };

      /*!
       * Single entry of time stamp and event.
       */
      struct SynchedEvent {
        SynchedEvent(unsigned int ms, EventType t) : time(ms), type(t) {}
        unsigned int time;
        EventType type;
      };

      /*!
       * List of synchronized events.
       */
      typedef TagLib::List<SynchedEvent> SynchedEventList;

      /*!
       * Construct an empty event timing codes frame.
       */
      explicit EventTimingCodesFrame();

      /*!
       * Construct a event timing codes frame based on the data in \a data.
       */
      explicit EventTimingCodesFrame(const ByteVector &data);

      /*!
       * Destroys this EventTimingCodesFrame instance.
       */
      virtual ~EventTimingCodesFrame();

      /*!
       * Returns a null string.
       */
      virtual String toString() const;

      /*!
       * Returns the timestamp format.
       */
      TimestampFormat timestampFormat() const;

      /*!
       * Returns the events with the time stamps.
       */
      SynchedEventList synchedEvents() const;

      /*!
       * Set the timestamp format.
       *
       * \see timestampFormat()
       */
      void setTimestampFormat(TimestampFormat f);

      /*!
       * Sets the text with the time stamps.
       *
       * \see text()
       */
      void setSynchedEvents(const SynchedEventList &e);

    protected:
      // Reimplementations.

      virtual void parseFields(const ByteVector &data);
      virtual ByteVector renderFields() const;

    private:
      /*!
       * The constructor used by the FrameFactory.
       */
      EventTimingCodesFrame(const ByteVector &data, Header *h);
      EventTimingCodesFrame(const EventTimingCodesFrame &);
      EventTimingCodesFrame &operator=(const EventTimingCodesFrame &);

      class EventTimingCodesFramePrivate;
      EventTimingCodesFramePrivate *d;
    };

  }
}
#endif
