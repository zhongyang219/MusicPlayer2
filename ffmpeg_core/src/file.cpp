#include "file.h"

#include <ctype.h>
#include "fileop.h"

#define chkstr(s) (s ? s : "")

int is_file(UrlParseResult* url) {
    if (!url) return 0;
    std::string scheme(chkstr(url->scheme));
    if (scheme.empty() || scheme == "file") return 1;
    if (scheme.length() == 1 && isalpha(scheme[0])) return 1;
    return 0;
}

int is_file_exists(MusicHandle* handle) {
    if (!handle || !handle->is_file) return 0;
    std::string scheme(chkstr(handle->parsed_url->scheme));
    if (scheme != "file") {
        return fileop::exists(handle->url);
    } else {
        std::string fn(chkstr(handle->parsed_url->netloc));
        fn += chkstr(handle->parsed_url->path);
        std::string query(chkstr(handle->parsed_url->query));
        if (!query.empty()) {
            fn += "?";
            fn += query;
        }
        std::string fragment(chkstr(handle->parsed_url->fragment));
        if (!fragment.empty()) {
            fn += "#";
            fn += fragment;
        }
        return fileop::exists(fn);
    }
}
