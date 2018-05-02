//
// Created by maxon on 02.05.18.
//

#ifndef UNIVERSALSTORAGE_PATHVIEW_H
#define UNIVERSALSTORAGE_PATHVIEW_H

#include <string_view>


class PathView
{
public:
    PathView(std::string_view path);

    std::string_view begin();
    std::string_view end() const;
    std::string_view next();

protected:
    std::string_view m_path;
    size_t m_start, m_end;
};


#endif //UNIVERSALSTORAGE_PATHVIEW_H
