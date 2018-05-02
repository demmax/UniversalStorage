//
// Created by maxon on 02.05.18.
//

#include <gtest/gtest.h>
#include <exceptions.h>
#include "PathView.h"

TEST(PathViewOkTest, PathViewTest)
{
    std::string path = "/asd/qwe/zxc";
    std::string parts[] = {"/", "asd", "qwe", "zxc"};
    PathView view(path);

    int i = 0;
    for (auto path_part = view.begin(); path_part != view.end(); path_part = view.next()) {
        EXPECT_EQ(path_part, parts[i++]);
    }
    EXPECT_EQ(i, sizeof(parts)/sizeof(std::string));
}


TEST(PathViewEndsWithSlashTest, PathViewTest)
{
    std::string path = "/asd/qwe/zxc/";
    std::string parts[] = {"/", "asd", "qwe", "zxc"};
    PathView view(path);

    int i = 0;
    for (auto path_part = view.begin(); path_part != view.end(); path_part = view.next()) {
        EXPECT_EQ(path_part, parts[i++]);
    }
    EXPECT_EQ(i, sizeof(parts)/sizeof(std::string));
}

TEST(PathViewWrongPathTest, PathViewTest)
{
    std::string path = "//asd//qwe/";
    PathView view(path);
    bool was_exception = false;
    auto path_part = view.begin();
    try {
        path_part = view.next();
    }
    catch (const BadPathException &) {
        was_exception = true;
    }
    EXPECT_EQ(was_exception, true);
}