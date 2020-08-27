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

#ifndef TAGLIB_SYNCHRONIZEDLYRICSFRAME_H
#define TAGLIB_SYNCHRONIZEDLYRICSFRAME_H

#include "id3v2frame.h"
#include "tlist.h"

namespace TagLib {

  namespace ID3v2 {

    //! ID3v2 synchronized lyrics frame
    /*!
     * An implementation of ID3v2 synchronized lyrics.
     */
    class TAGLIB_EXPORT SynchronizedLyricsFrame : public Frame
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
       * Specifies the type of text contained.
       */
      enum Type {
        //! The text is some other type of text.
        Other             = 0x00,
        //! The text contains lyrical data.
        Lyrics            = 0x01,
        //! The text contains a transcription.
        TextTranscription = 0x02,
        //! The text lists the movements in the piece.
        Movement          = 0x03,
        //! The text describes events that occur.
        Events            = 0x04,
        //! The text contains chord changes that occur in the music.
        Chord             = 0x05,
        //! The text contains trivia or "pop up" information about the media.
        Trivia            = 0x06,
        //! The text contains URLs for relevant webpages.
        WebpageUrls       = 0x07,
        //!  The text contains URLs for relevant images.
        ImageUrls         = 0x08
      };

      /*!
       * Single entry of time stamp and lyrics text.
       */
      struct SynchedText {
        SynchedText(unsigned int ms, String str) : time(ms), text(str) {}
        unsigned int time;
        String text;
      };

      /*!
       * List of synchronized lyrics.
       */
      typedef TagLib::List<SynchedText> SynchedTextList;

      /*!
       * Construct an empty synchronized lyrics frame that will use the text
       * encoding \a encoding.
       */
      explicit SynchronizedLyricsFrame(String::Type encoding = String::Latin1);

      /*!
       * Construct a synchronized lyrics frame based on the data in \a data.
       */
      explicit SynchronizedLyricsFrame(const ByteVector &data);

      /*!
       * Destroys this SynchronizedLyricsFrame instance.
       */
      virtual ~SynchronizedLyricsFrame();

      /*!
       * Returns the description of this synchronized lyrics frame.
       *
       * \see description()
       */
      virtual String toString() const;

      /*!
       * Returns the text encoding that will be used in rendering this frame.
       * This defaults to the type that was either specified in the constructor
       * or read from the frame when parsed.
       *
       * \see setTextEncoding()
       * \see render()
       */
      String::Type textEncoding() const;

      /*!
       * Returns the language encoding as a 3 byte encoding as specified by
       * <a href="http://en.wikipedia.org/wiki/ISO_639">ISO-639-2</a>.
       *
       * \note Most taggers simply ignore this value.
       *
       * \see setLanguage()
       */
      ByteVector language() const;

      /*!
       * Returns the timestamp format.
       */
      TimestampFormat timestampFormat() const;

      /*!
       * Returns the type of text contained.
       */
      Type type() const;

      /*!
       * Returns the description of this synchronized lyrics frame.
       *
       * \note Most taggers simply ignore this value.
       *
       * \see setDescription()
       */
      String description() const;

      /*!
       * Returns the text with the time stamps.
       */
      SynchedTextList synchedText() const;

      /*!
       * Sets the text encoding to be used when rendering this frame to
       * \a encoding.
       *
       * \see textEncoding()
       * \see render()
       */
      void setTextEncoding(String::Type encoding);

      /*!
       * Set the language using the 3 byte language code from
       * <a href="http://en.wikipedia.org/wiki/ISO_639">ISO-639-2</a> to
       * \a languageCode.
       *
       * \see language()
       */
      void setLanguage(const ByteVector &languageCode);

      /*!
       * Set the timestamp format.
       *
       * \see timestampFormat()
       */
      void setTimestampFormat(TimestampFormat f);

      /*!
       * Set the type of text contained.
       *
       * \see type()
       */
      void setType(Type t);

      /*!
       * Sets the description of the synchronized lyrics frame to \a s.
       *
       * \see description()
       */
      void setDescription(const String &s);

      /*!
       * Sets the text with the time stamps.
       *
       * \see text()
       */
      void setSynchedText(const SynchedTextList &t);

    protected:
      // Reimplementations.

      virtual void parseFields(const ByteVector &data);
      virtual ByteVector renderFields() const;

    private:
      /*!
       * The constructor used by the FrameFactory.
       */
      SynchronizedLyricsFrame(const ByteVector &data, Header *h);
      SynchronizedLyricsFrame(const SynchronizedLyricsFrame &);
      SynchronizedLyricsFrame &operator=(const SynchronizedLyricsFrame &);

      class SynchronizedLyricsFramePrivate;
      SynchronizedLyricsFramePrivate *d;
    };

  }
}
#endif
