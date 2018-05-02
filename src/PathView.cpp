//
// Created by maxon on 02.05.18.
//

#include "exceptions.h"
#include "PathView.h"

PathView::PathView(std::string_view path) : m_path(path) {}


std::string_view PathView::next()
{
    m_start = m_end + 1;
    m_end = m_path.find('/', m_start);

    if (m_end == std::string_view::npos) { // Handle last part of path /1/2/!3!
        if (m_start >= m_path.length() + 1)
            return end();
        m_end = m_path.length();
        m_start = m_path.rfind('/') + 1;
        return m_path.substr(m_start, m_end - m_start);
    }

    // TODO: Maybe, could be better to validate full path before using?
    if (m_end == m_start)
        throw BadPathException("Double '/' in path");

    return m_path.substr(m_start, m_end - m_start);
}

std::string_view PathView::begin()
{
    if (m_path.front() != '/')
        throw BadPathException("No starting '/' in path");
    m_end = 0;
    return m_path.substr(0, 1);
}

std::string_view PathView::end() const
{
    return std::string_view();
}
