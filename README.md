# GenericPieceTable
a C++ templated Piece Table implementation

```s
$ make CC=clang CXX=clang++ test_debug_ninja_valgrind && memusage --total --png=png -t -m ./debug_EXECUTABLE/GenericPieceTable_Tests
```

we provide 3 basic implementations, `String` is a `mock` version that forwards everything to `std::string`


`std::string` uses half the memory we do!
 - however this may be due to `small-string-optimization` since we only tested with small buffers

tho technically piece tables trade memory usage for speed
 - with `std::string` the performance of `non-end` modifications degrades as the buffer gets larger and larger

```
    CharListPieceTable   :

        ==641371==   total heap usage: 7,827 allocs, 7,827 frees, 443,848 bytes allocated

        Memory usage summary: heap total: 443856, heap peak: 87550, stack peak: 8640
                 total calls   total memory   failed calls
         malloc|       7827         443856              0
        realloc|          0              0              0  (nomove:0, dec:0, free:0)
         calloc|          0              0              0
           free|       7825         370128

    StringPieceTable     :

        ==640947==   total heap usage: 4,355 allocs, 4,355 frees, 400,667 bytes allocated

        Memory usage summary: heap total: 400675, heap peak: 82525, stack peak: 8688
                 total calls   total memory   failed calls
         malloc|       4355         400675              0
        realloc|          0              0              0  (nomove:0, dec:0, free:0)
         calloc|          0              0              0
           free|       4353         326947

    String               :

        ==640483==   total heap usage: 1,606 allocs, 1,606 frees, 246,577 bytes allocated

        Memory usage summary: heap total: 246585, heap peak: 81161, stack peak: 5424
                 total calls   total memory   failed calls
         malloc|       1606         246585              0
        realloc|          0              0              0  (nomove:0, dec:0, free:0)
         calloc|          0              0              0
           free|       1604         172857
```

## building and debugging

```s
make CC=clang CXX=clang++ test_debug<TAB>
```

## debugging

set `.debug_operations = true` to view debug function call information related to `insert` and `erase` from the `GenericPieceTable`

set `.debug = true` to view debug output from the `GenericPieceTable`, in addition to `debug_operations`

example output of `debug info`

```s
inserting X at position 1: this:     opendorigin is foodorigin is nothing
GenericPieceTable start
    origin: origin is gamesorigin is foodorigin is nothingorigin is a mess
    append: origin is gamesorigin is foodorigin is nothingorigin is a mess
  origin descriptor size: 5
      origin 0: s:6, len:0, buffer content: 
      origin 1: s:15, len:14, buffer content: origin is food
      origin 2: s:29, len:17, buffer content: origin is nothing
      origin 3: s:46, len:1, buffer content: o
      origin 4: s:56, len:0, buffer content: 
  append descriptor size: 3
      append 0: s:0, len:0, buffer content: 
      append 1: s:8, len:4, buffer content: pend
      append 2: s:12, len:0, buffer content: 
   order size: 8
      origin 0: s:46, len:1, buffer content: o
      append 1: s:12, len:0, buffer content: 
      origin 2: s:56, len:0, buffer content: 
      append 3: s:0, len:0, buffer content: 
      origin 4: s:6, len:0, buffer content: 
      append 5: s:8, len:4, buffer content: pend
      origin 6: s:15, len:14, buffer content: origin is food
      origin 7: s:29, len:17, buffer content: origin is nothing
GenericPieceTable end

clamped_position_start: 1
piece_order_size: 8
position: 1, next_LEN: 1
position: 1, next_LEN: 1
position: 1, next_LEN: 1
position: 1, next_LEN: 1
position: 1, next_LEN: 1
position: 1, next_LEN: 5
lookup return character p
target_start.third: 1
splitting buffer
looking up piece at position 1
is_end: false
target_start.third == clamped_position_start: true

inserted X at position 1: this:     oXpendorigin is foodorigin is nothing
GenericPieceTable start
    origin: origin is gamesorigin is foodorigin is nothingorigin is a messX
    append: origin is gamesorigin is foodorigin is nothingorigin is a messX
  origin descriptor size: 6
      origin 0: s:6, len:0, buffer content: 
      origin 1: s:15, len:14, buffer content: origin is food
      origin 2: s:29, len:17, buffer content: origin is nothing
      origin 3: s:46, len:1, buffer content: o
      origin 4: s:56, len:0, buffer content: 
      origin 5: s:62, len:1, buffer content: X
  append descriptor size: 3
      append 0: s:0, len:0, buffer content: 
      append 1: s:8, len:4, buffer content: pend
      append 2: s:12, len:0, buffer content: 
   order size: 9
      origin 0: s:46, len:1, buffer content: o
      append 1: s:12, len:0, buffer content: 
      origin 2: s:56, len:0, buffer content: 
      append 3: s:0, len:0, buffer content: 
      origin 4: s:6, len:0, buffer content: 
      origin 5: s:62, len:1, buffer content: X
      append 6: s:8, len:4, buffer content: pend
      origin 7: s:15, len:14, buffer content: origin is food
      origin 8: s:29, len:17, buffer content: origin is nothing
GenericPieceTable end
```

# usage and api

define `MINIDOC_GENERIC_PIECE_TABLE_FUNCTION_TYPE` before including `generic_piece_table.h` to override the function type used for storing functions
 - defaults to `std::function`

`GenericPieceTable` provides the following `public` constructors
```cpp
GenericPieceTable()
GenericPieceTable(
    GenericPieceTableDescriptorFunctions<DESCRIPTOR_CONTAINER_T> descriptor_functions,
    GenericPieceTableDescriptorOrderFunctions<DESCRIPTOR_ORDER_CONTAINER_T> descriptor_order_functions,
    GenericPieceTableFunctions<ORIGIN_CHAR_CONTAINER_T, ORIGIN_BUFFER_CONTAINER_T> origin_functions,
    GenericPieceTableFunctions<APPEND_CHAR_CONTAINER_T, APPEND_BUFFER_CONTAINER_T> append_functions
)
GenericPieceTable(const THIS & other) // copy constructor
THIS & operator= (const THIS & other) // copy assignment
virtual ~GenericPieceTable()
```

`GenericPieceTable` provides the following `public` functions
```cpp
const Info<ORIGIN_BUFFER_CONTAINER_T, ORIGIN_CHAR_CONTAINER_T> & get_origin_info() const
const Info<APPEND_BUFFER_CONTAINER_T, ORIGIN_CHAR_CONTAINER_T> & get_append_info() const
THIS & append_origin(ORIGIN_CHAR_CONTAINER_T content)
THIS & append(APPEND_CHAR_CONTAINER_T content)
THIS & insert_origin(ORIGIN_CHAR_CONTAINER_T content, std::size_t position)
THIS & insert(APPEND_CHAR_CONTAINER_T content, std::size_t position)
THIS & replace_origin(ORIGIN_CHAR_CONTAINER_T content, std::size_t position, std::size_t length)
THIS & replace(APPEND_CHAR_CONTAINER_T content, std::size_t position, std::size_t length)
THIS & erase_origin(std::size_t position, std::size_t length)
THIS & erase(std::size_t position, std::size_t length)
THIS & reset()
THIS & clear_origin()
THIS & clear()
std::string string() const
std::size_t length() const
std::size_t size() const
std::ostream & buffer_to_stream(std::ostream & os, const char * tag, INFO & info) const
std::ostream & order_to_stream(std::ostream & os) const
```
`GenericPieceTable` also provides `operator <<` support for printing to streams, however `.string()` is recommended
```cpp
// overload for operator <<
//   allows std::cout << table << std::endl;
//
// prints the entire content of the piece table including all buffers and descriptors
//
template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
std::ostream & operator<<(std::ostream & os, const GenericPieceTable<T1, T2, T3, T4, T5, T6> & obj)
```

both `.string()` and `operator<<` are expensive operations since `all descriptor buffers must be iterated` in order to provide the required data

`length()` and `size()` is a cheap operation since we only need to add up the length of each descriptor

# implementation usage

in the below implementation, we use
 - `linked list` to supply the container for the descriptors
   - a `vector` cannot be used since the descriptors would be `moved` during reallocation
   - we could use an index however this would be complicated to manage with descriptor insertion
 - `const char *` as the origin input
 - `const char *` as the append input
 - `std::string` as the origin buffer type
 - `std::string` as the append buffer type

`const char *` is passed as input, eg `append(const char * content)`

the `content` is then appended to the respective `buffer`, eg `buffer = buffer + content` in which `buffer` is `std::string`

```cpp
struct String : public GenericPieceTable<
    std::list<GenericPieceTableDescriptor>,
    std::list<GenericPieceTableDescriptorOrder>,
    const char*, const char*, std::string, std::string
> {
    String() : GenericPieceTable(
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
```

# implementation details

this section will deconstruct the behaviour of the piece table function by function

first, we maintain `5` buffers
 - origin buffer
 - append buffer
 - origin descriptors
 - append descriptors
 - ordered descriptors

the `origin descriptors`, `append descriptors`, and `ordered descriptors` all make it easier to manage the overall descriptor order as well as give more detailed output of descriptor layout

we could merge `origin descriptors`, `append descriptors` into `ordered descriptors` but then when editing certain buffers we would need to sort through potentially many descriptors that are unrelated to the buffer being edited
 - for example, we want to edit the origin buffer 
   - if we have 2 origin descriptors, and 500 append descriptors (all prepended edits, eg `"ORIGINAL TEXT" -> "PREPENDED TEXT, ORIGINAL TEXT"`)
   - then we would need to look through all 500 append descriptors before we get to the origin descriptors, thats a lot of descriptor checking !

#### beyond this point we discuss the details of the function implementations (both `public` and `non-public` ) (all debug lines omitted)

##### index operator

```cpp
const char operator[](std::size_t index) const {
    auto l = length();
    if (l == 0) {
        throw std::runtime_error("index out of range");
    }
    
    if (index == -1 || index > (l-1)) index = l-1;

    std::size_t LEN = 0;
    for (size_t i = 0; i < piece_order_size; i++) {
        auto & order = descriptor_order_functions.const_index(piece_order, i);
        auto & descriptor = *order.ptr;
        auto next_LEN = LEN + descriptor.length;
        if (index < next_LEN) {
            // avoid looping through the entire descriptor to find our index
            auto buffer_index = descriptor.start + (index - LEN);
            auto char_ = (order.origin ? origin_info : append_info).container_index_to_char(buffer_index);
            return char_;
        }
        LEN = next_LEN;
    }
    throw std::runtime_error("lookup reached end");
}
```

```cpp
    auto l = length();
    if (l == 0) {
        throw std::runtime_error("index out of range");
    }
    if (index == -1 || index > (l-1)) index = l-1;
```
first, we check if length is zero, because we may have `zero-length descriptors` there is no easy way around this

then the index is clamped so it cannot be greater than `length()-1`

```cpp
    std::size_t LEN = 0;
    for (size_t i = 0; i < piece_order_size; i++) {
        auto & order = descriptor_order_functions.const_index(piece_order, i);
        auto & descriptor = *order.ptr;
        auto next_LEN = LEN + descriptor.length;
        if (index < next_LEN) {
            // avoid looping through the entire descriptor to find our index
            auto buffer_index = descriptor.start + (index - LEN);
            auto char_ = (order.origin ? origin_info : append_info).container_index_to_char(buffer_index);
            return char_;
        }
        LEN = next_LEN;
    }
    throw std::runtime_error("lookup reached end");
```
next, we iterate the ordered pieces, and throw once we reach the end

in order to iterate, we keep a length variable `LEN`, this represents our absolute index

```cpp
        auto & order = descriptor_order_functions.const_index(piece_order, i);
        auto & descriptor = *order.ptr;
        auto next_LEN = LEN + descriptor.length;
        if (index < next_LEN) {
            // avoid looping through the entire descriptor to find our index
            auto buffer_index = descriptor.start + (index - LEN);
            auto char_ = (order.origin ? origin_info : append_info).container_index_to_char(buffer_index);
            return char_;
        }
        LEN = next_LEN;
```

in the loop body

```cpp
        auto & order = descriptor_order_functions.const_index(piece_order, i);
        auto & descriptor = *order.ptr;
```
we obtain the `descriptor order` that is located at index `i`, and store its `descriptor reference` into the `descriptor` variable

```cpp
        auto next_LEN = LEN + descriptor.length;
        if (index < next_LEN) {
            // avoid looping through the entire descriptor to find our index
            auto buffer_index = descriptor.start + (index - LEN);
            auto char_ = (order.origin ? origin_info : append_info).container_index_to_char(buffer_index);
            return char_;
        }
        LEN = next_LEN;
```
next we check if our index is in range
 - if we have piece length 10, absolute index 5, and target index 12, we know that `5 + 10` is `15`, and we know that `12` is inside the range of `5 to 15`

if our index `is not in range` then we set `LEN` to our computed `next_LEN` (`15` in the above example) and try again with the next `i`

if our index `is in range` then we return the character that corresponds to that range and index, from the buffer specified by `descriptor order`
- we shortcut the descriptor loop here
-   `for (i = des.start; i < (des.start + des.length); i++) { if (LEN == index) ... LEN++ }` is equivilant to `des.start + (index - LEN)`
-   and we know our index MUST be inside this range so `LEN == index` is gaurenteed to be true here

##### lookup

```cpp
triple<const GenericPieceTableDescriptorOrder *, std::size_t, std::size_t, bool>
lookup(std::size_t position) const {
    std::size_t LEN = 0;
    for (size_t i = 0; i < piece_order_size; i++) {
        auto & order = descriptor_order_functions.const_index(piece_order, i);
        auto & descriptor = *order.ptr;
        auto next_LEN = LEN + descriptor.length;
        if (position < next_LEN) {
            return {&order, i, LEN};
        }
        LEN = next_LEN;
    }
    return {nullptr, piece_order_size, LEN};
}
```

same deal as `operator[]` but actually returning information, this is a private function so we can be sure it is called correctly

we return the `descriptor order piece`, `descriptor order piece index`, `absolute index`

an `EOF` marker that signifies if we reached the end of the pieces can be determined by checking if `.first == nullptr`

### Info structure

this is used a lot, so it is important to understand it

for both `insert` and `erase`, we get passed an `Info` structure, each signifying the `buffer` and `descriptors` to operate on

```cpp
template <typename BUFFER, typename CHAR>
struct Info {
    BUFFER buffer;
    DESCRIPTOR_CONTAINER_T pieces;
    GenericPieceTableFunctions<CHAR, BUFFER> functions;
// ...
}

Info<ORIGIN_BUFFER_CONTAINER_T, ORIGIN_CHAR_CONTAINER_T> origin_info;
Info<APPEND_BUFFER_CONTAINER_T, APPEND_CHAR_CONTAINER_T> append_info;

DESCRIPTOR_ORDER_CONTAINER_T piece_order;
std::size_t piece_order_size = 0;

GenericPieceTableDescriptorFunctions<DESCRIPTOR_CONTAINER_T> descriptor_functions;
GenericPieceTableDescriptorOrderFunctions<DESCRIPTOR_ORDER_CONTAINER_T> descriptor_order_functions;
```

this `Info` struct defines a few functions that are self explanatory, and is merely for convinience of having `2` variables instead of `6` variables

the `buffer` is any container which satisfies the `origin_functions` and `append_functions` requirements, in which the example `StringPieceTable` comments each function

the `pieces` is any container which satisfies the `descriptor_functions` requirements, and additionally must use `non-invalidating references` in which a reference `MUST NOT` be `invalidated` upon a `resize` operation (anything that changes the length of the internal buffer counts as a `resize`)
 - for example, a `vector of int` and you obtain a reference to an element
   - if you expand or shrink the vector this reference will be `invalidated` due to `realloc-like` behaviour of `contigious array resizing`
     - execute `man realloc` in your local unix terminal (or look up the manpage online)
     - or look up `std::vector iterator invalidation`
 - a `list of int` does not have this `invalidating` behaviour upon `expand or shrink` of the list like a `vector` does
   - this is because each `int` is stored in an `allocated Node-like object` and `does not move during a resize-like operation`
   - look up `linked list data structure`

`PLEASE NOTE` that a `reference` is explicitly used, this is because using an `iterator invalidating data structure` would require complex `index tracking` for `descriptor insertion` logic to keep all indexes up-to-date
 - this `index updating` can be `expensive with large amounts of descriptors`

the `functions` holds functions that operate on `buffer`

the `descriptor_functions` operate on `pieces` variable of the `Info` struct

`piece_order` is any container which satisties the `descriptor_order_functions` requirements

`piece_order_size` is the cached length of the `piece_order`, how many elements it currently contains

##### split_insert

this one is a long one so lets split this into chunks, ironically

so, this one basically handles the insertion logic of the piece table

we have 4 cases here
 - case 1: index == start
 - case 2: index == end
 - case 3: index == middle and at start of piece boundary
 - case 4: index == middle and at middle/end of piece boundary

```cpp
template <typename INFO>
GenericPieceTableDescriptor & split_insert(bool is_origin, INFO & info, std::size_t clamped_position_start) {
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
```
this handles `case 1`, an obvious shortcut, we check if the position is zero and then append to the start of the pieces

lets explain this in more detail

```cpp
        auto & piece = info.pieces;
        auto len = descriptor_functions.length(piece);
```
first we obtain the `origin/append descriptors` from the specified `Info` struct, as well its `length`

```cpp
        descriptor_functions.append(piece, {0, 0});
        auto & insert_piece = descriptor_functions.index(piece, len);
```
we then append an empty `descriptor` to the specified `piece` and then obtain its `reference`

we cannot assume `append` will return anything as we are generic, so we must locate it by index

```cpp
        descriptor_order_functions.insert(piece_order, {is_origin, &insert_piece}, 0);
        piece_order_size++;
        return insert_piece;
```
we then insert this reference to our `piece_order` at index `0`, so it will be `pre-pended` to the list, think of it like `string("AB").insert("C", 0) == "CAB"`

we then update our element count and return the `reference`



so, that was for `zero index`

if our index `is not zero` then we move on to `case 2`

```cpp
    auto target_start = lookup(clamped_position_start);
```

we `lookup` our `clamped_position_start`, which is just the insertion position pased to `insert(content, position)` which has been clamped to reasonable range

```cpp
    bool is_end = target_start.first == nullptr;

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
```
this handles `case 2`

we check if our `position` represents the `end`, eg, `EOF` so to speak

if `position` is `end` then we use the same as `zero index` except we instead append to the `end` of the list, think of it like `string("AB").insert("C", -1) == "ABC"`

if `position` is not `end` then we move on

so far we have handled the case `index == start` and `index == end`

now we handle `index == middle` aka `case 3` and `case 4`

this can be thought of a `2 sub-cases` of a `case 3: index == middle`
 -  sub-case 1: we are inserting right at the start of a piece descriptor, in this case we follow `zero-index` logic except we insert at `target_start.second` instead of `0` for the `piece_order`, think of it like `string("ABBE").insert("C", 1) == "ABCBE"` where `AB` and `BE` are seperate `descriptor` pieces
 -  sub-case 2: we are inserting

```cpp
    if (target_start.third == clamped_position_start) {
        // ...
        return insert_piece;
```
this detects `case 3` aka `sub-case 1`, same deal as `case 1`/`case 2` but with different `piece_order` index

```cpp
    } else {
        auto diff = clamped_position_start - target_start.third;
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
        descriptor_functions.append(piece2, {target_start.first->ptr->start + diff, old_length - diff});
        auto & insert_piece2 = descriptor_functions.index(piece2, len2);
        descriptor_order_functions.insert(piece_order, {target_start.first->origin, &insert_piece2}, target_start.second+2);
        piece_order_size++;
        return insert_piece;
    }
}
```
this handles `case 4` akd `sub-case 2` and subsequently the final case of the `insertion logic`

basically for `case 4` we split into 2 pieces, `"ABCD" > "ABC" "" "D"`, calculating the offsets for the `descriptors` and insert those into `piece_order`

```cpp
        auto diff = clamped_position_start - target_start.third;
        auto old_length = target_start.first->ptr->length;
        target_start.first->ptr->length = diff;
```
chop text at index `"ABCD" > "ABC"`

```cpp
        auto & piece = info.pieces;
        auto len = descriptor_functions.length(piece);
        descriptor_functions.append(piece, {0, 0});
        auto & insert_piece = descriptor_functions.index(piece, len);
        descriptor_order_functions.insert(piece_order, {is_origin, &insert_piece}, target_start.second+1);
        piece_order_size++;
```
append empty `descriptor` into `piece` and its reference into `piece_order` at `target_start.second+1`

```cpp
        auto & piece2 = target_start.first->origin ? origin_info.pieces : append_info.pieces;
        auto len2 = descriptor_functions.length(piece2);
        descriptor_functions.append(piece2, {target_start.first->ptr->start + diff, old_length - diff});
        auto & insert_piece2 = descriptor_functions.index(piece2, len2);
        descriptor_order_functions.insert(piece_order, {target_start.first->origin, &insert_piece2}, target_start.second+2);
        piece_order_size++;
        return insert_piece;
```
insert the rest of the chopped text `"D"` into the buffer specified by `target_start` and insert its reference into `piece_order` after our empty descriptor

then return the reference to the empty descriptor

##### split_erase

this one is another long one so lets split this into chunks, ironically

first up

```cpp
    template <typename INFO>
    void split_erase(bool is_origin, INFO & info, std::size_t clamped_position_start, std::size_t clamped_position_end) {
        auto target_end = lookup(clamped_position_end);
        bool is_end = target_end.first == nullptr;
        if (is_end) {
            if (clamped_position_start == 0) {

                reset();
                return;
            }
        }
```
if we are erasing the entire buffer then we can simply call `reset()` and be done with the operation, `as-if it had just been constructed`

```cpp
    auto target_start = lookup(clamped_position_start);
    auto diff = clamped_position_start - target_start.third;
    auto old_length = target_start.first->ptr->length;
    target_start.first->ptr->length = diff;
    if (target_start.second == target_end.second) {
        if (target_start.third == clamped_position_start) {
            auto new_len = old_length - ((target_end.third + old_length) - clamped_position_end);
            *target_start.first->ptr = {target_start.first->ptr->start + new_len, old_length - new_len};
```
so, if we are modiifying the same buffer, and the start index points to the start of our buffer, then we simply offset the start and length, `"ABCDEF" > "DEF"`

```cpp
        } else {
            auto & piece = target_start.first->origin ? origin_info.pieces : append_info.pieces;
            auto len1 = descriptor_functions.length(piece);
            auto new_len = old_length - ((target_end.third + old_length) - clamped_position_end);
            descriptor_functions.append(piece, {target_start.first->ptr->start + new_len, old_length - new_len});
            auto & insert_piece1 = descriptor_functions.index(piece, len1);
            descriptor_order_functions.insert(piece_order, {target_start.first->origin, &insert_piece1}, target_start.second+1);
            piece_order_size++;
        }
```
otherwise if we are modifying the same buffer, and our start index is not pointing to the start of our buffer, then we split into 2 pieces, `"ABCDEF" > "ABC" "EF"`, calculating the offsets for the `descriptors` and insert those into `piece_order`

you should know the details by know

```cpp
            auto & piece = target_start.first->origin ? origin_info.pieces : append_info.pieces;
            auto len1 = descriptor_functions.length(piece);
```
get piece

```cpp
            auto new_len = old_length - ((target_end.third + old_length) - clamped_position_end);
            descriptor_functions.append(piece, {target_start.first->ptr->start + new_len, old_length - new_len});
```
calculate offsets

```cpp
            auto & insert_piece1 = descriptor_functions.index(piece, len1);
            descriptor_order_functions.insert(piece_order, {target_start.first->origin, &insert_piece1}, target_start.second+1);
            piece_order_size++;
```
insert descriptor

```cpp
    } else {
        if (!is_end) {
            auto diff = clamped_position_end - target_end.third;
            target_end.first->ptr->start += diff;
            target_end.first->ptr->length -= diff;
        }

        if (target_start.second+1 != target_end.second) {
            // clear all pieces between start and end
            for (auto i = target_start.second+1; i < target_end.second; i++) {
                auto & order = descriptor_order_functions.const_index(piece_order, i);
                auto & descriptor = *order.ptr;
                descriptor.length = 0;
            }
        }
    }
}
```
otherwise, we are erasing multiple buffers

if we are not at the end then we offset the start pointer

if we are erasing more than 2 buffers ( `"Agw", "ewgwgBgww", "C3r34w"` ) then we set the middle buffers to zero-length ( `"Ag", "", "r34w"` )

and we are done with erase

##### the rest

now that we have `split_insert` and `split_erase` done, the rest is straight forward

```cpp
    template <typename INFO, typename C>
    void insert_buffer(bool is_origin, INFO & info, C & content, std::size_t position) {
        auto content_length = info.functions.content_length(content);
        if (content_length == 0) {
            return;
        }
        auto end = length();
        auto position_ = position >= end ? end : position;
        auto & descriptor = split_insert(is_origin, info, position_);
        descriptor.start = info.container_length();
        descriptor.length = content_length;
        info.functions.append(info.buffer, content);
    }
```
`insert_buffer` with range checks

```cpp
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
        split_erase(is_origin, info, position_, position_end);
    }
```
`erase_buffer` with range checks

```cpp
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

    THIS & erase_origin(std::size_t position, std::size_t length) {
        erase_buffer(true, origin_info, position, length);
        return *this;
    }

    THIS & erase(std::size_t position, std::size_t length) {
        erase_buffer(false, append_info, position, length);
        return *this;
    }
```
`append`, `insert`, and `erase`

```cpp
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
```
`replace` with range checks