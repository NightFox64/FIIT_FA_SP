#include "../include/allocator_buddies_system.h"
#include <cstddef>
#include <not_implemented.h>
#include <cstdint>

inline size_t allocator_buddies_system::pow_2(size_t size) const noexcept
{
    return 1 << size;
}

bool allocator_buddies_system::is_busy(void *current_block) const noexcept
{
    return reinterpret_cast<block_metadata *>(current_block)->occupied;
}

byte allocator_buddies_system::stepen_of_2(size_t size) const noexcept
{
    size_t number = 1;
    byte stepen = 0;
    while (number < size)
    {
        number <<= 1;
        ++stepen;
    }
    return stepen;
}

std::pmr::memory_resource *allocator_buddies_system::get_memory_resource() const
{
    return *reinterpret_cast<std::pmr::memory_resource **>(reinterpret_cast<byte *>(_trusted_memory) + sizeof(std::mutex) + sizeof(logger *));
}

logger *allocator_buddies_system::get_logger() const
{
    return *reinterpret_cast<logger **>(reinterpret_cast<std::byte *>(_trusted_memory) + sizeof(std::mutex));
}
std::vector<allocator_test_utils::block_info> allocator_buddies_system::get_blocks_info_inner() const
{
    if (_trusted_memory == nullptr)
    {
        throw std::runtime_error("_trusted_memory is nullptr");
    }
    std::vector<allocator_test_utils::block_info> info_inne;

    auto *cur_block_ptr = reinterpret_cast<void *>(reinterpret_cast<byte *>(_trusted_memory) + allocator_metadata_size);

    const auto *end_ptr = reinterpret_cast<void *>(reinterpret_cast<byte *>(_trusted_memory) + allocator_metadata_size + get_allocator_pool_size());

    while (cur_block_ptr < end_ptr)
    {
        block_metadata *meta = reinterpret_cast<block_metadata *>(cur_block_ptr);
        info_inne.push_back({pow_2(meta->size), meta->occupied});
        cur_block_ptr = reinterpret_cast<void *>(reinterpret_cast<byte *>(cur_block_ptr) + pow_2(meta->size));
    }

    return info_inne;
}

std::vector<allocator_test_utils::block_info> allocator_buddies_system::get_blocks_info() const noexcept
{
    //std::lock_guard<std::mutex> lock(get_mutex());
    return get_blocks_info_inner();
}

allocator_with_fit_mode::fit_mode &allocator_buddies_system::get_fit_mode() const noexcept
{
    return *reinterpret_cast<allocator_with_fit_mode::fit_mode *>(reinterpret_cast<std::byte *>(_trusted_memory) + sizeof(std::mutex) + sizeof(logger *) + sizeof(std::pmr::memory_resource *));
}

std::string allocator_buddies_system::get_typename() const
{
    return "allocator_buddies_system";
}

void *allocator_buddies_system::get_block_twin(void *current_block) noexcept
{
    size_t ptr = reinterpret_cast<byte *>(current_block) - (reinterpret_cast<byte *>(_trusted_memory) + allocator_metadata_size);

    size_t ptr_twin = ptr ^ (pow_2(stepen_of_2(get_block_size(current_block))));

    return (reinterpret_cast<std::byte *>(_trusted_memory) + allocator_metadata_size + ptr_twin);
}

size_t allocator_buddies_system::get_block_size(void *current_block) const noexcept
{
    return pow_2(reinterpret_cast<block_metadata *>(current_block)->size);
}

std::mutex &allocator_buddies_system::get_mutex() const noexcept
{
    return *reinterpret_cast<std::mutex *>(_trusted_memory);
}

allocator_with_fit_mode::fit_mode &allocator_buddies_system::get_fit_mod() const noexcept
{
    return *reinterpret_cast<fit_mode *>(reinterpret_cast<byte *>(_trusted_memory) + sizeof(logger *) + sizeof(allocator_dbg_helper *));
}

size_t allocator_buddies_system::get_allocator_pool_size() const noexcept
{
    auto *ptr = reinterpret_cast<byte *>(_trusted_memory) + sizeof(std::mutex) + sizeof(logger *) + sizeof(std::pmr::memory_resource *) + sizeof(fit_mode);
    size_t size = pow_2(static_cast<size_t>(*ptr));
    return size;
}

size_t allocator_buddies_system::get_size_memory() const noexcept
{
    return pow_2(*(reinterpret_cast<byte *>(_trusted_memory) + sizeof(std::mutex) + sizeof(logger *) + sizeof(std::pmr::memory_resource *) + sizeof(fit_mode)));
}

void *allocator_buddies_system::find_first_suitable_block(size_t block_size_to_find) const noexcept
{
    byte *begin = static_cast<byte *>(_trusted_memory) + allocator_metadata_size;
    byte *end = begin + get_size_memory();

    for (byte *current = begin; current < end; current += get_block_size(current))
    {
        if (!is_busy(current) && (get_block_size(current) - occupied_block_metadata_size) >= block_size_to_find)
        {
            return current;
        }
    }
    return nullptr;
}

void *allocator_buddies_system::find_worst_suitable_block(size_t block_size_to_find) const noexcept
{
    byte *begin = static_cast<byte *>(_trusted_memory) + allocator_metadata_size;
    byte *end = begin + get_size_memory();

    void *worst_block = nullptr;
    size_t max_size = 0;

    for (byte *current = begin; current < end; current += get_block_size(current))
    {
        size_t available_size = get_block_size(current) - occupied_block_metadata_size;
        if (!is_busy(current) && available_size >= block_size_to_find && available_size > max_size)
        {
            max_size = available_size;
            worst_block = current;
        }
    }
    return worst_block;
}

void *allocator_buddies_system::find_best_suitable_block(size_t block_size_to_find) const noexcept
{
    byte *begin = static_cast<byte *>(_trusted_memory) + allocator_metadata_size;
    byte *end = begin + get_size_memory();

    void *best_block = nullptr;
    size_t min_size = SIZE_MAX;

    for (byte *current = begin; current < end; current += get_block_size(current))
    {
        size_t available_size = get_block_size(current) - occupied_block_metadata_size;
        if (!is_busy(current) && available_size >= block_size_to_find && available_size < min_size)
        {
            min_size = available_size;
            best_block = current;
        }
    }
    return best_block;
}

void allocator_buddies_system::split_block(void *block, byte required_power)
{
    auto *meta = reinterpret_cast<block_metadata *>(block);
    while (meta->size > required_power)
    {
        meta->size--;
        void *twin_block = get_block_twin(block);
        auto *twin_block_meta = reinterpret_cast<block_metadata *>(twin_block);
        twin_block_meta->occupied = false;
        twin_block_meta->size = meta->size;
    }
}

allocator_buddies_system::~allocator_buddies_system()
{
    //std::lock_guard<std::mutex> lock(get_mutex());

    debug_with_guard("called ~allocator_buddies_system");
    auto alloc = get_memory_resource();
    if (alloc == nullptr)
    {
        get_mutex().~mutex();
        ::operator delete(_trusted_memory);
    }
    else
    {
        alloc->deallocate(_trusted_memory, get_size_memory() + allocator_metadata_size, alignof(std::max_align_t));
    }
    debug_with_guard("~allocator_buddies_system completed");
}

allocator_buddies_system::allocator_buddies_system(allocator_buddies_system &&other) noexcept
    : smart_mem_resource(std::move(other)), allocator_test_utils(other), allocator_with_fit_mode(std::move(other)),
      logger_guardant(other), typename_holder(other), _trusted_memory(other._trusted_memory)
{

    debug_with_guard("allocator_buddies_system move called");
    other._trusted_memory = nullptr;
    debug_with_guard("allocator_buddies_system move finished");
}

allocator_buddies_system &allocator_buddies_system::operator=(allocator_buddies_system &&other) noexcept
{
    debug_with_guard("allocator_buddies_system operator= move started");
    if (this != &other)
    {
        if (_trusted_memory)
        {
            _trusted_memory = other._trusted_memory;
        }

        smart_mem_resource::operator=(std::move(other));
        allocator_test_utils::operator=(other);
        allocator_with_fit_mode::operator=(std::move(other));
        logger_guardant::operator=(other);
        typename_holder::operator=(other);

        _trusted_memory = other._trusted_memory;
        other._trusted_memory = nullptr;
    }
    debug_with_guard("allocator_buddies_system operator= move finished");

    return *this;
}

allocator_buddies_system::allocator_buddies_system(size_t space_size, std::pmr::memory_resource *parent_allocator,
                                                   logger *log, allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    byte space_size_power_of_two = stepen_of_2(space_size);

    if (space_size_power_of_two < min_k)
    {
        throw std::invalid_argument("space_size must be at least " + std::to_string(min_k));
    }

    size_t real_size = pow_2(space_size_power_of_two) + allocator_metadata_size;

    if (parent_allocator == nullptr)
    {
        try
        {
            _trusted_memory = ::operator new(real_size);
        }
        catch (std::bad_alloc &ex)
        {
            error_with_guard("bad alloc in allocator_buddies_system constructor");
            throw std::bad_alloc(ex);
        }
    }
    else
    {
        try
        {
            _trusted_memory = parent_allocator->allocate(real_size, 1);
        }
        catch (std::bad_alloc &ex)
        {
            error_with_guard("Bad allocation memory from parent allocator");
            throw std::bad_alloc(ex);
        }
    }

    // mutex - logger - parent_allocator - fit_mode - space_size_power_of_two - block_metadata

    void *mem = _trusted_memory;

    new (mem) std::mutex;
    mem = static_cast<char *>(mem) + sizeof(std::mutex);

    *reinterpret_cast<class logger **>(mem) = log;
    mem = static_cast<char *>(mem) + sizeof(class logger *);

    *reinterpret_cast<std::pmr::memory_resource **>(mem) = parent_allocator;
    mem = static_cast<char *>(mem) + sizeof(std::pmr::memory_resource *);

    *reinterpret_cast<fit_mode *>(mem) = allocate_fit_mode;
    mem = static_cast<char *>(mem) + sizeof(fit_mode);

    *reinterpret_cast<byte *>(mem) = space_size_power_of_two;
    mem = static_cast<char *>(mem) + sizeof(byte);

    auto *block = reinterpret_cast<block_metadata *>(mem);
    block->occupied = false;
    block->size = space_size_power_of_two;

    information_with_guard("allocator_buddies_system created with space size: " + std::to_string(space_size) +
                           ", real size: " + std::to_string(real_size) +
                           ", fit mode: " + std::to_string(static_cast<int>(allocate_fit_mode)));

    debug_with_guard("allocator_buddies_system constructor finished");
}

[[nodiscard]] void *allocator_buddies_system::do_allocate_sm(size_t size)
{
    debug_with_guard("do_allocate_sm allocator_buddies_system started");

    //std::lock_guard<std::mutex> lock(get_mutex());

    size_t required_size = size + occupied_block_metadata_size;
    byte power = stepen_of_2(required_size);

    void *block = nullptr;
    auto fit_mode = get_fit_mode();
    if (fit_mode == allocator_with_fit_mode::fit_mode::first_fit)
    {
        block = find_first_suitable_block(power);
    }
    else if (fit_mode == allocator_with_fit_mode::fit_mode::the_best_fit)
    {
        block = find_best_suitable_block(power);
    }
    else if (fit_mode == allocator_with_fit_mode::fit_mode::the_worst_fit)
    {
        block = find_worst_suitable_block(power);
    }

    if (block == nullptr)
    {
        error_with_guard("Didn't find free block to allocate, bad_alloc");

        throw std::bad_alloc();
    }

    split_block(block, power);

    auto *meta = reinterpret_cast<block_metadata *>(block);

    meta->occupied = true;

    auto metadata_ptr = reinterpret_cast<void **>(static_cast<byte *>(block) + sizeof(block_metadata));
    *metadata_ptr = _trusted_memory;

    debug_with_guard("do_allocate_sm allocator_buddies_system end");

    return reinterpret_cast<byte *>(block) + occupied_block_metadata_size;
}

void allocator_buddies_system::do_deallocate_sm(void *at)
{
    std::lock_guard<std::mutex> mutex_guard(get_mutex());
    debug_with_guard(" do_deallocate_sm in allocator_buddies_system");

    void *block_start = reinterpret_cast<byte *>(at) - occupied_block_metadata_size;
    if (*reinterpret_cast<void **>(reinterpret_cast<byte *>(block_start) + sizeof(block_metadata)) != _trusted_memory)
    {
        error_with_guard("memory was allocated from another allocator!!!!!!!?! ");
        throw std::runtime_error("memory was allocated from another allocator!!!!!!!");
    }

    reinterpret_cast<block_metadata *>(block_start)->occupied = false;
    void *twin = get_block_twin(block_start);
    while (get_block_size(block_start) < get_size_memory() && get_block_size(block_start) == get_block_size(twin) &&
           !is_busy(twin))
    {
        void *tmp_ptr = block_start < twin ? block_start : twin;

        auto *current_meta = reinterpret_cast<block_metadata *>(tmp_ptr);
        ++current_meta->size;

        block_start = tmp_ptr;
        twin = get_block_twin(block_start);
    }
    debug_with_guard("allocator_buddies_system do_deallocate_sm end");
}

bool allocator_buddies_system::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{

    auto *other_allocator = dynamic_cast<const allocator_buddies_system *>(&other);
    if (!other_allocator)
    {
        return false;
    }
    return _trusted_memory == other_allocator->_trusted_memory;
}
void allocator_buddies_system::set_fit_mode(allocator_with_fit_mode::fit_mode mode)
{
    //std::lock_guard lock(get_mutex());
    get_fit_mod() = mode;
}

allocator_buddies_system::buddy_iterator allocator_buddies_system::begin() const noexcept
{
    auto *start_block = reinterpret_cast<void *>(reinterpret_cast<byte *>(_trusted_memory) + allocator_metadata_size);
    return {start_block};
}

allocator_buddies_system::buddy_iterator allocator_buddies_system::end() const noexcept
{
    auto *end_block = reinterpret_cast<void *>(reinterpret_cast<byte *>(_trusted_memory) + allocator_metadata_size +
                                               get_size_memory());
    return {end_block};
}

bool allocator_buddies_system::buddy_iterator::operator==(
    const allocator_buddies_system::buddy_iterator &other) const noexcept
{
    return _block == other._block;
}

bool allocator_buddies_system::buddy_iterator::operator!=(
    const allocator_buddies_system::buddy_iterator &other) const noexcept
{
    return _block != other._block;
}

allocator_buddies_system::buddy_iterator &allocator_buddies_system::buddy_iterator::operator++() & noexcept
{
    size_t block_size = reinterpret_cast<block_metadata *>(_block)->size;
    _block = reinterpret_cast<void *>(reinterpret_cast<byte *>(_block) + (1 << block_size));
    return *this;
}

allocator_buddies_system::buddy_iterator allocator_buddies_system::buddy_iterator::operator++(int)
{
    buddy_iterator temp = *this;
    ++(*this);
    return temp;
}

size_t allocator_buddies_system::buddy_iterator::size() const noexcept
{
    return reinterpret_cast<block_metadata *>(_block)->size;
}

bool allocator_buddies_system::buddy_iterator::occupied() const noexcept
{
    return reinterpret_cast<block_metadata *>(_block)->occupied;
}

void *allocator_buddies_system::buddy_iterator::operator*() const noexcept
{
    return _block;
}

allocator_buddies_system::buddy_iterator::buddy_iterator(void *start) : _block(start)
{
}

allocator_buddies_system::buddy_iterator::buddy_iterator() : _block(nullptr)
{
}
