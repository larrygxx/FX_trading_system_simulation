#pragma once

#include <vector>

#include "ITrade.h"
#include "IPricer.h"

namespace minirisk {

struct Market;

typedef std::vector<std::pair<double, string>> portfolio_values_t;//Q5 modify typedef

// get pricer for each trade
//Q7
std::vector<ppricer_t> get_pricers(const portfolio_t& portfolio, const std::string& base_ccy);

// compute prices
portfolio_values_t compute_prices(const std::vector<ppricer_t>& pricers, Market& mkt, std::shared_ptr<const FixingDataServer> fds);

//portfolio_values_t compute_prices(const std::vector<ppricer_t>& pricers, Market& mkt);

// compute the cumulative book value

std::pair<double, std::vector<std::pair<size_t, string>>> portfolio_total(const portfolio_values_t& values);//Q5 modify double

// Compute PV01 (i.e. sensitivity with respect to interest rate dV/dr)
// Use central differences, absolute bump of 0.01%, rescale result for rate movement of 0.01%
std::vector<std::pair<string, portfolio_values_t>> compute_pv01_bucketed(const std::vector<ppricer_t>& pricers, const Market& mkt, std::shared_ptr<const FixingDataServer> fds);

std::vector<std::pair<string, portfolio_values_t>> compute_pv01_parallel(const std::vector<ppricer_t>& pricers, const Market& mkt, std::shared_ptr<const FixingDataServer> fds);

//Compute fx_delta Q10
std::vector<std::pair<string, portfolio_values_t>> compute_fx_delta(const std::vector<ppricer_t>& pricers, const Market& mkt, std::shared_ptr<const FixingDataServer> fds);

// save portfolio to file
void save_portfolio(const string& filename, const std::vector<ptrade_t>& portfolio);

// load portfolio from file
std::vector<ptrade_t>  load_portfolio(const string& filename);

// print portfolio to cout
void print_portfolio(const portfolio_t& portfolio);

// print portfolio to cout
void print_price_vector(const string& name, const portfolio_values_t& values);


} // namespace minirisk

