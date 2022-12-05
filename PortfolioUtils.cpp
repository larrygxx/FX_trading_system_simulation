#include "Global.h"
#include "PortfolioUtils.h"
#include "TradePayment.h"
#include "TradeFXForward.h"
#include <numeric>
#include <set>
#include <cmath>


namespace minirisk {

    namespace {
        std::pair<double, std::string> pv01ornan(std::pair<double, std::string> hi, std::pair<double, std::string> lo, double dr) {
            if (std::isnan(hi.first))
                return std::make_pair(std::numeric_limits<double>::quiet_NaN(), hi.second);
            if (std::isnan(lo.first))
                return std::make_pair(std::numeric_limits<double>::quiet_NaN(), lo.second);
            return std::make_pair((hi.first - lo.first) / dr, "");
        }

    }

    void print_portfolio(const portfolio_t& portfolio)
    {
        std::for_each(portfolio.begin(), portfolio.end(), [](auto& pt) { pt->print(std::cout); });
    }

    //Q7
    std::vector<ppricer_t> get_pricers(const portfolio_t& portfolio, const std::string& base_ccy)
    {
        std::vector<ppricer_t> pricers(portfolio.size());
        std::transform(portfolio.begin(), portfolio.end(), pricers.begin()
            , [base_ccy](auto& pt) -> ppricer_t { return pt->pricer(base_ccy); });
        return pricers;
    }


    portfolio_values_t compute_prices(const std::vector<ppricer_t>& pricers, Market& mkt, std::shared_ptr<const FixingDataServer> fds)
    {
        portfolio_values_t prices;
        for (const auto& pricer : pricers) {
            try {
                auto price = pricer->price(mkt,fds);
                prices.push_back(std::make_pair(price, ""));
            }
            catch (std::exception& e) {
                typedef std::numeric_limits<double> Info;
                double const NAN_d = Info::quiet_NaN();
                prices.push_back(std::make_pair(NAN_d, e.what()));//Q5 try{}and catch{}
            }
        }

        return prices;
    }

    std::pair<double, std::vector<std::pair<size_t, string>>> portfolio_total(const portfolio_values_t& values)//modify Q5
    {

        std::vector<double> valid_trade;//valid trades
        std::vector<std::pair<size_t, string>> error;//error part
        for (auto i = values.cbegin(); i != values.cend(); i++)
        {
            if (std::isnan(i->first))
            {
                error.push_back(std::make_pair(size_t(i - values.cbegin()), i->second));//Q5 invalid part
            }
            else {
                valid_trade.push_back(i->first);//Q5 valid trades'vector
            }
        }
        return std::make_pair(std::accumulate(valid_trade.begin(), valid_trade.end(), 0.0), error);
    }

    std::vector<std::pair<string, portfolio_values_t>> compute_pv01_bucketed(const std::vector<ppricer_t>& pricers, const Market& mkt, std::shared_ptr<const FixingDataServer> fds)
    {
        std::vector<std::pair<string, portfolio_values_t>> pv01;  // PV01 per trade

        const double bump_size = 0.01 / 100;


        // filter risk factors related to IR
        //auto base = mkt.get_risk_factors(ir_rate_prefix + "[A-Z]{3}");
        //Q4.1
        auto base = mkt.get_risk_factors(ir_rate_prefix + "[0-9]+(D|W|M|Y)\\.[A-Z]{3}");


        // Make a local copy of the Market object, because we will modify it applying bumps
        // Note that the actual market objects are shared, as they are referred to via pointers
        Market tmpmkt(mkt);

        // compute prices for perturbated markets and aggregate results
        pv01.reserve(base.size());
        for (const auto& d : base) {
            portfolio_values_t pv_up, pv_dn;//Q5modify
            std::vector<std::pair<string, double>> bumped(1, d);
            pv01.push_back(std::make_pair(d.first, portfolio_values_t(pricers.size())));//Q5modify

            // bump down and price
            bumped[0].second = d.second - bump_size;
            tmpmkt.set_risk_factors(bumped);
            pv_dn = compute_prices(pricers, tmpmkt,fds);

            // bump up and price
            bumped[0].second = d.second + bump_size; // bump up
            tmpmkt.set_risk_factors(bumped);
            pv_up = compute_prices(pricers, tmpmkt,fds);

            // restore original market state for next iteration
            // (more efficient than creating a new copy of the market at every iteration)
            bumped[0].second = d.second;
            tmpmkt.set_risk_factors(bumped);
            // compute estimator of the derivative via central finite differences
            double dr = 2.0 * bump_size;

            std::transform(pv_up.begin(), pv_up.end(), pv_dn.begin(), pv01.back().second.begin()
                , [dr](std::pair<double, string> hi, std::pair<double, string> lo) -> std::pair<double, string> { return { pv01ornan(hi,lo,dr) }; });
        }
        return pv01;
    }

    std::vector<std::pair<string, portfolio_values_t>> compute_pv01_parallel(const std::vector<ppricer_t>& pricers, const Market& mkt, std::shared_ptr<const FixingDataServer> fds)
    {
        std::vector<std::pair<string, portfolio_values_t>> pv01;  // PV01 per trade

        const double bump_size = 0.01 / 100;

        auto base = mkt.get_risk_factors(ir_rate_prefix + "[0-9]+(D|W|M|Y)\\.[A-Z]{3}");

        Market tmpmkt(mkt);
        pv01.reserve(pricers.size());


        std::set<std::string> portfolio_ccy;
        for (const auto& ccy : base) {
            portfolio_ccy.insert(ccy.first.substr(ccy.first.length() - 3, 3));
        }

        for (const auto& ccy : portfolio_ccy) {
            portfolio_values_t pv_up, pv_dn;//modify Q5

            pv01.push_back(std::make_pair("IR." + ccy, portfolio_values_t(pricers.size())));//modufy Q5

            std::vector<std::pair<string, double>> bumped;
            std::vector<std::pair<string, double>> bumped_up;
            std::vector<std::pair<string, double>> bumped_dn;

            //bump the whoel curve for parallel
            for (const auto& d : base) {
                if (ccy == d.first.substr(d.first.length() - 3, 3)) {
                    bumped.push_back(d);
                    bumped_up.push_back(std::make_pair(d.first, d.second + bump_size));
                    bumped_dn.push_back(std::make_pair(d.first, d.second - bump_size));
                }
            }
            tmpmkt.set_risk_factors(bumped_dn);
            pv_dn = compute_prices(pricers, tmpmkt,fds);

            tmpmkt.set_risk_factors(bumped_up);
            pv_up = compute_prices(pricers, tmpmkt,fds);

            tmpmkt.set_risk_factors(bumped);

            double dr = 2.0 * bump_size;
            std::transform(pv_up.begin(), pv_up.end(), pv_dn.begin(), pv01.back().second.begin()
                , [dr](std::pair<double, string> hi, std::pair<double, string> lo) -> std::pair<double, string> { return { pv01ornan(hi,lo,dr) }; });

        }


        return pv01;
    }

    std::vector<std::pair<string, portfolio_values_t>> compute_fx_delta(const std::vector<ppricer_t>& pricers, const Market& mkt, std::shared_ptr<const FixingDataServer> fds)
    {
        std::vector<std::pair<string, portfolio_values_t>> fx_delta;  // fx delta per trade

        const double bump_size = 0.1 / 100;

        auto base = mkt.get_risk_factors(fx_spot_prefix + "*");

        Market tmpmkt(mkt);
        fx_delta.reserve(base.size());
        for (const auto& d : base) {
            portfolio_values_t pv_up, pv_dn;
            std::vector<std::pair<string, double>> bumped(1, d);
            fx_delta.push_back(std::make_pair(d.first, portfolio_values_t(pricers.size())));

            // bump down and price
            bumped[0].second = d.second * (1 - bump_size);
            tmpmkt.set_risk_factors(bumped);
            pv_dn = compute_prices(pricers, tmpmkt,fds);

            // bump up and price
            bumped[0].second = d.second * (1 + bump_size); // bump up
            tmpmkt.set_risk_factors(bumped);
            pv_up = compute_prices(pricers, tmpmkt,fds);


            // restore original market state for next iteration
            // (more efficient than creating a new copy of the market at every iteration)
            bumped[0].second = d.second;
            tmpmkt.set_risk_factors(bumped);
        

            double dr = 2.0 * d.second * bump_size;
            std::transform(pv_up.begin(), pv_up.end(), pv_dn.begin(), fx_delta.back().second.begin()
                , [dr](std::pair<double, string> hi, std::pair<double, string> lo) -> std::pair<double, string> { return { pv01ornan(hi,lo,dr) }; });

        }

        return fx_delta;
    }

    ptrade_t load_trade(my_ifstream& is)
    {
        string name;
        ptrade_t p;

        // read trade identifier
        guid_t id;
        is >> id;

        if (id == TradePayment::m_id)
            p.reset(new TradePayment);
        else if (id == TradeFXForward::m_id)
            p.reset(new TradeFXForward);
        else
            THROW("Unknown trade type:" << id);

        p->load(is);

        return p;
    }

    void save_portfolio(const string& filename, const std::vector<ptrade_t>& portfolio)
    {
        // test saving to file
        my_ofstream of(filename);
        for (const auto& pt : portfolio) {
            pt->save(of);
            of.endl();
        }
        of.close();
    }

    std::vector<ptrade_t> load_portfolio(const string& filename)
    {
        std::vector<ptrade_t> portfolio;

        // test reloading the portfolio
        my_ifstream is(filename);
        while (is.read_line())
            portfolio.push_back(load_trade(is));

        return portfolio;
    }


    void print_price_vector(const string& name, const portfolio_values_t& values)//Q5 modify for print
    {
        auto result = portfolio_total(values);
        std::cout
            << "========================\n"
            << name << ":\n"
            << "========================\n"
            << "Total:  " << result.first << std::endl
            << "Errors: " << result.second.size() << std::endl
            << "\n========================\n";

        for (size_t i = 0, n = values.size(); i < n; ++i) {
            if (std::isnan(values[i].first))
                std::cout << std::setw(5) << i << ": " << values[i].second << "\n";
            else
                std::cout << std::setw(5) << i << ": " << values[i].first << "\n";
        }
        std::cout << "========================\n\n";

    }

} // namespace minirisk
