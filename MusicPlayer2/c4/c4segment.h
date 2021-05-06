/************************************************************************/
/*                                                                      */
/* c4-lib                                                               */
/* c4-lib is A Common Codes Converting Context library.                 */
/*                                                                      */
/* Version: 0.1                                                         */
/* Author:  wei_w (weiwl07@gmail.com)                                   */
/* Published under LGPL-3.0                                             */
/* https://www.gnu.org/licenses/lgpl-3.0.en.html                        */
/*                                                                      */
/* Copyright 2016 Project c4                                            */
/* https://github.com/kuyur/c4                                          */
/*                                                                      */
/************************************************************************/

#pragma once

// c4segment.h
#ifndef C4SEGMENT_H_
#define C4SEGMENT_H_

#include <stdlib.h>
#include <string.h>

struct CC4Segment_tag;

typedef struct CC4Segment_tag
{
    enum segmentRef {
        refASCII  = 0x01,
        refOxFFFD = 0x02,
        refBUFFER = 0x04,
        refSelf   = 0x08
    };

    wchar_t         m_begin;
    wchar_t         m_end;
    segmentRef      m_reference;
    int             m_offset;

    CC4Segment_tag()
    {
        m_begin     = 0x00;
        m_end       = 0x7F;
        m_reference = refASCII;
        m_offset    = -1;
    };
    static segmentRef toSegmentRef(const char* reference)
    {
        if (NULL == reference) return refOxFFFD;
        if (strcmp("ascii", reference) == 0) return refASCII;
        if (strcmp("0xFFFD", reference) == 0) return refOxFFFD;
        if (strcmp("buffer", reference) == 0) return refBUFFER;
        if (strcmp("self", reference) == 0) return refSelf;
        return refOxFFFD;
    };
}CC4Segment, *PCC4Segment;

class CC4Segments
{
private:
    CC4Segment*   m_segments;
    unsigned int  m_size;
public:
    CC4Segments(unsigned int num_segments);
    ~CC4Segments();
    bool checkValid() const;
    unsigned int calBufferLength() const;
    bool setSegment(unsigned int position, const CC4Segment& segment);
    const CC4Segment* findMatchedSegment(wchar_t chr) const;
};

#endif // C4SEGMENT_H_