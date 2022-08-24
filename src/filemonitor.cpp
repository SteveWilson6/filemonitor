#include "../include/filemonitor.hpp"
#include <unistd.h>
#include <cstring>

using namespace ::std::string_view_literals;

namespace wilson
{
    ::std::string_view to_string(file_event_t fe)
    {
        switch (fe)
        {
        case wilson::file_event_t::ACCESS        : return "IN_ACCESS"sv;         break;         
        case wilson::file_event_t::ATTRIB        : return "IN_ATTRIB"sv;         break;
        case wilson::file_event_t::CLOSE_WRITE   : return "IN_CLOSE_WRITE"sv;    break;
        case wilson::file_event_t::CLOSE_NOWRITE : return "IN_CLOSE_NOWRITE"sv;  break;
        case wilson::file_event_t::CREATE        : return "IN_CREATE"sv;         break;
        case wilson::file_event_t::DELETE        : return "IN_DELETE"sv;         break;
        case wilson::file_event_t::DELETE_SELF   : return "IN_DELETE_SELF"sv;    break;
        case wilson::file_event_t::MODIFY        : return "IN_MODIFY"sv;         break;
        case wilson::file_event_t::MOVE_SELF     : return "IN_MOVE_SELF"sv;      break;
        case wilson::file_event_t::MOVED_FROM    : return "IN_MOVED_FROM"sv;     break;
        case wilson::file_event_t::MOVED_TO      : return "IN_MOVED_TO"sv;       break;
        case wilson::file_event_t::OPEN          : return "IN_OPEN"sv;           break;
        }
        return ""sv;
    }

    filemonitor::filemonitor()
        : m_inotify_fd{ ::inotify_init1(IN_NONBLOCK | IN_CLOEXEC) }
    {
        if (m_inotify_fd == -1)
        {
            throw ::std::runtime_error{ ::strerror(errno) };
        }
    }

    filemonitor::~filemonitor()
    {
        for (auto& [wd, name] : m_asso_fds)
        {
            ::inotify_rm_watch(m_inotify_fd, wd);
        }
    }

    void filemonitor::add_watch(const ::std::string& path, uint32_t mask)
    {
        int wd{ ::inotify_add_watch(
            m_inotify_fd, 
            path.c_str(), 
            mask
        )};
        if (wd == -1) [[unlikely]]
        {
            const char* errmsg = ::strerror(errno);
            throw ::std::runtime_error{ errmsg };
        }
        m_asso_fds[wd] = path;
    }

    ::std::vector<modified_info>
    filemonitor::get_modified()
    {
        alignas(alignof(struct inotify_event)) char buffer[4096];
        int nread{};
        
        ::std::vector<modified_info> result;
        for (;;)
        {
            nread = ::read(m_inotify_fd, buffer, sizeof(buffer));
            if (nread == -1)
            {
                if (errno == EINTR) [[likely]] break;
                else if (errno != EAGAIN)
                {
                    const char* errmsg = ::strerror(errno);
                    throw ::std::runtime_error{ errmsg };
                }
            }
            if (nread < 0) break;
            const inotify_event* event;
            for (char* ptr = buffer; ptr < buffer + nread;
                    ptr += sizeof(inotify_event) + event->len)
            {
                modified_info cur_result{};
                event = reinterpret_cast<inotify_event*>(ptr);
                if (event->len > 0)
                    cur_result.filename = ::std::string{ event->name, event->len };
                else cur_result.filename = m_asso_fds[event->wd];
                cur_result.event = static_cast<file_event_t>(event->mask);
                cur_result.cookie = event->cookie;
                result.emplace_back(::std::move(cur_result));
            }
        }
        return result;
    }
}
