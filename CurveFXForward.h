#pragma once
#include "Date.h"
#include "ICurve.h"
#include "CurveFXSpot.h"
#include "CurveDiscount.h"
#include <string>

using namespace std;
namespace minirisk {
    //create new market object which implements the interface ICurveFXForward
    struct CurveFXForward :ICurveFXForward
    {

        CurveFXForward(Market* mkt, const Date& today, const std::string& name);

        virtual double fwd(const Date& t) const;
        virtual std::string name() const { return "FX.FWD." + ccy1 + "." + ccy2; }; // to create fwd name which matches MDS
        virtual Date today() const { return m_today; }

    private:
        Date m_today;
        ptr_disc_curve_t ccy1_df; //discount curve object of ccy1
        ptr_disc_curve_t ccy2_df; //discount curve object of ccy2
        double m_spot; //FX spot rate
        string ccy1;
        string ccy2;
    };

}