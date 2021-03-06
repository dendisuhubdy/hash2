#ifndef BOOST_HASH2_SPOOKY2_HPP_INCLUDED
#define BOOST_HASH2_SPOOKY2_HPP_INCLUDED

// Copyright 2017, 2018 Peter Dimov.
// Distributed under the Boost Software License, Version 1.0.
//
// SpookyHash V2, http://burtleburtle.net/bob/hash/spooky.html

#include <boost/hash2/byte_type.hpp>
#include <boost/hash2/detail/read.hpp>
#include <boost/hash2/detail/write.hpp>
#include <boost/hash2/detail/rot.hpp>
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <boost/assert.hpp>
#include <utility>
#include <cstring>

namespace boost
{
namespace hash2
{

class spooky2_128
{
private:

    static const boost::uint64_t sc_const = 0xdeadbeefdeadbeefULL;

private:

    static const int M = 12;
    boost::uint64_t v_[ M ];

    static const int N = 192;
    byte_type buffer_[ N ];

    int m_;

    std::size_t n_;

private:

    static inline void short_mix( boost::uint64_t & h0, boost::uint64_t & h1, boost::uint64_t & h2, boost::uint64_t & h3 )
    {
        h2 = detail::rotl( h2, 50 ); h2 += h3; h0 ^= h2;
        h3 = detail::rotl( h3, 52 ); h3 += h0; h1 ^= h3;
        h0 = detail::rotl( h0, 30 ); h0 += h1; h2 ^= h0;
        h1 = detail::rotl( h1, 41 ); h1 += h2; h3 ^= h1;
        h2 = detail::rotl( h2, 54 ); h2 += h3; h0 ^= h2;
        h3 = detail::rotl( h3, 48 ); h3 += h0; h1 ^= h3;
        h0 = detail::rotl( h0, 38 ); h0 += h1; h2 ^= h0;
        h1 = detail::rotl( h1, 37 ); h1 += h2; h3 ^= h1;
        h2 = detail::rotl( h2, 62 ); h2 += h3; h0 ^= h2;
        h3 = detail::rotl( h3, 34 ); h3 += h0; h1 ^= h3;
        h0 = detail::rotl( h0,  5 ); h0 += h1; h2 ^= h0;
        h1 = detail::rotl( h1, 36 ); h1 += h2; h3 ^= h1;
    }

    static inline void short_end( boost::uint64_t & h0, boost::uint64_t & h1, boost::uint64_t & h2, boost::uint64_t & h3 )
    {
        h3 ^= h2; h2 = detail::rotl( h2, 15 ); h3 += h2;
        h0 ^= h3; h3 = detail::rotl( h3, 52 ); h0 += h3;
        h1 ^= h0; h0 = detail::rotl( h0, 26 ); h1 += h0;
        h2 ^= h1; h1 = detail::rotl( h1, 51 ); h2 += h1;
        h3 ^= h2; h2 = detail::rotl( h2, 28 ); h3 += h2;
        h0 ^= h3; h3 = detail::rotl( h3,  9 ); h0 += h3;
        h1 ^= h0; h0 = detail::rotl( h0, 47 ); h1 += h0;
        h2 ^= h1; h1 = detail::rotl( h1, 54 ); h2 += h1;
        h3 ^= h2; h2 = detail::rotl( h2, 32 ); h3 += h2;
        h0 ^= h3; h3 = detail::rotl( h3, 25 ); h0 += h3;
        h1 ^= h0; h0 = detail::rotl( h0, 63 ); h1 += h0;
    }

    static void short_hash( byte_type const * p, std::size_t n, boost::uint64_t & hash1, boost::uint64_t & hash2 )
    {
        boost::uint64_t a = hash1;
        boost::uint64_t b = hash2;
        boost::uint64_t c = sc_const;
        boost::uint64_t d = sc_const;

        std::size_t m = n;

        while( m >= 32 )
        {
            c += detail::read64le( p +  0 );
            d += detail::read64le( p +  8 );

            short_mix( a, b, c, d );

            a += detail::read64le( p + 16 );
            b += detail::read64le( p + 24 );

            p += 32;
            m -= 32;
        }

        if( m >= 16 )
        {
            c += detail::read64le( p +  0 );
            d += detail::read64le( p +  8 );

            short_mix( a, b, c, d );

            p += 16;
            m -= 16;
        }

        if( m == 0 )
        {
            c += sc_const;
            d += sc_const;
        }
        else
        {
            BOOST_ASSERT( m < 16 );

            byte_type tmp[ 16 ] = { 0 };
            std::memcpy( tmp, p, m );

            c += detail::read64le( tmp + 0 );
            d += detail::read64le( tmp + 8 );
        }

        d += static_cast<boost::uint64_t>( n ) << 56;

        short_end( a, b, c, d );

        hash1 = a;
        hash2 = b;
    }

private:

    static inline void mix( byte_type const * p,
        boost::uint64_t & s0, boost::uint64_t & s1, boost::uint64_t &  s2, boost::uint64_t &  s3,
        boost::uint64_t & s4, boost::uint64_t & s5, boost::uint64_t &  s6, boost::uint64_t &  s7,
        boost::uint64_t & s8, boost::uint64_t & s9, boost::uint64_t & s10, boost::uint64_t & s11 )
    {
        s0  += detail::read64le( p +  0 ); s2  ^= s10; s11 ^= s0;  s0  = detail::rotl( s0,  11 ); s11 += s1;
        s1  += detail::read64le( p +  8 ); s3  ^= s11; s0  ^= s1;  s1  = detail::rotl( s1,  32 ); s0  += s2;
        s2  += detail::read64le( p + 16 ); s4  ^= s0;  s1  ^= s2;  s2  = detail::rotl( s2,  43 ); s1  += s3;
        s3  += detail::read64le( p + 24 ); s5  ^= s1;  s2  ^= s3;  s3  = detail::rotl( s3,  31 ); s2  += s4;
        s4  += detail::read64le( p + 32 ); s6  ^= s2;  s3  ^= s4;  s4  = detail::rotl( s4,  17 ); s3  += s5;
        s5  += detail::read64le( p + 40 ); s7  ^= s3;  s4  ^= s5;  s5  = detail::rotl( s5,  28 ); s4  += s6;
        s6  += detail::read64le( p + 48 ); s8  ^= s4;  s5  ^= s6;  s6  = detail::rotl( s6,  39 ); s5  += s7;
        s7  += detail::read64le( p + 56 ); s9  ^= s5;  s6  ^= s7;  s7  = detail::rotl( s7,  57 ); s6  += s8;
        s8  += detail::read64le( p + 64 ); s10 ^= s6;  s7  ^= s8;  s8  = detail::rotl( s8,  55 ); s7  += s9;
        s9  += detail::read64le( p + 72 ); s11 ^= s7;  s8  ^= s9;  s9  = detail::rotl( s9,  54 ); s8  += s10;
        s10 += detail::read64le( p + 80 ); s0  ^= s8;  s9  ^= s10; s10 = detail::rotl( s10, 22 ); s9  += s11;
        s11 += detail::read64le( p + 88 ); s1  ^= s9;  s10 ^= s11; s11 = detail::rotl( s11, 46 ); s10 += s0;
    }

    void update_( byte_type const * p, std::ptrdiff_t k )
    {
        boost::uint64_t h0  = v_[ 0];
        boost::uint64_t h1  = v_[ 1];
        boost::uint64_t h2  = v_[ 2];
        boost::uint64_t h3  = v_[ 3];
        boost::uint64_t h4  = v_[ 4];
        boost::uint64_t h5  = v_[ 5];
        boost::uint64_t h6  = v_[ 6];
        boost::uint64_t h7  = v_[ 7];
        boost::uint64_t h8  = v_[ 8];
        boost::uint64_t h9  = v_[ 9];
        boost::uint64_t h10 = v_[10];
        boost::uint64_t h11 = v_[11];

        for( std::ptrdiff_t i = 0; i < k; ++i, p += 96 )
        {
            mix( p, h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11 );
        }

        v_[ 0] = h0;
        v_[ 1] = h1;
        v_[ 2] = h2;
        v_[ 3] = h3;
        v_[ 4] = h4;
        v_[ 5] = h5;
        v_[ 6] = h6;
        v_[ 7] = h7;
        v_[ 8] = h8;
        v_[ 9] = h9;
        v_[10] = h10;
        v_[11] = h11;
    }

    static inline void end_partial(
        boost::uint64_t & h0, boost::uint64_t & h1, boost::uint64_t &  h2, boost::uint64_t &  h3,
        boost::uint64_t & h4, boost::uint64_t & h5, boost::uint64_t &  h6, boost::uint64_t &  h7, 
        boost::uint64_t & h8, boost::uint64_t & h9, boost::uint64_t & h10, boost::uint64_t & h11 )
    {
        h11 += h1;    h2  ^= h11;   h1  = detail::rotl( h1,  44 );
        h0  += h2;    h3  ^= h0;    h2  = detail::rotl( h2,  15 );
        h1  += h3;    h4  ^= h1;    h3  = detail::rotl( h3,  34 );
        h2  += h4;    h5  ^= h2;    h4  = detail::rotl( h4,  21);
        h3  += h5;    h6  ^= h3;    h5  = detail::rotl( h5,  38);
        h4  += h6;    h7  ^= h4;    h6  = detail::rotl( h6,  33);
        h5  += h7;    h8  ^= h5;    h7  = detail::rotl( h7,  10);
        h6  += h8;    h9  ^= h6;    h8  = detail::rotl( h8,  13);
        h7  += h9;    h10 ^= h7;    h9  = detail::rotl( h9,  38);
        h8  += h10;   h11 ^= h8;    h10 = detail::rotl( h10, 53);
        h9  += h11;   h0  ^= h9;    h11 = detail::rotl( h11, 42);
        h10 += h0;    h1  ^= h10;   h0  = detail::rotl( h0,  54);
    }

    static inline void end( byte_type const * p,
        boost::uint64_t & h0, boost::uint64_t & h1, boost::uint64_t &  h2, boost::uint64_t &  h3,
        boost::uint64_t & h4, boost::uint64_t & h5, boost::uint64_t &  h6, boost::uint64_t &  h7, 
        boost::uint64_t & h8, boost::uint64_t & h9, boost::uint64_t & h10, boost::uint64_t & h11 )
    {
        h0  += detail::read64le( p +  0 );
        h1  += detail::read64le( p +  8 );
        h2  += detail::read64le( p + 16 );
        h3  += detail::read64le( p + 24 );
        h4  += detail::read64le( p + 32 );
        h5  += detail::read64le( p + 40 );
        h6  += detail::read64le( p + 48 );
        h7  += detail::read64le( p + 56 );
        h8  += detail::read64le( p + 64 );
        h9  += detail::read64le( p + 72 );
        h10 += detail::read64le( p + 80 );
        h11 += detail::read64le( p + 88 );

        end_partial( h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11 );
        end_partial( h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11 );
        end_partial( h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11 );
    }

    void init( boost::uint64_t seed1, boost::uint64_t seed2 )
    {
        v_[ 0 ] = v_[ 3 ] = v_[ 6 ] = v_[  9 ] = seed1;
        v_[ 1 ] = v_[ 4 ] = v_[ 7 ] = v_[ 10 ] = seed2;
        v_[ 2 ] = v_[ 5 ] = v_[ 8 ] = v_[ 11 ] = sc_const;
    }

public:

    typedef boost::array<byte_type, 16> result_type;
    typedef boost::uint64_t size_type;

    explicit spooky2_128( boost::uint64_t seed1 = 0, boost::uint64_t seed2 = 0 ): m_( 0 ), n_( 0 )
    {
        init( seed1, seed2 );
    }

    explicit spooky2_128( byte_type const * p, std::ptrdiff_t n ): m_( 0 ), n_( 0 )
    {
        BOOST_ASSERT( n >= 0 );

        if( n <= 16 )
        {
            byte_type q[ 18 ] = {};
            std::memcpy( q, p, n );

            boost::uint64_t seed1 = detail::read64le( q + 0 );
            boost::uint64_t seed2 = detail::read64le( q + 8 );

            init( seed1, seed2 );
        }
        else
        {
            boost::uint64_t seed1 = detail::read64le( p + 0 );
            boost::uint64_t seed2 = detail::read64le( p + 8 );

            init( seed1, seed2 );

            p += 16;
            n -= 16;

            update( p, n );
            result();
        }
    }

    void update( byte_type const * p, std::ptrdiff_t n )
    {
        BOOST_ASSERT( n >= 0 );

        BOOST_ASSERT( m_ == static_cast<int>( n_ % N ) );

        if( n == 0 ) return;

        n_ += n;

        if( m_ > 0 )
        {
            int k = N - m_;

            if( n < k )
            {
                k = static_cast<int>( n );
            }

            std::memcpy( buffer_ + m_, p, k );

            p += k;
            n -= k;
            m_ += k;

            if( m_ < N ) return;

            BOOST_ASSERT( m_ == N );

            update_( buffer_, 2 );
            m_ = 0;
        }

        BOOST_ASSERT( m_ == 0 );

        {
            std::ptrdiff_t k = n / N;

            update_( p, k * 2 );

            p += k * N;
            n -= k * N;
        }

        BOOST_ASSERT( n < N );

        if( n > 0 )
        {
            std::memcpy( buffer_, p, n );
            m_ = static_cast<int>( n );
        }

        BOOST_ASSERT( m_ == static_cast<int>( n_ % N ) );
    }

    result_type result()
    {
        BOOST_ASSERT( m_ == static_cast<int>( n_ % N ) );

        boost::uint64_t h0 = v_[ 0 ];
        boost::uint64_t h1 = v_[ 1 ];

        if( n_ < N )
        {
            short_hash( buffer_, n_, h0, h1 );
        }
        else
        {
            boost::uint64_t h2  = v_[  2 ];
            boost::uint64_t h3  = v_[  3 ];
            boost::uint64_t h4  = v_[  4 ];
            boost::uint64_t h5  = v_[  5 ];
            boost::uint64_t h6  = v_[  6 ];
            boost::uint64_t h7  = v_[  7 ];
            boost::uint64_t h8  = v_[  8 ];
            boost::uint64_t h9  = v_[  9 ];
            boost::uint64_t h10 = v_[ 10 ];
            boost::uint64_t h11 = v_[ 11 ];

            byte_type * p = buffer_;
            int m = m_;

            if( m >= 96 )
            {
                mix( p, h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11 );

                p += 96;
                m -= 96;
            }

            std::memset( p + m, 0, 96 - m );
            p[ 95 ] = static_cast<byte_type>( m & 0xFF );

            end( p, h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11 );

            v_[  2 ] = h2;
            v_[  3 ] = h3;
            v_[  4 ] = h4;
            v_[  5 ] = h5;
            v_[  6 ] = h6;
            v_[  7 ] = h7;
            v_[  8 ] = h8;
            v_[  9 ] = h9;
            v_[ 10 ] = h10;
            v_[ 11 ] = h11;
        }

        v_[ 0 ] = h0;
        v_[ 1 ] = h1;

        n_ += N - m_;
        m_ = 0;

        if( n_ < N )
        {
            n_ = 0; // in case of overflow (N does not divide 2^32)
        }

        // clear buffered plaintext
        std::memset( buffer_, 0, N );

        result_type r;

        detail::write64le( &r[ 0 ], h0 );
        detail::write64le( &r[ 8 ], h1 );

        return r;
    }
};

} // namespace hash2
} // namespace boost

#endif // #ifndef BOOST_HASH2_SPOOKY2_HPP_INCLUDED
