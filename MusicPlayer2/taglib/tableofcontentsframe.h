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

#ifndef TAGLIB_TABLEOFCONTENTSFRAME
#define TAGLIB_TABLEOFCONTENTSFRAME

#include "id3v2tag.h"
#include "id3v2frame.h"

#include "tbytevectorlist.h"

namespace TagLib {

  namespace ID3v2 {

    /*!
     * This is an implementation of ID3v2 table of contents frames.  Purpose
     * of this frame is to allow a table of contents to be defined.
     */

    //! An implementation of ID3v2 table of contents frames

    class TAGLIB_EXPORT TableOfContentsFrame : public ID3v2::Frame
    {
      friend class FrameFactory;

    public:
      /*!
       * Creates a table of contents frame based on \a data.  \a tagHeader is
       * required as the internal frames are parsed based on the tag version.
       */
      TableOfContentsFrame(const ID3v2::Header *tagHeader, const ByteVector &data);

      /*!
       * Creates a table of contents frame with the element ID \a elementID,
       * the child elements \a children and embedded frames, which become owned
       * by this frame, in \a embeddedFrames.
       */
      TableOfContentsFrame(const ByteVector &elementID,
                           const ByteVectorList &children = ByteVectorList(),
                           const FrameList &embeddedFrames = FrameList());

      /*!
       * Destroys the frame.
       */
      ~TableOfContentsFrame();

      /*!
       * Returns the elementID of the frame. Element ID
       * is a null terminated string, however it's not human-readable.
       *
       * \see setElementID()
       */
      ByteVector elementID() const;

      /*!
       * Returns true, if the frame is top-level (doesn't have
       * any parent CTOC frame).
       *
       * \see setIsTopLevel()
       */
      bool isTopLevel() const;

      /*!
       * Returns true, if the child elements list entries
       * are ordered.
       *
       * \see setIsOrdered()
       */
      bool isOrdered() const;

      /*!
       * Returns count of child elements of the frame. It always
       * corresponds to size of child elements list.
       *
       * \see childElements()
       */
      unsigned int entryCount() const;

      /*!
       * Returns list of child elements of the frame.
       *
       * \see setChildElements()
       */
      ByteVectorList childElements() const;

      /*!
       * Sets the elementID of the frame to \a eID. If \a eID isn't
       * null terminated, a null char is appended automatically.
       *
       * \see elementID()
       */
      void setElementID(const ByteVector &eID);

      /*!
       * Sets, if the frame is top-level (doesn't have
       * any parent CTOC frame).
       *
       * \see isTopLevel()
       */
      void setIsTopLevel(const bool &t);

      /*!
       * Sets, if the child elements list entries
       * are ordered.
       *
       * \see isOrdered()
       */
      void setIsOrdered(const bool &o);

      /*!
       * Sets list of child elements of the frame to \a l.
       *
       * \see childElements()
       */
      void setChildElements(const ByteVectorList &l);

      /*!
       * Adds \a cE to list of child elements of the frame.
       *
       * \see childElements()
       */
      void addChildElement(const ByteVector &cE);

      /*!
       * Removes \a cE to list of child elements of the frame.
       *
       * \see childElements()
       */
      void removeChildElement(const ByteVector &cE);

      /*!
       * Returns a reference to the frame list map.  This is an FrameListMap of
       * all of the frames embedded in the CTOC frame.
       *
       * This is the most convenient structure for accessing the CTOC frame's
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
       * of all of the frames embedded in the CTOC frame in the order that they
       * were parsed.
       *
       * This can be useful if for example you want iterate over the CTOC frame's
       * embedded frames in the order that they occur in the CTOC frame.
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
       * Add an embedded frame to the CTOC frame.  At this point the CTOC frame
       * takes ownership of the embedded frame and will handle freeing its memory.
       *
       * \note Using this method will invalidate any pointers on the list
       * returned by embeddedFrameList()
       */
      void addEmbeddedFrame(Frame *frame);

      /*!
       * Remove an embedded frame from the CTOC frame.  If \a del is true the frame's
       * memory will be freed; if it is false, it must be deleted by the user.
       *
       * \note Using this method will invalidate any pointers on the list
       * returned by embeddedFrameList()
       */
      void removeEmbeddedFrame(Frame *frame, bool del = true);

      /*!
       * Remove all embedded frames of type \a id from the CTOC frame and free their
       * memory.
       *
       * \note Using this method will invalidate any pointers on the list
       * returned by embeddedFrameList()
       */
      void removeEmbeddedFrames(const ByteVector &id);

      virtual String toString() const;

      PropertyMap asProperties() const;

      /*!
       * CTOC frames each have a unique element ID. This searches for a CTOC
       * frame with the element ID \a eID and returns a pointer to it. This
       * can be used to link together parent and child CTOC frames.
       *
       * \see elementID()
       */
      static TableOfContentsFrame *findByElementID(const Tag *tag, const ByteVector &eID);

      /*!
       * CTOC frames each contain a flag that indicates, if CTOC frame is top-level (there isn't
       * any frame, which contains this frame in its child elements list). Only a single frame
       * within tag can be top-level. This searches for a top-level CTOC frame.
       *
       * \see isTopLevel()
       */
      static TableOfContentsFrame *findTopLevel(const Tag *tag);

    protected:
      virtual void parseFields(const ByteVector &data);
      virtual ByteVector renderFields() const;

    private:
      TableOfContentsFrame(const ID3v2::Header *tagHeader, const ByteVector &data, Header *h);
      TableOfContentsFrame(const TableOfContentsFrame &);
      TableOfContentsFrame &operator=(const TableOfContentsFrame &);

      class TableOfContentsFramePrivate;
      TableOfContentsFramePrivate *d;
    };
  }
}

#endif
