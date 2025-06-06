#include "../include/allocator_boundary_tags.h"
#include <cstddef>
#include <mutex>
#include <new>
#include <not_implemented.h>
#include <numeric>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

logger *allocator_boundary_tags::get_logger(void *ptr)
{
    return *reinterpret_cast<logger **>(reinterpret_cast<byte *>(ptr) + sizeof(std::mutex));
}

std::pmr::memory_resource *allocator_boundary_tags::get_memory_resource(void *ptr)
{
    return *reinterpret_cast<std::pmr::memory_resource **>(reinterpret_cast<byte *>(ptr) + sizeof(std::mutex) + sizeof(logger *));
}

allocator_with_fit_mode::fit_mode &allocator_boundary_tags::get_fit_mode(void *ptr)
{
    return *reinterpret_cast<allocator_with_fit_mode::fit_mode *>(reinterpret_cast<byte *>(ptr) + sizeof(std::mutex) + sizeof(logger *) + sizeof(std::pmr::memory_resource *));
}

size_t &allocator_boundary_tags::get_allocator_pool_size(void *ptr)
{
    return *reinterpret_cast<size_t *>(reinterpret_cast<byte *>(ptr) + sizeof(std::mutex) + sizeof(logger *) + sizeof(std::pmr::memory_resource *) + sizeof(allocator_with_fit_mode::fit_mode));
}

void *&allocator_boundary_tags::get_first_occupied_block(void *ptr)
{
    return *reinterpret_cast<void **>(reinterpret_cast<byte *>(ptr) + sizeof(std::mutex) + sizeof(logger *) + sizeof(std::pmr::memory_resource *) + sizeof(allocator_with_fit_mode::fit_mode) + sizeof(size_t));
}

size_t &allocator_boundary_tags::get_ptr_size(void *ptr)
{
    return *reinterpret_cast<size_t *>(ptr);
}

void *allocator_boundary_tags::get_prev_occupied(void *ptr)
{
    return *reinterpret_cast<void **>(reinterpret_cast<byte *>(ptr) + sizeof(size_t));
}

void *allocator_boundary_tags::get_next_occupied(void *ptr)
{
    return *reinterpret_cast<void **>(reinterpret_cast<byte *>(ptr) + sizeof(size_t) + sizeof(void *));
}

void *allocator_boundary_tags::get_pool_start(void *ptr)
{
    return reinterpret_cast<byte *>(ptr) + allocator_metadata_size;
}

void *allocator_boundary_tags::get_pool_end(void *ptr)
{
    return reinterpret_cast<byte *>(get_pool_start(ptr)) + get_allocator_pool_size(ptr);
}

size_t &allocator_boundary_tags::get_user_size_of_block(void *ptr)
{
    return *reinterpret_cast<size_t *>(reinterpret_cast<byte *>(ptr));
}

void *&allocator_boundary_tags::get_prev_block(void *ptr)
{
    return *reinterpret_cast<void **>(reinterpret_cast<byte *>(ptr) + sizeof(size_t));
}

void *&allocator_boundary_tags::get_next_block(void *ptr)
{
    return *reinterpret_cast<void **>(reinterpret_cast<byte *>(ptr) + sizeof(size_t) + sizeof(void *));
}

void *&allocator_boundary_tags::get_block_allocator(void *ptr)
{
    return *reinterpret_cast<void **>(reinterpret_cast<byte *>(ptr) + sizeof(size_t) + sizeof(void *) + sizeof(void *));
}

void *allocator_boundary_tags::get_user_ptr_from_meta(void *ptr)
{
    return reinterpret_cast<byte *>(ptr) + occupied_block_metadata_size;
}

void *allocator_boundary_tags::get_meta_ptr_from_user(void *ptr)
{
    return reinterpret_cast<byte *>(ptr) - occupied_block_metadata_size;
}

std::mutex &allocator_boundary_tags::get_mutex() const noexcept
{
    return *reinterpret_cast<std::mutex *>(_trusted_memory);
}

size_t allocator_boundary_tags::get_free_size_between_blocks(void *prev_block_meta, void *trusted_memory) noexcept
{
    if (!trusted_memory)
    {
        return 0; //  next is nullptr
    }

    void *ptr_to_free_block_start = nullptr;
    void *next_occupied_block = nullptr;

    if (prev_block_meta == trusted_memory)
    {
        ptr_to_free_block_start = get_pool_start(trusted_memory);
    }
    else
    {
        if (!prev_block_meta)
        {
            return 0;
        }
        size_t prev_block_size = get_user_size_of_block(prev_block_meta);
        ptr_to_free_block_start =
            reinterpret_cast<byte *>(prev_block_meta) + occupied_block_metadata_size + prev_block_size;
    }

    next_occupied_block = (prev_block_meta == trusted_memory) ? get_first_occupied_block(trusted_memory) : get_next_block(prev_block_meta);

    void *ptr_to_free_block_end = (next_occupied_block == nullptr) ? get_pool_end(trusted_memory) : next_occupied_block;

    if (reinterpret_cast<byte *>(ptr_to_free_block_start) >= reinterpret_cast<byte *>(ptr_to_free_block_end))
    {
        return 0;
    }

    return reinterpret_cast<byte *>(ptr_to_free_block_end) - reinterpret_cast<byte *>(ptr_to_free_block_start);
}

void *allocator_boundary_tags::find_first_suitable_block(size_t block_size_to_find) noexcept
{
    debug_with_guard("Method find_first_suitable_block started. Block size to find: " + std::to_string(block_size_to_find));

    if (!_trusted_memory)
    {
        debug_with_guard("Trusted memory is null. Exiting find_first_suitable_block.");
        return nullptr;
    }

    void *previous_block = _trusted_memory;
    void *current_block = get_first_occupied_block(_trusted_memory);

    while (true)
    {
        size_t free_space_size = get_free_size_between_blocks(previous_block, _trusted_memory);

        if (free_space_size >= block_size_to_find)
        {
            debug_with_guard("Suitable block found. Address: " + std::to_string(reinterpret_cast<uintptr_t>(previous_block)));
            return previous_block;
        }

        if (current_block == nullptr)
        {
            break;
        }

        previous_block = current_block;
        current_block = get_next_block(current_block);
    }

    debug_with_guard("No suitable block found in find_first_suitable_block.");
    return nullptr;
}

void *allocator_boundary_tags::find_best_suitable_block(size_t block_size_to_find) noexcept
{
    debug_with_guard("Method find_best_suitable_block started. Block size to find: " + std::to_string(block_size_to_find));

    if (!_trusted_memory)
    {
        debug_with_guard("Trusted memory is null. Exiting find_best_suitable_block.");
        return nullptr;
    }

    void *best_prev_block = nullptr;
    size_t min_suitable_gap = SIZE_MAX;

    void *current_occupied = nullptr;
    void *prev_occupied = _trusted_memory;

    current_occupied = get_first_occupied_block(_trusted_memory);

    while (true)
    {
        size_t gap_size = get_free_size_between_blocks(prev_occupied, _trusted_memory);

        if (gap_size >= block_size_to_find)
        {
            if (!best_prev_block || gap_size < min_suitable_gap)
            {
                min_suitable_gap = gap_size;
                best_prev_block = prev_occupied;
            }
        }

        if (current_occupied == nullptr)
        {
            break;
        }
        prev_occupied = current_occupied;
        current_occupied = get_next_block(current_occupied);
    }

    if (best_prev_block)
    {
        debug_with_guard(
            "Best suitable block found. Address: " + std::to_string(reinterpret_cast<uintptr_t>(best_prev_block)) + ", Gap size: " + std::to_string(min_suitable_gap));
    }
    else
    {
        debug_with_guard("No suitable block found in find_best_suitable_block.");
    }

    return best_prev_block;
}

void *allocator_boundary_tags::find_worst_suitable_block(size_t block_size_to_find) noexcept
{
    debug_with_guard("Method find_worst_suitable_block started. Block size to find: " + std::to_string(block_size_to_find));

    if (!_trusted_memory)
    {
        debug_with_guard("Trusted memory is null. Exiting find_worst_suitable_block.");
        return nullptr;
    }

    void *worst_prev_block = nullptr;
    size_t max_suitable_gap = 0;

    void *current_occupied = nullptr;
    void *prev_occupied = _trusted_memory;

    current_occupied = get_first_occupied_block(_trusted_memory);

    while (true)
    {
        size_t gap_size = get_free_size_between_blocks(prev_occupied, _trusted_memory);

        if (gap_size >= block_size_to_find)
        {
            if (!worst_prev_block || gap_size > max_suitable_gap)
            {
                max_suitable_gap = gap_size;
                worst_prev_block = prev_occupied;
            }
        }

        if (current_occupied == nullptr)
        {
            break;
        }
        prev_occupied = current_occupied;
        current_occupied = get_next_block(current_occupied);
    }

    if (worst_prev_block)
    {
        debug_with_guard(
            "Worst suitable block found. Address: " + std::to_string(reinterpret_cast<uintptr_t>(worst_prev_block)) + ", Gap size: " + std::to_string(max_suitable_gap));
    }
    else
    {
        debug_with_guard("No suitable block found in find_worst_suitable_block.");
    }

    return worst_prev_block;
}

size_t allocator_boundary_tags::get_free_size() const noexcept
{

    if (!_trusted_memory)
    {
        return 0;
    }

    size_t total_free_area = 0;
    void *current_occupied = nullptr;
    void *prev_occupied = nullptr;

    try
    {
        prev_occupied = _trusted_memory;
        current_occupied = get_first_occupied_block(_trusted_memory);

        while (true)
        {
            total_free_area += get_free_size_between_blocks(prev_occupied, _trusted_memory);

            if (current_occupied == nullptr)
            {
                break;
            }
            prev_occupied = current_occupied;
            current_occupied = get_next_block(current_occupied);
        }
    }
    catch (...)
    {
        return 0;
    }

    return total_free_area;
}

logger *allocator_boundary_tags::get_logger() const
{
    if (!_trusted_memory)
        return nullptr;
    return get_logger(_trusted_memory);
}

std::string allocator_boundary_tags::get_typename() const noexcept
{
    return "allocator_boundary_tags";
}

void allocator_boundary_tags::set_fit_mode(allocator_with_fit_mode::fit_mode mode)
{
    std::lock_guard lock(get_mutex());

    if (!_trusted_memory)
    {
        return;
    }
    allocator_with_fit_mode::fit_mode &current_mode = get_fit_mode(_trusted_memory);
    current_mode = mode;
}

allocator_boundary_tags::~allocator_boundary_tags()
{
    debug_with_guard("~allocator_boundary_tags started");

    if (_trusted_memory)
    {
        auto *alloc = get_memory_resource(_trusted_memory);

        if (alloc)
        {
            alloc->deallocate(_trusted_memory, get_allocator_pool_size(_trusted_memory) + allocator_metadata_size,
                              alignof(std::max_align_t));
        }
        else
        {
            ::operator delete(_trusted_memory);
        }
    }

    debug_with_guard("~allocator_boundary_tags finished");
}

allocator_boundary_tags::allocator_boundary_tags(allocator_boundary_tags &&other) noexcept
    : _trusted_memory(other._trusted_memory)
{
    other._trusted_memory = nullptr;
    debug_with_guard("allocator_boundary_tags(allocator_boundary_tags &&other) started");
}

allocator_boundary_tags &allocator_boundary_tags::operator=(allocator_boundary_tags &&other) noexcept
{
    if (this != &other)
    {
        if (_trusted_memory)
        {
            this->~allocator_boundary_tags();
            _trusted_memory = nullptr;
        }
        logger_guardant::operator=(std::move(other));
        typename_holder::operator=(std::move(other));
        _trusted_memory = other._trusted_memory;
        other._trusted_memory = nullptr;
    }
    return *this;
}

allocator_boundary_tags::allocator_boundary_tags(size_t space_size, std::pmr::memory_resource *parent_allocator,
                                                 logger *logger_instance,
                                                 allocator_with_fit_mode::fit_mode allocate_fit_mode)
    : _trusted_memory(nullptr)
{
    debug_with_guard("Constructor allocator_boundary_tags started.");

    if (space_size < occupied_block_metadata_size)
    {
        error_with_guard("Requested space size is too small (less than occupied block metadata).");
        throw std::logic_error("Requested space size is too small (less than occupied block metadata).");
    }

    size_t total_size_needed = space_size + allocator_metadata_size;

    void *allocated_memory = nullptr;

    try
    {
        if (parent_allocator == nullptr)
        {
            allocated_memory = ::operator new(total_size_needed);
        }
        else
        {
            allocated_memory = parent_allocator->allocate(total_size_needed, alignof(std::max_align_t));
        }
    }
    catch (...)
    {
        error_with_guard("Memory allocation failed.");
        throw;
    }

    if (!allocated_memory)
    {
        error_with_guard("Memory allocation returned nullptr.");
        throw std::runtime_error("Memory allocation returned nullptr.");
    }

    _trusted_memory = allocated_memory;

    try
    {
        new (reinterpret_cast<std::mutex *>(_trusted_memory)) std::mutex();

        *reinterpret_cast<logger **>(reinterpret_cast<byte *>(_trusted_memory) + sizeof(std::mutex)) = logger_instance;

        *reinterpret_cast<std::pmr::memory_resource **>(reinterpret_cast<byte *>(_trusted_memory) + sizeof(std::mutex) + sizeof(logger *)) = parent_allocator;

        *reinterpret_cast<allocator_with_fit_mode::fit_mode *>(reinterpret_cast<byte *>(_trusted_memory) + sizeof(std::mutex) + sizeof(logger *) + sizeof(std::pmr::memory_resource *)) = allocate_fit_mode;

        *reinterpret_cast<size_t *>(reinterpret_cast<byte *>(_trusted_memory) + sizeof(std::mutex) + sizeof(logger *) + sizeof(std::pmr::memory_resource *) + sizeof(allocator_with_fit_mode::fit_mode)) = space_size;

        *reinterpret_cast<void **>(reinterpret_cast<byte *>(_trusted_memory) + sizeof(std::mutex) + sizeof(logger *) + sizeof(std::pmr::memory_resource *) + sizeof(allocator_with_fit_mode::fit_mode) + sizeof(size_t)) = nullptr;
    }
    catch (...)
    {
        if (parent_allocator == nullptr)
        {
            ::operator delete(_trusted_memory);
        }
        else
        {
            parent_allocator->deallocate(_trusted_memory, total_size_needed, alignof(std::max_align_t));
        }
        _trusted_memory = nullptr;
        error_with_guard("Bad alloc");
        throw;
    }

    debug_with_guard("Constructor allocator_boundary_tags finished successfully.");
}

[[nodiscard]] void *allocator_boundary_tags::do_allocate_sm(size_t size)
{
    debug_with_guard("Method do_allocate_sm started.");
    std::lock_guard lock(get_mutex());

    try
    {
        size_t requested_total_size = size + occupied_block_metadata_size;

        void *current_block_to_alloc = nullptr;
        const allocator_with_fit_mode::fit_mode fit = get_fit_mode(_trusted_memory);

        if (fit == fit_mode::the_best_fit)
        {
            current_block_to_alloc = find_best_suitable_block(requested_total_size);
        }
        else if (fit == fit_mode::the_worst_fit)
        {
            current_block_to_alloc = find_worst_suitable_block(requested_total_size);
        }
        else
        {
            current_block_to_alloc = find_first_suitable_block(requested_total_size);
        }

        if (current_block_to_alloc == nullptr)
        {
            error_with_guard("No suitable block found for allocation.");
            throw std::bad_alloc();
        }

        const size_t current_block_size = get_free_size_between_blocks(current_block_to_alloc, _trusted_memory);

        size_t asked_size_to_alloc = size;
        if (current_block_size < requested_total_size + occupied_block_metadata_size)
        {
            requested_total_size = current_block_size;
            if (requested_total_size < occupied_block_metadata_size)
            {
                throw std::bad_alloc();
            }
            asked_size_to_alloc = requested_total_size - occupied_block_metadata_size;

            warning_with_guard("User allocation request overridden. Requested: " + std::to_string(size) + ", Allocated: " + std::to_string(asked_size_to_alloc));
        }

        void *new_block_meta_start = nullptr;
        if (current_block_to_alloc == _trusted_memory)
        {
            new_block_meta_start = get_pool_start(_trusted_memory);
        }
        else
        {
            new_block_meta_start = reinterpret_cast<byte *>(current_block_to_alloc) + occupied_block_metadata_size +
                                   get_ptr_size(current_block_to_alloc);
        }

        get_user_size_of_block(new_block_meta_start) = asked_size_to_alloc;

        get_prev_block(new_block_meta_start) = current_block_to_alloc;

        void *next_block_meta = (current_block_to_alloc == _trusted_memory) ? get_first_occupied_block(_trusted_memory)
                                                                            : get_next_block(current_block_to_alloc);
        get_next_block(new_block_meta_start) = next_block_meta;

        get_block_allocator(new_block_meta_start) = _trusted_memory;

        if (current_block_to_alloc == _trusted_memory)
        {
            get_first_occupied_block(_trusted_memory) = new_block_meta_start;
        }
        else
        {
            get_next_block(current_block_to_alloc) = new_block_meta_start;
        }
        if (next_block_meta != nullptr)
        {
            get_prev_block(next_block_meta) = new_block_meta_start;
        }

        information_with_guard("Available memory: " + std::to_string(get_free_size()) + " bytes.");

        debug_with_guard("Method do_allocate_sm finished.");
        return get_user_ptr_from_meta(new_block_meta_start);
    }
    catch (const std::exception &e)
    {
        error_with_guard(std::string("Exception caught in do_allocate_sm: ") + e.what());
        throw;
    }
    catch (...)
    {
        error_with_guard("Unknown exception caught in do_allocate_sm.");
        throw;
    }
}

void allocator_boundary_tags::do_deallocate_sm(void *at)
{
    debug_with_guard("Method do_deallocate_sm started.");

    if (at == nullptr)
    {
        debug_with_guard("free nullptr.");
        return;
    }

    if (!_trusted_memory)
    {
        error_with_guard("_trusted_memory == nullptr in deallocate.");
        throw std::runtime_error("_trusted_memory == nullptr in deallocate");
    }

    std::lock_guard lock(get_mutex());

    try
    {
        void *block_metadata = nullptr;
        void *pool_start = get_pool_start(_trusted_memory);
        void *pool_end = get_pool_end(_trusted_memory);
        void *user_pool_start = get_user_ptr_from_meta(pool_start);

        if (reinterpret_cast<byte *>(at) < reinterpret_cast<byte *>(user_pool_start) ||
            reinterpret_cast<byte *>(at) >= reinterpret_cast<byte *>(pool_end))
        {
            error_with_guard("ptr is out of managed pool range.");
            throw std::logic_error("ptr is out of managed pool range");
        }

        block_metadata = get_meta_ptr_from_user(at);
        void *block_allocator = get_block_allocator(block_metadata);

        if (block_allocator != _trusted_memory)
        {
            error_with_guard("Block allocators are not equal.");
            throw std::logic_error("Block allocators are not equal");
        }

        std::unordered_set<void *> visited_blocks;
        void *current_block = get_first_occupied_block(_trusted_memory);

        while (current_block)
        {
            if (!visited_blocks.insert(current_block).second)
            {
                error_with_guard("List corruption (potential cycle) detected.");
                throw std::runtime_error("List corruption (potential cycle) detected");
            }

            if (current_block == block_metadata)
            {
                break;
            }
            current_block = get_next_block(current_block);
        }

        if (current_block != block_metadata)
        {
            error_with_guard("Block not found.");
            throw std::runtime_error("Block not found");
        }

        void *previous_block = get_prev_block(block_metadata);
        void *next_block = get_next_block(block_metadata);

        if (previous_block == _trusted_memory)
        {
            get_first_occupied_block(_trusted_memory) = next_block;
        }
        else
        {
            get_next_block(previous_block) = next_block;
        }

        if (next_block != nullptr)
        {
            get_prev_block(next_block) = previous_block;
        }

        information_with_guard("Available memory after deallocation: " + std::to_string(get_free_size()) + " bytes.");

        debug_with_guard("Memory state after deallocation: ");
    }
    catch (const std::exception &e)
    {
        error_with_guard(std::string("Exception caught in do_deallocate_sm: ") + e.what());
        throw;
    }
    catch (...)
    {
        error_with_guard("Unknown exception caught in do_deallocate_sm.");
        throw;
    }

    debug_with_guard("Method do_deallocate_sm finished.");
}

std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info() const
{
    std::lock_guard lock(get_mutex());
    std::vector<allocator_test_utils::block_info> result;
    return get_blocks_info_inner();
}

std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info_inner() const
{
    std::vector<allocator_test_utils::block_info> res;

    if (!_trusted_memory)
    {
        return res;
    }

    void *pool_start = get_pool_start(_trusted_memory);
    void *pool_end = get_pool_end(_trusted_memory);
    void *current_occupied_meta = get_first_occupied_block(_trusted_memory);
    void *last_block_processed_end = pool_start;

    try
    {
        while (true)
        {
            void *free_block_start = last_block_processed_end;
            void *free_block_end = (current_occupied_meta == nullptr) ? pool_end : current_occupied_meta;

            if (reinterpret_cast<byte *>(free_block_start) < reinterpret_cast<byte *>(free_block_end))
            {
                size_t free_size =
                    reinterpret_cast<byte *>(free_block_end) - reinterpret_cast<byte *>(free_block_start);
                res.push_back({.block_size = free_size, .is_block_occupied = false});
            }
            if (current_occupied_meta == nullptr)
            {
                break;
            }
            size_t user_size = get_user_size_of_block(current_occupied_meta);
            size_t total_block_size = user_size + occupied_block_metadata_size;

            res.push_back({.block_size = total_block_size, .is_block_occupied = true});
            last_block_processed_end = reinterpret_cast<byte *>(current_occupied_meta) + total_block_size;
            current_occupied_meta = get_next_block(current_occupied_meta); // Move to next occupied block
        }
    }
    catch (...)
    {
        res.clear();
    }

    return res;
}

bool allocator_boundary_tags::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    const auto *other_cast = dynamic_cast<const allocator_boundary_tags *>(&other);
    return (other_cast != nullptr && this == other_cast);
}
allocator_boundary_tags::boundary_iterator::boundary_iterator()
    : _occupied_ptr(nullptr), _occupied(false), _trusted_memory(nullptr)
{
}

allocator_boundary_tags::boundary_iterator::boundary_iterator(void *trusted)
    : _trusted_memory(trusted), _occupied_ptr(trusted), _occupied(true)
{
    if (!_trusted_memory)
    {
        _occupied_ptr = nullptr;
        _occupied = false;
        return;
    }

    try
    {
        void *first_occupied = nullptr;
        void *pool_start = nullptr;
        first_occupied = allocator_boundary_tags::get_first_occupied_block(trusted);
        pool_start = allocator_boundary_tags::get_pool_start(trusted);
        if (first_occupied == pool_start)
        {
            _occupied = true;
            _occupied_ptr = first_occupied;
        }
        else
        {
            _occupied = false;
            _occupied_ptr = trusted;
        }
    }
    catch (...)
    {
        _occupied_ptr = nullptr;
        _occupied = false;
    }
}

bool allocator_boundary_tags::boundary_iterator::operator==(
    const allocator_boundary_tags::boundary_iterator &other) const noexcept
{
    bool result = (_occupied_ptr == other._occupied_ptr) && (_occupied == other._occupied || _occupied_ptr == nullptr);
    return result;
}

bool allocator_boundary_tags::boundary_iterator::operator!=(
    const allocator_boundary_tags::boundary_iterator &other) const noexcept
{
    return !(*this == other);
}

allocator_boundary_tags::boundary_iterator &allocator_boundary_tags::boundary_iterator::operator++() & noexcept
{
    if (_occupied_ptr == nullptr)
    {
        return *this;
    }

    if (!_occupied)
    {
        _occupied = true;
        _occupied_ptr = (_occupied_ptr == _trusted_memory)
                            ? allocator_boundary_tags::get_first_occupied_block(_trusted_memory)
                            : allocator_boundary_tags::get_next_block(_occupied_ptr);

        if (_occupied_ptr == nullptr)
        {
            _occupied = false;
        }
    }
    else
    {
        _occupied = false;
        void *next_occupied_meta = allocator_boundary_tags::get_next_block(_occupied_ptr);

        if (next_occupied_meta == nullptr)
        {
            size_t current_user_size = allocator_boundary_tags::get_user_size_of_block(_occupied_ptr);
            void *end_of_current_occupied = reinterpret_cast<byte *>(_occupied_ptr) +
                                            allocator_boundary_tags::occupied_block_metadata_size + current_user_size;
            void *pool_end_ptr = allocator_boundary_tags::get_pool_end(_trusted_memory);

            if (reinterpret_cast<byte *>(end_of_current_occupied) >= reinterpret_cast<byte *>(pool_end_ptr))
            {
                _occupied_ptr = nullptr;
                _occupied = false;
            }
        }
    }

    return *this;
}

allocator_boundary_tags::boundary_iterator &allocator_boundary_tags::boundary_iterator::operator--() & noexcept
{
    if (_occupied_ptr == nullptr && !_occupied)
    {
        return *this;
    }

    try
    {
        if (!_occupied)
        {
            if (_occupied_ptr == _trusted_memory)
            {
                _occupied_ptr = nullptr;
                _occupied = false;
            }
            else
            {
                _occupied = true;
            }
        }
        else
        {
            void *prev_meta = allocator_boundary_tags::get_prev_block(_occupied_ptr);
            _occupied = false;
            _occupied_ptr = prev_meta;
        }
    }
    catch (...)
    {
        _occupied_ptr = nullptr;
        _occupied = false;
    }
    return *this;
}

allocator_boundary_tags::boundary_iterator allocator_boundary_tags::boundary_iterator::operator++(int)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

allocator_boundary_tags::boundary_iterator allocator_boundary_tags::boundary_iterator::operator--(int)
{
    auto tmp = *this;
    --(*this);
    return tmp;
}

size_t allocator_boundary_tags::boundary_iterator::size() const noexcept
{
    if (_occupied_ptr == nullptr)
    {
        return 0;
    }
    if (_occupied)
    {
        return allocator_boundary_tags::get_user_size_of_block(_occupied_ptr) + allocator_boundary_tags::occupied_block_metadata_size;
    }
    else
    {
        return allocator_boundary_tags::get_free_size_between_blocks(_occupied_ptr, _trusted_memory);
    }
}

bool allocator_boundary_tags::boundary_iterator::occupied() const noexcept
{
    return _occupied_ptr != nullptr && _occupied;
}

void *allocator_boundary_tags::boundary_iterator::operator*() const noexcept
{
    if (_occupied_ptr == nullptr)
        return nullptr;

    if (_occupied)
    {
        return allocator_boundary_tags::get_user_ptr_from_meta(_occupied_ptr);
    }

    return (_occupied_ptr == _trusted_memory) ? allocator_boundary_tags::get_pool_start(_trusted_memory) : reinterpret_cast<byte *>(_occupied_ptr) + allocator_boundary_tags::occupied_block_metadata_size + allocator_boundary_tags::get_user_size_of_block(_occupied_ptr);
}

void *allocator_boundary_tags::boundary_iterator::get_ptr() const noexcept
{
    return _occupied_ptr;
}

allocator_boundary_tags::boundary_iterator allocator_boundary_tags::begin() const noexcept
{
    return boundary_iterator(_trusted_memory);
}

allocator_boundary_tags::boundary_iterator allocator_boundary_tags::end() const noexcept
{
    return boundary_iterator();
}
