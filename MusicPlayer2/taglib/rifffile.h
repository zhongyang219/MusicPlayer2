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

#ifndef TAGLIB_RIFFFILE_H
#define TAGLIB_RIFFFILE_H

#include "taglib_export.h"
#include "tfile.h"

namespace TagLib {

  //! An implementation of TagLib::File with RIFF specific methods

  namespace RIFF {

    //! An RIFF file class with some useful methods specific to RIFF

    /*!
     * This implements the generic TagLib::File API and additionally provides
     * access to properties that are distinct to RIFF files, notably access
     * to the different ID3 tags.
     */

    class TAGLIB_EXPORT File : public TagLib::File
    {
    public:
      /*!
       * Destroys this instance of the File.
       */
      virtual ~File();

    protected:

      enum Endianness { BigEndian, LittleEndian };

      File(FileName file, Endianness endianness);
      File(IOStream *stream, Endianness endianness);

      /*!
       * \return The size of the main RIFF chunk.
       */
      unsigned int riffSize() const;

      /*!
       * \return The number of chunks in the file.
       */
      unsigned int chunkCount() const;

      /*!
       * \return The offset within the file for the selected chunk number.
       */
      unsigned int chunkOffset(unsigned int i) const;

      /*!
       * \return The size of the chunk data.
       */
      unsigned int chunkDataSize(unsigned int i) const;

      /*!
       * \return The size of the padding after the chunk (can be either 0 or 1).
       */
      unsigned int chunkPadding(unsigned int i) const;

      /*!
       * \return The name of the specified chunk, for instance, "COMM" or "ID3 "
       */
      ByteVector chunkName(unsigned int i) const;

      /*!
       * Reads the chunk data from the file and returns it.
       *
       * \note This \e will move the read pointer for the file.
       */
      ByteVector chunkData(unsigned int i);

      /*!
       * Sets the data for the specified chunk to \a data.
       *
       * \warning This will update the file immediately.
       */
      void setChunkData(unsigned int i, const ByteVector &data);

      /*!
       * Sets the data for the chunk \a name to \a data.  If a chunk with the
       * given name already exists it will be overwritten, otherwise it will be
       * created after the existing chunks.
       *
       * \warning This will update the file immediately.
       */
      void setChunkData(const ByteVector &name, const ByteVector &data);

      /*!
       * Sets the data for the chunk \a name to \a data.  If a chunk with the
       * given name already exists it will be overwritten, otherwise it will be
       * created after the existing chunks.
       *
       * \note If \a alwaysCreate is true, a new chunk is created regardless of
       * whether or not the chunk \a name exists. It should only be used for
       * "LIST" chunks.
       *
       * \warning This will update the file immediately.
       */
      void setChunkData(const ByteVector &name, const ByteVector &data, bool alwaysCreate);

      /*!
       * Removes the specified chunk.
       *
       * \warning This will update the file immediately.
       */
      void removeChunk(unsigned int i);

      /*!
       * Removes the chunk \a name.
       *
       * \warning This will update the file immediately.
       * \warning This removes all the chunks with the given name.
       */
      void removeChunk(const ByteVector &name);

    private:
      File(const File &);
      File &operator=(const File &);

      void read();
      void writeChunk(const ByteVector &name, const ByteVector &data,
                      unsigned long offset, unsigned long replace = 0);

      /*!
       * Update the global RIFF size based on the current internal structure.
       */
      void updateGlobalSize();

      class FilePrivate;
      FilePrivate *d;
    };
  }
}

#endif
