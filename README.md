# **`EvoDex`**

> Peripheral EOSIO smart contracts for interacting with [Evolution Dex](https://evodex.io/)

## Dependencies

- [`sx.uniswap`](https://github.com/stableex/sx.uniswap)

## Quickstart

```c++
#include <sx.evodex/evodex.hpp>
#include <sx.uniswap/uniswap.hpp>

// user input
const asset quantity = asset{10000, symbol{"EOS", 4}};
const uint64_t pair_id = "EOSUSDT"; // EOS/USDT pair

// get evodex info
const auto [ reserve_in, reserve_out ] = evodex::get_reserves( pair_id, quantity.symbol );
const uint8_t fee = evodex::get_fee();

// calculate out price
const asset out = uniswap::get_amount_out( quantity, reserves_in, reserves_out, fee );
// => "2.6500 USDT"
```

## Table of Content

- [STATIC `get_reserves`](#static-get_reserves)
- [STATIC `get_fee`](#static-get_fee)

## STATIC `get_reserves`

Get reserves for a pair

### params

- `{string} pair_id` - pair id
- `{symbol} sort` - sort by symbol (reserve0 will be first item in pair)

### returns

- `{pair<asset, asset>}` - pair of reserve assets

### example

```c++
const uint64_t pair_id = "EOSUSDT";
const symbol sort = symbol{"EOS", 4};

const auto [reserve0, reserve1] = evodex::get_reserves( pair_id, sort );
// reserve0 => "4638.5353 EOS"
// reserve1 => "13614.8381 USDT"
```

## STATIC `get_fee`

Get total fee

### returns

- `{uint8_t}` - total fee (trade + protocol)

### example

```c++
const uint8_t fee = evodex::get_fee();
// => 30
```