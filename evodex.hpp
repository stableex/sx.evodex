#pragma once

#include <eosio/asset.hpp>

namespace evodex {

    using eosio::asset;
    using eosio::symbol;
    using eosio::name;
    using eosio::multi_index;

    using std::pair;

    const name id = "evodex"_n;
    const name code = "evolutiondex"_n;
    const string description = "Evolution Dex Converter";

    /**
     * pair index table
     */
    struct [[eosio::table]] evoindex_row {
        symbol      evo_symbol;
        checksum256 id_256;

        uint64_t primary_key() const { return evo_symbol.code().raw(); }
    };
    typedef eosio::multi_index< "evoindex"_n, evoindex_row > evoindex;

    /**
     * liquidity table
     */
    struct [[eosio::table]] stat_row {
        asset           supply;
        asset           max_supply;
        name            issuer;
        extended_asset  pool1;
        extended_asset  pool2;
        int32_t         fee;
        name            fee_contract;

        uint64_t primary_key() const { return supply.symbol.code().raw(); }
    };
    typedef eosio::multi_index< "stat"_n, stat_row > stat;


    /**
     * ## STATIC `get_fee`
     *
     * Get total fee
     *
     * ### returns
     *
     * - `{uint8_t}` - total fee (trade + protocol)
     *
     * ### example
     *
     * ```c++
     * const uint8_t fee = evodex::get_fee();
     * // => 30
     * ```
     */
    static uint8_t get_fee()
    {
        //all pools have 30 bips fee, if that changes - we could lookup it up in stat table
        return 30;
    }

    /**
     * ## STATIC `get_reserves`
     *
     * Get reserves for a pair
     *
     * ### params
     *
     * - `{string} pair_id` - pair id
     * - `{symbol} sort` - sort by symbol (reserve0 will be first item in pair)
     *
     * ### returns
     *
     * - `{pair<asset, asset>}` - pair of reserve assets
     *
     * ### example
     *
     * ```c++
     * const string pair_id = "EOSUSDT";
     * const symbol sort = symbol{"EOS", 4};
     *
     * const auto [reserve0, reserve1] = evodex::get_reserves( pair_id, sort );
     * // reserve0 => "4638.5353 EOS"
     * // reserve1 => "13614.8381 USDT"
     * ```
     */
    static pair<asset, asset> get_reserves( const string& pair_id, const symbol sort )
    {
        // table
        symbol_code evo_symcode {pair_id};
        check(evo_symcode.is_valid(), "EvodexLibrary: Invalid pair_id");

        evodex::stat _pools( evodex::code, evo_symcode.raw() );

        check(_pools.begin() != _pools.end(), "EvodexLibrary: Invalid Evodex symbol");

        auto pool = *_pools.begin();

        eosio::check( pool.pool1.quantity.symbol == sort || pool.pool2.quantity.symbol == sort, "EvodexLibrary: sort symbol doesn't match");

        return sort == pool.pool1.quantity.symbol ?
            pair<asset, asset>{ pool.pool1.quantity, pool.pool2.quantity } :
            pair<asset, asset>{ pool.pool2.quantity, pool.pool1.quantity };
    }

    /**
     * ## STATIC `get_amount_out`
     *
     * Given an input amount of an asset and pair reserves, returns the output amount of the other asset
     * source: https://github.com/EOSArgentina/evolutiondex/blob/master/evolutiondex.cpp#L165
     *
     * ### params
     *
     * - `{uint64_t} amount_in` - amount input
     * - `{uint64_t} reserve_in` - reserve input
     * - `{uint64_t} reserve_out` - reserve output
     * - `{uint64_t} fee` - trading fee (pips 1/10000)
     *
     * ### example
     *
     * ```c++
     * // Inputs
     * const uint64_t amount_in = 10000;
     * const uint64_t reserve_in = 45851931234;
     * const uint64_t reserve_out = 46851931234;
     * const uint64_t fee = 5;
     *
     * // Calculation
     * const uint64_t amount_out = evodex::get_amount_out( amount_in, reserve_in, reserve_out, fee );
     * // => 9996
     * ```
     */
    static uint64_t get_amount_out( const uint64_t amount_in, const uint64_t reserve_in, const uint64_t reserve_out,  const uint64_t fee )
    {
        // checks
        eosio::check(amount_in > 0, "sx.evodex: INSUFFICIENT_INPUT_AMOUNT");
        eosio::check(reserve_in > 0 && reserve_out > 0, "sx.evodex: INSUFFICIENT_LIQUIDITY");

        int128_t am_in = -int128_t(amount_in);
        int128_t res_in = (int128_t)reserve_in + (int128_t)amount_in;

        int128_t tmp = am_in * reserve_out / res_in;
        int128_t tmp_fee = (-tmp * fee + 9999)/10000;
        tmp += tmp_fee;

        return -tmp;
    }

}