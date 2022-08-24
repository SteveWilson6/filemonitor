#ifndef _WILSON_FILEMONITOR_HPP_
#define _WILSON_FILEMONITOR_HPP_

#include <string_view>
#include <unordered_map>
#include <string>
#include <cstddef>
#include <vector>
#include <sys/inotify.h>

#include "../include/unique_posix_fd.hpp"

namespace wilson
{
    enum class file_event_t : uint32_t
    {
        ACCESS        = IN_ACCESS,         
        ATTRIB        = IN_ATTRIB,
        CLOSE_WRITE   = IN_CLOSE_WRITE,
        CLOSE_NOWRITE = IN_CLOSE_NOWRITE,
        CREATE        = IN_CREATE,
        DELETE        = IN_DELETE,
        DELETE_SELF   = IN_DELETE_SELF,
        MODIFY        = IN_MODIFY,
        MOVE_SELF     = IN_MOVE_SELF,
        MOVED_FROM    = IN_MOVED_FROM,
        MOVED_TO      = IN_MOVED_TO,
        OPEN          = IN_OPEN,
    };

    struct modified_info 
    {
        ::std::string filename;
        file_event_t event;
        uint32_t cookie;
    };

    ::std::string_view to_string(file_event_t fe);

    class filemonitor
    {
    public:
        filemonitor();

        void add_watch(const ::std::string& path, uint32_t mask);
        ::std::vector<modified_info> get_modified();

        operator int() const noexcept { return m_inotify_fd; }
        
        ~filemonitor();

    private:
        unique_posix_fd m_inotify_fd;
        ::std::unordered_map<int, ::std::string> m_asso_fds;
    };
}

#endif
