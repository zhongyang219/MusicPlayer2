#ifndef TAGLIB_ID3V2_H
#define TAGLIB_ID3V2_H

namespace TagLib {
  //! An ID3v2 implementation

  /*!
   * This is a relatively complete and flexible framework for working with ID3v2
   * tags.
   *
   * \see ID3v2::Tag
   */
  namespace ID3v2 {
    /*!
     * Used to specify which version of the ID3 standard to use when saving tags.
     */
    enum Version {
      v3 = 3, //<! ID3v2.3
      v4 = 4  //<! ID3v2.4
    };
  }
}

#endif
