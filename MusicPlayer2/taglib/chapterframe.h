/***************************************************************************
    copyright            : (C) 2013 by Lukas Krejci
    email                : krejclu6@fel.cvut.cz
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

#ifndef TAGLIB_CHAPTERFRAME
#define TAGLIB_CHAPTERFRAME

#include "id3v2tag.h"
#include "id3v2frame.h"
#include "taglib_export.h"

namespace TagLib {

  namespace ID3v2 {

    /*!
     * This is an implementation of ID3v2 chapter frames.  The purpose of this
     * frame is to describe a single chapter within an audio file.
     */

    //! An implementation of ID3v2 chapter frames

    class TAGLIB_EXPORT ChapterFrame : public ID3v2::Frame
    {
      friend class FrameFactory;

    public:
      /*!
       * Creates a chapter frame based on \a data.  \a tagHeader is required as
       * the internal frames are parsed based on the tag version.
       */
      ChapterFrame(const ID3v2::Header *tagHeader, const ByteVector &data);

      /*!
       * Creates a chapter frame with the element ID \a elementID, start time
       * \a startTime, end time \a endTime, start offset \a startOffset,
       * end offset \a endOffset and optionally a list of embedded frames,
       * whose ownership will then be taken over by this Frame, in
       * \a embeededFrames;
       *
       * All times are in milliseconds.
       */
      ChapterFrame(const ByteVector &elementID,
                   unsigned int startTime, unsigned int endTime,
                   unsigned int startOffset, unsigned int endOffset,
                   const FrameList &embeddedFrames = FrameList());

      /*!
       * Destroys the frame.
       */
      virtual ~ChapterFrame();

      /*!
       * Returns the element ID of the frame. Element ID
       * is a null terminated string, however it's not human-readable.
       *
       * \see setElementID()
       */
      ByteVector elementID() const;

      /*!
       * Returns time of chapter's start (in milliseconds).
       *
       * \see setStartTime()
       */
      unsigned int startTime() const;

      /*!
       * Returns time of chapter's end (in milliseconds).
       *
       * \see setEndTime()
       */
      unsigned int endTime() const;

      /*!
       * Returns zero based byte offset (count of bytes from the beginning
       * of the audio file) of chapter's start.
       *
       * \note If returned value is 0xFFFFFFFF, start time should be used instead.
       * \see setStartOffset()
       */
      unsigned int startOffset() const;

      /*!
       * Returns zero based byte offset (count of bytes from the beginning
       * of the audio file) of chapter's end.
       *
       * \note If returned value is 0xFFFFFFFF, end time should be used instead.
       * \see setEndOffset()
       */
      unsigned int endOffset() const;

      /*!
       * Sets the element ID of the frame to \a eID. If \a eID isn't
       * null terminated, a null char is appended automatically.
       *
       * \see elementID()
       */
      void setElementID(const ByteVector &eID);

      /*!
       * Sets time of chapter's start (in milliseconds) to \a sT.
       *
       * \see startTime()
       */
      void setStartTime(const unsigned int &sT);

      /*!
       * Sets time of chapter's end (in milliseconds) to \a eT.
       *
       * \see endTime()
       */
      void setEndTime(const unsigned int &eT);

      /*!
       * Sets zero based byte offset (count of bytes from the beginning
       * of the audio file) of chapter's start to \a sO.
       *
       * \see startOffset()
       */
      void setStartOffset(const unsigned int &sO);

      /*!
       * Sets zero based byte offset (count of bytes from the beginning
       * of the audio file) of chapter's end to \a eO.
       *
       * \see endOffset()
       */
      void setEndOffset(const unsigned int &eO);

      /*!
       * Returns a reference to the frame list map.  This is an FrameListMap of
       * all of the frames embedded in the CHAP frame.
       *
       * This is the most convenient structure for accessing the CHAP frame's
       * embedded frames. Many frame types allow multiple instances of the same
       * frame type so this is a map of lists. In most cases however there will
       * only be a single frame of a certain type.
       *
       * \warning You should not modify this data structure directly, instead
       * use addEmbeddedFrame() and removeEmbeddedFrame().
       *
       * \see embeddedFrameList()
       */
      const FrameListMap &embeddedFrameListMap() const;

      /*!
       * Returns a reference to the embedded frame list.  This is an FrameList
       * of all of the frames embedded in the CHAP frame in the order that they
       * were parsed.
       *
       * This can be useful if for example you want iterate over the CHAP frame's
       * embedded frames in the order that they occur in the CHAP frame.
       *
       * \warning You should not modify this data structure directly, instead
       * use addEmbeddedFrame() and removeEmbeddedFrame().
       */
      const FrameList &embeddedFrameList() const;

      /*!
       * Returns the embedded frame list for frames with the id \a frameID
       * or an empty list if there are no embedded frames of that type.  This
       * is just a convenience and is equivalent to:
       *
       * \code
       * embeddedFrameListMap()[frameID];
       * \endcode
       *
       * \see embeddedFrameListMap()
       */
      const FrameList &embeddedFrameList(const ByteVector &frameID) const;

      /*!
       * Add an embedded frame to the CHAP frame.  At this point the CHAP frame
       * takes ownership of the embedded frame and will handle freeing its memory.
       *
       * \note Using this method will invalidate any pointers on the list
       * returned by embeddedFrameList()
       */
      void addEmbeddedFrame(Frame *frame);

      /*!
       * Remove an embedded frame from the CHAP frame.  If \a del is true the frame's
       * memory will be freed; if it is false, it must be deleted by the user.
       *
       * \note Using this method will invalidate any pointers on the list
       * returned by embeddedFrameList()
       */
      void removeEmbeddedFrame(Frame *frame, bool del = true);

      /*!
       * Remove all embedded frames of type \a id from the CHAP frame and free their
       * memory.
       *
       * \note Using this method will invalidate any pointers on the list
       * returned by embeddedFrameList()
       */
      void removeEmbeddedFrames(const ByteVector &id);

      virtual String toString() const;

      PropertyMap asProperties() const;

      /*!
       * CHAP frames each have a unique element ID. This searches for a CHAP
       * frame with the element ID \a eID and returns a pointer to it. This
       * can be used to link CTOC and CHAP frames together.
       *
       * \see elementID()
       */
      static ChapterFrame *findByElementID(const Tag *tag, const ByteVector &eID);

    protected:
      virtual void parseFields(const ByteVector &data);
      virtual ByteVector renderFields() const;

    private:
      ChapterFrame(const ID3v2::Header *tagHeader, const ByteVector &data, Header *h);
      ChapterFrame(const ChapterFrame &);
      ChapterFrame &operator=(const ChapterFrame &);

      class ChapterFramePrivate;
      ChapterFramePrivate *d;
    };
  }
}

#endif
