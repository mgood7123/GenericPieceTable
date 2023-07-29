#ifndef MINIDOC_GENERIC_PIECE_TABLE_H
#define MINIDOC_GENERIC_PIECE_TABLE_H

#include <string>
#include <functional>
#include <iostream>
#include <cstring>
#include <list>
#include <forward_list>

#ifndef MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE
#define MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE std::function
#endif

#define GENERIC_PIECE_TABLE__PRINT_BOOL(x) { if (debug) std::cout << #x << ": " << ((x) ? "true" : "false") << std::endl; }
#define GENERIC_PIECE_TABLE__PRINT_STRING(x) { if (debug) std::cout << #x << ": " << std::to_string((x)) << std::endl; }

namespace MiniDoc {

    struct GenericPieceTableDescriptor {
        std::size_t start;
        std::size_t length;
    };

    struct GenericPieceTableDescriptorOrder {
        bool origin;
        GenericPieceTableDescriptor * ptr;
    };

    template <typename CHAR_CONTAINER_T, typename BUFFER_CONTAINER_T>
    struct GenericPieceTableFunctions {
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(BUFFER_CONTAINER_T & container)> reset;
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<void(BUFFER_CONTAINER_T & container, CHAR_CONTAINER_T & content)> append;
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<std::size_t(const BUFFER_CONTAINER_T & container)> container_length;
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<std::size_t(CHAR_CONTAINER_T & content)> content_length;
        MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE<const char(const BUFFER_CONTAINER_T & container, std::size_t index)> container_index_to_char;
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

        const char operator[](std::size_t index) const {
            auto l = length();
            if (l == 0) {
                throw std::runtime_error("index out of range");
            }

            if (index == -1 || index > (l-1)) index = l-1;

            std::size_t LEN = 0;
            if (debug) {
                std::cout << "piece_order_size: " << std::to_string(piece_order_size) << std::endl;
            }
            for (size_t i = 0; i < piece_order_size; i++) {
                auto & order = descriptor_order_functions.const_index(piece_order, i);
                auto & descriptor = *order.ptr;
                auto next_LEN = LEN + descriptor.length;
                if (debug) {
                    std::cout << "index: " << std::to_string(index) << ", next_LEN: " << std::to_string(next_LEN) << std::endl;
                }
                if (index < next_LEN) {
                    // avoid looping through the entire descriptor to find our index
                    auto buffer_index = descriptor.start + (index - LEN);
                    auto char_ = (order.origin ? origin_info : append_info).container_index_to_char(buffer_index);
                    if (debug) {
                        std::cout << "lookup return character " << char_ << std::endl;
                    }
                    return char_;
                }
                LEN = next_LEN;
            }
            throw std::runtime_error("lookup reached end");
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
                auto next_LEN = LEN + descriptor.length;
                if (debug) {
                    std::cout << "position: " << std::to_string(position) << ", next_LEN: " << std::to_string(next_LEN) << std::endl;
                }
                if (position < next_LEN) {
                    if (debug) {
                        // avoid looping through the entire descriptor to find our index
                        auto buffer_index = descriptor.start + (position - LEN);
                        auto char_ = (order.origin ? origin_info : append_info).container_index_to_char(buffer_index);
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
                descriptor_functions.append(piece, {0, 0});
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
                descriptor_functions.append(piece, {0, 0});
                auto & insert_piece = descriptor_functions.index(piece, len);
                descriptor_order_functions.insert(piece_order, {is_origin, &insert_piece}, piece_order_size);
                piece_order_size++;
                return insert_piece;
            }

            GENERIC_PIECE_TABLE__PRINT_BOOL(target_start.third == clamped_position_start);

            if (target_start.third == clamped_position_start) {
                auto & piece = info.pieces;
                auto len = descriptor_functions.length(piece);
                descriptor_functions.append(piece, {0, 0});
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
                descriptor_functions.append(piece, {0, 0});
                auto & insert_piece = descriptor_functions.index(piece, len);
                descriptor_order_functions.insert(piece_order, {is_origin, &insert_piece}, target_start.second+1);
                piece_order_size++;

                auto & piece2 = target_start.first->origin ? origin_info.pieces : append_info.pieces;
                auto len2 = descriptor_functions.length(piece2);
                GENERIC_PIECE_TABLE__PRINT_STRING(target_start.first->ptr->start + diff);
                GENERIC_PIECE_TABLE__PRINT_STRING(old_length);
                GENERIC_PIECE_TABLE__PRINT_STRING(target_start.first->ptr->start - ((target_start.first->ptr->start + diff) - old_length));
                GENERIC_PIECE_TABLE__PRINT_STRING(old_length - diff);
                descriptor_functions.append(piece2, {target_start.first->ptr->start + diff, old_length - diff});
                auto & insert_piece2 = descriptor_functions.index(piece2, len2);
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

        public:

        THIS & reset() {
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
            
            GENERIC_PIECE_TABLE__PRINT_STRING(target_start.third);
            auto diff = clamped_position_start - target_start.third;
            GENERIC_PIECE_TABLE__PRINT_STRING(target_start.first->ptr->start);
            GENERIC_PIECE_TABLE__PRINT_STRING(target_start.first->ptr->length);
            GENERIC_PIECE_TABLE__PRINT_STRING(diff);
            auto old_length = target_start.first->ptr->length;
            target_start.first->ptr->length = diff;

            GENERIC_PIECE_TABLE__PRINT_BOOL(target_start.second == target_end.second);

            if (target_start.second == target_end.second) {
                GENERIC_PIECE_TABLE__PRINT_BOOL(target_start.third == clamped_position_start);
                if (target_start.third == clamped_position_start) {
                    auto new_len = old_length - ((target_end.third + old_length) - clamped_position_end);
                    *target_start.first->ptr = {target_start.first->ptr->start + new_len, old_length - new_len};
                } else {
                    auto & piece = target_start.first->origin ? origin_info.pieces : append_info.pieces;
                    auto len1 = descriptor_functions.length(piece);
                    GENERIC_PIECE_TABLE__PRINT_STRING(target_start.first->ptr->start);
                    GENERIC_PIECE_TABLE__PRINT_STRING(target_start.first->ptr->length);
                    auto new_len = old_length - ((target_end.third + old_length) - clamped_position_end);
                    descriptor_functions.append(piece, {target_start.first->ptr->start + new_len, old_length - new_len});
                    auto & insert_piece1 = descriptor_functions.index(piece, len1);
                    GENERIC_PIECE_TABLE__PRINT_STRING(insert_piece1.start);
                    GENERIC_PIECE_TABLE__PRINT_STRING(insert_piece1.length);
                    descriptor_order_functions.insert(piece_order, {target_start.first->origin, &insert_piece1}, target_start.second+1);
                    piece_order_size++;
                }
            } else {
                GENERIC_PIECE_TABLE__PRINT_BOOL(!is_end);
                if (!is_end) {
                    GENERIC_PIECE_TABLE__PRINT_STRING(target_end.third);
                    auto diff = clamped_position_end - target_end.third;
                    GENERIC_PIECE_TABLE__PRINT_STRING(diff);
                    target_end.first->ptr->start += diff;
                    target_end.first->ptr->length -= diff;
                }

                GENERIC_PIECE_TABLE__PRINT_BOOL(target_start.second+1 != target_end.second);
                if (target_start.second+1 != target_end.second) {
                    // clear all pieces between start and end
                    for (auto i = target_start.second+1; i < target_end.second; i++) {
                        auto & order = descriptor_order_functions.const_index(piece_order, i);
                        auto & descriptor = *order.ptr;
                        descriptor.length = 0;
                    }
                }
            }
            #undef GENERIC_PIECE_TABLE__PRINT_BOOL
            #undef GENERIC_PIECE_TABLE__PRINT_STRING
        }

        template <typename INFO, typename C>
        void insert_buffer(bool is_origin, INFO & info, C & content, std::size_t position) {
            auto content_length = info.functions.content_length(content);
            if (content_length == 0) {
                return;
            }
            auto end = length();
            auto position_ = position >= end ? end : position;
            if (debug || debug_operations) {
                std::cout << "inserting " << content << " at position " << std::to_string(position_) << ": this:     " << string() << std::endl;
                std::cout << *this << std::endl << std::endl;
            }
            auto & descriptor = split_insert(is_origin, info, position_);
            descriptor.start = info.container_length();
            descriptor.length = content_length;
            info.functions.append(info.buffer, content);
            if (debug || debug_operations) {
                std::cout << std::endl << "inserted " << content << " at position " << std::to_string(position_) << ": this:     " << string() << std::endl;
                std::cout << *this << std::endl << std::endl;
            }
        }

        template <typename INFO>
        void erase_buffer(bool is_origin, INFO & info, std::size_t position, std::size_t length) {
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
            if (debug || debug_operations) {
                std::cout << "erasing position " << std::to_string(position_) << " length " << std::to_string(length_) << ": this:     " << string() << std::endl;
                std::cout << *this << std::endl << std::endl;
            }
            split_erase(is_origin, info, position_, position_end);
            if (debug || debug_operations) {
                std::cout << std::endl << "erased position " << std::to_string(position_) << " length " << std::to_string(length_) << ": this:     " << string() << std::endl;
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
            insert_buffer(true, origin_info, content, position);
            return *this;
        }

        THIS & insert(APPEND_CHAR_CONTAINER_T content, std::size_t position) {
            insert_buffer(false, append_info, content, position);
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
            erase_buffer(true, origin_info, position, length);
            return *this;
        }

        THIS & erase(std::size_t position, std::size_t length) {
            erase_buffer(false, append_info, position, length);
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

        template <typename INFO>
        std::ostream & buffer_to_stream(std::ostream & os, const char * tag, INFO & info) const {
            auto s = descriptor_functions.length(info.pieces);
            os << "  " << tag << " descriptor size: " << std::to_string(s) << std::endl;
            for (size_t i = 0; i < s; i++) {
                auto & descriptor = descriptor_functions.const_index(info.pieces, i);
                std::string x = "      ";
                x += tag;
                x += " ";
                x += std::to_string(i);
                descriptor_to_stream(os, x.c_str(), info, descriptor);
                os << std::endl;
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
                    for (std::size_t i_ = descriptor.start; i_ < (descriptor.start + descriptor.length); i_++) {
                        str += origin_info.container_index_to_char(i_);
                    }
                } else {
                    for (std::size_t i_ = descriptor.start; i_ < (descriptor.start + descriptor.length); i_++) {
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
            for (size_t i = 0; i < s; i++) {
                auto & order = descriptor_order_functions.const_index(piece_order, i);
                std::string x = "      ";
                x += (order.origin ? "origin" : "append");
                x += " ";
                x += std::to_string(i);
                descriptor_to_stream(os, x.c_str(), order.origin ? origin_info : append_info, *order.ptr);
                os << std::endl;
            }
            return os;
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
        auto append_len = o.container_length();
        for (std::size_t i = 0; i < append_len; i++) {
            os << o.container_index_to_char(i);
        }
        os << std::endl;
        obj.buffer_to_stream(os, "origin", o);
        obj.buffer_to_stream(os, "append", a);
        obj.order_to_stream(os);
        return os << "GenericPieceTable end";
    }

    struct StringMockPieceTable {
        
        using THIS = String;
        
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
        const char*, const char*, std::list<char>, std::list<char>
    > {
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
                [](auto & c, auto & content) { for (std::size_t i = 0, max = strlen(content); i < max; i++) c.emplace_back(content[i]); },
                // container length
                [](auto & c) { return std::distance(c.begin(), c.end()); },
                // content length
                [](auto & content) { return content == nullptr ? 0 : strlen(content); },
                // container index to char
                [](auto & c, auto index) { return *std::next(c.begin(), index); }
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
                [](auto & c, auto index) { return *std::next(c.begin(), index); }
            }
        ) {}
    };

    struct StringPieceTable : public GenericPieceTable<
        std::list<GenericPieceTableDescriptor>,
        std::list<GenericPieceTableDescriptorOrder>,
        const char*, const char*, std::string, std::string
    > {
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
                [](auto & c, auto index) -> const char { return c[index]; }
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
                [](auto & c, auto index) -> const char { return c[index]; }
            }
        ) {}
    };
}

#undef GENERIC_PIECE_TABLE__PRINT_BOOL
#undef GENERIC_PIECE_TABLE__PRINT_STRING

#endif