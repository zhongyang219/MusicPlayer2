/***************************************************************************
    copyright            : (C) 2015 by Urs Fleisch
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

#ifndef TAGLIB_PODCASTFRAME_H
#define TAGLIB_PODCASTFRAME_H

#include "id3v2frame.h"
#include "taglib_export.h"

namespace TagLib {

  namespace ID3v2 {

    //! ID3v2 podcast frame
    /*!
     * An implementation of ID3v2 podcast flag, a frame with four zero bytes.
     */
    class TAGLIB_EXPORT PodcastFrame : public Frame
    {
      friend class FrameFactory;

    public:
      /*!
       * Construct a podcast frame.
       */
      PodcastFrame();

      /*!
       * Destroys this PodcastFrame instance.
       */
      virtual ~PodcastFrame();

      /*!
       * Returns a null string.
       */
      virtual String toString() const;

    protected:
      // Reimplementations.

      virtual void parseFields(const ByteVector &data);
      virtual ByteVector renderFields() const;

    private:
      /*!
       * The constructor used by the FrameFactory.
       */
      PodcastFrame(const ByteVector &data, Header *h);
      PodcastFrame(const PodcastFrame &);
      PodcastFrame &operator=(const PodcastFrame &);

      class PodcastFramePrivate;
      PodcastFramePrivate *d;
    };

  }
}
#endif
