// Scintilla source code edit control
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

//用于解析LRC歌词

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <string_view>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"

using namespace Scintilla;
using std::string_view;


static inline bool AtEOL(Accessor &styler, Sci_PositionU i) {
    return (styler[i] == '\n') ||
        ((styler[i] == '\r') && (styler.SafeGetCharAt(i + 1) != '\n'));
}

static inline bool IsLetterOrNumber(int ch)
{
    return ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
}


static void ColouriseMakeLine(char *lineBuffer, Sci_PositionU lengthLine, Sci_PositionU startLine, Sci_PositionU endPos, Accessor &styler, WordList* keywordlists[])
{
    WordList &keywords = *keywordlists[0];
    string_view strBuff{ lineBuffer };
    styler.ColourTo(startLine - 1, SCE_LYRIC_DEFAULT);

    int i = 0;
    int curStyle = SCE_LYRIC_DEFAULT;
    int tag_start_pos{ -1 };
    int tag_end_pos{ -1 };
    int separator_end{ -1 };
    for (auto ch : strBuff)
    {
        if (ch == '[')
        {
            tag_start_pos = i;
            styler.ColourTo(startLine + i - 1, curStyle);
            curStyle = SCE_LYRIC_TIMETAG;
        }
        if (ch == ':')
        {
            std::string str{ strBuff.substr(tag_start_pos + 1, i - tag_start_pos - 1) };
            
            if (keywords.InList(str.c_str()))
            {
                styler.ColourTo(startLine + tag_start_pos, curStyle);
                curStyle = SCE_LYRIC_TIME_TAG_KEYWORD;
                styler.ColourTo(startLine + i - 1, curStyle);
                curStyle = SCE_LYRIC_TIMETAG;
            }
        }
        if (ch == ']')
        {
            tag_end_pos = i;
            styler.ColourTo(startLine + i, curStyle);
            curStyle = SCE_LYRIC_TEXT;
        }
        if (i > 2 && lineBuffer[i - 2] == ' ' && lineBuffer[i - 1] == '/' && lineBuffer[i] == ' ')
        {
            separator_end = i;
            styler.ColourTo(startLine + i - 3, curStyle);
            curStyle = SCE_LYRIC_SEPARATOR;
            styler.ColourTo(startLine + i, curStyle);
            curStyle = SCE_LYRIC_TRANSLATION;
        }

        i++;
    }
    styler.ColourTo(endPos, curStyle);

}

static void ColouriseLyricDoc(Sci_PositionU startPos, Sci_Position length, int, WordList* keywordlists[],
                            Accessor &styler) {
    char lineBuffer[1024];
    styler.StartAt(startPos);
    styler.StartSegment(startPos);
    Sci_PositionU linePos = 0;
    Sci_PositionU startLine = startPos;
    for (Sci_PositionU i = startPos; i < startPos + length; i++) {
        lineBuffer[linePos++] = styler[i];
        if (AtEOL(styler, i) || (linePos >= sizeof(lineBuffer) - 1)) {
            // End of line (or of line buffer) met, colourise it
            lineBuffer[linePos] = '\0';
            ColouriseMakeLine(lineBuffer, linePos, startLine, i, styler, keywordlists);
            linePos = 0;
            startLine = i + 1;
        }
    }
    if (linePos > 0) {	// Last line does not have ending characters
        ColouriseMakeLine(lineBuffer, linePos, startLine, startPos + length - 1, styler, keywordlists);
    }
}

LexerModule lmLyric(SCLEX_LYRIC, ColouriseLyricDoc, "Lyric");
