#ifndef _WILSON_UNIQUE_POSIX_FD_HPP_
#define _WILSON_UNIQUE_POSIX_FD_HPP_

#include <unistd.h>
#include <utility>
#include <stdexcept>
#include <cassert>

namespace wilson
{
    class unique_posix_fd
    {
    public:
        explicit unique_posix_fd(int fd) 
            : m_fd{ fd } 
        {
            if (m_fd == -1) [[unlikely]]
                throw ::std::runtime_error{ "fd == -1" };
        }

        unique_posix_fd(unique_posix_fd&& other)
            : m_fd{ ::std::exchange(other.m_fd, 0) }
        {
            if (m_fd == -1) [[unlikely]]
                throw ::std::runtime_error{ "fd == -1" };
        }

        unique_posix_fd& operator=(unique_posix_fd& other)
        {
            m_fd = ::std::exchange(other.m_fd, 0);
            return *this;
        }

        operator int() const noexcept { return m_fd; }
        int value() const noexcept { return m_fd; }

        void close() noexcept
        {
            if (m_fd) 
            {
                int ret = ::close(m_fd);
                assert(ret == 0);
            }
        }
        
        ~unique_posix_fd()
        {
            this->close();
        }

    private:
        int m_fd;
    };
}

#endif
