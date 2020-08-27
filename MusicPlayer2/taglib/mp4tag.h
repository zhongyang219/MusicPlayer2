/**************************************************************************
    copyright            : (C) 2007,2011 by Lukáš Lalinský
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

#ifndef TAGLIB_MP4TAG_H
#define TAGLIB_MP4TAG_H

#include "tag.h"
#include "tbytevectorlist.h"
#include "tfile.h"
#include "tmap.h"
#include "tstringlist.h"
#include "taglib_export.h"
#include "mp4atom.h"
#include "mp4item.h"

namespace TagLib {

  namespace MP4 {

    /*!
     * \deprecated
     */
    TAGLIB_DEPRECATED typedef TagLib::Map<String, Item> ItemListMap;
    typedef TagLib::Map<String, Item> ItemMap;

    class TAGLIB_EXPORT Tag: public TagLib::Tag
    {
    public:
        Tag();
        Tag(TagLib::File *file, Atoms *atoms);
        virtual ~Tag();
        bool save();

        virtual String title() const;
        virtual String artist() const;
        virtual String album() const;
        virtual String comment() const;
        virtual String genre() const;
        virtual unsigned int year() const;
        virtual unsigned int track() const;

        virtual void setTitle(const String &value);
        virtual void setArtist(const String &value);
        virtual void setAlbum(const String &value);
        virtual void setComment(const String &value);
        virtual void setGenre(const String &value);
        virtual void setYear(unsigned int value);
        virtual void setTrack(unsigned int value);

        virtual bool isEmpty() const;

        /*!
         * \deprecated Use the item() and setItem() API instead
         */
        TAGLIB_DEPRECATED ItemMap &itemListMap();

        /*!
         * Returns a string-keyed map of the MP4::Items for this tag.
         */
        const ItemMap &itemMap() const;

        /*!
         * \return The item, if any, corresponding to \a key.
         */
        Item item(const String &key) const;

        /*!
         * Sets the value of \a key to \a value, overwriting any previous value.
         */
        void setItem(const String &key, const Item &value);

        /*!
         * Removes the entry with \a key from the tag, or does nothing if it does
         * not exist.
         */
        void removeItem(const String &key);

        /*!
         * \return True if the tag contains an entry for \a key.
         */
        bool contains(const String &key) const;

        PropertyMap properties() const;
        void removeUnsupportedProperties(const StringList& properties);
        PropertyMap setProperties(const PropertyMap &properties);

    private:
        AtomDataList parseData2(const Atom *atom, int expectedFlags = -1,
                                bool freeForm = false);
        ByteVectorList parseData(const Atom *atom, int expectedFlags = -1,
                                 bool freeForm = false);
        void parseText(const Atom *atom, int expectedFlags = 1);
        void parseFreeForm(const Atom *atom);
        void parseInt(const Atom *atom);
        void parseByte(const Atom *atom);
        void parseUInt(const Atom *atom);
        void parseLongLong(const Atom *atom);
        void parseGnre(const Atom *atom);
        void parseIntPair(const Atom *atom);
        void parseBool(const Atom *atom);
        void parseCovr(const Atom *atom);

        ByteVector padIlst(const ByteVector &data, int length = -1) const;
        ByteVector renderAtom(const ByteVector &name, const ByteVector &data) const;
        ByteVector renderData(const ByteVector &name, int flags,
                              const ByteVectorList &data) const;
        ByteVector renderText(const ByteVector &name, const Item &item,
                              int flags = TypeUTF8) const;
        ByteVector renderFreeForm(const String &name, const Item &item) const;
        ByteVector renderBool(const ByteVector &name, const Item &item) const;
        ByteVector renderInt(const ByteVector &name, const Item &item) const;
        ByteVector renderByte(const ByteVector &name, const Item &item) const;
        ByteVector renderUInt(const ByteVector &name, const Item &item) const;
        ByteVector renderLongLong(const ByteVector &name, const Item &item) const;
        ByteVector renderIntPair(const ByteVector &name, const Item &item) const;
        ByteVector renderIntPairNoTrailing(const ByteVector &name, const Item &item) const;
        ByteVector renderCovr(const ByteVector &name, const Item &item) const;

        void updateParents(const AtomList &path, long delta, int ignore = 0);
        void updateOffsets(long delta, long offset);

        void saveNew(ByteVector data);
        void saveExisting(ByteVector data, const AtomList &path);

        void addItem(const String &name, const Item &value);

        class TagPrivate;
        TagPrivate *d;
    };

  }

}

#endif
