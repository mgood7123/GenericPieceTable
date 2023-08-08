#ifndef MINIDOC_GENERIC_PIECE_TABLE_H
#define MINIDOC_GENERIC_PIECE_TABLE_H

#include <string>
#include <functional>
#include <iostream>
#include <cstring>
#include <list>
#include <forward_list>
#include <algorithm>
#include <cassert>

#ifndef MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE
#define MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE std::function
#endif

#define GENERIC_PIECE_TABLE__PRINTF(x) { std::cout << #x << ": " << (x) << std::endl; }
#define GENERIC_PIECE_TABLE__PRINT_BOOLF(x) { std::cout << #x << ": " << ((x) ? "true" : "false") << std::endl; }
#define GENERIC_PIECE_TABLE__PRINT_STRINGF(x) { std::cout << #x << ": " << std::to_string((x)) << std::endl; }
#define GENERIC_PIECE_TABLE__PRINT_POINTERF(x) { auto * p = (x); if (p == nullptr) { std::cout << #x << ": nullptr" << std::endl; } else { std::cout << #x << ": " << p << std::endl; } }

#define GENERIC_PIECE_TABLE__PRINT(x) { if (debug) { std::cout << #x << ": " << (x) << std::endl; } }
#define GENERIC_PIECE_TABLE__PRINT_BOOL(x) { if (debug) { std::cout << #x << ": " << ((x) ? "true" : "false") << std::endl; } }
#define GENERIC_PIECE_TABLE__PRINT_STRING(x) { if (debug) { std::cout << #x << ": " << std::to_string((x)) << std::endl; } }
#define GENERIC_PIECE_TABLE__PRINT_POINTER(x) { auto * p = (x); if (debug) { if (p == nullptr) { std::cout << #x << ": nullptr" << std::endl; } else { std::cout << #x << ": " << p << std::endl; } } }

#define GENERIC_PIECE_TABLE__EMPTY_DESCRIPTOR {0, 0, {}}

namespace MiniDoc {

    struct GenericPieceTableUserData {

        using COPY_T = MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void*(const void*)>;
        using DELETE_T = MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(void* &)>;
        
        std::optional<void *> user_data = std::nullopt;

        std::optional<COPY_T> copy = std::nullopt;

        std::optional<DELETE_T> deleter = std::nullopt;

        GenericPieceTableUserData() {}

        GenericPieceTableUserData(void* data) : user_data(data) {}

        GenericPieceTableUserData(void* data, const COPY_T & copy, const DELETE_T & deleter)
        : user_data(data), copy(copy), deleter(deleter) {}

        void copy_user_data(const std::optional<void *> & src, std::optional<void *> & dst) {
            dst = copy.value()(src.value());
            // std::cout << "copied user data: " << src.value() << " -> " << dst.value() << std::endl;
        }

        void move_user_data(std::optional<void *> & src, std::optional<void *> & dst) {
            dst = src.value();
            src = std::nullopt;
            // std::cout << "moved user data: 0 -> " << dst.value() << std::endl;
        }

        void copy_copy(const std::optional<COPY_T> & src, std::optional<COPY_T> & dst) {
            dst = src.value();
        }

        void move_copy(std::optional<COPY_T> & src, std::optional<COPY_T> & dst) {
            dst = src.value();
            src = std::nullopt;
        }

        void copy_deleter(const std::optional<DELETE_T> & src, std::optional<DELETE_T> & dst) {
            dst = src.value();
        }

        void move_deleter(std::optional<DELETE_T> & src, std::optional<DELETE_T> & dst) {
            dst = src.value();
            src = std::nullopt;
        }

        void call_deleter() {
            deleter.value()(user_data.value());
            user_data = std::nullopt;
        }

        GenericPieceTableUserData(const GenericPieceTableUserData & other) {
            if (&other != this) {
                if (other.copy.has_value()) copy_copy(other.copy, copy);
                if (other.deleter.has_value()) copy_deleter(other.deleter, deleter);
                if (other.user_data.has_value() && copy.has_value()) copy_user_data(other.user_data, user_data);
            }
        }
        GenericPieceTableUserData & operator=(const GenericPieceTableUserData & other) {
            if (&other != this) {
                if (user_data.has_value() && deleter.has_value()) call_deleter();
                if (other.copy.has_value()) copy_copy(other.copy, copy);
                if (other.deleter.has_value()) copy_deleter(other.deleter, deleter);
                if (other.user_data.has_value() && copy.has_value()) copy_user_data(other.user_data, user_data);
            }
            return *this;
        }
        GenericPieceTableUserData(GenericPieceTableUserData && other) {
            if (&other != this) {
                if (other.copy.has_value()) move_copy(other.copy, copy);
                if (other.deleter.has_value()) move_deleter(other.deleter, deleter);
                if (other.user_data.has_value()) move_user_data(other.user_data, user_data);
            }
        }
        GenericPieceTableUserData & operator=(GenericPieceTableUserData && other) {
            if (&other != this) {
                if (user_data.has_value() && deleter.has_value()) call_deleter();
                if (other.copy.has_value()) move_copy(other.copy, copy);
                if (other.deleter.has_value()) move_deleter(other.deleter, deleter);
                if (other.user_data.has_value()) move_user_data(other.user_data, user_data);
            }
            return *this;
        }
        virtual ~GenericPieceTableUserData() {
            if (user_data.has_value() && deleter.has_value()) call_deleter();
        }
    };

    struct GenericPieceTableDescriptor {
        std::size_t start = 0;
        std::size_t length = 0;
        GenericPieceTableUserData user_data;
        GenericPieceTableDescriptor() = default;
        GenericPieceTableDescriptor(std::size_t start, std::size_t length, const GenericPieceTableUserData & user_data) : start(start), length(length), user_data(user_data) {}
    };

    struct GenericPieceTableDescriptorOrder {
        bool origin = false;
        GenericPieceTableDescriptor * ptr = nullptr;
    };

    template <typename CHAR_CONTAINER_T, typename BUFFER_CONTAINER_T>
    struct GenericPieceTableFunctions {
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(BUFFER_CONTAINER_T & container)> reset;
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(BUFFER_CONTAINER_T & container, CHAR_CONTAINER_T & content)> append;
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<std::size_t(const BUFFER_CONTAINER_T & container)> container_length;
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<std::size_t(CHAR_CONTAINER_T & content)> content_length;
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<const char(const BUFFER_CONTAINER_T & container, std::size_t index)> container_index_to_char;
        // user data insert
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(void * instance, const bool & debug, GenericPieceTableUserData & user_data, const std::size_t & start, const CHAR_CONTAINER_T & content, const std::size_t & content_length)> act_on_user_data_insert;
        // user data split
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(void * instance, const bool & debug,
            GenericPieceTableUserData & user_data_left, const std::size_t & start_left, const std::size_t & length_left,
            GenericPieceTableUserData & user_data_right
        )> act_on_user_data_split;
        // user data erase
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(void * instance, const bool & debug, GenericPieceTableUserData & user_data, const std::size_t & start, const std::size_t & length, const bool & is_start)> act_on_user_data_erase;
    };

    template <typename DESCRIPTOR_CONTAINER_T>
    struct GenericPieceTableDescriptorFunctions {
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(DESCRIPTOR_CONTAINER_T & container)> reset;
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(DESCRIPTOR_CONTAINER_T & container, const GenericPieceTableDescriptor & descriptor)> append;
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<std::size_t(const DESCRIPTOR_CONTAINER_T & container)> length;
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<const GenericPieceTableDescriptor & (const DESCRIPTOR_CONTAINER_T & container, std::size_t index)> const_index;
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<GenericPieceTableDescriptor & (DESCRIPTOR_CONTAINER_T & container, std::size_t index)> index;
    };

    template <typename DESCRIPTOR_ORDER_CONTAINER_T>
    struct GenericPieceTableDescriptorOrderFunctions {
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(DESCRIPTOR_ORDER_CONTAINER_T & container)> reset;
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(DESCRIPTOR_ORDER_CONTAINER_T & container, const GenericPieceTableDescriptorOrder & descriptorOrder, std::size_t index)> insert;
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<std::size_t(const DESCRIPTOR_ORDER_CONTAINER_T & container)> length;
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<const GenericPieceTableDescriptorOrder & (const DESCRIPTOR_ORDER_CONTAINER_T & container, std::size_t index)> const_index;
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<GenericPieceTableDescriptorOrder & (DESCRIPTOR_ORDER_CONTAINER_T & container, std::size_t index)> index;
    };

    template <typename DESCRIPTOR_CONTAINER_T, typename DESCRIPTOR_ORDER_CONTAINER_T, typename ORIGIN_CHAR_CONTAINER_T, typename APPEND_CHAR_CONTAINER_T, typename ORIGIN_BUFFER_CONTAINER_T, typename APPEND_BUFFER_CONTAINER_T>
    struct GenericPieceTable {

        using THIS = GenericPieceTable<DESCRIPTOR_CONTAINER_T, DESCRIPTOR_ORDER_CONTAINER_T, ORIGIN_CHAR_CONTAINER_T, APPEND_CHAR_CONTAINER_T, ORIGIN_BUFFER_CONTAINER_T, APPEND_BUFFER_CONTAINER_T>;

        using USER_DATA_USER_DATA_T = GenericPieceTableUserData &;
        using USER_DATA_START_T = const std::size_t &;
        using USER_DATA_ORIGIN_CONTENT_T = const ORIGIN_CHAR_CONTAINER_T &;
        using USER_DATA_APPEND_CONTENT_T = const APPEND_CHAR_CONTAINER_T &;
        using USER_DATA_LENGTH_T = const std::size_t &;
        using USER_DATA_FLAGS_T = const bool &;

        template <typename BUFFER, typename CHAR>
        struct Info {
            BUFFER buffer;
            DESCRIPTOR_CONTAINER_T pieces;
            GenericPieceTableFunctions<CHAR, BUFFER> functions;

            std::size_t container_length() const {
                return functions.container_length(buffer);
            }

            const char container_index_to_char(std::size_t index) const {
                return functions.container_index_to_char(buffer, index);
            }

            void reset_buffer() {
                functions.reset(buffer);
            }
        };

        private:

        Info<ORIGIN_BUFFER_CONTAINER_T, ORIGIN_CHAR_CONTAINER_T> origin_info;
        Info<APPEND_BUFFER_CONTAINER_T, APPEND_CHAR_CONTAINER_T> append_info;
        
        DESCRIPTOR_ORDER_CONTAINER_T piece_order;
        std::size_t piece_order_size = 0;

        GenericPieceTableDescriptorFunctions<DESCRIPTOR_CONTAINER_T> descriptor_functions;
        GenericPieceTableDescriptorOrderFunctions<DESCRIPTOR_ORDER_CONTAINER_T> descriptor_order_functions;

        public:

        const Info<ORIGIN_BUFFER_CONTAINER_T, ORIGIN_CHAR_CONTAINER_T> & get_origin_info() const {
            return origin_info;
        }

        const Info<APPEND_BUFFER_CONTAINER_T, ORIGIN_CHAR_CONTAINER_T> & get_append_info() const {
            return append_info;
        }

        bool debug = false;
        bool debug_operations = false;

        enum LAST_OP { LAST_OP_INSERT, LAST_OP_REPLACE, LAST_OP_ERASE };
        enum LAST_BUFFER { LAST_BUFFER_ORIGIN, LAST_BUFFER_APPEND };

        LAST_OP last_op;
        LAST_BUFFER last_buffer;

        std::size_t last_calculated_insert_position_start;
        std::size_t last_calculated_replace_position_start;
        std::size_t last_calculated_replace_length;
        std::size_t last_calculated_erase_position_start;
        std::size_t last_calculated_erase_length;

        GenericPieceTable() = default;

        GenericPieceTable(
            GenericPieceTableDescriptorFunctions<DESCRIPTOR_CONTAINER_T> descriptor_functions,
            GenericPieceTableDescriptorOrderFunctions<DESCRIPTOR_ORDER_CONTAINER_T> descriptor_order_functions,
            GenericPieceTableFunctions<ORIGIN_CHAR_CONTAINER_T, ORIGIN_BUFFER_CONTAINER_T> origin_functions,
            GenericPieceTableFunctions<APPEND_CHAR_CONTAINER_T, APPEND_BUFFER_CONTAINER_T> append_functions
        ) :
        descriptor_functions(descriptor_functions), descriptor_order_functions(descriptor_order_functions),
        origin_info({{}, {}, origin_functions}), append_info({{}, {}, append_functions})
        {};

        private:

        void copy(const THIS & other) {
            descriptor_functions = other.descriptor_functions;
            descriptor_order_functions = other.descriptor_order_functions;
            last_op = other.last_op;
            last_buffer = other.last_buffer;
            last_calculated_insert_position_start = other.last_calculated_insert_position_start;
            last_calculated_replace_position_start = other.last_calculated_replace_position_start;
            last_calculated_replace_length = other.last_calculated_replace_length;
            last_calculated_erase_position_start = other.last_calculated_erase_position_start;
            last_calculated_erase_length = other.last_calculated_erase_length;
            origin_info = other.origin_info;
            append_info = other.append_info;
            piece_order = other.piece_order;
            piece_order_size = other.piece_order_size;
            debug = other.debug;
            debug_operations = other.debug_operations;
            for (size_t i = 0; i < piece_order_size; i++) {
                auto & order = descriptor_order_functions.index(piece_order, i);
                auto & other_order = descriptor_order_functions.const_index(other.piece_order, i);
                if (order.origin) {
                    std::size_t len = descriptor_functions.length(origin_info.pieces);
                    for (std::size_t i = 0; i < len; i++) {
                        if (&descriptor_functions.const_index(other.origin_info.pieces, i) == other_order.ptr) {
                            if (debug) {
                                std::cout << "fix origin reference" << std::endl;
                            }
                            order.ptr = &descriptor_functions.index(origin_info.pieces, i);
                        }
                    }
                } else {
                    std::size_t len = descriptor_functions.length(append_info.pieces);
                    for (std::size_t i = 0; i < len; i++) {
                        if (&descriptor_functions.const_index(other.append_info.pieces, i) == other_order.ptr) {
                            if (debug) {
                                std::cout << "fix append reference" << std::endl;
                            }
                            order.ptr = &descriptor_functions.index(append_info.pieces, i);
                        }
                    }
                }
            }
        }

        public:

        GenericPieceTable(const THIS & other) {
            copy(other);
        }

        THIS & operator= (const THIS & other) {
            copy(other);
            return *this;
        }

        virtual ~GenericPieceTable() {}

        std::size_t descriptor_count() const {
            return descriptor_order_functions.length(piece_order);
        }

        void THROW(const char * what) const {
            throw std::runtime_error(what);
        }

        const GenericPieceTableDescriptorOrder & descriptor_at(std::size_t index) const {
            auto s = descriptor_order_functions.length(piece_order);
            if (s == 0) {
                THROW("index out of range");
            }
            return descriptor_order_functions.const_index(piece_order, index < s-1 ? index : s-1);
        }

        std::string range_string_len(std::size_t start, std::size_t length) const {
            std::string string;
            range_string_len(start, length, string);
            return string;
        }

        void range_string_len(std::size_t start, std::size_t length, std::string & out) const {
            if (length == 0) {
                return;
            }
            auto end = this->length();
            auto position_ = start >= end ? end : start;
            if (position_ == end) {
                return;
            }
            if (length == -1) {
                length = end;
            }
            auto position_end = position_ + length;
            if (position_end >= end) {
                position_end = end;
            }
            auto length_ = position_end - position_;
            if (length_ == 0) {
                return;
            }
            return range_string(position_, position_end, out);
        }

        std::string range_string(std::size_t start, std::size_t end) const {
            std::string string;
            range_string(start, end, string);
            return string;
        }

        void range_string(std::size_t start, std::size_t end, std::string & out) const {
            auto len = this->length();
            if (end >= len) {
                end = len;
            }
            if (start >= len) {
                start = len;
            }
            if (start >= end) {
                start = end;
            }
            len = end - start;
            if (len == 0) {
                out.resize(0);
                return;
            }

            out.resize(len);

            std::size_t LEN = 0;
            std::size_t COUNT = 0;
            bool found_start = false;
            for (size_t i = 0; i < piece_order_size; i++) {
                auto & order = descriptor_order_functions.const_index(piece_order, i);
                auto & descriptor = *order.ptr;
                if (descriptor.length == 0) continue;
                auto next_LEN = LEN + descriptor.length;
                if (start < next_LEN) {
                    auto buffer_index = descriptor.start;
                    if (!found_start) {
                        found_start = true;
                        buffer_index += (start - LEN);
                    }
                    if (order.origin) {
                        for (auto i_ = buffer_index; i_ < (descriptor.start + descriptor.length); i_++) {
                            out[COUNT] = origin_info.container_index_to_char(i_);
                            COUNT++;
                            if (COUNT >= len) {
                                return;
                            }
                        }
                    } else {
                        for (auto i_ = buffer_index; i_ < (descriptor.start + descriptor.length); i_++) {
                            out[COUNT] = append_info.container_index_to_char(i_);
                            COUNT++;
                            if (COUNT >= len) {
                                return;
                            }
                        }
                    }
                } else {
                    LEN = next_LEN;
                }
            }
        }

        const char operator[](std::size_t index) const {
            auto l = length();
            if (l == 0) {
                THROW("index out of range");
            }

            if (index == -1 || index > (l-1)) index = l-1;

            std::size_t LEN = 0;
            if (debug) {
                std::cout << "piece_order_size: " << std::to_string(piece_order_size) << std::endl;
            }
            for (size_t i = 0; i < piece_order_size; i++) {
                auto & order = descriptor_order_functions.const_index(piece_order, i);
                auto & descriptor = *order.ptr;
                if (descriptor.length == 0) continue;
                auto next_LEN = LEN + descriptor.length;
                if (debug) {
                    std::cout << "index: " << std::to_string(index) << ", next_LEN: " << std::to_string(next_LEN) << std::endl;
                }
                if (index < next_LEN) {
                    // avoid looping through the entire descriptor to find our index
                    GENERIC_PIECE_TABLE__PRINT_STRING(index);
                    GENERIC_PIECE_TABLE__PRINT_STRING(LEN);
                    GENERIC_PIECE_TABLE__PRINT_STRING(index - LEN);
                    auto buffer_index = descriptor.start + (index - LEN);
                    GENERIC_PIECE_TABLE__PRINT_STRING(buffer_index);
                    auto char_ = (order.origin ? origin_info.container_index_to_char(buffer_index) : append_info.container_index_to_char(buffer_index));
                    if (debug) {
                        std::cout << "lookup return character " << char_ << std::endl;
                    }
                    return char_;
                }
                LEN = next_LEN;
            }
            THROW("lookup reached end");
            return '\0';
        }

        private:

        template <typename T1, typename T2, typename T3>
        struct triple {
            T1 first;
            T2 second;
            T3 third;
        };

        triple<const GenericPieceTableDescriptorOrder *, std::size_t, std::size_t>
        lookup(std::size_t position) const {
            std::size_t LEN = 0;
            if (debug) {
                std::cout << "piece_order_size: " << std::to_string(piece_order_size) << std::endl;
            }
            for (size_t i = 0; i < piece_order_size; i++) {
                auto & order = descriptor_order_functions.const_index(piece_order, i);
                auto & descriptor = *order.ptr;
                if (descriptor.length == 0) continue;
                auto next_LEN = LEN + descriptor.length;
                if (debug) {
                    std::cout << "position: " << std::to_string(position) << ", next_LEN: " << std::to_string(next_LEN) << std::endl;
                }
                if (position < next_LEN) {
                    if (debug) {
                        // avoid looping through the entire descriptor to find our index
                        auto buffer_index = descriptor.start + (position - LEN);
                        auto char_ = (order.origin ? origin_info.container_index_to_char(buffer_index) : append_info.container_index_to_char(buffer_index));
                        std::cout << "lookup return character " << char_ << std::endl;
                    }
                    return {&order, i, LEN};
                }
                LEN = next_LEN;
            }
            if (debug) {
                std::cout << "lookup reached end" << std::endl;
            }
            return {nullptr, piece_order_size, LEN};
        }

        /*
            to split a buffer, we first check if split position is zero

            if zero, then we insert a piece at the beginning and set it as the split middle

            if not zero, we lookup the ordered piece information for the specified split position,

            we then check if the split position matches the very end of the ordered pieces based on the
                lookup info

            if the split position matches the very end, then we insert a piece at the end and set it as the
                split middle

            otherwise do the actual split

            we simply assign the LHS to the lookup item, the middle to a new piece, and the RHS to the
                next ordered piece if we are splitting a 1-length piece, otherwise we assign the RHS to a
                new piece
        */

        template <typename INFO>
        GenericPieceTableDescriptor & split_insert(bool is_origin, INFO & info, std::size_t clamped_position_start) {

            GENERIC_PIECE_TABLE__PRINT_STRING(clamped_position_start);

            // no use splitting at the very start of the buffer
            if (clamped_position_start == 0) {
                auto & piece = info.pieces;
                auto len = descriptor_functions.length(piece);
                descriptor_functions.append(piece, GENERIC_PIECE_TABLE__EMPTY_DESCRIPTOR);
                auto & insert_piece = descriptor_functions.index(piece, len);
                descriptor_order_functions.insert(piece_order, {is_origin, &insert_piece}, 0);
                piece_order_size++;
                return insert_piece;
            }
            
            auto target_start = lookup(clamped_position_start);
            
            GENERIC_PIECE_TABLE__PRINT_STRING(target_start.third);

            if (debug) {
                std::cout << "splitting buffer" << std::endl;
                std::cout << "looking up piece at position " << std::to_string(clamped_position_start) << std::endl;
            }
            bool is_end = target_start.first == nullptr;
            GENERIC_PIECE_TABLE__PRINT_BOOL(is_end);

            // no use splitting at the very end of the buffer
            if (is_end) {
                auto & piece = info.pieces;
                auto len = descriptor_functions.length(piece);
                descriptor_functions.append(piece, GENERIC_PIECE_TABLE__EMPTY_DESCRIPTOR);
                auto & insert_piece = descriptor_functions.index(piece, len);
                descriptor_order_functions.insert(piece_order, {is_origin, &insert_piece}, piece_order_size);
                piece_order_size++;
                return insert_piece;
            }

            GENERIC_PIECE_TABLE__PRINT_BOOL(target_start.third == clamped_position_start);

            if (target_start.third == clamped_position_start) {
                auto & piece = info.pieces;
                auto len = descriptor_functions.length(piece);
                descriptor_functions.append(piece, GENERIC_PIECE_TABLE__EMPTY_DESCRIPTOR);
                auto & insert_piece = descriptor_functions.index(piece, len);
                descriptor_order_functions.insert(piece_order, {is_origin, &insert_piece}, target_start.second);
                piece_order_size++;
                return insert_piece;
            } else {
                auto diff = clamped_position_start - target_start.third;
                GENERIC_PIECE_TABLE__PRINT_STRING(target_start.first->ptr->start);
                GENERIC_PIECE_TABLE__PRINT_STRING(target_start.first->ptr->length);
                GENERIC_PIECE_TABLE__PRINT_STRING(diff);
                auto old_length = target_start.first->ptr->length;
                target_start.first->ptr->length = diff;

                auto & piece = info.pieces;
                auto len = descriptor_functions.length(piece);
                descriptor_functions.append(piece, GENERIC_PIECE_TABLE__EMPTY_DESCRIPTOR);
                auto & insert_piece = descriptor_functions.index(piece, len);
                descriptor_order_functions.insert(piece_order, {is_origin, &insert_piece}, target_start.second+1);
                piece_order_size++;

                auto & piece2 = target_start.first->origin ? origin_info.pieces : append_info.pieces;
                auto len2 = descriptor_functions.length(piece2);
                GENERIC_PIECE_TABLE__PRINT_STRING(target_start.first->ptr->start + diff);
                GENERIC_PIECE_TABLE__PRINT_STRING(old_length);
                GENERIC_PIECE_TABLE__PRINT_STRING(target_start.first->ptr->start - ((target_start.first->ptr->start + diff) - old_length));
                GENERIC_PIECE_TABLE__PRINT_STRING(old_length - diff);
                descriptor_functions.append(piece2, {target_start.first->ptr->start + diff, old_length - diff, nullptr});
                auto & insert_piece2 = descriptor_functions.index(piece2, len2);
                if (debug) std::cout << "USER DATA SPLIT" << std::endl;
                if (target_start.first->origin) {
                    origin_info.functions.act_on_user_data_split(this, debug, target_start.first->ptr->user_data, target_start.first->ptr->start, target_start.first->ptr->length, insert_piece2.user_data);
                } else {
                    append_info.functions.act_on_user_data_split(this, debug, target_start.first->ptr->user_data, target_start.first->ptr->start, target_start.first->ptr->length, insert_piece2.user_data);
                }
                descriptor_order_functions.insert(piece_order, {target_start.first->origin, &insert_piece2}, target_start.second+2);
                piece_order_size++;
                return insert_piece;
            }
        }

        /*
            to split a buffer, we first check if ordered pieces buffer is empty

            if empty, then we insert a piece at the beginning and set it as the split middle

            if not empty, we lookup the ordered piece information for the specified split position,

            we then check if the split position matches the very end of the ordered pieces based on the
                lookup info

            if we are inserting to the very end, then we insert a piece at the end and set it as the
                split middle

            otherwise do the actual split

            we simply assign the LHS to the lookup item, the middle to a new piece, and the RHS to the
                next ordered piece if we are splitting a 1-length piece, otherwise we assign the RHS to a
                new piece
        */

        protected:

        virtual void onReset() {};

        public:

        THIS & reset() {
            onReset();
            // erase all pieces
            //
            descriptor_functions.reset(origin_info.pieces);
            descriptor_functions.reset(append_info.pieces);
            descriptor_order_functions.reset(piece_order);
            piece_order_size = 0;

            // it makes no sense to keep the origin buffer contents after erasing all pieces
            //
            origin_info.reset_buffer();

            // it makes no sense to keep the append buffer contents after erasing all pieces
            //
            append_info.reset_buffer();
            return *this;
        }

        THIS & clear_origin() {
            return erase_origin(0, -1);
        }

        THIS & clear() {
            return erase(0, -1);
        }

        private:

        template <typename INFO>
        void split_erase(bool is_origin, INFO & info, std::size_t clamped_position_start, std::size_t clamped_position_end) {
            auto target_end = lookup(clamped_position_end);
            bool is_end = target_end.first == nullptr;
            GENERIC_PIECE_TABLE__PRINT_BOOL(is_end);
            GENERIC_PIECE_TABLE__PRINT_BOOL(clamped_position_start == 0);
            GENERIC_PIECE_TABLE__PRINT_STRING(clamped_position_start);
            GENERIC_PIECE_TABLE__PRINT_STRING(clamped_position_end);
            if (is_end) {
                if (clamped_position_start == 0) {

                    // we know the entire list is getting erased, so reset everything as if *this = {};
                    //
                    reset();
                    return;
                }
            }

            auto target_start = lookup(clamped_position_start);

            // we must do a conditional statement early otherwise  we may zero length only to then immediately extend it

            GENERIC_PIECE_TABLE__PRINT_BOOL(target_start.second == target_end.second && target_start.third == clamped_position_start);
            if (target_start.second == target_end.second && target_start.third == clamped_position_start) {
                auto new_len = target_start.first->ptr->length - ((target_end.third + target_start.first->ptr->length) - clamped_position_end);
                target_start.first->ptr->start = target_start.first->ptr->start + new_len;
                target_start.first->ptr->length = target_start.first->ptr->length - new_len;
                if (debug) std::cout << "USER DATA ERASE" << std::endl;
                if (target_start.first->origin) {
                    origin_info.functions.act_on_user_data_erase(this, debug, target_start.first->ptr->user_data, target_start.first->ptr->start, target_start.first->ptr->length, target_start.first->ptr->length != 0);
                } else {
                    append_info.functions.act_on_user_data_erase(this, debug, target_start.first->ptr->user_data, target_start.first->ptr->start, target_start.first->ptr->length, target_start.first->ptr->length != 0);
                }
            } else {

                GENERIC_PIECE_TABLE__PRINT_STRING(target_start.third);
                auto diff = clamped_position_start - target_start.third;
                GENERIC_PIECE_TABLE__PRINT_STRING(target_start.first->ptr->start);
                GENERIC_PIECE_TABLE__PRINT_STRING(target_start.first->ptr->length);
                GENERIC_PIECE_TABLE__PRINT_STRING(diff);
                auto old_start = target_start.first->ptr->start;
                auto old_length = target_start.first->ptr->length;
                target_start.first->ptr->length = diff;

                if (debug) std::cout << "USER DATA SPLIT" << std::endl;
                GenericPieceTableUserData data;
                if (target_start.first->origin) {
                    origin_info.functions.act_on_user_data_split(this, debug, target_start.first->ptr->user_data, target_start.first->ptr->start, target_start.first->ptr->length, data);
                } else {
                    append_info.functions.act_on_user_data_split(this, debug, target_start.first->ptr->user_data, target_start.first->ptr->start, target_start.first->ptr->length, data);
                }

                GENERIC_PIECE_TABLE__PRINT_BOOL(target_start.second == target_end.second);
                if (target_start.second == target_end.second) {
                    auto & piece = target_start.first->origin ? origin_info.pieces : append_info.pieces;
                    auto len = descriptor_functions.length(piece);
                    GENERIC_PIECE_TABLE__PRINT_STRING(target_start.first->ptr->start);
                    GENERIC_PIECE_TABLE__PRINT_STRING(target_start.first->ptr->length);
                    auto new_len = old_length - ((target_end.third + old_length) - clamped_position_end);
                    GENERIC_PIECE_TABLE__PRINT_STRING(new_len);
                    GENERIC_PIECE_TABLE__PRINT_STRING(target_start.first->ptr->start + new_len);
                    GENERIC_PIECE_TABLE__PRINT_STRING(old_length - new_len);
                    descriptor_functions.append(piece, {target_start.first->ptr->start + new_len, old_length - new_len, nullptr});
                    auto & insert_piece = descriptor_functions.index(piece, len);
                    GENERIC_PIECE_TABLE__PRINT_STRING(insert_piece.start);
                    GENERIC_PIECE_TABLE__PRINT_STRING(insert_piece.length);
                    auto split_start_ = target_start.first->ptr->start + target_start.first->ptr->length;
                    auto split_length_ = insert_piece.start - split_start_;
                    GENERIC_PIECE_TABLE__PRINT_STRING(split_start_);
                    GENERIC_PIECE_TABLE__PRINT_STRING(split_length_);
                    if (debug) std::cout << "USER DATA SPLIT" << std::endl;
                    if (target_start.first->origin) {
                        origin_info.functions.act_on_user_data_split(this, debug, data, split_start_, split_length_, insert_piece.user_data);
                    } else {
                        append_info.functions.act_on_user_data_split(this, debug, data, split_start_, split_length_, insert_piece.user_data);
                    }
                    descriptor_order_functions.insert(piece_order, {target_start.first->origin, &insert_piece}, target_start.second+1);
                    piece_order_size++;
                } else {
                    GENERIC_PIECE_TABLE__PRINT_BOOL(!is_end);
                    if (!is_end) {
                        GENERIC_PIECE_TABLE__PRINT_STRING(target_end.third);
                        auto diff = clamped_position_end - target_end.third;
                        GENERIC_PIECE_TABLE__PRINT_BOOL(diff != 0);
                        if (diff != 0) {
                            target_end.first->ptr->start += diff;
                            target_end.first->ptr->length -= diff;
                            GENERIC_PIECE_TABLE__PRINT_STRING(diff);
                            if (debug) std::cout << "USER DATA ERASE" << std::endl;
                            if (target_start.first->origin) {
                                origin_info.functions.act_on_user_data_erase(this, debug, target_end.first->ptr->user_data, target_end.first->ptr->start, target_end.first->ptr->length, target_end.first->ptr->length != 0);
                            } else {
                                append_info.functions.act_on_user_data_erase(this, debug, target_end.first->ptr->user_data, target_end.first->ptr->start, target_end.first->ptr->length, target_end.first->ptr->length != 0);
                            }
                        }
                    }

                    GENERIC_PIECE_TABLE__PRINT_BOOL(target_start.second+1 != target_end.second);
                    if (target_start.second+1 != target_end.second) {
                        // clear all pieces between start and end
                        for (auto i = target_start.second+1; i < target_end.second; i++) {
                            auto & order = descriptor_order_functions.const_index(piece_order, i);
                            auto & descriptor = *order.ptr;
                            if (descriptor.length == 0) continue;
                            descriptor.length = 0;
                            if (debug) std::cout << "USER DATA ERASE" << std::endl;
                            GENERIC_PIECE_TABLE__PRINT_STRING(descriptor.length);
                            if (target_start.first->origin) {
                                origin_info.functions.act_on_user_data_erase(this, debug, descriptor.user_data, descriptor.start, descriptor.length, false);
                            } else {
                                append_info.functions.act_on_user_data_erase(this, debug, descriptor.user_data, descriptor.start, descriptor.length, false);
                            }
                        }
                    }
                }
            }
        }

        template <typename INFO, typename C>
        void insert_buffer(bool is_replace, bool is_origin, INFO & info, C & content, std::size_t position) {
            auto content_length = info.functions.content_length(content);
            if (content_length == 0) {
                return;
            }
            std::size_t position_;
            if (is_replace) {
                position_ = position;
            } else {
                auto end = length();
                position_ = position >= end ? end : position;
            }
            last_calculated_insert_position_start = position_;
            if (debug || debug_operations) {
                std::cout << "inserting " << content << " at position " << std::to_string(position_) << ": this:     " << string() << std::endl;
                std::cout << *this << std::endl << std::endl;
            }
            auto & descriptor = split_insert(is_origin, info, position_);
            descriptor.start = info.container_length();
            descriptor.length = content_length;
            if (debug) std::cout << "USER DATA INSERT" << std::endl;
            info.functions.act_on_user_data_insert(this, debug, descriptor.user_data, descriptor.start, content, descriptor.length);
            info.functions.append(info.buffer, content);
            if (debug || debug_operations) {
                std::cout << std::endl << "inserted " << content << " at position " << std::to_string(position_) << ": this:     " << string() << std::endl;
                std::cout << *this << std::endl << std::endl;
            }
        }

        template <typename INFO>
        void erase_buffer(bool is_replace, bool is_origin, INFO & info, std::size_t position, std::size_t length) {
            std::size_t position_;
            std::size_t position_end;
            if (is_replace) {
                position_ = position;
                position_end = position + length;
                last_calculated_erase_position_start = position;
                last_calculated_erase_length = length;
            } else {
                if (length == 0) {
                    return;
                }
                auto end = this->length();
                position_ = position >= end ? end : position;
                if (position_ == end) {
                    return;
                }
                if (length == -1) {
                    length = end;
                }
                position_end = position_ + length;
                if (position_end >= end) {
                    position_end = end;
                }
                auto length_ = position_end - position_;
                if (length_ == 0) {
                    return;
                }
                last_calculated_erase_position_start = position_;
                last_calculated_erase_length = length_;
            }
            if (debug || debug_operations) {
                std::cout << "erasing position " << std::to_string(position_) << " length " << std::to_string(last_calculated_erase_length) << ": this:     " << string() << std::endl;
                std::cout << *this << std::endl << std::endl;
            }
            split_erase(is_origin, info, position_, position_end);
            if (debug || debug_operations) {
                std::cout << std::endl << "erased position " << std::to_string(position_) << " length " << std::to_string(last_calculated_erase_length) << ": this:     " << string() << std::endl;
                std::cout << *this << std::endl << std::endl;
            }
        }

        public:

        THIS & append_origin(ORIGIN_CHAR_CONTAINER_T content) {
            insert_origin(content, -1);
            return *this;
        }

        THIS & append(APPEND_CHAR_CONTAINER_T content) {
            insert(content, -1);
            return *this;
        }

        THIS & insert_origin(ORIGIN_CHAR_CONTAINER_T content, std::size_t position) {
            last_op = LAST_OP_INSERT;
            last_buffer = LAST_BUFFER_ORIGIN;
            insert_buffer(false, true, origin_info, content, position);
            return *this;
        }

        THIS & insert(APPEND_CHAR_CONTAINER_T content, std::size_t position) {
            last_op = LAST_OP_INSERT;
            last_buffer = LAST_BUFFER_APPEND;
            insert_buffer(false, false, append_info, content, position);
            return *this;
        }

        THIS & replace_origin(ORIGIN_CHAR_CONTAINER_T content, std::size_t position, std::size_t length) {
            auto end = this->length();
            auto position_ = position >= end ? end : position;
            if (position_ == end) {
                last_op = LAST_OP_INSERT;
                last_buffer = LAST_BUFFER_ORIGIN;
                insert_buffer(true, true, origin_info, content, position_);
                return *this;
            }
            if (length == -1) {
                length = end;
            }
            auto position_end = position_ + length;
            if (position_end >= end) {
                position_end = end;
            }
            auto length_ = position_end - position_;

            if (length_ != 0) {
                last_op = LAST_OP_REPLACE;
                last_buffer = LAST_BUFFER_ORIGIN;
                last_calculated_replace_position_start = position_;
                last_calculated_replace_length = length_;
                erase_buffer(true, true, origin_info, position_, length_);
                insert_buffer(true, true, origin_info, content, position_);
                return *this;
            } else {
                last_op = LAST_OP_INSERT;
                last_buffer = LAST_BUFFER_ORIGIN;
                insert_buffer(true, true, origin_info, content, position_);
                return *this;
            }
        }

        THIS & replace(APPEND_CHAR_CONTAINER_T content, std::size_t position, std::size_t length) {
            auto end = this->length();
            auto position_ = position >= end ? end : position;
            if (position_ == end) {
                last_op = LAST_OP_INSERT;
                last_buffer = LAST_BUFFER_APPEND;
                insert_buffer(true, false, append_info, content, position_);
                return *this;
            }
            if (length == -1) {
                length = end;
            }
            auto position_end = position_ + length;
            if (position_end >= end) {
                position_end = end;
            }
            auto length_ = position_end - position_;

            if (length_ != 0) {
                last_op = LAST_OP_REPLACE;
                last_buffer = LAST_BUFFER_APPEND;
                last_calculated_replace_position_start = position_;
                last_calculated_replace_length = length_;
                erase_buffer(true, false, append_info, position_, length_);
                insert_buffer(true, false, append_info, content, position_);
                return *this;
            } else {
                last_op = LAST_OP_INSERT;
                last_buffer = LAST_BUFFER_APPEND;
                insert_buffer(true, false, append_info, content, position_);
                return *this;
            }
        }

        THIS & erase_origin(std::size_t position, std::size_t length) {
            last_op = LAST_OP_ERASE;
            last_buffer = LAST_BUFFER_ORIGIN;
            erase_buffer(false, true, origin_info, position, length);
            return *this;
        }

        THIS & erase(std::size_t position, std::size_t length) {
            last_op = LAST_OP_ERASE;
            last_buffer = LAST_BUFFER_APPEND;
            erase_buffer(false, false, append_info, position, length);
            return *this;
        }

        private:


        template <typename INFO>
        std::ostream & descriptor_to_stream(std::ostream & os, const char * tag, INFO & info, const GenericPieceTableDescriptor & descriptor) const {
            os << tag << ": ";
            os << "s:" << std::to_string(descriptor.start);
            os << ", len:" << std::to_string(descriptor.length);
            os << ", buffer content: ";
            for (std::size_t i = descriptor.start; i < (descriptor.start + descriptor.length); i++) {
                os << info.container_index_to_char(i);
            }
            return os;
        }

        public:

        virtual std::ostream & user_data_to_stream(std::ostream & os, const char * tag, void * user_data) const {
            return os;
        }

        template <typename INFO>
        std::ostream & buffer_to_stream(std::ostream & os, const char * tag, INFO & info) const {
            auto s = descriptor_functions.length(info.pieces);
            os << "  " << tag << " descriptor size: " << std::to_string(s) << std::endl;
            if (s != 0) {
                for (size_t i = 0, m = s-1; i <= m; i++) {
                    auto & descriptor = descriptor_functions.const_index(info.pieces, i);
                    std::string x = "      ";
                    x += tag;
                    x += " ";
                    x += std::to_string(i);
                    descriptor_to_stream(os, x.c_str(), info, descriptor);
                    if (descriptor.user_data.user_data.has_value()) {
                        std::string t = "    ";
                        for (std::size_t i = 0, m = x.length(); i < m; i++) {
                            t += " ";
                        }
                        os << std::endl;
                        user_data_to_stream(os, t.c_str(), descriptor.user_data.user_data.value());
                    }
                    if (i != m) os << std::endl;
                }
            }
            return os;
        }

        std::string string() const {
            std::string str;
            auto s = descriptor_order_functions.length(piece_order);
            for (size_t i = 0; i < s; i++) {
                auto & order = descriptor_order_functions.const_index(piece_order, i);
                auto & descriptor = *order.ptr;
                if (order.origin) {
                    for (std::size_t i_ = descriptor.start, m_ = (descriptor.start + descriptor.length); i_ < m_; i_++) {
                        str += origin_info.container_index_to_char(i_);
                    }
                } else {
                    for (std::size_t i_ = descriptor.start, m_ = (descriptor.start + descriptor.length); i_ < m_; i_++) {
                        str += append_info.container_index_to_char(i_);
                    }
                }
            }
            return str;
        }

        std::size_t length() const {
            std::size_t len = 0;
            auto s = descriptor_order_functions.length(piece_order);
            for (size_t i = 0; i < s; i++) {
                len += descriptor_order_functions.const_index(piece_order, i).ptr->length;
            }
            return len;
        }

        std::size_t size() const {
            return length();
        }

        std::ostream & order_to_stream(std::ostream & os) const {
            auto s = descriptor_order_functions.length(piece_order);
            os << "  " << " order size: " << std::to_string(s) << std::endl;
            if (s != 0) {
                for (size_t i = 0, m = s-1; i <= m; i++) {
                    auto & order = descriptor_order_functions.const_index(piece_order, i);
                    std::string x = "      ";
                    x += (order.origin ? "origin" : "append");
                    x += " ";
                    x += std::to_string(i);
                    if (order.origin) {
                        descriptor_to_stream(os, x.c_str(), origin_info, *order.ptr);
                    } else {
                        descriptor_to_stream(os, x.c_str(), append_info, *order.ptr);
                    }
                    if (i != m) os << std::endl;
                }
            }
            return os;
        }

        std::vector<std::string> split(const char & splitter) const {
            std::vector<std::string> vec;
            std::string c;
            auto s = descriptor_count();
            GENERIC_PIECE_TABLE__PRINT_STRING(s);
            for (size_t i = 0; i < s; i++) {
                GENERIC_PIECE_TABLE__PRINT_STRING(i);
                auto & order = descriptor_at(i);
                auto & descriptor = *order.ptr;
                if (order.origin) {
                    for (std::size_t i_ = descriptor.start, m_ = (descriptor.start + descriptor.length); i_ < m_; i_++) {
                        GENERIC_PIECE_TABLE__PRINT_STRING(i_);
                        GENERIC_PIECE_TABLE__PRINT_STRING(m_);
                        const char t = origin_info.container_index_to_char(i_);
                        GENERIC_PIECE_TABLE__PRINT(t);
                        GENERIC_PIECE_TABLE__PRINT(splitter);
                        GENERIC_PIECE_TABLE__PRINT_BOOL(t == splitter);
                        if (t == splitter) {
                            vec.push_back(c);
                            c = {};
                        } else {
                            c.push_back(t);
                        }
                    }
                } else {
                    for (std::size_t i_ = descriptor.start, m_ = (descriptor.start + descriptor.length); i_ < m_; i_++) {
                        GENERIC_PIECE_TABLE__PRINT_STRING(i_);
                        GENERIC_PIECE_TABLE__PRINT_STRING(m_);
                        const char t = append_info.container_index_to_char(i_);
                        GENERIC_PIECE_TABLE__PRINT(t);
                        GENERIC_PIECE_TABLE__PRINT(splitter);
                        GENERIC_PIECE_TABLE__PRINT_BOOL(t == splitter);
                        if (t == splitter) {
                            vec.push_back(c);
                            c = {};
                        } else {
                            c.push_back(t);
                        }
                    }
                }
            }
            vec.push_back(c);
            return vec;
        }
    };

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    std::ostream & operator<<(std::ostream & os, const GenericPieceTable<T1, T2, T3, T4, T5, T6> & obj) {
        auto & o = obj.get_origin_info();
        auto & a = obj.get_append_info();
        os << "GenericPieceTable start" << std::endl;
        os << "    origin: ";
        auto origin_len = o.container_length();
        for (std::size_t i = 0; i < origin_len; i++) {
            os << o.container_index_to_char(i);
        }
        os << std::endl;
        os << "    append: ";
        auto append_len = a.container_length();
        for (std::size_t i = 0; i < append_len; i++) {
            os << a.container_index_to_char(i);
        }
        os << std::endl;
        obj.buffer_to_stream(os, "origin", o);
        os << std::endl;
        obj.buffer_to_stream(os, "append", a);
        os << std::endl;
        obj.order_to_stream(os);
        os << std::endl;
        return os << "GenericPieceTable end";
    }

    struct StringMockPieceTable {
        
        using THIS = StringMockPieceTable;
        
        using ORIGIN_CHAR_CONTAINER_T = const char*;
        using APPEND_CHAR_CONTAINER_T = const char*;

        std::string str;

        const char operator[](std::size_t index) const {
            return str[index];
        }

        THIS & append_origin(ORIGIN_CHAR_CONTAINER_T content) {
            insert_origin(content, -1);
            return *this;
        }

        THIS & append(APPEND_CHAR_CONTAINER_T content) {
            insert(content, -1);
            return *this;
        }

        THIS & insert_origin(ORIGIN_CHAR_CONTAINER_T content, std::size_t position) {
            insert_buffer(content, position);
            return *this;
        }

        THIS & insert(APPEND_CHAR_CONTAINER_T content, std::size_t position) {
            insert_buffer(content, position);
            return *this;
        }

        THIS & replace_origin(ORIGIN_CHAR_CONTAINER_T content, std::size_t position, std::size_t length) {
            auto end = this->length();
            auto position_ = position >= end ? end : position;
            if (position_ == end) {
                insert_origin(content, position_);
                return *this;
            }
            if (length == -1) {
                length = end;
            }
            auto position_end = position_ + length;
            if (position_end >= end) {
                position_end = end;
            }
            auto length_ = position_end - position_;

            if (length_ != 0) {
                erase_origin(position_, length_);
            }

            insert_origin(content, position_);

            return *this;
        }

        THIS & replace(APPEND_CHAR_CONTAINER_T content, std::size_t position, std::size_t length) {
            auto end = this->length();
            auto position_ = position >= end ? end : position;
            if (position_ == end) {
                insert(content, position_);
                return *this;
            }
            if (length == -1) {
                length = end;
            }
            auto position_end = position_ + length;
            if (position_end >= end) {
                position_end = end;
            }
            auto length_ = position_end - position_;

            if (length_ != 0) {
                erase(position_, length_);
            }

            insert(content, position_);

            return *this;
        }

        THIS & erase_origin(std::size_t position, std::size_t length) {
            erase_buffer(position, length);
            return *this;
        }

        THIS & erase(std::size_t position, std::size_t length) {
            erase_buffer(position, length);
            return *this;
        }

        template <typename C>
        void insert_buffer(C & content, std::size_t position) {
            auto content_length = strlen(content);
            if (content_length == 0) {
                return;
            }
            auto end = length();
            auto position_ = position >= end ? end : position;
            str.insert(position_, content);
        }

        void erase_buffer(std::size_t position, std::size_t length) {
            if (length == 0) {
                return;
            }
            auto end = this->length();
            auto position_ = position >= end ? end : position;
            if (position_ == end) {
                return;
            }
            if (length == -1) {
                length = end;
            }
            auto position_end = position_ + length;
            if (position_end >= end) {
                position_end = end;
            }
            auto length_ = position_end - position_;
            if (length_ == 0) {
                return;
            }
            str.erase(position_, length_);
        }

        THIS & reset() {
            str = {};
            return *this;
        }
        THIS & clear_origin() {
            str = {};
            return *this;
        }
        THIS & clear() {
            str = {};
            return *this;
        }
        std::string string() const {
            return str;
        }
        std::size_t length() const {
            return str.length();
        }
        std::size_t size() const {
            return length();
        }
    };

    struct CharListPieceTable : public GenericPieceTable<
        std::list<GenericPieceTableDescriptor>,
        std::list<GenericPieceTableDescriptorOrder>,
        const char*, const char*, std::string, std::list<char>
    > {
        using FINSERT_T = MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(void* instance, const bool & debug, USER_DATA_USER_DATA_T user_data, USER_DATA_START_T start, USER_DATA_ORIGIN_CONTENT_T content, USER_DATA_LENGTH_T length)>;
        using FSPLIT_T = MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(void* instance, const bool & debug, USER_DATA_USER_DATA_T user_data, USER_DATA_START_T start, USER_DATA_LENGTH_T length, USER_DATA_USER_DATA_T user_data2)>;
        using FERASE_T = MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(void* instance, const bool & debug, USER_DATA_USER_DATA_T user_data, USER_DATA_START_T start, USER_DATA_LENGTH_T length, USER_DATA_FLAGS_T is_start)>;
        
        FINSERT_T finsert = [](auto * this_, auto & debug, auto & user_data, auto & start, auto & content, auto & content_length) {};
        FSPLIT_T fsplit = [](auto * this_, auto & debug, auto & user_data, auto & start, auto & length, auto & user_data2) {};
        FERASE_T ferase = [](auto * this_, auto & debug, auto & user_data, auto & start, auto & length, auto & is_start) {};

        CharListPieceTable(const CharListPieceTable & other) : GenericPieceTable(other) {
            finsert = other.finsert;
            fsplit = other.fsplit;
            ferase = other.ferase;
        }

        CharListPieceTable & operator=(const CharListPieceTable & other) {
            GenericPieceTable::operator=(other);
            finsert = other.finsert;
            fsplit = other.fsplit;
            ferase = other.ferase;
            return *this;
        }

        CharListPieceTable() : GenericPieceTable(
            { // descriptor
                // reset
                [](auto & c) { c = {}; },
                // append
                [](auto & c, auto & d) { c.emplace_back(d); },
                // length
                [](auto & c) { return std::distance(c.begin(), c.end()); },
                // const index
                [](auto & c, auto index) -> const GenericPieceTableDescriptor & { return *std::next(c.begin(), index); },
                // index
                [](auto & c, auto index) -> GenericPieceTableDescriptor & { return *std::next(c.begin(), index); }
            },
            { // descriptor order
                // reset
                [](auto & c) { c = {}; },
                // insert
                [](auto & c, auto & d, auto index) { c.insert(std::next(c.begin(), index), d); },
                // length
                [](auto & c) { return std::distance(c.begin(), c.end()); },
                // const index
                [](auto & c, auto index) -> const GenericPieceTableDescriptorOrder & { return *std::next(c.begin(), index); },
                // index
                [](auto & c, auto index) -> GenericPieceTableDescriptorOrder & { return *std::next(c.begin(), index); }
            },
            { // origin
                // reset
                [](auto & c) { c = {}; },
                // append
                [](auto & c, auto & content) { c = c + content; },
                // container length
                [](auto & c) { return c.size(); },
                // content length
                [](auto & content) { return content == nullptr ? 0 : strlen(content); },
                // container index to char
                [](auto & c, auto index) -> const char { return c[index]; },
                // user data insert
                [](auto * this_, auto & debug, auto & user_data, auto & start, auto & content, auto & content_length) { static_cast<CharListPieceTable*>(this_)->finsert(this_, debug, user_data, start, content, content_length); },
                // user data split
                [](auto * this_, auto & debug, auto & user_data, auto & start, auto & length, auto & user_data_2) { static_cast<CharListPieceTable*>(this_)->fsplit(this_, debug, user_data, start, length, user_data_2); },
                // user data erase
                [](auto * this_, auto & debug, auto & user_data, auto & start, auto & length, auto & is_start) { static_cast<CharListPieceTable*>(this_)->ferase(this_, debug, user_data, start, length, is_start); }
            },
            { // append
                // reset
                [](auto & c) { c = {}; },
                // append
                [](auto & c, auto & content) { for (std::size_t i = 0, max = strlen(content); i < max; i++) c.emplace_back(content[i]); },
                // container length
                [](auto & c) { return std::distance(c.begin(), c.end()); },
                // content length
                [](auto & content) { return content == nullptr ? 0 : strlen(content); },
                // container index to char
                [](auto & c, auto index) { return *std::next(c.begin(), index); },
                // user data insert
                [](auto * this_, auto & debug, auto & user_data, auto & start, auto & content, auto & content_length) { static_cast<CharListPieceTable*>(this_)->finsert(this_, debug, user_data, start, content, content_length); },
                // user data split
                [](auto * this_, auto & debug, auto & user_data, auto & start, auto & length, auto & user_data_2) { static_cast<CharListPieceTable*>(this_)->fsplit(this_, debug, user_data, start, length, user_data_2); },
                // user data erase
                [](auto * this_, auto & debug, auto & user_data, auto & start, auto & length, auto & is_start) { static_cast<CharListPieceTable*>(this_)->ferase(this_, debug, user_data, start, length, is_start); }
            }
        ) {}
    };

    struct StringPieceTable : public GenericPieceTable<
        std::list<GenericPieceTableDescriptor>,
        std::list<GenericPieceTableDescriptorOrder>,
        const char*, const char*, std::string, std::string
    > {
        using FINSERT_T = MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(void* instance, const bool & debug, USER_DATA_USER_DATA_T user_data, USER_DATA_START_T start, USER_DATA_ORIGIN_CONTENT_T content, USER_DATA_LENGTH_T length)>;
        using FSPLIT_T = MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(void* instance, const bool & debug, USER_DATA_USER_DATA_T user_data, USER_DATA_START_T start, USER_DATA_LENGTH_T length, USER_DATA_USER_DATA_T user_data2)>;
        using FERASE_T = MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(void* instance, const bool & debug, USER_DATA_USER_DATA_T user_data, USER_DATA_START_T start, USER_DATA_LENGTH_T length, USER_DATA_FLAGS_T is_start)>;

        FINSERT_T finsert = [](auto * this_, auto & debug, auto & user_data, auto & start, auto & content, auto & content_length) {};
        FSPLIT_T fsplit = [](auto * this_, auto & debug, auto & user_data, auto & start, auto & length, auto & user_data2) {};
        FERASE_T ferase = [](auto * this_, auto & debug, auto & user_data, auto & start, auto & length, auto & is_start) {};

        StringPieceTable(const StringPieceTable & other) : GenericPieceTable(other) {
            finsert = other.finsert;
            fsplit = other.fsplit;
            ferase = other.ferase;
        }

        StringPieceTable & operator=(const StringPieceTable & other) {
            GenericPieceTable::operator=(other);
            finsert = other.finsert;
            fsplit = other.fsplit;
            ferase = other.ferase;
            return *this;
        }

        StringPieceTable() : GenericPieceTable(
            { // descriptor
                // reset
                [](auto & c) { c = {}; },
                // append
                [](auto & c, auto & d) { c.emplace_back(d); },
                // length
                [](auto & c) { return std::distance(c.begin(), c.end()); },
                // const index
                [](auto & c, auto index) -> const GenericPieceTableDescriptor & { return *std::next(c.begin(), index); },
                // index
                [](auto & c, auto index) -> GenericPieceTableDescriptor & { return *std::next(c.begin(), index); }
            },
            { // descriptor order
                // reset
                [](auto & c) { c = {}; },
                // insert
                [](auto & c, auto & d, auto index) { c.insert(std::next(c.begin(), index), d); },
                // length
                [](auto & c) { return std::distance(c.begin(), c.end()); },
                // const index
                [](auto & c, auto index) -> const GenericPieceTableDescriptorOrder & { return *std::next(c.begin(), index); },
                // index
                [](auto & c, auto index) -> GenericPieceTableDescriptorOrder & { return *std::next(c.begin(), index); }
            },
            { // origin
                // reset
                [](auto & c) { c = {}; },
                // append
                [](auto & c, auto & content) { c = c + content; },
                // container length
                [](auto & c) { return c.size(); },
                // content length
                [](auto & content) { return content == nullptr ? 0 : strlen(content); },
                // container index to char
                [](auto & c, auto index) -> const char { return c[index]; },
                // user data insert
                [](auto * this_, auto & debug, auto & user_data, auto & start, auto & content, auto & content_length) { static_cast<StringPieceTable*>(this_)->finsert(this_, debug, user_data, start, content, content_length); },
                // user data split
                [](auto * this_, auto & debug, auto & user_data, auto & start, auto & length, auto & user_data_2) { static_cast<StringPieceTable*>(this_)->fsplit(this_, debug, user_data, start, length, user_data_2); },
                // user data erase
                [](auto * this_, auto & debug, auto & user_data, auto & start, auto & length, auto & is_start) { static_cast<StringPieceTable*>(this_)->ferase(this_, debug, user_data, start, length, is_start); }
            },
            { // append
                // reset
                [](auto & c) { c = {}; },
                // append
                [](auto & c, auto & content) { c = c + content; },
                // container length
                [](auto & c) { return c.size(); },
                // content length
                [](auto & content) { return content == nullptr ? 0 : strlen(content); },
                // content index to char
                [](auto & c, auto index) -> const char { return c[index]; },
                // user data insert
                [](auto * this_, auto & debug, auto & user_data, auto & start, auto & content, auto & content_length) { static_cast<StringPieceTable*>(this_)->finsert(this_, debug, user_data, start, content, content_length); },
                // user data split
                [](auto * this_, auto & debug, auto & user_data, auto & start, auto & length, auto & user_data_2) { static_cast<StringPieceTable*>(this_)->fsplit(this_, debug, user_data, start, length, user_data_2); },
                // user data erase
                [](auto * this_, auto & debug, auto & user_data, auto & start, auto & length, auto & is_start) { static_cast<StringPieceTable*>(this_)->ferase(this_, debug, user_data, start, length, is_start); }
            }
        ) {}
    };

    class CharListPieceTableWithCharacterInformation : public CharListPieceTable {
        std::string characters;

        protected:

        struct CharInfo {
            char character;
            std::size_t index;
        };

        public:

        CharListPieceTableWithCharacterInformation(const CharListPieceTableWithCharacterInformation & other)
            : CharListPieceTable(other) {
                characters = other.characters;
        }

        CharListPieceTableWithCharacterInformation & operator=(const CharListPieceTableWithCharacterInformation & other) {
            CharListPieceTable::operator=(other);
            characters = other.characters;
            return *this;
        }

        CharListPieceTableWithCharacterInformation() : CharListPieceTableWithCharacterInformation("") {}

        CharListPieceTableWithCharacterInformation(const std::string & characters) : characters(characters) {
            finsert = [](auto * this_, auto & debug, auto & user_data, auto & start, auto & content, auto & content_length) {
                if (debug) std::cout << "finsert" << std::endl;
                std::vector<CharInfo> * info = nullptr;
                for(std::size_t i = 0; i < content_length; i++) {
                    for (auto & character : static_cast<CharListPieceTableWithCharacterInformation*>(this_)->characters) {
                        if (character == content[i]) {
                            if (info == nullptr) {
                                info = new std::vector<CharInfo>();
                            }
                            CharInfo char_info = {character, start + i};
                            info->push_back(char_info);
                        }
                    }
                }
                if (info != nullptr) {
                    user_data = GenericPieceTableUserData(
                        info,
                        [](auto src) {
                            std::vector<CharInfo>* vec = new std::vector<CharInfo>();
                            *vec = *static_cast<const std::vector<CharInfo>*>(src);
                            return vec;
                        },
                        [](auto p) {
                            delete static_cast<std::vector<CharInfo>*>(p);
                        }
                    );
                } else {
                    user_data = {};
                }
            };

            // https://godbolt.org/z/7sffYEnse

            // fsplit(v_lhs, 5, 1, v_rhs); // split vector v_lhs into half, v_lhs{5, 2, 4, 7, 2} > v_lhs{5, 2, 4} v_rhs{7, 2}
            // v_rhs is always empty
            fsplit = [](auto * this_, auto & debug, auto & user_data, auto & start, auto & length, auto & user_data_2) {
                if (debug) std::cout << "fsplit" << std::endl;
                GENERIC_PIECE_TABLE__PRINT_STRING(start);
                GENERIC_PIECE_TABLE__PRINT_STRING(length);
                auto & vec = *static_cast<std::vector<CharInfo>*>(user_data.user_data.value());
                auto vec2 = new std::vector<CharInfo>();
                user_data_2 = GenericPieceTableUserData(
                    vec2,
                    [](auto src) {
                        std::vector<CharInfo>* vec = new std::vector<CharInfo>();
                        *vec = *static_cast<const std::vector<CharInfo>*>(src);
                        return vec;
                    },
                    [](auto p) {
                        delete static_cast<std::vector<CharInfo>*>(p);
                    }
                );

                assert(vec.size() != 0);
                assert(vec[0].index >= start);
                auto begin = vec.begin();
                auto end = vec.end();
                auto max = start + length;
                auto index_min = std::find_if(begin, end, [&start, &max](auto p){return p.index >= start && p.index <= max;});
                auto index_max = std::find_if(begin, end, [&max](auto p){return p.index >= max;});
                vec2->insert(vec2->end(), index_max, end);
                vec.erase(begin, index_min);
                vec.erase(index_max, vec.end());
            };
            // ferase(v, 5, 3, {true}); // truncate to a min index of 5 and a max index of 5+3 (7), v{5, 6, 7, 8, 9} > v{5, 6, 7}
            // ferase(v, 10, 0, {false}); // truncate to max index of 10, including itself, v{4, 6, 10, 2} > v{4, 6}
            ferase = [](auto * this_, auto & debug, auto & user_data, auto & start, auto & length, auto & is_start) {
                if (debug) std::cout << "ferase" << std::endl;
                GENERIC_PIECE_TABLE__PRINT_STRING(start);
                GENERIC_PIECE_TABLE__PRINT_STRING(length);
                GENERIC_PIECE_TABLE__PRINT_BOOL(is_start);
                assert(user_data.user_data.has_value());
                auto & vec = *static_cast<std::vector<CharInfo>*>(user_data.user_data.value());
                assert(vec.size() != 0);
                auto end = vec.end();
                if (!is_start) {
                    if (debug) std::cout << "ferase END" << std::endl;
                    assert(length == 0); // do we always have zero length?
                    // benchmarks show find_if is almost 400 times faster than remove_if for chopping off the tail of a vector
                    // https://quick-bench.com/q/fpStaAWC9Pv3kJRLwwB7DGNDIXw
                    //
                    auto it = std::find_if(
                        vec.begin(),
                        end,
                        [&start](auto p){return p.index >= start;}
                    );
                    vec.erase(it, end);
                } else {
                    if (debug) std::cout << "ferase MIDDLE" << std::endl;
                    auto max = start + length;
                    // benchmarks show remove_if and find_if make little difference when slicing the vector, tho find_if is faster the difference is negleble
                    vec.erase(std::remove_if(
                        vec.begin(),
                        end,
                        [&start, &max](auto p){return p.index < start || p.index > max;}
                    ), end);
                }
            };
        }

        std::ostream & user_data_to_stream(std::ostream & os, const char * tag, void * user_data) const override {
            std::vector<CharInfo> & vec = *static_cast<std::vector<CharInfo>*>(user_data);
            auto m = vec.size();
            if (m == 0) return os;
            m--;
            for (std::size_t i = 0; i <= m; i++) {
                auto & v = vec[i];
                os << tag << "char: '" << v.character << "', index: " << std::to_string(v.index);
                if (i != m) os << std::endl;
            }
            return os;
        }
    };

    class StringPieceTableWithCharacterInformation : public StringPieceTable {
        std::string characters;

        protected:

        struct CharInfo {
            char character;
            std::size_t index;
        };

        public:

        StringPieceTableWithCharacterInformation(const StringPieceTableWithCharacterInformation & other)
            : StringPieceTable(other) {
                characters = other.characters;
        }

        StringPieceTableWithCharacterInformation & operator=(const StringPieceTableWithCharacterInformation & other) {
            StringPieceTable::operator=(other);
            characters = other.characters;
            return *this;
        }

        StringPieceTableWithCharacterInformation() : StringPieceTableWithCharacterInformation("") {}

        StringPieceTableWithCharacterInformation(const std::string & characters) : characters(characters) {
            finsert = [](auto * this_, auto & debug, auto & user_data, auto & start, auto & content, auto & content_length) {
                if (debug) std::cout << "finsert" << std::endl;
                std::vector<CharInfo> * info = nullptr;
                for(std::size_t i = 0; i < content_length; i++) {
                    for (auto & character : static_cast<StringPieceTableWithCharacterInformation*>(this_)->characters) {
                        if (character == content[i]) {
                            if (info == nullptr) {
                                info = new std::vector<CharInfo>();
                            }
                            CharInfo char_info = {character, start + i};
                            info->push_back(char_info);
                        }
                    }
                }
                if (info != nullptr) {
                    user_data = GenericPieceTableUserData(
                        info,
                        [](auto src) {
                            std::vector<CharInfo>* vec = new std::vector<CharInfo>();
                            *vec = *static_cast<const std::vector<CharInfo>*>(src);
                            return vec;
                        },
                        [](auto p) {
                            delete static_cast<std::vector<CharInfo>*>(p);
                        }
                    );
                } else {
                    user_data = {};
                }
            };

            // https://godbolt.org/z/7sffYEnse

            // fsplit(v_lhs, 5, 1, v_rhs); // split vector v_lhs into half, v_lhs{5, 2, 4, 7, 2} > v_lhs{5, 2, 4} v_rhs{7, 2}
            // v_rhs is always empty
            fsplit = [](auto * this_, auto & debug, auto & user_data, auto & start, auto & length, auto & user_data_2) {
                if (debug) std::cout << "fsplit" << std::endl;
                GENERIC_PIECE_TABLE__PRINT_STRING(start);
                GENERIC_PIECE_TABLE__PRINT_STRING(length);
                auto & vec = *static_cast<std::vector<CharInfo>*>(user_data.user_data.value());
                auto vec2 = new std::vector<CharInfo>();
                user_data_2 = GenericPieceTableUserData(
                    vec2,
                    [](auto src) {
                        std::vector<CharInfo>* vec = new std::vector<CharInfo>();
                        *vec = *static_cast<const std::vector<CharInfo>*>(src);
                        return vec;
                    },
                    [](auto p) {
                        delete static_cast<std::vector<CharInfo>*>(p);
                    }
                );

                assert(vec.size() != 0);
                assert(vec[0].index >= start);
                auto begin = vec.begin();
                auto end = vec.end();
                auto max = start + length;
                auto index_min = std::find_if(begin, end, [&start, &max](auto p){return p.index >= start && p.index <= max;});
                auto index_max = std::find_if(begin, end, [&max](auto p){return p.index >= max;});
                vec2->insert(vec2->end(), index_max, end);
                vec.erase(begin, index_min);
                vec.erase(index_max, vec.end());
            };
            // ferase(v, 5, 3, {true}); // truncate to a min index of 5 and a max index of 5+3 (7), v{5, 6, 7, 8, 9} > v{5, 6, 7}
            // ferase(v, 10, 0, {false}); // truncate to max index of 10, including itself, v{4, 6, 10, 2} > v{4, 6}
            ferase = [](auto * this_, auto & debug, auto & user_data, auto & start, auto & length, auto & is_start) {
                if (debug) std::cout << "ferase" << std::endl;
                GENERIC_PIECE_TABLE__PRINT_STRING(start);
                GENERIC_PIECE_TABLE__PRINT_STRING(length);
                GENERIC_PIECE_TABLE__PRINT_BOOL(is_start);
                assert(user_data.user_data.has_value());
                auto & vec = *static_cast<std::vector<CharInfo>*>(user_data.user_data.value());
                assert(vec.size() != 0);
                auto end = vec.end();
                if (!is_start) {
                    if (debug) std::cout << "ferase END" << std::endl;
                    assert(length == 0); // do we always have zero length?
                    // benchmarks show find_if is almost 400 times faster than remove_if for chopping off the tail of a vector
                    // https://quick-bench.com/q/fpStaAWC9Pv3kJRLwwB7DGNDIXw
                    //
                    auto it = std::find_if(
                        vec.begin(),
                        end,
                        [&start](auto p){return p.index >= start;}
                    );
                    vec.erase(it, end);
                } else {
                    if (debug) std::cout << "ferase MIDDLE" << std::endl;
                    auto max = start + length;
                    // benchmarks show remove_if and find_if make little difference when slicing the vector, tho find_if is faster the difference is negleble
                    vec.erase(std::remove_if(
                        vec.begin(),
                        end,
                        [&start, &max](auto p){return p.index < start || p.index > max;}
                    ), end);
                }
            };
        }

        std::ostream & user_data_to_stream(std::ostream & os, const char * tag, void * user_data) const override {
            std::vector<CharInfo> & vec = *static_cast<std::vector<CharInfo>*>(user_data);
            auto m = vec.size();
            if (m == 0) return os;
            m--;
            for (std::size_t i = 0; i <= m; i++) {
                auto & v = vec[i];
                os << tag << "char: '" << v.character << "', index: " << std::to_string(v.index);
                if (i != m) os << std::endl;
            }
            return os;
        }
    };
}

#undef GENERIC_PIECE_TABLE__PRINT_BOOL
#undef GENERIC_PIECE_TABLE__PRINT_STRING
#undef GENERIC_PIECE_TABLE__PRINT_POINTER
#undef GENERIC_PIECE_TABLE__PRINT
#undef GENERIC_PIECE_TABLE__PRINT_BOOLF
#undef GENERIC_PIECE_TABLE__PRINT_STRINGF
#undef GENERIC_PIECE_TABLE__PRINT_POINTERF
#undef GENERIC_PIECE_TABLE__PRINTF

#endif