#include <gtest/gtest.h>

#include <generic_piece_table.h>

using PIECE_TABLE = MiniDoc::CharListPieceTable;
// using PIECE_TABLE = MiniDoc::StringPieceTable;
// using PIECE_TABLE = MiniDoc::String;

TEST(string_table_origin, insert_origin) {
    PIECE_TABLE table;
    auto copy = table;

    for (int i = 0; i < 5; i++) table.append_origin("hello world! ");

    #define ERA(a, content) ASSERT_EQ(copy.string().insert(a, content), copy.insert_origin(content, a).string());

    copy = table;
    // copy.debug = true;
    ERA(3, "bye");
    ERA(1, "no");
    // copy.debug = true;
    ERA(6, "why");
    ERA(6, "because i said so");
    ERA(8, " who cares");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, "i care !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, "well i dont !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, "yes you do");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, "fine then");

    #undef ERA
}

TEST(string_table_origin, erase_origin) {
    PIECE_TABLE table;
    auto copy = table;

    for (int i = 0; i < 5; i++) table.append_origin("hello world! ");

    #define ERA(a, b) ASSERT_EQ(copy.string().erase(a, b), copy.erase_origin(a, b).string());

    copy = table;
    ERA(3, 2);
    // copy.debug = true;
    ERA(1, 1);
    ERA(6, 4);
    ERA(6, 4);
    ERA(8, 2);
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 2);
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 1);
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 0);
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 0);

    #undef ERA
}

TEST(string_table_origin, erase_origin__insert_origin) {
    PIECE_TABLE table;
    for (int i = 0; i < 5; i++) table.append_origin("hello world! ");
    // table.debug = true;
    auto copy = table;

    #define ERA(a, b, content) \
    ASSERT_EQ(copy.string().erase(a, b), copy.erase_origin(a, b).string()); \
    ASSERT_EQ(copy.string().insert(a, content), copy.insert_origin(content, a).string()); \

    PIECE_TABLE table2;
    table2.append("hlbye world! hello world! hello world! hello world! hello world! ");
    // table2.debug = true;
    ASSERT_EQ(table2.string().insert(1, "no"), table2.insert_origin("no", 1).string());

    copy = table;
    ERA(3, 2, "bye");
    ERA(1, 1, "no");
    ERA(6, 4, "why");
    ERA(6, 4, "because i said so");
    ERA(8, 2, " who cares");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 2, "i care !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 1, "well i dont !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 0, "yes you do");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 0, "fine then");

    #undef ERA
}

TEST(string_table_origin, replace_origin) {
    PIECE_TABLE table;
    auto copy = table;

    for (int i = 0; i < 5; i++) table.append_origin("hello world! ");
    #define ERA(a, b, content) ASSERT_EQ(copy.string().replace(a, b, content), copy.replace_origin(content, a, b).string());

    copy = table;
    ERA(3, 2, "bye");
    ERA(1, 1, "no");
    ERA(6, 4, "why");
    ERA(6, 4, "because i said so");
    ERA(8, 2, " who cares");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 2, "i care !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 1, "well i dont !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 0, "yes you do");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 0, "fine then");

    #undef ERA
}

TEST(string_table_append, insert) {
    PIECE_TABLE table;
    auto copy = table;

    for (int i = 0; i < 5; i++) table.append("hello world! ");

    #define ERA(a, content) ASSERT_EQ(copy.string().insert(a, content), copy.insert(content, a).string());

    copy = table;
    // copy.debug = true;
    ERA(3, "bye");
    ERA(1, "no");
    // copy.debug = true;
    ERA(6, "why");
    ERA(6, "because i said so");
    ERA(8, " who cares");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, "i care !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, "well i dont !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, "yes you do");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, "fine then");

    #undef ERA
}

TEST(string_table_append, erase) {
    PIECE_TABLE table;
    auto copy = table;

    for (int i = 0; i < 5; i++) table.append("hello world! ");

    #define ERA(a, b) ASSERT_EQ(copy.string().erase(a, b), copy.erase(a, b).string());

    copy = table;
    ERA(3, 2);
    ERA(1, 1);
    ERA(6, 4);
    ERA(6, 4);
    ERA(8, 2);
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 2);
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 1);
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 0);
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 0);

    #undef ERA
}

TEST(string_table_append, erase__insert) {
    PIECE_TABLE table;
    for (int i = 0; i < 5; i++) table.append("hello world! ");
    // table.debug = true;
    auto copy = table;

    #define ERA(a, b, content) \
    ASSERT_EQ(copy.string().erase(a, b), copy.erase(a, b).string()); \
    ASSERT_EQ(copy.string().insert(a, content), copy.insert(content, a).string()); \

    PIECE_TABLE table2;
    table2.append("hlbye world! hello world! hello world! hello world! hello world! ");
    // table2.debug = true;
    ASSERT_EQ(table2.string().insert(1, "no"), table2.insert("no", 1).string());

    copy = table;
    ERA(3, 2, "bye");
    ERA(1, 1, "no");
    ERA(6, 4, "why");
    ERA(6, 4, "because i said so");
    ERA(8, 2, " who cares");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 2, "i care !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 1, "well i dont !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 0, "yes you do");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 0, "fine then");

    #undef ERA
}

TEST(string_table_append, replace) {
    PIECE_TABLE table;
    auto copy = table;

    for (int i = 0; i < 5; i++) table.append("hello world! ");
    #define ERA(a, b, content) ASSERT_EQ(copy.string().replace(a, b, content), copy.replace(content, a, b).string());

    copy = table;
    ERA(3, 2, "bye");
    ERA(1, 1, "no");
    ERA(6, 4, "why");
    ERA(6, 4, "because i said so");
    ERA(8, 2, " who cares");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 2, "i care !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 1, "well i dont !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 0, "yes you do");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 0, "fine then");

    #undef ERA
}

TEST(string_table_origin, insert) {
    PIECE_TABLE table;
    auto copy = table;

    for (int i = 0; i < 5; i++) table.append_origin("hello world! ");

    #define ERA(a, content) ASSERT_EQ(copy.string().insert(a, content), copy.insert(content, a).string());

    copy = table;
    // copy.debug = true;
    ERA(3, "bye");
    ERA(1, "no");
    // copy.debug = true;
    ERA(6, "why");
    ERA(6, "because i said so");
    ERA(8, " who cares");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, "i care !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, "well i dont !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, "yes you do");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, "fine then");

    #undef ERA
}

TEST(string_table_origin, erase) {
    PIECE_TABLE table;
    // table.debug = true;
    auto copy = table;

    for (int i = 0; i < 5; i++) table.append_origin("hello world! ");

    #define ERA(a, b) ASSERT_EQ(copy.string().erase(a, b), copy.erase(a, b).string());

    copy = table;
    ERA(3, 2);
    ERA(1, 1);
    ERA(6, 4);
    ERA(6, 4);
    ERA(8, 2);
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 2);
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 1);
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 0);
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 0);

    #undef ERA
}

TEST(string_table_origin, erase__insert) {
    PIECE_TABLE table;
    for (int i = 0; i < 5; i++) table.append_origin("hello world! ");
    // table.debug = true;
    auto copy = table;

    #define ERA(a, b, content) \
    ASSERT_EQ(copy.string().erase(a, b), copy.erase(a, b).string()); \
    ASSERT_EQ(copy.string().insert(a, content), copy.insert(content, a).string()); \

    PIECE_TABLE table2;
    table2.append("hlbye world! hello world! hello world! hello world! hello world! ");
    // table2.debug = true;
    ASSERT_EQ(table2.string().insert(1, "no"), table2.insert("no", 1).string());

    copy = table;
    ERA(3, 2, "bye");
    ERA(1, 1, "no");
    ERA(6, 4, "why");
    ERA(6, 4, "because i said so");
    ERA(8, 2, " who cares");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 2, "i care !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 1, "well i dont !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 0, "yes you do");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 0, "fine then");

    #undef ERA
}

TEST(string_table_origin, replace) {
    PIECE_TABLE table;
    auto copy = table;

    for (int i = 0; i < 5; i++) table.append_origin("hello world! ");
    #define ERA(a, b, content) ASSERT_EQ(copy.string().replace(a, b, content), copy.replace(content, a, b).string());

    copy = table;
    ERA(3, 2, "bye");
    ERA(1, 1, "no");
    ERA(6, 4, "why");
    ERA(6, 4, "because i said so");
    ERA(8, 2, " who cares");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 2, "i care !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 1, "well i dont !");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(1, 0, "yes you do");
    copy = table;
    for (int i = 0; i < 7; i++) ERA(0, 0, "fine then");

    #undef ERA
}

TEST(string_table_mixed, mixed) {
    PIECE_TABLE table;
    table.append("append");
    table.append_origin("origin is games");
    table.append("append");
    table.append_origin("origin is food");
    ASSERT_EQ(table.string(), "appendorigin is gamesappendorigin is food");
    table.erase(6, 6);
    ASSERT_EQ(table.string(), "append is gamesappendorigin is food");
    table.append_origin("origin is nothing");
    ASSERT_EQ(table.string(), "append is gamesappendorigin is foodorigin is nothing");
    table.replace_origin("origin is a mess", 0, 17);
    ASSERT_EQ(table.string(), "origin is a messpendorigin is foodorigin is nothing");
    // table.debug = true;
    // table.debug_operations = true;
    table.replace("FOO FIGHTERS", 3, 7);
    ASSERT_EQ(table.string(), "oriFOO FIGHTERSa messpendorigin is foodorigin is nothing");
    table.replace_origin("X", 1, 20);
    ASSERT_EQ(table.string(), "oXpendorigin is foodorigin is nothing");
    ASSERT_EQ(table[0], 'o');
    ASSERT_EQ(table[1], 'X');
    ASSERT_EQ(table[2], 'p');
    ASSERT_EQ(table[3], 'e');
    ASSERT_EQ(table[4], 'n');
    ASSERT_EQ(table[5], 'd');
    ASSERT_EQ(table[6], 'o');
    ASSERT_EQ(table[7], 'r');

    table.replace("123456789", 0, -1);
    ASSERT_EQ(table.string(), "123456789");
    ASSERT_EQ(table.string().erase(3, 3), table.erase(3, 3).string());
    ASSERT_EQ(table.string().erase(2, 2), table.erase(2, 2).string());

    table.replace("12345789", 0, -1);
    ASSERT_EQ(table.string(), "12345789");
    ASSERT_EQ(table.string().erase(3, 3), table.erase(3, 3).string());
    ASSERT_EQ(table.string().erase(2, 2), table.erase(2, 2).string());
}
