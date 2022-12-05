#include <iostream>
#include <algorithm>

#include "MarketDataServer.h"
#include "PortfolioUtils.h"
#include "FixingDataServer.h"

using namespace::minirisk;

void run(const string& portfolio_file, const string& risk_factors_file, const string& base_ccy, const string& fixings_file)
{
    // load the portfolio from file
    portfolio_t portfolio = load_portfolio(portfolio_file);
    // save and reload portfolio to implicitly test round trip serialization
    save_portfolio("portfolio.tmp", portfolio);
    portfolio.clear();
    portfolio = load_portfolio("portfolio.tmp");

    // display portfolio
    print_portfolio(portfolio);

    // get pricers
    std::vector<ppricer_t> pricers(get_pricers(portfolio, base_ccy));

    // initialize market data server
    std::shared_ptr<const MarketDataServer> mds(new MarketDataServer(risk_factors_file));
    //Q9
    std::shared_ptr<const FixingDataServer> fds(new FixingDataServer(fixings_file));

    // Init market object
    Date today(2017,8,5);
    Market mkt(mds, today);

    // Price all products. Market objects are automatically constructed on demand,
    // fetching data as needed from the market data server.
    {
        auto prices = compute_prices(pricers, mkt, fds);
        print_price_vector("PV", prices);
    }

    // disconnect the market (no more fetching from the market data server allowed)
    mkt.disconnect();

    // display all relevant risk factors
    {
        //Test Q8
       /* double m_spot = mkt.get_fx_spot("FX.SPOT.EUR.USD");
        ptr_disc_curve_t ccy1_df = mkt.get_discount_curve(ir_curve_discount_name("EUR"));
        ptr_disc_curve_t ccy2_df = mkt.get_discount_curve(ir_curve_discount_name("USD"));
        auto d1 = ccy1_df->df(43130);
        auto d2 = ccy2_df->df(43130);
        std::cout << "EUR 6M rate is " << d1 << "\n";
        std::cout << "USD 6M rate is " << d2 << "\n";
        std::cout << "EUR USD spot rate is " << m_spot << "\n";
        std::cout << "EUR USD fwd rate is " << m_spot * d1 / d2 << "\n";*/

        std::cout << "Risk factors:\n";
        auto tmp = mkt.get_risk_factors(".+");
        for (const auto& iter : tmp)
            std::cout << iter.first << "\n";
        std::cout << "\n";
    }

    {   // Compute PV01 (i.e. sensitivity with respect to interest rate dV/dr)
        std::vector<std::pair<string, portfolio_values_t>> pv01(compute_pv01_bucketed(pricers,mkt,fds));  // PV01 per trade

        // display PV01 per currency
        for (const auto& g : pv01)
            print_price_vector("PV01 bucketed " + g.first, g.second);
    }

    {   
        std::vector<std::pair<string, portfolio_values_t>> pv01(compute_pv01_parallel(pricers, mkt,fds));  // PV01 per trade

        // display PV01 per currency
        for (const auto& g : pv01)
            print_price_vector("PV01 parallel " + g.first, g.second);
    }

    {//Compute FX delta Q10
        std::vector<std::pair<string, portfolio_values_t>> fx_delta(compute_fx_delta(pricers, mkt,fds));
        for (const auto& g : fx_delta)
            print_price_vector("FX delta " + g.first, g.second);
    }
}

void usage()
{
    std::cerr
        << "Invalid command line arguments\n"
        << "Example:\n"
        << "DemoRisk -p portfolio.txt -f risk_factors.txt\n";
    std::exit(-1);
}

int main(int argc, const char **argv)
{
    //below is the command that simulates input
    ///int argc = 9;
    //const char* argv[] = { "DemoRisk","-p", "A:\\Q10_finished\\data\\portfolio_5.txt", "-f","A:\\Q10_finished\\data\\risk_factors_5.txt","-b","","-x","A:\\Q10_finished\\data\\fixings.txt"};
    //const char* argv[] = { "DemoRisk","-p", "\\\\Mac\\Home\\Desktop\\ProjectMaterial\\Q11_done\\data\\portfolio_11.txt", "-f", "\\\\Mac\\Home\\Desktop\\ProjectMaterial\\Q11_done\\data\\risk_factors_5.txt","-b","","-x","\\\\Mac\\Home\\Desktop\\ProjectMaterial\\Q11_done\\data\\fixings.txt" };// parse command line arguments DemoRisk -p portfolio.txt -f risk factors.txt

    string portfolio, riskfactors, base_ccy, fixingdata;
    if (argc % 2 == 0)
        usage();
    for (int i = 1; i < argc; i += 2) {
        string key(argv[i]);
        string value(argv[i+1]);
        if (key == "-p")
            portfolio = value;
        else if (key == "-f")
            riskfactors = value;
        else if (key == "-b")
            base_ccy = value;
        else if (key == "-x")
            fixingdata = value;
        else
            usage();
    }
    if (portfolio == "" || riskfactors == "")
        usage();

    //Q7
    if (base_ccy == "")
        base_ccy = "USD";

    try {
        //Q7
        run(portfolio, riskfactors, base_ccy,fixingdata);
        return 0;  // report success to the caller
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        return -1; // report an error to the caller
    }
    catch (...)
    {
        std::cerr << "Unknown exception occurred\n";
        return -1; // report an error to the caller
    }


}
