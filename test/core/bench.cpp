//
// Copyright (c) 2013-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <beast/core/flat_buffer.hpp>
#include <beast/core/multi_buffer.hpp>
#include <beast/core/string_view.hpp>
#include <beast/core/detail/read_size_helper.hpp>
#include <beast/unit_test/suite.hpp>
#include <boost/asio/streambuf.hpp>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <locale>
#include <utility>
#include <vector>

namespace beast {

class bench_test : public beast::unit_test::suite
{
public:
    class timer
    {
        using clock_type =
            std::chrono::steady_clock;

        clock_type::time_point when_;
    
    public:
        using duration =
            clock_type::duration;

        timer()
            : when_(clock_type::now())
        {
        }

        duration
        elapsed() const
        {
            return clock_type::now() - when_;
        }
    };

    static
    inline
    std::size_t
    throughput(std::chrono::duration<
        double> const& elapsed, std::size_t items)
    {
        return static_cast<std::size_t>(
            1 / (elapsed/items).count());
    }

    template<class MutableBufferSequence>
    static
    std::size_t
    fill(MutableBufferSequence const& buffers)
    {
        std::size_t n = 0;
        using boost::asio::buffer_cast;
        using boost::asio::buffer_size;
        for(auto const& buffer : buffers)
        {
            std::fill(
                buffer_cast<char*>(buffer),
                buffer_cast<char*>(buffer) +
                    buffer_size(buffer), 0);
            n += buffer_size(buffer);
        }
        return n;
    }

    template<class DynamicBuffer>
    static
    std::size_t
    do_prepares(std::size_t repeat,
        std::size_t count, std::size_t size)
    {
        timer t;
        for(auto i = repeat; i--;)
        {
            DynamicBuffer b;
            for(auto j = count; j--;)
                b.commit(fill(b.prepare(size)));
        }
        return throughput(t.elapsed(),
            repeat * count * size);
    }

    template<class DynamicBuffer>
    static
    std::size_t
    do_hints(std::size_t repeat,
        std::size_t count, std::size_t size)
    {
        using beast::detail::read_size_helper;
        timer t;
        for(auto i = repeat; i--;)
        {
            DynamicBuffer b;
            for(auto j = count; j--;)
                b.commit(fill(b.prepare(
                    read_size_helper(b, size))));
        }
        return throughput(t.elapsed(),
            repeat * count * size);
    }

    template<class DynamicBuffer>
    static
    std::size_t
    do_random(std::size_t repeat,
        std::size_t count, std::size_t size)
    {
        timer t;
        for(auto i = repeat; i--;)
        {
            DynamicBuffer b;
            for(auto j = count; j--;)
                b.commit(fill(b.prepare(
                    1 + (rand()%size))));
        }
        return throughput(t.elapsed(),
            repeat * count * size);
    }

    static
    inline
    void
    do_trials_1()
    {
    }

    template<class F0, class... FN>
    void
    do_trials_1(F0&& f, FN... fn)
    {
        static std::size_t constexpr den = 1024 * 1024;
        log << std::right << std::setw(10) <<
            ((f() + (den / 2)) / den) << " MB/s";
        do_trials_1(fn...);
    }

    template<class F0, class... FN>
    void
    do_trials(string_view name,
        std::size_t trials, F0&& f0, FN... fn)
    {
        while(trials--)
        {
            log << std::left << std::setw(24) << name << ":";
            do_trials_1(f0, fn...);
            log << std::endl;
        }
    }

    void
    run() override
    {
        static std::size_t constexpr trials = 5;
        static std::size_t constexpr repeat = 250;
        std::vector<std::pair<std::size_t, std::size_t>> params;
        params.emplace_back(1024, 1024);
        params.emplace_back(512, 4096);
        params.emplace_back(256, 32768);
        for(auto const& param : params)
        {
            auto const count = param.first;
            auto const size = param.second;
            auto const s = std::string("count=") + std::to_string(count) +
                ", size=" + std::to_string(size);
            log << std::left << std::setw(24) << s << ":" <<
                std::right << std::setw(15) << "prepare" <<
                std::right << std::setw(15) << "with hint" <<
                std::right << std::setw(15) << "random" <<
                std::endl;
            do_trials("flat_buffer", trials,
                [&](){ return do_prepares<flat_buffer>(repeat, count, size); },
                [&](){ return do_hints   <flat_buffer>(repeat, count, size); },
                [&](){ return do_random  <flat_buffer>(repeat, count, size); });
            do_trials("multi_buffer", trials,
                [&](){ return do_prepares<multi_buffer>(repeat, count, size); },
                [&](){ return do_hints   <multi_buffer>(repeat, count, size); },
                [&](){ return do_random  <multi_buffer>(repeat, count, size); });
            do_trials("boost::asio::streambuf", trials,
                [&](){ return do_prepares<boost::asio::streambuf>(repeat, count, size); },
                [&](){ return do_hints   <boost::asio::streambuf>(repeat, count, size); },
                [&](){ return do_random  <boost::asio::streambuf>(repeat, count, size); });
            log << std::endl;
        }
        pass();
    }
};

BEAST_DEFINE_TESTSUITE(bench,core,beast);

} // beast
